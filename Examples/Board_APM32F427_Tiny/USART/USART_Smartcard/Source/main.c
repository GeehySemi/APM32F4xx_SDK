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
#include "log_printf.h"
#include <stdio.h>

/* Private macro **********************************************************/
#define DEBUG_USART  USART1

#define Smartcard_T0_Protocol       0x00    /* T0 PROTOCOL */
#define Smartcard_Setup_Lenght      20
#define Smartcard_Hist_Lenght       20
#define Smartcard_Receive_Timeout   0x4000  /* direction to reader */

/* Private typedef ********************************************************/
/**
 * @brief    Smartcard ATR structure definition
 */
typedef  struct
{
  uint8_t TS;                           /* Bit Convention */
  uint8_t T0;                           /* High Nibble = N. of setup byte; low nibble = N. of historical byte */
  uint8_t T[Smartcard_Setup_Lenght];    /* Setup array */
  uint8_t H[Smartcard_Hist_Lenght];     /* Historical array */
  uint8_t Tlenght;                      /* Setup array dimension */
  uint8_t Hlenght;                      /* Historical array dimension */
} Smartcard_ATR_T;

/* Private variables ******************************************************/
volatile uint32_t tick = 0;
Smartcard_ATR_T  Smartcard_A2RStruct;

__IO uint8_t ReceivedBuffer[50]=
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Private function prototypes ********************************************/
uint8_t Smartcard_decode_Answer2reset(__IO uint8_t* cardBuff);

/** USART SmartCard Init */
void USART_SmartCardInit(void);

uint32_t SmartcardTest(void);

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

    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Configure Delay */
    BOARD_DelayConfig();

    USART_SmartCardInit();

    printf("\r\nThis example provides a basic communication USART3 Smartcard mode.\r\n");
    printf(">> SystemCoreClock: %ld\r\n",SystemCoreClock);

    if (SmartcardTest() == SUCCESS)
    {
        BOARD_LEDOn(LED3);
        printf(">> Read Smartcard ATR response passed.\r\n");
    }
    else
    {
        BOARD_LEDOff(LED3);
        printf(">> Read Smartcard ATR response failed.\r\n");
    }

    while (1)
    {
        BOARD_DelayMs(500);
        BOARD_LEDToggle(LED2);
    }
}

/*!
 * @brief       Test the inserted card is ISO7816-3 T=0
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
uint32_t SmartcardTest(void)
{
    uint32_t Counter = 0;

    /* Set Smartcard Reset Pin to Low */
    GPIO_ResetBit(GPIOB, GPIO_PIN_11);

    /* Set Smartcard Reset Pin to High */
    GPIO_SetBit(GPIOB, GPIO_PIN_11);

    BOARD_DelayMs(100);

    /* Read Smartcard ATR response */
    for (uint16_t i = 0; i < 40; i++, Counter = 0)
    {
        while ((USART_ReadStatusFlag(USART3, USART_FLAG_RXBNE) == RESET) \
               && (Counter != Smartcard_Receive_Timeout))
        {
            Counter++;
        }

        if (Counter != Smartcard_Receive_Timeout)
        {
            ReceivedBuffer[i] = USART_RxData(USART3);
        }
    }

    Counter =0;

    for (uint16_t i = 0; i < 40; i++)
    {
        if ( ReceivedBuffer[i] == 0)
        {
            Counter ++;
        }
    }

    if (Counter == 40)
    {
        /* The data received by the Smartcard is abnormal */
        return ERROR;
    }

    /* Decode ATR, Test if the inserted card is ISO7816-3 T=0 compatible */
    if (Smartcard_decode_Answer2reset(ReceivedBuffer) == 0)
    {
        /* Inserted card is ISO7816-3 T=0 compatible */
        return SUCCESS;
    }
    else
    {
        /* Inserted Smartcard is not ISO7816-3 T=0 compatible */
        return ERROR;
    }
}

/*!
 * @brief       USART3 Smartcard mode initializes functions
 *
 * @param       None
 *
 * @retval      None
 *
 */
