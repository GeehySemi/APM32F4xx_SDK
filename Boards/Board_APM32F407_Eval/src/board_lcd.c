/*!
 * @file        board_lcd.c
 *
 * @brief       Driver for LCD
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
#include "board_lcd.h"
#include "apm32f4xx_smc.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_gpio.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
LCD_DeviceConfig_T g_lcdDev =
{
    0,
    LCD_SIDE_SHORT,
    LCD_SIDE_LONG,
    LCD_COLOR_BACKGROUND,
    LCD_SIDE_SHORT * LCD_SIDE_LONG,
    &g_font
};

/* Private function prototypes ********************************************/
static void LCD_Delay(uint32_t cnt);

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Configures LCD GPIO.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDGPIOInit(void)
{
    GPIO_Config_T GPIO_configStruct;

    /** Enable GPIO clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOF | RCM_AHB1_PERIPH_GPIOE | \
                              RCM_AHB1_PERIPH_GPIOD);
    RCM_EnableAHB1PeriphClock(LCD_GPIO_CS_CLK);
    RCM_EnableAHB1PeriphClock(LCD_GPIO_ADD_CLK);
    RCM_EnableAHB1PeriphClock(LCD_GPIO_RST_CLK);
    RCM_EnableAHB1PeriphClock(LCD_GPIO_BK_CLK);

    GPIO_configStruct.mode = GPIO_MODE_AF;
    GPIO_configStruct.otype = GPIO_OTYPE_PP;
    GPIO_configStruct.pupd = GPIO_PUPD_UP;
    GPIO_configStruct.speed = GPIO_SPEED_100MHz;
    /** Data pin of SMC */
    GPIO_configStruct.pin = GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9 | \
                            GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | \
                            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Config(GPIOE, &GPIO_configStruct);
    GPIO_configStruct.pin = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_8 | \
                            GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | \
                            GPIO_PIN_15;
    GPIO_Config(GPIOD, &GPIO_configStruct);

    /** WR and RD */
    GPIO_configStruct.pin = GPIO_PIN_5 | GPIO_PIN_4;
    GPIO_Config(GPIOD, &GPIO_configStruct);

    /** CS for bank of SMC */
    GPIO_configStruct.pin = LCD_GPIO_CS_PIN;
    GPIO_Config(LCD_GPIO_CS_PORT, &GPIO_configStruct);
    GPIO_ConfigPinAF(LCD_GPIO_CS_PORT, LCD_GPIO_CS_PIN_SOURC, LCD_GPIO_CS_AF);

    /** Address pin of SMC */
    GPIO_configStruct.pin = LCD_GPIO_ADD_PIN;
    GPIO_Config(LCD_GPIO_ADD_PORT, &GPIO_configStruct);
    GPIO_ConfigPinAF(LCD_GPIO_ADD_PORT, LCD_GPIO_ADD_PIN_SOURC, LCD_GPIO_ADD_AF);

    /** RST */
    GPIO_configStruct.pin = LCD_GPIO_RST_PIN;
    GPIO_configStruct.mode = GPIO_MODE_OUT;
    GPIO_configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(LCD_GPIO_RST_PORT, &GPIO_configStruct);
    
    /** Back light control */
    GPIO_configStruct.pin = LCD_GPIO_BK_PIN;
    GPIO_Config(LCD_GPIO_BK_PORT, &GPIO_configStruct);

    /** Data pin of SMC */
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_14, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_15, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_0, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_1, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_7, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_8, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_9, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_10, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_11, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_12, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_13, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_14, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_15, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_8, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_9, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_10, GPIO_AF_SMC);

    /** Read or Write CS */
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_5, GPIO_AF_SMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_4, GPIO_AF_SMC);
}

