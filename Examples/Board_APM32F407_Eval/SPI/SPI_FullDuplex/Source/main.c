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
#include "board.h"
#include "stdio.h"
#include "apm32f4xx_spi.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/** printf using USART1  */
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

enum
{
    FALSE,
    TRUE
};

/** Buffer Size */
#define DATA_BUF_SIZE       (56)
uint16_t txDataBufSPI1[DATA_BUF_SIZE] = {0};
uint16_t rxDataBufSPI1[DATA_BUF_SIZE] = {0};
uint16_t txDataBufSPI2[DATA_BUF_SIZE] = {0};
uint16_t rxDataBufSPI2[DATA_BUF_SIZE] = {0};

/** SPI TX Buffer*/
const uint16_t SPI_Data_TX[DATA_BUF_SIZE] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0X07,
};

/* Private function prototypes ********************************************/

/** Delay */
void Delay(__IO uint32_t nCount);
/** SPI Initialization */
void SPI_Init(void);
/** compare Buffer */
uint8_t BufferCompare(uint16_t* buf1, uint16_t* buf2, uint8_t size);
void Print_Buffer(uint16_t* pBuffer, uint16_t BufferLength);

void SendDataTest(void);
void FullDuplexTest(void);

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

    /** KEY init*/
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /** SPI init*/
    SPI_Init();

    printf("This is an SPI full-duplex communication example.\r\n");
    printf("KEY1: SendDataTest!\r\n");
    printf("KEY2: FullDuplexTest!\r\n");

    while (1)
    {
        if (BOARD_PBGetState(BUTTON_KEY1)==0)
        {
            /** SPI1 Send Data to SPI2 test */
            SendDataTest();
            printf("KEY1: SendDataTest!\r\n");
            printf("KEY2: FullDuplexTest!\r\n");
        }

        if (BOARD_PBGetState(BUTTON_KEY2)==0)
        {
            /** SPI1 and SPI2 Full Duplex Test */
            FullDuplexTest();
            printf("KEY1: SendDataTest!\r\n");
            printf("KEY2: FullDuplexTest!\r\n");
        }
    }
}

/*!
 * @brief       SPI Initialization
 *
 * @param       None
 *
 * @retval      None
 */
void SPI_Init(void)
{
    GPIO_Config_T GPIO_InitStructure;
    SPI_Config_T  SPI1_InitStructure;
    SPI_Config_T  SPI2_InitStructure;

    /** Enable related Clock */
    RCM_EnableAHB1PeriphClock (RCM_AHB1_PERIPH_GPIOA| RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_SPI2);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    /** Config SPI1 PinAF */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_4, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_5, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_6, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_SPI1);

    /** Config SPI1, PIN NSS=PA4, SCK=PA5, MISO=PA6, MOSI=PA7 */
    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOA, &GPIO_InitStructure);

    /** Config SPI1 */
    SPI_ConfigStructInit(&SPI1_InitStructure);
    SPI1_InitStructure.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SPI1_InitStructure.mode = SPI_MODE_MASTER;
    SPI1_InitStructure.length = SPI_DATA_LENGTH_16B;
    SPI1_InitStructure.polarity = SPI_CLKPOL_LOW;
    SPI1_InitStructure.phase = SPI_CLKPHA_2EDGE;
    SPI1_InitStructure.nss = SPI_NSS_HARD;
    SPI1_InitStructure.baudrateDiv = SPI_BAUDRATE_DIV_256;
    SPI1_InitStructure.firstBit = SPI_FIRSTBIT_MSB;
    SPI1_InitStructure.crcPolynomial = 7;
    SPI_Config(SPI1, &SPI1_InitStructure);
    SPI_DisableCRC(SPI1);
    SPI_EnableSSOutput(SPI1);

    /** Config SPI2 PinAF */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_13, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_14, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_15, GPIO_AF_SPI2);

    /** Config SPI2, PIN NSS=PB12, SCK=PB13, MISO=PB14, MOSI=PB15 */
    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

    /** Config SPI2 */
    SPI_ConfigStructInit(&SPI2_InitStructure);
    SPI2_InitStructure.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SPI2_InitStructure.mode = SPI_MODE_SLAVE;
    SPI2_InitStructure.length = SPI_DATA_LENGTH_16B;
    SPI2_InitStructure.polarity = SPI_CLKPOL_LOW;
    SPI2_InitStructure.phase = SPI_CLKPHA_2EDGE;
    SPI2_InitStructure.nss = SPI_NSS_HARD;
    SPI2_InitStructure.baudrateDiv = SPI_BAUDRATE_DIV_256;
    SPI2_InitStructure.firstBit = SPI_FIRSTBIT_MSB;
    SPI2_InitStructure.crcPolynomial = 7;
    SPI_Config(SPI2, &SPI2_InitStructure);
    SPI_DisableCRC(SPI2);

    /** Enable SPI  */
    SPI_Enable(SPI1);
    SPI_Enable(SPI2);
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

    while (delay--);
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
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * @brief       Print Buffer Data
 *
 * @param       pBuffer:buffer
 *
 * @param       length : length of the Buffer
 *
 * @retval      None
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
 * @brief       SPI1 Send Data to SPI2 test
 *
 * @param       None
 *
 * @retval      None
 */
