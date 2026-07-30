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
#include "stdarg.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

#define TEMPERATURE_THYS    31
#define TEMPERATURE_TOS     32

/* Waiting TIMEOUT*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x500)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

void Delay(void);
void USART_Write(uint8_t* dat, uint8_t len);

/* I2C Init */
void I2CInit(void);
/* I2C Write */
uint8_t I2CWrite(char* pBuffer);
void Uartx_Printf(USART_T* USARTx, char* fmt, ...);
void Uartx_SendString(USART_T* USARTx, char* pt);

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

    /* Configures LED3 */
    BOARD_LEDInit(LED3);

    /* Configures KEY1 */
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);

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

    I2CInit();

    printf("\r\nI am I2C Slave!\r\n");

    while (1)
    {
        BOARD_LEDToggle(LED3);
        Delay();
    }
}

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
void Delay(void)
{
    volatile uint32_t delay = 0x2FFFFF;

    while (delay--);
}

/*!
 * @brief     USART write
 *
 * @param     dat:    data
 *
 * @param     len:    Data length
 *
 * @retval    None
 */
void USART_Write(uint8_t* dat, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < len; i++)
    {
        while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

        USART_TxData(USART1, dat[i]);
    }
}

/*!
 * @brief     I2C Init
 *
 * @param     None
 *
 * @retval    None
 */
void I2CInit(void)
{
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

    /*  Config I2C1 */
    I2C_Reset(I2C1);
    i2cConfigStruct.mode = I2C_MODE_I2C;
    i2cConfigStruct.dutyCycle = I2C_DUTYCYCLE_2;
    i2cConfigStruct.ackAddress = I2C_ACK_ADDRESS_7BIT;
    i2cConfigStruct.ownAddress1 = 0XB0;
    i2cConfigStruct.ack = I2C_ACK_ENABLE;
    i2cConfigStruct.clockSpeed = 100000;
    I2C_Config(I2C1, &i2cConfigStruct);

    /* NVIC coniguration */
    NVIC_EnableIRQRequest(I2C1_EV_IRQn, 1, 0);
    I2C_DisableDualAddress(I2C1);

    /* Enable the I2C1 Interrupt */
    I2C_EnableInterrupt(I2C1, I2C_INT_EVT);

    /* Enable I2Cx */
    I2C_Enable(I2C1);
}

/*!
 * @brief     I2C interrupt service routine
 *
 * @param     None
 *
 * @retval    None
 */
void  I2C_Isr(void)
{
    uint8_t dat;

    if (I2C_ReadIntFlag(I2C1, I2C_INT_FLAG_ADDR) == SET)
    {
        /* Reading STS2  before clears the ADDR flag, even if the ADDR flag was set after reading I2C_STS1 */
        *(__IO uint16_t*)I2C1->STS2;
        I2C_ClearIntFlag(I2C1, I2C_INT_FLAG_ADDR);
    }

    if (I2C_ReadStatusFlag(I2C1, I2C_FLAG_RXBNE) == SET)
    {
        dat = I2C_RxData(I2C1);
        USART_Write(&dat, 1);
    }

    if (I2C_ReadIntFlag(I2C1, I2C_INT_FLAG_STOP) == SET)
    {
        I2C_Enable(I2C1);
    }
}

/*!
 * @brief     Used to locate the wrong location of the I2C_Write()
 *
 * @param     errorCode
 *
 * @retval    Error code location information
 */
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    printf("I2C Wait TIMEOUT! errorCode = %d\r\n", errorCode);
    return 0;
}

/*!
 * @brief     Write data to the I2C1
 *
 * @param     pBuffer: wiret buffer
 *
 * @retval    0: Error  1:Succee
 */
uint8_t I2CWrite(char* pBuffer)
{
    uint16_t I2CTimeout = I2CT_LONG_TIMEOUT;

    while (I2C_ReadStatusFlag(I2C1, I2C_FLAG_BUSBSY))
    {
        I2CInit();

        if ((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(4);
        }
    }

    I2C_DisableInterrupt(I2C1, I2C_INT_EVT);

    /* Send START condition */
    I2C_EnableGenerateStart(I2C1);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    while (!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  //EV5
    {
        if ((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(5);
        }
    }

    /* Send address for write */
    I2C_Tx7BitAddress(I2C1, 0xA0, I2C_DIRECTION_TX);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    while (!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  //EV6
    {
        if ((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(6);
        }
    }

    /* While there is data to be written */
    while (*pBuffer != '\0')
    {
        I2CTimeout = I2CT_LONG_TIMEOUT;

        /* Send the current byte */
        I2C_TxData(I2C1, *pBuffer);

        if (*(pBuffer+1)!='\0')
        {
            while (!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))   //EV8
            {
                if ((I2CTimeout--) == 0)
                {
                    return I2C_TIMEOUT_UserCallback(8);
                }
            }
        }
        else
        {
            while (!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))   //EV8-2
            {
                if ((I2CTimeout--) == 0)
                {
                    return I2C_TIMEOUT_UserCallback(9);
                }
            }

            I2C_EnableGenerateStop(I2C1);
        }

        /* Point to the next byte to be written */
        pBuffer++;
    }

    I2C_EnableInterrupt(I2C1, I2C_INT_EVT);
    return 1;
}

/*!
 * @brief     BUTTON_KEY interrupt service routine
 *
 * @param     None
 *
 * @retval    None
 *
 * @note      This function need to put into EINT1_IRQHandler
 *            in apm32f4xx_int.c
 */
void APM_MINI_PB_I2C_Isr()
{
    if (EINT_ReadStatusFlag(EINT_LINE_1)==SET)
    {
        I2CWrite("Hello master\r\n");
        EINT_ClearStatusFlag(EINT_LINE_1);
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
