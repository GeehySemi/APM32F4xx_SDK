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
#include "apm32f4xx_rtc.h"
#include "apm32f4xx_pmu.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/


/* used to indicate interrupt status*/
uint8_t intStatus;

/* Private function prototypes ********************************************/

/* Delay */
void Delay(uint32_t count);

/* RTC Init */
void RTC_Init(void);
/* RTC Alarm Init */
void RTC_AlarmsConfig(void);

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

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /* Allow access to RTC */
    PMU_EnableBackupAccess();

    /* Enable the LSI OSC */
    RCM_EnableLSI();

    /* Reset RTC*/
    RTC_Reset();

    /* Config RTC */
    RTC_Init();

    /* Config Alarm */
    RTC_AlarmsConfig();

    printf("Please wait 5 seconds\r\n");
//    Delay(10000);
    /* Alarm Enable */
    RTC_EnableAlarmA();
    BOARD_LEDOn(LED2);

    /* wait until time out */
    while(RTC_ReadStatusFlag(RTC_FLAG_ALRAF)==RESET);

    printf("Time out\r\n");

    BOARD_LEDOff(LED2);
    while(1)
    {
       BOARD_LEDToggle(LED3);
       Delay(0x4FFFFF);
    }
}

/*!
 * @brief       RTC Init
 *
 * @param       None
 *
 * @retval      None
 */
void RTC_Init(void)
{
    RTC_Config_T Struct;

    /* Enable the PWR clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /* Allow access to RTC */
    PMU_EnableBackupAccess();

    /* Enable the LSI OSC */
    RCM_EnableLSI();

    /* Wait till LSE is ready */
    while(RCM_ReadStatusFlag(RCM_FLAG_LSIRDY) == RESET)
    {
    }
    /* Select the RTC Clock Source */
    RCM_ConfigRTCCLK(RCM_RTCCLK_LSI);

    /* Enable RTC Clock*/
    RCM_EnableRTCCLK();

    /* Disable Write Proctection */
    RTC_DisableWriteProtection();

    /* Wait Synchro*/
    RTC_WaitForSynchro();

    RTC_ConfigStructInit(&Struct);

    RTC_Config(&Struct);
}

/*!
 * @brief       Config Alarm
 *
 * @param       None
 *
 * @retval      None
 */
void RTC_AlarmsConfig(void)
{
    RTC_TimeConfig_T TimeStruct;
    RTC_AlarmConfig_T alarmStruct;

    /* Config Alarm */
    RTC_ConfigAlarmStructInit(&alarmStruct);
    alarmStruct.alarmMask = 0x80808000;
    alarmStruct.time.seconds = 5;
    alarmStruct.time.hours = 0;
    alarmStruct.time.minutes = 0;
    RTC_ConfigAlarm(RTC_FORMAT_BIN,RTC_ALARM_A,&alarmStruct);

    /* Config Time */
    TimeStruct.h12 = RTC_H12_AM;
    TimeStruct.hours = 0;
    TimeStruct.minutes = 0;
    TimeStruct.seconds = 0;
    RTC_ConfigTime(RTC_FORMAT_BIN,&TimeStruct);
}

/*!
 * @brief       Delay
 *
 * @param       count:  delay count
 *
 * @retval      None
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
