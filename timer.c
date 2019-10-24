/**********************************************************
 * @file timer.c
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

#include "timer.h"

volatile static int contador;

void init_timer(void)
{
    contador = 0;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
        TIMER_A0->CCR[0] = 40000;
        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, continuous mode
                TIMER_A_CTL_MC__CONTINUOUS;

    // Enable global interrupt
    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE; // Deshabilito interrupciones de timer
}

void delay(int cs)
{
    // Tiempo en overflow : 40000 * 3/ 12M = 100s

    contador = 0;
    int aux = contador;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    while( contador - aux < 3*cs );
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE; // Deshabilito interrupciones de timer
}


// Timer A0 interrupt service routine

void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    contador++;
    if( contador >= 10000)
    {
        contador = 0;
    }
    TIMER_A0->CCR[0] += 40000;              // Add Offset to TACCR0
}

