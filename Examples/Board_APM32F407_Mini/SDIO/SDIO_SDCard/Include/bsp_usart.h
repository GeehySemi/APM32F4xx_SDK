/*!
 * @file        bsp_usart.h
 *
 * @brief       Header for bsp_usart.c module
 *
 * @version     V1.0.4
 *
 * @date        2025-02-15
 *
 * @attention
 *
 *  Copyright (C) 2021-2025 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef __BSP_USART_H
#define __BSP_USART_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "Board.h"
#include "board_delay.h"

/* Exported macro *********************************************************/

#define USART1_REV_LEN          200

/* Exported typedef *******************************************************/

/*!
 * @brief    USART receive status structure
 */
typedef struct
{
    union
    {
        uint16_t USART_RX_STA;

        struct
        {
            uint16_t VALID_LENGHT                   :14;    /*!< The lenght of valid data */
            uint16_t FLAG_FRAME_BREAK               :1;     /*!< Data frame break flag */
            uint16_t FLAG_FRAME_END                 :1;     /*!< Data frame end flag */
        } USART_RX_STA_B;
    };
} USART_RX_STA_T;

/* Exported function prototypes *******************************************/

extern u8 rxBufUSART1[USART1_REV_LEN];
extern USART_RX_STA_T rxStaUSART1;

/** function declaration*/
void USART1_Init(uint32_t baud);
void USART2_Init(uint32_t baud);
void USART_Write(USART_T* usart,uint8_t *dat, uint32_t count);
void USART_Receive_Isr(void);

#ifdef __cplusplus
}
#endif

#endif

