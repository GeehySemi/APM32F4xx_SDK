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

#define BUFFER_SIZE             40

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

const uint32_t FMC_Buffer[BUFFER_SIZE]=
{
    0X3A946304, 0X75E48BBD, 0XB039358D, 0X3850A3FA, 0X284BCDC7,
    0X43B33E68, 0X4EBD5D5F, 0X6D1B4980, 0XF5C3B9C3, 0X16466442,
    0XADF6A017, 0X7184EEA3, 0X66FEBAD3, 0X06AA73D0, 0X7710DFE6,
    0XCBD535E1, 0XA564CD4B, 0XBAF092D8, 0XEDF0A59B, 0X17445073,
    0X5C5FD295, 0X65014689, 0XB277C8ED, 0X7B00C9D5, 0X2D7A915B,
    0XBA61431E, 0X2FC28956, 0XEA9F166E, 0XBE7EACCA, 0X13454BDA,
    0X1E216947, 0X0D9248EB, 0X001CD302, 0XC46FAD04, 0XE18BBF7B,
    0XDEFD0706, 0XE27B3232, 0X74801A22, 0XA8D5B2BE, 0X738C7DED,
};
uint32_t SRAM_Buffer[BUFFER_SIZE];

/* Private function prototypes ********************************************/

void DMA_Init(void);
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);

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
    uint16_t i = 0;
    uint8_t flag = 0;

    /* USART Initialization */
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

    for (i=0; i<BUFFER_SIZE; i++)
    {
        SRAM_Buffer[i]=0;
    }

    /* Configure and enable the DMA Stream for Memory to Memory transfer */
    DMA_Init();
    DMA_Enable(DMA2_Stream0);

    while (DMA_ReadCmdStatus(DMA2_Stream0) != DISABLE)
    {
    }

    /* Check if the transmitted and received data are equal */
    flag = Buffercmp(FMC_Buffer, SRAM_Buffer, BUFFER_SIZE);

    if (flag == 1)
    {
        /* Turn LED2 on: Transfer pass */
        BOARD_LEDOn(LED2);
        printf("Transfer pass! \r\n");
    }
    else
    {
        /* Turn LED3 on: Transfer fail */
        BOARD_LEDOn(LED3);
        printf("Transfer fail! \r\n");
    }

    while (1)
    {
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
    dmaConfig.bufferSize = BUFFER_SIZE;
    dmaConfig.channel = DMA_CHANNEL_0;
    dmaConfig.dir = DMA_DIR_MEMORYTOMEMORY;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.loopMode = DMA_MODE_NORMAL;
    dmaConfig.memoryBaseAddr = (uint32_t)SRAM_Buffer;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_WORD;
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    dmaConfig.peripheralBaseAddr = (uint32_t)FMC_Buffer;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_WORD;
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_ENABLE;
    dmaConfig.priority = DMA_PRIORITY_VERYHIGH;

    DMA_Config(DMA2_Stream0, &dmaConfig);

    DMA_EnableInterrupt(DMA2_Stream0, DMA_INT_TCIFLG);
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
        DMA_ClearIntFlag(DMA2_Stream0, DMA_INT_TCIFLG0);
    }
}

/*!
 * @brief     Compares two buffers
 *
 * @param     pBuffer, pBuffer1: buffers to be compared
 *
 * @param     BufferLength: buffer's length
 *
 * @retval    None
 */
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return 0;
        }

        pBuffer++;
        pBuffer1++;
    }

    return 1;
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
