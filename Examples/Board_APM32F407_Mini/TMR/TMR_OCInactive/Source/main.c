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

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

#define DIV 83

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

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
    SystemClockConfig();
    GPIO_Config_T GPIO_ConfigStruct;
    TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA|RCM_AHB1_PERIPH_GPIOC);

    /* TMR3_CH1 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_6, GPIO_AF_TMR3);
    /* TMR3_CH2 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_7, GPIO_AF_TMR3);
    /* TMR3_CH3 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_8, GPIO_AF_TMR3);
    /* TMR3_CH4 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_9, GPIO_AF_TMR3);

    /* Config TMR3 GPIO for output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_6| GPIO_PIN_7| GPIO_PIN_8| GPIO_PIN_9;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.otype = GPIO_OTYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_100MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOC, &GPIO_ConfigStruct);

    /* Config GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3|GPIO_PIN_4;
    GPIO_ConfigStruct.mode = GPIO_MODE_OUT;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* config TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = DIV;
    TMR_TimeBaseStruct.period = 999;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    /* Configure channel1 */
    TMR_ConfigOCStructInit(&OCcongigStruct);

    OCcongigStruct.mode = TMR_OC_MODE_INACTIVE;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;

    /* TMR3 CH1 delay = 800/1MHz = 800us */
    OCcongigStruct.pulse = 800;
    TMR_ConfigOC1(TMR3, &OCcongigStruct);
    TMR_ConfigOC1Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH2 delay = 600/1MHz = 600us */
    OCcongigStruct.pulse = 600;
    TMR_ConfigOC2(TMR3, &OCcongigStruct);
    TMR_ConfigOC2Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH3 delay = 400/1MHz = 400us */
    OCcongigStruct.pulse = 400;
    TMR_ConfigOC3(TMR3, &OCcongigStruct);
    TMR_ConfigOC3Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH4 delay = 200/1MHz = 200us */
    OCcongigStruct.pulse = 200;
    TMR_ConfigOC4(TMR3, &OCcongigStruct);
    TMR_ConfigOC4Preload(TMR3, TMR_OC_PRELOAD_DISABLE);

    TMR_DisableAutoReload(TMR3);

    /* generate raising edge */
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_1, 1);
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_2, 1);
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_3, 1);
    GPIO_WriteBitValue(GPIOA, GPIO_PIN_4, 1);

    /* TMR4 Commutation Interrupt */
    NVIC_EnableIRQ(TMR3_IRQn);

    /* start generate four different signals */
    TMR_Enable(TMR3);

    TMR_EnableInterrupt(TMR3,TMR_INT_CC1|TMR_INT_CC2|TMR_INT_CC3|TMR_INT_CC4);

    while(1)
    {
    }
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
