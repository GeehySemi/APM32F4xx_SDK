/*!
 * @file       Board_Flash.c
 *
 * @brief      This file provides spi read and write Flash functions
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
#include "Board_W25Q16.h"
#include "board.h"
#include "stdio.h"
#include "apm32f4xx_spi.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       SPI Initialization
 *
 * @param       None
 *
 * @retval      None
 */
void SPI_Init(void)
{
    GPIO_Config_T GPIO_InitStructure;
    SPI_Config_T  SPI1_InitStructure;

    /** Enable related Clock */
    RCM_EnableAHB1PeriphClock (RCM_AHB1_PERIPH_GPIOF| RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    /** Config SPI1 PinAF */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_3, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_5, GPIO_AF_SPI1);

    /** Config SPI1, PIN CS=PF5, SCK=PA3, MISO=PA4, MOSI=PA5 */
    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_AF;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

    GPIO_ConfigStructInit(&GPIO_InitStructure);
    GPIO_InitStructure.pin = GPIO_PIN_5;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.mode = GPIO_MODE_OUT;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOF, &GPIO_InitStructure);
    GPIO_WriteBitValue(GPIOF, GPIO_PIN_5, BIT_SET);

    /** Config SPI1 */
    SPI_ConfigStructInit(&SPI1_InitStructure);
    SPI1_InitStructure.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    SPI1_InitStructure.mode = SPI_MODE_MASTER;
    SPI1_InitStructure.length = SPI_DATA_LENGTH_8B;
    SPI1_InitStructure.polarity = SPI_CLKPOL_HIGH;
    SPI1_InitStructure.phase = SPI_CLKPHA_2EDGE;
    SPI1_InitStructure.nss = SPI_NSS_SOFT;
    SPI1_InitStructure.baudrateDiv = SPI_BAUDRATE_DIV_2;
    SPI1_InitStructure.firstBit = SPI_FIRSTBIT_MSB;
    SPI1_InitStructure.crcPolynomial = 7;
    SPI_Config(SPI1, &SPI1_InitStructure);
    SPI_DisableCRC(SPI1);
    SPI_EnableSSOutput(SPI1);

    /** Enable SPI  */
    SPI_Enable(SPI1);
}

/*!
 * @brief       Erase Falsh Sector
 *
 * @param       SectorAddr:Sector Addr
 *
 * @retval      None
 */
void FalshEraseSector(uint32_t SectorAddr)
{
    /** Flash Write Enable  */
    EnableFlashWrite();
    WaitFlashWriteEnd();

    /** Select the FLASH */
    SPI_FLASH_CS_LOW();

    /** send cmd */
    FlashSendByte(W25Q16_SectorErase);

    /** send Sector Addr */
    FlashSendByte((SectorAddr & 0xFF0000) >> 16);
    FlashSendByte((SectorAddr & 0xFF00) >> 8);
    FlashSendByte(SectorAddr & 0xFF);

    /** Deselect the FLASH */
    SPI_FLASH_CS_HIGH();

    /** wait Falsh Erase end */
    WaitFlashWriteEnd();
}

/*!
 * @brief       Flash Write Page
 *
 * @param       pBuffer: pointer to the Write buffer
 *
 * @param       WriteAddr: Write the flash Address
 *
 * @param       NumToWrite: the number of byte to write
 *
 * @retval      None
 */
void FlashWritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumToWrite)
{
    uint16_t i = 0;

    /** Enable Flash Write */
    EnableFlashWrite();

    /** Select the FLASH */
    SPI_FLASH_CS_LOW();

    /** W25Q16 Page Program */
    FlashSendByte(W25Q16_PageProgram);
    FlashSendByte((WriteAddr & 0xFF0000) >> 16);
    FlashSendByte((WriteAddr & 0xFF00) >> 8);
    FlashSendByte(WriteAddr & 0xFF);

    if(NumToWrite > FLASH_PAGE_SIZE)
    {
        NumToWrite = FLASH_PAGE_SIZE;
    }

    /** Write data to flash*/
    for(i = 0; i < NumToWrite; i++)
    {
        FlashSendByte(*pBuffer);
        pBuffer++;
    }

    /** Deselect the FLASH */
    SPI_FLASH_CS_HIGH();

    /** Wait Write End*/
    WaitFlashWriteEnd();
}

