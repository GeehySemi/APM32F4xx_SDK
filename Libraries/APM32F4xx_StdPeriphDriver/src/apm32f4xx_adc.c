/*!
 * @file        apm32f4xx_adc.c
 *
 * @brief       This file contains all the functions for the ADC peripheral
 *
 * @version     V1.0.5
 *
 * @date        2025-12-15
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

#include "apm32f4xx_adc.h"
#include "apm32f4xx_rcm.h"

/** @addtogroup APM32F4xx_StdPeriphDriver
  @{
*/

#if defined (ADC1) || defined (ADC2) || defined (ADC3)

/** @defgroup ADC_Driver
  * @brief ADC driver modules
  @{
*/

/** @defgroup ADC_Functions
  @{
*/

/*!
 * @brief     Reset ADC peripheral registers to their default reset values.
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_Reset(void)
{
#if defined (APM32F411xx)
    RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_ADC1);
    RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_ADC1);
    RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_ADC2);
    RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_ADC2);
#else
    RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_ADC);
    RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_ADC);
#endif /* APM32F411xx */
}

/*!
 * @brief     Config the ADC peripheral according to the specified parameters in the adcConfig.
 *
 * @param     adc: ADC instance
 *
 * @param     adcConfig: pointer to a ADC_Config_T structure.
 *
 * @retval    None
 */
void ADC_Config(ADC_T* adc, ADC_Config_T* adcConfig)
{
    adc->CTRL1_B.RESSEL = adcConfig->resolution;
    adc->CTRL1_B.SCANEN = adcConfig->scanConvMode;

    adc->CTRL2_B.REGEXTTRGEN = adcConfig->extTrigEdge;
    adc->CTRL2_B.REGEXTTRGSEL = adcConfig->extTrigConv;
    adc->CTRL2_B.DALIGNCFG = adcConfig->dataAlign;
    adc->CTRL2_B.CONTCEN = adcConfig->continuousConvMode;

    adc->REGSEQ1_B.REGSEQLEN = adcConfig->nbrOfChannel - 1;
}

/*!
 * @brief     Fills each ADC_Config_T member with its default value.
 *
 * @param     adcConfig: pointer to an ADC_Config_T structure.
 *
 * @retval    None
 */
void ADC_ConfigStructInit(ADC_Config_T* adcConfig)
{
    adcConfig->resolution = ADC_RESOLUTION_12BIT;
    adcConfig->scanConvMode = DISABLE;
    adcConfig->continuousConvMode = DISABLE;
    adcConfig->extTrigEdge = ADC_EXT_TRIG_EDGE_NONE;
    adcConfig->extTrigConv = ADC_EXT_TRIG_CONV_TMR1_CC1;
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;
    adcConfig->nbrOfChannel = 1;
}

#if defined (APM32F411xx)
/*!
 * @brief     Config the ADCs peripherals according to the specified parameters in
 *             the adcCommonConfig.
 *
 * @param     adc: ADC instance
 *
 * @param     adcCommonConfig: pointer to an ADC_CommonConfig_T structure
 *
 * @retval    None
 */
void ADC_CommonConfig(ADC_T* adc, ADC_CommonConfig_T* adcCommonConfig)
{
    ADC_COMMON_REGISTER(adc)->CCTRL_B.ADCPRE = adcCommonConfig->prescaler;
}
#else
/*!
 * @brief     Config the ADCs peripherals according to the specified parameters in
 *             the adcCommonConfig.
 *
 * @param     adcCommonConfig: pointer to an ADC_CommonConfig_T structure
 *
 * @retval    None
 */
void ADC_CommonConfig(ADC_CommonConfig_T* adcCommonConfig)
{
    ADC_COMMON_INSTANCE->CCTRL_B.ADCPRE = adcCommonConfig->prescaler;
    ADC_COMMON_INSTANCE->CCTRL_B.ADCMSEL = adcCommonConfig->mode;
    ADC_COMMON_INSTANCE->CCTRL_B.DMAMODE = adcCommonConfig->accessMode;
    ADC_COMMON_INSTANCE->CCTRL_B.SMPDEL2 = adcCommonConfig->twoSampling;
}
#endif /* APM32F411xx */

/*!
 * @brief     Fills each ADC_CommonInitStruct member with its default value.
 *
 * @param     adccommonconfig: pointer to an ADC_CommonInitTypeDef structure.
 *
 * @retval    None
 */
void ADC_CommonConfigStructInit(ADC_CommonConfig_T* adccommonconfig)
{
    adccommonconfig->prescaler = ADC_PRESCALER_DIV2;
#if defined (APM32F405xx) || defined (APM32F407xx) || defined (APM32F415xx) || \
    defined (APM32F417xx) || defined (APM32F423xx) || defined (APM32F425xx) || defined (APM32F427xx)
    adccommonconfig->mode = ADC_MODE_INDEPENDENT;
    adccommonconfig->accessMode = ADC_ACCESS_MODE_DISABLED;
    adccommonconfig->twoSampling = ADC_TWO_SAMPLING_5CYCLES;
#endif /* APM32F405xx || APM32F407xx || APM32F415xx || APM32F417xx || APM32F423xx || APM32F425xx || APM32F427xx */
}

/*!
 * @brief     Enables the specified ADC peripheral.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_Enable(ADC_T* adc)
{
    adc->CTRL2_B.ADCEN = BIT_SET;
}

/*!
 * @brief     Disable the specified ADC peripheral.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_Disable(ADC_T* adc)
{
    adc->CTRL2_B.ADCEN = BIT_RESET;
}

/*!
 * @brief     Enable the specified ADC analog watchdog.
 *
 * @param     adc: ADC instance
 *
 * @param     analogWatchdog: The ADC analog watchdog configuration
 *               This parameter can be one of the following values:
 *               @arg ADC_ANALOG_WATCHDOG_SINGLE_REG        : Analog watchdog on a single regular channel
 *               @arg ADC_ANALOG_WATCHDOG_SINGLE_INJEC      : Analog watchdog on a single injected channel
 *               @arg ADC_ANALOG_WATCHDOG_SINGLE_REG_INJEC  : Analog watchdog on a single regular or injected channel
 *               @arg ADC_ANALOG_WATCHDOG_ALL_REG           : Analog watchdog on  all regular channel
 *               @arg ADC_ANALOG_WATCHDOG_ALL_INJEC         : Analog watchdog on  all injected channel
 *               @arg ADC_ANALOG_WATCHDOG_ALL_REG_ALL_INJEC : Analog watchdog on all regular and injected channels
 *
 * @retval    None
 */
void ADC_EnableAnalogWatchdog(ADC_T* adc, ADC_ANALOG_WATCHDOG_T analogWatchdog)
{
    adc->CTRL1_B.INJAWDEN = analogWatchdog & 0x01;
    adc->CTRL1_B.REGAWDEN = (analogWatchdog >> 1) & 0x01;
    adc->CTRL1_B.AWDSGLEN = analogWatchdog >> 4;
}

/*!
 * @brief     Disable the specified ADC analog watchdog.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableAnalogWatchdog(ADC_T* adc)
{
    adc->CTRL1_B.AWDSGLEN = BIT_RESET;
    adc->CTRL1_B.INJAWDEN = BIT_RESET;
    adc->CTRL1_B.REGAWDEN = BIT_RESET;
}

/*!
 * @brief     Configures the specified ADC high and low thresholds of the analog watchdog.
 *
 * @param     adc: ADC instance
 *
 * @param     highThreshold: The ADC analog watchdog High threshold value.
 *                           This parameter must be a 12bit value.
 *
 * @param     lowThreshold:  The ADC analog watchdog Low threshold value.
 *                           This parameter must be a 12bit value.
 * @retval    None
 */
void ADC_ConfigAnalogWatchdogThresholds(ADC_T* adc, uint16_t highThreshold, uint16_t lowThreshold)
{
    adc->AWDHT = highThreshold;
    adc->AWDLT = lowThreshold;
}

/*!
 * @brief     Configures the specified ADC high and low thresholds of the analog watchdog.
 *
 * @param     adc: ADC instance
 *
 * @param     lowThreshold: The ADC analog watchdog Low threshold value.
 *                          This parameter must be a 12bit value.
 *
 * @retval    None
 */
void ADC_AnalogWatchdogLowThresholds(ADC_T* adc,  uint16_t lowThreshold)
{
    adc->AWDLT = lowThreshold;
}

/*!
 * @brief      Configures the specified ADC analog watchdog guarded single channel
 *
 * @param      adc: ADC instance
 *
 * @param      channel: Select the ADC channel
 *                      This parameter can be one of the following values:
 *                        @arg ADC_CHANNEL_0:   Select ADC Channel 0
 *                        @arg ADC_CHANNEL_1:   Select ADC Channel 1
 *                        @arg ADC_CHANNEL_2:   Select ADC Channel 2
 *                        @arg ADC_CHANNEL_3:   Select ADC Channel 3
 *                        @arg ADC_CHANNEL_4:   Select ADC Channel 4
 *                        @arg ADC_CHANNEL_5:   Select ADC Channel 5
 *                        @arg ADC_CHANNEL_6:   Select ADC Channel 6
 *                        @arg ADC_CHANNEL_7:   Select ADC Channel 7
 *                        @arg ADC_CHANNEL_8:   Select ADC Channel 8
 *                        @arg ADC_CHANNEL_9:   Select ADC Channel 9
 *                        @arg ADC_CHANNEL_10:  Select ADC Channel 10
 *                        @arg ADC_CHANNEL_11:  Select ADC Channel 11
 *                        @arg ADC_CHANNEL_12:  Select ADC Channel 12
 *                        @arg ADC_CHANNEL_13:  Select ADC Channel 13
 *                        @arg ADC_CHANNEL_14:  Select ADC Channel 14
 *                        @arg ADC_CHANNEL_15:  Select ADC Channel 15
 *                        @arg ADC_CHANNEL_16:  Select ADC Channel 16
 *                        @arg ADC_CHANNEL_17:  Select ADC Channel 17
 *                        @arg ADC_CHANNEL_18:  Select ADC Channel 18
 *
 * @retval     None
 *
 * @note       adc can be ADC1, ADC2 or ADC3.
 */
