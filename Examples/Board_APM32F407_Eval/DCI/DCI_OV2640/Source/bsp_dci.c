/*!
 * @file        bsp_dci.c
 *
 * @brief       DCI board support package body
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
#include "bsp_dci.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       DCI Initialization
 *
 * @param       None
 *
 * @retval      None
 *
 */
void DCI_Init(void)
{
    GPIO_Config_T  gpioConfigStruct;
    DCI_Config_T dciConfigStruct;

    /** Enable the GPIO Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOC | RCM_AHB1_PERIPH_GPIOE);

    /** Enable the DCI Clock */
    RCM_EnableAHB2PeriphClock(RCM_AHB2_PERIPH_DCI);

    /** Configure the GPIO pin */
    gpioConfigStruct.pin = DCI_HSYNC_PIN | DCI_PIXCLK_PIN;
    gpioConfigStruct.mode = GPIO_MODE_AF;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    gpioConfigStruct.speed = GPIO_SPEED_100MHz;
    gpioConfigStruct.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOA, &gpioConfigStruct);

    gpioConfigStruct.pin = DCI_D5_PIN | DCI_VSYNC_PIN;
    GPIO_Config(GPIOB, &gpioConfigStruct);

    gpioConfigStruct.pin = DCI_D0_PIN | DCI_D1_PIN | DCI_D2_PIN | DCI_D3_PIN;
    GPIO_Config(GPIOC, &gpioConfigStruct);
    
    gpioConfigStruct.pin = DCI_D4_PIN | DCI_D6_PIN | DCI_D7_PIN;
    GPIO_Config(GPIOE, &gpioConfigStruct);

    /** PA4,AF13  DCI_HSYNC */
    GPIO_ConfigPinAF(GPIOA,GPIO_PIN_SOURCE_4,GPIO_AF_DCI);
    /** PA6,AF13  DCI_PCLK  */
    GPIO_ConfigPinAF(GPIOA,GPIO_PIN_SOURCE_6,GPIO_AF_DCI);
    /** PB7,AF13  DCI_VSYNC */
    GPIO_ConfigPinAF(GPIOB,GPIO_PIN_SOURCE_7,GPIO_AF_DCI);
    /** PC6,AF13  DCI_D0 */
    GPIO_ConfigPinAF(GPIOC,GPIO_PIN_SOURCE_6,GPIO_AF_DCI);
    /** PC7,AF13  DCI_D1 */
    GPIO_ConfigPinAF(GPIOC,GPIO_PIN_SOURCE_7,GPIO_AF_DCI);
    /** PC8,AF13  DCI_D2 */
    GPIO_ConfigPinAF(GPIOC,GPIO_PIN_SOURCE_8,GPIO_AF_DCI);
    /** PC9,AF13  DCI_D3 */
    GPIO_ConfigPinAF(GPIOC,GPIO_PIN_SOURCE_9,GPIO_AF_DCI);
    /** PE4,AF13 DCI_D4 */
    GPIO_ConfigPinAF(GPIOE,GPIO_PIN_SOURCE_4,GPIO_AF_DCI);
    /** PB6,AF13  DCI_D5 */
    GPIO_ConfigPinAF(GPIOB,GPIO_PIN_SOURCE_6,GPIO_AF_DCI);
    /** PE5,AF13  DCI_D6 */
    GPIO_ConfigPinAF(GPIOE,GPIO_PIN_SOURCE_5,GPIO_AF_DCI);
    /** PE6,AF13  DCI_D7 */
    GPIO_ConfigPinAF(GPIOE,GPIO_PIN_SOURCE_6,GPIO_AF_DCI);

    /** Reset DCI */
    DCI_Reset();

    /** Configure the DCI */
    /** continuous capture mode */
    dciConfigStruct.captureMode = DCI_CAPTURE_MODE_CONTINUOUS;
    /** capture all frame */
    dciConfigStruct.captureRate = DCI_CAPTURE_RATE_ALL_FRAME;
    /** 8 bytes extended data mode */
    dciConfigStruct.extendedDataMode = DCI_EXTENDED_DATA_MODE_8B;
    /** HSYNC low level effective */
    dciConfigStruct.hsyncPolarity = DCI_HSYNC_POL_LOW;
    /** PCLK clock polarity rising edge effective */
    dciConfigStruct.pckPolarity = DCI_PCK_POL_RISING;
    /** hardware sync mode */
    dciConfigStruct.synchroMode = DCI_SYNCHRO_MODE_HARDWARE;
    /** VSYNC low level effective */
    dciConfigStruct.vsyncPolarity = DCI_VSYNC_POL_LOW;
    DCI_Config(&dciConfigStruct);

    /** Enable DCI CC Interrupt */
    DCI_EnableInterrupt(DCI_INT_CC);
    NVIC_EnableIRQRequest(DCI_IRQn, 0, 1);

    /** Enable DCI */
    DCI_Enable();
}

/*!
 * @brief       DCI DMA Initialization
 *
 * @param       memoryBaseAddr: memory base addr
 *
 * @param       bufferSize: DMA transmission buffer size
 *
 * @param       memoryDataSize: memory data size
 *
 * @param       memoryInc : memory increase mode
 *
 * @param       memoryInc : memory brust mode
 *
 * @retval      None
 *
 */
void DCI_DMAInit(uint32_t memoryBaseAddr, uint32_t bufferSize, DMA_MEMORY_DATA_SIZE_T memoryDataSize, DMA_MEMORY_INC_T memoryInc,DMA_MEMORYBURST_T DMA_MemoryBurst)
{
    DMA_Config_T dciDmaConfigStruct;

    /** Enable the DMA2 Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    /** Reset DMA2 Stream1 */
    DMA_Reset(DMA2_Stream1);
    /** Wait DMA2 stream1 can be setting */
    while (DMA_ReadCmdStatus(DMA2_Stream1) != DISABLE)
    {
    }

    /** Configure the DMA Stream */
    /** DMA Stream 1 channel 1*/
    dciDmaConfigStruct.channel = DMA_CHANNEL_1;
    /** Set peripheral addr as DCI data register addr */
    dciDmaConfigStruct.peripheralBaseAddr = DCI_DATA_REG_ADDRESS;
    /** memory base addr */
    dciDmaConfigStruct.memoryBaseAddr = memoryBaseAddr;
    /** peripheral to memory mode */
    dciDmaConfigStruct.dir = DMA_DIR_PERIPHERALTOMEMORY;
    /** buffer size */
    dciDmaConfigStruct.bufferSize = bufferSize;
    /** peripheral no increase mode */
    dciDmaConfigStruct.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /** memory increase mode */
    dciDmaConfigStruct.memoryInc = memoryInc;
    /** set peripheral data size as 32 bits */
    dciDmaConfigStruct.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_WORD;
    /** memory data size */
    dciDmaConfigStruct.memoryDataSize = memoryDataSize;
    /** circular mode */
    dciDmaConfigStruct.loopMode = DMA_MODE_CIRCULAR;
    /** high priority mode */
    dciDmaConfigStruct.priority = DMA_PRIORITY_HIGH;
    /** enable FIFO mode */
    dciDmaConfigStruct.fifoMode = DMA_FIFOMODE_ENABLE;
    /** use FIFO threshold full mode */
    dciDmaConfigStruct.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    /** memory single burst */
    dciDmaConfigStruct.memoryBurst = DMA_MemoryBurst;
    /** peripheral single burst */
    dciDmaConfigStruct.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    /** init DAM stream */
    DMA_Config(DMA2_Stream1,&dciDmaConfigStruct);
}

/*!
 * @brief       Start DCI capture
 *
 * @param       None
 *
 * @retval      None
 *
 */
void DCI_Start(void)
{
    /** enable DMA2 stream 1 */
    DMA_Enable(DMA2_Stream1);
    /** enable DCI capture */
    DCI_EnableCapture();
}

/*!
 * @brief       Stop DCI capture
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void DCI_Stop(void)
{
    /** disable DCI capture */
    DCI_DisableCapture();
    /** wait last capture finish */
    while(DCI->CTRL & 0X01);

    /** disable DMA2 stream 1 */
    DMA_Disable(DMA2_Stream1);
}

