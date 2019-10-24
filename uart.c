/**********************************************************
 * @file uart.c
 * @brief Modulo UART para proyecto Sisem
 *
 * SISEMB
 * Modulo UART
 * Este modulo implementa procedimientos de comunicacion serie asincrona (UART)
 * Hacia la camara en pines 3.2 (RX) y 3.3 (TX)
 *
 * @author  Antonio Bracco <antoniobracco0@gmail.com>
 * @author  Federico Grunwald <fgrunwald7@gmail.com>
 * @author  Agustin Navcevich <agusnavce52@gmail.com>
 * @version 1.0
 * Version final
 * @date 21 de Junio de 2018
**********************************************************/

#include "uart.h"

//----------------------------------------------
/** Definicion privada de buffers y banderas. */
volatile static char tx_buffer[2048];
volatile static char rx_buffer[4096];
volatile static int size_tx;
volatile static int size_rx;
volatile static int tx_buffer_index;
volatile static int rx_buffer_index;
volatile static char finished_tx;
volatile static char finished_rx;
volatile static int offset;


volatile static int cap;
volatile static char flag_jpeg;





void init_uartcam()
{

     EUSCI_A0->IE &= ~EUSCI_A_IE_RXIE;   // Deshabilito interrupciones rx de pc

      // Configure UART pins
      P3->SEL0 |= BIT2 | BIT3;                // set 2-UART pin con funcion primaria


      // Configure UART
      EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
      EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
              EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
      // Baud Rate calculation
      // 12000000/(16*9600) = 78.125
      // Fractional portion = 0.125
      // User's Guide Table 21-4: UCBRSx = 0x10
      // UCBRFx = int ( (78.125-78)*16) = 2
      EUSCI_A2->BRW = 78;                     // 12000000/16/9600
      EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
                        EUSCI_A_MCTLW_OS16;

      EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
      EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
      EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A2 RX interrupt

      // Enable global interrupt
      __enable_irq();

      // Enable eUSCIA0 interrupt in NVIC module
      NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);

}


void init_uartcam38400()
{
    size_tx = 1;
    size_rx = 1;
    tx_buffer_index = 0;
    rx_buffer_index = 0;
    finished_rx = 0;
    cap = 0;
    flag_jpeg = 0;
    offset = 0;
      // Configure UART pins
      P3->SEL0 |= BIT2 | BIT3;                // set 2-UART pin con funcion primaria

      // Configure UART
      EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
      EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
              EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
      // Baud Rate calculation
      // N = 12000000/(38400) = 312,5 > 16
      // OS16 = 1, UCBRX = 19, UCBRFX = 8
      // UCBRSX (0.5) = 0xAA
      EUSCI_A2->BRW = 19;                     // 12000000/16/9600
      EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
                        (0xAA <<EUSCI_A_MCTLW_BRS_OFS) |
                        EUSCI_A_MCTLW_OS16;

      EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
      EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
      EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A2 RX interrupt

      // Enable global interrupt
      __enable_irq();

      // Enable eUSCIA0 interrupt in NVIC module
      NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);

}



static void uart_txcam(char* data , unsigned int size) {

    tx_buffer_index = 0;
    size_tx = size;

    // Cargo data en buffer
    int i;
    for( i = 0; i < size ; i++)
    {
        tx_buffer[i] = data[i];
    }

    finished_tx = 0;

    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));

    EUSCI_A2->TXBUF = tx_buffer[tx_buffer_index];
    tx_buffer[tx_buffer_index] = '\0';
    tx_buffer_index++;

    EUSCI_A2->IE |= EUSCI_A_IE_TXIE; // Habilito interrupcion TX

}

static unsigned int uart_rxcam(char* external_buffer) {

    EUSCI_A2->IE &= ~EUSCI_A_IE_RXIE;   //Disable USCI_A2 RX interrupt
    finished_rx = 0;

    // Cargo data en buffer externo hasta FF D9
    int j = 0;
    for ( j = 0; j < size_rx ; j++){
        external_buffer[j] = rx_buffer[j];
        rx_buffer[j] = '\0';
    }
    return(j+1); // Devuelve el size de lo recibido
}


static char is_finished_rx() {
    return finished_rx;
}


static void rst_finished_rx(){
    finished_rx = 0;
    cap = 0;
    flag_jpeg = 0;
    offset = 0;
    rx_buffer_index = 0;
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;   // Enable USCI_A0 RX interrupt
}




