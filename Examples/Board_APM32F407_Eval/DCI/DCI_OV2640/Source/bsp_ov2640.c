/*!
 * @file        bsp_ov2640.c
 *
 * @brief       OV2640 board support package body
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
#include "bsp_ov2640.h"
#include "cfg_ov2640.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Init SCCB
 *
 * @param       None
 *
 * @retval      None
 *
 */
static void OV2640_SCCBInit(void)
{
    GPIO_Config_T  gpioConfigStruct;

    /** Enable the GPIOD Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);

    /** Configure the GPIOD pin */
    gpioConfigStruct.pin = SCCB_SCL_PIN | SCCB_SDA_PIN;
    gpioConfigStruct.mode = GPIO_MODE_OUT;
    gpioConfigStruct.otype = GPIO_OTYPE_PP;
    gpioConfigStruct.speed = GPIO_SPEED_50MHz;
    gpioConfigStruct.pupd = GPIO_PUPD_UP;

    GPIO_Config(GPIOB, &gpioConfigStruct);

    GPIO_WriteBitValue(GPIOB,SCCB_SCL_PIN,BIT_SET);
    GPIO_WriteBitValue(GPIOB,SCCB_SDA_PIN,BIT_SET);
}

/*!
 * @brief       Start signal of SCCB transmission
 *
 * @param       None
 *
 * @retval      None
 *
 */
static void OV2640_SCCBStart(void)
{
    SCCB_SDA_H;
    SCCB_SCL_H;
    BOARD_DelayUs(50);
    SCCB_SDA_L;
    BOARD_DelayUs(50);
    SCCB_SCL_L;
}

/*!
 * @brief       Stop signal of SCCB transmission
 *
 * @param       None
 *
 * @retval      None
 *
 */
static void OV2640_SCCBStop(void)
{
    SCCB_SDA_L;
    BOARD_DelayUs(50);
    SCCB_SCL_H;
    BOARD_DelayUs(50);
    SCCB_SDA_H;
    BOARD_DelayUs(50);
}

/*!
 * @brief       No ack signal of SCCB transmission
 *
 * @param       None
 *
 * @retval      None
 *
 */
static void OV2640_SCCBNoAck(void)
{
    BOARD_DelayUs(50);
    SCCB_SDA_H;
    SCCB_SCL_H;
    BOARD_DelayUs(50);
    SCCB_SCL_L;
    BOARD_DelayUs(50);
    SCCB_SDA_L;
    BOARD_DelayUs(50);
}

/*!
 * @brief       SCCB Write Data Transmission
 *
 * @param       data: Input the data to be sent.
 *
 * @retval      The write status of SCCB.
 */
static uint8_t OV2640_SCCBWriteByte(uint8_t data)
{
    uint8_t j;
    uint8_t res;

    /** loop eight times to send data */
    for(j = 0; j < 8; j++)
    {
        if(data & 0x80)
        {
            SCCB_SDA_H;
        }
        else
        {
            SCCB_SDA_L;
        }
        data <<= 1;
        BOARD_DelayUs(50);
        SCCB_SCL_H;
        BOARD_DelayUs(50);
        SCCB_SCL_L;
    }

    SCCB_SDA_IN();
    BOARD_DelayUs(50);
    /** Get ack sign */
    SCCB_SCL_H;
    BOARD_DelayUs(50);

    if(SCCB_SDA_INPUT)
    {
        /** Write byte success */
        res = 1;
    }
    else
    {
        /** Write byte fail */
        res = 0;
    }

    SCCB_SCL_L;
    SCCB_SDA_OUT();

    return res;
}

/*!
 * @brief       SCCB Read Data Transmission
 *
 * @param       None
 *
 * @retval      The read value
 *
 */
static uint8_t OV2640_SCCBReadByte(void)
{
    uint8_t temp = 0;
    uint8_t j;

    SCCB_SDA_IN();
    /** loop eight times to get data */
    for(j = 8; j > 0; j--)
    {
        BOARD_DelayUs(50);
        SCCB_SCL_H;
        temp = temp << 1;
        if(SCCB_SDA_INPUT)
        {
            temp++;
        }
        BOARD_DelayUs(50);
        SCCB_SCL_L;
    }
    SCCB_SDA_OUT();

    return temp;
}

