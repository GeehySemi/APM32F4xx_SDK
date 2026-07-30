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
#include "apm32f4xx_dci.h"
#include "Board.h"
#include "board_usart.h"
#include "board_delay.h"
#include "bsp_ov2640.h"
#include "bsp_dci.h"
#include "bsp_key.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/*!
 * @brief    OV2640 work mode
 */
typedef enum
{
    RGB565_MODE,    //!< OV2640 rgb565 work mode
    JPEG_MODE,      //!< OV2640 jpeg work mode
}OV2640_MODE;

/* Exported function prototypes *******************************************/

/** extern variables*/
extern OV2640_MODE g_ov2640Mode;

void OV2640_JpegDataCallback(void);
void OV2640_Rgb565DataCallback(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
