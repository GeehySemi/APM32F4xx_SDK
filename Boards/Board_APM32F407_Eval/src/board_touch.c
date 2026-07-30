/*!
 * @file        board_touch.c
 *
 * @brief       Touch chip GT91xx board support package body
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
#include "board_touch.h"
#include "board_delay.h"
#include "board_i2c.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "app.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
#if UPDATE_CHIP_CONFIG
/** GT917S chip config array*/
const uint8_t GT917S_RegConfig[] = {
    0x84,0x20,0x03,0xE0,0x01,0x05,0x05,0x00,
    0x00,0x40,0x00,0x0F,0x78,0x64,0x53,0x11,
    0x00,0x00,0x00,0x00,0x23,0x17,0x19,0x1D,
    0x0F,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
    0x04,0x51,0x14,0x00,0x00,0x00,0x00,0x00,
    0x32,0x00,0x00,0x50,0x38,0x28,0x8A,0x20,
    0x11,0x37,0x39,0xA2,0x07,0x38,0x6D,0x28,
    0x11,0x03,0x24,0x00,0x01,0x28,0x50,0xC0,
    0x94,0x02,0x00,0x00,0x53,0xB8,0x2E,0xA2,
    0x35,0x8F,0x3B,0x80,0x42,0x75,0x49,0x6B,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,
    0x4C,0x3C,0xFF,0xFF,0x07,0x14,0x14,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x50,0x73,0x50,0x32,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1F,0x1D,0x1B,0x1A,
    0x19,0x18,0x17,0x16,0x15,0x09,0x0A,0x0B,
    0x0C,0x0D,0x0E,0x0F,0x10,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0x1C,0x1B,0x1A,0x19,
    0x18,0x17,0x15,0x14,0x13,0x12,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0x00,0x00,0x05,0x00,0x00,0x0F,
    0x00,0x00,0x00,0x80,0x46,0x08,0x96,0x50,
    0x32,0x0A,0x0A,0x64,0x32,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x32,0x03,0x0C,0x08,
    0x23,0x00,0x14,0x23,0x00,0x28,0x46,0x30,
    0x3C,0xD0,0x07,0x50,0x70,0xB0,0x01,
};
#endif

TOUCH_INFO_T touchInfo;

TOUCH_IC_T touchICType = GT917S;
const TOUCH_CHIP_INFO_T touchChipInfo[1] =
{
    /** GT917S,4.3inch LCD */
    {
        .configRegAddr = 0x8050,
        .contactMaxWidth = 800,
        .contactMaxHeight = 480,
    },
};

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       LCD touch EINT Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_EintInit(void)
{
    GPIO_Config_T     gpioConfigStruct;
    EINT_Config_T     eintConfigStruct;

    /** Enable the SYSCFG Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    RCM_EnableAHB1PeriphClock(LCD_GT_INT_GPIO_CLK);

    gpioConfigStruct.pin = LCD_GT_INT_GPIO_PIN;
    gpioConfigStruct.mode = GPIO_MODE_IN;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(LCD_GT_INT_GPIO_PORT, &gpioConfigStruct);

    /** Reset the SYSCFG Periph */
    SYSCFG_Reset();

    /** Connect EINT Line to GPIO Pin */
    SYSCFG_ConfigEINTLine(LCD_GT_INT_EINT_PORT_SOURCE, LCD_GT_INT_EINT_PIN_SOURCE);

    /** Configure EINT line */
    eintConfigStruct.line = LCD_GT_INT_EINT_LINE;
    eintConfigStruct.mode = EINT_MODE_INTERRUPT;
    eintConfigStruct.trigger = EINT_TRIGGER_RISING;
    eintConfigStruct.lineCmd = ENABLE;
    EINT_Config(&eintConfigStruct);

    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_1);

    /** Enable and set EINT Interrupt to the lowest priority */
    NVIC_EnableIRQRequest(LCD_GT_INT_EINT_IRQn, 0x01, 0x01);
}

