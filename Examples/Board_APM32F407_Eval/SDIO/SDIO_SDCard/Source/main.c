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
#include "board_delay.h"
#include "board_usart.h"
#include "bsp_key.h"
#include "bsp_sdio.h"
#include <stdio.h>
#include <string.h>
#include "board_lcd.h"
#include "board_lcdfont.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

SD_ERROR_T g_status = SD_OK;

/* Private function prototypes ********************************************/

/** SD write and read single block test function */
void SD_SingleBlockTest(void);
/** SD write and read multi block test function */
void SD_MultiBlockTest(void);

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    SystemClockConfig();
    uint8_t keyType;

    /** Set system interrupt priority grouping */
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_2);

    /** Init delay function */
    BOARD_DelayConfig();

    /** Init LED */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /** Init USART1 */
    BOARD_USART1Init(115200);

    /** Init KEY */
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

    BOARD_LCDGPIOInit();
    BOARD_LCDSMCInit();
    BOARD_LCDReset();
    BOARD_LCDConfigRegister();
    BOARD_LCDEnableBackLight();

    BOARD_LCDConfigScan(LCD_SCAN_MODE_6);
    BOARD_LCDClearScreen();

    BOARD_LCDPrintString(30, 40, "This is a SDIO Demo!", LCD_COLOR_USER);

    /** Wait board and module power on */
    BOARD_DelayMs(200);

    /** Init SD Card */
    if((g_status = SD_Init()) != SD_OK)
    {
        BOARD_LEDOn(LED3);

        printf("SD Init Fail\n");
        BOARD_LCDPrintString(30, 80, "SD Init Fail      ", LCD_COLOR_USER);

    }
    else
    {
        BOARD_LEDOff(LED3);

        printf("SD Init Success\n");
        BOARD_LCDPrintString(30, 80, "SD Init Success      ", LCD_COLOR_USER);
    }
    /** Show SD card informations */
    SD_ShowCardInfo();

    BOARD_LCDPrintString(30, 120, "KEY1: Single Block Test      ", LCD_COLOR_USER);
    BOARD_LCDPrintString(30, 160, "KEY2: Multi Block Test      ", LCD_COLOR_USER);

    while(1)
    {
        keyType = Key_Scan(KEY_MODE_SINGLE);

        /** Select single block test function  */
        if(keyType == KEY1_PRESSED)
        {
            printf("Single Block Test\r\n");
            BOARD_LCDPrintString(30, 120, "Single Block Test      ", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 160, "                       ", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 200, "                       ", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 240, "                       ", LCD_COLOR_USER);

            if(g_status == SD_OK)
            {
                SD_SingleBlockTest();
            }
            while(KEY1 == 0);
        }
        /** Select multi block test function  */
        else if(keyType == KEY2_PRESSED)
        {
            printf("Multi Block Test\r\n");
            BOARD_LCDPrintString(30, 120, "Multi Block Test       ", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 160, "                       ", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 200, "                       ", LCD_COLOR_USER);
            BOARD_LCDPrintString(30, 240, "                       ", LCD_COLOR_USER);

            if(g_status == SD_OK)
            {
                SD_MultiBlockTest();
            }
            while(KEY2 == 0);
        }

        BOARD_DelayMs(10);
    }
}

/*!
 * @brief       Write and read SD card single block
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SD_SingleBlockTest(void)
{
    uint32_t sdSize;
    uint8_t sta = SD_OK;
    long long sectorAddress = 0;
    uint8_t buffer[512];

    /** Fill the data to be written  */
    for(sdSize = 0; sdSize < 512; sdSize++)
    {
        buffer[sdSize] = sdSize + 1 ;
    }
    /** Write one sector data to sector 0 address  */
    if((sta = SD_WriteBlock(buffer,sectorAddress,512)) == SD_OK)
    {
        printf("write success\r\n");
        BOARD_LCDPrintString(30, 160, "write success", LCD_COLOR_USER);
        BOARD_DelayMs(50);
    }
    else
    {
        printf("Write Error Status:%d\r\n",sta);
        BOARD_LCDPrintString(30, 160, "write Error", LCD_COLOR_USER);
    }

    memset(buffer,0,512);

    if((sta = SD_ReadBlock(buffer,sectorAddress,512)) == SD_OK)
    {
        /** Print sector 0 data  */
        printf("Sector 0 Data:\r\n");

        for(sdSize = 1; sdSize <= 512; sdSize++)
        {
            printf("%X ",buffer[sdSize - 1]);

            if((sdSize != 0) && ((sdSize % 16) == 0))
            {
                printf("\r\n");
            }
        }
        printf("Sector 0 Data Ended\r\n");
        BOARD_LCDPrintString(30, 200, "Read success", LCD_COLOR_USER);
    }
    else
    {
        printf("Read Error Status %d\r\n",sta);
        BOARD_LCDPrintString(30, 200, "Read Error", LCD_COLOR_USER);
    }

    printf("Single Block Test OK\r\n");
    BOARD_LCDPrintString(30, 240, "Single Block Test OK", LCD_COLOR_USER);
}

/*!
 * @brief       Write and read SD card multi block
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SD_MultiBlockTest(void)
{
    int sdSize;
    uint8_t sta = SD_OK;
    uint32_t sectorAddress = 0;
    uint8_t sectorCnt = 2;
    uint8_t buffer[1024];
    uint8_t i;


    /** Fill the data to be written  */
    for(sdSize = 0; sdSize < 512; sdSize++)
    {
        buffer[sdSize] = sdSize ;
    }

    /** Fill the data to be written  */
    for(sdSize = 512; sdSize < 1024; sdSize++)
    {
        buffer[sdSize] = 0xAA ;
    }

    /** Write one sector data to sector 0 address  */
    if((sta = SD_WriteMultiBlocks(buffer,sectorAddress,1024,sectorCnt)) == SD_OK)
    {
        printf("write success\r\n");
        BOARD_LCDPrintString(30, 160, "write success", LCD_COLOR_USER);
        BOARD_DelayMs(50);
    }
    else
    {
        printf("Write Error Status:%d\r\n",sta);
        BOARD_LCDPrintString(30, 160, "write Error", LCD_COLOR_USER);
    }

    memset(buffer,0,1024);

    if((sta = SD_ReadMultiBlocks(buffer,sectorAddress,1024,sectorCnt)) == SD_OK)
    {
        /** Print sector 0 data  */
        for(i = sectorAddress; i < sectorCnt; i++)
        {
            printf("Sector %d Data:\r\n",i);

            for(sdSize = 1 + (i * 512); sdSize <= 512 + (i * 512); sdSize++)
            {
                printf("%X ",buffer[sdSize - 1]);

                if((sdSize != 0) && ((sdSize % 16) == 0))
                {
                    printf("\r\n");
                }
            }
            printf("Sector %d Data Ended\r\n",i);
        }
        BOARD_LCDPrintString(30, 200, "Read success", LCD_COLOR_USER);
    }
    else
    {
        printf("Read Error Status %d\r\n",sta);
        BOARD_LCDPrintString(30, 200, "Read Error", LCD_COLOR_USER);
    }

    printf("Multi Block Test OK\r\n");
    BOARD_LCDPrintString(30, 240, "Multi Block Test OK", LCD_COLOR_USER);
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

