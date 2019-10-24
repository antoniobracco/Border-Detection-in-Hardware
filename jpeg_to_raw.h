/**********************************************************
 * @file jpeg_to_raw.h
 * @brief Modulo jpeg_to_raw para proyecto Sisem
 *
 * SISEMB
 * Modulo fuzzy
 * Este modulo implementa el proceso que parte de imagen
 * jpeg para luego transmitir la informacion de bordes por uart
 * en formato RAW. La PC recibe una imagen RAW en escala de grises
 * con la informacion de bordes de la imagen.
 *
 * @author  Antonio Bracco <antoniobracco0@gmail.com>
 * @author  Federico Grunwald <fgrunwald7@gmail.com>
 * @author  Agustin Navcevich <agusnavce52@gmail.com>
 * @version 1.0
 * Version final
 * @date 21 Junio 2018
**********************************************************/
#ifndef JPEG_TO_RAW_H_
#define JPEG_TO_RAW_H_

#include "picojpeg.h"
#include "fuzzy.h"
#include "uartpc.h"


/* Imagen jpeg almacenada en r en sizerx bytes
* jpeg_to_raw obtiene de a 3 filas de MCUs la imagen RAW a partir del JPEG
* Utiliza el algoritmo de deteccion de borde en grupo de 3 filas
* Transmite por UART el resultado de la fila del medio a la pc
* En casos borde:
* Primer grupo: Tambien transmite primera fila
* Ultimo grupo: Tambien transmite ultima fila
* Repite con otras 3 filas de MCUs hasta finalizar la imagen
*/


/**
* jpeg_to_raw obtiene de a 3 filas de MCUs la imagen RAW a partir del JPEG
* Utiliza el algoritmo de deteccion de borde en grupo de 3 filas
* Transmite por UART el resultado de la fila del medio a la pc
* En casos borde:
* Primer grupo: Tambien transmite primera fila
* Ultimo grupo: Tambien transmite ultima fila
* Repite con otras 3 filas de MCUs hasta finalizar la imagen
* @param unsigned char* r: Puntero apuntando a buffer con JPEG almacenada
* @param unsigned int sizerx: Size de JPEG en bytes
* @param char* external_buffer: Puntero a buffer donde almacenar respuesta de camara
* @return int: 0 en caso de funcionar correctamente, 1 en caso de error
*/

int jpeg_to_raw(unsigned char *r, unsigned int sizerx);


#endif /* JPEG_TO_RAW_H_ */
