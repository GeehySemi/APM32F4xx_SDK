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
#include "apm32f4xx_cryp.h"
#include "stdio.h"
#include "apm32f4xx_rcm.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf using USART1  */
#define DEBUG_USART  USART1

/* if define AES_TEXT_SHORT then AES_TEXT_SIZE is 64 else AES_TEXT_SIZE is 128 */
#define AES_TEXT_SHORT

#define ECB              1
#define CBC              2
#define CTR              3
#define AESBUSY_TIMEOUT  ((uint32_t) 0x00010000)

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* key size 128 bytes */
uint8_t AES128key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                         0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
                        };

/* key size 192 bytes */
uint8_t AES192key[24] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                         0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                         0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
                        };

/* key size 256 bytes */
uint8_t AES256key[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                         0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                         0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                         0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
                        };

/* initialization vector */
uint8_t IV_1[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                   };

#ifdef AES_TEXT_SHORT
/* 16 x 4 */
#define AES_TEXT_SIZE    64
/* plaintext Size = 64 */
uint8_t Plaintext[AES_TEXT_SIZE] =
{
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

#else
/* 16 x 8 */
#define AES_TEXT_SIZE    128
/* plaintext  Size = 128 */
uint8_t Plaintext[AES_TEXT_SIZE] =
{
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
    0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09,
    0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7,
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};
#endif

/* Encrypted text */
uint8_t Encryptedtext[AES_TEXT_SIZE];

/* Decrypted text */
uint8_t Decryptedtext[AES_TEXT_SIZE];

/* Private function prototypes ********************************************/

void Print_PlainData(uint32_t length);
void Print_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t length);
void Print_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t length);

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

    /* Enable CRYP clock */
    RCM_EnableAHB2PeriphClock(RCM_AHB2_PERIPH_CRYP);

    /* Print Plain Data*/
    Print_PlainData(AES_TEXT_SIZE);

    printf("\r\n");
    printf("********************* AES mode ECB  *************************\r\n");
    /* AES mode ECB */
    /* Encryption ECB mode and Decryption ECB mode*/

    /* AES 128 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_ECB(CRYP_MODE_ENCRYPT, AES128key, 128, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(ECB, 128, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_ECB(CRYP_MODE_DECRYPT, AES128key, 128, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(ECB, 128, AES_TEXT_SIZE);
    }

    /* AES 192 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_ECB(CRYP_MODE_ENCRYPT, AES192key, 192, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(ECB, 192, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_ECB(CRYP_MODE_DECRYPT, AES192key, 192, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(ECB, 192, AES_TEXT_SIZE);
    }

    /* AES 256 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_ECB(CRYP_MODE_ENCRYPT, AES256key, 256, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(ECB, 256, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_ECB(CRYP_MODE_DECRYPT, AES256key, 256, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(ECB, 256, AES_TEXT_SIZE);
    }

    printf("\r\n");
    printf("********************* AES mode CBC  *************************\r\n");

    /* AES mode CBC */
    /* Encryption CBC mode and Decryption CBC mode*/

    /* AES 128 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_CBC(CRYP_MODE_ENCRYPT, AES128key, 128, IV_1, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(CBC, 128, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_CBC(CRYP_MODE_DECRYPT, AES128key, 128, IV_1, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(CBC, 128, AES_TEXT_SIZE);
    }

    /* AES 192 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_CBC(CRYP_MODE_ENCRYPT, AES192key, 192, IV_1, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(CBC, 192, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_CBC(CRYP_MODE_DECRYPT, AES192key, 192, IV_1, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(CBC, 192, AES_TEXT_SIZE);
    }

    /* AES 256 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_CBC(CRYP_MODE_ENCRYPT, AES256key, 256, IV_1, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(CBC, 256, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_CBC(CRYP_MODE_DECRYPT, AES256key, 256, IV_1, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(CBC, 256, AES_TEXT_SIZE);
    }

    printf("\r\n");
    printf("********************* AES mode CTR  *************************\r\n");

    CRYP_Reset();

    /*  AES mode CTR */
    /* Encryption CTR mode and Decryption CTR mode*/

    /* AES 128 */
    /* Encrypt the plaintext message*/
    if (CRYP_AES_CTR(CRYP_MODE_ENCRYPT, AES128key, 128, IV_1, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(CTR, 128, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_CTR(CRYP_MODE_DECRYPT, AES128key, 128, IV_1, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(CTR, 128, AES_TEXT_SIZE);
    }

    /* AES 192 */
    /* Encrypt the plaintext message*/
    if (CRYP_AES_CTR(CRYP_MODE_ENCRYPT, AES192key, 192, IV_1, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(CTR, 192, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_CTR(CRYP_MODE_DECRYPT, AES192key, 192, IV_1, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(CTR, 192, AES_TEXT_SIZE);
    }

    /* AES 256 */
    /* Encrypt the plaintext message*/
    CRYP_Reset();

    if (CRYP_AES_CTR(CRYP_MODE_ENCRYPT, AES256key, 256, IV_1, Plaintext, AES_TEXT_SIZE, Encryptedtext) == SUCCESS)
    {
        /* Print encrypted Data*/
        Print_EncryptedData(CTR, 256, AES_TEXT_SIZE);
    }

    /* Decrypt the plaintext message */
    CRYP_Reset();

    if (CRYP_AES_CTR(CRYP_MODE_DECRYPT, AES256key, 256, IV_1, Encryptedtext, AES_TEXT_SIZE, Decryptedtext) == SUCCESS)
    {
        /* Print decrypted data*/
        Print_DecryptedData(CTR, 256, AES_TEXT_SIZE);
    }

    while (1)
    {
    }
}

/*!
 * @brief     Print Plain Data
 *
 * @param     length: length of the data to display
 *
 * @retval    None
 */
void Print_PlainData(uint32_t length)
{
    uint16_t BufferCounter =0;
    uint32_t count = 0;

    printf("\r\n");
    printf(" *************************************************************\r\n");
    printf(" ********************* CRYP AES MODE *************************\r\n");
    printf(" *************************************************************\r\n");

    printf(" > Plain Data :\r\n");

    for (BufferCounter = 0; BufferCounter < length; BufferCounter++)
    {
        printf("[0x%02X]", Plaintext[BufferCounter]);

        count++;

        if (count == 16)
        {
            count = 0;
            printf(" BLOCK %d\r\n", BufferCounter/16);
        }
    }
}

/*!
 * @brief     Print Encrypted Data
 *
 * @param     mode: chaining mode
 *
 * @param     keysize: AES key size used
 *
 * @param     length: length of the data to display
 *
 * @retval    None
 */
void Print_EncryptedData(uint8_t mode, uint16_t keysize, uint32_t length)
{
    uint16_t BufferCounter = 0;
    uint32_t count = 0;

    printf("\r\n");
    printf(">> Encrypted Data with AES %d  Mode  ", keysize);

    if (mode == ECB)
    {
        printf("ECB\r\n");
    }
    else if (mode == CBC)
    {
        printf("CBC\r\n");
    }
    else
    {
        /* if(mode == CTR)*/
        printf("CTR\r\n");
    }

    for (BufferCounter = 0; BufferCounter < length; BufferCounter++)
    {
        printf("[0x%02X]", Encryptedtext[BufferCounter]);

        count++;

        if (count == 16)
        {
            count = 0;
            printf(" BLOCK %d\r\n", BufferCounter/16);
        }
    }
}

/*!
 * @brief     Print Decrypted Data
 *
 * @param     mode: chaining mode
 *
 * @param     keysize: AES key size used
 *
 * @param     length: length of the data to display
 *
 * @retval    None
 */
void Print_DecryptedData(uint8_t mode, uint16_t keysize, uint32_t length)
{
    uint16_t BufferCounter = 0;
    uint32_t count = 0;

    printf("\r\n");
    printf(">> Decrypted Data with AES %d  Mode  ", keysize);

    if (mode == ECB)
    {
        printf("ECB\r\n");
    }
    else if (mode == CBC)
    {
        printf("CBC\r\n");
    }
    else
    {
        /* if(mode == CTR)*/
        printf("CTR\r\n");
    }

    for (BufferCounter = 0; BufferCounter < length; BufferCounter++)
    {
        printf("[0x%02X]", Decryptedtext[BufferCounter]);
        count++;

        if (count == 16)
        {
            count = 0;
            printf(" BLOCK %d\r\n", BufferCounter/16);
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
