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
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_fmc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_misc.h"

#include "iap_usart.h"
#include "iap_flash.h"
#include "iap_common.h"
#include "iap_ymodem.h"
#include "iap_menu.h"

/* Exported macro *********************************************************/

#define Jump_to_APP1()  FMC_Lock();\
                        JumpAddress = *(__IO uint32_t *) (USER_APP1_ADDRESS + 4);\
                        Jump_To_Application = (pFunction) JumpAddress;\
                        __set_MSP(*(__IO uint32_t *) USER_APP1_ADDRESS);\
                        Jump_To_Application();

#define Jump_to_APP2()  FMC_Lock();\
                        JumpAddress = *(__IO uint32_t *) (USER_APP2_ADDRESS + 4);\
                        Jump_To_Application = (pFunction) JumpAddress;\
                        __set_MSP(*(__IO uint32_t *) USER_APP2_ADDRESS);\
                        Jump_To_Application();

#define Jump_to_App(Application)    if(Application == APP1){\
                                    Jump_to_APP1();}\
                                    else{\
                                    Jump_to_APP2();}

/* Exported typedef *******************************************************/

/*!
 * @brief    function hook
 */
typedef  void (*pFunction)(void);

/** extern variables*/
extern volatile pFunction Jump_To_Application;
extern __IO uint32_t JumpAddress;

/* Exported function prototypes *******************************************/

/** function declaration*/
void Delay(uint32_t count);
void SetJumpAddress(uint32_t Application);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
