/**********************************************************
 * @file fuzzy.h
 * @brief Modulo fuzzy para proyecto Sisem
 *
 * SISEMB
 * Modulo fuzzy
 * Este modulo implementa el algoritmo de deteccion de bordes
 *
 * @author  Antonio Bracco <antoniobracco0@gmail.com>
 * @author  Federico Grunwald <fgrunwald7@gmail.com>
 * @author  Agustin Navcevich <agusnavce52@gmail.com>
 * @version 1.0
 * Version final
 * @date 21 de Junio de 2018
**********************************************************/

#ifndef FUZZY_H_
#define FUZZY_H_

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define mod(x) (x % 3)
typedef unsigned char uint8;
typedef unsigned int uint;

/**
* Obtiene informacion de bordes de imagen a partir de imagen en RAW
* @param uint8 *image: Puntero apuntando a imagen
* @param int width, height: Dimensiones de imagen en pixeles
* @param uint8* output: Puntero a buffer donde almacenar informacion de bordes
* @return uint8 *: Puntero a buffer donde se almacena informacion de bordes
*/
uint8 *border_detection(uint8 *image, int width, int height, uint8* output);

#endif /* FUZZY_H_ */
