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

This example shows how to use UART1 to communicate between 1st board and 2nd board by interrupt.
1st board uses UART1 to send a string and then enters the data waiting for 2nd board to return data, 
while 2nd board uses UART1 to receive the string of 1st board, and then sends back the data. 
If the data received and sent are inconsistent, the LED3 of 1st board and 2nd board will turn off, 
otherwise LED3 will turn on.

note:The two boards share a firmware project, and you can use "IS_FIRST_BOARD" to switch different 
firmware to fit the two boards.

&par Hardware Description

    1st board            2nd board
  USART1_TX(PB6) ----> USART1_RX(PB7)
  USART1_RX(PB7) ----> USART1_TX(PB6)
  1st board GND  ----> 2nd board GND

&par Directory contents

  - USART/USART_TwoBoardsInterrupt/Source/main.c                 Main program
  - USART/USART_TwoBoardsInterrupt/Source/apm32f4xx_int.c        Interrupt handlers
&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