/*!
 * @brief       Configures SMC for LCD.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDSMCInit(void)
{
    SMC_NORSRAMConfig_T smcNORSRAMConfig;
    SMC_NORSRAMTimingConfig_T timingConfig;

    RCM_EnableAHB3PeriphClock(RCM_AHB3_PERIPH_EMMC);

    timingConfig.addressSetupTime = 0x4;
    timingConfig.addressHoldTime = 0x0;
    timingConfig.dataSetupTime = 0x02;
    timingConfig.busTurnaroundTime = 0x0;
    timingConfig.clockDivision = 0x0;
    timingConfig.dataLatency = 0x0;
    timingConfig.accessMode = SMC_ACCESS_MODE_B;

    smcNORSRAMConfig.bank = SMC_BANK1_NORSRAM_3;
    smcNORSRAMConfig.dataAddressMux = SMC_DATA_ADDRESS_MUX_DISABLE;
    smcNORSRAMConfig.memoryType = SMC_MEMORY_TYPE_NOR;
    smcNORSRAMConfig.memoryDataWidth = SMC_MEMORY_DATA_WIDTH_16BIT;
    smcNORSRAMConfig.burstAcceesMode = SMC_BURST_ACCESS_MODE_DISABLE;
    smcNORSRAMConfig.asynchronousWait = SMC_ASYNCHRONOUS_WAIT_DISABLE;
    smcNORSRAMConfig.waitSignalPolarity = SMC_WAIT_SIGNAL_POLARITY_LOW;
    smcNORSRAMConfig.wrapMode = SMC_WRAP_MODE_DISABLE;
    smcNORSRAMConfig.waitSignalActive = SMC_WAIT_SIGNAL_ACTIVE_BEFORE_WAIT_STATE;
    smcNORSRAMConfig.writeOperation = SMC_WRITE_OPERATION_ENABLE;
    smcNORSRAMConfig.waitSignal = SMC_WAIT_SIGNAL_DISABLE;
    smcNORSRAMConfig.extendedMode = SMC_EXTENDEN_MODE_DISABLE;
    smcNORSRAMConfig.writeBurst = SMC_WRITE_BURST_DISABLE;
    smcNORSRAMConfig.readWriteTimingStruct = &timingConfig;
    smcNORSRAMConfig.writeTimingStruct = &timingConfig;

    SMC_ConfigNORSRAM(&smcNORSRAMConfig);

    SMC_EnableNORSRAM(SMC_BANK1_NORSRAM_3);
}

/*!
 * @brief       Reset LCD.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDReset(void)
{
    GPIO_ResetBit(GPIOE, GPIO_PIN_3);
    LCD_Delay(10000);
    GPIO_SetBit(GPIOE, GPIO_PIN_3);
    LCD_Delay(10000);
}

/*!
 * @brief       Config Register for LCD.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDConfigRegister(void)
{
    LCD_Write(0xF000, 0x0055);
    LCD_Write(0xF001, 0x00AA);
    LCD_Write(0xF002, 0x0052);
    LCD_Write(0xF003, 0x0008);
    LCD_Write(0xF004, 0x0001);

    /** AVDD Set AVDD 5.2V */
    LCD_Write(0xB000, 0x000D);
    LCD_Write(0xB001, 0x000D);
    LCD_Write(0xB002, 0x000D);

    LCD_Write(0xB100, 0x000D);
    LCD_Write(0xB101, 0x000D);
    LCD_Write(0xB102, 0x000D);

    LCD_Write(0xB200, 0x0000);
    LCD_Write(0xB201, 0x0000);
    LCD_Write(0xB202, 0x0000);

    LCD_Write(0xB600, 0x0044);
    LCD_Write(0xB601, 0x0044);
    LCD_Write(0xB602, 0x0044);

    LCD_Write(0xB700, 0x0034);
    LCD_Write(0xB701, 0x0034);
    LCD_Write(0xB702, 0x0034);

    LCD_Write(0xB800, 0x0034);
    LCD_Write(0xB801, 0x0034);
    LCD_Write(0xB802, 0x0034);

    LCD_Write(0xBF00, 0x0001);

    /** VGH */
    LCD_Write(0xB300, 0x000f);
    LCD_Write(0xB301, 0x000f);
    LCD_Write(0xB302, 0x000f);

    /** VGH Ratio */
    LCD_Write(0xB900, 0x0034);
    LCD_Write(0xB901, 0x0034);
    LCD_Write(0xB902, 0x0034);

    /** VGL_REG */
    LCD_Write(0xB500, 0x0008);
    LCD_Write(0xB501, 0x0008);
    LCD_Write(0xB502, 0x0008);

    LCD_Write(0xC200, 0x0003);

    /** VGLX Ratio */
    LCD_Write(0xBA00, 0x0034);
    LCD_Write(0xBA01, 0x0034);
    LCD_Write(0xBA02, 0x0034);

    /** VGMP/VGSP */
    LCD_Write(0xBC00, 0x0000);
    LCD_Write(0xBC01, 0x0078);
    LCD_Write(0xBC02, 0x0000);

    /** VGMN/VGSN */
    LCD_Write(0xBD00, 0x0000);
    LCD_Write(0xBD01, 0x0078);
    LCD_Write(0xBD02, 0x0000);

    /** Vcom  */
    LCD_Write(0xBE00, 0x0000);
    LCD_Write(0xBE01, 0x0064);

    /** Gamma (R+) */
    LCD_Write(0xD100, 0x0000);
    LCD_Write(0xD101, 0x0033);
    LCD_Write(0xD102, 0x0000);
    LCD_Write(0xD103, 0x0034);
    LCD_Write(0xD104, 0x0000);
    LCD_Write(0xD105, 0x003A);
    LCD_Write(0xD106, 0x0000);
    LCD_Write(0xD107, 0x004A);
    LCD_Write(0xD108, 0x0000);
    LCD_Write(0xD109, 0x005C);
    LCD_Write(0xD10A, 0x0000);
    LCD_Write(0xD10B, 0x0081);
    LCD_Write(0xD10C, 0x0000);
    LCD_Write(0xD10D, 0x00A6);
    LCD_Write(0xD10E, 0x0000);
    LCD_Write(0xD10F, 0x00E5);
    LCD_Write(0xD110, 0x0001);
    LCD_Write(0xD111, 0x0013);
    LCD_Write(0xD112, 0x0001);
    LCD_Write(0xD113, 0x0054);
    LCD_Write(0xD114, 0x0001);
    LCD_Write(0xD115, 0x0082);
    LCD_Write(0xD116, 0x0001);
    LCD_Write(0xD117, 0x00CA);
    LCD_Write(0xD118, 0x0002);
    LCD_Write(0xD119, 0x0000);
    LCD_Write(0xD11A, 0x0002);
    LCD_Write(0xD11B, 0x0001);
    LCD_Write(0xD11C, 0x0002);
    LCD_Write(0xD11D, 0x0034);
    LCD_Write(0xD11E, 0x0002);
    LCD_Write(0xD11F, 0x0067);
    LCD_Write(0xD120, 0x0002);
    LCD_Write(0xD121, 0x0084);
    LCD_Write(0xD122, 0x0002);
    LCD_Write(0xD123, 0x00A4);
    LCD_Write(0xD124, 0x0002);
    LCD_Write(0xD125, 0x00B7);
    LCD_Write(0xD126, 0x0002);
    LCD_Write(0xD127, 0x00CF);
    LCD_Write(0xD128, 0x0002);
    LCD_Write(0xD129, 0x00DE);
    LCD_Write(0xD12A, 0x0002);
    LCD_Write(0xD12B, 0x00F2);
    LCD_Write(0xD12C, 0x0002);
    LCD_Write(0xD12D, 0x00FE);
    LCD_Write(0xD12E, 0x0003);
    LCD_Write(0xD12F, 0x0010);
    LCD_Write(0xD130, 0x0003);
    LCD_Write(0xD131, 0x0033);
    LCD_Write(0xD132, 0x0003);
    LCD_Write(0xD133, 0x006D);

    /** Gamma (G+) */
    LCD_Write(0xD200, 0x0000);
    LCD_Write(0xD201, 0x0033);
    LCD_Write(0xD202, 0x0000);
    LCD_Write(0xD203, 0x0034);
    LCD_Write(0xD204, 0x0000);
    LCD_Write(0xD205, 0x003A);
    LCD_Write(0xD206, 0x0000);
    LCD_Write(0xD207, 0x004A);
    LCD_Write(0xD208, 0x0000);
    LCD_Write(0xD209, 0x005C);
    LCD_Write(0xD20A, 0x0000);
    LCD_Write(0xD20B, 0x0081);
    LCD_Write(0xD20C, 0x0000);
    LCD_Write(0xD20D, 0x00A6);
    LCD_Write(0xD20E, 0x0000);
    LCD_Write(0xD20F, 0x00E5);
    LCD_Write(0xD210, 0x0001);
    LCD_Write(0xD211, 0x0013);
    LCD_Write(0xD212, 0x0001);
    LCD_Write(0xD213, 0x0054);
    LCD_Write(0xD214, 0x0001);
    LCD_Write(0xD215, 0x0082);
    LCD_Write(0xD216, 0x0001);
    LCD_Write(0xD217, 0x00CA);
    LCD_Write(0xD218, 0x0002);
    LCD_Write(0xD219, 0x0000);
    LCD_Write(0xD21A, 0x0002);
    LCD_Write(0xD21B, 0x0001);
    LCD_Write(0xD21C, 0x0002);
    LCD_Write(0xD21D, 0x0034);
    LCD_Write(0xD21E, 0x0002);
    LCD_Write(0xD21F, 0x0067);
    LCD_Write(0xD220, 0x0002);
    LCD_Write(0xD221, 0x0084);
    LCD_Write(0xD222, 0x0002);
    LCD_Write(0xD223, 0x00A4);
    LCD_Write(0xD224, 0x0002);
    LCD_Write(0xD225, 0x00B7);
    LCD_Write(0xD226, 0x0002);
    LCD_Write(0xD227, 0x00CF);
    LCD_Write(0xD228, 0x0002);
    LCD_Write(0xD229, 0x00DE);
    LCD_Write(0xD22A, 0x0002);
    LCD_Write(0xD22B, 0x00F2);
    LCD_Write(0xD22C, 0x0002);
    LCD_Write(0xD22D, 0x00FE);
    LCD_Write(0xD22E, 0x0003);
    LCD_Write(0xD22F, 0x0010);
    LCD_Write(0xD230, 0x0003);
    LCD_Write(0xD231, 0x0033);
    LCD_Write(0xD232, 0x0003);
    LCD_Write(0xD233, 0x006D);

    /** Gamma (B+) */
    LCD_Write(0xD300, 0x0000);
    LCD_Write(0xD301, 0x0033);
    LCD_Write(0xD302, 0x0000);
    LCD_Write(0xD303, 0x0034);
    LCD_Write(0xD304, 0x0000);
    LCD_Write(0xD305, 0x003A);
    LCD_Write(0xD306, 0x0000);
    LCD_Write(0xD307, 0x004A);
    LCD_Write(0xD308, 0x0000);
    LCD_Write(0xD309, 0x005C);
    LCD_Write(0xD30A, 0x0000);
    LCD_Write(0xD30B, 0x0081);
    LCD_Write(0xD30C, 0x0000);
    LCD_Write(0xD30D, 0x00A6);
    LCD_Write(0xD30E, 0x0000);
    LCD_Write(0xD30F, 0x00E5);
    LCD_Write(0xD310, 0x0001);
    LCD_Write(0xD311, 0x0013);
    LCD_Write(0xD312, 0x0001);
    LCD_Write(0xD313, 0x0054);
    LCD_Write(0xD314, 0x0001);
    LCD_Write(0xD315, 0x0082);
    LCD_Write(0xD316, 0x0001);
    LCD_Write(0xD317, 0x00CA);
    LCD_Write(0xD318, 0x0002);
    LCD_Write(0xD319, 0x0000);
    LCD_Write(0xD31A, 0x0002);
    LCD_Write(0xD31B, 0x0001);
    LCD_Write(0xD31C, 0x0002);
    LCD_Write(0xD31D, 0x0034);
    LCD_Write(0xD31E, 0x0002);
    LCD_Write(0xD31F, 0x0067);
    LCD_Write(0xD320, 0x0002);
    LCD_Write(0xD321, 0x0084);
    LCD_Write(0xD322, 0x0002);
    LCD_Write(0xD323, 0x00A4);
    LCD_Write(0xD324, 0x0002);
    LCD_Write(0xD325, 0x00B7);
    LCD_Write(0xD326, 0x0002);
    LCD_Write(0xD327, 0x00CF);
    LCD_Write(0xD328, 0x0002);
    LCD_Write(0xD329, 0x00DE);
    LCD_Write(0xD32A, 0x0002);
    LCD_Write(0xD32B, 0x00F2);
    LCD_Write(0xD32C, 0x0002);
    LCD_Write(0xD32D, 0x00FE);
    LCD_Write(0xD32E, 0x0003);
    LCD_Write(0xD32F, 0x0010);
    LCD_Write(0xD330, 0x0003);
    LCD_Write(0xD331, 0x0033);
    LCD_Write(0xD332, 0x0003);
    LCD_Write(0xD333, 0x006D);

    /** Gamma (R-) */
    LCD_Write(0xD400, 0x0000);
    LCD_Write(0xD401, 0x0033);
    LCD_Write(0xD402, 0x0000);
    LCD_Write(0xD403, 0x0034);
    LCD_Write(0xD404, 0x0000);
    LCD_Write(0xD405, 0x003A);
    LCD_Write(0xD406, 0x0000);
    LCD_Write(0xD407, 0x004A);
    LCD_Write(0xD408, 0x0000);
    LCD_Write(0xD409, 0x005C);
    LCD_Write(0xD40A, 0x0000);
    LCD_Write(0xD40B, 0x0081);
    LCD_Write(0xD40C, 0x0000);
    LCD_Write(0xD40D, 0x00A6);
    LCD_Write(0xD40E, 0x0000);
    LCD_Write(0xD40F, 0x00E5);
    LCD_Write(0xD410, 0x0001);
    LCD_Write(0xD411, 0x0013);
    LCD_Write(0xD412, 0x0001);
    LCD_Write(0xD413, 0x0054);
    LCD_Write(0xD414, 0x0001);
    LCD_Write(0xD415, 0x0082);
    LCD_Write(0xD416, 0x0001);
    LCD_Write(0xD417, 0x00CA);
    LCD_Write(0xD418, 0x0002);
    LCD_Write(0xD419, 0x0000);
    LCD_Write(0xD41A, 0x0002);
    LCD_Write(0xD41B, 0x0001);
    LCD_Write(0xD41C, 0x0002);
    LCD_Write(0xD41D, 0x0034);
    LCD_Write(0xD41E, 0x0002);
    LCD_Write(0xD41F, 0x0067);
    LCD_Write(0xD420, 0x0002);
    LCD_Write(0xD421, 0x0084);
    LCD_Write(0xD422, 0x0002);
    LCD_Write(0xD423, 0x00A4);
    LCD_Write(0xD424, 0x0002);
    LCD_Write(0xD425, 0x00B7);
    LCD_Write(0xD426, 0x0002);
    LCD_Write(0xD427, 0x00CF);
    LCD_Write(0xD428, 0x0002);
    LCD_Write(0xD429, 0x00DE);
    LCD_Write(0xD42A, 0x0002);
    LCD_Write(0xD42B, 0x00F2);
    LCD_Write(0xD42C, 0x0002);
    LCD_Write(0xD42D, 0x00FE);
    LCD_Write(0xD42E, 0x0003);
    LCD_Write(0xD42F, 0x0010);
    LCD_Write(0xD430, 0x0003);
    LCD_Write(0xD431, 0x0033);
    LCD_Write(0xD432, 0x0003);
    LCD_Write(0xD433, 0x006D);

    /** Gamma (G-) */
    LCD_Write(0xD500, 0x0000);
    LCD_Write(0xD501, 0x0033);
    LCD_Write(0xD502, 0x0000);
    LCD_Write(0xD503, 0x0034);
    LCD_Write(0xD504, 0x0000);
    LCD_Write(0xD505, 0x003A);
    LCD_Write(0xD506, 0x0000);
    LCD_Write(0xD507, 0x004A);
    LCD_Write(0xD508, 0x0000);
    LCD_Write(0xD509, 0x005C);
    LCD_Write(0xD50A, 0x0000);
    LCD_Write(0xD50B, 0x0081);
    LCD_Write(0xD50C, 0x0000);
    LCD_Write(0xD50D, 0x00A6);
    LCD_Write(0xD50E, 0x0000);
    LCD_Write(0xD50F, 0x00E5);
    LCD_Write(0xD510, 0x0001);
    LCD_Write(0xD511, 0x0013);
    LCD_Write(0xD512, 0x0001);
    LCD_Write(0xD513, 0x0054);
    LCD_Write(0xD514, 0x0001);
    LCD_Write(0xD515, 0x0082);
    LCD_Write(0xD516, 0x0001);
    LCD_Write(0xD517, 0x00CA);
    LCD_Write(0xD518, 0x0002);
    LCD_Write(0xD519, 0x0000);
    LCD_Write(0xD51A, 0x0002);
    LCD_Write(0xD51B, 0x0001);
    LCD_Write(0xD51C, 0x0002);
    LCD_Write(0xD51D, 0x0034);
    LCD_Write(0xD51E, 0x0002);
    LCD_Write(0xD51F, 0x0067);
    LCD_Write(0xD520, 0x0002);
    LCD_Write(0xD521, 0x0084);
    LCD_Write(0xD522, 0x0002);
    LCD_Write(0xD523, 0x00A4);
    LCD_Write(0xD524, 0x0002);
    LCD_Write(0xD525, 0x00B7);
    LCD_Write(0xD526, 0x0002);
    LCD_Write(0xD527, 0x00CF);
    LCD_Write(0xD528, 0x0002);
    LCD_Write(0xD529, 0x00DE);
    LCD_Write(0xD52A, 0x0002);
    LCD_Write(0xD52B, 0x00F2);
    LCD_Write(0xD52C, 0x0002);
    LCD_Write(0xD52D, 0x00FE);
    LCD_Write(0xD52E, 0x0003);
    LCD_Write(0xD52F, 0x0010);
    LCD_Write(0xD530, 0x0003);
    LCD_Write(0xD531, 0x0033);
    LCD_Write(0xD532, 0x0003);
    LCD_Write(0xD533, 0x006D);

    /** Gamma (B-) */
    LCD_Write(0xD600, 0x0000);
    LCD_Write(0xD601, 0x0033);
    LCD_Write(0xD602, 0x0000);
    LCD_Write(0xD603, 0x0034);
    LCD_Write(0xD604, 0x0000);
    LCD_Write(0xD605, 0x003A);
    LCD_Write(0xD606, 0x0000);
    LCD_Write(0xD607, 0x004A);
    LCD_Write(0xD608, 0x0000);
    LCD_Write(0xD609, 0x005C);
    LCD_Write(0xD60A, 0x0000);
    LCD_Write(0xD60B, 0x0081);
    LCD_Write(0xD60C, 0x0000);
    LCD_Write(0xD60D, 0x00A6);
    LCD_Write(0xD60E, 0x0000);
    LCD_Write(0xD60F, 0x00E5);
    LCD_Write(0xD610, 0x0001);
    LCD_Write(0xD611, 0x0013);
    LCD_Write(0xD612, 0x0001);
    LCD_Write(0xD613, 0x0054);
    LCD_Write(0xD614, 0x0001);
    LCD_Write(0xD615, 0x0082);
    LCD_Write(0xD616, 0x0001);
    LCD_Write(0xD617, 0x00CA);
    LCD_Write(0xD618, 0x0002);
    LCD_Write(0xD619, 0x0000);
    LCD_Write(0xD61A, 0x0002);
    LCD_Write(0xD61B, 0x0001);
    LCD_Write(0xD61C, 0x0002);
    LCD_Write(0xD61D, 0x0034);
    LCD_Write(0xD61E, 0x0002);
    LCD_Write(0xD61F, 0x0067);
    LCD_Write(0xD620, 0x0002);
    LCD_Write(0xD621, 0x0084);
    LCD_Write(0xD622, 0x0002);
    LCD_Write(0xD623, 0x00A4);
    LCD_Write(0xD624, 0x0002);
    LCD_Write(0xD625, 0x00B7);
    LCD_Write(0xD626, 0x0002);
    LCD_Write(0xD627, 0x00CF);
    LCD_Write(0xD628, 0x0002);
    LCD_Write(0xD629, 0x00DE);
    LCD_Write(0xD62A, 0x0002);
    LCD_Write(0xD62B, 0x00F2);
    LCD_Write(0xD62C, 0x0002);
    LCD_Write(0xD62D, 0x00FE);
    LCD_Write(0xD62E, 0x0003);
    LCD_Write(0xD62F, 0x0010);
    LCD_Write(0xD630, 0x0003);
    LCD_Write(0xD631, 0x0033);
    LCD_Write(0xD632, 0x0003);
    LCD_Write(0xD633, 0x006D);

    /** PAGE0 */
    LCD_Write(0xF000, 0x0055);
    LCD_Write(0xF001, 0x00AA);
    LCD_Write(0xF002, 0x0052);
    LCD_Write(0xF003, 0x0008);
    LCD_Write(0xF004, 0x0000);

    LCD_Write(0xB500, 0x0050);

    /** Dispay control */
    LCD_Write(0xB100, 0x00CC);
    LCD_Write(0xB101, 0x0000);

    /** Source hold time */
    LCD_Write(0xB600, 0x0005);

    /** Gate EQ control */
    LCD_Write(0xB700, 0x0077);
    LCD_Write(0xB701, 0x0077);

    /** Source EQ control */
    LCD_Write(0xB800, 0x0001);
    LCD_Write(0xB801, 0x0003);
    LCD_Write(0xB802, 0x0003);
    LCD_Write(0xB803, 0x0003);

    /** Inversion mode */
    LCD_Write(0xBC00, 0x0002);
    LCD_Write(0xBC01, 0x0000);
    LCD_Write(0xBC02, 0x0000);

    /** Frame rate */
    LCD_Write(0xBD00, 0x0001);
    LCD_Write(0xBD01, 0x0084);
    LCD_Write(0xBD02, 0x001c);
    LCD_Write(0xBD03, 0x001c);
    LCD_Write(0xBD04, 0x0000);

    /** LGD timing control */
    LCD_Write(0xC900, 0x00D0);
    LCD_Write(0xC901, 0x0002);
    LCD_Write(0xC902, 0x0050);
    LCD_Write(0xC903, 0x0050);
    LCD_Write(0xC904, 0x0050);

    LCD_Write(0x3600, 0x0000);
    LCD_Write(0x3500, 0x0000);

    LCD_Write(0xFF00, 0x00AA);
    LCD_Write(0xFF01, 0x0055);
    LCD_Write(0xFF02, 0x0025);
    LCD_Write(0xFF03, 0x0001);

    LCD_Write(0xFC00, 0x0016);
    LCD_Write(0xFC01, 0x00A2);
    LCD_Write(0xFC02, 0x0026);
    LCD_Write(0x3A00, 0x0006);

    LCD_Write(0x3A00, 0x0055);

    LCD_WriteCmd(0x1100);

    LCD_Delay(1000);

    LCD_WriteCmd(0x2900);
}

