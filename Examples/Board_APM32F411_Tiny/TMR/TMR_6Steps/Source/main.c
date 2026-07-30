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

uint8_t PWMStep;

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
    BOARD_DelayConfig();
    
    GPIO_Config_T GPIO_ConfigStruct;
    TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;
    TMR_BDTConfig_T BDTConfig;

   
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOE);

    /* TMR1_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_TMR1);
    /* TMR1_CH2 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_11, GPIO_AF_TMR1);
    /* TMR1_CH3 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_13, GPIO_AF_TMR1);
    /* TMR1_BKIN */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_TMR1);
    /* TMR1_CH1N */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_TMR1);
    /* TMR1_CH2N */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_14, GPIO_AF_TMR1);
    /* TMR1_CH3N */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_15, GPIO_AF_TMR1);

    /* Config TMR1 GPIO for complementary output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_7| GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_DOWN;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_12 | GPIO_PIN_14| GPIO_PIN_15;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_11 | GPIO_PIN_13;
    GPIO_Config(GPIOE, &GPIO_ConfigStruct);

    /* config TMR1 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 0;
    TMR_TimeBaseStruct.period = 2047;
    TMR_TimeBaseStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    /* Configure channel */
    OCcongigStruct.mode = TMR_OC_MODE_TMRING;
    OCcongigStruct.pulse = 1023;
    OCcongigStruct.idleState = TMR_OC_IDLE_STATE_SET;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.nIdleState = TMR_OC_NIDLE_STATE_SET;
    OCcongigStruct.outputNState = TMR_OC_NSTATE_ENABLE;
    OCcongigStruct.nPolarity = TMR_OC_NPOLARITY_HIGH;

    TMR_ConfigOC1(TMR1, &OCcongigStruct);

    OCcongigStruct.pulse = 511;
    TMR_ConfigOC2(TMR1, &OCcongigStruct);

    OCcongigStruct.pulse = 255;
    TMR_ConfigOC3(TMR1, &OCcongigStruct);
    
    /* Configures the Break feature, dead time, Lock level, the IMOS */
    BDTConfig.RMOS = TMR_RMOS_STATE_ENABLE;
    BDTConfig.IMOS = TMR_IMOS_STATE_ENABLE;
    BDTConfig.lockLevel = TMR_LOCK_LEVEL_OFF;
    BDTConfig.deadTime = 0x01;
    BDTConfig.BRKState = TMR_BRK_STATE_ENABLE;
    BDTConfig.BRKPolarity = TMR_BRK_POLARITY_HIGH;
    BDTConfig.automaticOutput = TMR_AUTOMATIC_OUTPUT_ENABLE;
    TMR_ConfigBDT(TMR1, &BDTConfig);

    /* Timer1 Commutation Interrupt */
    TMR_EnableInterrupt(TMR1,TMR_INT_COM);
    NVIC_EnableIRQ(TMR1_TRG_COM_TMR11_IRQn);

    /* Enable Capture Compare Preload Control */
    TMR_EnableCCPreload(TMR1);

    TMR_Enable(TMR1);
    TMR_EnablePWMOutputs(TMR1);

    while(1)
    {
        /* Generate the TMR1 Commutation event by software every 1 second */
        BOARD_DelayMs(100);
        TMR_GenerateEvent(TMR1, TMR_EVENT_COM);
    }
}

/*!
 * @brief       TMR1 Trigger and Commutation IRQ Handler
 *
 * @param       None
 *
 * @retval      None
 */
void TMR1_TRG_COM_TMR11_IRQHandler(void)
{
    /* Clear the TMR1 COM pending bit */
    TMR_ClearIntFlag(TMR1, TMR_INT_COM);

    PWMStep++;

    switch(PWMStep)
    {
        case 1:
            /* configuration: Channel1 and Channel2 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_EnableCCxChannel(TMR1,TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_2);
            TMR_EnableCCxNChannel(TMR1,TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_3);
        break;

        case 2:
            /* configuration: Channel2 and Channel3 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_2);
            TMR_EnableCCxNChannel(TMR1,TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_EnableCCxChannel(TMR1,TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_3);
        break;

        case 3:
            /* configuration: Channel3 and Channel1 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_1);
            TMR_EnableCCxNChannel(TMR1,TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_EnableCCxChannel(TMR1,TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_3);
        break;

        case 4:
            /* configuration: Channel1 and Channel2 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_1);
            TMR_EnableCCxNChannel(TMR1,TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_EnableCCxChannel(TMR1,TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_3);
        break;

        case 5:
            /* configuration: Channel2 and Channel3 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_EnableCCxChannel(TMR1,TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_3);
            TMR_EnableCCxNChannel(TMR1,TMR_CHANNEL_3);
        break;

        case 6:
            /* configuration: Channel3 and Channel1 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_EnableCCxChannel(TMR1,TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1,TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1,TMR_CHANNEL_3);
            TMR_EnableCCxNChannel(TMR1,TMR_CHANNEL_3);

            PWMStep = 0;
        break;

        default:
            PWMStep = 0;
        break;
    }
}
