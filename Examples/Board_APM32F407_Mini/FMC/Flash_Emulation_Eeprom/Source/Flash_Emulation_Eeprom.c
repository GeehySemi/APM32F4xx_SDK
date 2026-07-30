/*!
 * @file        Flash_Emulation_Eeprom.c
 *
 * @brief       This file provides a flash emulated eeprom read/write interface
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
#include "Flash_Emulation_Eeprom.h"
#include "apm32f4xx_fmc.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* The buffer that write or erase sector data */
static uint8_t Flash_EE_Ram_Buffer[FLASH_SECTOR_SIZE];

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief     flash read byte.
 *
 * @param     Addr:  flash address.
 *
 * @retval    the data of assign address.
 */
static uint8_t Flash_EE_ReadByte(uint32_t Addr)
{
    return (*(__IO uint8_t*)Addr); 
}

/*!
 * @brief     Read the specified length of data from the specified address.
 *
 * @param     ReadAddr: read address.
 *
 * @param     pData: save the read data.
 *
 * @param     len: read data length.
 *
 * @retval    None. 
 *
 * @note      The example must be performed in the sectors 1~3.
 *
 */
void Flash_EE_Read(uint32_t ReadAddr, uint8_t* pData, uint32_t len)
{
    /* illegal address direct return */
    if ((ReadAddr < FLASH_EE_START_ADDR) || ((ReadAddr + len) > FLASH_EE_END_ADDR))
        return;
    
    /* illegal pointer direct return */
    if (pData == NULL)
        return;
    
    /* read data */
    for (uint32_t i = 0; i < len; i++)
    {
        pData[i] = Flash_EE_ReadByte(ReadAddr);
        ReadAddr += 1;
    }
}

/*!
 * @brief     Get sector number.
 *
 * @param     Addr:  Flash address.
 *                   The value of address must be between 1~3 sector.
 *
 * @retval    The sector number.
 */
static FMC_SECTOR_T Get_Flash_Sector_Num(uint32_t Addr)
{
    if(Addr<ADDR_FLASH_SECTOR_2) return FMC_SECTOR_1;
    else if(Addr<ADDR_FLASH_SECTOR_3) return FMC_SECTOR_2;
    else return FMC_SECTOR_3;
}

/*!
 * @brief     In a sector, write the specified length of data from the specified address.
 *
 * @param     WriteAddr: write address.
 *
 * @param     pData: save the write data.
 *
 * @param     len: write data length.
 *
 * @retval    None. 
 *
 * @note      The example must be performed in the sectors 1~3.
 *
 */
static void Flash_EE_WriteOneSector(uint32_t WriteAddr, const uint8_t* pData, uint32_t len)
{
    uint32_t isErase = 0;
    uint32_t startAddr;
    uint32_t offesetAddr;
    uint32_t i = 0;

    startAddr = WriteAddr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
    offesetAddr = WriteAddr % FLASH_SECTOR_SIZE;
    
    /* illegal address direct return */
    if ((WriteAddr < FLASH_EE_START_ADDR) || ((WriteAddr + len) > FLASH_EE_END_ADDR))
        return;
    
    /* illegal pointer direct return */
    if (pData == NULL)
        return;
    
    /* unlock flash for erase or write*/
    FMC_Unlock();
    
    /* check whether the sector need to be erased */
    for (i=0; i<len; i++)
    {
        if (Flash_EE_ReadByte(WriteAddr + i) != 0xFF)
        {
            isErase = 1;
            break;
        }
    }
    
    /* the sector needs to be erase */
    if (isErase == 1)
    {
        /* read the entire sector data to the buffer before write or erase */
        Flash_EE_Read(startAddr, Flash_EE_Ram_Buffer, FLASH_SECTOR_SIZE);
        
        /* copy the data to the buffer */
        for (i=0; i<len; i++)
        {
            Flash_EE_Ram_Buffer[offesetAddr + i] = pData[i];
        }
        
        /* erase the sector where the address is located */
        FMC_EraseSector(Get_Flash_Sector_Num(WriteAddr), FMC_VOLTAGE_3);
        
        /* write the entire sector data */
        for (i=0; i<FLASH_SECTOR_SIZE; i++)
        {
            FMC_ProgramByte(startAddr, Flash_EE_Ram_Buffer[i]);
            startAddr += 1;
        }
    }
    /* the sector don't need to be erase */
    else
    {
        /* write n bytes of data to the sector */
        for (i=0; i<len; i++)
        {
            FMC_ProgramByte(WriteAddr, pData[i]);
            WriteAddr += 1;
        }
    }
    
    /* lock flash */
    FMC_Lock();
}