/*!
 * @brief       LCD touch GPIO Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_GPIOInit(void)
{
    GPIO_Config_T     gpioConfigStruct;

    RCM_EnableAHB1PeriphClock(LCD_GT_RST_GPIO_CLK);
    RCM_EnableAHB1PeriphClock(LCD_GT_INT_GPIO_CLK);

    gpioConfigStruct.pin = LCD_GT_RST_GPIO_PIN;
    gpioConfigStruct.mode = GPIO_MODE_OUT;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    gpioConfigStruct.pupd = GPIO_PUPD_DOWN;
    GPIO_Config(LCD_GT_RST_GPIO_PORT, &gpioConfigStruct);

    gpioConfigStruct.pin = LCD_GT_INT_GPIO_PIN;
    GPIO_Config(LCD_GT_INT_GPIO_PORT, &gpioConfigStruct);
}

/*!
 * @brief       LCD touch reset chip
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_ResetChip(void)
{
    GPIO_Config_T     gpioConfigStruct;

    RCM_EnableAHB1PeriphClock(LCD_GT_INT_GPIO_CLK);

    gpioConfigStruct.pin = LCD_GT_INT_GPIO_PIN;
    gpioConfigStruct.mode = GPIO_MODE_OUT;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    gpioConfigStruct.pupd = GPIO_PUPD_DOWN;
    GPIO_Config(LCD_GT_INT_GPIO_PORT, &gpioConfigStruct);

    /** Reset chip*/
    LCD_GT_RST_0;
    BOARD_DelayMs(100);
    LCD_GT_RST_1;
    BOARD_DelayMs(100);

    gpioConfigStruct.pin = LCD_GT_INT_GPIO_PIN;
    gpioConfigStruct.mode = GPIO_MODE_IN;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(LCD_GT_INT_GPIO_PORT, &gpioConfigStruct);
}

/*!
 * @brief       LCD touch init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Touch_Init(void)
{
    BOARD_I2CGPIOInit();
    Touch_GPIOInit();

    Touch_ResetChip();

    Touch_EintInit();
}

/*!
 * @brief       LCD touch I2C transfer
 *
 * @param       message
 *
 * @param       msgSize
 *
 * @retval      None
 *
 */
static int16_t Touch_I2CTransfer(I2C_MSG_T *message, int16_t msgSize)
{
    int16_t msgSuccessCnt;
    int16_t result;

    result = 0;
    msgSuccessCnt = 0;
    
    for(msgSuccessCnt = 0; result == 0 && msgSuccessCnt != msgSize; msgSuccessCnt++)
    {
        if((message[msgSuccessCnt].flags&I2C_OP_RD))
        {
            result = BOARD_I2CReadData(message[msgSuccessCnt].address, \
            message[msgSuccessCnt].buffer, message[msgSuccessCnt].lenght);
        }
        else
        {
            result = BOARD_I2CWriteData(message[msgSuccessCnt].address, \
            message[msgSuccessCnt].buffer, message[msgSuccessCnt].lenght);
        }
    }

    if(result)
    {
        return result;
    }

    return msgSuccessCnt;
}

/*!
 * @brief       LCD touch I2C read operation
 *
 * @param       deviceAddr
 *
 * @param       buffer
 *
 * @param       len
 *
 * @retval      None
 *
 */
static int32_t Touch_I2C_Read(uint8_t deviceAddr, uint8_t *buffer, uint32_t len)
{
    I2C_MSG_T message[2];
    int32_t result;
    int32_t retries;
    uint8_t timeout;
    
    timeout = 5;
    result = -1;
    retries = 0;

    /** write operation*/
    message[0].flags    = I2C_OP_WR;
    message[0].address  = deviceAddr;
    message[0].lenght   = LCD_TOUCH_ADDR_LEN;
    message[0].buffer   = &buffer[0];

    /** read operation*/
    message[1].flags    = I2C_OP_RD;
    message[1].address  = deviceAddr;
    message[1].lenght   = len - LCD_TOUCH_ADDR_LEN;
    message[1].buffer   = &buffer[LCD_TOUCH_ADDR_LEN];

    while(retries < timeout)
    {
        result = Touch_I2CTransfer(message, LCD_TOUCH_ADDR_LEN);
        if(result == LCD_TOUCH_ADDR_LEN)
        {
            break;
        }
        
        retries++;
    }
    if(retries >= timeout)
    {
        printf("Touch_I2C_Read: 0x%04X, %ld bytes failed, errcode: %ld! Please reset.\r\n", \
        (((uint16_t)(buffer[0] << 8)) | buffer[1]), len - 2, result);
    }
    
    return result;
}

/*!
 * @brief       LCD touch I2C write operation
 *
 * @param       deviceAddr
 *
 * @param       buffer
 *
 * @param       len
 *
 * @retval      None
 *
 */
