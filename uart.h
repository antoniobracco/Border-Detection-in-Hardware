/**********************************************************
 * @file uart.h
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
#ifndef UART_H_
#define UART_H_

#include "msp.h"

/**
* Inicializa el puerto UART en pines 3.2 (RX) y 3.3 (TX) con baud rate de 9600
*/
void init_uartcam();    // cam a 9600 baud

/**
* Inicializa el puerto UART en pines 3.2 (RX) y 3.3 (TX) con baud rate de 38400
*/
void init_uartcam38400(); // cam a 38400 baud


/**
* Manda comando a camara y espera a recibir respuesta (incluye recibir imagen jpeg)
* @param char* data: Puntero apuntando a buffer con comando a enviar
* @param unsigned int size: Size de comando a enviar
* @param char* external_buffer: Puntero a buffer donde almacenar respuesta de camara
* @return unsigned int: largo de respuesta obtenida
*/
unsigned int tx_rx_cam( char* data, unsigned int size ,char* external_buffer );

#endif /* UART_H_ */

