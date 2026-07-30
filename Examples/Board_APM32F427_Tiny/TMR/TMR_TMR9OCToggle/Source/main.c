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
    SystemCoreClockUpdate();

    GPIO_Config_T GPIO_ConfigStruct;
    TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR9);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* TMR9_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_TMR9);
    /* TMR9_CH2 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_TMR9);

    /* Config TMR9 GPIO for output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* config TMR9 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 0;
    TMR_TimeBaseStruct.period = 0xFFFF;
    TMR_ConfigTimeBase(TMR9, &TMR_TimeBaseStruct);

    /* Configure channel1 */
    TMR_ConfigOCStructInit(&OCcongigStruct);

    OCcongigStruct.mode = TMR_OC_MODE_TOGGLE;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;

    OCcongigStruct.pulse = (SystemCoreClock/10000);
    TMR_ConfigOC1(TMR9, &OCcongigStruct);
    TMR_ConfigOC1Preload(TMR9, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel2 */
    OCcongigStruct.pulse = (SystemCoreClock/100000);
    TMR_ConfigOC2(TMR9, &OCcongigStruct);
    TMR_ConfigOC2Preload(TMR9, TMR_OC_PRELOAD_DISABLE);

    /* Timer9 Commutation Interrupt */
    TMR_EnableInterrupt(TMR9,TMR_INT_CC1|TMR_INT_CC2);
    NVIC_EnableIRQ(TMR1_BRK_TMR9_IRQn);

    TMR_Enable(TMR9);

    while(1)
    {
    }
}

/*!
 * @brief       TMR9 Handler
 *
 * @param       None
 *
 * @retval      None
 */
void TMR1_BRK_TMR9_IRQHandler(void)
{
    uint16_t capValue = 0;

    /* TMR9_CH1 toggling with frequency = 5KHz */
    if (TMR_ReadIntFlag(TMR9, TMR_INT_CC1))
    {
        /* Clear the TMR9 CC1 pending bit */
        TMR_ClearIntFlag(TMR9, TMR_INT_CC1);
        capValue = TMR_ReadCaputer1(TMR9);
        TMR_ConfigCompare1(TMR9, capValue + (SystemCoreClock/10000));
    }

    /* TMR9_CH2 toggling with frequency = 50KHz */
    if (TMR_ReadIntFlag(TMR9, TMR_INT_CC2))
    {
        /* Clear the TMR9 CC1 pending bit */
        TMR_ClearIntFlag(TMR9, TMR_INT_CC2);
        capValue = TMR_ReadCaputer2(TMR9);
        TMR_ConfigCompare2(TMR9, capValue + (SystemCoreClock/100000));
    }
}
