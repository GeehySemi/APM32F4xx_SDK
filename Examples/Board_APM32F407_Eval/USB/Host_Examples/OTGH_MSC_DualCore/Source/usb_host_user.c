/*!
 * @file        usb_host_user.c
 *
 * @brief       usb host user configuration
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
#include "usb_host_user.h"

/* Private includes *******************************************************/
#include "usbh_msc.h"
#include "fatfs.h"
#include "board.h"
#include "board_delay.h"
#include "stdio.h"
#include <string.h>

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
USBH_INFO_T gUsbHostHS;
USBH_INFO_T gUsbHostFS;

USB_HOST_INFO_T hostInfoHS;
USB_HOST_INFO_T hostInfoFS;

uint8_t usbstate = 0x0;

FATFS hsfatfs;
FATFS fsfatfs;
FIL hsfile;
FIL fsfile;
FIL fileR;
DIR dir;
FILINFO fno;

/* Private function prototypes ********************************************/
static void USB_HSHostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus);
static void USB_FSHostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus);

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       USB host init
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostInit(void)
{
    /* USB host and class init */
    USBH_Init(&gUsbHostHS, USBH_SPEED_HS, &USBH_MSC_CLASS, USB_HSHostUserHandler);
    USBH_Init(&gUsbHostFS, USBH_SPEED_FS, &USBH_MSC_CLASS, USB_FSHostUserHandler);

    USBH_Start(&gUsbHostHS);
    USBH_Start(&gUsbHostFS);

    hsfatfs.pdrv = 0;
    fsfatfs.pdrv = 1;
}

/*!
 * @brief       USB host reset
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostReset(void)
{
    USBH_DeInit(&gUsbHostHS);
    USBH_DeInit(&gUsbHostFS);
}

/*!
 * @brief       USB host process
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostProcess(void)
{
    USBH_MSC_INFO_T* usbHSHostMSC = (USBH_MSC_INFO_T*)gUsbHostHS.activeClass->classData;
    USBH_MSC_INFO_T* usbFSHostMSC = (USBH_MSC_INFO_T*)gUsbHostFS.activeClass->classData;

    if(gUsbHostHS.hostState == USBH_HOST_DEVICE_ATTACHED)
    {
        usbstate = 0x1;
    }
    else if(gUsbHostHS.hostState == USBH_HOST_DEVICE_ATTACHED)
    {
        usbstate = 0x2;
    }

    if(usbstate == 0x1)
    {
        USBH_PollingProcess(&gUsbHostHS);
        if(usbHSHostMSC->state == USBH_MSC_IDLE)
        {
            usbstate = 0x0;
        }
    }
    else if(usbstate == 0x2)
    {
        USBH_PollingProcess(&gUsbHostFS);
        if(usbFSHostMSC->state == USBH_MSC_IDLE)
        {
            usbstate = 0x0;
        }
    }
    else
    {
        USBH_PollingProcess(&gUsbHostHS);
        USBH_PollingProcess(&gUsbHostFS);
    }
}

/**
 * @brief   User application
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostUserApplication(void)
{
    uint32_t timeoutHS;
    uint32_t timeoutFS;

    switch (hostInfoHS.state)
    {
        case USBH_APP_CONNECTION:
            /* To avoid the enumeration error */
            if (((gUsbHostHS.xferState == USBH_XFER_WAITING) && \
                ((gUsbHostHS.hostEnumState == USBH_ENUM_IDLE) || \
                 (gUsbHostHS.hostEnumState == USBH_ENUM_GET_DEV_DESC))) ||
                 ((gUsbHostHS.hostState == USBH_HOST_ABORT) && (gUsbHostHS.devInfo.rstCnt > USBH_DEVICE_RESET_MAX_CNT)))
                 {
                     timeoutHS = BOARD_ReadTick();

                    if (timeoutHS - hostInfoHS.timer > 1000)
                    {
                        USBH_USR_DBG("USB host enumeration timeout");
                        hostInfoHS.state = USBH_APP_ERROR;
                    }
                 }
            break;

        case USBH_APP_DISCONNECTION:
            break;

        case USBH_APP_READY:
            break;

        case USBH_APP_ERROR:
            USBH_USR_ERR("USB host error, retry to enumerate the device");
            hostInfoHS.state = USBH_APP_IDLE;

            USBH_DeInit(&gUsbHostHS);

            USBH_Init(&gUsbHostHS, USBH_SPEED_HS, &USBH_MSC_CLASS, USB_HSHostUserHandler);

            USBH_Start(&gUsbHostHS);
            break;

        default:
            break;
    }

    switch (hostInfoFS.state)
    {
        case USBH_APP_CONNECTION:
            /* To avoid the enumeration error */
            if (((gUsbHostFS.xferState == USBH_XFER_WAITING) && \
                ((gUsbHostFS.hostEnumState == USBH_ENUM_IDLE) || \
                 (gUsbHostFS.hostEnumState == USBH_ENUM_GET_DEV_DESC))) ||
                 ((gUsbHostFS.hostState == USBH_HOST_ABORT) && (gUsbHostFS.devInfo.rstCnt > USBH_DEVICE_RESET_MAX_CNT)))
                 {
                     timeoutFS = BOARD_ReadTick();

                    if (timeoutFS - hostInfoFS.timer > 1000)
                    {
                        USBH_USR_DBG("USB host enumeration timeout");
                        hostInfoFS.state = USBH_APP_ERROR;
                    }
                 }
            break;

        case USBH_APP_DISCONNECTION:
            break;

        case USBH_APP_READY:
            break;

        case USBH_APP_ERROR:
            USBH_USR_ERR("USB host error, retry to enumerate the device");
            hostInfoFS.state = USBH_APP_IDLE;

            USBH_DeInit(&gUsbHostFS);

            USBH_Init(&gUsbHostFS, USBH_SPEED_FS, &USBH_MSC_CLASS, USB_FSHostUserHandler);

            USBH_Start(&gUsbHostFS);
            break;

        default:
            break;
    }
}