static int32_t Touch_I2C_Write(uint8_t deviceAddr,uint8_t *buffer,int32_t len)
{
    I2C_MSG_T message;
    int32_t result;
    int32_t retries;
    uint8_t timeout;
    
    timeout = 5;
    
    result = -1;
    retries = 0;

    /** write operation*/
    message.flags = I2C_OP_WR;
    message.address  = deviceAddr;
    message.lenght   = len;
    message.buffer   = buffer;

    while(retries < timeout)
    {
        result = Touch_I2CTransfer(&message, 1);
        if(result == 1)
        {
            break;
        }
        
        retries++;
    }
    
    if(retries >= timeout)
    {

        printf("Touch_I2C_Write: 0x%04X, %ld bytes failed, errcode: %ld! Please reset.", \
        (((uint16_t)(buffer[0] << 8)) | buffer[1]), len-2, result);

    }
    
    return result;
}

/*!
 * @brief       Test I2C communication
 *
 * @param       None
 *
 * @retval      status code
 *
 */
static int8_t Touch_I2CTest(void)
{
    uint8_t testRegAddr[3] = 
    {
        LCD_TOUCH_CONFIG_REG_DATA >> 8, 
        LCD_TOUCH_CONFIG_REG_DATA & 0xff
    };
    
    uint8_t retries;
    int8_t result;
    uint8_t timeout;
    
    timeout = 5;
    retries = 0;
    result = -1;

    while(retries++ < timeout)
    {
        result = Touch_I2C_Read(LCD_TOUCH_ADDRESS, testRegAddr, 3);
        if (result > 0)
        {
            return result;
        }
        
        printf("Touch_I2C_Test failed time %d.\r\n",retries);
    }
    
    return result;
}

/*!
 * @brief       Read touch chip version
 *
 * @param       None
 *
 * @retval      status code
 *
 */
int32_t Touch_ReadVersion(void)
{
    uint8_t verRegAddr[8] = 
    {
        LCD_TOUCH_VERSION_REG_ADDR >> 8, 
        LCD_TOUCH_VERSION_REG_ADDR & 0xff
    };
    
    int32_t result;
    
    result = -1;

    result = Touch_I2C_Read(LCD_TOUCH_ADDRESS, verRegAddr, sizeof(verRegAddr));
    if(result < 0)
    {
        printf("Touch read version failed\r\n");
        return result;
    }

    if (verRegAddr[5] == 0x00)
    {
        printf("Touch IC Ver: %c%c%c_%02x%02x\r\n", \
        verRegAddr[2], verRegAddr[3], verRegAddr[4], verRegAddr[7], verRegAddr[6]);

        /** GT911*/
        if((verRegAddr[2] == '9') && (verRegAddr[3] == '1') && (verRegAddr[4] == '1'))
        {
            touchICType = GT911;
        }
    }
    else
    {
        printf("Touch IC Ver: %c%c%c_%02x%02x\r\n", \
        verRegAddr[2], verRegAddr[3], verRegAddr[4], verRegAddr[7], verRegAddr[6]);
    }

    return result;
}

/*!
 * @brief       Get touch chip information
 *
 * @param       None
 *
 * @retval      status code
 *
 */
static int32_t Touch_GetInfo(void)
{
    uint8_t opRegBuf[10];
    int32_t result;

    uint16_t axisXAbsMax;
    uint16_t axisYAbsMax;
    uint8_t trigIntTime;
    
    result = 0;
    axisXAbsMax = LCD_TOUCH_MAX_WIDTH;
    axisYAbsMax = LCD_TOUCH_MAX_HEIGHT;
    trigIntTime = LCD_TOUCH_INT_TRIG_TIME;

    opRegBuf[0] = (uint8_t)((LCD_TOUCH_CONFIG_REG_DATA + 1) >> 8);
    opRegBuf[1] = (uint8_t)((LCD_TOUCH_CONFIG_REG_DATA + 1) & 0xFF);

    result = Touch_I2C_Read(LCD_TOUCH_ADDRESS, opRegBuf, 10);
    if (result < 0)
    {
        return COM_FAIL;
    }

    axisXAbsMax = (opRegBuf[3] << 8) + opRegBuf[2];
    axisYAbsMax = (opRegBuf[5] << 8) + opRegBuf[4];

    opRegBuf[0] = (uint8_t)((LCD_TOUCH_CONFIG_REG_DATA + 6) >> 8);
    opRegBuf[1] = (uint8_t)((LCD_TOUCH_CONFIG_REG_DATA + 6) & 0xFF);
    
    result = Touch_I2C_Read(LCD_TOUCH_ADDRESS, opRegBuf, 3);
    if (result < 0)
    {
        return COM_FAIL;
    }
    
    trigIntTime = opRegBuf[2] & 0x03;

    printf("Axis_X_MAX = %d, Axis_Y_MAX = %d, TrigTime = 0x%02x\r\n",
            axisXAbsMax,axisYAbsMax,trigIntTime);

    return COM_SUCCESS;
}

