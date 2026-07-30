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

This example shows how to use one board  SPI Peripheral  to  transfer data.
Press KEY1 to send data from SPI1 to SPI2, if communication success,
LED2 will turn on.  The data will be shown on serial assistant trough usart1.
Press KEY2 to use SPI1 to SPI2 to Realize full-duplex transmission.
if communication success, LED3 will turn on.
The data will be shown on serial assistant trough usart1.

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Hardware Description

Connect SPI1 and SPI2 pins as follows:
SPI1 SCK    (PA5)   to  SPI2 SCK    (PB10)
SPI1 MISO   (PA6)   to  SPI2     (PC2)
SPI1 MOSI   (PA7)   to  SPI2 MOSI   (PC3)

&par Directory contents

  - SPI/SPI_FullDuplex/Source/apm32f4xx_int.c       Interrupt handlers
  - SPI/SPI_FullDuplex/Source/main.c                Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Tiny Devices.
