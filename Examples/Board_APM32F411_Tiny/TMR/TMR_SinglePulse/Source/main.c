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
#include "stdio.h"

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
    TMR_ICConfig_T ICConfig;

    /* Enable peripheral clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA|RCM_AHB1_PERIPH_GPIOE);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_TMR1);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_11, GPIO_AF_TMR1);

    /* Configure TMR1 channel GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

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

    /* config TMR1 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 1;
    TMR_TimeBaseStruct.repetitionCounter = 0;

    /* maximum frequency value to trigger the TMR1 input is 120MHz/32767 = 3.662KHz */
    TMR_TimeBaseStruct.period = 32767;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    TMR_ConfigOCStructInit(&OCcongigStruct);

    /* Configure TMR1 channel 1 */
    OCcongigStruct.mode = TMR_OC_MODE_PWM2;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.idleState = TMR_OC_IDLE_STATE_SET;
    OCcongigStruct.outputNState = TMR_OC_NSTATE_DISABLE;
    OCcongigStruct.nPolarity = TMR_OC_NPOLARITY_HIGH;
    OCcongigStruct.nIdleState = TMR_OC_NIDLE_STATE_RESET;

    /* defines the delay value = 8191/(120MHz) = 68.25us */
    OCcongigStruct.pulse = 8191;
    TMR_ConfigOC1(TMR1, &OCcongigStruct);

    TMR_ConfigSinglePulseMode(TMR1, TMR_SPM_SINGLE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR1, TMR_SLAVE_MODE_TRIGGER);
    TMR_SelectInputTrigger(TMR1, TMR_TRIGGER_SOURCE_TI2FP2);

    /* Enable the specified TMR peripheral */
    TMR_Enable(TMR1);

    TMR_EnablePWMOutputs(TMR1);

    while(1)
    {
    }
}
