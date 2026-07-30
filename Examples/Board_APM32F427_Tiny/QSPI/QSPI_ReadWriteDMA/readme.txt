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

This example shows how to use QSPI to control W25Qxx into qual mode.
Read W25Qxx ID and communicate with W25Qxx by DMA. If is ok, LED3 will blink.
Otherwish, LED2 will blink.

The model of W25Qxx is W25Q32JV.

&par Hardware Description

  - QSPI_IO0(PF8)
  - QSPI_IO1(PF9)
  - QSPI_IO2(PF7)
  - QSPI_IO3(PF6)
  - QSPI_CLK(PF10)
  - QSPI_CS (PB9)

&par Directory contents

  - QSPI/QSPI_ReadWrite/Source/apm32f4xx_int.c     Interrupt handlers
  - QSPI/QSPI_ReadWrite/Source/main.c                     Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F427 Tiny Devices.
