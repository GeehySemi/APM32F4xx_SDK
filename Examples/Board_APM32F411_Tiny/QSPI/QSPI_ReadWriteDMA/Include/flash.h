/*!
 * @file        flash.h
 *
 * @brief       This file contains the headers of the interrupt handlers
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

/* Define to prevent recursive inclusion */
#ifndef _FLASH_H
#define _FLASH_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include <stdint.h>

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/**
 * @brief   Flash command enumeration
 */
enum
{
    FLASH_CMD_READ_DATA                 = 0X03,
    FLASH_CMD_WRITE_DISABLE             = 0X04,
    FLASH_CMD_WRITE_ENABLE              = 0X06,
    FLASH_CMD_READ_SR1                  = 0X05,
    FLASH_CMD_READ_SR2                  = 0X35,
    FLASH_CMD_WRITE_SR1                 = 0X01,
    FLASH_CMD_WRITE_SR2                 = 0X31,
    FLASH_CMD_FAST_READ_DATA            = 0X0B,
    FLASH_CMD_FAST_READ_DUAL            = 0X3B,
    FLASH_CMD_FASH_READ_DUAD            = 0X6B,
    FLASH_CMD_PAGE_PROGRAM              = 0X02,
    FLASH_CMD_PAGE_PROGRAM_QUAD         = 0X32,
    FLASH_CMD_BLOCK_ERASE               = 0XD8,
    FLASH_CMD_SECTOR_ERASE              = 0X20,
    FLASH_CMD_CHIP_ERASE                = 0XC7,
    FLASH_CMD_POWER_DOWN                = 0XB9,
    FLASH_CMD_RELEASE_POWER_DOWN        = 0XAB,
    FLASH_CMD_DEVICE_ID                 = 0XAB,
    FLASH_CMD_MAUFACT_DEVICE_ID         = 0X90,
    FLASH_CMD_MAUFACT_DEVICE_ID_QAUD    = 0X94,
};

/* Exported function prototypes *******************************************/

void FLASH_Init(void);
uint16_t FLASH_ReadID(void);
void Flash_Read(uint32_t addr, uint8_t* rBuf,uint16_t rLen);
void Flash_Write(uint32_t addr, uint8_t* wBuf,uint16_t wLen);
uint8_t FLASH_ReadWriteTestDMA(void);
void FLASH_WriteEnable(void);

#ifdef __cplusplus
}
#endif

#endif /*_FLASH_H */

