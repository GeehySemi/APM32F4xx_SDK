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
#include "apm32f4xx_can.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* You can choose the way as polling or interrupt */
#define CAN_TEST_POLLING
#define CAN_TEST_INT

#define TIME1 CAN_TIME_SEGMENT1_2
#define TIME2 CAN_TIME_SEGMENT2_3
#define PRES  14

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

volatile uint8_t intFlag = 0;

volatile uint8_t intFlag1 = FAILED;
volatile uint8_t intFlag2 = FAILED;

/* Private function prototypes ********************************************/

void CAN_Init(uint8_t baud);
void CAN_GPIOInit(void);
void NVIC_Configuration(void);
uint8_t CAN_TestPolling(void);
uint8_t CAN_TestInterrupt(void);
void CAN_Delay(uint32_t cnt);
void CAN_TxMessageInit(CAN_TxMessage_T *txMessage);
void CAN_RxMessageInit(CAN_RxMessage_T *rxMessage);

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
    uint8_t state = 0;
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

#if defined(CAN_TEST_POLLING)
    printf(" ********** CAN Polling Test ********** \r\n");
    CAN_Init(CAN_BAUD_125K);

    /* Test polling */
    if (CAN_TestPolling() == PASSED)
    {
        state = 0x01;
        printf("> CAN polling test passed !\r\n");
    }
    else
    {
        state = 0x00;
        printf("> CAN polling test failed !\r\n");
    }
#endif

#if defined(CAN_TEST_INT)
    printf(" ********** CAN Interrupt Test ********** \r\n");
    CAN_Init(CAN_BAUD_500K);
    /* CAN FIFO0 message pending interrupt enable */
    CAN_EnableInterrupt(CAN1, CAN_INT_F0MP);
    CAN_EnableInterrupt(CAN2, CAN_INT_F0MP);
    NVIC_Configuration();

    /* Test interrupt */
    if (CAN_TestInterrupt() == PASSED)
    {
        state |= 0x02;
        printf("> CAN interrupt test passed !\r\n");
    }
    else
    {
        printf("> CAN interrupt test failed !\r\n");
    }
#endif

    while (1)
    {
        switch (state)
        {
            /* CAN polling and interrupt test are Fail */
            case 0x00:
                BOARD_LEDToggle(LED2);
                BOARD_LEDToggle(LED3);
                break;
            /* CAN polling is OK, but interrupt is Fail */
            case 0x01:
                BOARD_LEDOn(LED2);
                BOARD_LEDToggle(LED3);
                break;
            /* CAN polling is Fail, but interrupt is OK */
            case 0x02:
                BOARD_LEDToggle(LED2);
                BOARD_LEDOn(LED3);
                break;
            /* CAN polling and interrupt OK */
            case 0x03:
                BOARD_LEDOn(LED2);
                BOARD_LEDOn(LED3);
                break;
            default :
                break;
        }
        CAN_Delay(0x7FFFFF);
    }
}

/*!
 * @brief     Configures the CAN, transmit and receive by polling
 *
 * @param     None
 *
 * @retval    PASSED if the reception is well done, FAILED in other case
 */