#if 0
/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostUserApplication(void)
{
    uint8_t hsstatus = gUsbHSHostAppStatus;
    uint8_t fsstatus = gUsbFSHostAppStatus;
    static uint8_t hsuserAppState = USER_APP_INIT;
    static uint8_t fsuserAppState = USER_APP_INIT;

    switch (hsstatus)
    {
        case USBH_APP_CONNECTION:
            break;

        case USBH_APP_DISCONNECTION:
            hsuserAppState = USER_APP_INIT;
            break;

        case USBH_APP_READY:
            switch (hsuserAppState)
            {
                case USER_APP_INIT:
                    printf("\r\n***********OP Menu************\r\n");
                    printf("1 - Press KEY 1 to scan the files on the USB drive\r\n");
                    printf("2 - Press KEY 2 to write and read string into file on the USB drive\r\n");
                    printf("***********OP Menu************\r\n");

                    hsuserAppState = USER_APP_RUN;
                    break;

                case USER_APP_RUN:
                    if (!BOARD_PBGetState(BUTTON_KEY1))
                    {
                        BOARD_DelayMs(10);
                        if (!BOARD_PBGetState(BUTTON_KEY1))
                        {
                            if (gUsbHostHS.devInfo.connectedStatus == ENABLE)
                            {
                                printf("------ Write files to HS U disk ------ \r\n");

                                if (f_mount(&hsfatfs, "0:", 0) == FR_OK)
                                {
                                    FATFS_WriteFile(&hsfile, "0:Host_HS_MSC_Test.txt");
                                }
                                else
                                {
                                    printf("fatFs mount fail!\r\n");
                                }
                            }

                            while (!BOARD_PBGetState(BUTTON_KEY1));
                        }
                    }

                    if (!BOARD_PBGetState(BUTTON_KEY2))
                    {
                        BOARD_DelayMs(10);
                        if (!BOARD_PBGetState(BUTTON_KEY2))
                        {
                            if (gUsbHostHS.devInfo.connectedStatus == ENABLE)
                            {
                                printf("------ read HS U disk file ------\r\n");

                                if (f_mount(&hsfatfs, "0:", 0) == FR_OK)
                                {
                                    FATFS_ReadFile(&hsfile, "0:Host_HS_MSC_Test.txt");
                                }
                                else
                                {
                                    printf("fatFs mount fail!\r\n");
                                }
                            }

                            while (!BOARD_PBGetState(BUTTON_KEY2));
                        }
                    }
                    break;
            }
            break;

        default:
            break;
    }

    switch (fsstatus)
    {
        case USBH_APP_CONNECTION:
            break;

        case USBH_APP_DISCONNECTION:
            fsuserAppState = USER_APP_INIT;
            break;

        case USBH_APP_READY:
            switch (fsuserAppState)
            {
                case USER_APP_INIT:
                    printf("\r\n***********OP Menu************\r\n");
                    printf("1 - Press KEY 3 to scan the files on the USB drive\r\n");
                    printf("2 - Press KEY 4 to write and read string into file on the USB drive\r\n");
                    printf("***********OP Menu************\r\n");

                    fsuserAppState = USER_APP_RUN;
                    break;

                case USER_APP_RUN:
                    if (!BOARD_PBGetState(BUTTON_KEY3))
                    {
                        BOARD_DelayMs(10);
                        if (!BOARD_PBGetState(BUTTON_KEY3))
                        {
                            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
                            {
                                printf("------ Write files to FS U disk ------ \r\n");

                                if (f_mount(&fsfatfs, "1:", 0) == FR_OK)
                                {
                                    FATFS_WriteFile(&fsfile ,"1:Host_FS_MSC_Test.txt");
                                }
                                else
                                {
                                    printf("fatFs mount fail!\r\n");
                                }
                            }

                            while (!BOARD_PBGetState(BUTTON_KEY3));
                        }
                    }

                    if (!BOARD_PBGetState(BUTTON_KEY4))
                    {
                        BOARD_DelayMs(10);
                        if (!BOARD_PBGetState(BUTTON_KEY4))
                        {
                            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
                            {
                                printf("------ read FS U disk file ------\r\n");

                                if (f_mount(&fsfatfs, "1:", 0) == FR_OK)
                                {
                                    FATFS_ReadFile(&fsfile,"1:Host_FS_MSC_Test.txt");
                                }
                                else
                                {
                                    printf("fatFs mount fail!\r\n");
                                }
                            }

                            while (!BOARD_PBGetState(BUTTON_KEY4));
                        }
                    }
                    break;
            }
            break;

        default:
            break;
    }
}
#endif

