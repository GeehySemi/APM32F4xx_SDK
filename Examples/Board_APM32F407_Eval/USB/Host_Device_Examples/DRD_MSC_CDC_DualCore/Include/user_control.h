/*!
 * @file        user_control.h
 *
 * @brief       Control USB switch role file
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
#ifndef __USER_CONTROL_H
#define __USER_CONTROL_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

typedef enum
{
    USB_HOST,
    USB_DEVICE,
} USB_STA_T;

/* Exported function prototypes *******************************************/

extern USB_STA_T usbstatus;

void USB_Process(void);

#ifdef __cplusplus
}
#endif

#endif /*__USER_CONTROL_H */