/*!
 * @brief       Enable Back light.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDEnableBackLight(void)
{
    GPIO_SetBit(GPIOF, GPIO_PIN_14);
}

/*!
 * @brief       Disable Back light.
 *
 * @param       None
 *
 * @retval      None
 */
/*!
 * @brief       Config Register for LCD.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDDisableBackLight(void)
{
    GPIO_ResetBit(GPIOF, GPIO_PIN_14);
}

/*!
 * @brief       Config scan mode.
 *
 * @param       scanMode : scan mode, the top left of the following axis examples
 *                         is the same as the board.
 *                         This parameter can be one of the following values:
 *                         @arg LCD_SCAN_MODE_0 : axis-X and axis-Y on screen:
 *                                                  (y)^
 *                                                     |
 *                                                     |
 *                                              (x)<---0
 *
 *                         @arg LCD_SCAN_MODE_1 : axis-X and axis-Y on screen:
 *                                                  (x)^
 *                                                     |
 *                                                     |
 *                                              (y)<---0
 *
 *                         @arg LCD_SCAN_MODE_2 : axis-X and axis-Y on screen:
 *                                               ^(y)
 *                                               |
 *                                               |
 *                                               0--->(x)
 *
 *                         @arg LCD_SCAN_MODE_3 : axis-X and axis-Y on screen:
 *                                               ^(x)
 *                                               |
 *                                               |
 *                                               0--->(y)
 *
 *                         @arg LCD_SCAN_MODE_4 : axis-X and axis-Y on screen:
 *                                               (x)<---0
 *                                                      |
 *                                                      |
 *                                                      V(y)
 *
 *                         @arg LCD_SCAN_MODE_5 : axis-X and axis-Y on screen:
 *                                               (y)<---0
 *                                                      |
 *                                                      |
 *                                                      V(x)
 *
 *                         @arg LCD_SCAN_MODE_6 : axis-X and axis-Y on screen:
 *                                               0--->(x)
 *                                               |
 *                                               |
 *                                               V(y)
 *
 *                         @arg LCD_SCAN_MODE_7 : axis-X and axis-Y on screen:
 *                                               0--->(y)
 *                                               |
 *                                               |
 *                                               V(x)
 *
 * @retval      None
 */
