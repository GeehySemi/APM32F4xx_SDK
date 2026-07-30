/*!
 * @file        low_power_modes.c
 *
 * @brief       LP modes program body
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
#include "low_power_modes.h"
#include <stdio.h>

/* Private includes *******************************************************/
#include "Board.h"
#include "board_delay.h"
#include "log_printf.h"
#include "apm32f4xx_pmu.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_rtc.h"

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief     Consumption measure in sleep mode
 *              Wakeup using EINT line 1
 * @param     None
 *
 * @retval    None
 */
void ConsumptionMeasure_SleepMode(void)
{
    GPIO_Config_T  gpioConfigStruct;
    USART_Config_T usartConfigStruct;
    
    /* Enable GPIO clock*/
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB |
                              RCM_AHB1_PERIPH_GPIOC | RCM_AHB1_PERIPH_GPIOD |
                              RCM_AHB1_PERIPH_GPIOE | RCM_AHB1_PERIPH_GPIOF |
                              RCM_AHB1_PERIPH_GPIOG | RCM_AHB1_PERIPH_GPIOH);
    
    /* Setup all GPIO pin to Analog input mode*/
    gpioConfigStruct.pin = GPIO_PIN_ALL;
    gpioConfigStruct.mode = GPIO_MODE_AN;
    gpioConfigStruct.speed = GPIO_SPEED_100MHz;
    gpioConfigStruct.pupd = GPIO_PUPD_NOPULL;
    
    GPIO_Config(GPIOA,&gpioConfigStruct);
    GPIO_Config(GPIOB,&gpioConfigStruct);
    GPIO_Config(GPIOC,&gpioConfigStruct);
    GPIO_Config(GPIOD,&gpioConfigStruct);
    GPIO_Config(GPIOE,&gpioConfigStruct);
    GPIO_Config(GPIOH,&gpioConfigStruct);
    GPIO_Config(GPIOF,&gpioConfigStruct);
    GPIO_Config(GPIOG,&gpioConfigStruct);
    
    /* Disable GPIO clock*/
    RCM_DisableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB |
                              RCM_AHB1_PERIPH_GPIOC | RCM_AHB1_PERIPH_GPIOD |
                              RCM_AHB1_PERIPH_GPIOE | RCM_AHB1_PERIPH_GPIOF |
                              RCM_AHB1_PERIPH_GPIOG | RCM_AHB1_PERIPH_GPIOH);

    /* Configure EINT line to wake up system*/
    BOARD_PBInit(BUTTON_KEY2,BUTTON_MODE_EINT);

    BOARD_DelayMs(200);
    /* Watting to enter SLEEP mode*/
    __WFI();
    
    BOARD_LEDInit(LED2);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);
    
    LOG_Print("\r\nWakeup from sleep mode.\r\n");
    
    while(1)
    {
        BOARD_LEDToggle(LED2);
        BOARD_DelayMs(0x3FFFFF);
    }
}

/*!
 * @brief     Consumption measure in stop mode
 *
 * @param     None
 *
 * @retval    None
 */
void ConsumptionMeasure_StopMode(void)
{
    GPIO_Config_T  gpioConfigStruct;
    USART_Config_T usartConfigStruct;
    EINT_Config_T eintConfigStruct;
    
    /* Allow access to Backup domain*/
    PMU_EnableBackupAccess();
    
    /* Enable LSI clock*/
    RCM_EnableLSI();

    while(RCM_ReadStatusFlag(RCM_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select LSI as RTC clock source*/
    RCM_ConfigRTCCLK(RCM_RTCCLK_LSI);
    
    /* Enable RTC clock*/
    RCM_EnableRTCCLK();
    
    /* Wait for RTC sync*/
    RTC_WaitForSynchro();
    
    /* Enable GPIO clock*/
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB |
                              RCM_AHB1_PERIPH_GPIOC | RCM_AHB1_PERIPH_GPIOD |
                              RCM_AHB1_PERIPH_GPIOE | RCM_AHB1_PERIPH_GPIOF |
                              RCM_AHB1_PERIPH_GPIOG | RCM_AHB1_PERIPH_GPIOH);
    
    /* Setup all GPIO pin to Analog input mode*/
    gpioConfigStruct.pin = GPIO_PIN_ALL;
    gpioConfigStruct.mode = GPIO_MODE_AN;
    gpioConfigStruct.speed = GPIO_SPEED_100MHz;
    gpioConfigStruct.pupd = GPIO_PUPD_NOPULL;
    
    GPIO_Config(GPIOA,&gpioConfigStruct);
    GPIO_Config(GPIOB,&gpioConfigStruct);
    GPIO_Config(GPIOC,&gpioConfigStruct);
    GPIO_Config(GPIOD,&gpioConfigStruct);
    GPIO_Config(GPIOE,&gpioConfigStruct);
    GPIO_Config(GPIOH,&gpioConfigStruct);
    GPIO_Config(GPIOF,&gpioConfigStruct);
    GPIO_Config(GPIOG,&gpioConfigStruct);

    /* Disable GPIO clock*/
    RCM_DisableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB |
                              RCM_AHB1_PERIPH_GPIOC | RCM_AHB1_PERIPH_GPIOD |
                              RCM_AHB1_PERIPH_GPIOE | RCM_AHB1_PERIPH_GPIOF |
                              RCM_AHB1_PERIPH_GPIOG | RCM_AHB1_PERIPH_GPIOH);
    
    /* Configure RTC wakeup event to wake up system*/
    EINT_ClearIntFlag(EINT_LINE_22);
    eintConfigStruct.line = EINT_LINE_22;
    eintConfigStruct.mode = EINT_MODE_INTERRUPT;
    eintConfigStruct.trigger = EINT_TRIGGER_RISING;
    eintConfigStruct.lineCmd = ENABLE;
    EINT_Config(&eintConfigStruct);
    
    NVIC_EnableIRQRequest(RTC_WKUP_IRQn,0,0);
    
    /* RTC wakeup time ~10s*/
    RTC_ConfigWakeUpClock(RTC_WAKEUP_CLOCK_RTC_DIV16);
    RTC_ConfigWakeUpValue(0x5000 - 1);
    
    /* Enable RTC wakeup interrupt*/
    RTC_EnableInterrupt(RTC_INT_WT);
    
    RTC_EnableWakeUp();
    
    /* Enanle flash deep power down mode*/
    PMU_EnableFlashPowerDown();
    
    /* Enter stop mode*/
    PMU_EnterSTOPMode(PMU_REGULATOR_LOWPOWER, PMU_STOP_ENTRY_WFI);
    
    BOARD_LEDInit(LED2);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);
    
    LOG_Print("\r\nWakeup from stop mode.\r\n");
    
    while(1)
    {
        BOARD_LEDToggle(LED2);
        BOARD_DelayMs(500);
    }
}

