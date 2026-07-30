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
#include "apm32f4xx_dma.h"
#include "apm32f4xx.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Set CS pin high level */
#define QSPI_CS_HIGH    GPIO_SetBit(GPIOB, GPIO_PIN_9)
/* Set CS pin low level */
#define QSPI_CS_LOW     GPIO_ResetBit(GPIOB, GPIO_PIN_9)

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/*!
 * @brief       DMA Init Tx
 *
 * @param       None
 *
 * @retval      None
 *
 */
static void DMA_InitTx(uint16_t length, uint8_t* buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    /* size of buffer*/
    dmaConfig.bufferSize = length;
    /* set memory Data Size*/
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;
    /* Set peripheral Data Size*/
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;
    /* Enable Memory Address increase*/
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Disable Peripheral Address increase*/
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Normal Mode*/
    dmaConfig.loopMode = DMA_MODE_NORMAL;
    /* set priority*/
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    /* read from memory */
    dmaConfig.dir = DMA_DIR_MEMORYTOPERIPHERAL;
    /* Set memory Address*/
    dmaConfig.memoryBaseAddr = (uint32_t)buf;
    /* Set Peripheral Address*/
    dmaConfig.peripheralBaseAddr = (uint32_t)&QSPI->DATA;

    dmaConfig.channel = DMA_CHANNEL_3;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;

    DMA_Config(DMA2_Stream7, &dmaConfig);
    /* Clear DMA TF flag*/
    DMA_ClearIntFlag(DMA2_Stream7, DMA_INT_TCIFLG7);
}

/*!
 * @brief       DMA Init Rx
 *
 * @param       None
 *
 * @retval      None
 *
 */
static void DMA_InitRx(uint16_t length, uint8_t* buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    /* size of buffer*/
    dmaConfig.bufferSize = length;
    /* set memory Data Size*/
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;
    /* Set peripheral Data Size*/
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;
    /* Enable Memory Address increase*/
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Disable Peripheral Address increase*/
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Normal Mode*/
    dmaConfig.loopMode = DMA_MODE_NORMAL;
    /* set priority*/
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    /* read from peripheral */
    dmaConfig.dir = DMA_DIR_PERIPHERALTOMEMORY;
    /* Set memory Address*/
    dmaConfig.memoryBaseAddr = (uint32_t)buf;
    /* Set Peripheral Address*/
    dmaConfig.peripheralBaseAddr = (uint32_t)&QSPI->DATA;

    dmaConfig.channel = DMA_CHANNEL_3;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;

    DMA_Config(DMA2_Stream4, &dmaConfig);
    /* Clear DMA TF flag*/
    DMA_ClearIntFlag(DMA2_Stream4, DMA_INT_TCIFLG4);
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
        GPIO_ResetBit(GPIOB, GPIO_PIN_9);
    }
    else
    {
        GPIO_SetBit(GPIOB, GPIO_PIN_9);
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

    RCM_EnableAHB1PeriphClock((RCM_APB2_PERIPH_T)(RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOF));

    /* QSPI_IO0 */
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_8, GPIO_AF10_QSPI);
    /* QSPI_IO1 */
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_9, GPIO_AF10_QSPI);
    /* QSPI_IO2 */
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_7, GPIO_AF10_QSPI);
    /* QSPI_IO3 */
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_6, GPIO_AF10_QSPI);
    /* QSPI_CLK */
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_10, GPIO_AF10_QSPI);

    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_ConfigStruct.pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_Config(GPIOF, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.mode = GPIO_MODE_OUT;
    GPIO_ConfigStruct.pin = GPIO_PIN_9;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_SetBit(GPIOB, GPIO_PIN_9);
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

    RCM_EnableAHB3PeriphClock(RCM_AHB3_PERIPH_QSPI);

    QSPI_GPIOInit();

    QSPI_Reset();

    configStruct.selectSlaveToggle = QSPI_SST_DISABLE;
    configStruct.clockDiv = 0x64;
    configStruct.dataFrameSize = QSPI_DFS_8BIT;
    configStruct.frameFormat = QSPI_FRF_STANDARD;
    QSPI_Config(&configStruct);

    QSPI_ConfigTxFifoThreshold(0);
    QSPI_ConfigRxFifoThreshold(1);
    QSPI_EnableClockStretch();
    QSPI_EnableSlave();
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
 * @brief       QSPI write in quad mode
 *
 * @param       wParam: Write parameter
 *
 * @retval      None
 *
 */
void QSPI_Quad_WriteDMA(QSPI_ReadWriteParam_T *wParam)
{
    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_TX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)wParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)wParam->instLen);
    QSPI_ConfigInstAddrType(wParam->instAddrType);
    QSPI_ConfigFrameNum(wParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_QUAD);
    QSPI_EnableDMA(QSPI_DMA_TX);
    QSPI_ConfigDMATDL(7);
    QSPI_Enable();

    DMA_InitTx(wParam->dataLen, wParam->dataBuf);
    
    if(wParam->instLen)
    {
        while (QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
        QSPI_TxData(wParam->instruction);
        
    }

    if(wParam->addrLen)
    {
        while (QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
        QSPI_TxData(wParam->addr);
    }

    DMA_Enable(DMA2_Stream7);
    while (DMA_ReadStatusFlag(DMA2_Stream7, DMA_FLAG_TCIFLG7) == RESET);

    DMA_ClearStatusFlag(DMA2_Stream7, DMA_FLAG_TCIFLG7);
    DMA_Disable(DMA2_Stream7);
    
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
void QSPI_Quad_ReadDMA(QSPI_ReadWriteParam_T *rParam)
{
    QSPI_CS_LOW;

    QSPI_Disable();
    QSPI_ConfigTansMode(QSPI_TRANS_MODE_RX);
    QSPI_ConfigAddrLen((QSPI_ADDR_LEN_T)rParam->addrLen);
    QSPI_ConfigInstLen((QSPI_INST_LEN_T)rParam->instLen);
    QSPI_ConfigWaitCycle(rParam->waitCycle);
    QSPI_ConfigInstAddrType(rParam->instAddrType);
    QSPI_ConfigFrameNum(rParam->dataLen - 1);
    QSPI_ConfigFrameFormat(QSPI_FRF_QUAD);
    QSPI_EnableDMA(QSPI_DMA_RX);
    QSPI_ConfigDMARDL(0);
    QSPI_Enable();

    DMA_InitRx(rParam->dataLen, rParam->dataBuf);
    
    if(rParam->instLen)
    {
        QSPI_TxData(rParam->instruction);
        while (QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
    }

    if(rParam->addrLen)
    {
        QSPI_TxData(rParam->addr);
        while (QSPI_ReadStatusFlag(QSPI_FLAG_TFNF) == RESET);
    }

    
    DMA_Enable(DMA2_Stream4);
    while (DMA_ReadStatusFlag(DMA2_Stream4, DMA_FLAG_TCIFLG4) == RESET);

    DMA_ClearStatusFlag(DMA2_Stream4, DMA_FLAG_TCIFLG4);
    DMA_Disable(DMA2_Stream4);
    
    while(QSPI_ReadStatusFlag(QSPI_FLAG_BUSY) == SET);

    QSPI_CS_HIGH;
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