void BOARD_LCDConfigScan(uint8_t scanMode)
{
    /** Memory Data Access Control */
    LCD_Write(0x3600, scanMode << 5);

    /** Row/Column Exchange */
    if (scanMode & 0x01)
    {
        g_lcdDev.width = LCD_SIDE_LONG;
        g_lcdDev.height = LCD_SIDE_SHORT;
    }
    else
    {
        g_lcdDev.width = LCD_SIDE_SHORT;
        g_lcdDev.height = LCD_SIDE_LONG;
    }
    g_lcdDev.scanMode = scanMode;

    /** Column Address Set */
    LCD_Write(0x2A00, 0x00);
    LCD_Write(0x2A01, 0x00);
    LCD_Write(0x2A02, ((g_lcdDev.width - 1) >> 8) & 0xFF);
    LCD_Write(0x2A03, (g_lcdDev.width - 1) & 0xFF);

    /** Row Address Set */
    LCD_Write(0x2B00, 0x00);
    LCD_Write(0x2B01, 0x00);
    LCD_Write(0x2B02, ((g_lcdDev.height - 1) >> 8) & 0xFF);
    LCD_Write(0x2B03, (g_lcdDev.height - 1) & 0xFF);

    /** Write Memory */
    LCD_WriteCmd(0x2C00);
}