/*!
 * @brief     Write the specified length of data from the specified address.
 *            Can be written across sectors.
 *
 * @param     WriteAddr: write address.
 *
 * @param     pData: save the write data.
 *
 * @param     len: write data length.
 *
 * @retval    None.
 *
 * @note      The example must be performed in the sectors 1~3.
 *
 */
void Flash_EE_Write(uint32_t WriteAddr, uint8_t* pData, uint32_t len)
{
    uint32_t NumOfSector = 0, NumOfByte = 0, OfsetAddr = 0;
    uint32_t count = 0, temp = 0;
    
    /* offerset address in the sector */
    OfsetAddr = WriteAddr % FLASH_SECTOR_SIZE;
    
    /* The size of the remaining space inthe sector from WriteAddr */
    count = FLASH_SECTOR_SIZE - OfsetAddr;
    
    /* Calculate how many sectors to write */
    NumOfSector =  len / FLASH_SECTOR_SIZE;
    
    /* Calculate how many bytes are left less than one sector */
    NumOfByte = len % FLASH_SECTOR_SIZE;
    
    /* OfsetAddr = 0, WriteAddr is sector aligned */
    if (OfsetAddr == 0)
    {
        /* len < FLASH_SECTOR_SIZE */
        if (NumOfSector == 0) 
        {
            Flash_EE_WriteOneSector(WriteAddr, pData, len);
        }
        /* len > FLASH_SECTOR_SIZE */
        else 
        {
            /* write NumOfSector sector */
            while (NumOfSector--)
            {
                Flash_EE_WriteOneSector(WriteAddr, pData, FLASH_SECTOR_SIZE);
                WriteAddr +=  FLASH_SECTOR_SIZE;
                pData += FLASH_SECTOR_SIZE;
            }
            
            /* write remaining data */
            Flash_EE_WriteOneSector(WriteAddr, pData, NumOfByte);
        }
    }
    /* OfsetAddr != 0, WriteAddr is not sector aligned */
    else 
    {
        /* len < FLASH_SECTOR_SIZE, the data length is less than one sector */
        if (NumOfSector == 0)
        {
            /* NumOfByte > count,  need to write across the sector */
            if (NumOfByte > count) 
            {
                temp = NumOfByte - count;
                /* fill the current sector */
                Flash_EE_WriteOneSector(WriteAddr, pData, count);
                
                WriteAddr +=  count;
                pData += count;
                /* write remaining data */
                Flash_EE_WriteOneSector(WriteAddr, pData, temp);
            }
            else 
            {
                Flash_EE_WriteOneSector(WriteAddr, pData, len);
            }
        }
        /* len > FLASH_SECTOR_SIZE */
        else 
        {
            len -= count;
            NumOfSector =  len / FLASH_SECTOR_SIZE;
            NumOfByte = len % FLASH_SECTOR_SIZE;
                
            /* write count data */
            Flash_EE_WriteOneSector(WriteAddr, pData, count);
            
            WriteAddr +=  count;
            pData += count;
            
            /* write NumOfSector sector */
            while (NumOfSector--)
            {
                Flash_EE_WriteOneSector(WriteAddr, pData, FLASH_SECTOR_SIZE);
                WriteAddr +=  FLASH_SECTOR_SIZE;
                pData += FLASH_SECTOR_SIZE;
            }
            
            if (NumOfByte != 0)
            {
                Flash_EE_WriteOneSector(WriteAddr, pData, NumOfByte);
            }
        }
    }
}

