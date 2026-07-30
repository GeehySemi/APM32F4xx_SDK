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

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    BOARD_LEDInit(LED2);

    /* TMR2_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_0, GPIO_AF_TMR2);
    /* TMR2_CH2 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_1, GPIO_AF_TMR2);
    /* TMR2_CH3 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_TMR2);
    /* TMR2_CH4 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_TMR2);

    /* Config TMR2 GPIO for output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* config TMR2 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 119;
    TMR_TimeBaseStruct.period = 999;
    TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);

    /* Configure channel1 */
    TMR_ConfigOCStructInit(&OCcongigStruct);

    OCcongigStruct.mode = TMR_OC_MODE_ACTIVE;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;

    /* TMR2 CH1 delay = 800/1MHz = 800us */
    OCcongigStruct.pulse = 800;
    TMR_ConfigOC1(TMR2, &OCcongigStruct);
    TMR_ConfigOC1Preload(TMR2, TMR_OC_PRELOAD_DISABLE);

    /* TMR2 CH2 delay = 600/1MHz = 600us */
    OCcongigStruct.pulse = 600;
    TMR_ConfigOC2(TMR2, &OCcongigStruct);
    TMR_ConfigOC2Preload(TMR2, TMR_OC_PRELOAD_DISABLE);

    /* TMR2 CH3 delay = 400/1MHz = 400us */
    OCcongigStruct.pulse = 400;
    TMR_ConfigOC3(TMR2, &OCcongigStruct);
    TMR_ConfigOC3Preload(TMR2, TMR_OC_PRELOAD_DISABLE);

    /* TMR2 CH4 delay = 200/1MHz = 200us */
    OCcongigStruct.pulse = 200;
    TMR_ConfigOC4(TMR2, &OCcongigStruct);
    TMR_ConfigOC4Preload(TMR2, TMR_OC_PRELOAD_DISABLE);

    TMR_DisableAutoReload(TMR2);

    /* PE6 generate falling edge */
    BOARD_LEDOn(LED2);

    /* start generate four different signals */
    TMR_Enable(TMR2);

    while(1)
    {
    }
}
