/**********************************************************
 * @file jpeg_to_raw.c
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
 * @date 21 de Junio de 2018
**********************************************************/

#include "jpeg_to_raw.h"
#include "timer.h"


/* Variables "globales" */
static unsigned char *g_pbuffer; //Puntero al buffer con img JPEG
static unsigned int g_buf_size;  // Size de img
static unsigned int g_suma;      // Trackea cuanto se va leyendo del buffer

unsigned char pjpeg_need_bytes_callback(unsigned char *pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    uint n;
    pCallback_data;

    unsigned char *p2 = g_pbuffer + g_suma;
    n = min(g_buf_size - g_suma, buf_size);

    int i = 0;
    for (i = 0; i < n; i++)
    {
        *(pBuf + i) = *(p2 + i);
    }

    *pBytes_actually_read = (unsigned char)(n);
    g_suma += n;

    return 0;
}

int jpeg_to_raw(unsigned char *r, unsigned int sizerx)
{
    int i = 0;
    g_pbuffer = r;
    g_buf_size = sizerx;

    uint8 *output_img[8*160];

    //Verifico JPEG empieza con FF D8 y termina en FF D9

    if ((*g_pbuffer == 255) && (*(g_pbuffer + 1) == 216))
    {
        1 == 1;
       // printf("Primeros 2 bytes OK\n");
    }
    else
    {
        uart_txpc("Error 01\n", 9 );
       // printf("Error de formato primeros 2 bytes\n");
        return 1;
    }

    if ((*(g_pbuffer + g_buf_size - 2) == 255) && (*(g_pbuffer + g_buf_size - 1) == 217))
    {
       2 == 2;
      //  printf("Ultimos 2 bytes OK\n");

    }
    else
    {
        uart_txpc("Error 02\n", 9 );
        //printf("Error de formato ul 2 btimosytes\n");
        return 1;
    }

    g_suma = 0;
    pjpeg_image_info_t image_info;

    int reduce = 0;
    uint8 status = pjpeg_decode_init(&image_info, pjpeg_need_bytes_callback, 0, (unsigned char)reduce);
    if (status)
    {

        uart_txpc("Error 03\n", 9 );
        return 1;
        //printf("pjpeg_decode_init() failed with status %u\n", status);
    }


    int mcu_x = 0;
    int mcu_y = 0;

    uint row_pitch = image_info.m_comps * image_info.m_width;

    int init_block_1 = 0;
    int init_block_2 = 8 * row_pitch;
    int init_block_3 = init_block_2 * 2;
    int buffer_size = image_info.m_width * 3 *8 * image_info.m_comps;

    int start = 0;
    int block_y;

    uint8 temp_buffer[16384];
    uint8* block_pointer = (uint8*)&temp_buffer;

    for (;;)
    {
        int y, x;
        uint8* pDst_row;

        status = pjpeg_decode_mcu();

        if (status)
        {
            if (status != PJPG_NO_MORE_BLOCKS)
            {
                return 1;
            }
            break;
        }

        if (mcu_y >= image_info.m_MCUSPerCol)
        {
            return 1;
        }

        block_y = start < 3 ? mod(mcu_y) : 2;
        pDst_row = block_pointer + (block_y * image_info.m_MCUHeight) * row_pitch + (mcu_x * image_info.m_MCUWidth * image_info.m_comps);

        for (y = 0; y < image_info.m_MCUHeight; y += 8)
        {
            const int by_limit = min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

            for (x = 0; x < image_info.m_MCUWidth; x += 8)
            {
                uint8 *pDst_block = pDst_row + x * image_info.m_comps;

                // Compute source byte offset of the block in the decoder's MCU buffer.
                uint src_ofs = (x * 8U) + (y * 16U);
                const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
                const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
                const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

                const int bx_limit = min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

                int bx, by;
                for (by = 0; by < by_limit; by++)
                {
                    uint8 *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++)
                    {
                        pDst[0] = *pSrcR++;
                        pDst[1] = *pSrcG++;
                        pDst[2] = *pSrcB++;
                        pDst += 3;
                    }

                    pSrcR += (8 - bx_limit);
                    pSrcG += (8 - bx_limit);
                    pSrcB += (8 - bx_limit);

                    pDst_block += row_pitch;
                }
            }

            pDst_row += (row_pitch * 8);
        }

        mcu_x++;
        if (mcu_x == image_info.m_MCUSPerRow)
        {
            mcu_x = 0;
            mcu_y++;
            start++;

            if (start == 2) {
                uint8* bordered_image = border_detection(temp_buffer, image_info.m_width, 8 * 2, output_img);
                uart_txpc( bordered_image, image_info.m_width * 8);
            }
            else if (start > 2){
                uint8* bordered_image = border_detection(temp_buffer, image_info.m_width, 8 * 3, output_img);
                uart_txpc( bordered_image + image_info.m_width * 8, image_info.m_width * 8);

                for (i = init_block_2; i < buffer_size; i++){
                    temp_buffer[i - init_block_2] = temp_buffer[i];
                }
                if (mcu_y == image_info.m_MCUSPerCol) {
                    uart_txpc(bordered_image + image_info.m_width * 8 * 2, image_info.m_width * 8);
                }
            }
        }
    }
    return 0;
}
