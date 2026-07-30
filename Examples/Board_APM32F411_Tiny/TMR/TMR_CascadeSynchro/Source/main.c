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
#include "apm32f4xx_tmr.h"

/* Private includes *******************************************************/

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

    /* Enable peripheral clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2|RCM_APB1_PERIPH_TMR3|RCM_APB1_PERIPH_TMR4);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA |RCM_AHB1_PERIPH_GPIOB);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_0, GPIO_AF_TMR2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_TMR3);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF_TMR4);

    /* Configure TMR2, TMR3, TMR4 channel 1 GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_0;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    /* config TMR2,TMR3,TMR4 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 0;
    TMR_TimeBaseStruct.period = 127;
    TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = 3;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = 1;
    TMR_ConfigTimeBase(TMR4, &TMR_TimeBaseStruct);

    TMR_ConfigOCStructInit(&OCcongigStruct);

    /* Configure TMR2, TMR3, TMR4 channel 1 */
    OCcongigStruct.mode = TMR_OC_MODE_PWM1;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.pulse = 64;
    TMR_ConfigOC1(TMR2, &OCcongigStruct);

    OCcongigStruct.pulse = 2;
    TMR_ConfigOC1(TMR3, &OCcongigStruct);

    OCcongigStruct.pulse = 1;
    TMR_ConfigOC1(TMR4, &OCcongigStruct);

    /* Enable TMR2 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR2);
    TMR_SelectOutputTrigger(TMR2, TMR_TRGO_SOURCE_UPDATE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR3, TMR_SLAVE_MODE_GATED);
    TMR_SelectInputTrigger(TMR3, TMR_TRIGGER_SOURCE_ITR1);

    /* Enable TMR3 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR3);
    TMR_SelectOutputTrigger(TMR3, TMR_TRGO_SOURCE_UPDATE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR4, TMR_SLAVE_MODE_GATED);
    TMR_SelectInputTrigger(TMR4, TMR_TRIGGER_SOURCE_ITR2);

    /* Enable the specified TMR peripheral */
    TMR_Enable(TMR2);
    TMR_Enable(TMR3);
    TMR_Enable(TMR4);

    while(1)
    {
    }
}
