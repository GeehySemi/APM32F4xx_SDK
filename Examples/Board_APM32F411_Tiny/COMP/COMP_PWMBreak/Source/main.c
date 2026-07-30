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
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_tmr.h"
#include "apm32f4xx_comp.h"
#include "apm32f4xx_usart.h"
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
uint32_t compOutVal = 0U;

/* Private function prototypes ********************************************/

void TMR_Init(void);
void COMP_Init(void);
void GPIO_Init(void);

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
    /* USART Initialization */
    SystemClockConfig();
    USART_Config_T usartConfigStruct;

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);
    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_DelayConfig();

    GPIO_Init();
    TMR_Init();
    COMP_Init();

    while (1)
    {
        compOutVal = COMP_ReadOutPutLevel(COMP_SELECT_COMP1);
        printf("Current comp out val: %ld\r\n", compOutVal);
        BOARD_LEDToggle(LED2);
        BOARD_DelayMs(500);
    }
}

/**
 * @brief   GPIO Init
 *
 * @param   None
 *
 * @retval  None
 */
void GPIO_Init(void)
{
    GPIO_Config_T  GPIO_InitStructure;
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC);

    GPIO_InitStructure.pin = GPIO_PIN_0;
    GPIO_InitStructure.mode = GPIO_MODE_AN;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief   TMR1 init
 *
 * @param   None
 *
 * @retval  None
 */
void TMR_Init(void)
{
    /* Enable TMR1 Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);

    /* Config TMR1 */
    TMR_BaseConfig_T tmrBaseConfig;
    tmrBaseConfig.clockDivision = TMR_CLOCK_DIV_1;
    tmrBaseConfig.countMode = TMR_COUNTER_MODE_UP;
    tmrBaseConfig.division = 119;
    tmrBaseConfig.period = 999;
    tmrBaseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &tmrBaseConfig);

    TMR_OCConfig_T tmrOCConfig;
    tmrOCConfig.idleState = TMR_OC_IDLE_STATE_SET;
    tmrOCConfig.nIdleState = TMR_OC_NIDLE_STATE_SET;
    tmrOCConfig.mode = TMR_OC_MODE_PWM1;
    tmrOCConfig.nPolarity = TMR_OC_NPOLARITY_HIGH;
    tmrOCConfig.polarity = TMR_OC_POLARITY_HIGH;
    tmrOCConfig.pulse = 499;
    TMR_ConfigOC1(TMR1, &tmrOCConfig);

    TMR_BDTConfig_T tmrBDTConfig;
    tmrBDTConfig.automaticOutput = TMR_AUTOMATIC_OUTPUT_ENABLE;
    tmrBDTConfig.BRKPolarity = TMR_BRK_POLARITY_HIGH;
    tmrBDTConfig.BRKState = TMR_BRK_STATE_ENABLE;
    tmrBDTConfig.deadTime = 0;
    tmrBDTConfig.IMOS = TMR_IMOS_STATE_DISABLE;
    tmrBDTConfig.lockLevel = TMR_LOCK_LEVEL_OFF;
    tmrBDTConfig.RMOS = TMR_RMOS_STATE_DISABLE;
    TMR_ConfigBDT(TMR1, &tmrBDTConfig);

    TMR_Enable(TMR1);
    TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_1);
    TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_1);
}

/**
 * @brief   COMP init
 *
 * @param   None
 *
 * @retval  None
 */
void COMP_Init(void)
{
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_COMP);

    COMP_Config_T compConfig;
    compConfig.invertingInput = COMP_INVERTING_INPUT_VREFINT;
    compConfig.mode = COMP_MODE_LOWSPEED;
    compConfig.output = COMP_OUTPUT_TIM1BKIN;
    compConfig.outputPol = COMP_OUTPUTPOL_NONINVERTED;
    COMP_Config(COMP_SELECT_COMP1, &compConfig);

    COMP_Enable(COMP_SELECT_COMP1);

    COMP_ConfigLOCK(COMP_SELECT_COMP1);
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    UNUSED(file);
    int i;

    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif
