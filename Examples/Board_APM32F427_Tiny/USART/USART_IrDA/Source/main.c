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
#define DEBUG_USART    USART1

/* Private typedef ********************************************************/
typedef enum {FALSE, TRUE} BOOL;

/* Private variables ******************************************************/
uint8_t USART2ReceivedData = 0x00;
uint8_t USART2SendData = 0x00;

uint8_t USART3ReceivedData = 0x00;
uint8_t USART3SendData = 0xFF;

/* Private function prototypes ********************************************/
/** USART IrDA Init */
void USART_IrDAInit(void);
/** Buffer compare*/
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size);

void IrDA_USART2_TxData(uint8_t data);
uint8_t IrDA_USART2_RxData(void);

void IrDA_USART3_TxData(uint8_t data);
uint8_t IrDA_USART3_RxData(void);

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

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /* COM1 init */
    BOARD_COMInit(COM1, &usartConfigStruct);

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();

    /* Init USART IrDA mode*/
    USART_IrDAInit();

    printf("\r\nThis example provides a basic communication USARTx IrDA receive and transmit mode.\r\n");

    while (1)
    {
        BOARD_LEDToggle(LED2);
        BOARD_DelayMs(500);

        IrDA_USART2_TxData(USART2SendData);
        USART3ReceivedData = IrDA_USART3_RxData();
        printf(">> USART2 Send Data : 0x%02X ,USART3 Received Data : 0x%02X. \r\n", USART2SendData, USART3ReceivedData);

        IrDA_USART3_TxData(USART3SendData);
        USART2ReceivedData = IrDA_USART2_RxData();
        printf(">> USART3 Send Data : 0x%02X ,USART2 Received Data : 0x%02X. \r\n", USART3SendData, USART2ReceivedData);

        USART2SendData++;
        USART3SendData--;
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
 * @brief       USARTS IrDA Initialization
 *
 * @param       None
 *
 * @retval      None
 */
void USART_IrDAInit(void)
{
    USART_Config_T usartConfigStruct;
    GPIO_Config_T gpioConfigStruct;
    
    /* System Clocks Configuration */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOC);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART2);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART3);
    /* Configure the GPIO ports */
    /* USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    gpioConfigStruct.pin = GPIO_PIN_2;
    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &gpioConfigStruct);
    
    gpioConfigStruct.pin = GPIO_PIN_3;
    GPIO_Config(GPIOA, &gpioConfigStruct);
    
    /* Connect PA2 to USART2_Tx */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_USART2);

    /* Connect PA3 to USART2_Rx */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_USART2);

    /* USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    gpioConfigStruct.pin = GPIO_PIN_10;
    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &gpioConfigStruct);
    
    gpioConfigStruct.pin = GPIO_PIN_11;
    GPIO_Config(GPIOB, &gpioConfigStruct);
    
    /* Connect PB10 to USART3_Tx */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_10, GPIO_AF_USART3);

    /* Connect PB11 to USART3_Rx */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_11, GPIO_AF_USART3);

    /* USARTx configuration */
    /* USARTx configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    
    /* Configure the USARTx */
    USART_Config(USART2, &usartConfigStruct);
    USART_Config(USART3, &usartConfigStruct);
    
    /* Enable the USARTx */
    USART_Enable(USART2);
    USART_Enable(USART3);
    
    /* Set the USARTx prescaler */
    USART_ConfigPrescaler(USART2, 0x01);
    USART_ConfigPrescaler(USART3, 0x01);

    /* Configure the USARTx IrDA mode */
    USART_ConfigIrDA(USART2, USART_IRDALP_NORMAL);
    USART_ConfigIrDA(USART3, USART_IRDALP_NORMAL);

    /* Enable the USARTx IrDA mode */
    USART_EnableIrDA(USART2);
    USART_EnableIrDA(USART3);
}

/*!
 * @brief       USART2 Sends data function in IrDA mode
 *
 * @param       data : The sent data.
 *
 * @retval      None
 *
 * @note
 */
void IrDA_USART2_TxData(uint8_t data)
{
    USART_TxData(USART2, data);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART2, USART_FLAG_TXBE) == RESET);
}

/*!
 * @brief       USART3 Receive data function in IrDA mode.
 *
 * @param       None
 *
 * @retval      The received data.
 *
 * @note
 */
uint8_t IrDA_USART2_RxData(void)
{
    while (USART_ReadStatusFlag(USART2, USART_FLAG_RXBNE) == RESET);

    return (USART_RxData(USART2));
}

/*!
 * @brief       USART3 Sends data function in IrDA mode.
 *
 * @param       data : The sent data.
 *
 * @retval      None
 *
 * @note
 */
void IrDA_USART3_TxData(uint8_t data)
{
    USART_TxData(USART3, data);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART3, USART_FLAG_TXBE) == RESET);
}

/*!
 * @brief       USART3 Receive data function in IrDA mode.
 *
 * @param       None
 *
 * @retval      The received data.
 *
 * @note
 */
uint8_t IrDA_USART3_RxData(void)
{
    while (USART_ReadStatusFlag(USART3, USART_FLAG_RXBNE) == RESET);

    return (USART_RxData(USART3));
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
