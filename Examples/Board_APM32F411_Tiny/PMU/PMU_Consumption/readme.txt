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

This example shows how to configure system to measure different low power modes 
consumption. Includes the following modes:
    - Sleep Mode
    - Stop mode with RTC wakeup
    - Standby mode with WKUP pin
    - Standby mode with RTC wakeup
    - Standby mode with RTC wakeup and BKPSRAM

To run this example, users has to follow the following steps:
    - Select different modes by setting 1 of the following code:
    /*
    * #define SYS_SLEEP_MODE
    * #define SYS_STOP_MODE
    * #define SYS_STANDBY_MODE
    * #define SYS_STANDBY_RTC_MODE
    * #define SYS_STANDBY_RTC_BKPSRAM_MODE
    */
    - Use an external ammeter to measure the IDD current.

Here below a detailed description of the example code:
After system reset, the program waits for KEY1 button to be pressed to enter
the selected Low Power mode. The following are different modes of awakening:
    - SYS_SLEEP_MODE
    Wake up by pressing the KEY1 button.
    
    - SYS_STOP_MODE
    Wake up by RTC wakeup(10s) interrupt.
    
    - SYS_STANDBY_MODE
    Wake up by WKUP PIN(PA0) input high logical voltage.
    
    - SYS_STANDBY_RTC_MODE
    Wake up by RTC wakeup(10s) event.
    
    - SYS_STANDBY_RTC_BKPSRAM_MODE
    Wake up by RTC wakeup(10s) event.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - PMU/PMU_Consumption/Source/apm32f4xx_int.c          Interrupt handlers
  - PMU/PMU_Consumption/Source/main.c                   Main program
  - PMU/PMU_Consumption/Source/low_power_modes.c        Low power modes

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F411 Tiny Devices.
