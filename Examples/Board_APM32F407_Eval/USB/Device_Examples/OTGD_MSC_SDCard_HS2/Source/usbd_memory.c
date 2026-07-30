/*!
 * @file        usbd_memory.c
 *
 * @brief       USB device memory management program body
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
#include "usbd_memory.h"
#include "board_sdcard.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/
#define MEMORY_LUN_NUM              1
#define MEMORY_BLOCK_NUM            10000
#define MEMORY_BLOCK_SIZE           512

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* USB Mass storage Standard Inquiry Data */
const uint8_t memoryInquiryData[] =
{
    /* lun 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (USBD_LEN_STD_INQUIRY - 5),
    0x00,
    0x00,
    0x00,
    /* Manufacturer : 8 bytes */
    'G', 'e', 'e', 'h', 'y', ' ', ' ', ' ',
    /* Product : 16 Bytes */
    'S', 't', 'o', 'r', 'a', 'g', 'e', ' ',
    'D', 'i', 's', 'k', ' ', ' ', ' ', ' ',
    /* Version : 4 Bytes */
    '1', '.', '0', '0',
};

/* USB FS MSC memory management handler */
USBD_MSC_MEMORY_T USBD_MEMORY_INTERFACE_HS =
{
    "MSC Memory HS",
    (uint8_t*)memoryInquiryData,
    USBD_HS_MemoryReadMaxLun,
    USBD_HS_MemoryInit,
    USBD_HS_MemoryReadCapacity,
    USBD_HS_MemoryCheckReady,
    USBD_HS_MemoryCheckWPR,
    USBD_HS_MemoryReadData,
    USBD_HS_MemoryWriteData,
};

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       USB device MSC memory unit init handler
 *
 * @param       lun: lun number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_HS_MemoryInit(uint8_t lun)
{
    UNUSED(lun);
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit read capacity handler
 *
 * @param       lun: lun number
 *
 * @param       blockNum: block number
 *
 * @param       blockSize: block size
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_HS_MemoryReadCapacity(uint8_t lun, uint32_t* blockNum, \
                                      uint16_t* blockSize)
{
    UNUSED(lun);
    USBD_STA_T usbStatus = USBD_OK;

    *blockNum = sdmmcInfo.logBlockNum;
    *blockSize = sdmmcInfo.logBlockSize;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit check read status handler
 *
 * @param       lun: lun number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_HS_MemoryCheckReady(uint8_t lun)
{
    UNUSED(lun);
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit check write protected status handler
 *
 * @param       lun: lun number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_HS_MemoryCheckWPR(uint8_t lun)
{
    UNUSED(lun);
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory read max LUN handler
 *
 * @param       None
 *
 * @retval      Max LUN number
 */
uint8_t USBD_HS_MemoryReadMaxLun(void)
{
    return (MEMORY_LUN_NUM - 1);
}

/*!
 * @brief       USB device MSC memory unit read data handler
 *
 * @param       lun: lun number
 *
 * @param       buffer: data buffer
 *
 * @param       blockAddr: block address
 *
 * @param       blockLength: block number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_HS_MemoryReadData(uint8_t lun, uint8_t* buffer, uint32_t blockAddr, \
                                  uint16_t blockLength)
{
    UNUSED(lun);
    USBD_STA_T usbStatus = USBD_OK;
    
    if(BOARD_SDReadBlocks(&sdmmcInfo, buffer, blockAddr, blockLength, 0xFFFF) != SD_OK) 
    {
        return (USBD_FAIL);
    }
    else
    {
        while(BOARD_SDReadState(&sdmmcInfo) == SD_STATE_BUSY);
        while(BOARD_SDReadCardState(&sdmmcInfo) != SD_CARD_TRANSFER);
    }

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit write data handler
 *
 * @param       lun: lun number
 *
 * @param       buffer: data buffer
 *
 * @param       blockAddr: block address
 *
 * @param       blockLength: block number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_HS_MemoryWriteData(uint8_t lun, uint8_t* buffer, uint32_t blockAddr, \
                                   uint16_t blockLength)
{
    UNUSED(lun);
    USBD_STA_T usbStatus = USBD_OK;

    if(BOARD_SDWriteBlocks(&sdmmcInfo, buffer, blockAddr, blockLength, 0xFFFF) != SD_OK)
    {
        return (USBD_FAIL);
    }
    else
    {
        while(BOARD_SDReadState(&sdmmcInfo) == SD_STATE_BUSY);
        while(BOARD_SDReadCardState(&sdmmcInfo) != SD_CARD_TRANSFER);
    }
    
    return usbStatus;
}