void ADC_ConfigAnalogWatchdogSingleChannel(ADC_T* adc, uint8_t channel)
{
    adc->CTRL1_B.AWDCHSEL = channel;
}

/*!
 * @brief     Enables the temperature sensor and Vrefint channels.
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_EnableTempSensorVrefint(void)
{
    ADC_COMMON_INSTANCE->CCTRL_B.TSVREFEN = BIT_SET;
}

/*!
 * @brief     Disable the temperature sensor and Vrefint channel.
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_DisableTempSensorVrefint(void)
{
    ADC_COMMON_INSTANCE->CCTRL_B.TSVREFEN = BIT_RESET;
}

/*!
 * @brief     Enables  the VBAT (Voltage Battery) channel.
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_EnableVbat(void)
{
    ADC_COMMON_INSTANCE->CCTRL_B.VBATEN = BIT_SET;
}

/*!
 * @brief     Disables the VBAT (Voltage Battery) channel.
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_DisableVbat(void)
{
    ADC_COMMON_INSTANCE->CCTRL_B.VBATEN = BIT_RESET;
}

/*!
 * @brief     Configures the specified ADC regular channel.
 *
 * @param     adc: ADC instance
 *
 * @param     channel: the ADC channel to configure.
 *                     This parameter can be one of the following values:
 *                     @arg ADC_CHANNEL_0:  ADC channel 0
 *                     @arg ADC_CHANNEL_1:  ADC channel 1
 *                     @arg ADC_CHANNEL_2:  ADC channel 2
 *                     @arg ADC_CHANNEL_3:  ADC channel 3
 *                     @arg ADC_CHANNEL_4:  ADC channel 4
 *                     @arg ADC_CHANNEL_5:  ADC channel 5
 *                     @arg ADC_CHANNEL_6:  ADC channel 6
 *                     @arg ADC_CHANNEL_7:  ADC channel 7
 *                     @arg ADC_CHANNEL_8:  ADC channel 8
 *                     @arg ADC_CHANNEL_9:  ADC channel 9
 *                     @arg ADC_CHANNEL_10: ADC channel 10
 *                     @arg ADC_CHANNEL_11: ADC channel 11
 *                     @arg ADC_CHANNEL_12: ADC channel 12
 *                     @arg ADC_CHANNEL_13: ADC channel 13
 *                     @arg ADC_CHANNEL_14: ADC channel 14
 *                     @arg ADC_CHANNEL_15: ADC channel 15
 *                     @arg ADC_CHANNEL_16: ADC channel 16
 *                     @arg ADC_CHANNEL_17: ADC channel 17
 *                     @arg ADC_CHANNEL_18: ADC Channel 18
 *
 * @param     rank: The rank in the regular group sequencer
 *                     This parameter must be between 1 to 16.
 *
 * @param     sampleTime: the specified ADC channel SampleTime
 *                     The parameter can be one of following values:
 *                     @arg ADC_SAMPLETIME_3CYCLES:   Sample time equal to 3 cycles
 *                     @arg ADC_SAMPLETIME_15CYCLES:  Sample time equal to 15 cycles
 *                     @arg ADC_SAMPLETIME_28CYCLES:  Sample time equal to 28 cycles
 *                     @arg ADC_SAMPLETIME_56CYCLES:  Sample time equal to 56 cycles
 *                     @arg ADC_SAMPLETIME_84CYCLES:  Sample time equal to 84 cycles
 *                     @arg ADC_SAMPLETIME_112CYCLES: Sample time equal to 112 cycles
 *                     @arg ADC_SAMPLETIME_144CYCLES: Sample time equal to 144 cycles
 *                     @arg ADC_SAMPLETIME_480CYCLES: Sample time equal to 480 cycles
 *
 * @retval None
 */

void ADC_ConfigRegularChannel(ADC_T* adc, uint8_t channel, uint8_t rank, uint8_t sampleTime)
{
    uint32_t temp1 = 0;
    uint32_t temp2 = 0;

    if (channel > ADC_CHANNEL_9)
    {
        temp1 = adc->SMPTIM1;
        temp2 = (uint32_t)7 << (3 * (channel - 10));
        temp1 &= ~temp2;
        temp2 = (uint32_t)sampleTime << (3 * (channel - 10));
        temp1 |= temp2;
        adc->SMPTIM1 = temp1;
    }
    else
    {
        temp1 = adc->SMPTIM2;
        temp2 = (uint32_t)7 << (3 * channel);
        temp1 &= ~temp2;
        temp2 = (uint32_t)sampleTime << (3 * channel);
        temp1 |= temp2;
        adc->SMPTIM2 = temp1;
    }

    if (rank < 7)
    {
        temp1 = adc->REGSEQ3;
        temp2 = (uint32_t)0x1F << (5 * (rank - 1));
        temp1 &= ~temp2;
        temp2 = (uint32_t)channel << (5 * (rank - 1));
        temp1 |= temp2;
        adc->REGSEQ3 = temp1;
    }
    else if (rank < 13)
    {
        temp1 = adc->REGSEQ2;
        temp2 = (uint32_t)0x1F << (5 * (rank - 7));
        temp1 &= ~temp2;
        temp2 = (uint32_t)channel << (5 * (rank - 7));
        temp1 |= temp2;
        adc->REGSEQ2 = temp1;
    }
    else
    {
        temp1 = adc->REGSEQ1;
        temp2 = (uint32_t)0x1F << (5 * (rank - 13));
        temp1 &= ~temp2;
        temp2 = (uint32_t)channel << (5 * (rank - 13));
        temp1 |= temp2;
        adc->REGSEQ1 = temp1;
    }
}

