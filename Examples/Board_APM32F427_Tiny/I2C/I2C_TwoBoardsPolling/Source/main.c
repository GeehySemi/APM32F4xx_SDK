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
#include "apm32f4xx_i2c.h"
#include "apm32f4xx_dma.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_rcm.h"
#include "board_delay.h"
#include "log_printf.h"
#include <stdio.h>

/* Private macro **********************************************************/
#define DEBUG_USART  USART1

/* Define firmware to switch between Master board or Slave board */
#define IS_MASTER_BOARD

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/
void I2C_Init(void);
uint8_t BufferCompare(uint8_t* buf1, uint8_t* buf2, uint8_t size);

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
    uint8_t dataToSend[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
#if defined(IS_MASTER_BOARD)
#else
    uint8_t receivedData[10] = {0};
#endif /* IS_MASTER_BOARD */

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

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();

    I2C_Init();

#if defined(IS_MASTER_BOARD)
    while (BOARD_PBGetState(BUTTON_KEY2) != RESET);
    
    I2C_EnableGenerateStart(I2C1);
    while (!I2C_ReadStatusFlag(I2C1, I2C_FLAG_START)); 

    I2C_Tx7BitAddress(I2C1, 0xB0, I2C_DIRECTION_TX);
    while (!I2C_ReadStatusFlag(I2C1, I2C_FLAG_ADDR));

    (void)I2C1->STS2; 

    for (uint8_t i = 0; i < 10; i++) 
    {
        I2C_TxData(I2C1, dataToSend[i]);
        while (!I2C_ReadStatusFlag(I2C1, I2C_FLAG_BTC)); 
    }
#endif /* IS_MASTER_BOARD */

    while (1)
    {
#if defined(IS_MASTER_BOARD)
#else
        if (I2C_ReadStatusFlag(I2C1, I2C_FLAG_ADDR))
        {
            (void)I2C1->STS2;

            for (uint8_t i = 0; i < 10; i++) 
            {
                while (!I2C_ReadStatusFlag(I2C1, I2C_FLAG_RXBNE)); 
                receivedData[i] = I2C_RxData(I2C1);
            }

            I2C_EnableGenerateStop(I2C1);
            
            if (BufferCompare(receivedData, dataToSend, 10))
            {
                BOARD_LEDOn(LED2);
            }
            else
            {
                BOARD_LEDOn(LED3);
            }
        }
#endif /* IS_MASTER_BOARD */
    }
}

/*!
 * @brief   I2C Init
 *
 * @param   None
 *
 * @retval  None
 *
 */
void I2C_Init(void)
{
#if defined(IS_MASTER_BOARD)
    GPIO_Config_T gpioConfigStruct;
    I2C_Config_T i2cConfigStruct;

    /* Enable I2C related Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_I2C1);

    /* Free I2C_SCL and I2C_SDA */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF_I2C1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_7, GPIO_AF_I2C1);

    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpioConfigStruct.otype = GPIO_OTYPE_OD;
    gpioConfigStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    /* Config I2C1 */
    I2C_Reset(I2C1);
    i2cConfigStruct.mode = I2C_MODE_I2C;
    i2cConfigStruct.dutyCycle = I2C_DUTYCYCLE_2;
    i2cConfigStruct.ackAddress = I2C_ACK_ADDRESS_7BIT;
    i2cConfigStruct.ownAddress1 = 0XA0;
    i2cConfigStruct.ack = I2C_ACK_ENABLE;
    i2cConfigStruct.clockSpeed = 100000;
    I2C_Config(I2C1,&i2cConfigStruct);

    /* Enable I2Cx */
    I2C_Enable(I2C1);
#else
    GPIO_Config_T gpioConfigStruct;
    I2C_Config_T i2cConfigStruct;

    /* Enable I2C related Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_I2C1);

    /* Free I2C_SCL and I2C_SDA */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF_I2C1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_7, GPIO_AF_I2C1);

    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpioConfigStruct.otype = GPIO_OTYPE_OD;
    gpioConfigStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    /* Config I2C1 */
    I2C_Reset(I2C1);
    i2cConfigStruct.mode = I2C_MODE_I2C;
    i2cConfigStruct.dutyCycle = I2C_DUTYCYCLE_2;
    i2cConfigStruct.ackAddress = I2C_ACK_ADDRESS_7BIT;
    i2cConfigStruct.ownAddress1 = 0XB0;
    i2cConfigStruct.ack = I2C_ACK_ENABLE;
    i2cConfigStruct.clockSpeed = 100000;
    I2C_Config(I2C1,&i2cConfigStruct);

    /* Enable I2Cx */
    I2C_Enable(I2C1);
#endif /* IS_MASTER_BOARD */
}

/*!
 * @brief   Buffer Compare
 *
 * @param   buf1  Buffer1
 *
 * @param   buf2  Buffer2
 *
 * @param   size  Buffer size
 *
 * @retval  0 or 1
 *
 */
uint8_t BufferCompare(uint8_t* buf1, uint8_t* buf2, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
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
