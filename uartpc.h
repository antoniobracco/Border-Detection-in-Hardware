/**********************************************************
 * @file uartpc.h
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
#ifndef UARTPC_H_
#define UARTPC_H_

#include "msp.h"
#include "timer.h"

/**
* Inicializa la comunicacion UART con la PC con Baud rate de 9600
*/
void init_uartpc();  // pc a 9600 baud



/**
* Envia mediante UART hacia PC los datos contenidos en {data} (string)
* @param char* data: String a enviar por UART
* @param unsigned int size: cuantos datos cargar
*/
void uart_txpc(char* data , unsigned int size);



#endif /* UARTPC_H_ */