uint8_t CAN_TestPolling(void)
{
    CAN_TxMessage_T TxMessage;
    CAN_RxMessage_T RxMessage;
    CAN_TX_MAILBIX_T TransmitMailbox = CAN_TX_MAILBIX_1;
    uint32_t i = 0;

    /* Reset Message */
    CAN_TxMessageInit(&TxMessage);
    CAN_RxMessageInit(&RxMessage);
    /* CAN1 transmit data to CAN2 */
    TxMessage.stdID         = 0x11;
    TxMessage.remoteTxReq   = CAN_RTXR_DATA;
    TxMessage.typeID        = CAN_TYPEID_STD;
    TxMessage.dataLengthCode= 2;
    TxMessage.data[0]       = 0xA1;
    TxMessage.data[1]       = 0xB1;
    TransmitMailbox = (CAN_TX_MAILBIX_T)CAN_TxMessage(CAN1, &TxMessage);

    i = 0;
    while ((!CAN_TxMessageStatus(CAN1, TransmitMailbox)) && (i != 0xFFFF))
    {
        i++;
    }

    i = 0;
    while ((CAN_PendingMessage(CAN2, CAN_RX_FIFO_0) < 1) && (i != 0xFFFF))
    {
        i++;
    }

    /* CAN2 receive data from CAN1 */
    CAN_RxMessage(CAN2, CAN_RX_FIFO_0, &RxMessage);

    /* Check data */
    if ((RxMessage.stdID          != TxMessage.stdID)   || \
        (RxMessage.typeID         != TxMessage.typeID)  || \
        (RxMessage.dataLengthCode != TxMessage.dataLengthCode) || \
        (RxMessage.data[0]        != TxMessage.data[0]) || \
        (RxMessage.data[1]        != TxMessage.data[1]))
    {
        return FAILED;
    }

    /* Reset Message */
    CAN_TxMessageInit(&TxMessage);
    CAN_RxMessageInit(&RxMessage);
    /* CAN2 transmit data to CAN1 */
    TxMessage.stdID         = 0x22;
    TxMessage.remoteTxReq   = CAN_RTXR_DATA;
    TxMessage.typeID        = CAN_TYPEID_STD;
    TxMessage.dataLengthCode= 2;
    TxMessage.data[0]       = 0xA2;
    TxMessage.data[1]       = 0xB2;
    TransmitMailbox = (CAN_TX_MAILBIX_T)CAN_TxMessage(CAN2, &TxMessage);

    i = 0;
    while ((!CAN_TxMessageStatus(CAN2, TransmitMailbox)) && (i != 0xFFFF))
    {
        i++;
    }

    i = 0;
    while ((CAN_PendingMessage(CAN2, CAN_RX_FIFO_0) < 1) && (i != 0xFFFF))
    {
        i++;
    }

    /* CAN1 receive data from CAN2 */
    CAN_RxMessage(CAN1, CAN_RX_FIFO_0, &RxMessage);

    /* Check data */
    if ((RxMessage.stdID          != TxMessage.stdID)   || \
        (RxMessage.typeID         != TxMessage.typeID)  || \
        (RxMessage.dataLengthCode != TxMessage.dataLengthCode) || \
        (RxMessage.data[0]        != TxMessage.data[0]) || \
        (RxMessage.data[1]        != TxMessage.data[1]))
    {
        return FAILED;
    }

    /* Test Passed */
    return PASSED;
}

/*!
 * @brief     Configures the CAN, transmit and receive using interrupt
 *
 * @param     None
 *
 * @retval    PASSED if the reception is well done, FAILED in other case
 */
uint8_t CAN_TestInterrupt(void)
{
    CAN_TxMessage_T TxMessage;
    uint32_t i = 0;

    /* transmit 1 message */
    TxMessage.stdID          = 0x0;
    TxMessage.extID          = 0x333333;
    TxMessage.remoteTxReq    = CAN_RTXR_DATA;
    TxMessage.typeID         = CAN_TYPEID_EXT;
    TxMessage.dataLengthCode = 2;
    TxMessage.data[0]        = 0xC3;
    TxMessage.data[1]        = 0xD3;

    CAN_TxMessage(CAN1, &TxMessage);

    /* initialize the value */
    intFlag1 = FAILED;
    intFlag2 = FAILED;

    /* receive message with interrupt handling */
    while (intFlag2 == FAILED)
    {
        if (++i > 0x7FFFFF)
        {
            return FAILED;
        }
    }

    i = 0;
    while (intFlag1 == FAILED)
    {
        if (++i > 0x7FFFFF)
        {
            return FAILED;
        }
    }

    return PASSED;
}

/*!
 * @brief     This function handles CAN1 Interrpt Handler
 *
 * @param     None
 *
 * @retval    None
 */
void CAN1_RxIsr(void)
{
    CAN_RxMessage_T RxMessage;

    CAN_RxMessageInit(&RxMessage);

    /* receive */
    CAN_RxMessage(CAN1, CAN_RX_FIFO_0, &RxMessage);

    if ((RxMessage.extID          == 0x444444) && \
        (RxMessage.typeID         == CAN_TYPEID_EXT) && \
        (RxMessage.dataLengthCode == 2   ) && \
        (RxMessage.data[0]        == 0xC4) && \
        (RxMessage.data[1]        == 0xD4))
    {
        intFlag1 = PASSED;
    }
    else
    {
        intFlag1 = FAILED;
    }
}