/*!
 * @brief       USB host user handler
 *
 * @param       usbInfo
 *
 * @param       userStatus
 *
 * @retval      None
 */
static void USB_HSHostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus)
{
    switch (userStatus)
    {
        case USBH_USER_CONNECTION:
            hostInfoHS.state = USBH_APP_CONNECTION;
            hostInfoHS.timer = BOARD_ReadTick();
            break;

        case USBH_USER_DISCONNECTION:
            hostInfoHS.state = USBH_APP_DISCONNECTION;
            if (f_mount(NULL, "0:", 0) != FR_OK)
            {
                USBH_USR_LOG("Unmount FatFs fail");
            }
            break;

        case USBH_USER_DETECTED_SPEED:
            switch(usbInfo->devInfo.speed)
            {
                case USBH_PORT_SPEED_HS:
                    USBH_USR_LOG("USB device speed is HS");
                    break;

                case USBH_PORT_SPEED_FS:
                    USBH_USR_LOG("USB device speed is FS");
                    break;

                case USBH_PORT_SPEED_LS:
                    USBH_USR_LOG("USB device speed is LS");
                    if(usbInfo->hostSpeed == USBH_SPEED_HS)
                    {
                        USBH_USR_LOG("HS2:High speed host is not support for LS device.");
                        usbInfo->userCallback(usbInfo,USBH_USER_NOT_SUPPORT);
                    }
                    break;

                default:
                    USBH_USR_LOG("USB device speed is not support");
                    break;
            }
            break;

        case USBH_USER_ENUMERATION:
            USBH_USR_LOG("USB device enumration ok");
            break;

        case USBH_USER_CLASS_LAUNCHED:
            hostInfoHS.state = USBH_APP_READY;
            USBH_USR_LOG("Class is ready");
            if (gUsbHostHS.devInfo.connectedStatus == ENABLE)
            {
                USBH_USR_LOG("------ Write files to U disk ------");

                if (f_mount(&hsfatfs, "0:", 0) == FR_OK)
                {
                    FATFS_WriteFile(&hsfile);
                }
                else
                {
                    USBH_USR_LOG("FatFs mount fail!");
                }
                
                USBH_USR_LOG("------ Read U disk file ------");

                if (f_mount(&hsfatfs, "0:", 0) == FR_OK)
                {
                    FATFS_ReadFile(&hsfile);
                }
                else
                {
                    USBH_USR_LOG("FatFs mount fail!");
                }
            }
            break;

        case USBH_USER_NOT_SUPPORT:
            USBH_USR_LOG("USB device is Unsupported Device");
            break;

        case USBH_USER_ERROR:
            USBH_USR_DBG("User error");
            break;

        default:
            break;
    }
}