/*!
 * @brief       SCCB Write Register
 *
 * @param       None
 *
 * @retval      The status of write register
 *
 */
uint8_t OV2640_SCCBWriteReg(uint8_t reg, uint8_t data)
{
    uint8_t res = 0;
    /** Start transmission */
    OV2640_SCCBStart();
    /** Send device ID */
    if(OV2640_SCCBWriteByte(SCCB_ID))
    {
        res = 1;
    }
    BOARD_DelayUs(100);
    /** Send register address */
    if(OV2640_SCCBWriteByte(reg))
    {
        res = 1;
    }
    BOARD_DelayUs(100);
    /** Send register data */
    if(OV2640_SCCBWriteByte(data))
    {
        res = 1;
    }
    /** Stop transmission */
    OV2640_SCCBStop();

    return res;
}

/*!
 * @brief       SCCB Read Register
 *
 * @param       None
 *
 * @retval      The status of read register
 *
 */
uint8_t OV2640_SCCBReadReg(uint8_t reg)
{
    uint8_t val = 0;
    /** Start transmission */
    OV2640_SCCBStart();
    /** Send device ID */
    OV2640_SCCBWriteByte(SCCB_ID);
    BOARD_DelayUs(100);
    /** Send register address */
    OV2640_SCCBWriteByte(reg);
    BOARD_DelayUs(100);
    /** Stop transmission */
    OV2640_SCCBStop();
    BOARD_DelayUs(100);

    /** Start transmission */
    OV2640_SCCBStart();
    /** Send register read command */
    OV2640_SCCBWriteByte(SCCB_ID | 0X01);
    BOARD_DelayUs(100);
    /** Read register data */
    val = OV2640_SCCBReadByte();
    /** Send no ack sign to device */
    OV2640_SCCBNoAck();
    /** Stop transmission */
    OV2640_SCCBStop();

    return val;
}

/*!
 * @brief       Software Reset OV2640
 *
 * @param       None
 *
 * @retval      None
 *
 */
void OV2640_Reset(void)
{
    /** Select OV2640 register group*/
    OV2640_SCCBWriteReg(DCR_DSP_RA_DLMT, 0x01);

    OV2640_SCCBWriteReg(DCR_SENSOR_COM7, 0x80);
}

/*!
 * @brief       Init OV2640
 *
 * @param       None
 *
 * @retval      None
 *
 */
uint8_t OV2640_Init(void)
{
    uint16_t reg;

    GPIO_Config_T  configStruct;

    /** Enable the GPIOG Clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOF);

    /** Configure the GPIO pin */
    GPIO_ConfigStructInit(&configStruct);
    configStruct.pin = OV2640_PWDN_PIN;
    configStruct.mode = GPIO_MODE_OUT;
    configStruct.otype = GPIO_OTYPE_PP;
    configStruct.speed = GPIO_SPEED_50MHz;
    configStruct.pupd = GPIO_PUPD_UP;

    GPIO_Config(GPIOA, &configStruct);
    
    configStruct.pin = OV2640_RST_PIN;
    configStruct.mode = GPIO_MODE_OUT;
    configStruct.otype = GPIO_OTYPE_PP;
    configStruct.speed = GPIO_SPEED_50MHz;
    configStruct.pupd = GPIO_PUPD_UP;

    GPIO_Config(GPIOF, &configStruct);

    /** Power on OV2640 */
    OV2640_PWDN_L;
    BOARD_DelayMs(10);
    /** Reset OV2640 */
    OV2640_RST_L;
    BOARD_DelayMs(10);
    OV2640_RST_H;

    /** Init OV2640 SCCB*/
    OV2640_SCCBInit();

    /** Software reset OV2640*/
    OV2640_Reset();
    BOARD_DelayMs(50);

    /** Read OV2640 PID*/
    OV2640_SCCBWriteReg(DCR_DSP_RA_DLMT, 0x01);
    
    reg = OV2640_SCCBReadReg(DCR_SENSOR_PIDH);
    reg <<= 8;
    reg |= OV2640_SCCBReadReg(DCR_SENSOR_PIDL);

    if(reg != OV2640_PID)
    {
        /** Init OV2640 Fail*/
        return 1;
    }

    /** Init OV2640 Success*/
    return 0;
}

