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

The program to show how to configure the TMR1 peripheral to generate 7 PWM 
with 7 different duty cycles (80%, 70%, 60%, 50%, 40%, 30% and 20%).

The objective is to generate 7 PWM signal at 1 KHz:
  - SystemCoreClock = 168 MHz.
  - TMR1CLK = SystemCoreClock, Prescaler = 167, TMR1 counter clock = 1 MHz
  - TMR1_Period = (1000000 / 1000) - 1
  - Channel 1 duty cycle = 80%
  - Channel 2 duty cycle = 70%
  - Channel 3 duty cycle = 60%
  - Channel 4 duty cycle = 50%
  - Channel 3N duty cycle = 40%
  - Channel 2N duty cycle = 30%
  - Channel 1N duty cycle = 20%

Display TMR1 waveform by oscilloscope.

&par Hardware Description

  - using TMR1 Channel 1 (PA8) to output PWM
  - using TMR1 Channel 2 (PE11) to output PWM
  - using TMR1 Channel 3 (PE13) to output PWM
  - using TMR1 Channel 4 (PE14) to output PWM
  - using TMR1 Channel 3N (PB15) to output PWM
  - using TMR1 Channel 2N (PB14) to output PWM
  - using TMR1 Channel 1N (PA7) to output PWM

&par Directory contents

  - TMR/TMR_TMR1PWMOutput/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_TMR1PWMOutput/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
