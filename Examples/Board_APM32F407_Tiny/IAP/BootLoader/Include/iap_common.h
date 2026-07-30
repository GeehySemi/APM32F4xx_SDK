/*!
 * @file        common.h
 *
 * @brief       Header for common.c module
 *
 * @version     V1.0.0
 *
 * @date        2025-02-15
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
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
#ifndef _COMMON_H
#define _COMMON_H

/* Includes ***************************************************************/
#include "main.h"
#include "iap_flash.h"
#include "iap_usart.h"

/* Exported macro *********************************************************/

#define USART_COM                           (DEBUG_USART)

#define USART_COM_RXDATA                    (USART_RxData(USART_COM))
#define USART_COM_TXDATA(c)                 (USART_TxData(USART_COM,c))

#define USART_COM_RXREADY                   (USART_ReadStatusFlag(USART_COM, USART_FLAG_RXBNE) == SET)
#define USART_COM_TXFINISH                  (USART_ReadStatusFlag(USART_COM, USART_FLAG_TXBE) == RESET)

#define USART_COM_CLEAR_OVREE               (USART_ClearStatusFlag(USART_COM, USART_FLAG_OVRE))

#define CHECK_AF(s)                         ((s >= 'A') && (s <= 'F'))
#define CHECK_af(s)                         ((s >= 'a') && (s <= 'f'))
#define CHECK_09(s)                         ((s >= '0') && (s <= '9'))
#define CHECK_HEX(s)                        CHECK_AF(s) || CHECK_af(s) || CHECK_09(s)
#define CHECK_DEC(s)                        CHECK_09(s)
#define CON_DEC(s)                          (s - '0')

#define CON_HEX_alpha(s)                    (CHECK_AF(s) ? (s - 'A'+10) : (s - 'a'+10))
#define CON_HEX(s)                          (CHECK_09(s) ? (s - '0') : CON_HEX_alpha(s))

#define SendString(x)                       Serial_SendString((uint8_t*)(x))

/* Exported typedef *******************************************************/

/* Exported function prototypes *******************************************/

/** function declaration*/
void IntConStr(uint8_t *str, int32_t conint);
uint32_t StrConInt(uint8_t* str, int32_t* conint);
uint32_t GetKeyPressed(uint8_t* inputKey);
uint8_t ReadKey(void);
uint8_t ReadKey_TimeOut(uint32_t delay_timeOut);
void Send_Char(uint8_t character);
void Serial_SendString(uint8_t* str);
void ReadInputString(uint8_t* p);

#endif