/*!
 * @brief       Open a Window to show.
 *
 * @param       x : axis X lable
 *
 * @param       y : axis Y lable
 *
 * @param       xLen : length for x
 *
 * @param       yLen : length for y
 *
 * @retval      None
 */
void BOARD_LCDOpenWindow(uint16_t x, uint16_t y, uint16_t xLen, uint16_t yLen)
{
    /** Column Address Set */
    LCD_Write(0x2A00, x >> 8);
    LCD_Write(0x2A01, x & 0xFF);
    LCD_Write(0x2A02, (x + xLen - 1) >> 8);
    LCD_Write(0x2A03, (x + xLen - 1) & 0xFF);

    /** Row Address Set */
    LCD_Write(0x2B00, y >> 8);
    LCD_Write(0x2B01, y & 0xFF);
    LCD_Write(0x2B02, (y + yLen - 1) >> 8);
    LCD_Write(0x2B03, (y + yLen - 1) & 0xFF);

    g_lcdDev.lastWindowArea = xLen * yLen;
}

/*!
 * @brief       Fill color for the last window.
 *
 * @param       color : color to be filled
 *
 * @retval      None
 */
void BOARD_LCDFillWindowColor(uint16_t color)
{
    /** Write Memory */
    LCD_WriteCmd(0x2C00);

    for (uint32_t i = 0; i < g_lcdDev.lastWindowArea; i++)
    {
        LCD_WriteData(color);
    }
}

