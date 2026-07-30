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

The program shows how to using USART Smartcard mode, in this case,
USART1 sends data to upper computer. You can check the data in a Serial Port Utility.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

Smartcard USART (USART3) configured as follow:
  - BaudRate    = 12096 baud
  - Word Length = 9 Bits
  - 1.5 Stop Bit
  - Even parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Tx and Rx enabled
  - USART Clock enabled
  - USART CPOL Low
  - USART CPHA on first edge
  - USART Last Bit Clock Enabled

To use this example, the user needs to prepare a smartcard module.
  PB10 (USART3_TX) ---> Smartcard IO Pin
  PB11 (Pin)       ---> Smartcard RST Pin
  PB12 (USART3_CK) ---> Smartcard CLK Pin

&par Directory contents

  - USART/USART_Smartcard/Source/main.c                 Main program
  - USART/USART_Smartcard/Source/apm32f4xx_int.c        Interrupt handlers

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
