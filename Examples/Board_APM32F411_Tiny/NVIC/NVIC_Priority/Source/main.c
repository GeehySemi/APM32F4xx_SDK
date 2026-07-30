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
#include "stdio.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_fmc.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/* Delay */
void Delay(uint32_t count);

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    SystemClockConfig();
    USART_Config_T usartConfigStruct;

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);

    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_EINT);

    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_1);

    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    /* Configure NVIC_IRQRequest */
    NVIC_EnableIRQRequest(EINT0_IRQn, 0, 1);
    NVIC_EnableIRQRequest(EINT1_IRQn, 1, 1);

    printf("push KEY1 to enter interrupt.\r\n\r\n");

    while (1)
    {
    }
}

/*!
 * @brief     The interrupt will happen when the button 1 is press
 *
 * @param     None
 *
 * @retval    None
 *
 * @note      This function should put into EINT1_IRQHandler() in apm32f4xx_int.c
 */
void APM_TINY_PB1_Isr(void)
{
    if(EINT_ReadStatusFlag(KEY1_BUTTON_EINT_LINE) == SET)
    {
        EINT_ClearStatusFlag(KEY1_BUTTON_EINT_LINE);
        while(1)
        {
            Delay(0x8FFFFF);
            BOARD_LEDToggle(LED2);
            printf("working in EINT1_IRQHandler\r\n");
            printf("push KEY2 to enter higher interrupt.\r\n\r\n");
        }
    }
}

/*!
 * @brief     The interrupt will happen when the button 2 is press
 *
 * @param     None
 *
 * @retval    None
 *
 * @note      This function should put into EINT2_IRQHandler() in apm32f4xx_int.c
 */
void APM_TINY_PB2_Isr(void)
{
    if(EINT_ReadStatusFlag(KEY2_BUTTON_EINT_LINE) == SET)
    {
        EINT_ClearStatusFlag(KEY2_BUTTON_EINT_LINE);
        BOARD_LEDOff(LED2);
        while(1)
        {
            Delay(0x8FFFFF);
            BOARD_LEDToggle(LED3);
            printf("working in EINT0_IRQHandler\r\n");
            printf("It is higher interrupt.\r\n\r\n");
        }
    }
}

/*!
 * @brief     Delay
 *
 * @param     count:  delay count
 *
 * @retval    None
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;
    while(delay--);
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