/*!
 * @brief       USB host user handler
 *
 * @param       usbInfo
 *
 * @param       userStatus
 *
 * @retval      None
 */
static void USB_FSHostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus)
{
    switch (userStatus)
    {
        case USBH_USER_CONNECTION:
            hostInfoFS.state = USBH_APP_CONNECTION;
            hostInfoFS.timer = BOARD_ReadTick();
            break;

        case USBH_USER_DISCONNECTION:
            hostInfoFS.state = USBH_APP_DISCONNECTION;
            if (f_mount(NULL, "1:", 0) != FR_OK)
            {
                USBH_USR_LOG("Unmount FatFs fail");
            }
            break;

        case USBH_USER_DETECTED_SPEED:
            switch(usbInfo->devInfo.speed)
            {
                case USBH_PORT_SPEED_HS:
                    USBH_USR_LOG("USB device speed is HS");
                    break;

                case USBH_PORT_SPEED_FS:
                    USBH_USR_LOG("USB device speed is FS");
                    break;

                case USBH_PORT_SPEED_LS:
                    USBH_USR_LOG("USB device speed is LS");
                    if(usbInfo->hostSpeed == USBH_SPEED_HS)
                    {
                        USBH_USR_LOG("HS2:High speed host is not support for LS device.");
                        usbInfo->userCallback(usbInfo,USBH_USER_NOT_SUPPORT);
                    }
                    break;

                default:
                    USBH_USR_LOG("USB device speed is not support");
                    break;
            }
            break;

        case USBH_USER_ENUMERATION:
            USBH_USR_LOG("USB device enumration ok");
            break;

        case USBH_USER_CLASS_LAUNCHED:
            hostInfoFS.state = USBH_APP_READY;
            USBH_USR_LOG("Class is ready");
            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
            {
                USBH_USR_LOG("------ Write files to U disk ------");

                if (f_mount(&fsfatfs, "1:", 0) == FR_OK)
                {
                    FATFS_WriteFile(&fsfile);
                }
                else
                {
                    USBH_USR_LOG("FatFs mount fail!");
                }
                
                USBH_USR_LOG("------ Read U disk file ------");

                if (f_mount(&fsfatfs, "0:", 0) == FR_OK)
                {
                    FATFS_ReadFile(&fsfile);
                }
                else
                {
                    USBH_USR_LOG("FatFs mount fail!");
                }
            }
            break;

        case USBH_USER_NOT_SUPPORT:
            USBH_USR_LOG("USB device is Unsupported Device");
            break;

        case USBH_USER_ERROR:
            USBH_USR_DBG("User error");
            break;

        default:
            break;
    }
}
