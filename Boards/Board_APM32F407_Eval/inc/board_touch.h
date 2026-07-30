/*!
 * @file        board_touch.h
 *
 * @brief       Header for board_touch.c module
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
#ifndef _BOARD_TOUCH_H
#define _BOARD_TOUCH_H

/* Includes ***************************************************************/
#include "main.h"

/* Exported macro *********************************************************/
#define LCD_GT_RST_GPIO_PIN             GPIO_PIN_6
#define LCD_GT_RST_GPIO_PORT            GPIOD
#define LCD_GT_RST_GPIO_CLK             RCM_AHB1_PERIPH_GPIOD
#define LCD_GT_RST_SOURCE               GPIO_PIN_SOURCE_6

#define LCD_GT_RST_0                    GPIO_ResetBit(LCD_GT_RST_GPIO_PORT,LCD_GT_RST_GPIO_PIN)
#define LCD_GT_RST_1                    GPIO_SetBit(LCD_GT_RST_GPIO_PORT,LCD_GT_RST_GPIO_PIN)

#define LCD_GT_INT_GPIO_PIN             GPIO_PIN_2
#define LCD_GT_INT_GPIO_PORT            GPIOE
#define LCD_GT_INT_GPIO_CLK             RCM_AHB1_PERIPH_GPIOE
#define LCD_GT_INT_SOURCE               GPIO_PIN_SOURCE_2

#define LCD_GT_INT_EINT_PORT_SOURCE     SYSCFG_PORT_GPIOE
#define LCD_GT_INT_EINT_LINE            EINT_LINE_2
#define LCD_GT_INT_EINT_PIN_SOURCE      SYSCFG_PIN_2
#define LCD_GT_INT_EINT_IRQn            EINT2_IRQn

/**
 * 1: config to update touch chip data
 * 0: do not update touch chip data
 */
#define UPDATE_CHIP_CONFIG              0
#define UPDATA_CHIP_CONFIG_CHECK        1

#define LCD_TOUCH_ADDRESS               0xBA

#define LCD_TOUCH_MAX_HEIGHT            480
#define LCD_TOUCH_MAX_WIDTH             800
#define LCD_TOUCH_INT_TRIG_TIME         0
#define LCD_TOUCH_MAX_TOUCH_POINT       5
#define LCD_TOUCH_ADDR_LEN              2
#define LCD_TOUCH_CFG_MIN_LEN           186
#define LCD_TOUCH_CFG_MAX_LEN           256

/** Registers define*/
#define LCD_TOUCH_READ_COOR_REG_ADDR    0x814E
#define LCD_TOUCH_SLEEP_REG_ADDR        0x8040
#define LCD_TOUCH_SENSOR_REG_ID         0x814A
#define LCD_TOUCH_CONFIG_REG_DATA       touchChipInfo[touchICType].configRegAddr
#define LCD_TOUCH_VERSION_REG_ADDR      0x8140

#define LCD_TOUCH_X2Y_LOC               0x08

#define LCD_TOUCH_GROUP_LENGHT(pCfgGrp) (sizeof(pCfgGrp) / sizeof(pCfgGrp[0]))

/* Exported typedef *******************************************************/
/**
 * @brief Touch press status
 */
typedef enum
{
    PRESS_NONE,
    PRESS_DOWN,
    PRESS_UP,
} TOUCH_PRESS_STATUS_T;

/**
 * @brief Touch information
 */
typedef struct
{
    uint16_t touchID;
    uint32_t axisLableX;
    uint32_t axisLableY;
    uint32_t touchPointWidth;
    TOUCH_PRESS_STATUS_T pressStatus;
} TOUCH_INFO_T;

/**
 * @brief Touch chip I2C operation type
 */
typedef enum
{
    I2C_OP_WR,
    I2C_OP_RD,
} I2C_OP_T;

/**
 * @brief Touch chip I2C Message information
 */
typedef struct
{
    uint8_t *buffer;
    uint8_t address;
    uint16_t lenght;
    uint16_t flags;
} I2C_MSG_T;

/**
 * @brief LCD touch chip information
 */
typedef struct
{
  uint16_t contactMaxWidth;  //!< Maximun width of contact point
  uint16_t contactMaxHeight;  //!< Maximun height of contact point
  uint16_t configRegAddr;  //!< chip config register
} TOUCH_CHIP_INFO_T;

/**
 * @brief Touch IC type
 */
typedef enum
{
    GT917S,
    GT911,
} TOUCH_IC_T;

/**
 * @brief Touch IC communication status
 */
typedef enum
{
    COM_FAIL,
    COM_SUCCESS,
} TOUCH_IC_COM_STA_T;

/* Exported variables *****************************************************/
extern TOUCH_INFO_T touchInfo;

/* Exported function prototypes *******************************************/
int8_t BOARD_TouchBoardInit(void);
void BOARD_TouchBoardProcess(void);

#endif /* _BOARD_TOUCH_H */
