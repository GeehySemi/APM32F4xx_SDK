/*!
 * @file       ethernetif.h
 *
 * @brief      Header for ethernetif.c module
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
#ifndef __ETHERNETIF_H
#define __ETHERNETIF_H

/* Includes ***************************************************************/
#include "lwip/err.h"
#include "lwip/netif.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/* Exported variables *****************************************************/

/* Exported function prototypes *******************************************/
err_t ethernetif_init(struct netif* netif);
err_t ethernetif_input(struct netif *netif);

#endif /** __ETHERNETIF_H */
