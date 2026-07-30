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

This example describes how to use the usb host to enum a U disk.
And use it to upgrade application firmware.

Running Steps:
- Load the binary image of the user file that needs to be upgraded into the
  root directory of the USB drive, and rename it to image.bin. The offset
  address of the upgrade file must be set correctly (0x08010000).

- Download "MSC_FWUpgrade" into the internal flash memory.

- Insert the U disk into the USB FS connector.

- Press the KEY2 button and reset system.

- After waiting for the UART1 prompt to release the KEY2 button and release
KEY1, the program will enter USB upgrade mode.

- After upgrade is completed, the program will jump to application.

Flash space:
0x08000000 - 0x0800FFFF : USB FW upgrade bootloader
0x08010000 - 0x080FFFFF : USB FW upgrade application

note:
- The starting address of the user application program can be changed by "APPLICATION_ADDRESS"
in the bsp_flash.h file.

- The image download and upload information can be changed in the usb_iap.h file.
    - IAP_UPLOAD_FILENAME   : The name of binary file to be upload.
    - IAP_DOWNLOAD_FILENAME : The name of binary file to be download.
    - IAP_FILE_PATH         : The path of image file.
    - IAP_DISK_PATH         : The path of U disk.
    - IAP_BUFFER_SIZE       : Buffer for IAP download. In this project is fixed to 16KB.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

    - VCC5V            ----------> VBUS
    - OTG_FS_DM(PA11)  ----------> USB_DN
    - OTG_FS_DP(PA12)  ----------> USB_DP
    - GND              ----------> GND

&par Directory contents

  - Host_Examples/OTGH_MSC_FWUpgrade/Source/apm32f4xx_int.c          Interrupt handlers
  - Host_Examples/OTGH_MSC_FWUpgrade/Source/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.40
  - EWARM V8.50.5.26295
  - Eclipse V2022-06（4.24.0）

&par Hardware and Software environment

  - This example runs on APM32F407 Mini Devices.
