/*!
 * @file        qspi.c
 *
 * @brief       This file contains the headers of the interrupt handlers
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
#include "qspi.h"
#include "apm32f4xx_qspi.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

volatile uint8_t qspi_tx_done = 0;
volatile uint8_t qspi_rx_done = 0;

/* Set CS pin high level */
#define QSPI_CS_HIGH    GPIO_SetBit(GPIOB, GPIO_PIN_6)
/* Set CS pin low level */
#define QSPI_CS_LOW     GPIO_ResetBit(GPIOB, GPIO_PIN_6)

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
QSPI_ReadWriteParamInt_T tparam_int;
QSPI_ReadWriteParamInt_T rparam_int;
/* Private function prototypes ********************************************/
static void QSPI_ITParamInt(void)
{
    /* Tx paramer for interrupt */
    tparam_int.dataBuf = NULL;
    tparam_int.dataCnt = 0;
    tparam_int.dataLen = 0;
    tparam_int.addrstate = 0;
    tparam_int.addr = 0;
    tparam_int.inststate = 0;
    tparam_int.instruction = 0;
    /* Rx paramer for interrupt */
    rparam_int.dataBuf = NULL;
    rparam_int.dataCnt = 0;
    rparam_int.dataLen = 0;
    rparam_int.addrstate = 0;
    rparam_int.addr = 0;
    rparam_int.inststate = 0;
    rparam_int.instruction = 0;
}
/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Chip select
 *
 * @param       select: Chip select.
 *                      0: CS pin high; 1: CS pin low
 * @retval      None
 *
 */
void QSPI_ChipSelect(uint8_t select)
{
    if(select)
    {
        GPIO_ResetBit(GPIOB, GPIO_PIN_6);
    }
    else
    {
        GPIO_SetBit(GPIOB, GPIO_PIN_6);
    }
}

/*!
 * @brief       QSPI GPIO Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void QSPI_GPIOInit(void)
{
    GPIO_Config_T GPIO_ConfigStruct;

    RCM_EnableAHB1PeriphClock((RCM_APB2_PERIPH_T)(RCM_AHB1_PERIPH_GPIOE | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOC));

    /* QSPI_IO0 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_7, GPIO_AF10_QSPI);
    /* QSPI_IO1 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_8, GPIO_AF10_QSPI);
    /* QSPI_IO2 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_9, GPIO_AF10_QSPI);
    /* QSPI_IO3 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_5, GPIO_AF10_QSPI);
    /* QSPI_CLK */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_1, GPIO_AF_QSPI);
    /* QSPI_CS */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_6, GPIO_AF10_QSPI);

    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_ConfigStruct.pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_Config(GPIOE, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_5;
    GPIO_Config(GPIOC, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_1;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.mode = GPIO_MODE_OUT;
    GPIO_ConfigStruct.pin = GPIO_PIN_6;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_SetBit(GPIOB, GPIO_PIN_6);
}

/*!
 * @brief       QSPI initialization
 *
 * @param       None
 *
 * @retval      None
 *
 */
void QSPI_Init(void)
{
    QSPI_Config_T configStruct;

    RCM_EnableAHB2PeriphClock(RCM_AHB2_PERIPH_QSPI);

    QSPI_GPIOInit();

    QSPI_Reset();

    configStruct.selectSlaveToggle = QSPI_SST_DISABLE;
    configStruct.clockDiv = 0X64;
    configStruct.clockPhase = QSPI_CLKPHA_1EDGE;
    configStruct.clockPolarity = QSPI_CLKPOL_LOW;
    configStruct.dataFrameSize = QSPI_DFS_8BIT;
    configStruct.frameFormat = QSPI_FRF_STANDARD;
    QSPI_Config(&configStruct);

    QSPI_ConfigTxFifoThreshold(0);
    QSPI_ConfigRxFifoThreshold(1);
    QSPI_EnableClockStretch();
    QSPI_EnableSlave();

    QSPI_DisableInterrupt(QSPI_INT_TFE | QSPI_INT_TFO | QSPI_INT_RFU | QSPI_INT_RFO | QSPI_INT_RFF | QSPI_INT_MST);

    NVIC_EnableIRQRequest(QSPI_IRQn, 1, 1);
}

/*!
 * @brief       Write and read one byte data in standard mode.The data size is 8 bits.
 *
 * @param       data:   Data to be writed
 *
 * @retval      Return read data
 *
 */
uint8_t QSPI_Std_WriteReadByte(uint8_t data)
{
    uint8_t ret = 0;

    QSPI_Disable();
    QSPI_ConfigFrameFormat(QSPI_FRF_STANDARD);
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_TX_RX);
    QSPI_Enable();


    while(QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
    QSPI_TxData(data);

    while((QSPI_ReadStatusFlag(QSPI_FLAG_RFNE) == RESET) || (QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET));
    ret = (uint8_t)QSPI_RxData();

    return(ret);
}

/*!
 * @brief       Write data in standard mode.The data size is 8 bits
 *
 * @param       wBuf:   Data buffer
 *
 * @param       wLen:   Data length
 *
 * @retval      None
 *
 */
void QSPI_Std_Write(uint8_t *wBuf, uint16_t wLen)
{
    uint16_t i;

    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigFrameFormat(QSPI_FRF_STANDARD);
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_TX);
    QSPI_Enable();

    for(i = 0; i < wLen; i++)
    {
        while(QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
        QSPI_TxData(wBuf[i]);
    }

    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);
    /* Extend the low level time of CS */
    if(wLen == 0x1)
    {
        volatile uint8_t j = 0xff;
        while(j>0)
        {
            j--;
        }
    }
    QSPI_CS_HIGH;
}

