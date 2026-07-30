/*!
 * @file        board_lcdFont.h
 *
 * @brief       Font for LCD
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

#ifndef __BOARD_LCD_FONT_H
#define __BOARD_LCD_FONT_H

/* Define to prevent recursive inclusion */
#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "apm32f4xx.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/
typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t *pTable;
} LCD_FontConfig_T;

/* Exported variables *****************************************************/
extern LCD_FontConfig_T g_font;

/* Exported function prototypes *******************************************/

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_LCD_FONT_H */