/*!
 * @brief     Enables the selected ADC software start conversion of the regular channels.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_SoftwareStartConv(ADC_T* adc)
{
    adc->CTRL2_B.REGSWSC = BIT_SET;
}

/*!
 * @brief     Reads the selected ADC Software start regular conversion Status.
 *
 * @param     adc: ADC instance
 *
 * @retval    The new state of ADC software start conversion (SET or RESET).
 */
uint8_t ADC_ReadSoftwareStartConvStatus(ADC_T* adc)
{
    return (uint8_t)adc->CTRL2_B.REGSWSC;
}

/**
 * @brief     Enables the EOC on each regular channel conversion
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableEOCOnEachChannel(ADC_T* adc)
{
    adc->CTRL2_B.EOCSEL = BIT_SET;

}

/**
 * @brief     Disables the EOC on each regular channel conversion
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableEOCOnEachChannel(ADC_T* adc)
{
    adc->CTRL2_B.EOCSEL = BIT_RESET;
}

/*!
 * @brief     Enables the ADC continuous conversion mode
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableContinuousMode(ADC_T* adc)
{
    adc->CTRL2_B.CONTCEN = BIT_SET;
}

/*!
 * @brief     Disables the ADC continuous conversion mode
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableContinuousMode(ADC_T* adc)
{
    adc->CTRL2_B.CONTCEN = BIT_RESET;
}

/*!
 * @brief     Configures the discontinuous mode for the selected ADC regular group
 *            channel.
 *
 * @param     adc: ADC instance
 *
 * @param     number: specifies the discontinuous mode regular channel count value.
 *                    This number must be between 1 and 8.
 *
 * @retval    None
 */
void ADC_ConfigDiscMode(ADC_T* adc, uint8_t number)
{
    adc->CTRL1_B.DISCNUMCFG = number - 1;
}

/*!
 * @brief     Enables the discontinuous mode on regular group channel.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableDiscMode(ADC_T* adc)
{
    adc->CTRL1_B.REGDISCEN = BIT_SET;
}

/*!
 * @brief     Disables the discontinuous mode for injected group channel.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableDiscMode(ADC_T* adc)
{
    adc->CTRL1_B.REGDISCEN = BIT_RESET;
}

/*!
 * @brief     Reads the specified ADC conversion result data.
 *
 * @param     adc: ADC instance
 *
 * @retval    The Data conversion value.
 */
uint16_t ADC_ReadConversionValue(ADC_T* adc)
{
    return (uint16_t) adc->REGDATA;
}

#if defined (APM32F405xx) || defined (APM32F407xx) || defined (APM32F415xx) || \
    defined (APM32F417xx) || defined (APM32F423xx) || defined (APM32F425xx) || defined (APM32F427xx)
/*!
 * @brief     Reads the specified ADC conversion result data in dual mode.
 *
 * @param     adc: ADC instance
 *
 * @retval    The Data conversion value.
 *            - In dual mode:
 *                Data[15:0] contain the regular data of ADC1.
 *                Data[31:16] contain the regular data of ADC2.
 *            - In triple mode:
 *                Data[15:0] contain alternatively the regular data of ADC1, ADC3 and ADC2.
 *                Data[31:16] contain alternatively the regular data of ADC2, ADC1 and ADC3.
 */
uint32_t ADC_ReadMultiValue(void)
{
    return ADC_COMMON_INSTANCE->CDATA;
}
#endif /* APM32F405xx || APM32F407xx || APM32F415xx || APM32F417xx || APM32F423xx || APM32F425xx || APM32F427xx */

/*!
 * @brief     Enables the specified ADC DMA request.
 *
 * @param     adc: ADC instance
 *
 * @retval    The Data conversion value.
 */
void ADC_EnableDMA(ADC_T* adc)
{
    adc->CTRL2_B.DMAEN = BIT_SET;
}

/*!
 * @brief     Disables the specified ADC DMA request.
 *
 * @param     adc: ADC instance
 *
 * @retval    The Data conversion value.
 */
void ADC_DisableDMA(ADC_T* adc)
{
    adc->CTRL2_B.DMAEN = BIT_RESET;
}

/*!
 * @brief     Enables  the ADC DMA request after last transfer (Single-ADC mode)
 *
 * @param     adc: ADC instance
 *
 * @retval    The Data conversion value.
 */
void ADC_EnableDMARequest(ADC_T* adc)
{
    adc->CTRL2_B.DMADISSEL = BIT_SET;
}

/*!
 * @brief     Disables  the ADC DMA request after last transfer (Single-ADC mode)
 *
 * @param     adc: ADC instance
 *
 * @retval    The Data conversion value.
 */
void ADC_DisableDMARequest(ADC_T* adc)
{
    adc->CTRL2_B.DMADISSEL = BIT_RESET;
}

