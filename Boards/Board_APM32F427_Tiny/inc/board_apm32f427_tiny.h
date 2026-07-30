/*!
 * @file        board_apm32f427_tiny.h
 *
 * @brief       This file contains definitions for Leds, push-buttons hardware resources
 *
 * @version     V1.0.3
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
#ifndef __BOARD_APM32F427_TINY_H
#define __BOARD_APM32F427_TINY_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "apm32f4xx.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_eint.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_syscfg.h"
#include "apm32f4xx_dma.h"

/* Exported typedef *******************************************************/

typedef enum
{
    LED2 = 0,
    LED3 = 1
} BOARD_LED_T;

typedef enum
{
    BUTTON_KEY2 = 0,
    BUTTON_KEY3 = 1
} BOARD_BUTTON_T;

typedef enum
{
    BUTTON_MODE_GPIO = 0,
    BUTTON_MODE_EINT = 1
} BOARD_BUTTON_MODE_T;

typedef enum
{
    COM1 = 0,
    COM2 = 1
} BOARD_COM_T;

/**
 * @brief   gpio port source define
 */
typedef enum
{
   GPIO_PORT_SOURCE_A,
   GPIO_PORT_SOURCE_B,
   GPIO_PORT_SOURCE_C,
   GPIO_PORT_SOURCE_D,
   GPIO_PORT_SOURCE_E,
   GPIO_PORT_SOURCE_F,
   GPIO_PORT_SOURCE_G,
} GPIO_PORT_SOURCE_T;

/* Exported macro *********************************************************/
#define LEDn                             2

#define LED2_PIN                         GPIO_PIN_0
#define LED2_GPIO_PORT                   GPIOF
#define LED2_GPIO_CLK                    RCM_AHB1_PERIPH_GPIOF

#define LED3_PIN                         GPIO_PIN_1
#define LED3_GPIO_PORT                   GPIOF
#define LED3_GPIO_CLK                    RCM_AHB1_PERIPH_GPIOF

#define BUTTONn                          2

/**
 * @brief Key2 push-button
 */
#define KEY2_BUTTON_PIN                   GPIO_PIN_9
#define KEY2_BUTTON_GPIO_PORT             GPIOD
#define KEY2_BUTTON_GPIO_CLK              RCM_AHB1_PERIPH_GPIOD
#define KEY2_BUTTON_EINT_LINE             EINT_LINE_9
#define KEY2_BUTTON_EINT_PORT_SOURCE      SYSCFG_PORT_GPIOD
#define KEY2_BUTTON_EINT_PIN_SOURCE       SYSCFG_PIN_9
#define KEY2_BUTTON_EINT_IRQn             EINT9_5_IRQn
/**
 * @brief Key3 push-button
 */
#define KEY3_BUTTON_PIN                   GPIO_PIN_10
#define KEY3_BUTTON_GPIO_PORT             GPIOD
#define KEY3_BUTTON_GPIO_CLK              RCM_AHB1_PERIPH_GPIOD
#define KEY3_BUTTON_EINT_LINE             EINT_LINE_10
#define KEY3_BUTTON_EINT_PORT_SOURCE      SYSCFG_PORT_GPIOD
#define KEY3_BUTTON_EINT_PIN_SOURCE       SYSCFG_PIN_10
#define KEY3_BUTTON_EINT_IRQn             EINT15_10_IRQn

#define COMn                        2

/**
 * @brief Definition for COM port1, connected to USART1
 */
#define COM1_PORT                   USART1
#define COM1_CLK                    RCM_APB2_PERIPH_USART1
#define COM1_TX_PIN                 GPIO_PIN_9
#define COM1_TX_GPIO_PORT           GPIOA
#define COM1_TX_GPIO_CLK            RCM_AHB1_PERIPH_GPIOA
#define COM1_TX_PIN_SOURCE          GPIO_PIN_SOURCE_9
#define COM1_RX_PIN                 GPIO_PIN_10
#define COM1_RX_GPIO_PORT           GPIOA
#define COM1_RX_GPIO_CLK            RCM_AHB1_PERIPH_GPIOA
#define COM1_RX_PIN_SOURCE          GPIO_PIN_SOURCE_10
#define COM1_IRQn                   USART1_IRQn
#define COM1_GPIO_AF                GPIO_AF_USART1

/**
 * @brief Definition for COM port2, connected to USART6
 */
#define COM2_PORT                   USART6
#define COM2_CLK                    RCM_APB2_PERIPH_USART6
#define COM2_TX_PIN                 GPIO_PIN_6
#define COM2_TX_GPIO_PORT           GPIOC
#define COM2_TX_PIN_SOURCE          GPIO_PIN_SOURCE_6
#define COM2_TX_GPIO_CLK            RCM_AHB1_PERIPH_GPIOC
#define COM2_RX_PIN                 GPIO_PIN_7
#define COM2_RX_GPIO_PORT           GPIOC
#define COM2_RX_GPIO_CLK            RCM_AHB1_PERIPH_GPIOC
#define COM2_RX_PIN_SOURCE          GPIO_PIN_SOURCE_6
#define COM2_IRQn                   USART6_IRQn
#define COM2_GPIO_AF                GPIO_AF_USART6

/* Exported function prototypes *******************************************/

void BOARD_LEDInit(BOARD_LED_T Led);
void BOARD_LEDOn(BOARD_LED_T Led);
void BOARD_LEDOff(BOARD_LED_T Led);
void BOARD_LEDToggle(BOARD_LED_T Led);

void BOARD_PBInit(BOARD_BUTTON_T Button, BOARD_BUTTON_MODE_T Button_Mode);
uint32_t BOARD_PBGetState(BOARD_BUTTON_T Button);

void BOARD_COMInit(BOARD_COM_T COM, USART_Config_T* configStruct);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_APM32F427_TINY_H */
