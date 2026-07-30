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

This example describes how to use OTG FS to simulate a CDC device and how to bridge
USB CDC data to SPI full-duplex interrupt communication by using the APM32F4xx standard library.

Test flow:
  - The PC sends 1 to 32 bytes to the USB CDC port.
  - The firmware starts SPI1(master) and SPI2(slave) interrupt communication at the same time.
  - SPI2 returns a generated response byte stream: SPI2_TX[i] = USB_RX[i] XOR 0xA5.
  - After the interrupt transfer is complete, the firmware sends one result report back by USB CDC.

&par Hardware Description

USB FS connection:
  - OTG_FS_VBUS(PA9) ----------> X(Use to USART1)
  - OTG_FS_ID(PA10)  ----------> X(Use to USART1)
  - VCC5V            ----------> VBUS
  - OTG_FS_DM(PA11)  ----------> USB_DN
  - OTG_FS_DP(PA12)  ----------> USB_DP
  - GND              ----------> GND

USART1 debug connection:
  - TX:PA9, RX:PA10, 115200-8-N-1

SPI loopback jumpers on the same board:
  - SPI1_NSS(PA15) ----------> SPI2_NSS(PB12)
  - SPI1_SCK(PB3)  ----------> SPI2_SCK(PB13)
  - SPI1_MISO(PB4) ----------> SPI2_MISO(PC2)
  - SPI1_MOSI(PB5) ----------> SPI2_MOSI(PC3)
  - GND             ----------> GND

SPI role configuration:
  - SPI1 works as master with hardware NSS output.
  - SPI2 works as slave with hardware NSS input.
  - Both use 8-bit full-duplex mode and interrupt-driven transfer.

&par Test Method

  - Build and download the project.
  - Connect the USB FS port to the PC and open the virtual COM port.
  - Send any 1 to 32 bytes to the CDC port.
  - The firmware returns a formatted report containing:
      USB->SPI1 TX, SPI2 RX, SPI2 TX, SPI1 RX.

&par Directory contents

  - Device_Examples/OTGD_CDC_SPI/Source/apm32f4xx_int.c      Interrupt handlers
  - Device_Examples/OTGD_CDC_SPI/Source/main.c               USB CDC and SPI interrupt bridge
  - Device_Examples/OTGD_CDC_SPI/Source/usbd_cdc_vcp.c       USB CDC interface

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F427 Tiny Devices.
