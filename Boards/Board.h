/*!
 * @file       Board.h
 *
 * @brief      Header file for Board
 *
 * @version    V1.0.3
 *
 * @date       2025-02-15
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
#ifndef BOARD_H
#define BOARD_H

/* Includes */
#if defined (APM32F407_MINI)
#include "Board_APM32F407_Mini\inc\board_apm32f407_mini.h"

#elif defined (APM32F407_EVAL)
#include "Board_APM32F407_Eval\inc\board_apm32f407_eval.h"

#elif defined (APM32F407_TINY)
#include "Board_APM32F407_Tiny\inc\board_apm32f407_tiny.h"

#elif defined (APM32F411_TINY)
#include "Board_APM32F411_Tiny\inc\board_apm32f411_tiny.h"

#elif defined (APM32F427_TINY)
#include "Board_APM32F427_Tiny\inc\board_apm32f427_tiny.h"

#else
#error "Please select first the APM32  board to be used (in board.h)"
#endif

/** @addtogroup Board
  @{
*/

#endif

/**@} end of group Board */
