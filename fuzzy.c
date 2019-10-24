/**********************************************************
 * @file fuzzy.c
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

typedef unsigned char uint8;
typedef unsigned int uint;

#include "fuzzy.h"


int img_width, img_height;

uint8 get(uint8 *array, int x, int y)
{
    return *(array + x * 3 + y * img_width * 3);
}

void set(uint8 *array, int x, int y, uint8 value)
{
    array[x + y * img_width] = value;
}

uint8 get_output(uint8* array, int x, int y) {
    return *(array + x + y * img_width);
}

uint8 *border_detection(uint8 *image, int width, int height, uint8* output)
{
    img_height = height;
    img_width = width;

    int i, j;

    for (i = 0; i < width; i++) {
            for (j = 0; j < height; j++)
                set(output, i, j, 255);
        }

        for (i = 2; i < (width - 1); i++)
        {
            for (j = 2; j < (height - 1); j++)
            {
                if (abs(get(image, i - 1, j) - get(image, i - 1, j + 1))>10)
                {
                    if (abs(get(image, i - 1, j - 1) - get(image, i, j))>10)
                    {
                        if (abs(get(image, i, j - 1) - get(image, i + 1, j - 1)) >10)
                        {
                            set(output, i - 1, j + 1, 0);
                            set(output, i, j, 0);
                            set(output, i + 1, j - 1, 0);
                        }
                    }
                }
                if (abs(get(image, i, j + 1) - get(image, i - 1, j + 1))>10)
                {
                    if (abs(get(image, i + 1, j + 1) - get(image, i, j))> 10)
                    {
                        if (abs(get(image, i + 1, j) - get(image, i + 1, j - 1))>10)
                        {
                            set(output, i - 1, j + 1, 0);
                            set(output, i, j, 0);
                            set(output, i + 1, j - 1, 0);
                        }
                    }
                }
                if (abs(get(image, i, j - 1) - get(image, i - 1, j - 1)) >10)
                {
                    if (abs(get(image, i + 1, j - 1) - get(image, i, j)) >10)
                    {
                        if (abs(get(image, i + 1, j) - get(image, i + 1, j + 1)) >10)
                        {
                            set(output, i - 1, j - 1, 0);
                            set(output, i, j, 0);
                            set(output, i + 1, j + 1, 0);
                        }
                    }
                }
                if (abs(get(image, i - 1, j) -get(image, i - 1, j - 1)) >10)
                {
                    if (abs(get(image, i - 1, j + 1) - get(image, i, j))>10)
                    {
                        if (abs(get(image, i, j + 1) - get(image, i + 1, j + 1))>10)
                        {
                            set(output, i - 1, j - 1, 0);
                            set(output, i, j, 0);
                            set(output, i + 1, j + 1, 0);
                        }
                    }
                }
                if (abs(get(image, i, j - 1) - get(image, i - 1, j - 1))>10)
                {
                    if (abs(get(image, i, j) - get(image, i - 1, j)) > 10)
                    {
                        if (abs(get(image, i, j + 1) - get(image, i - 1, j + 1)) >10)
                        {
                            set(output, i - 1, j - 1, 0);
                            set(output, i - 1, j, 0);
                            set(output, i - 1, j + 1, 0);
                        }
                    }
                }
                if (abs(get(image, i, j - 1) - get(image, i + 1, j - 1))>10)
                {
                    if (abs(get(image, i, j) - get(image, i + 1, j))>10)
                    {
                        if (abs(get(image, i, j + 1) - get(image, i + 1, j + 1))>10)
                        {
                            set(output, i + 1, j - 1, 0);
                            set(output, i + 1, j, 0);
                            set(output, i + 1, j + 1, 0);
                        }
                    }
                }
                if (abs((image, i - 1, j) - get(image, i - 1, j + 1))>10)
                {
                    if (abs(get(image, i, j) - get(image, i, j + 1))>10)
                    {
                        if (abs(get(image, i + 1, j) - get(image, i + 1, j + 1))>10)
                        {
                            set(output, i - 1, j + 1, 0);
                            set(output, i, j + 1, 0);
                            set(output, i + 1, j + 1, 0);
                        }
                    }
                }
                if (abs(get(image, i - 1, j) - get(image, i - 1, j - 1))>10)
                {
                    if (abs(get(image, i, j) -get(image, i, j - 1))>10)
                    {
                        if (abs(get(image, i + 1, j) - get(image, i + 1, j - 1))>10)
                        {
                            set(output, i - 1, j - 1, 0);
                            set(output, i, j - 1, 0);
                            set(output, i + 1, j - 1, 0);
                        }
                    }
                }
            }
        }
        for (i = 2; i < (width - 1); i++)
        {
            for (j = 2; j < (height - 1); j++)
            {
                if ((get_output(output, i - 1, j) == 255 && get_output(output, i, j) == 0 && get_output(output, i + 1, j) == 255 && get_output(output, i, j - 1) == 255 && get_output(output, i, j + 1) == 255))
                {
                    if ((get_output(output, i - 1, j - 1) == 0 && get_output(output, i + 1, j - 1) == 0 && get_output(output, i - 1, j + 1) == 0 && get_output(output, i + 1, j + 1) == 0) || (get_output(output, i - 1, j - 1) == 255 && get_output(output, i + 1, j - 1) == 255 && get_output(output, i - 1, j + 1) == 255 && get_output(output, i + 1, j + 1) == 255))
                    {
                        set(output, i, j, 255);
                    }
                }
            }
        }


        return output;
    }


