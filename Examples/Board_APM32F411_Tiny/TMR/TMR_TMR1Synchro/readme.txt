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

This example shows how to synchronize TMR1 and TMR peripherals in parallel mode.

  - parallel mode:
  - TMR1 Master for TMR2 and TMR3, Config PWM Mode and TMR1 Update event is used as Trigger Output
  - TMR2 is slave for TMR1,
  - Config PWM Mode
  - The ITR0(TMR1) is used as input trigger 
  - TMR3 is slave for TMR1,
  - Config PWM Mode
  - The ITR0(TMR1) is used as input trigger

APM32F411 TINY Devices:
  - The TMR1 peripherals clock is 120MHz.

  - The Master Timer TMR1 frequency :
  - TMR1 frequency = (120M)/ (255 + 1) = 468.75 KHz 
  - duty cycle = 128/(255 + 1) = 50%.

  - The TMR2 is running at:
  - (468.75 KHz)/ (3 + 1) = 117.1875 KHz
  - duty cycle = 2/(3 + 1) = 50%

  - The TMR3 is running at:
  - (468.75 KHz)/ (1 + 1) = 234.375 KHz 
  - duty cycle = 1/(1 + 1) = 50%  

&par Hardware Description

  - using TMR1 CHANNEL1(PA8) to output PWM
  - using TMR2 CHANNEL1(PA0) to output PWM
  - using TMR3 CHANNEL1(PB4) to output PWM

&par Directory contents

  - TMR/TMR_TMR1Synchro/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_TMR1Synchro/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F411 Tiny Devices.
