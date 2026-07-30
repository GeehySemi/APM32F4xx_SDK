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
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_rcm.h"
#include "board_delay.h"
#include "log_printf.h"
#include "sdio.h"
#include <stdio.h>
#include <string.h>

/* Private macro **********************************************************/
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
/* Record SD card command and data status */
SD_ERROR_T g_status = SD_OK;

/* Private function prototypes ********************************************/
/* SD write and read single block test function */
void SD_SingleBlockTest(void);
/** SD write and read multi block test function */
void SD_MultiBlockTest(void);

/* External variables *****************************************************/

/* External functions *****************************************************/
extern SD_CardInfo_T SDCardInfo;

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

    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();

    /* Wait board and module power on */
    BOARD_DelayMs(200);

    /* Init SD Card */
    while((g_status = SD_Init()) != SD_OK)
    {
        BOARD_LEDOn(LED3);

        LOG_Print("SD Init Fail\n");

    }
    BOARD_LEDOff(LED3);

    LOG_Print("SD Init Success\n");

    /* Show SD card informations */
    SD_ShowCardInfo();

    while (1)
    {
        /* Select single block test function  */
        if (BOARD_PBGetState(BUTTON_KEY1) == RESET)
        {
            BOARD_DelayMs(10);

            if (BOARD_PBGetState(BUTTON_KEY1) == RESET)
            {
                printf("Single Block Test\r\n");

                if(g_status == SD_OK)
                {
                    SD_SingleBlockTest();
                }
                while(BOARD_PBGetState(BUTTON_KEY1) == RESET);
            }
        }
        /* Select multi block test function  */
        else if (BOARD_PBGetState(BUTTON_KEY2) == RESET)
        {
            BOARD_DelayMs(10);

            if (BOARD_PBGetState(BUTTON_KEY2) == RESET)
            {
                printf("Multi Block Test\r\n");

                if(g_status == SD_OK)
                {
                    SD_MultiBlockTest();
                }
                while(BOARD_PBGetState(BUTTON_KEY2) == RESET);
            }
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

    /* Fill the data to be written  */
    for(sdSize = 0; sdSize < 512; sdSize++)
    {
        buffer[sdSize] = sdSize + 1 ;
    }
    /* Write one sector data to sector 0 address  */
    if((sta = SD_WriteBlock(buffer,sectorAddress,512)) == SD_OK)
    {
        LOG_Print("write success\r\n");
        BOARD_DelayMs(50);
    }
    else
    {
        LOG_Print("Write Error Status:%d\r\n",sta);
    }

    memset(buffer,0,512);

    if((sta = SD_ReadBlock(buffer,sectorAddress,512)) == SD_OK)
    {
        /* Print sector 0 data  */
        LOG_Print("Sector 0 Data:\r\n");

        for(sdSize = 1; sdSize <= 512; sdSize++)
        {
            LOG_Print("%X ",buffer[sdSize - 1]);

            if((sdSize != 0) && ((sdSize % 16) == 0))
            {
                LOG_Print("\r\n");
            }
        }
        LOG_Print("Sector 0 Data Ended\r\n");
    }
    else
    {
        LOG_Print("Read Error Status %d\r\n",sta);
    }

    LOG_Print("Single Block Test OK\r\n");
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
    uint32_t sdSize;
    uint8_t sta = SD_OK;
    uint32_t sectorAddress = 0;
    uint8_t sectorCnt = 2;
    uint8_t buffer[1024];
    uint8_t i;


    /* Fill the data to be written  */
    for(sdSize = 0; sdSize < 512; sdSize++)
    {
        buffer[sdSize] = sdSize ;
    }

    /* Fill the data to be written  */
    for(sdSize = 512; sdSize < 1024; sdSize++)
    {
        buffer[sdSize] = 0xAA ;
    }

    /* Write one sector data to sector 0 address  */
    if((sta = SD_WriteMultiBlocks(buffer,sectorAddress,1024,sectorCnt)) == SD_OK)
    {
        LOG_Print("write success\r\n");
        BOARD_DelayMs(50);
    }
    else
    {
        LOG_Print("Write Error Status:%d\r\n",sta);
    }

    memset(buffer,0,1024);

    if((sta = SD_ReadMultiBlocks(buffer,sectorAddress,1024,sectorCnt)) == SD_OK)
    {
        /* Print sector 0 data  */
        for(i = sectorAddress; i < sectorCnt; i++)
        {
            LOG_Print("Sector %d Data:\r\n",i);
            
            for(sdSize = 1 + (i * 512); sdSize <= (uint32_t)(512 + (i * 512)); sdSize++)
            {
                LOG_Print("%X ",buffer[sdSize - 1]);
                 
                if((sdSize != 0) && ((sdSize % 16) == 0))
                {
                    LOG_Print("\r\n");
                }
            }
            LOG_Print("Sector %d Data Ended\r\n",i);
        }
    }
    else
    {
        LOG_Print("Read Error Status %d\r\n",sta);
    }

    LOG_Print("Multi Block Test OK\r\n");
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