/*!
 * @brief       Clear LCD Screen.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDClearScreen(void)
{
    if (g_lcdDev.scanMode & 0x01)
    {
        BOARD_LCDOpenWindow(0, 0, LCD_SIDE_LONG, LCD_SIDE_SHORT);
    }
    else
    {
        BOARD_LCDOpenWindow(0, 0, LCD_SIDE_SHORT, LCD_SIDE_LONG);
    }

    BOARD_LCDFillWindowColor(g_lcdDev.backColor);
}

/*!
 * @brief       Start To Write LCD Memory.
 *
 * @param       None
 *
 * @retval      None
 */
void BOARD_LCDStartToWriteMemory(void)
{
    LCD_WriteCmd(0x2C00);
}

/*!
 * @brief       Read the max length for axis X of the scan mode now.
 *
 * @param       None
 *
 * @retval      max length for axis X
 */
uint32_t BOARD_LCDReadMaxLenX(void)
{
    return (g_lcdDev.scanMode & 0x01) ? LCD_SIDE_LONG : LCD_SIDE_SHORT;
}

/*!
 * @brief       Read the max length for axis Y of the scan mode now.
 *
 * @param       None
 *
 * @retval      max length for axis Y
 */
uint32_t BOARD_LCDReadMaxLenY(void)
{
    return (g_lcdDev.scanMode & 0x01) ? LCD_SIDE_SHORT : LCD_SIDE_LONG;
}