#if defined (APM32F405xx) || defined (APM32F407xx) || defined (APM32F415xx) || \
    defined (APM32F417xx) || defined (APM32F423xx) || defined (APM32F425xx) || defined (APM32F427xx)
/*!
 * @brief     Enables the ADC DMA request after last transfer in multi ADC mode
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_EnableMultiModeDMARequest(void)
{
    ADC_COMMON_INSTANCE->CCTRL_B.DMADISSEL = BIT_SET;
}

/*!
 * @brief     Disables the ADC DMA request after last transfer in multi ADC mode
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_DisableMultiModeDMARequest(void)
{
    ADC_COMMON_INSTANCE->CCTRL_B.DMADISSEL = BIT_RESET;
}
#endif /* APM32F405xx || APM32F407xx || APM32F415xx || APM32F417xx || APM32F423xx || APM32F425xx || APM32F427xx */

/*!
 * @brief     Configures the specified ADC injected channel.
 *
 * @param     adc: ADC instance
 *
 * @param     channel: the ADC channel to configure.
 *                    This parameter can be one of the following values:
 *                    @arg ADC_CHANNEL_0:  ADC channel 0
 *                    @arg ADC_CHANNEL_1:  ADC channel 1
 *                    @arg ADC_CHANNEL_2:  ADC channel 2
 *                    @arg ADC_CHANNEL_3:  ADC channel 3
 *                    @arg ADC_CHANNEL_4:  ADC channel 4
 *                    @arg ADC_CHANNEL_5:  ADC channel 5
 *                    @arg ADC_CHANNEL_6:  ADC channel 6
 *                    @arg ADC_CHANNEL_7:  ADC channel 7
 *                    @arg ADC_CHANNEL_8:  ADC channel 8
 *                    @arg ADC_CHANNEL_9:  ADC channel 9
 *                    @arg ADC_CHANNEL_10: ADC channel 10
 *                    @arg ADC_CHANNEL_11: ADC channel 11
 *                    @arg ADC_CHANNEL_12: ADC channel 12
 *                    @arg ADC_CHANNEL_13: ADC channel 13
 *                    @arg ADC_CHANNEL_14: ADC channel 14
 *                    @arg ADC_CHANNEL_15: ADC channel 15
 *                    @arg ADC_CHANNEL_16: ADC channel 16
 *                    @arg ADC_CHANNEL_17: ADC channel 17
 *                    @arg ADC_CHANNEL_18: ADC Channel 18
 *
 * @param     rank: The rank in the regular group sequencer
 *                     This parameter must be between 1 to 16.
 *
 * @param     sampleTime: the specified ADC channel SampleTime
 *                     The parameter can be one of following values:
 *                     @arg ADC_SAMPLETIME_3CYCLES:   Sample time equal to 3 cycles
 *                     @arg ADC_SAMPLETIME_15CYCLES:  Sample time equal to 15 cycles
 *                     @arg ADC_SAMPLETIME_28CYCLES:  Sample time equal to 28 cycles
 *                     @arg ADC_SAMPLETIME_56CYCLES:  Sample time equal to 56 cycles
 *                     @arg ADC_SAMPLETIME_84CYCLES:  Sample time equal to 84 cycles
 *                     @arg ADC_SAMPLETIME_112CYCLES: Sample time equal to 112 cycles
 *                     @arg ADC_SAMPLETIME_144CYCLES: Sample time equal to 144 cycles
 *                     @arg ADC_SAMPLETIME_480CYCLES: Sample time equal to 480 cycles
 *
 * @retval    None
 */
void ADC_ConfigInjectedChannel(ADC_T* adc, uint8_t channel, uint8_t rank, uint8_t sampleTime)
{
    uint32_t temp1 = 0;
    uint32_t temp2 = 0;

    if (channel > ADC_CHANNEL_9)
    {
        temp1 = adc->SMPTIM1;
        temp2 = (uint32_t)7 << (3 * (channel - 10));
        temp1 &= ~temp2;
        temp2 = (uint32_t)sampleTime << (3 * (channel - 10));
        temp1 |= temp2;
        adc->SMPTIM1 = temp1;
    }
    else
    {
        temp1 = adc->SMPTIM2;
        temp2 = (uint32_t)7 << (3 * channel);
        temp1 &= ~temp2;
        temp2 = (uint32_t)sampleTime << (3 * channel);
        temp1 |= temp2;
        adc->SMPTIM2 = temp1;
    }

    temp1 = adc->INJSEQ;
    temp2 = (uint32_t)0x1F << (5 * ((rank + 3) - (adc->INJSEQ_B.INJSEQLEN + 1)));
    temp1 &= ~temp2;
    temp2 = (uint32_t)channel << (5 * ((rank + 3) - (adc->INJSEQ_B.INJSEQLEN + 1)));
    temp1 |= temp2;
    adc->INJSEQ = temp1;
}

/*!
 * @brief     Configures the sequencer length for injected channels
 *
 * @param     adc: ADC instance
 *
 * @param     length: The sequencer length.
 *                    This parameter must be a number between 1 to 4.
 *
 * @retval    None
 */
