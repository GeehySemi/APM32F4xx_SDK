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

This example describes how to use the ADC to convert Channel0 
in Dual interleaved mode using DMA in mode 3.

A DMA request is generated each time 2 data items are available
1st request: ADC->CDATA[15:0] = (ADC2->REGDATA[7:0] << 8) | ADC1->REGDATA[7:0] 
2nd request: ADC->CDATA[15:0] = (ADC2->REGDATA[7:0] << 8) | ADC1->REGDATA[7:0]

The ADC1 and ADC2 are configured to convert ADC Channel 0, with conversion 
triggered by software.
The Dual interleaved delay is configured 6 ADC clock cycles.
By this way, ADC channel 0 is converted each 6 cycles.

The converted voltage is displayed on serial assistant through USART1.

&par Hardware Description

using ADC1_Channel0(PA0)

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - ADC/ADC_DualInterleavedMode/Source/main.c                 Main program
  - ADC/ADC_DualInterleavedMode/Source/apm32f4xx_int.c        Interrupt handlers

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Tiny Devices.
