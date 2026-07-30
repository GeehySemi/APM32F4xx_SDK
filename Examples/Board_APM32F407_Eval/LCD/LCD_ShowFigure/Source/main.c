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
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_adc.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "board_lcd.h"
#include "board_lcdFont.h"
#include "picture.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

uint32_t i = 0;
uint32_t status = 0;

/* Private function prototypes ********************************************/

/** Delay */
void Delay(uint32_t count);
void LED_Demo(void);
void LCD_Demo(void);

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
    /** Configure LED GPIO Pin */
    SystemClockConfig();
    BOARD_LEDInit(LED1);
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /** USART Initialization */
    USART_Config_T usartConfigStruct;

    /** USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    /** COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    BOARD_LCDGPIOInit();
    BOARD_LCDSMCInit();
    BOARD_LCDReset();
    BOARD_LCDConfigRegister();
    BOARD_LCDEnableBackLight();

    BOARD_LCDConfigScan(LCD_SCAN_MODE_6);
    BOARD_LCDConfigBackColor(LCD_COLOR_WHITE);
    BOARD_LCDClearScreen();

    LCD_Demo();

    while (1)
    {
    }
}

void LCD_Demo(void)
{
    /** Display Geehy logo */
    BOARD_LCDDisplayPic(120, 1, 240, 108, imageGeehyLogo);
    BOARD_LCDPrintString(185, 90, "LCD Demo", LCD_COLOR_USER);
    BOARD_LCDDrawLine(10, 120, 470, 120, LCD_COLOR_USER);
    BOARD_LCDDrawRectangle(10, 130, BOARD_LCDReadMaxLenX() - 10, BOARD_LCDReadMaxLenY() - 10, LCD_COLOR_USER);

    while (1)
    {
        /** Open a window for demo */
        BOARD_LCDOpenWindow(11, 131, BOARD_LCDReadMaxLenX() - 21, BOARD_LCDReadMaxLenY() - 141);
        /** Clear window */
        BOARD_LCDFillWindowColor(BOARD_LCDReadBackColor());

        switch(status)
        {
            case 0:
                BOARD_LCDDrawCircle(240, 300, 100, LCD_COLOR_RED, ENABLE);
                BOARD_LCDDrawCircle(360, 500, 100, LCD_COLOR_YELLOW, ENABLE);
                BOARD_LCDDrawCircle(120, 500, 100, LCD_COLOR_BLUE, ENABLE);
                BOARD_LEDOn(LED1);       
                BOARD_LEDOff(LED2);
                BOARD_LEDOff(LED3);
                break;
            case 1:
                BOARD_LCDDrawRectangle(100, 400, 380, 500, LCD_COLOR_BLACK);
                BOARD_LCDDrawRectangle(180, 320, 300, 580, LCD_COLOR_RED);
                BOARD_LEDOff(LED1);
                BOARD_LEDOn(LED2);
                BOARD_LEDOff(LED3);
                break;
            default:
                BOARD_LCDDrawLine(240, 300, 120, 500, LCD_COLOR_RED);
                BOARD_LCDDrawLine(240, 300, 360, 500, LCD_COLOR_YELLOW);
                BOARD_LCDDrawLine(120, 500, 360, 500, LCD_COLOR_BLUE);
                BOARD_LCDDrawLine(240, 550, 120, 350, LCD_COLOR_BLACK);
                BOARD_LCDDrawLine(240, 550, 360, 350, LCD_COLOR_USER);
                BOARD_LCDDrawLine(120, 350, 360, 350, LCD_COLOR_GREEN);
                BOARD_LCDDrawCircle(240, 425, 150, LCD_COLOR_RED, DISABLE);
                BOARD_LEDOff(LED1);
                BOARD_LEDOff(LED2);
                BOARD_LEDOn(LED3);
                break;
        }
        while (1)
        {
            if (!BOARD_PBGetState(BUTTON_KEY1))
            {
                while (!BOARD_PBGetState(BUTTON_KEY1));
                
                if (status++ > 1)
                {
                    status = 0;
                }
                break;
                
               
            }
        }
    }
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
