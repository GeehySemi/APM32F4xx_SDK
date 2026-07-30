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

This example shows how to control I2C devices and communicate to EEPROM.

To use this example, you need to load the software into APM32F407_TINY boards then connect EEPROM through I2C lines,VCC and GND.

- During startup, press KEY1 to put the development board into write mode. The board will send "Hello!" to the EEPROM and store it in the 0x00 memory space of the EEPROM.  
- When KEY2 is pressed to select read mode after the software download is completed, the development board will receive data from the EEPROM's memory space.

&par Hardware Description

   I2C     BOARD         
   SCL      PB6          (External Pull up resistance)
   SDA      PB7          (External Pull up resistance)

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - I2C/I2C_EEPROM/Source/apm32f4xx_int.c          Interrupt handlers
  - I2C/I2C_EEPROM/Source/main.c                        Main program

&par IDE environment

  - MDK-ARM V5.40

&par Hardware and Software environment

  - This example runs on APM32F407_MINI Devices.
