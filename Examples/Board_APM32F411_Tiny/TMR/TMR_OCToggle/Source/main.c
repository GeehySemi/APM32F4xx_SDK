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

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

uint16_t CC1_Val = (30000000/5000);
uint16_t CC2_Val = (30000000/10000);
uint16_t CC3_Val = (30000000/50000);
uint16_t CC4_Val = (30000000/100000);

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

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR4);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);

    /* TMR4_CH1 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF_TMR4);
    /* TMR4_CH2 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_7, GPIO_AF_TMR4);
    /* TMR4_CH3 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_8, GPIO_AF_TMR4);
    /* TMR4_CH4 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_9, GPIO_AF_TMR4);

    /* Config TMR4 GPIO for output */
    GPIO_ConfigStruct.pin = GPIO_PIN_6| GPIO_PIN_7| GPIO_PIN_8| GPIO_PIN_9;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    /* config TMR4 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 0;
    TMR_TimeBaseStruct.period = 0xFFFF;
    TMR_ConfigTimeBase(TMR4, &TMR_TimeBaseStruct);

    /* Configure channel */
    TMR_ConfigOCStructInit(&OCcongigStruct);

    OCcongigStruct.mode = TMR_OC_MODE_TOGGLE;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_LOW;

    /* Configure channel1 */
    OCcongigStruct.pulse = CC1_Val;
    TMR_ConfigOC1(TMR4, &OCcongigStruct);
    TMR_ConfigOC1Preload(TMR4, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel2 */
    OCcongigStruct.pulse = CC2_Val;
    TMR_ConfigOC2(TMR4, &OCcongigStruct);
    TMR_ConfigOC2Preload(TMR4, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel3 */
    OCcongigStruct.pulse = CC3_Val;
    TMR_ConfigOC3(TMR4, &OCcongigStruct);
    TMR_ConfigOC3Preload(TMR4, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel4 */
    OCcongigStruct.pulse = CC4_Val;
    TMR_ConfigOC4(TMR4, &OCcongigStruct);
    TMR_ConfigOC4Preload(TMR4, TMR_OC_PRELOAD_DISABLE);

    /* TMR4 Commutation Interrupt */
    NVIC_EnableIRQ(TMR4_IRQn);

    /* start generate four different signals */
    TMR_Enable(TMR4);

    TMR_EnableInterrupt(TMR4,TMR_INT_CC1|TMR_INT_CC2|TMR_INT_CC3|TMR_INT_CC4);

    while(1)
    {
    }
}

