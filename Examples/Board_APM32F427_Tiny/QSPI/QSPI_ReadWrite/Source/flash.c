/*!
 * @file        flash.c
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
#include "flash.h"
#include "qspi.h"
#include "main.h"
#include <string.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Sector size */
#define FLASH_SECTOR_SIZE (256)
/* Page size */
#define FLASH_PAGE_SISE (256)

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Buf */
static uint8_t s_buf[FLASH_SECTOR_SIZE];

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Read Status register 1
 *
 * @param       None
 *
 * @retval      Status register 1 value
 *
 */
uint8_t Flash_ReadSR1(void)
{
    uint8_t ret;

    QSPI_ChipSelect(1);

    QSPI_Std_WriteReadByte(FLASH_CMD_READ_SR1);
    ret = QSPI_Std_WriteReadByte(0XFF);

    QSPI_ChipSelect(0);

    return ret;
}

/*!
 * @brief       Write Status register 2
 *
 * @param       None
 *
 * @retval      Status register 2 value
 *
 */
void FLASH_WriteSR2(uint8_t sr2)
{
    uint8_t dat[3];

    dat[0] = FLASH_CMD_WRITE_SR1;
    dat[1] = 0x0;
    dat[2] = sr2;
    QSPI_Std_Write(dat, 3);
}

/*!
 * @brief       Read Status register 2
 *
 * @param       None
 *
 * @retval      Status register 2 value
 *
 */
uint8_t Flash_ReadSR2(void)
{
    uint8_t ret;

    QSPI_ChipSelect(1);

    QSPI_Std_WriteReadByte(FLASH_CMD_READ_SR2);
    ret = QSPI_Std_WriteReadByte(0Xff);

    QSPI_ChipSelect(0);

    return ret;
}

/*!
 * @brief       Flash init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void FLASH_Init(void)
{
    uint8_t sr2;

    QSPI_Init();

    /* Set QE bit */
    sr2 = Flash_ReadSR2();

    if (!(sr2 & 0x02))
    {
        FLASH_WriteEnable();
        sr2 |= 0X02;
        FLASH_WriteSR2(sr2);
    }
}

/*!
 * @brief       Write enable
 *
 * @param       None
 *
 * @retval      None
 *
 */
void FLASH_WriteEnable(void)
{
    uint8_t cmd = FLASH_CMD_WRITE_ENABLE;
    uint8_t ret;

    while (1)
    {
        QSPI_Std_Write(&cmd, 1);

        ret = Flash_ReadSR1();

        if (ret & 0x02)
            break;

    }
}

/*!
 * @brief       Write disable
 *
 * @param       None
 *
 * @retval      None
 *
 */
void FLASH_WriteDisable(void)
{
    uint8_t cmd = FLASH_CMD_WRITE_DISABLE;

    QSPI_Std_Write(&cmd, 1);
}

/*!
 * @brief       Read manufacturer ID
 *
 * @param       None
 *
 * @retval      None
 *
 */
uint16_t FLASH_ReadID(void)
{
    uint8_t id[2];

    QSPI_ReadWriteParam_T param;

    param.addr = 0X000000;
    param.addrLen = QSPI_ADDR_LEN_24BIT;
    param.dataBuf = id;
    param.dataLen = 2;
    param.instAddrType = QSPI_INST_TYPE_STANDARD;

    param.instLen = QSPI_INST_LEN_8BIT;
    param.instruction = FLASH_CMD_MAUFACT_DEVICE_ID_QAUD;

    param.waitCycle = 6;

    QSPI_Quad_Read(&param);

    return ((id[0] << 8) | id[1]);
}

/*!
 * @brief       Read array
 *
 * @param       addr: Flash address
 *
 * @param       rBuf: Read buffer
 *
 * @param       rLen: Read data length
 *
 * @retval      None
 *
 */
void Flash_Read(uint32_t addr, uint8_t *rBuf, uint16_t rLen)
{
    QSPI_ReadWriteParam_T param;

    param.addr = addr;
    param.addrLen = QSPI_ADDR_LEN_24BIT;
    param.dataBuf = rBuf;
    param.dataLen = rLen;
    param.instAddrType = QSPI_INST_ADDR_TYPE_STANDARD;
    param.instLen = QSPI_INST_LEN_8BIT;
    param.instruction = FLASH_CMD_FASH_READ_DUAD;
    param.waitCycle = 8;

    QSPI_Quad_Read(&param);
}

/*!
 * @brief       Wait busy
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Flash_WaitBusy(void)
{
    while (Flash_ReadSR1() & 0x01);

}

/*!
 * @brief       Write page
 *
 * @param       addr:   Flash address
 *
 * @param       wBuf:   Write Buffer
 *
 * @param       wLen:   Buffer length
 *
 * @retval      None
 *
 */
void Flash_WritePage(uint32_t addr, uint8_t *wBuf, uint16_t wLen)
{
    QSPI_ReadWriteParam_T param;

    FLASH_WriteEnable();

    param.addr = addr;
    param.addrLen = QSPI_ADDR_LEN_24BIT;
    param.dataBuf = wBuf;
    param.dataLen = wLen;
    param.instAddrType = QSPI_INST_ADDR_TYPE_STANDARD;
    param.instLen = QSPI_INST_LEN_8BIT;
    param.instruction = FLASH_CMD_PAGE_PROGRAM_QUAD;

    QSPI_Quad_Write(&param);

    Flash_WaitBusy();
}

/*!
 * @brief       Erase sector
 *
 * @param       addr
 *
 * @retval      None
 *
 */
void FLASH_EraseSector(uint32_t addr)
{
    uint8_t dat[4];

    FLASH_WriteEnable();
    Flash_WaitBusy();

    dat[0] = FLASH_CMD_SECTOR_ERASE;
    dat[1] = (addr >> 16) & 0xff;
    dat[2] = (addr >> 8) & 0xff;
    dat[3] = addr & 0xff;

    QSPI_Std_Write(dat, 4);

    Flash_WaitBusy();
}


void FLASH_EraseChip()
{
    uint8_t cmd = FLASH_CMD_CHIP_ERASE;

    FLASH_WriteEnable();

    Flash_WaitBusy();

    QSPI_Std_Write(&cmd, 1);

    Flash_WaitBusy();
}

/*!
 * @brief       Write
 *
 * @param       addr:   Flash address
 *
 * @param       wBuf:   Write Buffer
 *
 * @param       wLen:   Buffer length
 *
 * @retval      None
 *
 */
void Flash_Write(uint32_t addr, uint8_t *wBuf, uint16_t wLen)
{
    uint16_t tmp;
    uint32_t sectorOffset;
    uint32_t sectorRemain;

    sectorOffset = addr % FLASH_SECTOR_SIZE;
    sectorRemain = FLASH_SECTOR_SIZE - sectorOffset;

    if (wLen <= sectorRemain)
    {
        sectorRemain = wLen;
    }

    while (wLen)
    {
        if (!sectorOffset)
        {
            FLASH_EraseSector(addr);
        }

        while (sectorRemain)
        {
            tmp = sectorRemain > FLASH_PAGE_SISE ? FLASH_PAGE_SISE : sectorRemain;
            Flash_WritePage(addr, wBuf, tmp);

            addr += tmp;
            wBuf += tmp;
            wLen -= tmp;
            sectorRemain -= tmp;
        }

        sectorRemain = wLen >= FLASH_SECTOR_SIZE ? FLASH_SECTOR_SIZE : wLen;
        sectorOffset = addr % FLASH_SECTOR_SIZE;
    }
}

/*!
 * @brief       FLASH read write test
 *
 * @param       None
 *
 * @retval      0: Test ok; 1: Test Error;
 *
 */
uint8_t FLASH_ReadWriteTest(void)
{
    uint16_t i;

    for (i = 0; i < sizeof(s_buf); i++)
    {
        s_buf[i] = 0x25;
    }

    FLASH_EraseSector(0x8000);

    Flash_Write(0x8000, s_buf, sizeof(s_buf));

    for (i = 0; i < sizeof(s_buf); i++)
    {
        s_buf[i] = 0;
    }

    Flash_Read(0x8000, s_buf, sizeof(s_buf));

    for (i = 0; i < sizeof(s_buf); i++)
    {
        if (s_buf[i] != (0x25 & 0xff))
        {
            return 1;
        }
    }

    return 0;
}
