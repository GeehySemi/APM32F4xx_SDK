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

This example shows how to synchronize TMR peripherals in cascade mode.

  - cascade mode:
  - TMR2 Master for TMR3,Config PWM Mode and TMR2 Update event is used as Trigger Output
  - TMR3 is slave for TMR2 and Master for TMR4,
  - Config PWM Mode and TMR3 Update event is used as Trigger Output
  - The ITR1(TMR2) is used as input trigger 
  - TMR4 is slave for TMR3,
  - Config PWM Mode
  - The ITR2(TMR3) is used as input trigger

APM32F411 TINY Devices:
  - The TMR2 peripherals clock is 84 MHz.

  - The Master Timer TMR2 frequency :
  - TMR2 frequency = (120M)/ (127 + 1) = 937.5 KHz 
  - duty cycle = 64/(127 + 1) = 50%.

  - The TMR3 is running at:
  - (937.5 KHz)/ (3 + 1) = 234.375 KHz and a 
  - duty cycle = 2/(3 + 1) = 50%

  - The TMR4 is running at:
  - (234.375 KHz)/ (1 + 1) = 117.1875 KHz 
  - duty cycle = 1/(1 + 1) = 50%  

&par Hardware Description

  - using TMR2 CHANNEL1(PA0) to output PWM
  - using TMR3 CHANNEL1(PB4) to output PWM
  - using TMR4 CHANNEL1(PB6) to output PWM

&par Directory contents

  - TMR/TMR_CascadeSynchro/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_CascadeSynchro/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F411 Tiny Devices.
