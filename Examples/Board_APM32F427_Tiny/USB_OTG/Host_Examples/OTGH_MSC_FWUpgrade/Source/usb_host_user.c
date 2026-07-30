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
USBH_INFO_T gUsbHostFS;

USB_HOST_INFO_T hostInfo;

/* Private function prototypes ********************************************/
static void USB_HostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus);

/* External variables *****************************************************/

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
__weak void USB_HostUserApplication(void)
{

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
            break;

        case USBH_USER_ERROR:
            USBH_USR_DBG("User error");
            break;

        default:
            break;
    }
}
