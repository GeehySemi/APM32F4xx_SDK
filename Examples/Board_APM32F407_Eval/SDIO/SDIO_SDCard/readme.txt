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

The program aims to show how to DMA or polling mode to write and read SD card data by SDIO, in this case,
The data can write to SD card or read data from SD card by SDIO. 
Verification will occur after transmission,

The SD card sector data can be displayed using serial assistant.
After power on, can switch between single block test or multi block test by KEY1 and KEY2.
KEY1 ----> single block test
KEY2 ----> multi block test

notes:
It is best to use SD card V2.0 to test.

&par Hardware Description

USART1_TX(PA9) ----> host_RX
USART1_RX(PA10)----> host_TX

  - USART1 configured as follow:
  - BaudRate = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

  - SDIO_D0(PC8)     ----> SD_D0
  - SDIO_D1(PC9)     ----> SD_D1
  - SDIO_D2(PC10)    ----> SD_D2
  - SDIO_D3(PC11)    ----> SD_D3
  - SDIO_CK(PC12)    ----> SD_CK
  - SDIO_CMD(PD2)    ----> SD_CMD

&par Directory contents

  - SDIO/SDIO_SDCard/Source/apm32f4xx_int.c     Interrupt handlers
  - SDIO/SDIO_SDCard/Source/main.c              Main program
  - Boards/Board_APM32F407_EVAL/src/bsp_usart.c         Usart board support package body
  - Boards/Board_APM32F407_EVAL/src/bsp_delay.c         Delay board support package body
  - SDIO/SDIO_SDCard/Source/bsp_sdio.c          SDIO board support package body
  - SDIO/SDIO_SDCard/Source/bsp_key.c           Key board support package body

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 EVAL Devices.
