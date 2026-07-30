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

This example describes how to use OTG FS2 to simulate a HID mouse.
When PC enter sleep mode, device will enter lowpower mode.
You can wake up the PC and the device at the same time by using
another keyboard and mouse device on the PC or by pressing the KEY2.

Press KEY2 will move the cursor to the right.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

    - VCC5V             ----------> VBUS
    - OTG_FS2_DM(PB14)  ----------> USB_DN
    - OTG_FS2_DP(PB15)  ----------> USB_DP
    - GND               ----------> GND

&par Directory contents

  - Device_Examples/OTGD_HID_WakeUp_LowPower_FS2/Source/apm32f4xx_int.c          Interrupt handlers
  - Device_Examples/OTGD_HID_WakeUp_LowPower_FS2/Source/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F427 Tiny Devices.