void SendDataTest(void)
{
    uint16_t i = 0;

    /** initialization  Buffer*/
    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        txDataBufSPI1[i] = SPI_Data_TX[i]+0x1010;
        rxDataBufSPI2[i] = 0;
    }

    printf("SPI1 Send Data to SPI2\r\n");

    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        /** SPI1 master send data */
        while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE) == RESET);

        SPI_I2S_TxData(SPI1, txDataBufSPI1[i]);

        /** SPI2 slave receive data */
        while (SPI_I2S_ReadStatusFlag(SPI2, SPI_FLAG_RXBNE) == RESET);

        rxDataBufSPI2[i] = SPI_I2S_RxData(SPI2);
    }

    /** Compare receive Buffer */
    /** Data is ok then turn on LED2 */
    if (BufferCompare(txDataBufSPI1, rxDataBufSPI2, DATA_BUF_SIZE) == TRUE)
    {
        BOARD_LEDOn(LED2);
        printf("SPI1 Send Data :\r\n");
        Print_Buffer(txDataBufSPI1, DATA_BUF_SIZE);

        printf("SPI2 Recieve Data :\r\n");
        Print_Buffer(rxDataBufSPI2, DATA_BUF_SIZE);
        printf("Transfer OK!\r\n");

        if (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_OVR) == SET)
        {
            i = SPI_I2S_RxData(SPI1);
            SPI_I2S_ClearStatusFlag(SPI1, SPI_FLAG_OVR);
        }

        Delay(0x5FFFFF);
    }
    else
    {
        BOARD_LEDToggle(LED2);
        Delay(0x4FFFFF);
        BOARD_LEDToggle(LED2);
        printf("Error! \r\n");
    }
}

/*!
 * @brief       SPI1 and SPI2 Full Duplex Test
 *
 * @param       None
 *
 * @retval      None
 */
void FullDuplexTest(void)
{
    uint16_t i = 0;

    /** initialization  Buffer*/
    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        txDataBufSPI1[i] = SPI_Data_TX[i]+0x1010;
        txDataBufSPI2[i] = SPI_Data_TX[i]+0x1010;
        rxDataBufSPI1[i] = 0;
        rxDataBufSPI2[i] = 0;
    }

    printf("SPI1 and SPI2 Full Duplex\r\n");

    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        /** SPI1 master send data */
        while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE) == RESET);

        SPI_I2S_TxData(SPI1, txDataBufSPI1[i]);

        /** SPI2 slave Send data */
        while (SPI_I2S_ReadStatusFlag(SPI2, SPI_FLAG_TXBE) == RESET);

        SPI_I2S_TxData(SPI2, txDataBufSPI2[i]);

        /** SPI2 Slave receive data */
        while (SPI_I2S_ReadStatusFlag(SPI2, SPI_FLAG_RXBNE) == RESET);

        rxDataBufSPI2[i] = SPI_I2S_RxData(SPI2);

        /** SPI1 Master receive data */
        while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE) == RESET);

        rxDataBufSPI1[i] = SPI_I2S_RxData(SPI1);

    }

    /** Compare SPI1 recieve and SPI2 recieve Buffer */
    /** Data is ok then turn on LED3 */
    if (BufferCompare(rxDataBufSPI1, rxDataBufSPI2, DATA_BUF_SIZE) == TRUE)
    {
        BOARD_LEDOn(LED3);

        printf("SPI1 Recieve Data :\r\n");
        Print_Buffer(rxDataBufSPI1, DATA_BUF_SIZE);

        printf("SPI2 Recieve Data :\r\n");
        Print_Buffer(rxDataBufSPI2, DATA_BUF_SIZE);

        printf("Transfer OK!\r\n");
        Delay(0x5FFFFF);
    }
    else
    {
        BOARD_LEDToggle(LED3);
        Delay(0x4FFFFF);
        BOARD_LEDToggle(LED3);
        printf("Transfer Fail!\r\n");
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
