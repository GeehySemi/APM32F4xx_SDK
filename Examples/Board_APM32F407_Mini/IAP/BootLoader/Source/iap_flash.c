/*!
 * @file        iap_flash.c
 *
 * @brief       Include IAP flash operation
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
#include "iap_flash.h"
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Unlocks Flash for write access
 *
 * @param       None
 *
 * @retval      None
 *
 */
void FLASH_IAP_Init(void)
{
    /* Unlock the flash memory */
    FMC_Unlock();

    /* Clear all FMC flags */
    FMC_ClearStatusFlag(FMC_FLAG_ENDOP | FMC_FLAG_ERROP \
    | FMC_FLAG_ERRPGA | FMC_FLAG_ERRPGP | FMC_FLAG_ERRPGS | FMC_FLAG_ERRWRP);
}

/*!
 * @brief       Erase of all user flash area
 *
 * @param       Application
 *              @arg APP1 : Application1
 *              @arg APP2 : Application2
 *
 * @retval      SUCCESS: user flash area successfully erased
 *              ERROR: error occurred
 */
uint32_t FLASH_IAP_Erase(APP_TypeDef Application)
{
    FMC_SECTOR_T flashSector;
    FMC_SECTOR_T i = FMC_SECTOR_0;

    if (Application == APP1)
    {
        flashSector = USER_APP1_START_SECTOR;
    }
    else
    {
        flashSector = USER_APP2_START_SECTOR;
    }

    for(i = flashSector; i <= FMC_SECTOR_7; i+= 8)
    {
        /* Using Voltage range 3*/
        if(FMC_EraseSector(i,FMC_VOLTAGE_3) != FMC_COMPLETE)
        {
            /* Error occurred while page erase */
            return (ERROR);
        }
    }

    return (SUCCESS);
}

/*!
 * @brief       Write the appoint data buffer in flash
 *
 * @param       address: start address for writing data buffer
 *
 * @param       data: pointer on data buffer
 *
 * @param       length: length of data buffer (unit is 32-bit word)
 *
 * @param       Application
 *              @arg APP1 : Application1
 *              @arg APP2 : Application2
 *
 * @retval      SUCCESS: data successfully written to Flash memory
 *              ERROR: Error occurred while writing data in Flash memory
 *
 * @note        buffer data are 32-bit aligned
 */
uint32_t FLASH_IAP_Write(__IO uint32_t* address, uint32_t* data, uint16_t length, APP_TypeDef Application)
{
    uint32_t i = 0;
    uint32_t addressEnd;

    if (Application == APP1)
    {
        addressEnd = USER_APP1_END_ADDRESS - 4;
    }
    else
    {
        addressEnd = USER_APP2_END_ADDRESS - 4;
    }

    for (i = 0; i < length; i++)
    {
        if (*address > addressEnd)
        {
            return (ERROR);
        }

        /* The operation will be done by word and Check the written value*/
        if ((FMC_ProgramWord(*address, *(uint32_t*)(data + i)) != FMC_COMPLETE) \
            || (*(uint32_t*)*address != *(uint32_t*)(data + i)))
        {
            return (ERROR);
        }

        *address += 4;
    }

    return (SUCCESS);
}
