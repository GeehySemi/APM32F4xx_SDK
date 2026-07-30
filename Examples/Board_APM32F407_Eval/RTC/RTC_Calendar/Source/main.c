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
#include "stdio.h"
#include "apm32f4xx_rtc.h"
#include "apm32f4xx_pmu.h"
#include "board_lcd.h"
#include "board_lcdfont.h"
/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1
#define RTC_BAKP_REG_0_DATA  0x1314

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

uint8_t intStatus;

/* Private function prototypes ********************************************/

/** Delay */
void Delay(uint32_t count);

/** RTC Init */
void RTC_Init(RCM_RTCCLK_T rtcClkSelect);
/** RTC Calendar Init */
void RTC_CalendarConfig(void);

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
    uint8_t temp = 0;
    char CalendarBuf[2][50] = {0};

    RTC_DateConfig_T DateStruct;
    RTC_TimeConfig_T TimeStruct;
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

    BOARD_LCDGPIOInit();
    BOARD_LCDSMCInit();
    BOARD_LCDReset();
    BOARD_LCDConfigRegister();
    BOARD_LCDEnableBackLight();

    BOARD_LCDConfigScan(LCD_SCAN_MODE_6);
    BOARD_LCDClearScreen();

    /** Config RTC */
    RTC_Init(RCM_RTCCLK_LSE);

    if (RTC_ReadBackup(RTC_BAKP_REG_0) != RTC_BAKP_REG_0_DATA)
    {
        /** Config Calendar */
        RTC_CalendarConfig();
        RTC_WriteBackup(RTC_BAKP_REG_0, RTC_BAKP_REG_0_DATA);
    }

    while(1)
    {
        RTC_ReadDate(RTC_FORMAT_BIN, &DateStruct);
        RTC_ReadTime(RTC_FORMAT_BIN, &TimeStruct);

        /** display Calendar */
        if(temp != TimeStruct.seconds)
        {
            /** Refresh Calendar date string */
            sprintf(CalendarBuf[0],"Date : 20%02d/%02d/%02d weekday:%d\n",
            DateStruct.year,
            DateStruct.month,
            DateStruct.date,
            DateStruct.weekday);


            /** Refresh Calendar time string */
            sprintf(CalendarBuf[1],"Time : %02d:%02d:%02d\r\n",
            TimeStruct.hours,
            TimeStruct.minutes,
            TimeStruct.seconds);

            /** printf date and time to uart */
            printf("\nThis is a RTC Calendar Demo!\n");
            printf("%s",CalendarBuf[0]);
            printf("%s",CalendarBuf[1]);

            /** printf date and time to LCD */
            BOARD_LCDPrintString(30, 40,  "This is a RTC Calendar Demo!", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 80,  CalendarBuf[0], LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 120, CalendarBuf[1], LCD_COLOR_USER);

            BOARD_LEDToggle(LED1);
            temp = TimeStruct.seconds;
        }
    }
}

/*!
 * @brief       RTC Init
 *
 * @param     rtcClkSelect : specifies the RTC clock source.
 *                           This parameter can be one of the following values:
 *                           @arg RCM_RTCCLK_LSE        : RTCCLK = LSE clock
 *                           @arg RCM_RTCCLK_LSI        : RTCCLK = LSI clock
 *
 * @retval      None
 */
void RTC_Init(RCM_RTCCLK_T rtcClkSelect)
{
    RTC_Config_T Struct;

    /** Enable the PWR clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /** Allow access to RTC */
    PMU_EnableBackupAccess();

    if(rtcClkSelect == RCM_RTCCLK_LSI)
    {
        /** Enable the LSI OSC */
        RCM_EnableLSI();

        /** Wait till LSE is ready */
        while(RCM_ReadStatusFlag(RCM_FLAG_LSIRDY) == RESET)
        {
        }
        /** Select the RTC Clock Source */
        RCM_ConfigRTCCLK(RCM_RTCCLK_LSI);
    }
    else
    {
        /** Enable the LSI OSC */
        RCM_ConfigLSE(RCM_LSE_OPEN);

        /** Wait till LSE is ready */
        while(RCM_ReadStatusFlag(RCM_FLAG_LSERDY) == RESET)
        {
        }
        /** Select the RTC Clock Source */
        RCM_ConfigRTCCLK(RCM_RTCCLK_LSE);
    }

    /** Enable RTC Clock*/
    RCM_EnableRTCCLK();

    /** Disable Write Proctection */
    RTC_DisableWriteProtection();

    /** Wait Synchro*/
    RTC_WaitForSynchro();

    RTC_ConfigStructInit(&Struct);

    if (RTC_Config(&Struct) == SUCCESS)
    {
        printf("\n\r RTC initialization completed \r\n");
        BOARD_LEDOn(LED2);
    }
    else
    {
        printf("\n\r RTC initialization failed \r\n");
        printf("\n\r please motherboard power down \r\n");
        printf("\n\r then remove the battery and reinstall it \r\n");

        BOARD_LEDOff(LED2);
    }

    /** Enable the PWR clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /** Allow access to RTC */
    PMU_EnableBackupAccess();

    /** Wait Synchro*/
    RTC_WaitForSynchro();
}

/*!
 * @brief       Config Calendar
 *
 * @param       None
 *
 * @retval      None
 */
void RTC_CalendarConfig(void)
{
    RTC_DateConfig_T DateStruct;
    RTC_TimeConfig_T TimeStruct;

    /** Config Alarm */
    RTC_ConfigDateStructInit(&DateStruct);
    DateStruct.year = 22;
    DateStruct.month = RTC_MONTH_MAY;
    DateStruct.date = 20;
    DateStruct.weekday = RTC_WEEKDAY_FRIDAY;
    RTC_ConfigDate(RTC_FORMAT_BIN,&DateStruct);

    /** Config Time */
    RTC_ConfigTimeStructInit(&TimeStruct);
    TimeStruct.hours = 2;
    TimeStruct.minutes = 2;
    TimeStruct.seconds = 2;
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

