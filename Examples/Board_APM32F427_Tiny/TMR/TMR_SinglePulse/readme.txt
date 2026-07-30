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

This example shows how to config TMR peripherals to generate a Single Pulse
with an external trigger

  - TMR1 Config Single Pulse Mode and Enable event is used as Trigger Output
  - The TMR1 peripherals clock is 240MHz.
  - Config Timer TMR1 counter clock at 120 MHz:
  - TMR1 counter clock = (240M)/ (1 + 1) = 120MHz

  - TMR1->AUTORLD is 32767, so the trigger input max frequency value
  - is 120MHz/32767 = 3.662KHz .
  - TMR1 pulse = 8191
  - Single Pulse delay value = 8191/(240MHz/2) = 68.25us
  - And the pulse value is fixed to single pulse value
  - Pulse value = (period - pulse)/counter clock
  - = (32767- 8191)/ 120MHz = 204.8us

&par Hardware Description

  - using TMR1 CHANNEL1(PA8) to output Pulse by TMR1 CHANNEL2(PE11)
  - using TMR1 CHANNEL2(PE11) connect the external signal

&par Directory contents

  - TMR/TMR_ExtTriggerSynchro/Source/apm32f4xx_int.c        Interrupt handlers
  - TMR/TMR_ExtTriggerSynchro/Source/main.c                       Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F427 Tiny Devices.
