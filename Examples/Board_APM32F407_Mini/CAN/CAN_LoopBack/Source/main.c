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

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

#define CANx        CAN1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

enum {FAILED = 0, PASSED = 1};

volatile uint8_t intFlag = 0;

/* Private function prototypes ********************************************/

void NVIC_Configuration(void);
uint8_t CAN_Polling(void);
uint8_t CAN_Interrupt_Init(void);

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
    uint8_t state;
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
    BOARD_COMInit(COM1,&usartConfigStruct);

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN1);
    NVIC_Configuration();

    printf(" CAN_LoopBack Test \r\n");

    state = CAN_Polling();
    if (state == FAILED)
    {
        /* Turn off LED2 */
        BOARD_LEDOff(LED2);
        printf("> CAN1 polling test is FAILED \r\n");
    }
    else
    {
        /* Turn on LED2 */
        BOARD_LEDOn(LED2);
        printf("> CAN1 polling test is PASSED \r\n");
    }

    /* CAN transmit at 500Kb/s and receive by interrupt in loopback mode */
    state = CAN_Interrupt_Init();

    if (state == FAILED)
    {
        /* Turn off LED3 */
        BOARD_LEDOff(LED3);
        printf("> CAN1 interrupt test is FAILED \r\n");
    }
    else
    {
        /* Turn on LED3 */
        BOARD_LEDOn(LED3);
        printf("> CAN1 interrupt test is PASSED \r\n");
    }
    while (1)
    {
    }
}

/*!
 * @brief     Configures the CAN, transmit and receive by polling
 *
 * @param     None
 *
 * @retval    PASSED if the reception is well done, FAILED in other case
 */
