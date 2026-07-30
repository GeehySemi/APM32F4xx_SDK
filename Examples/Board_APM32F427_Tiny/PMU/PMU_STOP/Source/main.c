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
#include "apm32f4xx_pmu.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_rcm.h"
#include "board_delay.h"
#include "log_printf.h"
#include <stdio.h>

/* Private macro **********************************************************/
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
__IO int8_t g_enterStopFlag = BIT_RESET;
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
    USART_Config_T usartConfigStruct;

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_EINT);
    BOARD_PBInit(BUTTON_KEY3, BUTTON_MODE_EINT);

    /* Configure Delay */
    BOARD_DelayConfig();

    /* Enable PMU Periph Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_Reset();

    LOG_Print("\r\nSystem is Init\r\n");
    LOG_Print("Press KEY2 to Enter STOP Mode\r\n");

    while (1)
    {
        BOARD_DelayMs(500);
        BOARD_LEDToggle(LED3);
        LOG_Print("\r\nSystem is running...\r\n");

        if(g_enterStopFlag == BIT_SET)
        {
            g_enterStopFlag = BIT_RESET;

            LOG_Print("\r\nSystem have entered STOP mode\r\n");
            LOG_Print("now, LED2 remains in the previous state\r\n");
            LOG_Print("please press KEY2 or reset System to exit STOP mode\r\n");
            BOARD_DelayMs(500);

            /* Enter STOP Mode in WFI*/
            PMU_EnterSTOPMode(PMU_REGULATOR_ON, PMU_STOP_ENTRY_WFI);
        }
    }
}

/*!
 * @brief       WakeUp from STOP mode Interrupt
 *
 * @param       None
 *
 * @retval      None
 */
void EINT9_5_IRQRequest(void)
{
    SystemInit();
    SystemClockConfig();
    LOG_Print("\r\nSystem have exit Stop mode\r\n");
}

/*!
 * @brief     Enter STOP mode Interrupt
 *
 * @param     None
 *
 * @retval    None
 */
void EINT15_10_IRQRequest(void)
{
    g_enterStopFlag = BIT_SET;
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

    /* wait for the data to be send  */
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

    /* wait for the data to be send  */
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
