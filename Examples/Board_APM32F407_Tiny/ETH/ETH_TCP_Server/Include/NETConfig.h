/*!
 * @file       NETConfig.h
 *
 * @brief      This file provides all the config DP83848 functions
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
#ifndef __NET_CONFIG_H
#define __NET_CONFIG_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "apm32f4xx_eth.h"
#include "netif.h"

/* Exported macro *********************************************************/
/** ETH_PHY_ADDRESS */
#define ETH_PHY_ADDRESS       LAN8720A_PHY_ADDRESS

#define COMP_IP_ADDR0               192
#define COMP_IP_ADDR1               168
#define COMP_IP_ADDR2               1
#define COMP_IP_ADDR3               158
#define COMP_PORT                   6000


#define LOCAL_SERVER_PORT     5000

/** Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0                    192
#define IP_ADDR1                    168
#define IP_ADDR2                    1  //1
#define IP_ADDR3                    22

/** NETMASK */
#define NETMASK_ADDR0               255
#define NETMASK_ADDR1               255
#define NETMASK_ADDR2               255
#define NETMASK_ADDR3                 0

/** Gateway Address */
#define GW_ADDR0                    192
#define GW_ADDR1                    168
#define GW_ADDR2                    1  //1
#define GW_ADDR3                    1

#define ETH_GET_LINK_STATUS         (ETH_ReadPHYRegister(ETH_PHY_ADDRESS, PHY_BSR) & 0x00000004)

/* Exported typedef *******************************************************/

/* Exported variables *****************************************************/

/* Exported function prototypes *******************************************/

#ifdef __cplusplus
}
#endif

#endif /** __NET_CONFIG_H */
