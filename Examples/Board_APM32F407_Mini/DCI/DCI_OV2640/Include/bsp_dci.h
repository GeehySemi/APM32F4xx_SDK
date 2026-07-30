/*!
 * @file        bsp_dci.h
 *
 * @brief       Header for bsp_dci.c module
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
#ifndef _BSP_DCI_H
#define _BSP_DCI_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "Board.h"
#include "apm32f4xx_dci.h"

/* Exported macro *********************************************************/

#define DCI_DATA_REG_ADDRESS    (uint32_t) &DCI->DATA

/** DCI GPIO*/
#define DCI_HSYNC_PIN               GPIO_PIN_4
#define DCI_PIXCLK_PIN              GPIO_PIN_6
#define DCI_VSYNC_PIN               GPIO_PIN_7
#define DCI_D0_PIN                  GPIO_PIN_6
#define DCI_D1_PIN                  GPIO_PIN_7
#define DCI_D2_PIN                  GPIO_PIN_8
#define DCI_D3_PIN                  GPIO_PIN_9
#define DCI_D4_PIN                  GPIO_PIN_11
#define DCI_D5_PIN                  GPIO_PIN_6
#define DCI_D6_PIN                  GPIO_PIN_8
#define DCI_D7_PIN                  GPIO_PIN_9

/* Exported typedef *******************************************************/

/* Exported function prototypes *******************************************/
void DCI_Init(void);
void DCI_DMAInit(uint32_t memoryBaseAddr, uint32_t bufferSize, DMA_MEMORY_DATA_SIZE_T memoryDataSize, DMA_MEMORY_INC_T memoryInc,DMA_MEMORYBURST_T DMA_MemoryBurst);
void DCI_Start(void);
void DCI_Stop(void);

#ifdef __cplusplus
}
#endif

#endif
