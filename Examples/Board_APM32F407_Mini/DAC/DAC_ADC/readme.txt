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

This example provides a example of how to use DAC channel 1(PA4) to output
voltage to ADC channel 0(PA0). The converted voltage of PA4 is detected by
ADC  channel 0 and displayed on serial assistant through USART1.

 -DAC generates voltage from 0V to 3.3V

 -when the voltage is greater than 3.1V, then LED2 turns on and LED3 turns off.
 -when the voltage is less than 0.8V, then LED2 turns off and LED3 turns on.
 -when the voltage is greater than 0.8V but less than 3.1V, then LED2 and LED3 turn on.

&par Hardware Description

connect DAC Channel1 (PA4) to ADC Channel 0 (PA0)

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - DAC/DAC_ADC/Source/apm32f4xx_int.c     Interrupt handlers
  - DAC/DAC_ADC/Source/main.c                    Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
