/*!
 * @file        qspi.h
 *
 * @brief       This file contains the headers of the interrupt handlers
 *
 * @version     V1.0.0
 *
 * @date        2025-02-15
 *
 * @attention
 *
 *  Copyright (C) 2021-2023 Geehy Semiconductor
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
#ifndef __QSPI_H
#define __QSPI_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include <stdint.h>
#include "apm32f4xx_qspi.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/**
 * @brief   QSPI Frame format
 */
enum
{
    QSPI_STANDARD,
    QSPI_DUAL,
    QSPI_QUAD
};

/**
 * @brief   Read and write parameter
 */
typedef struct
{
    uint8_t     *dataBuf;
    uint16_t    dataLen;

    uint8_t     addrLen;
    uint32_t    addr;

    uint8_t     instLen;
    uint32_t    instruction;

    uint8_t     waitCycle;

    QSPI_INST_ADDR_TYPE_T instAddrType;
}QSPI_ReadWriteParam_T;

/* Exported function prototypes *******************************************/

void QSPI_Init(void);

uint8_t QSPI_Std_WriteReadByte(uint8_t data);
void QSPI_Std_Read(uint8_t *rBuf, uint16_t rLen);
void QSPI_Std_Write(uint8_t *wBuf, uint16_t wLen);

void QSPI_Quad_Read(QSPI_ReadWriteParam_T *rParam);
void QSPI_Quad_Write(QSPI_ReadWriteParam_T *wParam);

void QSPI_Dual_Read(QSPI_ReadWriteParam_T *rParam);

void QSPI_ChipSelect(uint8_t select);

void QSPI_Eeprom_Read(uint32_t cmd, uint8_t cmdSize, uint32_t addr, uint8_t addrSize, uint8_t *rBuf, uint32_t rLen);

#ifdef __cplusplus
}
#endif

#endif /*__QSPI_H */
