/*!
 * @file        bsp_flash.h
 *
 * @brief       Header for bsp_flash.c module
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
#ifndef __IAP_FLASH_H
#define __IAP_FLASH_H

/* Includes ***************************************************************/
#include "main.h"

/* Exported macro *********************************************************/

#define FLASH_BASE                  (uint32_t)0x08000000

#define USER_APP1_ADDRESS           (uint32_t)0x08006000
#define USER_APP1_START_SECTOR       FMC_SECTOR_1
#define USER_APP1_END_SECTOR         FMC_SECTOR_2
/* 16 KBytes */
#define USER_APP1_END_ADDRESS       (uint32_t)0x08008000
#define USER_APP1_FLASH_SIZE        (USER_APP1_END_ADDRESS - USER_APP1_ADDRESS)

#define USER_APP2_ADDRESS           (uint32_t)0x08008000
#define USER_APP2_START_SECTOR      FMC_SECTOR_2
#define USER_APP2_END_SECTOR        FMC_SECTOR_3
/* 16 KBytes */
#define USER_APP2_END_ADDRESS       (uint32_t)0x0800A000
#define USER_APP2_FLASH_SIZE        (USER_APP2_END_ADDRESS - USER_APP2_ADDRESS)

#define USER_FLASH_SIZE             (USER_APP2_END_ADDRESS - USER_APP1_ADDRESS)

/* Exported typedef *******************************************************/

/*!
 * @brief    APP type define
 */
typedef enum
{
    APP1 = 0,   /*!< Application1 */
    APP2 = 1    /*!< Application2 */
} APP_TypeDef;

/*!
 * @brief    FMC operation type define
 */
typedef enum
{
    FMC_OP_SUCCESS, /*!< Flash operation success */
    FMC_OP_ERROR    /*!< Flash operation error */
} FMC_OP_TypeDef;

/* Exported function prototypes *******************************************/

/** function declaration*/
void FLASH_IAP_Init(void);
uint32_t FLASH_IAP_Erase(APP_TypeDef Application);
uint32_t FLASH_IAP_Write(__IO uint32_t* address, uint32_t* data, uint16_t length, APP_TypeDef Application);

#endif
