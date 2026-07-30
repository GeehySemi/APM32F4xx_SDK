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
#include "stdio.h"
#include "apm32f4xx_tmr.h"
#include "log_printf.h"

/* Private macro **********************************************************/
/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

void Delay(void);

/* External variables *****************************************************/

volatile uint32_t tick = 0;

/* External functions *****************************************************/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        TMR1 as time base for delay,
 *              TMR9 as High 16 bit count value,
 *              TMR10 as Low  16 bit count value.
 *
 */
int main(void)
{
    SystemClockConfig();
    uint32_t temp;
    USART_Config_T   usartConfig;
    TMR_BaseConfig_T baseConfig;

    BOARD_LEDInit(LED2);
    BOARD_LEDOff(LED2);

    /* USART1 Tx PA9 Rx PA10 */
    usartConfig.baudRate = 115200;
    usartConfig.mode     = USART_MODE_TX_RX;
    usartConfig.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfig.parity   = USART_PARITY_NONE;
    usartConfig.stopBits =  USART_STOP_BIT_1;
    usartConfig.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1,&usartConfig);

    /* TMR1,9,10 Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1 | RCM_APB2_PERIPH_TMR9 | RCM_APB2_PERIPH_TMR10);

    /* TMR9 Configure */
    baseConfig.division = 0;
    baseConfig.period = 0xFFFF;
    baseConfig.clockDivision = TMR_CLOCK_DIV_1;
    baseConfig.countMode = TMR_COUNTER_MODE_UP;
    TMR_ConfigTimeBase(TMR9, &baseConfig);
    TMR_ConfigPrescaler(TMR9,0,TMR_PSC_RELOAD_UPDATE);
    TMR_EnableUpdate(TMR9);

    /* Configure TMR9 Input */
    TMR_SelectInputTrigger(TMR9,TMR_TRIGGER_SOURCE_ITR3);
    TMR_SelectSlaveMode(TMR9,TMR_SLAVE_MODE_EXTERNAL1);
    TMR_EnableMasterSlaveMode(TMR9);
    TMR_EnableAutoReload(TMR9);

    /* TMR10 Configure */
    baseConfig.division = 239;
    baseConfig.period = 0xFFFF;
    baseConfig.clockDivision = TMR_CLOCK_DIV_1;
    baseConfig.countMode = TMR_COUNTER_MODE_UP;
    TMR_ConfigTimeBase(TMR10, &baseConfig);
    TMR_EnableAutoReload(TMR10);

    /* Configure TMR10 Output */
    TMR_SelectSlaveMode(TMR10,TMR_SLAVE_MODE_RESET);
    TMR_ConfigUpdateRequest(TMR10,TMR_UPDATE_SOURCE_REGULAR);
    TMR_SelectOutputTrigger(TMR10,TMR_TRGO_SOURCE_UPDATE);
    TMR_EnableMasterSlaveMode(TMR10);

    /* TMR1 Configure */
    baseConfig.clockDivision = TMR_CLOCK_DIV_1;
    baseConfig.countMode = TMR_COUNTER_MODE_UP;
    baseConfig.division = 239;
    baseConfig.period = 999;
    baseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &baseConfig);

    /* Enable TMR1 Interrupt */
    TMR_EnableInterrupt(TMR1, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR1_UP_TMR10_IRQn, 0, 0);

    /* Enable TMR */
    TMR_Enable(TMR1);
    TMR_Enable(TMR9);
    TMR_Enable(TMR10);

    while(1)
    {
        temp = TMR9->CNT << 16;
        temp |= TMR10->CNT;
        LOG_Print(" Count Value: 0x%08x\r\n", temp);
        BOARD_LEDToggle(LED2);
        Delay();
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    tick = 0;

    while(tick < 500);
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
    int i;

    UNUSED(file);

    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

