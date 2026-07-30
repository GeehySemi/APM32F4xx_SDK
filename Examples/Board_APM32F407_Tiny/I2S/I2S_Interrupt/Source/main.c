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
#include "stdio.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf using USART1  */
#define DEBUG_USART  USART1

/* printf using USART1  */
#define BUFFERSIZE  32

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

enum
{
    FAILED,
    PASSED
};

const uint16_t I2S3_Buffer_Tx[BUFFERSIZE] =
{
    0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
    0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
    0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
    0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
    0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
    0x3D3E, 0x3F40
};

uint16_t I2S2_Buffer_Rx[BUFFERSIZE];
volatile uint32_t txCnt = 0, rxCnt = 0;

/* Private function prototypes ********************************************/

void RCM_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
void Print_Buffer(uint16_t* pBuffer, uint16_t BufferLength);

/* I2S Initialization */
void I2S_Init(void);

/* Delay */
void Delay(uint32_t count);


/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    SystemClockConfig();
    USART_Config_T usartConfigStruct;

    /* Configures LED2 and LED3 */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

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

    RCM_Configuration();
    GPIO_Configuration();
    NVIC_Configuration();
    I2S_Init();

    printf("Send Data: \r\n");
    Print_Buffer((uint16_t *)I2S3_Buffer_Tx, BUFFERSIZE);

    I2S_Enable(SPI2);
    I2S_Enable(SPI3);

    while (rxCnt < 32);

    I2S_Disable(SPI2);
    I2S_Disable(SPI3);

    printf("Recieve Data: \r\n");
    Print_Buffer((uint16_t *)I2S2_Buffer_Rx, BUFFERSIZE);

    if (Buffercmp(I2S2_Buffer_Rx, (uint16_t*)I2S3_Buffer_Tx, BUFFERSIZE) == PASSED)
    {
        BOARD_LEDOn(LED2);
        printf("Transfer OK!\r\n");
    }
    else
    {
        BOARD_LEDOff(LED2);
        printf("Transfer Fail!\r\n");
    }

    while (1)
    {
        Delay(0x2FFFFF);

        BOARD_LEDToggle(LED3);
    }
}

/*!
 * @brief     I2S Initialization
 *
 * @param     None
 *
 * @retval    None
 */
void I2S_Init(void)
{
    I2S_Config_T I2S_ConfigStruct;

    /* I2S Reset */
    SPI_I2S_Reset(SPI3);
    SPI_I2S_Reset(SPI2);

    /* Config I2S */
    I2S_ConfigStruct.standard = I2S_STANDARD_PHILLIPS;
    I2S_ConfigStruct.length = I2S_DATA_LENGHT_16BEX;
    I2S_ConfigStruct.MCLKOutput = I2S_MCLK_OUTPUT_DISABLE;
    I2S_ConfigStruct.audioDiv = I2S_AUDIO_DIV_48K;
    I2S_ConfigStruct.polarity = I2S_CLKPOL_LOW;

    I2S_ConfigStruct.mode = I2S_MODE_MASTER_TX;
    I2S_Config(SPI3, &I2S_ConfigStruct);

    I2S_ConfigStruct.mode = I2S_MODE_SLAVE_RX;
    I2S_Config(SPI2, &I2S_ConfigStruct);

    SPI_I2S_EnableInterrupt(SPI3, SPI_I2S_INT_TXBE);
    SPI_I2S_EnableInterrupt(SPI2, SPI_I2S_INT_RXBNE);
}

/*!
 * @brief     Configures the different system clocks
 *
 * @param     None
 *
 * @retval    None
 */
void RCM_Configuration(void)
{
    /* Config PLL2 for I2S */
    RCM_ConfigPLL2(50, 7);
    RCM_EnablePLL2();

    /* wait for PLL2 Ready */
    while (!RCM_ReadStatusFlag(RCM_FLAG_PLL2RDY));

    RCM_ConfigI2SCLK(RCM_I2S_CLK_PLLI2S);

    /* Enable SPI2, SPI3, GPIOA and GPIOB Periph Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOC);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_SPI2 | RCM_APB1_PERIPH_SPI3);
}

/*!
 * @brief     Configures the different GPIO ports.
 *
 * @param     None
 *
 * @retval    None
 */
void GPIO_Configuration(void)
{
    GPIO_Config_T GPIO_ConfigStruct;

    /* GPIOA And GPIOB initialization */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_13, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_3, GPIO_AF_SPI2);

    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_3, GPIO_AF_SPI3);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_5, GPIO_AF_SPI3);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_15, GPIO_AF_SPI3);

    GPIO_ConfigStructInit(&GPIO_ConfigStruct);
    GPIO_ConfigStruct.pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_3 | GPIO_PIN_5;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_15;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);
    
    GPIO_ConfigStruct.pin = GPIO_PIN_3;
    GPIO_Config(GPIOC, &GPIO_ConfigStruct);
}
/*!
 * @brief     Configure the nested vectored interrupt controller.
 *
 * @param     None
 *
 * @retval    None
 */
void NVIC_Configuration(void)
{
    NVIC_EnableIRQRequest(SPI3_IRQn, 1, 1);
    NVIC_EnableIRQRequest(SPI2_IRQn, 0, 1);
}

/*!
 * @brief     Compares two buffers.
 *
 * @param     pBuffer1:       buffer1
 *
 * @param     pBuffer2:       buffer2
 *
 * @param     BufferLength:   buffer length
 *
 * @retval    None
 */
uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
{
    uint16_t i;

    for (i = 0; i < BufferLength; i++)
    {
        if (pBuffer1[i] != pBuffer2[i])
        {
            return FAILED;
        }
    }

    return PASSED;
}

/**
 * @brief     Print Buffer Data
 *
 * @param     pBuffer:buffer
 *
 * @param     length : length of the Buffer
 *
 * @retval    None
 */
void Print_Buffer(uint16_t* pBuffer, uint16_t BufferLength)
{
    uint16_t i;

    for (i=0; i  < BufferLength; i++)
    {
        printf("0x%04X  ", pBuffer[i]);

        if ((i+1)%8 == 0)
        {
            printf("\r\n");
        }
    }
}

/*!
 * @brief     Delay
 *
 * @param     count:  delay count
 *
 * @retval    None
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;

    while (delay--);
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
