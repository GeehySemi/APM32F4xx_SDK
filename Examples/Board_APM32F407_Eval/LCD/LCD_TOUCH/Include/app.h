/*!
 * @file        app.h
 *
 * @brief       Header for app.c module
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
#ifndef _APP_H
#define _APP_H

/* Includes ***************************************************************/
#include "main.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/
/**
 * @brief Touch event
 */
typedef struct
{
    uint8_t touchDownStatus;
    uint8_t touchUpStatus;
} TOUCH_EVENT_T;

/**
 * @brief Touch button type
 */
typedef enum
{
    TOUCH_LED,
    TOUCH_REFRESH,
    TOUCH_HELP,
} TOUCH_BUTTON_T;

/* Exported variables *****************************************************/
extern TOUCH_EVENT_T touchLED;
extern TOUCH_EVENT_T touchRefresh;
extern TOUCH_EVENT_T touchHelp;

/* Exported function prototypes *******************************************/
void Touch_DisplayInitUI(void);
void Touch_EventHandle(void);

#endif /* _APP_H */
