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
#include "stdio.h"
#include "apm32f4xx_hash.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

const uint8_t input[261] =
{
    0x54, 0x68, 0x65, 0x20, 0x68, 0x61, 0x73, 0x68,
    0x20, 0x70, 0x72, 0x6f, 0x63, 0x65, 0x73, 0x73,
    0x6f, 0x72, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20,
    0x66, 0x75, 0x6c, 0x6c, 0x79, 0x20, 0x63, 0x6f,
    0x6d, 0x70, 0x6c, 0x69, 0x61, 0x6e, 0x74, 0x20,
    0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e,
    0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6f,
    0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x65,
    0x63, 0x75, 0x72, 0x65, 0x20, 0x68, 0x61, 0x73,
    0x68, 0x20, 0x61, 0x6c, 0x67, 0x6f, 0x72, 0x69,
    0x74, 0x68, 0x6d, 0x20, 0x28, 0x53, 0x48, 0x41,
    0x2d, 0x31, 0x29, 0x2c, 0x20, 0x74, 0x68, 0x65,
    0x20, 0x4d, 0x44, 0x35, 0x20, 0x28, 0x6d, 0x65,
    0x73, 0x73, 0x61, 0x67, 0x65, 0x2d, 0x64, 0x69,
    0x67, 0x65, 0x73, 0x74, 0x20, 0x61, 0x6c, 0x67,
    0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x20, 0x35,
    0x29, 0x20, 0x68, 0x61, 0x73, 0x68, 0x20, 0x61,
    0x6c, 0x67, 0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d,
    0x20, 0x61, 0x6e, 0x64, 0x20, 0x74, 0x68, 0x65,
    0x20, 0x48, 0x4d, 0x41, 0x43, 0x20, 0x28, 0x6b,
    0x65, 0x79, 0x65, 0x64, 0x2d, 0x68, 0x61, 0x73,
    0x68, 0x20, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67,
    0x65, 0x20, 0x61, 0x75, 0x74, 0x68, 0x65, 0x6e,
    0x74, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e,
    0x20, 0x63, 0x6f, 0x64, 0x65, 0x29, 0x20, 0x61,
    0x6c, 0x67, 0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d,
    0x20, 0x73, 0x75, 0x69, 0x74, 0x61, 0x62, 0x6c,
    0x65, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x61, 0x20,
    0x76, 0x61, 0x72, 0x69, 0x65, 0x74, 0x79, 0x20,
    0x6f, 0x66, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69,
    0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x2e,
    0x2a, 0x2a, 0x2a, 0x20, 0x53, 0x54, 0x4d, 0x33,
    0x32, 0x20, 0x2a, 0x2a, 0x2a
};

static uint8_t sha1output[20];

/* Private function prototypes ********************************************/

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
    uint8_t i = 0;
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

    RCM_EnableAHB2PeriphClock(RCM_AHB2_PERIPH_HASH);

    /* SHA-1 Digest Computation */
    printf("\r\n");
    printf("SHA-1 Digest Computation Begin\n\r");
    HASH_ComputeSHA1((uint8_t*)input, 261, sha1output);

    /* Digest Display */

    for (i=0; i<20; i++)
    {
        if (i%4==0)
        {
            printf(" H%d = ", i/4);

        }

        printf("0x%02X  ", sha1output[i]);

        if ((i+1)%4==0)
        {
            printf("\r\n");
        }
    }

    printf("SHA-1 Digest Computation Done\r\n");

    while (1)
    {
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
