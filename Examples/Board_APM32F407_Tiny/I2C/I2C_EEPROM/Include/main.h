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

/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include "apm32f4xx.h"
#include "apm32f4XX_i2c.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup I2C_EEPROM_interrupt
  @{
  */

/** @defgroup I2C_EEPROM_interrupt_Functions Functions
  @{
  */
void I2C1_Isr(void);
void I2C2_Isr(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/**@} end of group I2C_EEPROM_interrupt_Functions */
/**@} end of group I2C_EEPROM_interrupt */
/**@} end of group Examples */
