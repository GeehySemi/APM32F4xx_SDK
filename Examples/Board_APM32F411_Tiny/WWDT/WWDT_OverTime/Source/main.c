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
#include "apm32f4xx_wwdt.h"
#include "apm32f4xx_eint.h"
#include <stdio.h>
#include "board_delay.h"

/* Private macro **********************************************************/

/** printf using USART1  */
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/** The OverTime is used to change the test way*/
uint8_t is_OverTime = 0;

static __IO u32 TimingDelay;

/* External functions *****************************************************/

void SysTick_Init(void);
void SysTick_Delay_ms(__IO u32 nTime);
void TimingDelay_Decrement(void);

/** WWDT_Init */
void WWDT_Init(void);

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

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /* Clear WWDTRST flag */
    if (RCM_ReadStatusFlag(RCM_FLAG_WWDTRST)!=RESET)
    {
        BOARD_LEDOn(LED3);
        RCM_ClearStatusFlag();
    }

    printf("\r\n System reset \r\n");
    printf("\r\n The overtime is from 30.5ms to 41.6ms \r\n");

    BOARD_DelayConfig();
    BOARD_DelayMs(500);
    WWDT_Init();

    while (1)
    {
        if (is_OverTime == RESET)
        {
            BOARD_LEDToggle(LED2);

            /* Delay 31ms , 30.5<31<41.6 ,Refresh allowed, system still running */
            BOARD_DelayMs(31);

            WWDT_ConfigCounter(127);
            printf("running...\r\n");
        }

        if (is_OverTime == SET)
        {
            BOARD_LEDOn(LED3);

            /* Delay 45ms  ,41.6<45 ,Refresh not allowed, system reset */
            BOARD_DelayMs(45);

            WWDT_ConfigCounter(127);
        }

        if (!BOARD_PBGetState(BUTTON_KEY1))
        {
            BOARD_DelayMs(5);
            if (!BOARD_PBGetState(BUTTON_KEY1))
            {
                is_OverTime = SET;
            }
        }
    }
}

/*!
 * @brief       WWDT configuration
 *
 * @param       None
 *
 * @retval      None
 */
void WWDT_Init(void)
{
    /* Enable WWDT clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_WWDT);

    /* WWDT TimeBase =  PCLK1 (60MHz)/4096)/8 = 1831Hz (~0.54 ms)  */
    WWDT_ConfigTimebase(WWDT_TIME_BASE_8);

    /* Set Window value to 80; WWDT counter should be refreshed only when the counter
    is below 80 (and greater than 64) otherwise a reset will be generated  */
    WWDT_ConfigWindowData(80);

    /* Enable WWDG and set counter value to 127
     In this case the refresh window is: ~0.54 * (127-80)= 25.38ms < refresh window < ~0.54 * 64 = 34.56ms*/
    WWDT_Enable(127);
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
