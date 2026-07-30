/*!
 * @file        main.h
 *
 * @brief       Header for main.c module
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

/* Includes ***************************************************************/
#include "apm32f4xx.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

typedef enum
{
    USER_APP_OK,
    USER_APP_ERR_FATFS,
    USER_APP_ERR_DISK_WP,
    USER_APP_ERR_FSIZE_OVER,
    USER_APP_ERR_FMC_ERASE,
    USER_APP_ERR_FMC_PG,
    USER_APP_ERR_FMC_WP,
    USER_APP_ERR_DISK_DISCONNECTED,
} USER_APP_ERR_T;

typedef enum
{
    USER_OK,
    USER_WAITING,
    USER_ERROR
} USER_STATE_T;

typedef enum
{
    USER_APP_INIT,
    USER_APP_RUN,
    USER_APP_JUMP,
} USER_APP_STATE_T;

/* Exported function prototypes *******************************************/

extern void USER_ErrorHandler(uint8_t errCode);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
