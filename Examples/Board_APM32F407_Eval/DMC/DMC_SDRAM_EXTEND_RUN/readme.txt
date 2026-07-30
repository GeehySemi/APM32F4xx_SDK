/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2025-04-01
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */
 
&par Example Description

The program aims to show how to use DMC to read and write data to external SDRAM, the test
information will print by USART1.This is a demo program that uses the APM32F407-EVAL board 
and runs on external SDRAM.

• It includes two projects:

  - DMC_SDRAM_BOOT: Stores the source data (located in data.h) that needs to be written to external SDRAM, 
                                     as well as the PC-side logic for jumping to the SDRAM to run the program.
  - DMC_SDRAM_UART: The source code project that runs in the SDRAM.

• Example project usage:

  You need to first compile the DMC_SDRAM_UART project to generate a bin file. Then convert the data in 
  he bin file into a C array (for example, using J-Flash) and store it in the predefined array in data.h of the 
  DMC_SDRAM_BOOT project. After that, compile and download the DMC_SDRAM_BOOT project. Finally, 
  connect the serial port and follow the prompts displayed on the serial terminal for further steps.

&par Hardware Description

using USART1(TX:PA9、RX:PA10).
  - USART1 configured as follow:
  - BaudRate = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Transmit enabled

&par Directory contents

  - DMC/DMC_SDRAM_EXTEND_RUN/DMC_SDRAM_BOOT/Source/apm32f4xx_int.c           Interrupt handlers
  - DMC/DMC_SDRAM_EXTEND_RUN/DMC_SDRAM_BOOT/Source/main.c                    Main program

  - DMC/DMC_SDRAM_EXTEND_RUN/DMC_SDRAM_UART/Source/apm32f4xx_int.c           Interrupt handlers
  - DMC/DMC_SDRAM_EXTEND_RUN/DMC_SDRAM_UART/Source/main.c                    Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407_EVAL Devices.
