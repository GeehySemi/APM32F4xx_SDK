/*!
 * @file        app.c
 *
 * @brief       Application processing program body
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
#include "app.h"
#include "board_touch.h"
#include "board_i2c.h"
#include "board_delay.h"
#include "picture.h"
#include "board_lcd.h"
#include "board_lcdFont.h"
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
TOUCH_EVENT_T touchLED;
TOUCH_EVENT_T touchRefresh;
TOUCH_EVENT_T touchHelp;

static uint8_t ledStatus = 0;

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Touch board display initial UI
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_DisplayInitUI(void)
{
    BOARD_LCDDisplayPic(260,100,240,108,imageGeehyLogo);
    
    BOARD_LCDDisplayPic(250,220,50,50,imageLedOFF);
    
    BOARD_LCDDisplayPic(350,220,50,50,imageRefresh);
    
    BOARD_LCDDisplayPic(450,220,50,50,imageHelp);
    
    BOARD_LCDPrintString(30, 10, "APM32F407 EVAL Touch Example!", LCD_COLOR_USER);
}

/*!
 * @brief       Touch board help button event handle
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_HelpHandle(void)
{
    BOARD_LCDPrintString(270, 300, "www.geehy.com", LCD_COLOR_BLUE);
}

/*!
 * @brief       Touch board refresh button event handle
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_RefreshHandle(void)
{
    BOARD_LCDClearScreen();
    
    Touch_DisplayInitUI();
    
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);
    
    ledStatus = 0;
}

/*!
 * @brief       Touch board led button event handle
 *
 * @param       status: led button touch status
 *
 * @retval      None
 *
 */
void Touch_LedHandle(uint8_t status)
{
    if(status)
    {
        BOARD_LCDDisplayPic(250,220,50,50,imageLedON);
        BOARD_LEDOn(LED2);
        BOARD_LEDOn(LED3);
    }
    else
    {
        BOARD_LCDDisplayPic(250,220,50,50,imageLedOFF);
        BOARD_LEDOff(LED2);
        BOARD_LEDOff(LED3);
    }
}

/*!
 * @brief       Touch board event handle
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_EventHandle(void)
{
    /** touch help button down event*/
    if(touchHelp.touchDownStatus)
    {
        BOARD_LCDDisplayPic(450,220,50,50,imageHelp2);
    }
    
    /** touch help button up event*/
    if(touchHelp.touchUpStatus)
    {
        BOARD_LCDDisplayPic(450,220,50,50,imageHelp);
    }
    
    /** touch refresh button down event*/
    if(touchRefresh.touchDownStatus)
    {
        BOARD_LCDDisplayPic(350,220,50,50,imageRefresh2);
    }
    
    /** touch refresh button up event*/
    if(touchRefresh.touchUpStatus)
    {
        BOARD_LCDDisplayPic(350,220,50,50,imageRefresh);
    }
    
    if(ledStatus)
    {
        /** touch led button down event*/
        if(touchLED.touchDownStatus)
        {
            BOARD_LCDDisplayPic(250,220,50,50,imageLedON2);
        }
        
        /** touch led button up event*/
        if(touchLED.touchUpStatus)
        {
            BOARD_LCDDisplayPic(250,220,50,50,imageLedON);
        }
    }
    else
    {
        /** touch led button down event*/
        if(touchLED.touchDownStatus)
        {
            BOARD_LCDDisplayPic(250,220,50,50,imageLedOFF2);
        }
        
        /** touch led button up event*/
        if(touchLED.touchUpStatus)
        {
            BOARD_LCDDisplayPic(250,220,50,50,imageLedOFF);
        }
    }
    
    /** touch led button click event*/
    if(touchLED.touchDownStatus && touchLED.touchUpStatus)
    {
        touchLED.touchDownStatus = 0;
        touchLED.touchUpStatus = 0;
        
        ledStatus ^= 1;
        
        Touch_LedHandle(ledStatus);
    }
    
    /** touch help button click event*/
    if(touchHelp.touchDownStatus && touchHelp.touchUpStatus)
    {
        touchHelp.touchDownStatus = 0;
        touchHelp.touchUpStatus = 0;

        Touch_HelpHandle();
    }
    
    /** touch refresh button click event*/
    if(touchRefresh.touchDownStatus && touchRefresh.touchUpStatus)
    {
        touchRefresh.touchDownStatus = 0;
        touchRefresh.touchUpStatus = 0;
        
        Touch_RefreshHandle();
    }
}