void USART_SmartCardInit(void)
{
    USART_Config_T usartConfigStruct;
    USART_ClockConfig_T usartClockConfigStruct;
    GPIO_Config_T gpioConfigStruct;
    
    /* System Clocks Configuration */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART3);
    
    /* USART3 GPIO Configuration
       PB10 (USART3_TX) ---> Smartcard Tx Pin
       PB11 (GPIO)      ---> Smartcard Reset Pin
       PB12 (USART3_CK) ---> Smartcard CK Pin
    */
    gpioConfigStruct.pin = GPIO_PIN_10;
    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.otype = GPIO_OTYPE_OD;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &gpioConfigStruct);
    
    gpioConfigStruct.pin = GPIO_PIN_11;
    gpioConfigStruct.mode = GPIO_MODE_OUT;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_Config(GPIOB, &gpioConfigStruct);
    
    gpioConfigStruct.pin = GPIO_PIN_12;
    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_Config(GPIOB, &gpioConfigStruct);
    
    /* Connect PB10 to USART3_Tx */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_10, GPIO_AF_USART3);

    /* Connect PB11 to USART3_Rx */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_USART3);
    
    /* Set Smartcard Reset Pin to High */
    GPIO_SetBit(GPIOB, GPIO_PIN_11);
    
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_1);
    NVIC_EnableIRQRequest(USART3_IRQn, 1, 0);
    
    /* Smartcard USART (USART3) configuration */
    /* Smartcard USART (USART3) configured as follow:
          - Word Length = 9 Bits
          - 1.5 Stop Bit
          - Even parity
          - BaudRate = 9677 baud
          - Hardware flow control disabled (RTS and CTS signals)
          - Tx and Rx enabled
          - USART Clock enabled
          - USART CPOL Low
          - USART CPHA on first edge
          - USART Last Bit Clock Enabled
    */

    /* Smartcard USART (USART3) Clock set to 4.5MHz (PCLK1 = 36 MHZ / 10) */
    USART_ConfigPrescaler(USART3, 0x05);
    /* Smartcard USART (USART3) Guard Time set to 16 Bit */
    USART_ConfigGuardTime(USART3, 16);

    usartClockConfigStruct.clock = USART_CLKEN_ENABLE;
    usartClockConfigStruct.polarity = USART_CLKPOL_LOW;
    usartClockConfigStruct.phase = USART_CLKPHA_1EDGE;
    usartClockConfigStruct.lastBit = USART_LBCP_ENABLE;
    USART_ConfigClock(USART3, &usartClockConfigStruct);

    usartConfigStruct.baudRate = 9677;
    usartConfigStruct.wordLength = USART_WORD_LEN_9B;
    usartConfigStruct.stopBits = USART_STOP_BIT_1_5;
    usartConfigStruct.parity = USART_PARITY_EVEN;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_Config(USART3, &usartConfigStruct);

    /* Enable the Smartcard USART (USART3) Parity Error Interrupt */
    USART_EnableInterrupt(USART3, USART_INT_PE);

    /* Enable Smartcard USART (USART3) */
    USART_Enable(USART3);

    /* Enable the NACK Transmission */
    USART_EnableSmartCardNACK(USART3);

    /* Enable the Smartcard Interface */
    USART_EnableSmartCard(USART3);
}

/*!
 * @brief       Decode the Card ATR Response.
 *
 * @param       card: pointer to the buffer containing the Card ATR.
 *
 * @retval      Card protocol
 *
 */
uint8_t Smartcard_decode_Answer2reset(__IO uint8_t* cardBuff)
{
    uint32_t i = 0, flag = 0, buf = 0, protocol = 0;

    Smartcard_A2RStruct.TS = 0;
    Smartcard_A2RStruct.T0 = 0;

    for (i = 0; i < Smartcard_Setup_Lenght; i++)
    {
        Smartcard_A2RStruct.T[i] = 0;
    }

    for (i = 0; i < Smartcard_Hist_Lenght; i++)
    {
        Smartcard_A2RStruct.H[i] = 0;
    }

    Smartcard_A2RStruct.Tlenght = 0;
    Smartcard_A2RStruct.Hlenght = 0;

    Smartcard_A2RStruct.TS = cardBuff[0]; /* INITIAL CHARACTER */
    Smartcard_A2RStruct.T0 = cardBuff[1]; /* FORMAT CHARACTER */

    Smartcard_A2RStruct.Hlenght = Smartcard_A2RStruct.T0 & 0x0F;

    if ((Smartcard_A2RStruct.T0 & 0x80) == 0x80)
    {
        flag = 1;
    }

    for (i = 0; i < 4; i++)
    {
        Smartcard_A2RStruct.Tlenght = Smartcard_A2RStruct.Tlenght + \
                                      (((Smartcard_A2RStruct.T0 & 0xF0) >> (4 + i)) & 0x1);
    }

    for (i = 0; i < Smartcard_A2RStruct.Tlenght; i++)
    {
        Smartcard_A2RStruct.T[i] = cardBuff[i + 2];
    }

    protocol = Smartcard_A2RStruct.T[Smartcard_A2RStruct.Tlenght - 1] & 0x0F;

    while (flag)
    {
        if ((Smartcard_A2RStruct.T[Smartcard_A2RStruct.Tlenght-1] & 0x80)== 0x80)
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }

        buf = Smartcard_A2RStruct.Tlenght;
        Smartcard_A2RStruct.Tlenght = 0;

        for (i = 0; i < 4; i++)
        {
            Smartcard_A2RStruct.Tlenght = Smartcard_A2RStruct.Tlenght + \
                                          (((Smartcard_A2RStruct.T[buf - 1] & 0xF0) >> (4 + i)) & 0x1);
        }

        for (i = 0; i < Smartcard_A2RStruct.Tlenght; i++)
        {
            Smartcard_A2RStruct.T[buf + i] = cardBuff[i + 2 + buf];
        }

        Smartcard_A2RStruct.Tlenght += buf;
    }

    for (i = 0; i < Smartcard_A2RStruct.Hlenght; i++)
    {
        Smartcard_A2RStruct.H[i] = cardBuff[i + 2 + Smartcard_A2RStruct.Tlenght];
    }

    return ((uint8_t)protocol);
}

/*!
 * @brief       This function handles Smartcard USART global interrupt request.
 *
 * @param       None
 *
 * @retval      None
 *
 */
void USART3_IRQRequest(void)
{
    /* If the SC_USART detects a parity error */
    if (USART_ReadIntFlag(USART3, USART_INT_PE) != RESET)
    {
        /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
        USART_EnableInterrupt(USART3, USART_INT_RXBNE);
        /* Flush the SC_USART DR register */
        USART_RxData(USART3);
    }

    if (USART_ReadIntFlag(USART3, USART_INT_RXBNE) != RESET)
    {
        /* Disable SC_USART RXNE Interrupt */
        USART_DisableInterrupt(USART3, USART_INT_RXBNE);
        USART_RxData(USART3);
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
int _write(int file, char *ptr, int len)
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