/*!
 * @brief       Flash Write Buffer
 *
 * @param       pBuffer: pointer to the Write buffer
 *
 * @param       WriteAddr: Write the flash Address
 *
 * @param       NumToWrite: the number of byte to write
 *
 * @retval      None
 */
void FlashWriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumToWrite)
{
    uint8_t i = 0;
    uint8_t PageNumber = 0;
    uint8_t RemainNumber = 0;
    uint8_t PageRemainNumber = 0;

    /** complete Page need to write */
    PageNumber =  NumToWrite / FLASH_PAGE_SIZE;

    /** Remain byte */
    RemainNumber = NumToWrite % FLASH_PAGE_SIZE;

    /** if WriteAddr align */
    if ((WriteAddr % FLASH_PAGE_SIZE) == 0)
    {
        /** NumToWrite < FLASH_PAGE_SIZE */
        if (PageNumber == 0)
        {
            FlashWritePage(pBuffer, WriteAddr, NumToWrite);
        }
        else
        {
            /** write complete Page */
            for(i = 0; i < PageNumber; i++)
            {
                FlashWritePage(pBuffer, WriteAddr, FLASH_PAGE_SIZE);
                WriteAddr +=  FLASH_PAGE_SIZE;
                pBuffer += FLASH_PAGE_SIZE;
            }

            /** write remain data */
            if (RemainNumber > 0)
            {
                FlashWritePage(pBuffer, WriteAddr, RemainNumber);
            }
        }
    }
    else
    {
        /** Write Address Page Remain Number */
        PageRemainNumber = FLASH_PAGE_SIZE - (WriteAddr % FLASH_PAGE_SIZE);

        /** NumToWrite < FLASH_PAGE_SIZE */
        if (PageNumber == 0)
        {
            if (RemainNumber > PageRemainNumber)
            {
                /** write all over the current page */
                FlashWritePage(pBuffer, WriteAddr, PageRemainNumber);
                WriteAddr +=  PageRemainNumber;
                pBuffer += PageRemainNumber;

                RemainNumber = RemainNumber - PageRemainNumber;;

                /** write remain data */
                FlashWritePage(pBuffer, WriteAddr, RemainNumber);
            }
            else
            {
                FlashWritePage(pBuffer, WriteAddr, RemainNumber);
            }
        }
        else
        {
            /** write all over the current page */
            FlashWritePage(pBuffer, WriteAddr, PageRemainNumber);
            WriteAddr +=  PageRemainNumber;
            pBuffer += PageRemainNumber;

            NumToWrite -= PageRemainNumber;
            PageNumber =  NumToWrite / FLASH_PAGE_SIZE;
            RemainNumber = NumToWrite % FLASH_PAGE_SIZE;

            /** write complete Page */
            for(i = 0; i < PageNumber; i++)
            {
                FlashWritePage(pBuffer, WriteAddr, FLASH_PAGE_SIZE);
                WriteAddr +=  FLASH_PAGE_SIZE;
                pBuffer += FLASH_PAGE_SIZE;
            }

            /** write remain data */
            if (RemainNumber > 0)
            {
                FlashWritePage(pBuffer, WriteAddr, RemainNumber);
            }
        }
    }
}

/*!
 * @brief       Flash Read Buffer
 *
 * @param       pBuffer: pointer to the read buffer
 *
 * @param       ReadAddr: read the flash Address
 *
 * @param       NumToWrite: the number of byte to read
 *
 * @retval      None
 */
void FlashReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumToRead)
{
    uint16_t i = 0;

    /** Select the FLASH */
    SPI_FLASH_CS_LOW();

    /** send read cmd */
    FlashSendByte(W25Q16_ReadData);
    FlashSendByte((ReadAddr & 0xFF0000) >> 16);
    FlashSendByte((ReadAddr& 0xFF00) >> 8);
    FlashSendByte(ReadAddr & 0xFF);

    /** read data to flash*/
    for(i = 0; i < NumToRead; i++)
    {
        *pBuffer = FlashSendByte(Dummy_Byte);
        pBuffer++;
    }

    /** Deselect the FLASH */
    SPI_FLASH_CS_HIGH();
}


/*!
 * @brief       Flash Read ID
 *
 * @param       None
 *
 * @retval      None
 */
uint32_t ReadFlashID(void)
{
    uint32_t TempBuffer[3] = {0};
    uint32_t FlashID = 0;

    /** Select the FLASH */
    SPI_FLASH_CS_LOW();

    /** send read ID cmd */
    FlashSendByte(W25Q16_JedecDeviceID);
    TempBuffer[0] = FlashSendByte(Dummy_Byte);
    TempBuffer[1] = FlashSendByte(Dummy_Byte);
    TempBuffer[2] = FlashSendByte(Dummy_Byte);

    /** Deselect the FLASH */
    SPI_FLASH_CS_HIGH();

    FlashID = (TempBuffer[0] << 16) | (TempBuffer[1] << 8) | TempBuffer[2];

    return FlashID;
}

/*!
 * @brief       Read Flash DeviceID
 *
 * @param       None
 *
 * @retval      None
 */
uint32_t ReadFlashDeviceID(void)
{
    uint32_t DeviceID = 0;

    /** Select the FLASH */
    SPI_FLASH_CS_LOW();

    /** Send W25Q16 DeviceID cmd */
    FlashSendByte(W25Q16_DeviceID);
    FlashSendByte(Dummy_Byte);
    FlashSendByte(Dummy_Byte);
    FlashSendByte(Dummy_Byte);

    /** Read DeviceID from the FLASH */
    DeviceID = FlashSendByte(Dummy_Byte);

    /** Deselect the FLASH */
    SPI_FLASH_CS_HIGH();

    return DeviceID;
}

/*!
 * @brief       SPI read Byte from flash
 *
 * @param       None
 *
 * @retval      None
 */
uint8_t FlashReadByte(void)
{
    uint8_t temp = 0;

    temp = FlashSendByte(Dummy_Byte);

    return temp;
}

/*!
 * @brief       SPI Send Byte to flash
 *
 * @param       None
 *
 * @retval      None
 */
uint8_t FlashSendByte(uint8_t data)
{
    /** SPI master send data */
    while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE) == RESET);

    SPI_I2S_TxData(SPI1, data);

    /** SPI1 slave receive data */
    while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE) == RESET);

    return SPI_I2S_RxData(SPI1);
}

/*!
 * @brief       Enable Flash Write
 *
 * @param       None
 *
 * @retval      None
 */
void EnableFlashWrite(void)
{
    SPI_FLASH_CS_LOW();

    /** send W25Q16 Write Enable cmd */
    FlashSendByte(W25Q16_WriteEnable);

    SPI_FLASH_CS_HIGH();
}

/*!
 * @brief       Wait Flash Write End
 *
 * @param       None
 *
 * @retval      None
 */
void WaitFlashWriteEnd(void)
{
    uint8_t RegStatus = 0;

    SPI_FLASH_CS_LOW();

    /** send Read W25Q16 Status cmd */
    FlashSendByte(W25Q16_ReadStatusReg);

    do
    {
        /** Read W25Q16 Status Reg */
        RegStatus = FlashSendByte(Dummy_Byte);
    }
    while ((RegStatus & WIP_Flag) != 0);

    SPI_FLASH_CS_HIGH();
}

/*!
 * @brief       Flash Into PowerDown
 *
 * @param       None
 *
 * @retval      None
 */
void FlashIntoPowerDown(void)
{
    SPI_FLASH_CS_LOW();

    /** send PowerDown cmd*/
    FlashSendByte(W25Q16_PowerDown);

    SPI_FLASH_CS_HIGH();
}
