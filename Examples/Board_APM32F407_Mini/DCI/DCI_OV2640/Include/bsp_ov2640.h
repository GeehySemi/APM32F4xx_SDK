/*!
 * @file        bsp_ov2640.h
 *
 * @brief       Header for bsp_ov2640.c module
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

/* Define to prevent recursive inclusion */
#ifndef _BSP_OV2640_H
#define _BSP_OV2640_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "Board.h"

/* Exported macro *********************************************************/

/* OV2640 SCCB GPIO*/
#define SCCB_SCL_PIN                GPIO_PIN_6
#define SCCB_SDA_PIN                GPIO_PIN_7

#define SCCB_SDA_IN()               { GPIOD->MODE &= ~(3 << (7 * 2)); GPIOD->MODE |= 0 << 7 * 2; }
#define SCCB_SDA_OUT()              { GPIOD->MODE &= ~(3 << (7 * 2)); GPIOD->MODE |= 1 << 7 * 2; }

#define SCCB_SCL_H                  GPIOD->BSCL = SCCB_SCL_PIN
#define SCCB_SCL_L                  GPIOD->BSCH = SCCB_SCL_PIN

#define SCCB_SDA_H                  GPIOD->BSCL = SCCB_SDA_PIN
#define SCCB_SDA_L                  GPIOD->BSCH = SCCB_SDA_PIN

#define SCCB_SDA_INPUT              GPIOD->IDATA & SCCB_SDA_PIN

/* OV2640 SCCB ID*/
#define SCCB_ID                     0X60

/* OV2640 power down and reset GPIO*/
#define OV2640_PWDN_PIN             GPIO_PIN_9
#define OV2640_RST_PIN              GPIO_PIN_15

#define OV2640_PWDN_H               GPIOG->BSCL = OV2640_PWDN_PIN
#define OV2640_PWDN_L               GPIOG->BSCH = OV2640_PWDN_PIN

#define OV2640_RST_H                GPIOG->BSCL = OV2640_RST_PIN
#define OV2640_RST_L                GPIOG->BSCH = OV2640_RST_PIN

/* OV2640 MID default value*/
#define OV2640_MID                  0X7FA2
/* OV2640 PID default value*/
#define OV2640_PID                  0X2642

/* Exported typedef *******************************************************/

/*!
 * @brief     OV2640 image format
 */
typedef enum
{
    BMP_QQVGA             =   0x00,     //!< BMP Image QQVGA 160x120 Size
    BMP_QVGA              =   0x01,     //!< BMP Image VGA 800*480 Size
    JPEG_160x120          =   0x02,     //!< JPEG Image 160x120 Size
    JPEG_176x144          =   0x03,     //!< JPEG Image 176x144 Size
    JPEG_320x240          =   0x04,     //!< JPEG Image 320x240 Size
    JPEG_352x288          =   0x05      //!< JPEG Image 352x288 Size
}ImageFormat_TypeDef;

/* OV2640 Registers definition when DSP bank selected (0xFF = 0x00)*/
#define DCR_DSP_RA_DLMT      0xFF

/* OV2640 Registers definition when sensor bank selected (0xFF = 0x01)*/
#define DCR_SENSOR_COM7       0x12
#define DCR_SENSOR_MIDH       0x1C
#define DCR_SENSOR_MIDL       0x1D

/* Exported function prototypes *******************************************/

uint8_t OV2640_SCCBWriteReg(uint8_t reg, uint8_t data);
uint8_t OV2640_SCCBReadReg(uint8_t reg);
uint8_t OV2640_Init(void);
void OV2640_JpegModeConfig(ImageFormat_TypeDef ImageFormat);
void OV2640_Rgb565ModeConfig(void);
void OV2640_AutoExposure(uint8_t level);
void OV2640_LightModeConfig(uint8_t mode);
void OV2640_ColorSaturation(uint8_t saturation);
void OV2640_Brightness(uint8_t bright);
void OV2640_Contrast(uint8_t contrast);
void OV2640_SpecialEffects(uint8_t effects);
void OV2640_ColorBar(uint8_t switchMode);
void OV2640_WindowConfig(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
uint8_t OV2640_OutputSizeConfig(uint16_t width, uint16_t height);
uint8_t OV2640_ImageWindowConfig(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height);
uint8_t OV2640_ImageSizeConfig(uint16_t width, uint16_t height);

#ifdef __cplusplus
}
#endif

#endif

