/**********************************************************
 * @file uartpc.c
 * @brief Modulo UARTPC para proyecto Sisem
 *
 * SISEMB
 * Modulo UARTPC
 * Este modulo implementa procedimientos de comunicacion serie asincrona (UART)
 * Hacia la PC
 *
 * @author  Antonio Bracco <antoniobracco0@gmail.com>
 * @author  Federico Grunwald <fgrunwald7@gmail.com>
 * @author  Agustin Navcevich <agusnavce52@gmail.com>
 * Version final
 * @date 21 de Junio de 2018
**********************************************************/

#include "uartpc.h"

// Buffers y banderas para pc
volatile static char tx_bufferpc[8096];
volatile static int size_txpc = 1;
volatile static int tx_buffer_indexpc = 0;
volatile static char finished_txpc = 0;



void init_uartpc()
{
      // Configure UART pins
      P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

      // Configure UART
      EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
      EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
              EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
      // Baud Rate calculation
      // 12000000/(16*9600) = 78.125
      // Fractional portion = 0.125
      // User's Guide Table 21-4: UCBRSx = 0x10
      // UCBRFx = int ( (78.125-78)*16) = 2
      EUSCI_A0->BRW = 78;                     // 12000000/16/9600
      EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
                        EUSCI_A_MCTLW_OS16;

      EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
      //EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag

      // Enable global interrupt
      __enable_irq();

      // Enable eUSCIA0 interrupt in NVIC module
      NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

}



void uart_txpc(char* data , unsigned int size) {

    tx_buffer_indexpc = 0;
    size_txpc = size;

    // Cargo data en buffer
    int i;
    for( i = 0; i < size ; i++)
    {
        tx_bufferpc[i] = data[i];
    }

    finished_txpc = 0;

    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

    EUSCI_A0->TXBUF = tx_bufferpc[tx_buffer_indexpc];
    tx_bufferpc[tx_buffer_indexpc] = '\0';
    tx_buffer_indexpc++;

    EUSCI_A0->IE |= EUSCI_A_IE_TXIE; // Habilito interrupcion TX

    delay(150);
}



char is_finished_txpc() {
    return finished_txpc;
}


// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{

    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) // Interrupcion Rx
    {
       EUSCI_A0->IE &= ~EUSCI_A_IE_RXIE;  // Deshabilito mas interrupciones RX
    }




    else if(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG) // Interrupcion Tx
    {

        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
         EUSCI_A0->TXBUF = tx_bufferpc[tx_buffer_indexpc];
         tx_bufferpc[tx_buffer_indexpc] = '\0';
         if( tx_buffer_indexpc >= size_txpc -1 )
         {
             finished_txpc = 1;
             EUSCI_A0->IE &= ~EUSCI_A_IE_TXIE; // Deshabilito interrupcion TX
         }
         tx_buffer_indexpc++;
    }
}

