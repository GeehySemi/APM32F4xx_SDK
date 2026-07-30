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

#define BufSize 0x100

#define REG_ADDRESS 0x4001204C      //adc1 data register

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

const volatile uint32_t* value = (uint32_t*)REG_ADDRESS;

/* Private function prototypes ********************************************/

/* Delay */
void Delay(uint32_t count);
/* ADC Init */
void ADC_Init(void);
/* DMA Init */
void DMA_Init(uint32_t* Buf);

uint32_t getValue(void);

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
    /* DMA value from ADC Dual Interleaved Mode*/
    SystemClockConfig();
    uint32_t DMA_ConvertedValue = 0;
    uint16_t DataBuf = 0;

    /* VSensor voltage*/
    float VSensorValue = 0;
    /* VSensor Temperature*/
    float TSensorValue = 0;
    /* USART Initialization */
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

    DMA_Init(&DMA_ConvertedValue);
    /* ADC init */
    ADC_Init();

    while (1)
    {
        if (DMA_ReadStatusFlag(DMA2_Stream0, DMA_FLAG_TCIFLG0))
        {
            uint32_t currentValue = getValue();
            
            printf("%ld", currentValue);
            
            
            DataBuf = DMA_ConvertedValue;
            VSensorValue = (float)DataBuf/4095*3.3f;

            /*!
             * According to actual test data of multiple bathes of chips, 
             * V28 is adopted instead of V25 for this example. And 0.7782v is the voltage for 28??
             */
            TSensorValue = (VSensorValue - 0.7782f)/0.0024f + 28;

            printf("\r\n");
            printf("ADC REGDATA = 0x%04X \r\n", DataBuf);
            printf("Voltage    = %f V \r\n", VSensorValue);
            printf("Temperature    = %f Celsius \r\n", TSensorValue);

            Delay(0xFFFFFF);
            DMA_ClearStatusFlag(DMA2_Stream0, DMA_FLAG_TCIFLG0);
        }
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
    ADC_Config_T  adcConfig;
    ADC_CommonConfig_T  adcCommonConfig;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC Configuration */
    ADC_Reset();
    ADC_ConfigStructInit(&adcConfig);
    /* Set resolution*/
    adcConfig.resolution = ADC_RESOLUTION_12BIT;
    /* Set dataAlign*/
    adcConfig.dataAlign = ADC_DATA_ALIGN_RIGHT;
    /* Set scanDir*/
    adcConfig.scanConvMode = DISABLE;
    /* Set convMode continous*/
    adcConfig.continuousConvMode = ENABLE;
    /* Set extTrigEdge*/
    adcConfig.extTrigEdge = ADC_EXT_TRIG_EDGE_NONE;

    /*Set ADC Clock Prescaler. ADC_Clock = APB2_Clock/4, 84/4=21MHz*/
    adcCommonConfig.prescaler = ADC_PRESCALER_DIV4;
    ADC_CommonConfig(ADC1, &adcCommonConfig);
    
    ADC_Config(ADC1, &adcConfig);
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_16, 1, ADC_SAMPLETIME_480CYCLES);

    ADC_EnableTempSensorVrefint();

    ADC_EnableDMA(ADC1);
    ADC_EnableDMARequest(ADC1);

    /* Enable ADC*/
    ADC_Enable(ADC1);
    ADC_SoftwareStartConv(ADC1);
}

/*!
 * @brief     DMA Init
 *
 * @param     None
 *
 * @retval    value
 */
uint32_t getValue(void) 
{
    return *value; 
}

/*!
 * @brief     DMA Init
 *
 * @param     None
 *
 * @retval    None
 */
void DMA_Init(uint32_t* Buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

    /* size of buffer*/
    dmaConfig.bufferSize = 1;
    /* set memory Data Size*/
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_HALFWORD;
    /* Set peripheral Data Size*/
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_HALFWORD;
    /* Enable Memory Address increase*/
    dmaConfig.memoryInc = DMA_MEMORY_INC_DISABLE;
    /* Disable Peripheral Address increase*/
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Reset Circular Mode*/
    dmaConfig.loopMode = DMA_MODE_CIRCULAR;
    /* set priority*/
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    /* read from peripheral*/
    dmaConfig.dir = DMA_DIR_PERIPHERALTOMEMORY;
    /* Set memory Address*/
    dmaConfig.memoryBaseAddr = (uint32_t)Buf;
    /* Set Peripheral Address*/
    dmaConfig.peripheralBaseAddr = (uint32_t)&ADC1->REGDATA;

    dmaConfig.channel = DMA_CHANNEL_0;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;

    DMA_Config(DMA2_Stream0, &dmaConfig);
    /* Clear DMA TF flag*/
    DMA_ClearIntFlag(DMA2_Stream0, DMA_INT_TCIFLG0);
    /* Enable DMA Interrupt*/
    DMA_EnableInterrupt(DMA2_Stream0, DMA_INT_TCIFLG);
    DMA_Enable(DMA2_Stream0);
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

    while (delay--);
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
