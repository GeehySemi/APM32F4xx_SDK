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
#include "apm32f4xx_iwdt.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf using USART1  */
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

volatile uint32_t TimingDelay = 0;
volatile uint8_t bKEY1_Flag = 0;

/* Private function prototypes ********************************************/

/* USART Initialization */
void USART_Init(void);
void Delay(void);

/* External variables *****************************************************/

/* External functions *****************************************************/

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
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);

    /* USART Initialization */
    USART_Init();
    printf("IWDT is running, if press KEY1 to start feed watchdog, Ohterwise System will reset about 4 seconds!\r\n");

    SysTick_Config(SystemCoreClock / 1000);

    if(RCM_ReadStatusFlag(RCM_FLAG_IWDTRST) == SET)
    {
        /* IWDT Reset */
        BOARD_LEDOn(LED3);
        printf("\r\n IWDT Reset! \r\n");
        RCM_ClearStatusFlag();
    }
    else
    {
        BOARD_LEDOff(LED3);
    }

    IWDT_EnableWriteAccess();

    /* Colck is 1000Hz(32k/32) */
    IWDT_ConfigDivider(IWDT_DIVIDER_32);

    /* About 4 seconds */
    IWDT_ConfigReload(4000);
    IWDT_Refresh();
    IWDT_Enable();

    while(1)
    {
        if(bKEY1_Flag == 1)
        {
            BOARD_LEDToggle(LED2);

            Delay();

            IWDT_Refresh();
            printf("\r\nif press KEY1 again to finish feeding watchdog, System will reset about 4 seconds!\r\n");
        }
        else
        {
            BOARD_LEDOff(LED2);
        }
    }
}

/*!
 * @brief     USART Initialization
 *
 * @param     None
 *
 * @retval    None
 */
void USART_Init(void)
{
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
}

/*!
 * @brief     Delay
 *
 * @param     None
 *
 * @retval    None
 */
void Delay(void)
{
    TimingDelay = 0;
    while(TimingDelay < 300);
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
