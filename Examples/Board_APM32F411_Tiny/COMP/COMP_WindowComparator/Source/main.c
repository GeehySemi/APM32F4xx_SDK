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
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_comp.h"
#include "apm32f4xx_usart.h"
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
uint32_t comp1OutVal = 0U;
uint32_t comp2OutVal = 0U;

/* Private function prototypes ********************************************/

void COMP_Init(void);
void GPIO_Init(void);

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    /* USART Initialization */
    SystemClockConfig();
    USART_Config_T usartConfigStruct;

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);
    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    BOARD_DelayConfig();

    COMP_Init();

    while (1)
    {
        comp1OutVal = COMP_ReadOutPutLevel(COMP_SELECT_COMP1);
        comp2OutVal = COMP_ReadOutPutLevel(COMP_SELECT_COMP2);

        if (comp1OutVal != 0U)
        {
            BOARD_LEDToggle(LED2);
        }

        if (comp2OutVal != 0U)
        {
            BOARD_LEDToggle(LED3);
        }
        BOARD_DelayMs(500);
    }
}

/**
 * @brief   GPIO init
 *
 * @param   None
 *
 * @retval  None
 */
void GPIO_Init(void)
{
    GPIO_Config_T  GPIO_InitStructure;
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC);

    /* PC1   ------>   COMP1_INM */
    GPIO_InitStructure.pin = GPIO_PIN_1;
    GPIO_InitStructure.mode = GPIO_MODE_AN;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);

    /* PC3   ------>   COMP2_INM */
    /* PC2   ------>   COMP2_INP */
    GPIO_InitStructure.pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.mode = GPIO_MODE_AN;
    GPIO_InitStructure.otype = GPIO_OTYPE_PP;
    GPIO_InitStructure.speed = GPIO_SPEED_100MHz;
    GPIO_InitStructure.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief   COMP init
 *
 * @param   None
 *
 * @retval  None
 */
void COMP_Init(void)
{
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_COMP);

    COMP_Config_T compConfig;
    compConfig.invertingInput = COMP_INVERTING_INPUT_PC1_3;
    compConfig.mode = COMP_MODE_LOWSPEED;
    compConfig.output = COMP_OUTPUT_NONE;
    compConfig.outputPol = COMP_OUTPUTPOL_NONINVERTED;
    COMP_Config(COMP_SELECT_COMP1, &compConfig);

    COMP_EnableWindow();
    COMP_Enable(COMP_SELECT_COMP1);
    COMP_ConfigLOCK(COMP_SELECT_COMP1);

    compConfig.invertingInput = COMP_INVERTING_INPUT_PC1_3;
    COMP_Enable(COMP_SELECT_COMP2);
    COMP_ConfigLOCK(COMP_SELECT_COMP2);
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
