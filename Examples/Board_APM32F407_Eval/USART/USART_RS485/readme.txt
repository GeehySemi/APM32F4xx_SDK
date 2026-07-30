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

This example describes how to use MAX485 module to transfer data with
serial COM. When you send some data from PC to USART3(PB10,PB11), then
MCU will send the same data to PC.


The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

The USART3 is configured as follows:
    - TX:PB10, RX:PB11
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

The CAN connection as follows:
     _____________                    ____________          ____________
    |             |                  |            |        |            |
    |             |                  |            |-TX  <->|            |
    |             |-MAX485-A <--> A- |            |        |            |
    |             |                  |            |-RX  <->|            |
    | EVAL Board  |                  | 485 module |        |     PC     |
    |             |-MAX485-B <--> B- |            |-VCC <->|            |
    |             |                  |            |        |            |
    |             |                  |            |-GND <->|            |
    |_____________|                  |____________|        |____________|
                                               


&par Directory contents

  - USART/USART_RS485/Source/apm32f4xx_int.c           Interrupt handlers
  - USART/USART_RS485/Source/main.c                    Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 EVAL Devices.
