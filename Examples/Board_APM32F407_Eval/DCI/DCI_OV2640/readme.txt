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

The program aims to show how to use interrupt or DMA to get camera image data by using DCI, in this case,
DCI continuously get the data of the OV2640 camera through DMA and sends it to the host by USART2(jpeg mode).
In another regb 565 mode, DCI continuously get the data of the OV2640 camera through DMA and transfer it to
LCD data register to display.And OV2640 can be set to jpeg and rgb565 data output format.

After power on, can switch between jpeg mode or rgb565 mode by KEY1 and KEY2.
KEY1 ----> rgb565 mode
KEY2 ----> jpeg mode

&par Hardware Description

  - USART1_TX(PA9) ----> host_RX
  - USART1_RX(PA10)----> host_TX

  - USART1 configured as follow:
  - BaudRate = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  
  - USART2_TX(PA2) ----> host_RX
  - USART2_RX(PA3) ----> host_TX

  - USART1 configured as follow:
  - BaudRate = 460800
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

  - SCCB_SCL(PB8)     ----> OV2640_SCCB_SCL
  - SCCB_SDA(PB9)     ----> OV2640_SCCB_SDA
  - OV2640_PWDN(PA0)  ----> OV2640_PWDN
  - OV2640_RST(PF13)  ----> OV2640_RESET
  - DCI_HSYNC(PA4)    ----> OV2640_HREF
  - DCI_PCLK(PA6)     ----> OV2640_PCLK
  - DCI_VSYNC(PB7)    ----> OV2640_VSYNC
  - DCI_D0(PC6)       ----> OV2640_Y0
  - DCI_D1(PC7)       ----> OV2640_Y1
  - DCI_D2(PC8)       ----> OV2640_Y2
  - DCI_D3(PC9)       ----> OV2640_Y3
  - DCI_D4(PE4)       ----> OV2640_Y4
  - DCI_D5(PB6)       ----> OV2640_Y5
  - DCI_D6(PE5)       ----> OV2640_Y6
  - DCI_D7(PE6)       ----> OV2640_Y7

&par Directory contents

  - DCI/DCI_OV2640/Source/apm32f4xx_int.c                   Interrupt handlers
  - DCI/DCI_OV2640/Source/main.c                            Main program
  - Boards/Board_APM32F407_EVAL/src/bsp_usart.c             Usart board support package body
  - DCI/DCI_OV2640/Source/bsp_ov2640.c                      OV2640 board support package body
  - Boards/Board_APM32F407_EVAL/src/bsp_delay.c             Delay board support package body
  - DCI/DCI_OV2640/Source/bsp_dci.c                         DCI board support package body
  - DCI/DCI_OV2640/Source/bsp_key.c                         Key board support package body

&par IDE environment

  - MDK-ARM V5.29
  - EWARM V8.50.4.26131
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407_EVAL Devices.
