/*!
 * @file        main.c
 *
 * @brief       Main program body
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

/* Includes ***************************************************************/
#include "main.h"
#include "Board.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

__IO uint32_t JumpAddress;
volatile pFunction Jump_To_Application;

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    /* Initialize LED on APM32F4xx MINI Board */
    SystemClockConfig();
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /* Initialize UART mounted on APM32F4xx MINI Board */
    USART1_Init(115200);

    BOARD_LEDOn(LED2);
    BOARD_LEDOn(LED3);

    /* Configure the Flash to re-program the Flash */
    FLASH_IAP_Init();

    /* Display Select menu */
    Select_Menu();

    /* Jump to user application 1 */
    SendString(">> Jump to user application 1\r\n");

    Jump_to_App(APP1);

    while (1)
    {
    }
}

/*!
 * @brief       Delay
 *
 * @param       count:  delay count
 *
 * @retval      None
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;
    while(delay--);
}