/*!
 * @brief       Touch board init
 *
 * @param       None
 *
 * @retval      status code
 *
 */
int8_t BOARD_TouchBoardInit(void)
{
    int32_t result;
    
#if UPDATE_CHIP_CONFIG
    int32_t i;
    uint16_t checkSum;
    int32_t retries;
    const uint8_t* configInfo;
    uint8_t configInfoLen;
    uint8_t* configBuf;
    uint8_t configNum;
    
    i = 0;
    checkSum = 0;
    retries = 0;
    configNum = 0;
#endif
    
    result = -1;
    
    /** Touch chip init*/
    Touch_Init();

    result = Touch_I2CTest();
    if (result < 0)
    {
        printf("Touch Chip I2C communication failed\r\n!");
        
        return result;
    }

    /** Read touch chip version*/
    Touch_ReadVersion();

#if UPDATE_CHIP_CONFIG

    configBuf = (uint8_t *)malloc (LCD_TOUCH_CFG_MAX_LEN + LCD_TOUCH_ADDR_LEN);

    configBuf[0] = LCD_TOUCH_CONFIG_REG_DATA >> 8;
    configBuf[1] = LCD_TOUCH_CONFIG_REG_DATA & 0xff;

    /** select config array*/
    if(touchICType == GT917S)
    {
        configInfo = GT917S_RegConfig;
        configInfoLen = LCD_TOUCH_GROUP_LENGHT(GT917S_RegConfig);
    }
    else
    {
        configInfo = GT917S_RegConfig;
        configInfoLen = LCD_TOUCH_GROUP_LENGHT(GT917S_RegConfig);
    }

    memset(&configBuf[LCD_TOUCH_ADDR_LEN], 0, LCD_TOUCH_CFG_MAX_LEN);
    memcpy(&configBuf[LCD_TOUCH_ADDR_LEN], configInfo, configInfoLen);


    configNum = configInfoLen;

    printf("configInfoLen = %d\r\n",configInfoLen);
    printf("configNum = %d\r\n",configNum);

    /** set-up resolution*/
    configBuf[LCD_TOUCH_ADDR_LEN+1] = lcdWidth & 0xFF;
    configBuf[LCD_TOUCH_ADDR_LEN+2] = lcdWidth >> 8;
    configBuf[LCD_TOUCH_ADDR_LEN+3] = lcdHight & 0xFF;
    configBuf[LCD_TOUCH_ADDR_LEN+4] = lcdHight >> 8;

    /** set-up X2Y logic*/
    switch(5)
    {
        case 0:
        case 2:
        case 4: 
        case 6:
            configBuf[LCD_TOUCH_ADDR_LEN+6] |= (LCD_TOUCH_X2Y_LOC);
            break;

        case 1:
        case 3:
        case 5: 
        case 7:
            configBuf[LCD_TOUCH_ADDR_LEN+6] &= ~(LCD_TOUCH_X2Y_LOC);
            break;
    }

    /** calculate checksum*/
    checkSum = 0;

    for (i = LCD_TOUCH_ADDR_LEN; i < (configNum+LCD_TOUCH_ADDR_LEN -3); i += 2)
    {
        checkSum += (configBuf[i] << 8) + configBuf[i + 1];
    }

    checkSum = 0 - checkSum;
    printf("Config checksum: 0x%04X\r\n", checkSum);
    
    /** Update checksum*/
    configBuf[(configNum+LCD_TOUCH_ADDR_LEN -3)] = (checkSum >> 8) & 0xFF;
    configBuf[(configNum+LCD_TOUCH_ADDR_LEN -2)] = checkSum & 0xFF;
    configBuf[(configNum+LCD_TOUCH_ADDR_LEN -1)] = 0x01;

    /** write config*/
    for (retries = 0; retries < 5; retries++)
    {
        result = Touch_I2C_Write(LCD_TOUCH_ADDRESS, configBuf , configNum + LCD_TOUCH_ADDR_LEN + 2);
        if (result > 0)
        {
            break;
        }
    }
    Delay_ms(100);

#if UPDATA_CHIP_CONFIG_CHECK
    {
        uint16_t i;
        uint8_t buf[300];
        buf[0] = configBuf[0];
        buf[1] = configBuf[1];

        result = Touch_I2C_Read(LCD_TOUCH_ADDRESS, buf, sizeof(buf));

        for(i = 1;i < configNum + LCD_TOUCH_ADDR_LEN - 3;i++)
        {
            if(configBuf[i] != buf[i])
            {
                printf("Config fail! i = %d\r\n",i);
                free(configBuf);
                
                return -1;
            }
        }
        if(i == configNum + LCD_TOUCH_ADDR_LEN - 3)
        {
            printf("Config success! i = %d\r\n",i);
        }
    }
#endif
    free(configBuf);
#endif

    /** Enable Touch gtxxx chip interrupt*/
    Touch_EintInit();

    /** Get touch chip information*/
    Touch_GetInfo();

    return 0;
}

