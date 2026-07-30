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

#define DIV 119

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/** DMA Init */
void DMA_Init(uint32_t* Buf);

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
    uint16_t ConfigBuf[3] = {800, 400, 200};

    GPIO_Config_T GPIO_ConfigStruct;
    TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR8);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOC);

    /* TMR8_CH1 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_6, GPIO_AF_TMR8);
    /* TMR8_CH1N */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_5, GPIO_AF_TMR8);

    /* Config TMR8 GPIO for complementary output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_6;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_Config(GPIOC, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_5;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    DMA_Init((uint32_t*)ConfigBuf);

    /* config TMR8 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = DIV;
    TMR_TimeBaseStruct.period = 999;
    TMR_TimeBaseStruct.repetitionCounter = 6;
    TMR_ConfigTimeBase(TMR8, &TMR_TimeBaseStruct);

    /* Configure channel */
    OCcongigStruct.mode = TMR_OC_MODE_PWM1;
    OCcongigStruct.pulse = 500;
    OCcongigStruct.idleState = TMR_OC_IDLE_STATE_RESET;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.nIdleState = TMR_OC_NIDLE_STATE_RESET;
    OCcongigStruct.outputNState = TMR_OC_NSTATE_ENABLE;
    OCcongigStruct.nPolarity = TMR_OC_NPOLARITY_HIGH;

    TMR_ConfigOC1(TMR8, &OCcongigStruct);
    TMR_ConfigOC1Preload(TMR8, TMR_OC_PRELOAD_ENABLE);
    TMR_EnableAutoReload(TMR8);

    TMR_EnableDMASoure(TMR8, TMR_DMA_SOURCE_CC1);

    TMR_Enable(TMR8);

    /* Enable TMR8 PWM output */
    TMR_EnablePWMOutputs(TMR8);

    while(1)
    {
    }
}

/*!
 * @brief       DMA Init
 *
 * @param       None
 *
 * @retval      None
 */
void DMA_Init(uint32_t* Buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    /* size of buffer*/
    dmaConfig.bufferSize = 3;
    /* set memory Data Size*/
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_HALFWORD;
    /* Set peripheral Data Size*/
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_HALFWORD;
    /* Enable Memory Address increase*/
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Disable Peripheral Address increase*/
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Reset Circular Mode*/
    dmaConfig.loopMode = DMA_MODE_CIRCULAR;
    /* set priority*/
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    /* read from peripheral*/
    dmaConfig.dir = DMA_DIR_MEMORYTOPERIPHERAL;
    /* Set memory Address*/
    dmaConfig.memoryBaseAddr = (uint32_t)Buf;
    /* Set Peripheral Address*/
    dmaConfig.peripheralBaseAddr = (uint32_t)&TMR8->CC1;

    dmaConfig.channel = DMA_CHANNEL_7;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;

    DMA_Config(DMA2_Stream2, &dmaConfig);

    DMA_Enable(DMA2_Stream2);
}
