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

This example shows how to use SPI Peripheral  to  read and write Flash.
Press KEY1 to write data  to Flash. And read data form Flash. if read and 
write success,LED1 will turn on.  The data will be shown on serial assistant 
trough usart1.if error, LED2 will turn on. if initialize the flash error, LED3 will 
turn on.

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Hardware Description
    - Config SPI1 
    - PIN CS=PF5
    - SCK=PA3 
    - MISO=PA4 
    - MOSI=PA5

&par Directory contents

  - SPI/SPI_Flash/Source/apm32f4xx_int.c       Interrupt handlers
  - SPI/SPI_Flash/Source/main.c                      Main program
  - SPI/SPI_Flash/Source/Board_W25Q16.c      provides spi read and write Flash functions

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 EVAL Devices.
