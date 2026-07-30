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
#include "apm32f4xx_pmu.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_fmc.h"
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf using USART1  */
#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

FMC_OPT_BOR_T BOR_LEVEL_Table[] = {
    FMC_OPT_BOR_LV3,    /*!< Supply voltage ranges from 2.7 to 3.6 V */
    FMC_OPT_BOR_LV2,    /*!< Supply voltage ranges from 2.4 to 2.7 V */
    FMC_OPT_BOR_LV1,    /*!< Supply voltage ranges from 2.1 to 2.4 V */
    FMC_OPT_BOR_OFF,    /*!< Supply voltage ranges from 1.62 to 2.1 V */
};

/* Private function prototypes ********************************************/

/* Delay */
void Delay(uint32_t count);

/* BOR*/
void BOR_Config(FMC_OPT_BOR_T level);

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
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_1);
    
    USART_Config_T usartConfigStruct;

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_PBInit(BUTTON_KEY1,BUTTON_MODE_GPIO);

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);

    printf("System reset\r\n");
    
    while (1)
    {
        Delay(0x3FFFFF);
        BOARD_LEDToggle(LED3);
        
        if(BOARD_PBGetState(BUTTON_KEY1) == RESET)
        {
            Delay(0x0FFFFF);
            if(BOARD_PBGetState(BUTTON_KEY1) == RESET)
            {
                 BOR_Config(BOR_LEVEL_Table[3]);
            }
            while(BOARD_PBGetState(BUTTON_KEY1) == RESET)
            {
            }
        }
    }
}

/*!
 * @brief     Configure Brown-out Reset
 *
 * @param     level :BOR level
 *              @arg FMC_OPT_BOR_LV3 : Supply voltage ranges from 2.7 to 3.6 V
 *              @arg FMC_OPT_BOR_LV2 : Supply voltage ranges from 2.4 to 2.7 V
 *              @arg FMC_OPT_BOR_LV1 : Supply voltage ranges from 2.1 to 2.4 V
 *              @arg FMC_OPT_BOR_OFF : Supply voltage ranges from 1.62 to 2.1 V
 *
 * @retval    None
 */
void BOR_Config(FMC_OPT_BOR_T level)
{
    uint8_t BOROptionByteTemp;
    
    /* Read BOR OB from flash*/
    BOROptionByteTemp = FMC_OPT_ReadBrownoutReset();

    if((BOROptionByteTemp & 0x0C) != level)
    {
        /* Unlocks option bytes block*/
        FMC_UnlockOptionByte();
        
        /* Configures the BOR level*/
        FMC_OPT_ConfigBrownoutReset(level);
        
        /* Launch the option byte loading*/
        FMC_OPT_Launch();
        
        /* Locks option bytes block*/
        FMC_LockOptionByte();
        
        printf("The BOR value is set from 0x%02X to 0x%02X\r\n",BOROptionByteTemp,level);
    }
    else
    {
        printf("The BOR setting is the same as in option bytes\r\n");
    }
}

/*!
 * @brief     Delay
 *
 * @param     count:  delay count
 *
 * @retval    None
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;
    
    while(delay--);
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
