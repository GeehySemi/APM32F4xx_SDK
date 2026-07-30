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

/* Private macro **********************************************************/

#define DATA_BUF_SIZE       (32)

/* Private typedef ********************************************************/

typedef enum {FALSE, TRUE} BOOL;

/* Private variables ******************************************************/

/** USART1 receive buffer*/
uint8_t rxDataBufUSART1[DATA_BUF_SIZE] = {0};
/** USART1 transmit buffer*/
uint8_t txDataBufUSART1[DATA_BUF_SIZE] = {0};

/** USART2 receive buffer*/
uint8_t rxDataBufUSART2[DATA_BUF_SIZE] = {0};
/** USART2 transmit buffer*/
uint8_t txDataBufUSART2[DATA_BUF_SIZE] = {0};

uint32_t rxCountUSART1 = 0;
uint32_t rxCountUSART2 = 0;

/* Private function prototypes ********************************************/

/** USART Initialization */
void USART_Init(void);
/** USART Write data */
void USART_Write(USART_T* usart,uint8_t *dat, uint32_t count);

/** Delay */
void Delay(uint32_t count);
/** Buffer compare*/
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size);

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
    BOOL state = TRUE;
    /** Init txDataBufUSART2 */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        txDataBufUSART1[i] = i;
        txDataBufUSART2[i] = i;
    }

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    USART_Init();

    /** Send Data from USART1 to USART2 */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        /** Wait until end of transmission */
        while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
        USART_TxData(USART1, txDataBufUSART1[i]);

        /** Wait until end of reception */
        while(USART_ReadStatusFlag(USART2, USART_FLAG_RXBNE) == RESET);
        rxDataBufUSART2[i] = USART_RxData(USART2);
    }

     /** Verify data */
    state = BufferCompare(rxDataBufUSART2, txDataBufUSART1, DATA_BUF_SIZE);

    /** Data is ok then turn off LED2 */
    if(state == TRUE)
    {
        BOARD_LEDOn(LED2);
    }

    /** Send Data from USART2 to USART1 */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        /** Wait until end of transmission */
        while(USART_ReadStatusFlag(USART2, USART_FLAG_TXBE) == RESET);
        USART_TxData(USART2, txDataBufUSART2[i]);

        /** Wait until end of reception */
        while(USART_ReadStatusFlag(USART1, USART_FLAG_RXBNE) == RESET);
        rxDataBufUSART1[i] = USART_RxData(USART1);
    }

     /** Verify data */
    state = BufferCompare(rxDataBufUSART1, txDataBufUSART2, DATA_BUF_SIZE);

    /** Data is ok then turn off LED2 */
    if(state == TRUE)
    {
        BOARD_LEDOn(LED3);
    }

    while(1)
    {
    }
}

/*!
 * @brief       Compares two buffers
 *
 * @param       buf1:    First buffer to be compared
 *
 * @param       buf1:    Second buffer to be compared
 *
 * @param       size:    Buffer size
 *
 * @retval      Return SET if buf1 = buf2. If not then return RESET
 */
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size)
{
    uint8_t i;

    for(i = 0; i < size; i++)
    {
        if(buf1[i] != buf2[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * @brief       USARTS Initialization
 *
 * @param       None
 *
 * @retval      None
 */
void USART_Init(void)
{
    USART_Config_T usartConfigStruct;

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;

    BOARD_COMInit(COM1, &usartConfigStruct);
    BOARD_COMInit(COM2, &usartConfigStruct);

    Delay(0x7FFF);

    /** Disable USART1 RXBNE interrput */
    USART_DisableInterrupt(USART1,USART_INT_RXBNE);
    USART_DisableInterrupt(USART2,USART_INT_RXBNE);

    /** Disable USART2 RXBNE interrput */
    USART_ClearStatusFlag(USART1, USART_FLAG_RXBNE);
    USART_ClearStatusFlag(USART2, USART_FLAG_RXBNE);
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
