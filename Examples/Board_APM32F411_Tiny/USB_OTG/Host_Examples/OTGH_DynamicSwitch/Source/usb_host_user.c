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
#include "usbh_msc.h"
#include "usbh_hid.h"
#include "usbh_hid_keyboard.h"
#include "usbh_hid_mouse.h"
#include "usbh_cdc.h"
#include "board.h"
#include "board_delay.h"
#include "fatfs.h"
#include "stdio.h"
#include <string.h>

/* Private macro **********************************************************/
#define USBH_CDC_DATA_SIZE  1024

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
USBH_INFO_T gUsbHostFS;

USB_HOST_INFO_T hostInfo;

/* FatFS */
FATFS fatfs;
FIL file;
FIL fileR;
DIR dir;
FILINFO fno;
uint8_t cdcTxBuffer[USBH_CDC_DATA_SIZE];
uint8_t cdcRxBuffer[USBH_CDC_DATA_SIZE];

USBH_APP_STA_T appStatus;

/* Private function prototypes ********************************************/
static void USB_HostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus);

/* External variables *****************************************************/
extern FATFS* fs[FF_VOLUMES];
extern char fileScanPath[255];

/* External functions *****************************************************/

/**
 * @brief   USB host init
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostInitalize(void)
{
    /* USB host and class init */
    USBH_Init(&gUsbHostFS, USBH_SPEED_FS, &USBH_MSC_CLASS, USB_HostUserHandler);

    USBH_RegisterClass(&gUsbHostFS, &USBH_HID_CLASS);

    USBH_RegisterClass(&gUsbHostFS, &USBH_CDC_CLASS);

    USBH_Start(&gUsbHostFS);
}