uint8_t CAN_Polling(void)
{
    CAN_Config_T    CAN_ConfigStructure;
    CAN_FilterConfig_T CAN_FilterStruct;
    CAN_TxMessage_T TxMessage;
    CAN_RxMessage_T RxMessage;
    uint32_t i = 0;
    CAN_TX_MAILBIX_T TransmitMailbox = CAN_TX_MAILBIX_1;

    /* CAN register init */
    CAN_Reset(CANx);

    CAN_ConfigStructInit(&CAN_ConfigStructure);

    /* CAN cell init */
    CAN_ConfigStructure.autoBusOffManage=DISABLE;
    CAN_ConfigStructure.autoWakeUpMode=DISABLE;
    CAN_ConfigStructure.nonAutoRetran=DISABLE;
    CAN_ConfigStructure.rxFIFOLockMode=DISABLE;
    CAN_ConfigStructure.txFIFOPriority=DISABLE;
    CAN_ConfigStructure.mode=CAN_MODE_LOOPBACK;

    /* Baudrate = (apb1Clock*1000)/(prescaler*(timeSegment1+timeSegment2+3)) = 125kbps*/
    CAN_ConfigStructure.syncJumpWidth=CAN_SJW_1;
    CAN_ConfigStructure.timeSegment1=CAN_TIME_SEGMENT1_2;
    CAN_ConfigStructure.timeSegment2=CAN_TIME_SEGMENT2_3;
    CAN_ConfigStructure.prescaler=56;
    CAN_Config(CANx, &CAN_ConfigStructure);

    /* CAN filter init */
    CAN_FilterStruct.filterNumber=0;
    CAN_FilterStruct.filterMode=CAN_FILTER_MODE_IDMASK;
    CAN_FilterStruct.filterScale=CAN_FILTER_SCALE_32BIT;
    CAN_FilterStruct.filterIdHigh=0x0000;
    CAN_FilterStruct.filterIdLow=0x0000;
    CAN_FilterStruct.filterMaskIdHigh=0x0000;
    CAN_FilterStruct.filterMaskIdLow=0x0000;
    CAN_FilterStruct.filterFIFO=CAN_FILTER_FIFO_0;
    CAN_FilterStruct.filterActivation=ENABLE;
    CAN_ConfigFilter(&CAN_FilterStruct);

    /* transmit */
    TxMessage.stdID=0x11;
    TxMessage.remoteTxReq=CAN_RTXR_DATA;
    TxMessage.typeID=CAN_TYPEID_STD;
    TxMessage.dataLengthCode=2;
    TxMessage.data[0]=0xCA;
    TxMessage.data[1]=0xFE;

    TransmitMailbox=(CAN_TX_MAILBIX_T)CAN_TxMessage(CANx, &TxMessage);
    i = 0;
    while((!CAN_TxMessageStatus(CANx, TransmitMailbox)) && (i != 0xFFFF))
    {
        i++;
    }

    i = 0;
    while((CAN_PendingMessage(CANx, CAN_RX_FIFO_0) < 1) && (i != 0xFFFF))
    {
        i++;
    }

    /* receive */
    RxMessage.stdID=0x00;
    RxMessage.typeID=CAN_TYPEID_STD;
    RxMessage.dataLengthCode=0;
    RxMessage.data[0]=0x00;
    RxMessage.data[1]=0x00;
    CAN_RxMessage(CANx, CAN_RX_FIFO_0, &RxMessage);

    if (RxMessage.stdID!=0x11)
    {
        return FAILED;
    }

    if (RxMessage.typeID!=CAN_TYPEID_STD)
    {
        return FAILED;
    }

    if (RxMessage.dataLengthCode!=2)
    {
        return FAILED;
    }

    if ((RxMessage.data[0]<<8|RxMessage.data[1])!=0xCAFE)
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
uint8_t CAN_Interrupt_Init(void)
{
    CAN_Config_T    CAN_ConfigStructure;
    CAN_FilterConfig_T CAN_FilterStruct;
    CAN_TxMessage_T TxMessage;
    uint32_t i = 0;

    /* CAN register init */
    CAN_Reset(CANx);

    CAN_ConfigStructInit(&CAN_ConfigStructure);

    /* CAN cell init */
    CAN_ConfigStructure.autoBusOffManage=DISABLE;
    CAN_ConfigStructure.autoWakeUpMode=DISABLE;
    CAN_ConfigStructure.nonAutoRetran=DISABLE;
    CAN_ConfigStructure.rxFIFOLockMode=DISABLE;
    CAN_ConfigStructure.txFIFOPriority=DISABLE;
    CAN_ConfigStructure.mode=CAN_MODE_LOOPBACK;

    /* Baudrate = (apb1Clock*1000)/(prescaler*(timeSegment1+timeSegment2+3)) = 500 Kbps */
    CAN_ConfigStructure.syncJumpWidth=CAN_SJW_1;
    CAN_ConfigStructure.timeSegment1=CAN_TIME_SEGMENT1_2;
    CAN_ConfigStructure.timeSegment2=CAN_TIME_SEGMENT2_3;
    CAN_ConfigStructure.prescaler=14;
    CAN_Config(CANx, &CAN_ConfigStructure);

    /* CAN filter init */
    CAN_FilterStruct.filterNumber=1;
    CAN_FilterStruct.filterMode=CAN_FILTER_MODE_IDMASK;
    CAN_FilterStruct.filterScale=CAN_FILTER_SCALE_32BIT;
    CAN_FilterStruct.filterIdHigh=0x0000;
    CAN_FilterStruct.filterIdLow=0x0000;
    CAN_FilterStruct.filterMaskIdHigh=0x0000;
    CAN_FilterStruct.filterMaskIdLow=0x0000;
    CAN_FilterStruct.filterFIFO=CAN_FILTER_FIFO_0;
    CAN_FilterStruct.filterActivation=ENABLE;
    CAN_ConfigFilter(&CAN_FilterStruct);

    /* CAN FIFO0 message pending interrupt enable */
    CAN_EnableInterrupt(CANx, CAN_INT_F0MP);

    /* transmit 1 message */
    TxMessage.stdID=0x0;
    TxMessage.extID = 0x1234;
    TxMessage.remoteTxReq=CAN_RTXR_DATA;
    TxMessage.typeID=CAN_TYPEID_EXT;
    TxMessage.dataLengthCode=2;
    TxMessage.data[0]=0xDE;
    TxMessage.data[1]=0xCA;

    CAN_TxMessage(CANx, &TxMessage);

    /* initialize the value that will be returned */
    intFlag = 0xFF;

    /* receive message with interrupt handling */
    i=0;
    while((intFlag == 0xFF) && (i < 0xFFF))
    {
        i++;
    }

    if (i == 0xFFF)
    {
        intFlag=0;
    }

    /* CAN1 Handler */

    CAN_DisableInterrupt(CANx, CAN_INT_F0MP);

    return intFlag;
}

/*!
 * @brief     This function handles CAN1 Interrpt Handler
 *
 * @param     None
 *
 * @retval    None
 */
void CAN1_RxIsr()
{
    CAN_RxMessage_T RxMessage;

    /* receive */
    RxMessage.stdID=0x00;
    RxMessage.extID = 0x00;
    RxMessage.typeID=0;
    RxMessage.dataLengthCode=0;
    RxMessage.data[0]=0x00;
    RxMessage.data[1]=0x00;
    CAN_RxMessage(CAN1, CAN_RX_FIFO_0, &RxMessage);

    if((RxMessage.extID==0x1234) && (RxMessage.typeID==CAN_TYPEID_EXT)
     && (RxMessage.dataLengthCode==2) && (RxMessage.data[0] == 0xDE) && (RxMessage.data[1]==0xCA))
    {
        intFlag = 1;
    }
    else
    {
        intFlag = 0;
    }
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
    NVIC_EnableIRQRequest(CAN1_RX0_IRQn , 0, 0);
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
