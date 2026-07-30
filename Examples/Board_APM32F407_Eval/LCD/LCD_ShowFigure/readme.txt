/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
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

&par Example Description

The program aims to show how to use LCD Driver to show Figure on LCD Screen.
You can push KEY1 to switch the figures, and the LED will show different status.
(LCD Screen is 4.3 inch and 800 x 480 pixe).

&par Hardware Description

  - LCD_I2C_SCL(PD7)    ----> CTP_SCL
  - LCD_I2C_SDA(PD3)    ----> CTP_SDA
  - LCD_GT_RST(PD6)     ----> CTP_RST
  - LCD_GT_INT(PE2)     ----> CTP_INT
  - LCD_BK(PF14)        ----> LCD BK
  - LCD_CS(PG10)        ----> LCD_CS
  - LCD_WR(PD5)         ----> LCD_WR
  - LCD_RD(PD4)         ----> LCD_RD
  - LCD_RS(PG0)         ----> LCD_RS
  - LCD_RST(PE3)        ----> LCD_RST
  - SMC_D0(PD14)        ----> LCD_DB0
  - SMC_D1(PD15)        ----> LCD_DB1
  - SMC_D2(PD0)         ----> LCD_DB2
  - SMC_D3(PD1)         ----> LCD_DB3
  - SMC_D4(PE7)         ----> LCD_DB4
  - SMC_D5(PE8)         ----> LCD_DB5
  - SMC_D6(PE9)         ----> LCD_DB6
  - SMC_D7(PE10)        ----> LCD_DB7
  - SMC_D8(PE11)        ----> LCD_DB8
  - SMC_D9(PE12)        ----> LCD_DB9
  - SMC_D10(PE13)       ----> LCD_DB10
  - SMC_D11(PE14)       ----> LCD_DB11
  - SMC_D12(PE15)       ----> LCD_DB12
  - SMC_D13(PD8)        ----> LCD_DB13
  - SMC_D14(PD9)        ----> LCD_DB14
  - SMC_D15(PD10)       ----> LCD_DB15

&par Directory contents

  - LCD/LCD_ShowFigure/Source/apm32f4xx_int.c                    Interrupt handlers
  - LCD/LCD_ShowFigure/Source/main.c                             Main program
  - Boards/Board_APM32F407_EVAL/src/bsp_lcd.c                    LCD Driver
  - Boards/Board_APM32F407_EVAL/src/bsp_lcdFont.c                Font for LCD Driver

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 EVAL Devices.