/*!
 * @brief       Read color for (x, y).
 *
 * @param       x : axis X lable
 *
 * @param       y : axis Y lable
 *
 * @retval      None
 */
uint16_t Board_LCDReadPointColor(uint16_t x, uint16_t y)
{
    uint16_t colorRG, colorB;

    BOARD_LCDOpenWindow(x, y, 1, 1);
    LCD_WriteCmd(0x2E00);

    colorRG = LCD_ReadData();
    colorRG = LCD_ReadData();
    colorRG = LCD_ReadData();
    
    colorRG = LCD_ReadData();
    colorB = LCD_ReadData();

    return (colorRG & 0xF800)| ((colorRG << 3) & 0x7E0) | (colorB >> 11);
}

/*!
 * @brief       Config LCD background color.
 *
 * @param       color : background color
 *
 * @retval      None
 */
void BOARD_LCDConfigBackColor(uint16_t color)
{
    g_lcdDev.backColor = color;
}

/*!
 * @brief       Read LCD background color.
 *
 * @param       None
 *
 * @retval      LCD background color
 */
uint16_t BOARD_LCDReadBackColor(void)
{
    return g_lcdDev.backColor;
}

/*!
 * @brief       Config font type for LCD Char and String.
 *
 * @param       font : Pointer to LCD_FontConfig_T
 *
 * @retval      None
 */
void BOARD_LCDConfigFontType(LCD_FontConfig_T *font)
{
    g_lcdDev.font->height = font->height;
    g_lcdDev.font->width = font->width;
    g_lcdDev.font->pTable = font->pTable;
}

/*!
 * @brief       LCD print char.
 *
 * @param       x     : axis X lable
 *
 * @param       y     : axis Y lable
 *
 * @param       charNum  : ascii for char
 *
 * @param       color    : color of the char
 *
 * @retval      None
 */
uint8_t BOARD_LCDPrintChar(uint16_t x,
                      uint16_t y,
                      uint16_t charNum,
                      uint16_t color)
{
    uint8_t index, data;
    uint16_t tableOffset;
    uint8_t* pTable;

    if ((charNum < ' ') || \
        ((x > g_lcdDev.width - g_lcdDev.font->width)) || \
        (y > g_lcdDev.height - g_lcdDev.font->height))
    {
        return ERROR;
    }
    else
    {
        tableOffset = (charNum - ' ');
        tableOffset *= ((g_lcdDev.font->height * g_lcdDev.font->width) >> 3);
    }

    pTable = &g_lcdDev.font->pTable[tableOffset];

    BOARD_LCDOpenWindow(x, y, g_lcdDev.font->width, g_lcdDev.font->height);
    LCD_WriteCmd(0x2C00);

    for (int32_t i = 0; i < (g_lcdDev.font->width* g_lcdDev.font->height) >> 3; i++)
    {
        data = *pTable++;
        index = 8;

        while (index--)
        {
            if ((data >> index) & 0x1)
            {
                LCD_WriteData(color);
            }
            else
            {
                LCD_WriteData(g_lcdDev.backColor);
            }
        }
    }

    return SUCCESS;
}

/*!
 * @brief       LCD print string.
 *
 * @param       x     : axis X lable
 *
 * @param       y     : axis Y lable
 *
 * @param       pStr  : Pointer to the string
 *
 * @param       color    : color of the string
 *
 * @retval      None
 */
void BOARD_LCDPrintString(uint16_t x, uint16_t y, char* pStr, uint16_t color)
{
    do
    {
        BOARD_LCDPrintChar(x, y, *pStr++, color);
        x += g_lcdDev.font->width;
    }
    while (*pStr != '\0');

}

/*!
 * @brief       LCD display picture
 *
 * @param       x     : axis X lable
 *
 * @param       y     : axis Y lable
 *
 * @param       color : color of the point
 *
 * @retval      None
 */
void BOARD_LCDDrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    BOARD_LCDOpenWindow(x, y, 1, 1);
    BOARD_LCDFillWindowColor(color);
}

/*!
 * @brief       LCD Draw a line
 *
 * @param       xStart    : start of axis X lable
 *
 * @param       yStart    : start of axis Y lable
 *
 * @param       xEnd      : End of axis X lable
 *
 * @param       yEnd      : End of axis Y lable
 *
 * @param       color     : color of the line
 *
 * @retval      None
 */