/*!
 * @brief       Read data in standard mode.The data size is 8 bits
 *
 * @param       rBuf:   Data buffer
 *
 * @param       rLen:   Data length
 *
 * @retval      None
 *
 */
void QSPI_Std_Read(uint8_t *rBuf, uint16_t rLen)
{
    uint16_t i;

    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigFrameFormat(QSPI_FRF_STANDARD);
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_RX);
    QSPI_Enable();

    for(i = 0; i < rLen; i++)
    {
        while(QSPI_ReadStatusFlag(QSPI_FLAG_RFNE) == RESET);
        rBuf[i] = (uint8_t)QSPI_RxData();
    }

    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);

    QSPI_CS_HIGH;
}

/*!
 * @brief       QSPI write in quad mode
 *
 * @param       wParam: Write parameter
 *
 * @retval      None
 *
 */
void QSPI_Quad_Write(QSPI_ReadWriteParam_T *wParam)
{
    uint16_t i;

    QSPI_CS_LOW;

    QSPI_Disable();;
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_TX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)wParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)wParam->instLen);
    QSPI_ConfigInstAddrType(wParam->instAddrType);
    QSPI_ConfigFrameNum(wParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_QUAD);
    QSPI_Enable();

    if(wParam->instLen)
    {
        QSPI_TxData(wParam->instruction);
    }

    if(wParam->addrLen)
    {
        QSPI_TxData(wParam->addr);
    }

    for(i = 0; i < wParam->dataLen; i++)
    {
        while(QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
        QSPI_TxData(wParam->dataBuf[i]);
    }

    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);

    QSPI_CS_HIGH;
}

/*!
 * @brief       QSPI read in quad mode
 *
 * @param       rParam: Read parameter
 *
 * @retval      None
 *
 */
void QSPI_Quad_Read(QSPI_ReadWriteParam_T *rParam)
{
    uint16_t i;
    uint8_t *pdata;

    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_RX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)rParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)rParam->instLen);
    QSPI_ConfigWaitCycle(rParam->waitCycle);
    QSPI_ConfigInstAddrType(rParam->instAddrType);
    QSPI_ConfigFrameNum(rParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_QUAD);
    QSPI_Enable();

    if(rParam->instLen)
    {
        QSPI_TxData(rParam->instruction);
    }

    if(rParam->addrLen)
    {
        QSPI_TxData(rParam->addr);
    }

    pdata = rParam->dataBuf;
    i = rParam->dataLen;
    while(i)
    {
        while((QSPI->STS & QSPI_FLAG_RFNE) == 0){};
        *pdata++ = (uint32_t)QSPI->DATA;
        i--;
    }

    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);

    QSPI_CS_HIGH;
}

/*!
 * @brief       QSPI write in quad mode by interrupt
 *
 * @param       wParam: Write parameter
 *
 * @retval      None
 *
 */
void QSPI_Quad_WriteIT(QSPI_ReadWriteParam_T *wParam)
{
    qspi_tx_done = 0;
    QSPI_CS_LOW;

    QSPI_Disable();;
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_TX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)wParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)wParam->instLen);
    QSPI_ConfigInstAddrType(wParam->instAddrType);
    QSPI_ConfigFrameNum(wParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_QUAD);

    QSPI_ITParamInt();

    QSPI_ConfigTxFifoEmptyThreshold(1);

    if(wParam->instLen)
    {
        tparam_int.inststate = 1;
        tparam_int.instruction = wParam->instruction;
    }

    if(wParam->addrLen)
    {
        tparam_int.addrstate = 1;
        tparam_int.addr = wParam->addr;
    }

    tparam_int.dataBuf = wParam->dataBuf;
    tparam_int.dataLen = wParam->dataLen;
    tparam_int.dataCnt = 0;

    QSPI_EnableInterrupt(QSPI_INT_TFE);
    QSPI_Enable();

    while(qspi_tx_done == 0);

    QSPI_CS_HIGH;
}

/*!
 * @brief       QSPI read in quad mode by interrupt
 *
 * @param       rParam: Read parameter
 *
 * @retval      None
 *
 */
