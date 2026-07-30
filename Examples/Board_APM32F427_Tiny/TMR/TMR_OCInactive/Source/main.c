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

/* Private includes *******************************************************/
#include "apm32f4xx_tmr.h"

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

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
    SystemClockConfig();
    GPIO_Config_T GPIO_ConfigStruct;
    TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA|RCM_AHB1_PERIPH_GPIOC);

    /* TMR3_CH1 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_6, GPIO_AF_TMR3);
    /* TMR3_CH2 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_7, GPIO_AF_TMR3);
    /* TMR3_CH3 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_8, GPIO_AF_TMR3);
    /* TMR3_CH4 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_9, GPIO_AF_TMR3);

    /* Config TMR3 GPIO for output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_6| GPIO_PIN_7| GPIO_PIN_8| GPIO_PIN_9;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOC, &GPIO_ConfigStruct);

    /* Config GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3|GPIO_PIN_4;
    GPIO_ConfigStruct.mode = GPIO_MODE_OUT;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* config TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 119;
    TMR_TimeBaseStruct.period = 999;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    /* Configure channel1 */
    TMR_ConfigOCStructInit(&OCcongigStruct);

    OCcongigStruct.mode = TMR_OC_MODE_INACTIVE;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;

    /* TMR3 CH1 delay = 800/1MHz = 800us */
    OCcongigStruct.pulse = 800;
    TMR_ConfigOC1(TMR3, &OCcongigStruct);
    TMR_ConfigOC1Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH2 delay = 600/1MHz = 600us */
    OCcongigStruct.pulse = 600;
    TMR_ConfigOC2(TMR3, &OCcongigStruct);
    TMR_ConfigOC2Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH3 delay = 400/1MHz = 400us */
    OCcongigStruct.pulse = 400;
    TMR_ConfigOC3(TMR3, &OCcongigStruct);
    TMR_ConfigOC3Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH4 delay = 200/1MHz = 200us */
    OCcongigStruct.pulse = 200;
    TMR_ConfigOC4(TMR3, &OCcongigStruct);
    TMR_ConfigOC4Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    TMR_DisableAutoReload(TMR3);

    /* generate raising edge */
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_1, 1);
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_2, 1);
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_3, 1);
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_4, 1);

    /* TMR4 Commutation Interrupt */
    NVIC_EnableIRQ(TMR3_IRQn);

    /* start generate four different signals */
    TMR_Enable(TMR3);

    TMR_EnableInterrupt(TMR3,TMR_INT_CC1|TMR_INT_CC2|TMR_INT_CC3|TMR_INT_CC4);

    while(1)
    {
    }
}

/*!
 * @brief       TMR3 Handler
 *
 * @param       None
 *
 * @retval      None
 */
void TMR3_IRQHandler(void)
{
    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC1))
    {
        /* Clear the TMR3 CC1 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC1);

        /* delay 800us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_1, 0);
    }

    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC2))
    {
        /* Clear the TMR3 CC2 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC2);

        /* delay 600us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_2, 0);
    }

    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC3))
    {
        /* Clear the TMR3 CC3 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC3);

        /* delay 400us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_3, 0);
    }

    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC4))
    {
        /* Clear the TMR3 CC4 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC4);

        /* delay 200us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_4, 0);
    }
}
