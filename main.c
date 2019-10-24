/**********************************************************
 * @file main.c
 * @brief Archivo main para proyecto Sisem Border Detection in Hardware
 *
 * SISEMB
 * main
 * Se implementa la secuencia a ser ejecutada para obtener la informacion
 * de bordes a partir de la camara.
 *
 * @author  Antonio Bracco <antoniobracco0@gmail.com>
 * @author  Federico Grunwald <fgrunwald7@gmail.com>
 * @author  Agustin Navcevich <agusnavce52@gmail.com>
 * @version 1.0
 * Version final
 * @date 21 de Junio de 2018
 **********************************************************/


#include "msp.h"
#include "uart.h"
#include "uartpc.h"
#include "timer.h"
#include "jpeg_to_raw.h"



int main(){

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
                 WDT_A_CTL_HOLD;

    //Inicializacion de relojes
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
          CS->CTL0 = 0;                           // Reset tuning parameters
          CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
          CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                      // SMCLK = DCO
            CS_CTL1_SELM_3;                        // MCLK = DCO
          CS->KEY = 0;                            // Lock CS module from unintended accesses


    // Lista de comandos para camara
    char stopcmd[5] = { 0x56 , 0x00 , 0x36, 0x01, 0x03};
    char rstcmd[4] = { 0x56 , 0x00 , 0x26, 0x00};
    char takephotocmd[5] = { 0x56 , 0x00 , 0x36, 0x01, 0x00};
    char setbaudcmd[7] = { 0x56 , 0x00 , 0x24, 0x03, 0x01, 0xAE, 0xC8};
    char imgsizecmd[9] = { 0x56 , 0x00 , 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x22};
    char rdimgsizecmd[5] = { 0x56 , 0x00 , 0x34, 0x01, 0x00};

    //Almacenar size de imagen
    char sizejpg[2];


    //Datos recibidos por camara guardados en r, ocupando sizerx bytes
    unsigned int sizerx;
    char r[4096];

    int j = 0;
    for ( j = 0; j < 4096; j++ )
    {
        r[j] = 0;
    }


    init_timer(); // Para implementar delays. No hay interrupciones de timer fuera de delay()
    init_uartpc(); // Inicializo com serial con PC a 9600 baud
    init_uartcam38400(); // Inicializo com serial con camara a 38400 baud (default de camara)



    //Mando reset
    sizerx = tx_rx_cam(rstcmd, 4 , r);
    delay(300);        // Espero 3 segundos luego del reset

    //Cambio baudrate
    sizerx = tx_rx_cam(setbaudcmd, 7 , r);
    //Como cambie baudrate reinicializo puerto uart a 9600
    init_uartcam();


    while(1){


        // Saco foto
        sizerx = tx_rx_cam(takephotocmd, 5 , r);
        


        // Tomo largo de jpeg
        sizerx = tx_rx_cam(rdimgsizecmd, 5 , r);
        sizejpg[0] = r[7];
        sizejpg[1] = r[8];

        


        // Comando para leer archivo. comienzo en dir 00,00 y size sizejpg (XH XL bytes)
        char readjpgcmd[16] = { 0x56 , 0x00 , 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, sizejpg[0] , sizejpg[1], 0x00, 0x0A};
        // Mando comando y obtengo la imagen en r
        sizerx = tx_rx_cam(readjpgcmd, 16 , r);

        

        /* Imagen almacenada en r+5 (Los primeros 5 bytes son respuesta a comando)
         * jpeg_to_raw obtiene de a 3 filas de MCUs la imagen RAW a partir del JPEG
         * Utiliza el algoritmo de deteccion de borde en grupo de 3 filas
         * Transmite por UART el resultado de la fila del medio.
         * En casos borde:
         * Primer grupo: Tambien transmite primera fila
         * Ultimo grupo: Tambien transmite ultima fila
         * Obtiene otras 3 filas de MCUs hasta finalizar la imagen
         */
        jpeg_to_raw( r + 5 , sizerx - 6);

        for ( j = 0; j < 4096; j++ )
            {
                r[j] = 0;
            }

        init_uartcam();

        delay(200);
        //Mando stop taking pictures
        tx_rx_cam(stopcmd, 5 , r);

        delay(200);

    }
}
