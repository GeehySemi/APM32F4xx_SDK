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

This example shows how to synchronize TMR8 to TMR1 on APM32F411.

  - TMR1 is the master. It is configured in PWM mode, and its update event is the trigger output.
  - TMR8 is the slave. It is configured in PWM mode and gated mode.
  - ITR0 of TMR8 is TMR1.

On APM32F411, the internal connection from TMR1 to TMR8 stays inactive unless the APB1 clock of 
TMR2, TMR3, or TMR4 is enabled. This example enables the TMR2 clock and does not use TMR2. 
Comment out that clock enable to reproduce the limitation: PA8 still outputs PWM, and PC6 stays 
idle. APM32F407 and APM32F427 do not need this extra clock.

APM32F411 TINY Devices:
  - The TMR1 and TMR8 clock is 120MHz.

  - The Master Timer TMR1 frequency :
  - TMR1 frequency = (120M)/ (255 + 1) = 468.75 KHz
  - duty cycle = 128/(255 + 1) = 50%.

  - The TMR8 is running at:
  - (468.75 KHz)/ (3 + 1) = 117.1875 KHz
  - duty cycle = 2/(3 + 1) = 50%

&par Hardware Description

  - using TMR1 CHANNEL1(PA8) to output PWM
  - using TMR8 CHANNEL1(PC6) to output PWM

&par Directory contents

  - TMR/TMR_TMR8Synchro/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_TMR8Synchro/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F411 Tiny Devices.
