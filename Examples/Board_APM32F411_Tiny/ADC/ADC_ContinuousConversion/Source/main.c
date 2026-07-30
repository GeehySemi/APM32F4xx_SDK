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
#include "stdio.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_adc.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_pmu.h"
#include "apm32f4xx_rcm.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* ADC AWD Active Flag */
uint8_t iAWD_Flag = 0;

/* Private function prototypes ********************************************/

/* ADC init */
void ADC_Init(void);

/* Delay */
void Delay(uint32_t count);

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
    BOARD_LEDInit(LED3);
    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    /* ADC init */
    ADC_Init();

    while (1)
    {
    }
}

/*!
 * @brief     ADC Init
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_Init(void)
{
    GPIO_Config_T   gpioConfig;
    ADC_Config_T    adcConfig;
    ADC_CommonConfig_T  adcCommonConfig;

    /* Enable GPIOA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* ADC channel 0 configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_0;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable ADC clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC configuration */
    ADC_Reset();
    ADC_ConfigStructInit(&adcConfig);
    adcConfig.resolution          = ADC_RESOLUTION_12BIT;
    adcConfig.continuousConvMode  = ENABLE;
    adcConfig.dataAlign           = ADC_DATA_ALIGN_RIGHT;
    adcConfig.extTrigEdge         = ADC_EXT_TRIG_EDGE_NONE;
    adcConfig.scanConvMode        = DISABLE;
    ADC_Config(ADC1, &adcConfig);

    ADC_CommonConfigStructInit(&adcCommonConfig);
    /* Set ADC Clock Prescaler */
    adcCommonConfig.prescaler = ADC_PRESCALER_DIV4;
    ADC_CommonConfig(ADC1, &adcCommonConfig);

    /* ADC channel 10 Convert configuration */
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_112CYCLES);

    /* Enable complete conversion interupt and Analog watchdog interrupt */
    ADC_EnableInterrupt(ADC1, ADC_INT_EOC);

    /* NVIC configuration */
    NVIC_EnableIRQRequest(ADC_IRQn, 1, 1);

    /* Enable ADC */
    ADC_Enable(ADC1);

    /* ADC start conversion */
    ADC_SoftwareStartConv(ADC1);
}

/*!
 * @brief     ADC interrupt service routine
 *
 * @param     None
 *
 * @retval    None
 */

void ADC_Isr(void)
{
    uint16_t adcData = 0;
    uint16_t voltage = 0;

    if (ADC_ReadIntFlag(ADC1, ADC_INT_FLAG_EOC))
    {
        ADC_ClearStatusFlag(ADC1, ADC_FLAG_EOC);
        adcData = ADC_ReadConversionValue(ADC1);
        voltage = (adcData * 3300) / 4095;
        printf("\r\n voltage : %d mV\r\n", voltage);
    }
}

/*!
 * @brief       Delay
 *
 * @param       count:  delay count
 *
 * @retval      None
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