unsigned int tx_rx_cam( char* data, unsigned int size ,char* external_buffer ){

    unsigned int sizerx;

    rst_finished_rx();
    uart_txcam(data , size);
    while(!is_finished_rx());

    sizerx = uart_rxcam(external_buffer);

    return(sizerx);
}


//1 eUSCI_A Transmit Interrupt Operation
//The UCTXIFG interrupt flag is set by the transmitter to indicate that UCAxTXBUF is ready to accept
//another character. An interrupt request is generated if UCTXIE is set. UCTXIFG is automatically reset if a
//character is written to UCAxTXBUF.


//eUSCI_A Receive Interrupt Operation
//The UCRXIFG interrupt flag is set each time a character is received and loaded into UCAxRXBUF. An
//interrupt request is generated if UCRXIE is set. UCRXIFG and UCRXIE are reset by a Hard Reset signal
//or when UCSWRST = 1. UCRXIFG is automatically reset when UCAxRXBUF is read.






void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG) // Interrupcion Rx
    {
           rx_buffer[rx_buffer_index] = EUSCI_A2->RXBUF;



           /*Defino condicion a esperar para haber recibido mensaje completo
            * Si obtengo respuestas de camara:
            * RESET : 76 00 26 00 (en total con init end 69 bytes)
            * Init end (post-reset): 36 32 35 0D 0A 49 6E 69 74 20 65 6E 64 0D 0A
            * TAKE PHOTO: 76 00 36 00 00
            * RD JPEG:  76 00 32 00 00 FFD8 JPEG FF D9
            * IMG SIZE: 76 00 31 00 00
            * Baud rate: 76 00 24 00 00
            * RDIMGSIZE: 76 00 34 00 04 00 00 XH XL
            */

           // Estrategia: Si reconozco un retorno, con su size marco hasta donde esperarlo (en buff index)
           // En el caso de take photo, img size y baud rate espero al 5to byte  (cap = 4)
           // En el caso de reset espero al byte 70 (para esperar al init end) (cap = 69)
           // En el caso de JPEG espero a ffd9  (flag jpeg)

           // Para verificar el 3er byte necesito rx_buffer_index >= 2
           // uso statics flag_jpeg y cap

           if (( (cap == 0) && (flag_jpeg == 0) ))          // Si todavia no defini flags
           {
               if( (rx_buffer_index >= 2) && (rx_buffer[rx_buffer_index -2] == 0x76) )
               {
                  // P1->OUT ^= BIT0; // Cambio LED
                   switch( rx_buffer[rx_buffer_index] )
                   {
                   case 0x24 :  //Baud rate
                       cap = 4;
                       break;
                   case 0x31 : // Img size
                       cap = 4;
                       break;
                   case 0x36 : // Take Photo (o stop)
                       cap = 4;
                       break;
                   case 0x26 : // Reset
                       cap = 69;
                       break;
                   case 0x34: // Read jpeg size
                       cap = 8;
                       break;
                   case 0x32 : // Leyendo JPEG
                       flag_jpeg = 1;
                       //cap = 4;
                       //P1->OUT ^= BIT0;
                   }
                   offset = rx_buffer_index -2;
               }
               rx_buffer_index++;
           }
           else if ( (cap!= 0) && ( rx_buffer_index - offset >= cap ))  // Si tengo flag cap y llegue al final
           {
               finished_rx = 1;
               size_rx = rx_buffer_index + 1;
               rx_buffer_index = 0;
               EUSCI_A2->IE &= ~EUSCI_A_IE_RXIE;  // Deshabilito mas interrupciones RX
           }

           else if( (cap == 0) && (flag_jpeg == 1) && (rx_buffer[rx_buffer_index -1] == 0xff) &&  (rx_buffer[rx_buffer_index] == 0xD9) ) //  Si es jpg y llegue al final
           {
               finished_rx = 1;
               size_rx = rx_buffer_index + 1;
               rx_buffer_index = 0;
               EUSCI_A2->IE &= ~EUSCI_A_IE_RXIE;  // Deshabilito mas interrupciones RX
           }
           else
               rx_buffer_index++;
           EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; //Despejo para seguir recibiendo
    }




    else if(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG) // Interrupcion Tx
    {

         //while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
         EUSCI_A2->TXBUF = tx_buffer[tx_buffer_index];
         tx_buffer[tx_buffer_index] = '\0';
         if( tx_buffer_index >= size_tx -1 )
         {
             finished_tx = 1;
             EUSCI_A2->IE &= ~EUSCI_A_IE_TXIE; // Deshabilito interrupcion TX
         }
         tx_buffer_index++;
    }
}



