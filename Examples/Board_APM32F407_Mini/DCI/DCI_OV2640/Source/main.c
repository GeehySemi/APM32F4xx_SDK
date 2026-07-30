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
#include "bsp_usart.h"
#include "bsp_ov2640.h"
#include "bsp_dci.h"
#include "bsp_key.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Camera data buffer size*/
#define CAMERA_BUF_SIZE 31 * 1024
/* Camera data buffer*/
#if defined   (__CC_ARM) //!< ARM Compiler
__align(4) u32 cameraBuffer[CAMERA_BUF_SIZE];

#elif defined ( __ICCARM__ )
unsigned int cameraBuffer[CAMERA_BUF_SIZE];

#elif defined ( __GNUC__ )
unsigned int cameraBuffer[CAMERA_BUF_SIZE];

#endif

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* OV2640 work mode variable */
OV2640_MODE g_ov2640Mode = JPEG_MODE;

/*
   Camera data capture status variable
   0:Capture not completed
   1:Capture completed but data has not processed
   2:Data processing completed
*/
volatile u8 g_staCameraDataCaputure = 0;

/* Statistics of the effective lenght of camera data buffer*/
volatile u32 g_cameraDataLenght = 0;

/* Private function prototypes ********************************************/

/* private function prototypes */
/* OV2640 jpeg mode test function */
void OV2640_JpegTest(void);
/* OV2640 rgb565 mode test function */
void OV2640_Rgb565Test(void);

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 *
 */
int main(void)
{
    SystemClockConfig();
    uint8_t keyType;

    /* Set system interrupt priority grouping */
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_2);

    /* Init delay function */
    BOARD_DelayConfig();

    /* Init LED */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /* Init USART1 */
    USART1_Init(115200);

    /* Init KEY */
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /* Init OV2640 Module */
    while(OV2640_Init())
    {
        BOARD_DelayMs(200);
    }

    /* Wait for the user to select test function  */
    while(1)
    {
        keyType = Key_Scan(KEY_MODE_SINGLE);

        if(keyType == KEY1_PRESSED)
        {
            BOARD_LEDOn(LED2);
            /* Select rgb565 test function  */
            g_ov2640Mode = RGB565_MODE;
            break;
        }
        else if(keyType == KEY2_PRESSED)
        {
             BOARD_LEDOff(LED2);
            /* Select jpeg test function  */
            g_ov2640Mode = JPEG_MODE;
            break;
        }

        BOARD_DelayMs(10);
    }

    /* Enter test function  */
    if(g_ov2640Mode == JPEG_MODE)
    {
        OV2640_JpegTest();
    }
    else if(g_ov2640Mode == RGB565_MODE)
    {
        OV2640_Rgb565Test();
    }
}

/*!
 * @brief     Start the jpeg data output test of OV2640.
 *            This function will continuously output data with jpeg format from USART1.
 *
 * @param     None
 *
 * @retval    None
 *
 */
void OV2640_JpegTest(void)
{
    /* Count the data lenght of jpeg */
    volatile u32 jpgLenght = 0;
    /* Flag the start position of jpeg data */
    volatile uint32_t jpgStartFlag = 0;
    /* Flag the data head of jpeg */
    volatile uint8_t headFlag = 0;

    uint32_t i;
    uint8_t *p;

    uint8_t effect = 0;
    uint8_t saturation = 2;
    uint8_t contrast = 2;

    /* Set OV2640 output mode */
    OV2640_JpegModeConfig(JPEG_320x240);
    /* Init DCI */
    DCI_Init();
    /* Init DMA */
    DCI_DMAInit((uint32_t)&cameraBuffer,CAMERA_BUF_SIZE,DMA_MEMORY_DATA_SIZE_WORD,DMA_MEMORY_INC_ENABLE,DMA_MEMORYBURST_SINGLE);

    /* Set OV2640 image parameter */
    OV2640_Contrast(contrast);
    OV2640_SpecialEffects(effect);
    OV2640_ColorSaturation(saturation);
    /* Start image data transmission */
    DCI_Start();

    /* Hundle jpeg data */
    while(1)
    {
        /* Get whole image */
        if(g_staCameraDataCaputure == 1)
        {
            p = (uint8_t*)cameraBuffer;
            /* DMA memory size is 4 bytes */
            for(i = 0; i < g_cameraDataLenght * 4; i++)
            {
                /* Find the head of data */
                if((p[i] == 0xFF) && (p[i+1] == 0xD8))
                {
                    /* Record the position of jpeg head */
                    jpgStartFlag = i;

                    headFlag = 1;
                }
                /* Find the end of data */
                if((p[i] == 0xFF) && (p[i+1] == 0xD9) && headFlag)
                {
                    /* Avoid some error image data */
                    if(jpgStartFlag < i)
                    {
                        jpgLenght = i - jpgStartFlag + 2;
                    }
                    else
                    {
                        jpgLenght = 0;
                    }
                    break;
                }
            }

            /* Get right jpeg image */
            if(jpgLenght)
            {
                /* Shift to the data head of jpeg */
                p += jpgStartFlag;
                for(i = 0; i < jpgLenght; i++)
                {
                    USART_Write(USART1,&p[i],1);
                }
            }
            /* DMA can get next image */
            g_staCameraDataCaputure = 2;
        }
    }
}
/*!
 * @brief     OV2640_JpegDataCallback
 *
 * @param     None
 *
 * @retval    None
 *
 * @note
 */
