/*!
 * @file        board_lcd.h
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

/* Define to prevent recursive inclusion */
#ifndef __BOARD_LCD_H
#define __BOARD_LCD_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "apm32f4xx.h"
#include "board_lcdFont.h"

/* Exported macro *********************************************************/
/** Select bank of SMC */
#define LCD_GPIO_CS_CLK         RCM_AHB1_PERIPH_GPIOG
#define LCD_GPIO_CS_PORT        GPIOG
#define LCD_GPIO_CS_PIN         GPIO_PIN_10
#define LCD_GPIO_CS_PIN_SOURC   GPIO_PIN_SOURCE_10
#define LCD_GPIO_CS_AF          GPIO_AF_DMC

/** Address pin of SMC(note: Once change this , you should check @arg LCD_SMC_GPIO_ADDR) */
#define LCD_GPIO_ADD_CLK         RCM_AHB1_PERIPH_GPIOG
#define LCD_GPIO_ADD_PORT        GPIOG
#define LCD_GPIO_ADD_PIN         GPIO_PIN_0
#define LCD_GPIO_ADD_PIN_SOURC   GPIO_PIN_SOURCE_0
#define LCD_GPIO_ADD_AF          GPIO_AF_DMC

/** RST */
#define LCD_GPIO_RST_CLK         RCM_AHB1_PERIPH_GPIOE
#define LCD_GPIO_RST_PORT        GPIOE
#define LCD_GPIO_RST_PIN         GPIO_PIN_3

/** Back light control */
#define LCD_GPIO_BK_CLK          RCM_AHB1_PERIPH_GPIOF
#define LCD_GPIO_BK_PORT         GPIOF
#define LCD_GPIO_BK_PIN          GPIO_PIN_14

/** Length of the screen side */
#define LCD_SIDE_SHORT          480  //!< The Short Side
#define LCD_SIDE_LONG           800  //!< The Long Side

/** color */
#define LCD_COLOR_WHITE         ((uint16_t)0xFFFF)
#define LCD_COLOR_BLACK         ((uint16_t)0x0000)
#define LCD_COLOR_RED           ((uint16_t)0xF800)
#define LCD_COLOR_GRED          ((uint16_t)0XFFE0)
#define LCD_COLOR_BRED          ((uint16_t)0XF81F)
#define LCD_COLOR_BRRED         ((uint16_t)0XFC07)
#define LCD_COLOR_GREEN         ((uint16_t)0x07E0)
#define LCD_COLOR_BLUE          ((uint16_t)0x001F)
#define LCD_COLOR_GBLUE         ((uint16_t)0X07FF)
#define LCD_COLOR_YELLOW        ((uint16_t)0xFFE0)
#define LCD_COLOR_MAGENTA       ((uint16_t)0xF81F)
#define LCD_COLOR_CYAN          ((uint16_t)0x7FFF)
#define LCD_COLOR_BROWN         ((uint16_t)0XBC40)
#define LCD_COLOR_GRAY          ((uint16_t)0X8430)

#define LCD_R                   1    //!< User define, must be less than 31
#define LCD_G                   60   //!< User define, must be less than 63
#define LCD_B                   23   //!< User define, must be less than 31
#define LCD_COLOR_USER          ((uint16_t)((LCD_R << 11 & 0xF800) | (LCD_G << 5 & 0x07E0) | (LCD_B & 0x001F)))

#define LCD_COLOR_BACKGROUND    LCD_COLOR_WHITE   //!< Default background color
#define LCD_COLOR_TEXT          LCD_COLOR_USER    //!< Default background color

/** Address AF number for SMC */
#define LCD_SMC_GPIO_ADDR       10

#define LCD_SMC_ADDR_CMD        (0x68000000)
#define LCD_SMC_ADDR_DATA       (0x68000000 | (1 << (LCD_SMC_GPIO_ADDR + 1)))

/** Write Cmd from SMC to LCD */
#define LCD_WriteCmd(cmd)       (*(__IO uint16_t*)(LCD_SMC_ADDR_CMD) = cmd)
/** Write data from SMC to LCD */
#define LCD_WriteData(data)     (*(__IO uint16_t*)(LCD_SMC_ADDR_DATA) = data)
/** Write cmd and data from SMC to LCD */
#define LCD_Write(cmd, data)    {LCD_WriteCmd(cmd); LCD_WriteData(data);}
/** Read data of LCD through SMC */
#define LCD_ReadData()          (*(__IO uint16_t*)(LCD_SMC_ADDR_DATA))

/* Exported typedef *******************************************************/
typedef enum
{
    LCD_SCAN_MODE_0,
    LCD_SCAN_MODE_1,
    LCD_SCAN_MODE_2,
    LCD_SCAN_MODE_3,
    LCD_SCAN_MODE_4,
    LCD_SCAN_MODE_5,
    LCD_SCAN_MODE_6,
    LCD_SCAN_MODE_7
} LCD_SCAN_MODE_T;

typedef struct 
{
    uint8_t  scanMode;
    uint16_t width;
    uint16_t height;
    uint16_t backColor;
    uint32_t lastWindowArea;
    LCD_FontConfig_T *font;
} LCD_DeviceConfig_T;

/* Exported variables *****************************************************/

/* Exported function prototypes *******************************************/
/** Initialization */
void BOARD_LCDGPIOInit(void);
void BOARD_LCDSMCInit(void);
void BOARD_LCDReset(void);
void BOARD_LCDConfig(LCD_DeviceConfig_T lcdDev);
void BOARD_LCDConfigRegister(void);
void BOARD_LCDEnableBackLight(void);
void BOARD_LCDDisableBackLight(void);

/** Common */
void BOARD_LCDConfigScan(uint8_t scanMode);
void BOARD_LCDOpenWindow(uint16_t x, uint16_t y, uint16_t xLen, uint16_t yLen);
void BOARD_LCDFillWindowColor(uint16_t color);
void BOARD_LCDClearScreen(void);
void BOARD_LCDStartToWriteMemory(void);
uint32_t BOARD_LCDReadMaxLenX(void);
uint32_t BOARD_LCDReadMaxLenY(void);

/** Color Configuration */
uint16_t Board_LCDReadPointColor(uint16_t x, uint16_t y);
void BOARD_LCDConfigBackColor(uint16_t color);
uint16_t BOARD_LCDReadBackColor(void);

/** Font */
void BOARD_LCDConfigFontType(LCD_FontConfig_T *font);
uint8_t BOARD_LCDPrintChar(uint16_t x,
                      uint16_t y,
                      uint16_t charNum,
                      uint16_t color);
void BOARD_LCDPrintString(uint16_t x,
                     uint16_t y,
                     char     *pStr,
                     uint16_t color);

/** Figure */
void BOARD_LCDDrawPoint(uint16_t x, uint16_t y, uint16_t color);
void BOARD_LCDDrawLine(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void BOARD_LCDDrawRectangle(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void BOARD_LCDDrawCircle(uint16_t xCircle, uint16_t yCircle, uint16_t r, uint16_t color, uint8_t mode);
void BOARD_LCDDisplayPic(uint16_t xStart, uint16_t yStart, uint16_t picWidth, uint16_t picHeight, const uint8_t *imgBuffer);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_LCD_H */
