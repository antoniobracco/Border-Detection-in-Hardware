/**********************************************************
 * @file timer.h
 * @brief Modulo timer para proyecto Sisem
 *
 * SISEMB
 * Modulo timer
 * Este modulo implementa un timer y una funcion de delay
 * para controlar los tiempos en la ejecucion del loop principal
 * y para darle tiempo a transmitir por uartpc.
 *
 * @author  Antonio Bracco <antoniobracco0@gmail.com>
 * @author  Federico Grunwald <fgrunwald7@gmail.com>
 * @author  Agustin Navcevich <agusnavce52@gmail.com>
 * @version 1.0
 * Version final
 * @date 21 de Junio de 2018
**********************************************************/

#include "msp.h"


/**
* Inicializa el timer
* 
*/
void init_timer(void);


/**
* Implementa un delay en
* centisegundos. No se debe usar con numeros mayores
* a 100 segundos (10000cs).
*/

/**
* Implementa un retardo en centisegundos
* @param int cs: Cantidad de centisegundos a esperar
*/
void delay( int cs);