void OV2640_JpegDataCallback(void)
{
    if(g_staCameraDataCaputure == 0)
    {
        /* Disalbe DMA2 stream1 */
        DMA_Disable(DMA2_Stream1);
        /* Wait DMA stream1 can be setting */
        while (DMA_ReadCmdStatus(DMA2_Stream1) != DISABLE){}
        /* Get data lenght at this transmission */
        g_cameraDataLenght = CAMERA_BUF_SIZE - DMA_ReadDataNumber(DMA2_Stream1);

        /* Flag the data collected */
        g_staCameraDataCaputure = 1;
    }
    /* Start next capture */
    if(g_staCameraDataCaputure == 2)
    {
        DMA2_Stream1->NDATA = CAMERA_BUF_SIZE;
        DMA_ConfigDataNumber(DMA2_Stream1,CAMERA_BUF_SIZE);
        DMA_Enable(DMA2_Stream1);
        /* Flag the data collection is not completed */
        g_staCameraDataCaputure = 0;
    }
}

/*!
 * @brief     Start the rgb565 data output test of OV2640.
 *            This function will continuously output data with jpeg format from USART1.
 *
 * @param     None
 *
 * @retval    None
 *
 */
void OV2640_Rgb565Test(void)
{
    uint32_t i;
    uint8_t *p;

    uint8_t effect = 0;
    uint8_t saturation = 2;
    uint8_t contrast = 2;

    /* Set OV2640 output mode */
    OV2640_Rgb565ModeConfig();
    /* Set OV2640 output size */
    OV2640_OutputSizeConfig(800,400);

    /* Init DCI */
    DCI_Init();

    /*
       Init DMA
       If use the LCD screen to display the rgb565 image,
       can set the DMA buffer address to the data address of LCD.
    */
    /* Init DMA */
    DCI_DMAInit((uint32_t)&cameraBuffer,CAMERA_BUF_SIZE,DMA_MEMORY_DATA_SIZE_WORD,DMA_MEMORY_INC_ENABLE,DMA_MEMORYBURST_SINGLE);

    /* Set OV2640 image parameter */
    OV2640_Contrast(contrast);
    OV2640_SpecialEffects(effect);
    OV2640_ColorSaturation(saturation);

    /* Start image data transmission */
    DCI_Start();

    while(1)
    {
        /* Get whole image */
        if(g_staCameraDataCaputure == 1)
        {
            p = (uint8_t*)cameraBuffer;

            /* DMA memory size is 4 bytes */
            for(i = 0; i < g_cameraDataLenght * 1; i++)
            {
                USART_Write(USART1,&p[i],1);
            }

            /* DMA can get next image */
            g_staCameraDataCaputure = 2;
        }
    }
}

/*!
 * @brief     OV2640_Rgb565DataCallback
 *
 * @param     None
 *
 * @retval    None
 *
 * @note
 */
void OV2640_Rgb565DataCallback(void)
{
    if(g_staCameraDataCaputure == 0)
    {
        /* Disalbe DMA2 stream1 */
        DMA_Disable(DMA2_Stream1);
        /* Wait DMA stream1 can be setting */
        while (DMA_ReadCmdStatus(DMA2_Stream1) != DISABLE){}
        /* Get data lenght at this transmission */
        g_cameraDataLenght = CAMERA_BUF_SIZE - DMA_ReadDataNumber(DMA2_Stream1);

        /* Flag the data collected */
        g_staCameraDataCaputure = 1;
    }
    /* Start next capture */
    if(g_staCameraDataCaputure == 2)
    {
        DMA2_Stream1->NDATA = CAMERA_BUF_SIZE;
        DMA_ConfigDataNumber(DMA2_Stream1,CAMERA_BUF_SIZE);
        DMA_Enable(DMA2_Stream1);
        /* Flag the data collection is not completed */
        g_staCameraDataCaputure = 0;
    }
}
