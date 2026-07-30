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

/** printf function configs to USART1*/
#define DEBUG_USART  USART1

/** max number of rx buffer */
#define RS485_RXBUF_MAX_NUM   512
/** Time out for data receive */
#define RS485_RX_TIME_OUT     1000
/** Delay for RS485 Control line */
#define RS485_BUS_DELAY       1000
/* Private typedef ********************************************************/

typedef enum {FALSE, TRUE} BOOL;

/* Private variables ******************************************************/

uint8_t rxData[RS485_RXBUF_MAX_NUM];

/* Private function prototypes ********************************************/

void RS485_Init(uint32_t baud);
void RS485_ConfigTxRx(RS485_STATUS_T status);
void RS485_TxByte(uint8_t data);
void RS485_TxString(char* pStr);
void Delay(uint32_t delayTime);

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

    /** Configures LED2 and LED3 */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

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
    printf("[USART1] This is a RS485 Demo.\r\n");

    /** Config RS485 */
    RS485_Init(115200);

    /** RS485 Send data */
    RS485_ConfigTxRx(RS485_STATUS_TX);
    RS485_TxString("[USART3] This is a RS485 Demo.\r\n");
    RS485_TxString("[USART3] You can send data to this com, and MCU will send back.\r\n");
    RS485_ConfigTxRx(RS485_STATUS_RX);

    while (1)
    {
    }
}

/*!
 * @brief       Init RS485
 *
 * @param       baud : the baudrate for usart3
 *
 * @retval      None
 */
void RS485_Init(uint32_t baud)
{
    USART_Config_T usartConfigStruct;
    GPIO_Config_T gpioConfig;

    /** RS485_EN Config Rx or Tx */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOF);
    gpioConfig.mode = GPIO_MODE_OUT;
    gpioConfig.otype = GPIO_OTYPE_PP;
    gpioConfig.pin = GPIO_PIN_15;
    gpioConfig.pupd = GPIO_PUPD_NOPULL;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOF, &gpioConfig);
    GPIO_ResetBit(GPIOF, GPIO_PIN_15);

    /** USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = baud;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /** RS485 init, use COM3*/
    BOARD_COMInit(COM3, &usartConfigStruct);

    USART_EnableInterrupt(USART3, USART_INT_RXBNE);
    NVIC_EnableIRQ(USART3_IRQn);

}

/*!
 * @brief       Config RS485 to Rx or Tx.
 *
 * @param       status : Choose Send or Receive.
 *                       This parameter can be one of the following values:
 *                       @arg RS485_STATUS_TX : Send data
 *                       @arg RS485_STATUS_RX : Receive data
 *
 * @retval      None
 */
void RS485_ConfigTxRx(RS485_STATUS_T status)
{
    /** Prepare to thransfer data though RS485 */
    Delay(RS485_BUS_DELAY);

    if (status == RS485_STATUS_TX)
    {
        GPIO_SetBit(GPIOF, GPIO_PIN_15);
    }
    else
    {
        GPIO_ResetBit(GPIOF, GPIO_PIN_15);
    }
}

/*!
 * @brief       RS485 Send a byte data.
 *
 * @param       data : data to send.
 *
 * @retval      None
 */
void RS485_TxByte(uint8_t data)
{
    USART_TxData(USART3, data);

    while (!USART_ReadStatusFlag(USART3, USART_FLAG_TXBE));
}

/*!
 * @brief       RS485 Send String.
 *
 * @param       pStr : pointer to the string to send.
 *
 * @retval      None
 */
void RS485_TxString(char* pStr)
{
    while (*pStr != 0)
    {
        USART_TxData(USART3, *pStr++);

        while (!USART_ReadStatusFlag(USART3, USART_FLAG_TXBE));
    }
}

/*!
 * @brief       RS485 Reveive Interrupt handler.
 *
 * @param       None
 *
 * @retval      None
 */
void RS485_RxIsr(void)
{
    __IO uint32_t timeCnt = 0, bufIndex = 0;

    while (timeCnt < RS485_RX_TIME_OUT)
    {
        /** Keep Receive the a serie data */
        if (USART_ReadStatusFlag(USART3, USART_FLAG_RXBNE))
        {
            rxData[bufIndex++] = USART_RxData(USART3);
            timeCnt = 0;
        }
        else
        {
            timeCnt++;
        }

        /** If data more than buffer then break out */
        if (bufIndex == RS485_RXBUF_MAX_NUM)
        {
            printf("RS485 Receive buffer is full!\r\n");
            break;
        }
    }

    /** Send data */
    RS485_ConfigTxRx(RS485_STATUS_TX);

    for (uint32_t i = 0; i < bufIndex; i++)
    {
        RS485_TxByte(rxData[i]);
    }

    RS485_ConfigTxRx(RS485_STATUS_RX);
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 */
void Delay(uint32_t delayTime)
{
    __IO uint32_t i = 0;
    __IO uint32_t j = 0;

    do
    {
        i++;
        j++;
    }
    while (i < (delayTime * 6));
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