void QSPI_Quad_ReadIT(QSPI_ReadWriteParam_T *rParam)
{
    qspi_rx_done = 0;
    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_RX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)rParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)rParam->instLen);
    QSPI_ConfigWaitCycle(rParam->waitCycle);
    QSPI_ConfigInstAddrType(rParam->instAddrType);
    QSPI_ConfigFrameNum(rParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_QUAD);

    QSPI_ITParamInt();

    if(rParam->instLen)
    {
        tparam_int.inststate = 1;
        tparam_int.instruction = rParam->instruction;
    }

    if(rParam->addrLen)
    {
        tparam_int.addrstate = 1;
        tparam_int.addr = rParam->addr;
    }

    rparam_int.dataBuf = rParam->dataBuf;
    rparam_int.dataLen = rParam->dataLen;
    rparam_int.dataCnt = 0;

    QSPI_ConfigRxFifoThreshold(1);
    QSPI_EnableInterrupt(QSPI_INT_RFU);
    QSPI_Enable();

    while(qspi_rx_done == 0);
}

/*!
 * @brief   QSPI Tx interrupt service handling
 *
 * @param   None
 *
 * @retval  None
 */
void QSPI_TxIRQHandler(void)
{
    if(QSPI_ReadIntFlag(QSPI_INT_FLAG_TFE))
    {
        if(tparam_int.inststate)
        {
            QSPI_TxData(tparam_int.instruction);
            tparam_int.inststate = 0;
        }
        else if(tparam_int.addrstate)
        {
            QSPI_TxData(tparam_int.addr);
            tparam_int.addrstate = 0;
        }
        else if(tparam_int.dataCnt < tparam_int.dataLen)
        {
            QSPI_TxData(tparam_int.dataBuf[tparam_int.dataCnt]);
            tparam_int.dataCnt++;
        }
        else
        {
            QSPI_DisableInterrupt(QSPI_INT_TFE);
            QSPI_CS_HIGH;
            qspi_tx_done = 1;
        }
    }
}

/*!
 * @brief   QSPI Rx interrupt service handling
 *
 * @param   None
 *
 * @retval  None
 */
void QSPI_RxIRQHandler(void)
{
    if(QSPI_ReadIntFlag(QSPI_INT_FLAG_RFU))
    {
        if(rparam_int.dataLen > rparam_int.dataCnt)
        {
            rparam_int.dataBuf[rparam_int.dataCnt++] = QSPI_RxData();
        }
        else
        {
            QSPI_DisableInterrupt(QSPI_INT_RFU);
            QSPI_CS_HIGH;
            qspi_rx_done = 1;
        }
    }
}

/*!
 * @brief       QSPI read in eeprom read mode
 *
 * @param       rParam: Read parameter
 *
 * @retval      None
 *
 */
void QSPI_Eeprom_Read(uint32_t cmd, uint8_t cmdSize, uint32_t addr, uint8_t addrSize, uint8_t *rBuf, uint32_t rLen)
{
    uint16_t i;

    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_EEPROM_READ);
    QSPI_ConfigFrameFormat(QSPI_FRF_STANDARD);
    QSPI_ConfigFrameNum(rLen);
    QSPI_ConfigTxFifoThreshold(cmdSize + addr - 1);
    QSPI_Enable();

    for(i = cmdSize; i; i--)
    {
        QSPI_TxData(cmd >> ((i - 1) << 3));

    }

    for(i = addrSize; i; i--)
    {
        QSPI_TxData(addr >> ((i - 1) << 3));
    }

    for(i = 0; i < rLen; i++)
    {
        while(QSPI_ReadStatusFlag(QSPI_FLAG_RFNE) == RESET);
        rBuf[i] = QSPI_RxData();
    }

    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);

    QSPI_CS_HIGH;
}

/*!
 * @brief       QSPI read in dual mode
 *
 * @param       rParam: Read parameter
 *
 * @retval      None
 *
 */
void QSPI_Dual_Read(QSPI_ReadWriteParam_T *rParam)
{
    uint16_t i;

    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_RX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)rParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)rParam->instLen);
    QSPI_ConfigWaitCycle(rParam->waitCycle);
    QSPI_ConfigInstAddrType(rParam->instAddrType);
    QSPI_ConfigFrameNum(rParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_DUAL);
    QSPI_Enable();

    if(rParam->instLen)
    {
        QSPI_TxData(rParam->instruction);
    }

    if(rParam->addrLen)
    {
        QSPI_TxData(rParam->addr);
    }

    for(i = 0; i < rParam->dataLen; i++)
    {
        while(QSPI_ReadStatusFlag(QSPI_FLAG_RFNE) == RESET);
        rParam->dataBuf[i] = QSPI_RxData();
    }

    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);

    QSPI_CS_HIGH;
}
