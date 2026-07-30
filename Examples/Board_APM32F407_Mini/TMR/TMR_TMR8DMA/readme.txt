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

The program to show how to use DMA to transfer Data from
memory to TMR8 Capture Compare Register1 to change the Duty Cycle.

  - TMR8 Repetition counter = 6

  - SystemCoreClock = 168 MHz.
  - TMR8CLK = SystemCoreClock, Prescaler = 167, TMR8 counter clock = 1 MHz
  - TMR8_Period = (1000000 / 1000) - 1

TMR8 channel 1 generate complementary PWM signal at 1 KHz, and a variable 
duty cycle changed by the DMA after six(TMR8 Repetition counter) of Update DMA request.

each 6 Update Requests, the TMR8 Channel 1 Duty Cycle changes to the next new value:
  - value1 = 800
  - Channel 1 duty cycle = 80%
  - Channel 1N duty cycle = 20%
  - value2 = 400
  - Channel 1 duty cycle = 40%
  - Channel 1N duty cycle = 60%
  - value3 = 200
  - Channel 1 duty cycle = 20%
  - Channel 1N duty cycle = 80%

Display TMR8 waveform by oscilloscope.

&par Hardware Description

  - using TMR8 Channel 1 (PC6) to output PWM
  - using TMR8 Channel 1N (PA5) to output PWM

&par Directory contents

  - TMR/TMR_TMR8DMA/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_TMR8DMA/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
