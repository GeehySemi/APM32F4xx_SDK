/*!
 * @file        low_power_modes.h
 *
 * @brief       Header for low_power_modes.c module
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
#ifndef __LPMODES_H
#define __LPMODES_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "main.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/* Exported function prototypes *******************************************/
void ConsumptionMeasure_SleepMode(void);
void ConsumptionMeasure_StopMode(void);
void ConsumptionMeasure_StandbyMode(void);
void ConsumptionMeasure_StandbyRTCMode(void);
void ConsumptionMeasure_StandbyRTCBKPSRAMMode(void);

#ifdef __cplusplus
}
#endif

#endif /* __LPMODES_H */
