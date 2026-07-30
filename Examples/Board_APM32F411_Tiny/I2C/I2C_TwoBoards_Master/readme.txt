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

This example shows how to control I2C devices and communicate between two different boards.

To use this example, you need to load the software into two boards(let's call them
Board master and Board Slave) then connect these two boards through I2C lines and GND.

 - At startup, Boards master and slave are both in slave receiver mode and wait for messages
   to be received.
 - When KEY1 is pressed on Board master, the master transmitter sents "Hello slave" to Board slave.
   The message is displayed on serial assistant through USART1 on Board slave. LED2 is alternately On
 - When KEY1 is pressed on Board slave, the slave transmitter sents "Hello master" to Board master.
   The message is displayed on serial assistant through USART1 on Board master. LED3 is alternately On

&par Hardware Description

   I2C     BOARD1         BOARD2
   SCL      PB6             PB6     (External Pull up resistance)
   SDA      PB7             PB7     (External Pull up resistance)

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - I2C/I2C_TwoBoards_Master/Source/apm32f4xx_int.c         Interrupt handlers
  - I2C/I2C_TwoBoards_Master/Source/main.c                  Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F411 Tiny Devices.
