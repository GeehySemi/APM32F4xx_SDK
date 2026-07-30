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
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_fmc.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* Config system clock */
void ConfigSysClock(void);
/* Delay */
void Delay(uint32_t count);

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    SystemClockConfig();
    GPIO_Config_T GPIO_ConfigStruct;

    /* Set system clock to 168MHz(APM32F40X) or 100MHz(APM32F411)*/
    ConfigSysClock();
    RCM_EnableCSS();

    BOARD_LEDInit(LED2);

    /* Enable GPIOA Periph Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* GPIOA initialization */
    GPIO_ConfigStructInit(&GPIO_ConfigStruct);
    GPIO_ConfigStruct.pin = GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_MCO);

    /* MCO Out System Clock divided by 5 */
    RCM_ConfigMCO1(RCM_MCO1_SEL_PLLCLK,RCM_MCO1_DIV_5);

    while (1)
    {
        Delay(0x7FFFFF);
        BOARD_LEDToggle(LED2);
    }
}

/*!
 * @brief     Configures the System clock frequency, HCLK, PCLK2 and PCLK1
 *
 * @param     None
 *
 * @retval    None
 */
void ConfigSysClock(void)
{
    RCM_Reset();

    /* Open HSE 8MHz */
    RCM_ConfigHSE(RCM_HSE_OPEN);

    if(RCM_WaitHSEReady() == SUCCESS)
    {
        FMC_EnablePrefetchBuffer();
        FMC_ConfigLatency(FMC_LTNCY_4);

        RCM_ConfigAHB(RCM_AHB_DIV_1);
        RCM_ConfigAPB2(RCM_APB_DIV_2);
        RCM_ConfigAPB1(RCM_APB_DIV_4);

        RCM_ConfigPLL1(RCM_PLLSEL_HSE, 8,336,RCM_PLL_SYS_DIV_2,7);
        RCM_EnablePLL1();

        /* Wait for PLL1 Ready */
        while(RCM_ReadStatusFlag(RCM_FLAG_PLL1RDY) == RESET);

        /* Select PLL1 as System Clock */
        RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);
        while(RCM_ReadSYSCLKSource() != RCM_SYSCLK_SEL_PLL)
        {
        }
    }
    else
    {
        while(1);
    }
}

/*!
 * @brief     Delay
 *
 * @param     count:  delay count
 *
 * @retval    None
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;
    while(delay--);
}