void BOARD_LCDDrawLine(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
    uint16_t xLength = (xStart > xEnd) ? (xStart - xEnd) : (xEnd - xStart);
    uint16_t yLength = (yStart > yEnd) ? (yStart - yEnd) : (yEnd - yStart);
    int32_t xError = 0, yError = 0, xAdd = 0, yAdd = 0, length; 

    /** Increase or Decrease */
    if (xStart != xEnd)
    {
        xAdd = (xStart > xEnd) ? -1 : 1;
    }

    /** Increase or Decrease */
    if (yStart != yEnd)
    {
        yAdd = (yStart > yEnd) ? -1 : 1;
    }

    if (xLength > yLength)
    {
        length = xLength;
    }
    else
    {
        length = yLength;
    }

    /** Draw Line */
    for (int32_t i = 0; i <= length + 1; i ++ )
    {  
        BOARD_LCDDrawPoint( xStart, yStart, color);

        xError += xLength;
        yError += yLength;

        if (xError > length)
        { 
            xError -= length;
            xStart += xAdd;
        }

        if (yError > length) 
        { 
            yError -= length;
            yStart += yAdd;
        }
    }
}


void BOARD_LCDDrawRectangle(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
    BOARD_LCDDrawLine(xStar, yStar, xEnd, yStar, color);
    BOARD_LCDDrawLine(xStar, yStar, xStar, yEnd, color);
    BOARD_LCDDrawLine(xStar, yStar, xStar, yEnd, color);
    BOARD_LCDDrawLine(xEnd, yStar, xEnd, yEnd, color);
    BOARD_LCDDrawLine(xStar, yEnd, xEnd, yEnd, color);
}

/*!
 * @brief       LCD Draw a circle.
 *
 * @param       xCircle : Axis X lable for Center of circle.
 *
 * @param       yCircle : Axis Y lable for Center of circle.
 *
 * @param       r       : radius of circle.
 *
 * @param       color   : color for the circle
 *
 * @param       fillMode : solid circle or hollow circle,
 *                         This parameter can be one of the following values:
 *                         @arg ENABLE  : Fill color for circle, will be solid circle.
 *                         @arg DISABLE : Don't Fill color for circle, will be hollow circle.
 *
 * @retval      None
 */
void BOARD_LCDDrawCircle(uint16_t xCircle, uint16_t yCircle, uint16_t r, uint16_t color, uint8_t fillMode)
{
    int16_t xEdge = 0;
    int16_t yEdge = r;
    int16_t error = 3 - (r << 1);

    do
    {
        if (!fillMode)
        {
            BOARD_LCDDrawPoint(xCircle + yEdge, yCircle + xEdge, color);
            BOARD_LCDDrawPoint(xCircle + yEdge, yCircle - xEdge, color);
            BOARD_LCDDrawPoint(xCircle + xEdge, yCircle - yEdge, color);
            BOARD_LCDDrawPoint(xCircle - xEdge, yCircle - yEdge, color);
            BOARD_LCDDrawPoint(xCircle - yEdge, yCircle - xEdge, color);
            BOARD_LCDDrawPoint(xCircle - yEdge, yCircle + xEdge, color);
            BOARD_LCDDrawPoint(xCircle - xEdge, yCircle + yEdge, color);
            BOARD_LCDDrawPoint(xCircle + xEdge, yCircle + yEdge, color);
        }
        else
        {
            /** Fill color for section */
            for (int16_t yCnt = xEdge; yCnt <= yEdge; yCnt++)
            {
                BOARD_LCDDrawPoint(xCircle + yCnt,  yCircle + xEdge, color);
                BOARD_LCDDrawPoint(xCircle + yCnt,  yCircle - xEdge, color);
                BOARD_LCDDrawPoint(xCircle + xEdge, yCircle - yCnt,  color);
                BOARD_LCDDrawPoint(xCircle - xEdge, yCircle - yCnt,  color);
                BOARD_LCDDrawPoint(xCircle - yCnt,  yCircle - xEdge, color);
                BOARD_LCDDrawPoint(xCircle - yCnt,  yCircle + xEdge, color);
                BOARD_LCDDrawPoint(xCircle - xEdge, yCircle + yCnt,  color);
                BOARD_LCDDrawPoint(xCircle + xEdge, yCircle + yCnt,  color);
            }
        }

        xEdge++;

        /** Bresenham Arithmetic */
        if (error < 0)
        {
            error += 4 * xEdge + 6;
        }
        else
        {
            error += 10 + 4 * ( xEdge - yEdge);
            yEdge--;
        }
    }
    while (xEdge <= yEdge);
}


/*!
 * @brief       LCD delay
 *
 * @param       cnt: delay counter
 *
 * @retval      None
 */
void LCD_Delay(uint32_t cnt)
{
    __IO uint32_t i = 0;
    __IO uint32_t j = 0;

    do
    {
        i++;
        j++;
    }
    while (i < (cnt * 6));
}

/*!
 * @brief       LCD display picture
 *
 * @param       xStart    : start axis X lable
 *
 * @param       yStart    : start axis Y lable
 *
 * @param       picWidth  : picture width pixe
 *
 * @param       picHeight : picture height pixe
 *
 * @param       imgBuffer : picture buffer array
 *
 * @retval      None
 */
void BOARD_LCDDisplayPic(uint16_t xStart, uint16_t yStart, uint16_t picWidth, uint16_t picHeight, const uint8_t *imgBuffer)
{
    uint16_t i;
    uint8_t picH,picL;
    
    BOARD_LCDOpenWindow(xStart,yStart, picWidth, picHeight);
    LCD_WriteCmd(0x2C00);
    
    for(i = 0; i < picWidth * picHeight; i++)
    {
        picH = *(imgBuffer + i * 2);
        picL = *(imgBuffer + i * 2 + 1);
        LCD_WriteData(picH << 8 | picL);
    }
}