void ADC_ConfigInjectedSequencerLength(ADC_T* adc, uint8_t length)
{
    adc->INJSEQ_B.INJSEQLEN = length - 1;
}

/*!
 * @brief     Configures the specified ADC injected channel conversion value offset.
 *
 * @param     adc: ADC instance
 *
 * @param     channel: Select the ADC injected channel.
 *                     This parameter can be one of the following values:
 *                     @arg ADC_INJEC_CHANNEL_1: select Injected Channel 1
 *                     @arg ADC_INJEC_CHANNEL_2: select Injected Channel 2
 *                     @arg ADC_INJEC_CHANNEL_3: select Injected Channel 3
 *                     @arg ADC_INJEC_CHANNEL_4: select Injected Channel 4
 *
 * @param     offset: The specified ADC injected channel offset.
 *                    This parameter must be a 12bit value.
 *
 * @retval    None
 */
void ADC_ConfigInjectedOffset(ADC_T* adc, ADC_INJEC_CHANNEL_T channel, uint16_t offset)
{
    switch ((uint8_t)channel)
    {
        case 0x01:
            adc->INJDOF1_B.INJDOF1 = offset;
            break;
        case 0x02:
            adc->INJDOF2_B.INJDOF2 = offset;
            break;
        case 0x03:
            adc->INJDOF3_B.INJDOF3 = offset;
            break;
        case 0x04:
            adc->INJDOF4_B.INJDOF4 = offset;
            break;
        default :
            break;
    }
}

/*!
 * @brief     Configures the specified ADC external trigger for injected channels conversion
 *
 * @param     adc: ADC instance
 *
 * @param     extTrigInjecConv: Select the ADC trigger to start injected conversion
 *                 This parameter can be one of the following values:
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR1_CC4:  Timer1 capture compare4 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR1_TRGO: Timer1 TRGO event selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR2_CC1:  Timer2 capture compare1 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR2_TRGO: Timer2 TRGO event selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR3_CC2:  Timer3 capture compare2 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR3_CC4:  Timer3 capture compare4 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR4_CC1:  Timer4 capture compare1 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR4_CC2:  Timer4 capture compare2 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR4_CC3:  Timer4 capture compare3 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR4_TRGO: Timer4 TRGO event selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR5_CC4:  Timer5 capture compare4 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR5_TRGO: Timer5 TRGO event selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR8_CC2:  Timer8 capture compare2 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR8_CC3:  Timer8 capture compare3 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_TMR8_CC4:  Timer8 capture compare4 selected
 *                 @arg ADC_EXT_TRIG_INJEC_CONV_EINT15: External interrupt line 15 event selected
 *
 * @retval    None
 */
void ADC_ConfigExternalTrigInjectedConv(ADC_T* adc, ADC_EXT_TRIG_INJEC_CONV_T extTrigInjecConv)
{
    adc->CTRL2_B.INJGEXTTRGSEL = extTrigInjecConv;
}

/*!
 * @brief     Configures the ADCx external trigger edge for injected channels conversion.
 *
 * @param     adc: ADC instance
 *
 * @param     extTrigInjecConvEdge: Select the ADC trigger to start injected conversion
 *                 This parameter can be one of the following values:
 *                 @arg ADC_EXT_TRIG_INJEC_EDGE_NONE          :external trigger disabled for injected conversion
 *                 @arg ADC_EXT_TRIG_INJEC_EDGE_RISING        :detection on rising edge
 *                 @arg ADC_EXT_TRIG_INJEC_EDGE_FALLING       :detection on falling edge
 *                 @arg ADC_EXT_TRIG_INJEC_EDGE_RISING_FALLING:detection on both rising and falling edge
 *
 * @retval    None
 */
void ADC_ConfigExternalTrigInjectedConvEdge(ADC_T* adc, ADC_EXT_TRIG_INJEC_EDGE_T extTrigInjecConvEdge)
{
    adc->CTRL2_B.INJEXTTRGEN = extTrigInjecConvEdge;
}

/*!
 * @brief     Enables the selected ADC software start conversion of the injected channels.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableSoftwareStartInjectedConv(ADC_T* adc)
{
    adc->CTRL2_B.INJSWSC = BIT_SET;
}

/*!
 * @brief     Reads the selected ADC Software start injected conversion Status.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
uint8_t ADC_ReadSoftwareStartInjectedConvStatus(ADC_T* adc)
{
    return (uint8_t)(adc->CTRL2_B.INJSWSC);
}

/*!
 * @brief     Enables the selected ADC automatic injected group conversion after regular one.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableAutoInjectedConv(ADC_T* adc)
{
    adc->CTRL1_B.INJGACEN = BIT_SET;
}

/*!
 * @brief     Disables the selected ADC automatic injected group conversion after regular one.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableAutoInjectedConv(ADC_T* adc)
{
    adc->CTRL1_B.INJGACEN = BIT_RESET;
}

/*!
 * @brief     Enable the specified ADC discontinuous mode for injected group.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableInjectedDiscMode(ADC_T* adc)
{
    adc->CTRL1_B.INJDISCEN = BIT_SET;
}

/*!
 * @brief     Disable the specified ADC discontinuous mode for injected group.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableInjectedDiscMode(ADC_T* adc)
{
    adc->CTRL1_B.INJDISCEN = BIT_RESET;
}

/*!
 * @brief     Reads the ADC injected channel conversion result
 *
 * @param     adc: ADC instance
 *
 * @param     channel: the converted ADC injected channel.
 *               This parameter can be one of the following values:
 *               @arg ADC_INJEC_CHANNEL_1: Injected Channel1 selected
 *               @arg ADC_INJEC_CHANNEL_2: Injected Channel2 selected
 *               @arg ADC_INJEC_CHANNEL_3: Injected Channel3 selected
 *               @arg ADC_INJEC_CHANNEL_4: Injected Channel4 selected
 *
 * @retval    The Data conversion value.
 */
