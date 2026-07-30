/*!
 * @file        board_i2c.h
 *
 * @brief       Header for board_i2c.c module
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
#ifndef _BOARD_I2C_H
#define _BOARD_I2C_H

/* Includes ***************************************************************/
#include "main.h"

/* Exported macro *********************************************************/
#define LCD_I2C_SCL_GPIO_PIN            GPIO_PIN_7
#define LCD_I2C_SCL_GPIO_PORT           GPIOD
#define LCD_I2C_SCL_GPIO_CLK            RCM_AHB1_PERIPH_GPIOD
#define LCD_I2C_SCL_SOURCE              GPIO_PIN_SOURCE_7

#define LCD_I2C_SDA_GPIO_PIN            GPIO_PIN_3
#define LCD_I2C_SDA_GPIO_PORT           GPIOD
#define LCD_I2C_SDA_GPIO_CLK            RCM_AHB1_PERIPH_GPIOD
#define LCD_I2C_SDA_SOURCE              GPIO_PIN_SOURCE_3

#define LCD_I2C_SDA_IN()               { LCD_I2C_SDA_GPIO_PORT->MODE &= ~(3 << (3 * 2)); LCD_I2C_SDA_GPIO_PORT->MODE |= 0 << 3 * 2; }
#define LCD_I2C_SDA_OUT()              { LCD_I2C_SDA_GPIO_PORT->MODE &= ~(3 << (3 * 2)); LCD_I2C_SDA_GPIO_PORT->MODE |= 1 << 3 * 2; }

#define LCD_I2C_SCL_1                   GPIO_SetBit(LCD_I2C_SCL_GPIO_PORT,LCD_I2C_SCL_GPIO_PIN)
#define LCD_I2C_SCL_0                   GPIO_ResetBit(LCD_I2C_SCL_GPIO_PORT,LCD_I2C_SCL_GPIO_PIN)

#define LCD_I2C_SDA_1                   GPIO_SetBit(LCD_I2C_SDA_GPIO_PORT,LCD_I2C_SDA_GPIO_PIN)
#define LCD_I2C_SDA_0                   GPIO_ResetBit(LCD_I2C_SDA_GPIO_PORT,LCD_I2C_SDA_GPIO_PIN)

#define LCD_I2C_SDA_INPUT               GPIO_ReadInputBit(LCD_I2C_SDA_GPIO_PORT,LCD_I2C_SDA_GPIO_PIN)

#define LCD_I2C_WR_BIT                  0
#define LCD_I2C_RD_BIT                  1

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/* Exported variables *****************************************************/

/* Exported function prototypes *******************************************/

void BOARD_I2CGPIOInit(void);
uint8_t BOARD_I2CReadData(uint8_t deviceAddr, uint8_t* buffer, uint16_t readNumber);
uint8_t BOARD_I2CWriteData(uint8_t deviceAddr, uint8_t* buffer, uint16_t writeNumber);

#endif /* _BOARD_I2C_H */
