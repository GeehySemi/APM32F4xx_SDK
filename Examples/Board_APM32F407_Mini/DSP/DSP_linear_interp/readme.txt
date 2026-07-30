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

Example code demonstrating usage of sin function
and uses linear interpolation to get higher precision.

The demo can be displayed using serial assistant.
After power on, can press KEY1 to start demo test.

&par Hardware Description

USART1_TX(PA9) ----> host_RX
USART1_RX(PA10)----> host_TX

  - USART1 configured as follow:
  - BaudRate = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - DSP/DSP_linear_interp/Source/apm32f4xx_int.c                     Interrupt handlers
  - DSP/DSP_linear_interp/Source/main.c                              Main program
  - DSP/DSP_linear_interp/Source/arm_linear_interp_data.c            Data file used for example.  Generation method described below
  - DSP/DSP_linear_interp/Source/math_helper.c                       Definition of all helper functions required
  - DSP/DSP_linear_interp/Source/arm_linear_interp_example_f32.c     Example code demonstrating usage of sin function
                                                                     and uses linear interpolation to get higher precision

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
