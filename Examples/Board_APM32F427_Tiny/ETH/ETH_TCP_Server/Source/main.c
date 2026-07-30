/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "main.h"
#include "Board.h"
#include "NETConfig.h"
#include "echo_s.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define DEBUG_USART  USART1

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/** current Ethernet LocalTime value */
volatile uint32_t ETHTimer = 0;
/** lwip network interface structure for ethernetif */
struct netif UserNetif;

/** TCP periodic Timer */
uint32_t TCPTimer = 0;
/** ARP periodic Timer */
uint32_t ARPTimer = 0;
/** Link periodic Timer */
uint32_t LinkTimer = 0;
/** MAC address */
uint8_t SetMACaddr[6] = {0,0,0,0,0,8};

/* Private function prototypes ********************************************/

/** Configure SysTick */
void ConfigSysTick(void);
void EthResetDelay(__IO uint32_t count);

/* External variables *****************************************************/

extern ETH_DMADescConfig_T  *DMARxDescToGet;

/* External functions *****************************************************/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    SystemClockConfig();
    uint8_t flag = 0;
    char DisplayBuf[100] = {0};

    USART_Config_T usartConfig;

    /** Configure SysTick */
    ConfigSysTick();

    /** Configure USART */
    usartConfig.baudRate = 115200;
    usartConfig.wordLength = USART_WORD_LEN_8B;
    usartConfig.stopBits = USART_STOP_BIT_1;
    usartConfig.parity = USART_PARITY_NONE ;
    usartConfig.mode = USART_MODE_TX_RX;
    usartConfig.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    BOARD_COMInit(COM1,&usartConfig);

    /** Configures LED2 and LED3 */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /** KEY init*/
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);
    BOARD_PBInit(BUTTON_KEY3, BUTTON_MODE_GPIO);

    printf("\nThis is a ETH TCP Server Demo!\n");

    /** Ethernet reset delay */
    EthResetDelay(10000);

    /** Configure ethernet (GPIOs, clocks, MAC, DMA) */
    ConfigEthernet();

    /** Initilaize the LwIP stack */
    LwIP_Init();

    /** Use Com printf static IP address*/
    sprintf(DisplayBuf,"EVAL board Static IP address \n");
    printf("%s",DisplayBuf);

    sprintf(DisplayBuf,"IP: %d.%d.%d.%d\n",
    IP_ADDR0,
    IP_ADDR1,
    IP_ADDR2,
    IP_ADDR3);
    printf("%s",DisplayBuf);

    sprintf(DisplayBuf,"NETMASK: %d.%d.%d.%d\n",
    NETMASK_ADDR0,
    NETMASK_ADDR1,
    NETMASK_ADDR2,
    NETMASK_ADDR3);
    printf("%s",DisplayBuf);

    sprintf(DisplayBuf,"Gateway: %d.%d.%d.%d\n",
    GW_ADDR0,
    GW_ADDR1,
    GW_ADDR2,
    GW_ADDR3);
    printf("%s",DisplayBuf);

    printf("\n\rKEY2: create TCP server\n");
    printf("KEY3: close TCP server\n");

    while(1)
    {

        if ((BOARD_PBGetState(BUTTON_KEY2)==0)&&(flag==0))
        {
            BOARD_LEDOn(LED2);
            if (EthLinkStatus == 0)
            {
                /** create tcp server */
                printf("\n\rcreate TCP server IP: %d.%d.%d.%d:%d\n",
                       IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3,LOCAL_SERVER_PORT);
                tcps_echo_init(LOCAL_SERVER_PORT);
                flag=1;
            }
        }
        if ((BOARD_PBGetState(BUTTON_KEY3)==0)&&(flag==1))
        {
            BOARD_LEDOff(LED2);
            printf("\n\rclose TCP server\n");
            tcps_echo_disable();
            flag=0;
        }

        /** check if any packet received */
        if (ETH_CheckReceivedFrame())
        {
            /** process received ethernet packet */
            LwIP_Pkt_Handle();
        }
            /** handle periodic timers for LwIP */
        LwIP_Periodic_Handle(ETHTimer);
    }
}


/*!
 * @brief       Ethernet reset delay
 *
 * @param       count: delay count
 * 
 * @retval      None
 */
void EthResetDelay(__IO uint32_t count)
{
    __IO uint32_t i = 0;
    for(i = count; i!= 0; i--)
    {
    }
}

 /*!
 * @brief       Configure SysTick
 *
 * @param       None
 *
 * @retval      None
 */
 void ConfigSysTick(void)
{
    if(SysTick_Config(SystemCoreClock/100))
    {
        /** Capture error */
        while (1);
    }
}

/*!
 * @brief       This function initializes the lwIP stack
 *
 * @param       None
 *
 * @retval      None
 */
void LwIP_Init(void)
{
    struct ip4_addr ipaddr;
    struct ip4_addr netmask;
    struct ip4_addr gw;

    /** Initializes the dynamic memory heap */
    mem_init();

    /** Initializes the memory pools */
    memp_init();

    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    /** Config MAC Address */
    ETH_ConfigMACAddress(ETH_MAC_ADDRESS0, SetMACaddr);

    /** Add a network interface to the list of lwIP netifs */
    netif_add(&UserNetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /** Registers the default network interface */
    netif_set_default(&UserNetif);

    /** When the netif is fully configured this function must be called */
    netif_set_up(&UserNetif);
}

/*!
 * @brief       This function received ethernet packet
 *
 * @param       None
 *
 * @retval      None
 */
void LwIP_Pkt_Handle(void)
{
    /** Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    ethernetif_input(&UserNetif);
}

/*!
 * @brief       This function LwIP periodic tasks
 *
 * @param       ETHTimer the current Ethernet Timer value
 *
 * @retval      None
 */
void LwIP_Periodic_Handle(__IO uint32_t ETHTimer)
{
    static uint8_t flagToggle = 0;

#if LWIP_TCP
    /** TCP periodic process every 250 ms */
    if (ETHTimer - TCPTimer >= TCP_TMR_INTERVAL)
    {
        TCPTimer =  ETHTimer;
        tcp_tmr();
    }
#endif

    /** ARP periodic process every 5s */
    if ((ETHTimer - ARPTimer) >= ARP_TMR_INTERVAL)
    {
        ARPTimer =  ETHTimer;
        etharp_tmr();
    }

    /** Check link status */
    if ((ETHTimer - LinkTimer) >= 1000)
    {
        if ((ETH_GET_LINK_STATUS != 0) && (flagToggle == 0))
        {
            /** link goes up */
            netif_set_link_up(&UserNetif);
            flagToggle = 1;
        }

        if ((ETH_GET_LINK_STATUS == 0) && (flagToggle == 1))
        {
            EthLinkStatus = 1;
            /** link goes down */
            netif_set_link_down(&UserNetif);
            flagToggle = 0;
        }
    }
}

/*!
 * @brief       User config the different system Clock
 *
 * @param       None
 *
 * @retval      None
 */
void UserRCMClockConfig(void)
{
    RCM_Reset();
    RCM_ConfigHSE(RCM_HSE_OPEN);

    if(RCM_WaitHSEReady() == SUCCESS)
    {
        RCM_ConfigPLL1(RCM_PLLSEL_HSE,8,200,RCM_PLL_SYS_DIV_2,5);
    }
    RCM_EnablePLL1();

    while(RCM_ReadStatusFlag(RCM_FLAG_PLL1RDY) == RESET)
    {
        ;
    }

    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);

    while(RCM_ReadSYSCLKSource()!= RCM_SYSCLK_SEL_PLL)
    {
        ;
    }
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;

    UNUSED(file);

    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