/*!
 * @brief     Consumption measure in standby mode
 *
 * @param     None
 *
 * @retval    None
 */
void ConsumptionMeasure_StandbyMode(void)
{
    PMU_EnableWakeUpPin();
    
    PMU_ClearStatusFlag(PMU_FLAG_WUE);
    
    PMU_EnterSTANDBYMode();
    
    /*Input high logic voltage to PA0 WKUP pin to wake up system*/
    while(1)
    {
        
    }
}

/*!
 * @brief     Consumption measure in standby RTC mode
 *
 * @param     None
 *
 * @retval    None
 */
void ConsumptionMeasure_StandbyRTCMode(void)
{
    /* Allow access to Backup domain*/
    PMU_EnableBackupAccess();
    
    /* Enable LSI clock*/
    RCM_EnableLSI();

    while(RCM_ReadStatusFlag(RCM_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select LSI as RTC clock source*/
    RCM_ConfigRTCCLK(RCM_RTCCLK_LSI);
    
    /* Enable RTC clock*/
    RCM_EnableRTCCLK();
    
    /* Wait for RTC sync*/
    RTC_WaitForSynchro();
    
    /* RTC wakeup time ~10s*/
    RTC_ConfigWakeUpClock(RTC_WAKEUP_CLOCK_RTC_DIV16);
    RTC_ConfigWakeUpValue(0x5000 - 1);
    
    RTC_DisableInterrupt(RTC_INT_WT);
    
    PMU_ClearStatusFlag(PMU_FLAG_WUE);
    
    RTC_EnableInterrupt(RTC_INT_WT);
    
    RTC_EnableWakeUp();
    
    RTC_ClearStatusFlag(RTC_FLAG_WTF);
    
    /* Enter standby mode*/
    PMU_EnterSTANDBYMode();

    while(1)
    {
        
    }
}

/*!
 * @brief     Consumption measure in standby RTC backup SRAM mode
 *
 * @param     None
 *
 * @retval    None
 */
void ConsumptionMeasure_StandbyRTCBKPSRAMMode(void)
{
    /* Allow access to Backup domain*/
    PMU_EnableBackupAccess();
    
    /* Enable LSI clock*/
    RCM_EnableLSI();

    while(RCM_ReadStatusFlag(RCM_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select LSI as RTC clock source*/
    RCM_ConfigRTCCLK(RCM_RTCCLK_LSI);
    
    /* Enable RTC clock*/
    RCM_EnableRTCCLK();
    
    /* Wait for RTC sync*/
    RTC_WaitForSynchro();
    
    /* Enable BKUP SRAM clock*/
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_BKPSRAM);
    
    /* Enable BKUP SRAM low power regulator*/
    PMU_EnableBackupRegulator();
    
    while(PMU_ReadStatusFlag(PMU_FLAG_BKPR) == RESET)
    {
    }
    
    /* RTC wakeup time ~10s*/
    RTC_ConfigWakeUpClock(RTC_WAKEUP_CLOCK_RTC_DIV16);
    RTC_ConfigWakeUpValue(0x5000 - 1);
    
    RTC_DisableInterrupt(RTC_INT_WT);
    
    PMU_ClearStatusFlag(PMU_FLAG_WUE);
    
    RTC_EnableInterrupt(RTC_INT_WT);
    
    RTC_EnableWakeUp();
    
    RTC_ClearStatusFlag(RTC_FLAG_WTF);
    
    /* Enter standby mode*/
    PMU_EnterSTANDBYMode();

    while(1)
    {
        
    }
}
