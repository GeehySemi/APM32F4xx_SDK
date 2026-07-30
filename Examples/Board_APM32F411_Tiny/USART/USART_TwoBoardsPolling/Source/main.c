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
#include <stdio.h>

/* Private macro **********************************************************/
/* Define firmware to switch between 1st board or 2nd board */
#define IS_FIRST_BOARD

#define DATA_BUF_SIZE       (32)

/* Private typedef ********************************************************/
typedef enum {FALSE, TRUE} BOOL;

/* Private variables ******************************************************/
/** USART1 receive buffer*/
uint8_t rxDataBufUSART1[DATA_BUF_SIZE] = {0};
/** USART1 transmit buffer*/
uint8_t txDataBufUSART1[DATA_BUF_SIZE] = {0};

/* Private function prototypes ********************************************/
void USART1_Init(void);
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

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();

    /* Configure USART1 */
    USART1_Init();

    /* Init txDataBufUSART1 */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        txDataBufUSART1[i] = i;
    }

#if defined(IS_FIRST_BOARD)

    /* Send Data */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        /* Wait until end of transmission */
        while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
        USART_TxData(USART1, txDataBufUSART1[i]);
    }
    
    /* Receive Data */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        /* Wait until end of reception */
        while(USART_ReadStatusFlag(USART1, USART_FLAG_RXBNE) == RESET);
        rxDataBufUSART1[i] = USART_RxData(USART1);
    }

    state = BufferCompare(rxDataBufUSART1, txDataBufUSART1, DATA_BUF_SIZE);

    /* Data is ok then turn off LED3 */
    if(state == TRUE)
    {
        BOARD_LEDOn(LED3);
        while(1);
    }
#else
    /* Receive Data */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        /* Wait until end of reception */
        while(USART_ReadStatusFlag(USART1, USART_FLAG_RXBNE) == RESET);
        rxDataBufUSART1[i] = USART_RxData(USART1);
    }
    
    state = BufferCompare(rxDataBufUSART1, txDataBufUSART1, DATA_BUF_SIZE);

    /* Data is ok then turn off LED3 */
    if(state == TRUE)
    {
        BOARD_LEDOn(LED3);
        while(1);
    }
    
    /* Send Data */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        /* Wait until end of transmission */
        while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
        USART_TxData(USART1, txDataBufUSART1[i]);
    }
#endif /* IS_FIRST_BOARD */


    while (1)
    {
        BOARD_LEDToggle(LED2);
        BOARD_DelayMs(500U);
    }
}

/*!
 * @brief       USART1 configuration
 *
 * @param       None
 *
 * @retval      None
 */
void USART1_Init(void)
{
    USART_Config_T USART_ConfigStruct;
    GPIO_Config_T GPIO_ConfigStruct;

    /* Enable GPIO clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* Enable USART1 clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);

    /* Configure the UART TX and RX pin */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_USART1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_USART1);

    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* USART configuration */
    USART_ConfigStructInit(&USART_ConfigStruct);
    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.mode = USART_MODE_TX_RX;
    USART_ConfigStruct.parity = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
    USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_Config(USART1, &USART_ConfigStruct);

    /* Enable USART */
    USART_Enable(USART1);
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
