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

The example shows how to use FLASH to emulation EEPROM.
You can read/write bytes to Flash using the read/write interface
provided in this module.

notes:
The following is the Flash regional planning of this example.
In this example, you must use between sector 1 and sector 3.

/*
 +--------------------------------------------------------------+
 |                             Flash                            |
 +-------------------------------+------------------------------+
 |  Code  |      User Para area      |           Code           |
 +--------+--------+--------+--------+--------+-----------------+
 |  16KB  |  16KB  |  16KB  |  16KB  |  64KB  | 128KB  | 128KB  |
 | sector | sector | sector | sector | sector | sector | sector |
 |   0    |    1   |   2    |   3    |    4   |   N    | N + 1  |
 |        |        |        |        |        |        |        |
 +--------+--------+--------+--------+--------+-----------------+
*/

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - FMC/Flash_Emulation_Eeprom/Source/apm32f4xx_int.c        Interrupt handlers
  - FMC/Flash_Emulation_Eeprom/Source/main.c                 Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Tiny Devices.