/*!
 * @brief       Touch board touch point release
 *
 * @param       id: touch point id
 *
 * @retval      None
 *
 */
static void LCD_TouchUp(uint32_t id)
{
	(void)id;
    if(touchLED.touchDownStatus)
    {
        touchLED.touchUpStatus = 1;
    }
    else if(touchRefresh.touchDownStatus)
    {
        touchRefresh.touchUpStatus = 1;
    }
    else if(touchHelp.touchDownStatus)
    {
        touchHelp.touchUpStatus = 1;
    }
}

/*!
 * @brief       Touch board touch point press down
 *
 * @param       id: touch point id
 *
 * @param       axisLableX: touch axis lable X
 *
 * @param       axisLableY: touch axis lable Y
 *
 * @param       touchPointWidth: touch point width
 *
 * @retval      None
 *
 */
static void LCD_TouchDown(uint32_t id,uint32_t axisLableX,uint32_t axisLableY,uint32_t touchPointWidth)
{
	(void)id;
	(void)touchPointWidth;
    if((axisLableX > 500) && (axisLableX < 540))
    {
        if((axisLableY > 200) && (axisLableY < 270))
        {
            if((touchHelp.touchDownStatus == 0) && (touchRefresh.touchDownStatus == 0))
            {
                touchLED.touchDownStatus = 1;
            }
        }
    }
    
    if((axisLableX > 380) && (axisLableX < 450))
    {
        if((axisLableY > 200) && (axisLableY < 270))
        {
            if((touchLED.touchDownStatus == 0) && (touchHelp.touchDownStatus == 0))
            {
                touchRefresh.touchDownStatus = 1;
            }
        }
    }
    
    if((axisLableX > 280) && (axisLableX < 350))
    {
        if((axisLableY > 200) && (axisLableY < 270))
        {
            if((touchLED.touchDownStatus == 0) && (touchRefresh.touchDownStatus == 0))
            {
                touchHelp.touchDownStatus = 1;
            }
        }
    }
}

/*!
 * @brief       Touch board handle function
 *
 * @param       touch information
 *
 * @retval      None
 *
 */