/*!
 * @brief     This function handles CAN2 Interrpt Handler
 *
 * @param     None
 *
 * @retval    None
 */
void CAN2_RxIsr(void)
{
    CAN_RxMessage_T RxMessage;
    CAN_TxMessage_T TxMessage;

    CAN_TxMessageInit(&TxMessage);
    CAN_RxMessageInit(&RxMessage);

    /* receive */
    CAN_RxMessage(CAN2, CAN_RX_FIFO_0, &RxMessage);

    if ((RxMessage.extID          == 0x333333) && \
        (RxMessage.typeID         == CAN_TYPEID_EXT) && \
        (RxMessage.dataLengthCode == 2   ) && \
        (RxMessage.data[0]        == 0xC3) && \
        (RxMessage.data[1]        == 0xD3))
    {
        intFlag2 = PASSED;

        /* CAN2 transmit data to CAN1  */
        TxMessage.extID          = 0x444444;
        TxMessage.remoteTxReq    = CAN_RTXR_DATA;
        TxMessage.typeID         = CAN_TYPEID_EXT;
        TxMessage.dataLengthCode = 2;
        TxMessage.data[0]        = 0xC4;
        TxMessage.data[1]        = 0xD4;
        CAN_TxMessage(CAN2, &TxMessage);
    }
    else
    {
        intFlag2 = FAILED;
    }
}

/*!
 * @brief     CAN GPIO initialization
 *
 * @param     None
 *
 * @retval    None
 */
void CAN_GPIOInit(void)
{
    GPIO_Config_T  GPIO_InitStructure;
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);

    /* USE_CAN2 */
    GPIO_InitStructure.pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

#ifdef APM32F411
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_8, GPIO_AF8_CAN1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_9, GPIO_AF8_CAN1);
#else
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_8, GPIO_AF_CAN1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_9, GPIO_AF_CAN1);
#endif
    /* USE_CAN2 */
    GPIO_InitStructure.pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_CAN2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_13, GPIO_AF_CAN2);
}

/*!
 * @brief     CAN initialization
 *
 * @param     baud: baud rate for CAN
 *                  This parameter can be one of the following values:
 *                  @arg CAN_BAUD_125K : CAN baud rate config to 125K
 *                  @arg CAN_BAUD_250K : CAN baud rate config to 250K
 *                  @arg CAN_BAUD_500K : CAN baud rate config to 500K
 *                  @arg CAN_BAUD_1M   : CAN baud rate config to 1M
 *
 * @retval    None
 */
