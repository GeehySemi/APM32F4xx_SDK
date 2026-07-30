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

#define PERIOD1 119
#define PERIOD2 29
#define PERIOD3 14

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
    TMR_ICConfig_T ICConfig;

    /* Enable peripheral clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2 | RCM_APB1_PERIPH_TMR3);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOE);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_TMR1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_0, GPIO_AF_TMR2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_TMR3);

    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_11, GPIO_AF_TMR1);

    /* Configure TMR1, TMR2, TMR3 channel 1 GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_0 | GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_4;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_11;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_DOWN;
    GPIO_Config(GPIOE, &GPIO_ConfigStruct);

    /* TMR1 Input Capture mode */
    ICConfig.channel = TMR_CHANNEL_2;
    ICConfig.polarity = TMR_IC_POLARITY_RISING;
    ICConfig.selection = TMR_IC_SELECTION_DIRECT_TI;
    ICConfig.prescaler = TMR_IC_PSC_1;
    ICConfig.filter = 0x00;
    TMR_ConfigIC(TMR1, &ICConfig);

    /* config TMR1,TMR2,TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 0;
    TMR_TimeBaseStruct.repetitionCounter = 0;

    TMR_TimeBaseStruct.period = PERIOD1;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = PERIOD2;
    TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = PERIOD3;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    TMR_ConfigOCStructInit(&OCcongigStruct);

    /* Configure TMR1, TMR2, TMR3 channel 1 */
    OCcongigStruct.mode = TMR_OC_MODE_TOGGLE;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.idleState = TMR_OC_IDLE_STATE_SET;
    OCcongigStruct.outputNState = TMR_OC_NSTATE_DISABLE;
    OCcongigStruct.nPolarity = TMR_OC_NPOLARITY_HIGH;
    OCcongigStruct.nIdleState = TMR_OC_NIDLE_STATE_RESET;

    TMR_ConfigOC1(TMR1, &OCcongigStruct);
    TMR_ConfigOC1(TMR2, &OCcongigStruct);
    TMR_ConfigOC1(TMR3, &OCcongigStruct);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectInputTrigger(TMR1, TMR_TRIGGER_SOURCE_TI2FP2);
    TMR_SelectSlaveMode(TMR1, TMR_SLAVE_MODE_GATED);

    /* Enable TMR1 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR1);
    TMR_SelectOutputTrigger(TMR1, TMR_TRGO_SOURCE_ENABLE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectInputTrigger(TMR2, TMR_TRIGGER_SOURCE_ITR0);
    TMR_SelectSlaveMode(TMR2, TMR_SLAVE_MODE_GATED);

    /* Enable TMR2 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR2);
    TMR_SelectOutputTrigger(TMR2, TMR_TRGO_SOURCE_ENABLE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectInputTrigger(TMR3, TMR_TRIGGER_SOURCE_ITR1);
    TMR_SelectSlaveMode(TMR3, TMR_SLAVE_MODE_GATED);

    /* Enable the specified TMR peripheral */
    TMR_Enable(TMR1);
    TMR_Enable(TMR2);
    TMR_Enable(TMR3);

    TMR_EnablePWMOutputs(TMR1);

    while(1)
    {
    }
}
