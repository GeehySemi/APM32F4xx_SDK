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

This example shows how to configure TCP Client to connect server.

 phenomenon :
  - After initialization, You can see the system information on serial assisatant through USART1 or LCD screen.
  - connect server(IP 192.168.73.158:6000) by KEY1. And disconnect server by KEY2.
  - you can send data to the EVAL board by TCP server using tcp assisatant.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents
  - ETH/ETH_TCP_client/Source/apm32f4xx_int.c         Interrupt handlers
  - ETH/ETH_TCP_client/Source/main.c                  Main program
  - ETH/ETH_TCP_client/Source/board_LAN8720A.c        Provides functions for configurate LAN8720A

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment
  - This example runs on APM32F407-EVAL Devices, need to define APM32F407_EVAL.
