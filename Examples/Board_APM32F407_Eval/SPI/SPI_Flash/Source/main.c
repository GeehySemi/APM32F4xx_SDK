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
#include "Board_W25Q16.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/** printf using USART1  */
#define DEBUG_USART           USART1

#define  FLASH_WRITE_ADDR     0x00000
#define  FLASH_READ_ADDR      FLASH_WRITE_ADDR

/** Buffer Size */
#define DATA_BUF_SIZE         (56)
uint16_t txDataBufSPI1[DATA_BUF_SIZE] = {0};
uint16_t rxDataBufSPI1[DATA_BUF_SIZE] = {0};

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

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

enum
{
    FALSE,
    TRUE
};

/* Private function prototypes ********************************************/

/** Delay */
void Delay(__IO uint32_t nCount);
/** Test Flash */
void FlashTest(void);
/** compare Buffer */
uint8_t BufferCompare(uint16_t* buf1, uint16_t* buf2, uint8_t size);
void Print_Buffer(uint16_t* pBuffer, uint16_t BufferLength);

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
    uint32_t DeviceID = 0;
    uint32_t FlashID = 0;

    USART_Config_T usartConfigStruct;

    /** Configures LED2 and LED3 */
    BOARD_LEDInit(LED1);
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

    printf("\r\n This is a 16M flash(W25Q16) example.\r\n");

    /** Get Flash Device ID */
    DeviceID = ReadFlashDeviceID();

    Delay( 200 );

    /** Get SPI Flash ID */
    FlashID = ReadFlashID();

    printf("\r\nFlashID is 0x%lX,  Manufacturer Device ID is 0x%lX\r\n", FlashID, DeviceID);

    /** check SPI Flash ID */
    if (FlashID == FLASH_ID)
    {
        printf("\r\nSPI FLASH W25Q16 detected!\r\n");
        printf("\r\nPush KEY1 to Read and Write FLASH W25Q16 by SPI. \r\n");

        while (1)
        {
            if (BOARD_PBGetState(BUTTON_KEY1)==0)
            {
                /** SPI1 Send Data to SPI2 test */
                FlashTest();
            }
        }
    }
    else
    {
        BOARD_LEDOn(LED3);
        printf("\r\nCan not detect W25Q128 ID!\n\r");
    }

    FlashIntoPowerDown();

    while (1)
    {

    }
}

void FlashTest(void)
{
    uint16_t i = 0;

    /** initialization  Buffer*/
    for (i=0; i<DATA_BUF_SIZE; i++)
    {
        txDataBufSPI1[i] = SPI_Data_TX[i]+0x1010;
        rxDataBufSPI1[i] = 0;
    }

    printf("\r\nSPI write Data to flash \r\n");

    /** Erase SPI FLASH Sector */
    FalshEraseSector(FLASH_WRITE_ADDR);

    /** Write data to flash */
    FlashWriteBuffer((uint8_t*)txDataBufSPI1, FLASH_WRITE_ADDR, DATA_BUF_SIZE*2);
    printf("\r\nWrite data: \r\n");
    Print_Buffer(txDataBufSPI1, DATA_BUF_SIZE);

    /** Read data from flash */
    FlashReadBuffer((uint8_t*)rxDataBufSPI1, FLASH_READ_ADDR, DATA_BUF_SIZE*2);
    printf("\r\nRead data: \r\n");
    Print_Buffer(rxDataBufSPI1, DATA_BUF_SIZE);

    /** Compare receive Buffer */
    /** Data is ok then turn on LED2 */
    if (BufferCompare(txDataBufSPI1, rxDataBufSPI1, DATA_BUF_SIZE) == TRUE)
    {
        BOARD_LEDOn(LED1);
        BOARD_LEDOff(LED2);
        printf("\r\n16M flash(W25Q16) test OK! LED1 on\n\r");
        Delay(0x5FFFFF);
    }
    else
    {
        BOARD_LEDOff(LED1);
        BOARD_LEDOn(LED2);
        printf("\r\n16M flash(W25Q16) test fail! LED2 on\n\r");
        Delay(0x4FFFFF);
    }
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
