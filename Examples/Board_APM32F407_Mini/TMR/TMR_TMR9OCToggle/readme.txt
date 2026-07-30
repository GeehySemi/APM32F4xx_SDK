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

The program to show how to configure the TMR9 peripheral to generate 2 waveform
with 2 different frequencies (5KHz and 50KHz).

  - SystemCoreClock = 168MHz.
  - TMR9CLK = SystemCoreClock = 168MHz.Prescaler = 0, TMR counter clock = 168 MHz
  - The TMR9 CC1 register = SystemCoreClock/10000
  - CC1 update rate = 10KHz
  - Channel 1 waveform frequency = 5KHz
  - The TMR9 CC2 register = SystemCoreClock/100000
  - CC2 update rate = 100KHz
  - Channel 2 waveform frequency = 50KHz

Display TMR9 waveform by oscilloscope.

&par Hardware Description

  - using TMR9 Channel 1 (PA2) to output waveform 
  - using TMR9 Channel 2 (PA3) to output waveform 

&par Directory contents

  - TMR/TMR_TMR9OCToggle/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_TMR9OCToggle/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