/*!
 * @brief   USB host reset
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostReset(void)
{
    USBH_DeInit(&gUsbHostFS);
}

/*!
 * @brief   USB host process
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostProcess(void)
{
    USBH_PollingProcess(&gUsbHostFS);
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
    uint32_t timeout;

    switch (hostInfo.state)
    {
        case USBH_APP_CONNECTION:
            /* To avoid the enumeration error */
            if (((gUsbHostFS.xferState == USBH_XFER_WAITING) && \
                ((gUsbHostFS.hostEnumState == USBH_ENUM_IDLE) || \
                 (gUsbHostFS.hostEnumState == USBH_ENUM_GET_DEV_DESC))) ||
                 ((gUsbHostFS.hostState == USBH_HOST_ABORT) && (gUsbHostFS.devInfo.rstCnt > USBH_DEVICE_RESET_MAX_CNT)))
                 {
                     timeout = BOARD_ReadTick();

                    if (timeout - hostInfo.timer > 1000)
                    {
                        USBH_USR_DBG("USB host enumeration timeout");
                        hostInfo.state = USBH_APP_ERROR;
                    }
                 }
            break;

        case USBH_APP_DISCONNECTION:
            break;

        case USBH_APP_READY:
            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
            {
                switch (USBH_ReadActiveClass(&gUsbHostFS))
                {
                    case USBH_CLASS_HID:
                        appStatus = USBH_APP_HID;
                        break;

                    case USBH_CLASS_MSC:
                        appStatus = USBH_APP_MSC;
                        break;

                    case USBH_CLASS_CDCC:
                        appStatus = USBH_APP_CDC;
                        if (!BOARD_PBGetState(BUTTON_KEY1))
                        {
                            BOARD_DelayMs(10);
                            if (!BOARD_PBGetState(BUTTON_KEY1))
                            {
                                if (USBH_CDC_GetTxDataState(&gUsbHostFS) == USBH_CDC_DATA_TX_IDLE)
                                {
                                    USBH_USR_LOG("USB host send data");
                                    (void)memset(cdcTxBuffer, 0xAA, USBH_CDC_DATA_SIZE);
                                    USBH_CDC_SendData(&gUsbHostFS, cdcTxBuffer, USBH_CDC_DATA_SIZE);
                                }
                            }

                            while (!BOARD_PBGetState(BUTTON_KEY1));
                        }

                        if (!BOARD_PBGetState(BUTTON_KEY2))
                        {
                            BOARD_DelayMs(10);
                            if (!BOARD_PBGetState(BUTTON_KEY2))
                            {
                                if (USBH_CDC_GetRxDataState(&gUsbHostFS) == USBH_CDC_DATA_RX_IDLE)
                                {
                                    USBH_USR_LOG("USB host receive data");
                                    (void)memset(cdcRxBuffer, 0, USBH_CDC_DATA_SIZE);
                                    USBH_CDC_ReceiveData(&gUsbHostFS, cdcRxBuffer, USBH_CDC_DATA_SIZE);
                                }
                            }

                            while (!BOARD_PBGetState(BUTTON_KEY2));
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case USBH_APP_ERROR:
            USBH_USR_ERR("USB host error, retry to enumerate the device");
            hostInfo.state = USBH_APP_IDLE;

            USB_HostReset();

            USB_HostInitalize();
            break;

        default:
            break;
    }
}

/*!
 * @brief       USB host HID poll callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
void USBH_HID_PollCallback(USBH_INFO_T* usbInfo)
{
    USBH_HID_KEYBOARD_INFO_T* hidkbInfo;
    USBH_HID_MOUSE_INFO_T* hidmouseInfo;
    uint8_t c;

    if (hostInfo.state == USBH_APP_READY)
    {
        if (appStatus == USBH_APP_HID)
        {
            if (USBH_HID_ReadDevType(usbInfo) == HID_KEYBOARD)
            {
                hidkbInfo = USBH_HID_KeyboardCallback(usbInfo);
                if (hidkbInfo != NULL)
                {
                    c = USBH_HID_KeyboardValueToAsciiCode(hidkbInfo);
                    if (c != 0)
                    {
                        printf("%c", c);
                        fflush(stdout);
                    }
                }
            }
            else if (USBH_HID_ReadDevType(usbInfo) == HID_MOUSE)
            {
                hidmouseInfo = USBH_HID_MouseCallback(usbInfo);
                if (hidmouseInfo != NULL)
                {
                    USBH_USR_LOG("Mouse x:%d, y:%d, z:%d, button1:%d, button2:%d, button3:%d",
                                 hidmouseInfo->x, hidmouseInfo->y, hidmouseInfo->z,
                                 hidmouseInfo->button[0], hidmouseInfo->button[1], hidmouseInfo->button[2]);
                }
            }
        }
    }
}

/*!
 * @brief       USB host CDC send data finish callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
void USBH_CDC_XferEndCallback(USBH_INFO_T* usbInfo)
{
    UNUSED(usbInfo);
    USBH_USR_LOG("USB host send data finish");
}

/*!
 * @brief       USB host CDC receive data finish callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
void USBH_CDC_RevEndCallback(USBH_INFO_T* usbInfo)
{
    uint16_t size;
    uint32_t i;
    uint8_t *buffer = cdcRxBuffer;

    size = USBH_CDC_ReadRevDataSize(usbInfo);
    USBH_USR_LOG("USB host receive data finish, size = %d", size);

    for (i = 0; i < size; i++)
    {
        printf("0x%02X ", *buffer++);
    }

    printf("\r\n");
}

/**
 * @brief   USB host user handler
 *
 * @param   usbInfo: USB core information
 *
 * @param   userStatus: user status
 *
 * @retval  None
 */
static void USB_HostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus)
{
    switch (userStatus)
    {
        case USBH_USER_CONNECTION:
            hostInfo.state = USBH_APP_CONNECTION;
            hostInfo.timer = BOARD_ReadTick();
            break;

        case USBH_USER_DISCONNECTION:
            hostInfo.state = USBH_APP_DISCONNECTION;
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
            hostInfo.state = USBH_APP_READY;
            USBH_USR_LOG("Class is ready");

            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
            {
                switch (USBH_ReadActiveClass(&gUsbHostFS))
                {
                    case USBH_CLASS_HID:
                        break;

                    case USBH_CLASS_MSC:
                        USBH_USR_LOG("------ Write files to U disk ------");

                        if (f_mount(&fatfs, "0:", 0) == FR_OK)
                        {
                            FATFS_WriteFile(&file);
                        }
                        else
                        {
                            USBH_USR_LOG("FatFs mount fail!");
                        }
                        
                        USBH_USR_LOG("------ Read U disk file ------");

                        if (f_mount(&fatfs, "0:", 0) == FR_OK)
                        {
                            FATFS_ReadFile(&file);
                        }
                        else
                        {
                            USBH_USR_LOG("FatFs mount fail!");
                        }
                        break;

                    case USBH_CLASS_CDCC:
                        break;

                    default:
                        break;
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
