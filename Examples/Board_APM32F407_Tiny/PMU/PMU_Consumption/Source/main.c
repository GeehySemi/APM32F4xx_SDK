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
#include "low_power_modes.h"
#include <stdio.h>


/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf using USART1  */
#define DEBUG_USART                     USART1

#define SYS_SLEEP_MODE                  1
#define SYS_STOP_MODE                   0
#define SYS_STANDBY_MODE                0
#define SYS_STANDBY_RTC_MODE            0
#define SYS_STANDBY_RTC_BKPSRAM_MODE    0

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

int8_t g_enterStopFlag = BIT_RESET;

/* Private function prototypes ********************************************/

/* Delay */
void Delay(uint32_t count);

/* BOR*/
void BOR_Config(FMC_OPT_BOR_T level);

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
    USART_Config_T usartConfigStruct;

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_PBInit(BUTTON_KEY1,BUTTON_MODE_GPIO);

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);

    printf("System reset\r\n");
    
    /* Enable PMU clock*/
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_Reset();

    /* Allow access to Backup domain*/
    PMU_EnableBackupAccess();
    
    /* Reset RTC*/
    RCM_EnableBackupReset();
    RCM_DisableBackupReset();
    
    /* If system was resumed from Standby mode*/
    if(PMU_ReadStatusFlag(PMU_FLAG_SB) != RESET)
    {
        PMU_ClearStatusFlag(PMU_FLAG_SB);
        printf("System was resumed from Standby mode\r\n");
        
        while(1)
        {
            BOARD_LEDToggle(LED2);
            Delay(0x3FFFFF);
        }
    }
    
    printf("Please press KEY1 to run specific mode\r\n");
    while(BOARD_PBGetState(BUTTON_KEY1) != RESET)
    {
    }
    printf("Waitting KEY1 release\r\n");
    while(BOARD_PBGetState(BUTTON_KEY1) == RESET)
    {
    }
    
#if (SYS_STANDBY_RTC_BKPSRAM_MODE)
    printf("Run SYS_STANDBY_RTC_BKPSRAM_MODE\r\n");
    ConsumptionMeasure_StandbyRTCBKPSRAMMode();
#elif (SYS_STANDBY_RTC_MODE)
    printf("Run SYS_STANDBY_RTC_MODE\r\n");
    ConsumptionMeasure_StandbyRTCMode();
#elif (SYS_STANDBY_MODE)
    printf("Run SYS_STANDBY_MODE\r\n");
    ConsumptionMeasure_StandbyMode();
#elif (SYS_STOP_MODE)
    printf("Run SYS_STOP_MODE\r\n");
    ConsumptionMeasure_StopMode();
#elif (SYS_SLEEP_MODE)
    printf("Run SYS_SLEEP_MODE\r\n");
    ConsumptionMeasure_SleepMode();
#else
    printf("Run normal mode\r\n");
    while (1)
    {
        Delay(0x3FFFFF);
        APM_MINI_LEDToggle(LED3);
    }
#endif
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
