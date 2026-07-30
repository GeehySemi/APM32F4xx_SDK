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

The program to show how to configure the TMR11 peripheral in PWM mode 

The objective is to generate 1 PWM signal at 1 KHz:

  - SystemCoreClock = 168MHz.
  - TMR11CLK = SystemCoreClock = 168MHz.Prescaler = 167, TMR counter clock = 1MHz

  - TMR11_Period = (1000000 / 1000) - 1
  - TMR11_pulse = 800
  - Channel 1 duty cycle = 80%

Display TMR11 waveform by oscilloscope.

&par Hardware Description

  - using TMR11 Channel 1 (PB9) to output PWM

&par Directory contents

  - TMR/TMR_TMR11PWMOutput/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_TMR11PWMOutput/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Tiny Devices.