/*!
 * @brief       OV2640 jpeg mode configuration
 *
 * @param       ImageFormat:
 *              @arg 0: QQVGA 160 x 120
 *              @arg 1: VGA 800 x 480
 *              @arg 2: JPEG 160 x 120
 *              @arg 3: JPEG 176 x 144
 *              @arg 4: JPEG 320 x 240
 *              @arg 5: JPEG 352 x 288
 *
 * @retval      None
 *
 */
void OV2640_JpegModeConfig(ImageFormat_TypeDef ImageFormat)
{
    uint32_t i;

    /** Software reset OV2640*/
    OV2640_Reset();
    BOARD_DelayMs(200);

    /** Set OV2640 resolution is SXGA(800 * 600)*/
    for(i = 0; i < sizeof(ov2640_svga_init_reg_tbl) / 2; i++)
    {
            OV2640_SCCBWriteReg(ov2640_svga_init_reg_tbl[i][0],ov2640_svga_init_reg_tbl[i][1]);
    }

    /** Set to output YUV422 format*/
    for(i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
    {
        OV2640_SCCBWriteReg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]);
    }

    OV2640_SCCBWriteReg(0xff, 0x01);
    OV2640_SCCBWriteReg(0x15, 0x00);

    /** Set to output JPEG */
    for(i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
    {
        OV2640_SCCBWriteReg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);
    }

    BOARD_DelayMs(100);

    /** Set JPEG image format size */
    switch(ImageFormat)
    {
        case JPEG_160x120:
        {
            for(i = 0; i < (sizeof(OV2640_160x120_JPEG) / 2); i++)
            {
                OV2640_SCCBWriteReg(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
            }
            break;
        }

        case JPEG_176x144:
        {
            for(i = 0; i < (sizeof(OV2640_176x144_JPEG) / 2); i++)
            {
                OV2640_SCCBWriteReg(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
            }
            break;
        }

        case JPEG_320x240:
        {
             for(i = 0; i < (sizeof(OV2640_320x240_JPEG) / 2); i++)
            {
                OV2640_SCCBWriteReg(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
            }
            break;
        }

        case JPEG_352x288:
        {
            for(i = 0; i < (sizeof(OV2640_352x288_JPEG) / 2); i++)
            {
                OV2640_SCCBWriteReg(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
            }
            break;
        }

        default:
        {
            for(i = 0; i < (sizeof(OV2640_160x120_JPEG) / 2); i++)
            {
                OV2640_SCCBWriteReg(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
            }
            break;
        }
    }
}

/*!
 * @brief       OV2640 reg565 mode configuration
 *
 * @param       None
 *
 * @retval      None
 *
 */
void OV2640_Rgb565ModeConfig(void)
{
    uint32_t i;

    /** Software reset OV2640*/
    OV2640_Reset();
    BOARD_DelayMs(200);

    for(i = 0; i < (sizeof(OV2640_UXGA) / 2); i++)
    {
      OV2640_SCCBWriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);
    }

    for(i = 0; i < (sizeof(OV2640_UXGA) / 2); i++)
    {
      OV2640_SCCBWriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);
    }

    for(i = 0; i < (sizeof(OV2640_UXGA) / 2); i++)
    {
      OV2640_SCCBWriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);
    }
}

/** Auto exposure level table*/
const uint8_t OV2640_AUTOEXPOSURE_LEVEL[5][8] =
{
    {
        0xFF,0x01,
        0x24,0x20,
        0x25,0x18,
        0x26,0x60,
    },
    {
        0xFF,0x01,
        0x24,0x34,
        0x25,0x1c,
        0x26,0x00,
    },
    {
        0xFF,0x01,
        0x24,0x3e,
        0x25,0x38,
        0x26,0x81,
    },
    {
        0xFF,0x01,
        0x24,0x48,
        0x25,0x40,
        0x26,0x81,
    },
    {
        0xFF,0x01,
        0x24,0x58,
        0x25,0x50,
        0x26,0x92,
    },
};

/*!
 * @brief       Set the auto exposure level of OV2640
 *
 * @param       level: 0,1,2,3,4 auto exposure level
 *
 * @retval      None
 *
 */
void OV2640_AutoExposure(uint8_t level)
{
    uint8_t i;
    uint8_t *p = (uint8_t*)OV2640_AUTOEXPOSURE_LEVEL[level];
    for(i = 0; i < 4; i++)
    {
        OV2640_SCCBWriteReg(p[i*2],p[i*2+1]);
    }
}

/*!
 * @brief       Set the light mode of OV2640
 *
 * @param       mode: The light mode
 *              @arg 0: auto
 *              @arg 1: sunny
 *              @arg 2: cloudy
 *              @arg 3: office
 *              @arg 4: home
 *
 * @retval      None
 *
 */
void OV2640_LightModeConfig(uint8_t mode)
{
    /** sunny mode*/
    uint8_t regccval = 0X5E;
    uint8_t regcdval = 0X41;
    uint8_t regceval = 0X54;

    switch(mode)
    {
        /** auto mode*/
        case 0:
            OV2640_SCCBWriteReg(0XFF,0X00);
            /** AWB ON */
            OV2640_SCCBWriteReg(0XC7,0X10);
            return;

        /** cloudy mode*/
        case 2:
            regccval=0X65;
            regcdval=0X41;
            regceval=0X4F;
            break;

        /** office mode*/
        case 3:
            regccval=0X52;
            regcdval=0X41;
            regceval=0X66;
            break;

        /** home mode*/
        case 4:
            regccval=0X42;
            regcdval=0X3F;
            regceval=0X71;
            break;
    }

    OV2640_SCCBWriteReg(0XFF,0X00);
    /** AWB OFF  */
    OV2640_SCCBWriteReg(0XC7,0X40);
    OV2640_SCCBWriteReg(0XCC,regccval);
    OV2640_SCCBWriteReg(0XCD,regcdval);
    OV2640_SCCBWriteReg(0XCE,regceval);
}

/*!
 * @brief       Set color saturation of OV2640
 *
 * @param       sat:saturation
 *              @arg 0:-2
 *              @arg 1:-1
 *              @arg 2:0
 *              @arg 3:+1
 *              @arg 4:+2
 *
 * @retval      None
 *
 */
void OV2640_ColorSaturation(uint8_t saturation)
{
    uint8_t reg7dval = ((saturation + 2) << 4) | 0X08;

    OV2640_SCCBWriteReg(0XFF,0X00);
    OV2640_SCCBWriteReg(0X7C,0X00);
    OV2640_SCCBWriteReg(0X7D,0X02);
    OV2640_SCCBWriteReg(0X7C,0X03);
    OV2640_SCCBWriteReg(0X7D,reg7dval);
    OV2640_SCCBWriteReg(0X7D,reg7dval);
}

/*!
 * @brief       Set brightness of OV2640
 *
 * @param       bright: brightness
 *              @arg 0: (0x00) - 2
 *              @arg 1: (0x10) - 1
 *              @arg 2: (0x20)   0
 *              @arg 3: (0x30) + 1
 *              @arg 4: (0x40) + 2
 *
 * @retval      None
 *
 */
void OV2640_Brightness(uint8_t bright)
{
    OV2640_SCCBWriteReg(0xff, 0x00);
    OV2640_SCCBWriteReg(0x7c, 0x00);
    OV2640_SCCBWriteReg(0x7d, 0x04);
    OV2640_SCCBWriteReg(0x7c, 0x09);
    OV2640_SCCBWriteReg(0x7d, bright << 4);
    OV2640_SCCBWriteReg(0x7d, 0x00);
}

/*!
 * @brief       Set contrast of OV2640
 *
 * @param       contrast:
 *              @arg 0: -2
 *              @arg 1: -1
 *              @arg 2:  0
 *              @arg 3: +1
 *              @arg 4: +2
 *
 * @retval      None
 *
 */
void OV2640_Contrast(uint8_t contrast)
{
  /** contrast 0  */
    uint8_t reg7d0val = 0X20;
    uint8_t reg7d1val = 0X20;

    switch(contrast)
    {
        case 0:
            reg7d0val = 0X18;
            reg7d1val = 0X34;
            break;

        case 1:
            reg7d0val = 0X1C;
            reg7d1val = 0X2A;
            break;

        case 3:
            reg7d0val = 0X24;
            reg7d1val = 0X16;
            break;

        case 4:
            reg7d0val = 0X28;
            reg7d1val = 0X0C;
            break;
    }
    OV2640_SCCBWriteReg(0xff,0x00);
    OV2640_SCCBWriteReg(0x7c,0x00);
    OV2640_SCCBWriteReg(0x7d,0x04);
    OV2640_SCCBWriteReg(0x7c,0x07);
    OV2640_SCCBWriteReg(0x7d,0x20);
    OV2640_SCCBWriteReg(0x7d,reg7d0val);
    OV2640_SCCBWriteReg(0x7d,reg7d1val);
    OV2640_SCCBWriteReg(0x7d,0x06);
}

/*!
 * @brief       Set special effects of OV2640
 *
 * @param       eft: effects
 *              @arg 0: common mode
 *              @arg 1: negative film
 *              @arg 2: black and white
 *              @arg 3: reddish
 *              @arg 4: greenish
 *              @arg 5: bluish
 *              @arg 6: retro
 *
 * @retval      None
 *
 */
void OV2640_SpecialEffects(uint8_t effects)
{
    /** common mode  */
    uint8_t reg7d0val = 0X00;
    uint8_t reg7d1val = 0X80;
    uint8_t reg7d2val = 0X80;

    switch(effects)
    {
        case 1:
            reg7d0val = 0X40;
            break;

        case 2:
            reg7d0val = 0X18;
            break;

        case 3:
            reg7d0val = 0X18;
            reg7d1val = 0X40;
            reg7d2val = 0XC0;
            break;

        case 4:
            reg7d0val = 0X18;
            reg7d1val = 0X40;
            reg7d2val = 0X40;
            break;

        case 5:
            reg7d0val = 0X18;
            reg7d1val = 0XA0;
            reg7d2val = 0X40;
            break;

        case 6:
            reg7d0val = 0X18;
            reg7d1val = 0X40;
            reg7d2val = 0XA6;
            break;
    }

    OV2640_SCCBWriteReg(0xff,0x00);
    OV2640_SCCBWriteReg(0x7c,0x00);
    OV2640_SCCBWriteReg(0x7d,reg7d0val);
    OV2640_SCCBWriteReg(0x7c,0x05);
    OV2640_SCCBWriteReg(0x7d,reg7d1val);
    OV2640_SCCBWriteReg(0x7d,reg7d2val);
}

/*!
 * @brief       Enable or disable OV2640 color bar mode
 *
 * @param       switchMode: switch mode
 *              @arg 0: disable
 *              @arg 1: enable
 *
 * @retval      None
 *
 * @note        The color bar of OV2640 is superimposed on the image
 */
void OV2640_ColorBar(uint8_t switchMode)
{
    uint8_t reg;
    OV2640_SCCBWriteReg(0XFF,0X01);
    reg = OV2640_SCCBReadReg(0X12);
    reg &=~(1 << 1);
    if(switchMode)
    {
        reg |= 1 << 1;
    }
    OV2640_SCCBWriteReg(0X12,reg);
}

/*!
 * @brief       Config the image output window of OV2640
 *
 * @param       sx:X axis address
 *
 * @param       sy:Y axis address
 *
 * @param       width:horizontal size
 *
 * @param       height:vertical size
 *
 * @retval      None
 *
 */
void OV2640_WindowConfig(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t endx;
    uint16_t endy;
    uint8_t temp;
    endx = sx + width / 2;
    endy = sy + height / 2;

    OV2640_SCCBWriteReg(0XFF,0X01);
    /** Read the before value of Vref  */
    temp = OV2640_SCCBReadReg(0X03);

    temp &= 0XF0;
    temp |= ((endy & 0X03) << 2) | (sy & 0X03);
    /** Set the lowest two bits of start and end of Vref  */
    OV2640_SCCBWriteReg(0X03,temp);

    /** Set the high 8 bits of Vref's start  */
    OV2640_SCCBWriteReg(0X19,sy >> 2);
    /** Set the high 8 bits of Vref's end  */
    OV2640_SCCBWriteReg(0X1A,endy >> 2);

    /** Read the before value of Href  */
    temp = OV2640_SCCBReadReg(0X32);

    temp &= 0XC0;
    temp |= ((endx & 0X07) << 3) | (sx & 0X07);
    /** Set the lowest three bits of start and end of Href  */
    OV2640_SCCBWriteReg(0X32,temp);
    /** Set the high 8 bits of Href's start  */
    OV2640_SCCBWriteReg(0X17,sx >> 3);
    /** Set the high 8 bits of Href's end  */
    OV2640_SCCBWriteReg(0X18,endx >> 3);
}

/*!
 * @brief       Set the output size of image
 *
 * @param       width:horizontal
 *
 * @param       height:vertical
 *
 * @retval      Setting Status 0:success, else:fail
 *
 * @note        Width and height must be multiples of four
 */
uint8_t OV2640_OutputSizeConfig(uint16_t width, uint16_t height)
{
    uint16_t outh;
    uint16_t outw;
    uint8_t temp;

    if(width % 4)
    {
        return 1;
    }
    if(height % 4)
    {
        return 2;
    }

    outw = width / 4;
    outh = height / 4;
    OV2640_SCCBWriteReg(0xFF,0x00);
    OV2640_SCCBWriteReg(0xE0,0x04);
    OV2640_SCCBWriteReg(0x50,outw & 0x00);
    /** Set the low 8 bits of OUTW  */
    OV2640_SCCBWriteReg(0x5A,outw & 0xFF);
    /** Set the low 8 bits of OUTH  */
    OV2640_SCCBWriteReg(0x5B,outh & 0xFF);
    temp = (outw >> 8) & 0x03;
    temp |= (outh >> 6) & 0x04;
    /** Set the high 8 bits of OUTW and OUTH  */
    OV2640_SCCBWriteReg(0x5C,temp);
    OV2640_SCCBWriteReg(0xE0,0x00);

    /** Setting Success  */
    return 0;
}

/*!
 * @brief       Config the image open window of OV2640
 *
 * @param       offx: XOFF register
 *
 * @param       offy: YOFF register
 *
 * @param       width: horizontal
 *
 * @param       height: vertical
 *
 * @retval      None
 *
 * @note        Width and height must be multiples of four.
 *              Width and height must be large than OV2640_OutputSizeConfig's width and height.
 *
 */
uint8_t OV2640_ImageWindowConfig(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{
    uint16_t hsize;
    uint16_t vsize;
    uint8_t temp;

    if(width % 4)return 1;
    if(height % 4)return 2;

    hsize = width / 4;
    vsize = height / 4;
    OV2640_SCCBWriteReg(0XFF,0X00);
    OV2640_SCCBWriteReg(0XE0,0X04);
    /** Set the low 8 bits of horizontal size */
    OV2640_SCCBWriteReg(0X51,hsize & 0XFF);
    /** Set the low 8 bits of vertical size */
    OV2640_SCCBWriteReg(0X52,vsize & 0XFF);
    /** Set the low 8 bits of XOFF size */
    OV2640_SCCBWriteReg(0X53,offx & 0XFF);
    /** Set the low 8 bits of YOFF size */
    OV2640_SCCBWriteReg(0X54,offy & 0XFF);

    temp = (vsize >> 1) & 0X80;
    temp |= (offy >> 4) & 0X70;
    temp |= (hsize >> 5) & 0X08;
    temp |= (offx >> 8) & 0X07;
    /** Set the high 8 bits of XOFF / YOFF / horizontal / vertical */
    OV2640_SCCBWriteReg(0X55,temp);
    OV2640_SCCBWriteReg(0X57,(hsize >> 2) & 0X80);
    OV2640_SCCBWriteReg(0XE0,0X00);

    /** Setting Success  */
    return 0;
}

/*!
 * @brief       Sets the image size, that the output resolution of the selected format
 *
 * @param       width: horizontal
 *
 * @param       height: vertical
 *
 * @retval      None
 *
 */
uint8_t OV2640_ImageSizeConfig(uint16_t width, uint16_t height)
{
    uint8_t temp;

    OV2640_SCCBWriteReg(0XFF,0X00);
    OV2640_SCCBWriteReg(0XE0,0X04);
    /** Set the [10:3] bits of horizontal size */
    OV2640_SCCBWriteReg(0XC0,(width) >> 3 & 0XFF);
    /** Set the [10:3] bits of vertical size */
    OV2640_SCCBWriteReg(0XC1,(height) >> 3 & 0XFF);

    temp = (width & 0X07) << 3;
    temp |= height & 0X07;
    temp |= (width >> 4) & 0X80;
    OV2640_SCCBWriteReg(0X8C,temp);
    OV2640_SCCBWriteReg(0XE0,0X00);

    /** Setting Success  */
    return 0;
}
