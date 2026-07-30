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

This example describes how to config a communication the CAN in normal mode.
CAN1 transmit a message to CAN2. Then compare the received message with transmitted
message. The result of Polling transmit and Interrupt transmit will displayed on serial assistant
through USART1.

  - Polling transmit success: The LED2 turns on, printf "CAN polling test passed!".
                              Otherwise LED2 toggles, printf "CAN polling test failed!".

  - Interrupt transmit success: The LED3 turns on, printf "CAN interrupt test passed!".
                                 Otherwise LED3 toggles, printf "CAN interrupt test failed".

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

The CAN connection as follows:
     _____________                                     ________________
    |             |                                   |                |
    |             |-PB8(CAN1_RX)  <--------->  CAN_RX-| CAN Transceiver|-CANH  <--------
    |             |-PB9(CAN1_TX)  <--------->  CAN_TX-|                |-CANL  <--     |
    |     MCU     |                                   |________________|          |    |
    |             |                                    ________________           |    |
    |             |                                   |                |          |    |
    |             |-PB12(CAN1_RX) <--------->  CAN_RX-| CAN Transceiver|-CANL  <--     |
    |             |-PB13(CAN1_TX) <--------->  CAN_TX-|                |-CANH  <--------
     _____________                                    |________________|
                                               


&par Directory contents

  - CAN/CAN_Normal/Source/apm32f4xx_int.c           Interrupt handlers
  - CAN/CAN_Normal/Source/main.c                    Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407_EVAL Devices.