static void TouchBoard_Handle(TOUCH_INFO_T *info)
{
    uint8_t deviceAddr = LCD_TOUCH_ADDRESS;
    
    uint8_t  pointData[2 + 1 + 8 * LCD_TOUCH_MAX_TOUCH_POINT + 1] = 
    {
        LCD_TOUCH_READ_COOR_REG_ADDR >> 8,
        LCD_TOUCH_READ_COOR_REG_ADDR & 0xFF
    };
    
    uint8_t  errorEndCmd[3] = 
    {
        LCD_TOUCH_READ_COOR_REG_ADDR >> 8,
        LCD_TOUCH_READ_COOR_REG_ADDR & 0xFF,
        0
    };
    
    static uint16_t lastTouch;
    static uint8_t lastID[LCD_TOUCH_MAX_TOUCH_POINT] = { 0 };
    uint8_t touchNum;
    uint8_t fingerData;

    uint8_t* coorData = NULL;
    int32_t inputXlable;
    int32_t inputYlable;
    int32_t inputWidth;
    uint8_t id;
    
    int32_t i;
    uint8_t j;
    int32_t result;
    
    result = -1;

    /** 10 bytes register data and 2 bytes register address*/
    result = Touch_I2C_Read(deviceAddr, pointData, 12);
    if (result < 0)
    {
        printf("Touch I2C Read error. errno:%ld\n", result);

        return;
    }

    /** status register data*/
    fingerData = pointData[LCD_TOUCH_ADDR_LEN];

    if (fingerData == 0x00)
    {
        return;
    }

    if((fingerData & 0x80) == 0)
    {
        /** lable invaild*/
        goto exitWork;
    }

    touchNum = fingerData & 0x0F;
    
    if (touchNum > LCD_TOUCH_MAX_TOUCH_POINT)
    {
        /** exceed the max of touch point*/
        goto exitWork;
    }

    if (touchNum > 1)
    {
        uint8_t buf[8 * LCD_TOUCH_MAX_TOUCH_POINT] =
        {
            (LCD_TOUCH_READ_COOR_REG_ADDR + 10) >> 8,
            (LCD_TOUCH_READ_COOR_REG_ADDR + 10) & 0xff
        };

        result = Touch_I2C_Read(deviceAddr, buf, 2 + 8 * (touchNum - 1));
        memcpy(&pointData[12], &buf[2], 8 * (touchNum - 1));
    }

    /** some touch point release*/
    if (lastTouch > touchNum)
    {
        for (i = 0; i < lastTouch; i++)
        {
            for(j=0; j<touchNum; j++)
            {
                coorData = &pointData[j * 8 + 3];
                id = coorData[0] & 0x0F;
                if(lastID[i] == id)
                {
                    break;
                }

                if(j >= touchNum - 1)
                {
                     if((lastTouch > 10) || (touchNum > 10))
                    {
                        lastTouch = 0;
                        touchNum = 0;
                        printf("process error and reset\r\n");
                        
                        break;
                    }
                    
                    /** return struct */
                    info->axisLableX = 0;
                    info->axisLableY = 0;
                    info->touchID = lastID[i];
                    info->touchPointWidth = 0;
                    info->pressStatus = PRESS_UP;
                    
                    LCD_TouchUp(lastID[i]);
                }
            }
       }
    }

    if(touchNum)
    {
        for(i = 0; i < touchNum; i++)
        {
            coorData = &pointData[i * 8 + 3];

            id = coorData[0] & 0x0F;
            lastID[i] = id;

            inputXlable  = coorData[1] | (coorData[2] << 8);
            inputYlable  = coorData[3] | (coorData[4] << 8);
            inputWidth  = coorData[5] | (coorData[6] << 8);

            /** return struct */
            info->axisLableX = inputXlable;
            info->axisLableY = inputYlable;
            info->touchID = id;
            info->touchPointWidth = inputWidth;
            info->pressStatus = PRESS_DOWN;
            
            LCD_TouchDown(id, inputXlable, inputYlable, inputWidth);
        }
    }
    else if(lastTouch)
    {
        for(i = 0; i < lastTouch; i++)
        {
            if((lastTouch > 10) || (touchNum > 10))
            {
                lastTouch = 0;
                touchNum = 0;
                printf("process error and reset\r\n");

                break;
            }
            /** return struct */
            info->axisLableX = 0;
            info->axisLableY = 0;
            info->touchID = lastID[i];
            info->touchPointWidth = 0;
            info->pressStatus = PRESS_UP;
            
            LCD_TouchUp(lastID[i]);
        }
    }

    lastTouch = touchNum;

exitWork:
    result = Touch_I2C_Write(deviceAddr, errorEndCmd, 3);
    if (result < 0)
    {
        printf("I2C write errorEndCmd error!");
    }
}

/*!
 * @brief       Touch board process callback from interrupt
 *
 * @param       None
 *
 * @retval      None
 *
 */
void BOARD_TouchBoardProcess(void)
{
    char *str;
    
    TouchBoard_Handle(&touchInfo);
    
    /** Print touch information*/
    if(touchInfo.pressStatus != PRESS_NONE)
    {
        printf("ID[%d]",touchInfo.touchID);
        
        if(touchInfo.pressStatus == PRESS_DOWN)
        {
            str = "Down,";
            printf(",Press Status:%s",str);
            
            printf("X:%ld, Y:%ld, W:%ld\r\n",touchInfo.axisLableX,touchInfo.axisLableY,touchInfo.touchPointWidth);
        }
        else if(touchInfo.pressStatus == PRESS_UP)
        {
            str = "Released,";
            printf(",Press Status:%s\r\n",str);
        }
    }
    BOARD_LEDOff(LED1);
}
