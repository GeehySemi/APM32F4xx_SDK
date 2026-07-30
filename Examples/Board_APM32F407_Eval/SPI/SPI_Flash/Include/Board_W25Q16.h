/**
 * @file        Board_W25Q16.h
 *
 * @brief       This file provides firmware functions to manage w25q64.
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
#ifndef __BOARD_W25Q16_H
#define __BOARD_W25Q16_H

/* Includes ***************************************************************/
#include "apm32f4xx.h"
#include <stdio.h>

/* Exported macro *********************************************************/
/** W25Q16 */
#define FLASH_ID                          0xEF4015   //W25Q16
#define FLASH_PAGE_SIZE                   256

/** W25Q16 CMD */
#define W25Q16_WriteEnable                0x06
#define W25Q16_WriteDisable               0x04
#define W25Q16_ReadStatusReg              0x05
#define W25Q16_WriteStatusReg             0x01
#define W25Q16_ReadData                   0x03
#define W25Q16_FastReadData               0x0B
#define W25Q16_FastReadDual               0x3B
#define W25Q16_PageProgram                0x02
#define W25Q16_BlockErase                 0xD8
#define W25Q16_SectorErase                0x20
#define W25Q16_ChipErase                  0xC7
#define W25Q16_PowerDown                  0xB9
#define W25Q16_ReleasePowerDown           0xAB
#define W25Q16_DeviceID                   0xAB
#define W25Q16_ManufactDeviceID           0x90
#define W25Q16_JedecDeviceID              0x9F
#define WIP_Flag                          0x01
#define Dummy_Byte                        0xFF

/** FLASH CS */
#define SPI_FLASH_CS_LOW()      {GPIO_WriteBitValue(GPIOF, GPIO_PIN_5, BIT_RESET);}
#define SPI_FLASH_CS_HIGH()     {GPIO_WriteBitValue(GPIOF, GPIO_PIN_5, BIT_SET);}

/* Exported typedef *******************************************************/

/* Exported variables *****************************************************/

/* Exported function prototypes *******************************************/

void FalshEraseSector(uint32_t SectorAddr);
void FlashIntoPowerDown(void);
uint8_t FlashSendByte(uint8_t byte);
uint8_t FlashReadByte(void);
void SPI_Init(void);
void FlashWritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumToWrite);
void FlashWriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumToWrite);
void FlashReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumToRead);
uint32_t ReadFlashID(void);
uint32_t ReadFlashDeviceID(void);
void EnableFlashWrite(void);
void WaitFlashWriteEnd(void);

#endif /* __BOARD_W25Q16_H */
