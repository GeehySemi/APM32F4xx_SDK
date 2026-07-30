/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "main.h"
#include "Board.h"

/* Private includes *******************************************************/
#include "apm32f4xx_adc.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_rcm.h"
#include "board_delay.h"
#include "log_printf.h"
#include <stdio.h>

/* Private macro **********************************************************/
#define DEBUG_USART         USART1
#define ADC_CH_SIZE         3
#define ADC_DR_ADDR         ((uint32_t)ADC1_BASE + 0x4C)
/* Private typedef ********************************************************/

/* Private variables ******************************************************/
uint16_t adcData[ADC_CH_SIZE];

static const char* tag = "main"; 
/* Private function prototypes ********************************************/
void ADC_Init(void);
void ADC_MultiChannelPolling(void);

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
    USART_Config_T usartConfigStruct;

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    /* Configure LED GPIO Pin */
    BOARD_LEDInit(LED2);

    /* Configure Delay */
    BOARD_DelayConfig();

    /* ADC init */
    ADC_Init();

    while (1)
    {
        ADC_MultiChannelPolling();
        BOARD_DelayMs(500);
    }
}

/*!
 * @brief     DMA Init
 *
 * @param     None
 *
 * @retval    None
 */
void DMA_Init(void)
{
    DMA_Config_T dmaConfig;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    dmaConfig.peripheralBaseAddr = ADC_DR_ADDR;
    dmaConfig.memoryBaseAddr = (uint32_t)&adcData;
    dmaConfig.dir = DMA_DIR_PERIPHERALTOMEMORY;
    dmaConfig.bufferSize = ADC_CH_SIZE;
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_HALFWORD;
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_HALFWORD;
    dmaConfig.loopMode = DMA_MODE_CIRCULAR;

    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_HALFFULL;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.channel = DMA_CHANNEL_0;
    DMA_Config(DMA2_Stream0,&dmaConfig);

    DMA_Enable(DMA2_Stream0);
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
    GPIO_Config_T           gpioConfig;
    ADC_Config_T            adcConfig;
    ADC_CommonConfig_T      adcCommonConfig;

    /* Enable GPIOA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    /* ADC channel 0 configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable ADC clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC configuration */
    ADC_Reset();

    adcCommonConfig.mode            = ADC_MODE_INDEPENDENT;
    adcCommonConfig.prescaler       = ADC_PRESCALER_DIV4;
    adcCommonConfig.accessMode      = ADC_ACCESS_MODE_DISABLED;
    adcCommonConfig.twoSampling     = ADC_TWO_SAMPLING_20CYCLES;
    ADC_CommonConfig(&adcCommonConfig);

    ADC_ConfigStructInit(&adcConfig);
    adcConfig.resolution            = ADC_RESOLUTION_12BIT;
    adcConfig.scanConvMode          = ENABLE;
    adcConfig.continuousConvMode    = ENABLE;
    adcConfig.dataAlign             = ADC_DATA_ALIGN_RIGHT;
    adcConfig.extTrigEdge           = ADC_EXT_TRIG_EDGE_NONE;
    adcConfig.extTrigConv           = ADC_EXT_TRIG_CONV_TMR1_CC1;
    adcConfig.nbrOfChannel          = ADC_CH_SIZE;
    ADC_Config(ADC1, &adcConfig);

    /* ADC channel Convert configuration */
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_480CYCLES);
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_1, 2, ADC_SAMPLETIME_480CYCLES);
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_2, 3, ADC_SAMPLETIME_480CYCLES);

    /* Config DMA*/
    DMA_Init();

    /* Enable ADC DMA Request*/
    ADC_EnableDMARequest(ADC1);

    /* Enable ADC DMA*/
    ADC_EnableDMA(ADC1);

    /* Enable ADC */
    ADC_Enable(ADC1);

    /* ADC start conversion */
    ADC_SoftwareStartConv(ADC1);
}

/*!
 * @brief     ADC multi channel polling
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_MultiChannelPolling(void)
{
    float voltage;
    uint8_t index;

    for(index = 0; index < ADC_CH_SIZE; index++)
    {
        voltage = (adcData[index] * 3300.0 ) / 4095.0;

        LOGI(tag, "ADC CH[%d] voltage = %.3f mV\n", index, voltage);
    }
    printf("\r\n");
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

    /* wait for the data to be send  */
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

    /* wait for the data to be send  */
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
    int i;

    UNUSED(file);

    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif
