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
#include "apm32f4xx_spi.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_rcm.h"
#include "board_delay.h"
#include "log_printf.h"
#include <stdio.h>

/* Private macro **********************************************************/
#define DEBUG_USART  USART1

#define DATA_BUF_SIZE       128

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
uint16_t txDataBufSPI1[DATA_BUF_SIZE] = {0};
uint16_t rxDataBufSPI1[DATA_BUF_SIZE] = {0};
uint16_t txDataBufSPI2[DATA_BUF_SIZE] = {0};
uint16_t rxDataBufSPI2[DATA_BUF_SIZE] = {0};

/* Private function prototypes ********************************************/
void SPI_FullDuplexConfig(void);
uint8_t BufferCompare(uint16_t* buf1, uint16_t* buf2, uint8_t size);

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
    uint32_t i = 0;

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
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();
    
    SPI_FullDuplexConfig();

    /* initialization  Buffer*/
    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        txDataBufSPI1[i] = i;
        txDataBufSPI2[i] = i;
        rxDataBufSPI1[i] = 0;
        rxDataBufSPI2[i] = 0;
    }

    printf("SPI1 and SPI2 Full Duplex\r\n");

    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        /* SPI2 slave Send data */
        while (((SPI2->STS & SPI_FLAG_TXBE) != SPI_FLAG_TXBE));

        SPI_I2S_TxData(SPI2, txDataBufSPI2[i]);

        /* SPI1 master send data */
        while (((SPI1->STS & SPI_FLAG_TXBE) != SPI_FLAG_TXBE));

        SPI_I2S_TxData(SPI1, txDataBufSPI1[i]);

        /* SPI1 Master receive data */
        while (((SPI1->STS & SPI_FLAG_RXBNE) != SPI_FLAG_RXBNE));

        rxDataBufSPI1[i] = SPI_I2S_RxData(SPI1);
        /* SPI2 Slave receive data */
        while (((SPI2->STS & SPI_FLAG_RXBNE) != SPI_FLAG_RXBNE));

        rxDataBufSPI2[i] = SPI_I2S_RxData(SPI2);
    }

    if (BufferCompare(txDataBufSPI1, rxDataBufSPI2, DATA_BUF_SIZE))
    {
        BOARD_LEDOn(LED2);
    }
    else
    {
        BOARD_LEDOn(LED3);
    }

    while (1)
    {
    }
}

/**
 * @brief   SPI full duplex configuration
 *
 * @param   None
 *
 * @retval  None
 */
void SPI_FullDuplexConfig(void)
{
    GPIO_Config_T GPIO_InitStructure;
    SPI_Config_T  SPI1_InitStructure;
    SPI_Config_T  SPI2_InitStructure;

    /* Enable related Clock */
    RCM_EnableAHB1PeriphClock (RCM_AHB1_PERIPH_GPIOA| RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOC);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_SPI2);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    /* Config SPI1 PinAF */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_5, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_6, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_5, GPIO_AF_SPI1);

    /* Config SPI1 */
    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.pin = GPIO_PIN_5;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

    /* Config SPI1 */
    SPI_ConfigStructInit(&SPI1_InitStructure);
    SPI1_InitStructure.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SPI1_InitStructure.mode = SPI_MODE_MASTER;
    SPI1_InitStructure.length = SPI_DATA_LENGTH_16B;
    SPI1_InitStructure.polarity = SPI_CLKPOL_LOW;
    SPI1_InitStructure.phase = SPI_CLKPHA_2EDGE;
    SPI1_InitStructure.nss = SPI_NSS_SOFT;
    SPI1_InitStructure.baudrateDiv = SPI_BAUDRATE_DIV_256;
    SPI1_InitStructure.firstBit = SPI_FIRSTBIT_MSB;
    SPI1_InitStructure.crcPolynomial = 7;
    SPI_Config(SPI1, &SPI1_InitStructure);
    SPI_DisableCRC(SPI1);
    SPI_DisableSSOutput(SPI1);

    /* Config SPI2 PinAF */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_10, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_2, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_3, GPIO_AF_SPI2);

    /* Config SPI2, SCK=PB10, MISO=PC2, MOSI=PC3 */
    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_10;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOC, &GPIO_InitStructure);

    /* Config SPI2 */
    SPI_ConfigStructInit(&SPI2_InitStructure);
    SPI2_InitStructure.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SPI2_InitStructure.mode = SPI_MODE_SLAVE;
    SPI2_InitStructure.length = SPI_DATA_LENGTH_16B;
    SPI2_InitStructure.polarity = SPI_CLKPOL_LOW;
    SPI2_InitStructure.phase = SPI_CLKPHA_2EDGE;
    SPI2_InitStructure.nss = SPI_NSS_SOFT;
    SPI2_InitStructure.baudrateDiv = SPI_BAUDRATE_DIV_256;
    SPI2_InitStructure.firstBit = SPI_FIRSTBIT_MSB;
    SPI2_InitStructure.crcPolynomial = 7;
    SPI_Config(SPI2, &SPI2_InitStructure);
    SPI_DisableCRC(SPI2);

    /* Enable SPI  */
    SPI_Enable(SPI1);
    SPI_Enable(SPI2);
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
 * @retval      Return TRUE if buf1 = buf2. If not then return FALSE
 */
uint8_t BufferCompare(uint16_t* buf1, uint16_t* buf2, uint8_t size)
{
    uint8_t i;

    for (i = 0; i < size; i++)
    {
        if (buf1[i] != buf2[i])
        {
            return 0;
        }
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