uint16_t ADC_ReadInjectedConversionValue(ADC_T* adc, ADC_INJEC_CHANNEL_T channel)
{
    switch ((uint8_t)channel)
    {
        case 0x01:
            return (uint16_t)adc->INJDATA1_B.INJDATA;
        case 0x02:
            return (uint16_t)adc->INJDATA2_B.INJDATA;
        case 0x03:
            return (uint16_t)adc->INJDATA3_B.INJDATA;
        case 0x04:
            return (uint16_t)adc->INJDATA4_B.INJDATA;
        default :
            return 0;
    }
}

#if defined (APM32F423xx) || defined (APM32F425xx) || defined (APM32F427xx)

/*!
 * @brief     Enables the ADC oversampling mode.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_EnableOverSampling(ADC_T* adc)
{
    adc->OSAMPCTRL_B.OSEN = BIT_SET;
}

/*!
 * @brief     Disables the ADC oversampling mode.
 *
 * @param     adc: ADC instance
 *
 * @retval    None
 */
void ADC_DisableOverSampling(ADC_T* adc)
{
    adc->OSAMPCTRL_B.OSEN = BIT_RESET;
}

/*!
 * @brief     Configures the ADC oversampling ratio.
 *
 * @param     adc: ADC instance
 *
 * @param     ratio: The oversampling ratio.
 *                   This parameter can be one of the following values:
 *                   @arg ADC_OVERSAMPLING_RATIO_2:   Oversampling ratio x2
 *                   @arg ADC_OVERSAMPLING_RATIO_4:   Oversampling ratio x4
 *                   @arg ADC_OVERSAMPLING_RATIO_8:   Oversampling ratio x8
 *                   @arg ADC_OVERSAMPLING_RATIO_16:  Oversampling ratio x16
 *                   @arg ADC_OVERSAMPLING_RATIO_32:  Oversampling ratio x32
 *                   @arg ADC_OVERSAMPLING_RATIO_64:  Oversampling ratio x64
 *                   @arg ADC_OVERSAMPLING_RATIO_128: Oversampling ratio x128
 *                   @arg ADC_OVERSAMPLING_RATIO_256: Oversampling ratio x256
 *
 * @retval    None
 */
void ADC_ConfigOverSamplingRatio(ADC_T* adc, ADC_OVERSAMPLING_RATIO_T ratio)
{
    adc->OSAMPCTRL_B.OSR = ratio;
}

/*!
 * @brief     Configures the ADC oversampling shift.
 *
 * @param     adc: ADC instance
 *
 * @param     shift: The oversampling shift.
 *                   This parameter can be one of the following values:
 *                   @arg ADC_OVERSAMPLING_SHIFT_0:    No shift
 *                   @arg ADC_OVERSAMPLING_SHIFT_1:    Shift 1 bit
 *                   @arg ADC_OVERSAMPLING_SHIFT_2:    Shift 2 bits
 *                   @arg ADC_OVERSAMPLING_SHIFT_3:    Shift 3 bits
 *                   @arg ADC_OVERSAMPLING_SHIFT_4:    Shift 4 bits
 *                   @arg ADC_OVERSAMPLING_SHIFT_5:    Shift 5 bits
 *                   @arg ADC_OVERSAMPLING_SHIFT_6:    Shift 6 bits
 *                   @arg ADC_OVERSAMPLING_SHIFT_7:    Shift 7 bits
 *                   @arg ADC_OVERSAMPLING_SHIFT_8:    Shift 8 bits
 *
 * @retval    None
 */
void ADC_ConfigOverSamplingShift(ADC_T* adc, ADC_OVERSAMPLING_SHIFT_T shift)
{
    adc->OSAMPCTRL_B.OSS = shift;
}

/*!
 * @brief     Configures the ADC oversampling trigger.
 *
 * @param     adc: ADC instance
 *
 * @param     trigger: The oversampling trigger.
 *                     This parameter can be one of the following values:
 *                     @arg ADC_OVERSAMPLING_TRIGGER_ALL: After one trigger, execute all the oversampling channel conversions
 *                     @arg ADC_OVERSAMPLING_TRIGGER_EACH: Need trigger for each oversampling channel conversion
 *
 * @retval    None
 */
void ADC_ConfigOverSamplingTrigger(ADC_T* adc, ADC_OVERSAMPLING_TRIGGER_T trigger)
{
    adc->OSAMPCTRL_B.TOS = trigger;
}

#endif /* APM32F423xx || APM32F425xx || APM32F427xx */

