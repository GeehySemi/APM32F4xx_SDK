/*!
 * @file        main.c
 *
 * @brief       Main program body
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

/* Includes ***************************************************************/
#include "main.h"
#include "Board.h"
#include "flash.h"
#include <string.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

enum
{
    PASSED,
    FAILED
};

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

void Delay(void);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    SystemClockConfig();
    volatile uint16_t flashID = 0;

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    FLASH_Init();

    while(1)
    {
        flashID = FLASH_ReadID();
        if(flashID != 0)
        {
            break;
        }
        Delay();
        BOARD_LEDToggle(LED2);
    }

    while(1)
    {
        if(FLASH_ReadWriteTest() == FAILED)
        {
            Delay();
            BOARD_LEDToggle(LED2);
        }
        else
        {
            break;
        }
    }

    while(1)
    {
        Delay();
        BOARD_LEDToggle(LED3);
    }

}

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    volatile uint32_t delay = 0xfffff * 5;

    while (delay--);
}
