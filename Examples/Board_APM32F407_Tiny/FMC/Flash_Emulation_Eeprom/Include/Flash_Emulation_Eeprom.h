/*!
 * @file        Flash_Emulation_Eeprom.h
 *
 * @brief       Header for Flash_Emulation_Eeprom.c module
 *
 * @version     V1.0.4
 *
 * @date        2025-02-15
 *
 * @attention
 *
 *  Copyright (C) 2021-2025 Geehy Semiconductor
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

/* Define to prevent recursive inclusion */
#ifndef __FLASH_EMULATION_EEPROM_H
#define __FLASH_EMULATION_EEPROM_H

#ifdef __cplusplus
  extern "C" {
#endif

 /* Includes ***************************************************************/
#include "apm32f4xx.h"

/*
 +--------------------------------------------------------------+
 |                             Flash                            |
 +-------------------------------+------------------------------+
 |  Code  |      User Para area      |           Code           |
 +--------+--------+--------+--------+--------+-----------------+
 |  16KB  |  16KB  |  16KB  |  16KB  |  64KB  | 128KB  | 128KB  |
 | sector | sector | sector | sector | sector | sector | sector |
 |   0    |    1   |   2    |   3    |    4   |   N    | N + 1  |
 |        |        |        |        |        |        |        |
 +--------+--------+--------+--------+--------+-----------------+
*/

/* Exported macro *********************************************************/

/* flash sector satrt address */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000)   /* 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000)   /* 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000)   /* 16 Kbytes */

/* flash sector size */
#define FLASH_SECTOR_SIZE                ((uint32_t)(1024 * 16))

/* flash emulation eeprom total size. This value must be a multiple of 16KB */
#define FLASH_EE_TOTAL_SIZE              ((uint32_t)(1024 * 16 * 2))

/* flash emulation eeprom sector start address, it's must be sector aligned */
#define FLASH_EE_START_ADDR              ADDR_FLASH_SECTOR_1

/* flash emulation eeprom sector start address */
#define FLASH_EE_END_ADDR                (ADDR_FLASH_SECTOR_1 + FLASH_EE_TOTAL_SIZE)

/* Exported typedef *******************************************************/

/* Exported function prototypes *******************************************/

/* Read the specified length of data from the specified address */
void Flash_EE_Read(uint32_t readAddr, uint8_t* pData, uint32_t len);
/* Write the specified length of data from the specified address */
void Flash_EE_Write(uint32_t WriteAddr, uint8_t* pData, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /*__FLASH_EMULATION_EEPROM_H */