/*!
 * @brief     Enable the specified ADC interrupt.
 *
 * @param     adc: ADC instance
 *
 * @param     interrupt: Select the ADC interrupt sources
 *                       This parameter can be any combination of the following values:
 *                       @arg ADC_INT_EOC: End of conversion interrupt mask
 *                       @arg ADC_INT_AWD: Analog watchdog interrupt mask
 *                       @arg ADC_INT_INJEOC: End of injected conversion interrupt mask
 *                       @arg ADC_INT_OVR: Overrun interrupt enable
 *
 * @retval    None
 */
void ADC_EnableInterrupt(ADC_T* adc, uint32_t interrupt)
{
    adc->CTRL1 |= interrupt;
}

/*!
 * @brief     Disables the specified ADC interrupt.
 *
 * @param     adc: ADC instance
 *
 * @param     interrupt: Select the ADC interrupt sources
 *                       This parameter can be any combination of the following values:
 *                       @arg ADC_INT_EOC: End of conversion interrupt mask
 *                       @arg ADC_INT_AWD: Analog watchdog interrupt mask
 *                       @arg ADC_INT_INJEOC: End of injected conversion interrupt mask
 *                       @arg ADC_INT_OVR: Overrun interrupt enable
 *
 * @retval    None
 */
void ADC_DisableInterrupt(ADC_T* adc, uint32_t interrupt)
{
    adc->CTRL1 &= (uint32_t)~interrupt;
}

/*!
 * @brief     Reads the pending ADC flag
 *
 * @param     adc: ADC instance
 *
 * @param     flag: Select the flag to check
 *                  This parameter can be one of the following values:
 *                  @arg ADC_FLAG_AWD: Analog watchdog flag
 *                  @arg ADC_FLAG_EOC: End of conversion flag
 *                  @arg ADC_FLAG_INJEOC: End of injected group conversion flag
 *                  @arg ADC_FLAG_INJCS: Start of injected group conversion flag
 *                  @arg ADC_FLAG_REGCS: Start of regular group conversion flag
 *                  @arg ADC_FLAG_OVR: Overrun flag
 *
 * @retval    SET or RESET
 */
uint8_t ADC_ReadStatusFlag(ADC_T* adc, ADC_FLAG_T flag)
{
    return (adc->STS & flag) ? SET : RESET;
}

/*!
 * @brief     Clears the pending ADC flag
 *
 * @param     adc: ADC instance
 *
 * @param     flag: Select the flag to check
 *                  This parameter can be one of the following values:
 *                  @arg ADC_FLAG_AWD: Analog watchdog flag
 *                  @arg ADC_FLAG_EOC: End of conversion flag
 *                  @arg ADC_FLAG_INJEOC: End of injected group conversion flag
 *                  @arg ADC_FLAG_INJCS: Start of injected group conversion flag
 *                  @arg ADC_FLAG_REGCS: Start of regular group conversion flag
 *                  @arg ADC_FLAG_OVR: Overrun flag
 *
 * @retval    None
 */
void ADC_ClearStatusFlag(ADC_T* adc, uint32_t flag)
{
    adc->STS = ~(uint32_t)flag;
}

/*!
 * @brief     Reads the specified ADC Interrupt flag.
 *
 * @param     adc: ADC instance
 *
 * @param     flag: Select the ADC interrupt source.
 *                  This parameter can be one of the following values:
 *                  @arg ADC_INT_FLAG_EOC: End of conversion interrupt flag
 *                  @arg ADC_INT_FLAG_AWD: Analog watchdog interrupt flag
 *                  @arg ADC_INT_FLAG_INJEOC: End of injected conversion interrupt flag
 *                  @arg ADC_INT_FLAG_OVR: Overrun interrupt flag
 *
 * @retval    SET or RESET
 */
uint16_t ADC_ReadIntFlag(ADC_T* adc, ADC_INT_FLAG_T flag)
{
    uint32_t itmask = 0;
    uint32_t intStatus = 0;

    itmask = (uint32_t)1 << (flag >> 8);
    intStatus = adc->CTRL1 & itmask;

    if (((adc->STS & (uint32_t)(flag & 0x3F)) != (uint32_t)RESET) && intStatus)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*!
 * @brief     Clears the specified ADC Interrupt pending bits.
 *
 * @param     adc: ADC instance
 *
 * @param     flag: Select the ADC interrupt source.
 *                  This parameter can be any combination of the following values:
 *                  @arg ADC_INT_FLAG_EOC: End of conversion interrupt flag
 *                  @arg ADC_INT_FLAG_AWD: Analog watchdog interrupt flag
 *                  @arg ADC_INT_FLAG_INJEOC: End of injected conversion interrupt flag
 *                  @arg ADC_INT_FLAG_OVR: Overrun interrupt flag
 *
 * @retval    None
 */
void ADC_ClearIntFlag(ADC_T* adc, uint32_t flag)
{
    adc->STS = ~(uint32_t)(flag & 0x3F);
}

/**@} end of group ADC_Functions */
/**@} end of group ADC_Driver */

#endif /* ADC1 || ADC2 || ADC3 */

/**@} end of group APM32F4xx_StdPeriphDriver */