void CAN_Init(uint8_t baud)
{
    CAN_Config_T       CAN_ConfigStructure;
    CAN_FilterConfig_T CAN_FilterStruct;
    uint32_t apb1Clock;
    uint16_t prescaler;
    uint16_t timeSegment1;
    uint16_t timeSegment2;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN1 | RCM_APB1_PERIPH_CAN2);

    RCM_ReadPCLKFreq(&apb1Clock, NULL);
    apb1Clock /= 1000000;

    /* Config GPIO for CAN */
    CAN_GPIOInit();

    /* CAN register init */
    CAN_Reset(CAN1);
    CAN_Reset(CAN2);

    CAN_ConfigStructInit(&CAN_ConfigStructure);

    /* CAN cell init */
    CAN_ConfigStructure.autoBusOffManage = DISABLE;
    CAN_ConfigStructure.autoWakeUpMode   = DISABLE;
    CAN_ConfigStructure.nonAutoRetran    = DISABLE;
    CAN_ConfigStructure.rxFIFOLockMode   = DISABLE;
    CAN_ConfigStructure.txFIFOPriority   = DISABLE;
    CAN_ConfigStructure.mode             = CAN_MODE_NORMAL;

    /* Baudrate = PCLK1 / (prescaler * (timeSegment1 + timeSegment2 + 1)) */
    CAN_ConfigStructure.syncJumpWidth = CAN_SJW_1;
    /* When APB1 Clock is not 42M, you should calculate by you clock!! */
    /* APM32F407 APB1 Clock is 42M */
    /* APM32F411 APB1 Clock is 50M */
    if (baud == CAN_BAUD_500K)
    {
        CAN_ConfigStructure.timeSegment1  = TIME1;
        CAN_ConfigStructure.timeSegment2  = TIME2;
        CAN_ConfigStructure.prescaler     = PRES;
    }
    else if (baud == CAN_BAUD_1M)
    {
        CAN_ConfigStructure.timeSegment1  = TIME1;
        CAN_ConfigStructure.timeSegment2  = TIME2;
        CAN_ConfigStructure.prescaler     = PRES/2;
    }
    else //!< 125K and 250K
    {
        CAN_ConfigStructure.timeSegment1  = CAN_TIME_SEGMENT1_3;
        CAN_ConfigStructure.timeSegment2  = CAN_TIME_SEGMENT2_4;
        CAN_ConfigStructure.prescaler     = apb1Clock >> baud;
    }
    CAN_Config(CAN1, &CAN_ConfigStructure);
    CAN_Config(CAN2, &CAN_ConfigStructure);

    /* CAN filter init */
    CAN_FilterStruct.filterMode       = CAN_FILTER_MODE_IDMASK;
    CAN_FilterStruct.filterScale      = CAN_FILTER_SCALE_32BIT;
    CAN_FilterStruct.filterIdHigh     = 0x0000;
    CAN_FilterStruct.filterIdLow      = 0x0000;
    CAN_FilterStruct.filterMaskIdHigh = 0x0000;
    CAN_FilterStruct.filterMaskIdLow  = 0x0000;
    CAN_FilterStruct.filterFIFO       = CAN_FILTER_FIFO_0;
    CAN_FilterStruct.filterActivation = ENABLE;

    /* CAN1 filter can be from 0 to 13 */
    CAN_FilterStruct.filterNumber = 0;
    CAN_ConfigFilter(&CAN_FilterStruct);

    /* CAN2 filter can be from 14 to 27(using CAN1 register) */
    CAN_FilterStruct.filterNumber = 14;
    CAN_ConfigFilter(&CAN_FilterStruct);

    prescaler = CAN_ConfigStructure.prescaler;
    timeSegment1 = CAN_ConfigStructure.timeSegment1;
    timeSegment2 = CAN_ConfigStructure.timeSegment2;

    printf("   Configuration :\r\n");
    printf("   APB1 Clock : %d MHz\r\n", (uint16_t)apb1Clock);
    printf("   CAN Baud   : %d K\r\n",
           (uint16_t)(apb1Clock * 1000) / (prescaler * (timeSegment1 + timeSegment2 + 3)));
}

/*!
 * @brief     Configures NVIC and Vector Table base location
 *
 * @param     None
 *
 * @retval    None
 */
void NVIC_Configuration(void)
{
    NVIC_EnableIRQRequest(CAN1_RX0_IRQn, 0, 0);
    NVIC_EnableIRQRequest(CAN2_RX0_IRQn, 0, 0);
}

/*!
 * @brief     Delay
 *
 * @param     None
 *
 * @retval    None
 */
void CAN_Delay(uint32_t cnt)
{
    while(cnt--)
    {
        __NOP();
    }
}

/*!
 * @brief     Configures CAN_TxMessage_T structure
 *
 * @param     txMessage : CAN_TxMessage_T structure
 *
 * @retval    None
 */
void CAN_TxMessageInit(CAN_TxMessage_T *txMessage)
{
    txMessage->stdID          = 0x00;
    txMessage->extID          = 0x00;
    txMessage->typeID         = CAN_TYPEID_STD;
    txMessage->dataLengthCode = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        txMessage->data[i]=0x00;
    }
}

/*!
 * @brief     Configures CAN_RxMessage_T structure
 *
 * @param     rxMessage : CAN_RxMessage_T structure
 *
 * @retval    None
 */
void CAN_RxMessageInit(CAN_RxMessage_T *rxMessage)
{
    rxMessage->stdID            = 0x00;
    rxMessage->extID            = 0x00;
    rxMessage->typeID           = 0;
    rxMessage->dataLengthCode   = 0;
    rxMessage->filterMatchIndex = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        rxMessage->data[i]=0x00;
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
