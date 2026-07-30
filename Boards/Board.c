/*!
 * @file       Board.c
 *
 * @brief      This file provides firmware functions to manage Leds and push-buttons
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

/* Includes */
#include "Board.h"

#if defined (APM32F407_MINI)
#include "Board_APM32F407_Mini\src\board_apm32f407_mini.c"

#elif defined (APM32F407_EVAL)
#include "Board_APM32F407_Eval\src\board_apm32f407_eval.c"

#elif defined (APM32F407_TINY)
#include "Board_APM32F407_Tiny\src\board_apm32f407_tiny.c"

#elif defined (APM32F411_TINY)
#include "Board_APM32F411_Tiny\src\board_apm32f411_tiny.c"

#elif defined (APM32F427_TINY)
#include "Board_APM32F427_Tiny\src\board_apm32f427_tiny.c"

#else
#error "Please select first the APM32 board to be used (in board.c)"
#endif

/** @addtogroup Board
  @{

*/

/**@} end of group Board */
