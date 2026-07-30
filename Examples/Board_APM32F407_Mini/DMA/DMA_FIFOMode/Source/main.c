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
#include "apm32f4xx_dma.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

#define DMA_BUFFER_SIZE             64U

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

static const uint32_t srcBuffer[DMA_BUFFER_SIZE] =
{
    0x01020304U, 0x05060708U, 0x090A0B0CU, 0x0D0E0F10U,
    0x11121314U, 0x15161718U, 0x191A1B1CU, 0x1D1E1F20U,
    0x21222324U, 0x25262728U, 0x292A2B2CU, 0x2D2E2F30U,
    0x31323334U, 0x35363738U, 0x393A3B3CU, 0x3D3E3F40U,
    0x41424344U, 0x45464748U, 0x494A4B4CU, 0x4D4E4F50U,
    0x51525354U, 0x55565758U, 0x595A5B5CU, 0x5D5E5F60U,
    0x61626364U, 0x65666768U, 0x696A6B6CU, 0x6D6E6F70U,
    0x71727374U, 0x75767778U, 0x797A7B7CU, 0x7D7E7F80U,

    0x81828384U, 0x85868788U, 0x898A8B8CU, 0x8D8E8F90U,
    0x91929394U, 0x95969798U, 0x999A9B9CU, 0x9D9E9FA0U,
    0xA1A2A3A4U, 0xA5A6A7A8U, 0xA9AAABACU, 0xADAEAFB0U,
    0xB1B2B3B4U, 0xB5B6B7B8U, 0xB9BABBBCU, 0xBDBEBFC0U,
    0xC1C2C3C4U, 0xC5C6C7C8U, 0xC9CACBCCU, 0xCDCECFD0U,
    0xD1D2D3D4U, 0xD5D6D7D8U, 0xD9DADBDCU, 0xDDDEDFE0U,
    0xE1E2E3E4U, 0xE5E6E7E8U, 0xE9EAEBECU, 0xEDEEEFF0U,
    0xF1F2F3F4U, 0xF5F6F7F8U, 0xF9FAFBFCU, 0xFDFEFF00U,
};

static uint32_t dstBuffer[DMA_BUFFER_SIZE];

/* Private function prototypes ********************************************/

void DMA_Init(void);

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
    /* USART Initialization */
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
    
    BOARD_DelayConfig();

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    
    DMA_Init();
    DMA_Enable(DMA2_Stream0);

    while (1)
    {
        BOARD_LEDToggle(LED2);
        BOARD_DelayMs(500U);
    }
}

/*!
 * @brief       DMA Init
 *
 * @param       None
 *
 * @retval      None
 */
void DMA_Init(void)
{
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    /* Config DMA */
    DMA_Config_T dmaConfig;
    dmaConfig.bufferSize = DMA_BUFFER_SIZE;
    dmaConfig.channel = DMA_CHANNEL_0;
    dmaConfig.dir = DMA_DIR_MEMORYTOMEMORY;
    dmaConfig.fifoMode = DMA_FIFOMODE_ENABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.loopMode = DMA_MODE_NORMAL;
    dmaConfig.memoryBaseAddr = (uint32_t)dstBuffer;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_INC4;
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_WORD;
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    dmaConfig.peripheralBaseAddr = (uint32_t)srcBuffer;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_INC4;
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_WORD;
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_ENABLE;
    dmaConfig.priority = DMA_PRIORITY_VERYHIGH;

    DMA_Config(DMA2_Stream0, &dmaConfig);

    DMA_EnableInterrupt(DMA2_Stream0, DMA_INT_TCIFLG | DMA_INT_TEIFLG);
    NVIC_EnableIRQRequest(DMA2_STR0_IRQn, 0, 0);
}

/*!
 * @brief       DMA2 Stream0 Isr
 *
 * @param       None
 *
 * @retval      None
 */
void DMA2_STR0_Isr(void)
{
    if (DMA_ReadIntFlag(DMA2_Stream0, DMA_INT_TCIFLG0))
    {
        printf("DMA Transfer Complete!\r\n");
        DMA_ClearIntFlag(DMA2_Stream0, DMA_INT_TCIFLG0);
    }
    
    if (DMA_ReadIntFlag(DMA2_Stream0, DMA_INT_TEIFLG0))
    {
        printf("DMA Transfer Error!\r\n");
        DMA_ClearIntFlag(DMA2_Stream0, DMA_INT_TEIFLG0);
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
