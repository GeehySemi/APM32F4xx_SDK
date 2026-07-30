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

/* Private function prototypes ********************************************/
/* PVD*/
void PVD_Config(void);

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

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();

    PMU_Reset();

    PVD_Config();

    while (1)
    {
        BOARD_DelayMs(500);
        BOARD_LEDToggle(LED3);
    }
}

/*!
 * @brief     Configure power valtage detector
 *
 * @param     None
 *
 * @retval    None
 */
void PVD_Config(void)
{
    EINT_Config_T eintConfigStruct;
    
    /* Enable PMU clock*/
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    
    EINT_ClearIntFlag(EINT_LINE_16);
    eintConfigStruct.line = EINT_LINE_16;
    eintConfigStruct.mode = EINT_MODE_INTERRUPT;
    eintConfigStruct.trigger = EINT_TRIGGER_RISING;
    eintConfigStruct.lineCmd = ENABLE;
    EINT_Config(&eintConfigStruct);
    
    /* PVD detection level set to 2.6V*/
    PMU_ConfigPVDLevel(PMU_PVD_LEVEL_2V6);
    
    NVIC_EnableIRQRequest(PVD_IRQn, 0, 0);
    
    PMU_EnablePVD();
}

/*!
 * @brief     PVD interrupt callback
 *
 * @param     None
 *
 * @retval    None
 */
void PMU_PVDIRQRequest(void)
{
    uint8_t temp = 3;
    
    while(temp--)
    {
        LOG_Print("PVD detected that VDD rose above the threshold\r\n");
        BOARD_DelayMs(500);
        BOARD_LEDToggle(LED2);
    }
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
