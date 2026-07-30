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

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define MESSAGE0    "****************************************"
#define MESSAGE1    "* APM32F40x DMC SDRAM Example "
#define MESSAGE2    "* AHB Clock       :"
#define MESSAGE3    "* SDRAM Clock Psc :"
#define MESSAGE4    "* SDRAM Clock     :"

/** printf function configs to USART2 */
#define DEBUG_USART  USART1

/** SDRAM GPIO */
#define  RCM_SDRAM_GPIO_PERIPH (RCM_AHB1_PERIPH_GPIOA | \
                                RCM_AHB1_PERIPH_GPIOD | \
                                RCM_AHB1_PERIPH_GPIOF | \
                                RCM_AHB1_PERIPH_GPIOG | \
                                RCM_AHB1_PERIPH_GPIOH | \
                                RCM_AHB1_PERIPH_GPIOI )

#define RCM_AHB3_PERIPH_EMMC    (0x00000001)

/** SDRAM Address */
#define SDRAM_START_ADDR ((uint32_t)0x60000000)
#define SDRAM_END_ADDR   ((uint32_t)0x60200000)
#define TEST_LENGTH      ((uint32_t)0x00001000)


/* Private typedef ********************************************************/

/* Private variables ******************************************************/

volatile uint32_t tick = 0;

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

void SDRAM_Init(void);
void SDRAM_GPIOConfig(void);

uint32_t DMC_SDRAM_WriteRead_8Bit(void);
uint32_t DMC_SDRAM_WriteRead_16Bit(void);
uint32_t DMC_SDRAM_WriteRead_32Bit(void);

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
    USART_Config_T usartConfig;
    uint32_t sdramClock,sdramDiv;


    /** Configures COM port. */
    usartConfig.baudRate = 115200;
    usartConfig.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfig.mode = USART_MODE_TX;
    usartConfig.parity = USART_PARITY_NONE;
    usartConfig.stopBits = USART_STOP_BIT_1;
    usartConfig.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfig);

    /** SDRAM Clock: HCLK / SDRAMPSC */
    RCM_ConfigSDRAM(RCM_SDRAM_DIV_4);
    sdramDiv = ((1 << RCM->CFG_B.SDRAMPSC) > 2) ? 4 : (1 << RCM->CFG_B.SDRAMPSC);
    sdramClock = (RCM_ReadHCLKFreq() / sdramDiv) / 1000000;

    /** printf message */
    printf("\r\n");
    printf("%s \r\n", MESSAGE0);
    printf("%s \r\n", MESSAGE1);
    printf("%s %ld MHz\r\n", MESSAGE2, RCM_ReadHCLKFreq() / 1000000);
    printf("%s %ld div\r\n", MESSAGE3, sdramDiv);
    printf("%s %ld MHz\r\n", MESSAGE4, sdramClock);

    SDRAM_GPIOConfig();
    SDRAM_Init();
 
    /** SDRAM bit test */
    DMC_SDRAM_8BitTest();
    DMC_SDRAM_16BitTest();
    DMC_SDRAM_32BitTest();
    
    printf("%s \r\n", MESSAGE0);
    while (1)
    {
    }
}

/*!
 * @brief       SDRAM GPIO Config
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SDRAM_GPIOConfig(void)
{
    GPIO_Config_T gpioConfig;

    RCM_EnableAHB1PeriphClock(RCM_SDRAM_GPIO_PERIPH);

    /* SDRAM pin configuration 
       A0   ----> PF1
       A1   ----> PF2
       A2   ----> PF3
       A3   ----> PF4
       A4   ----> PF6
       A5   ----> PF7
       A6   ----> PF8
       A7   ----> PF9
       A8   ----> PF10
       A9   ----> PH3
       A10  ----> PF0
       D0   ----> PG3
       D1   ----> PG4
       D2   ----> PG5
       D3   ----> PG6
       D4   ----> PG8
       D5   ----> PH13
       D6   ----> PH15
       D7   ----> PI3
       D8   ----> PH8
       D9   ----> PH10
       D10  ----> PD10
       D11  ----> PD12
       D12  ----> PD13
       D13  ----> PD14
       D14  ----> PD15
       D15  ----> PG2
       BA   ----> PI11
       CKE  ----> PA3
       CLK  ----> PG1
       LDQM ----> PG15
       UNQM ----> PF11
       NWE  ----> PI7
       NCAS ----> PI8
       NRAS ----> PI9
       NCS  ----> PI10
    */
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.otype = GPIO_OTYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_NOPULL;

    gpioConfig.pin = GPIO_PIN_10 | GPIO_PIN_12 |
                     GPIO_PIN_13 | GPIO_PIN_14 |
                     GPIO_PIN_15;
    GPIO_Config(GPIOD, &gpioConfig);
    
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_10, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_12, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_13, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_14, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOD, GPIO_PIN_SOURCE_15, GPIO_AF_DMC);

    gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1 |
                     GPIO_PIN_2 | GPIO_PIN_3 |
                     GPIO_PIN_4 | GPIO_PIN_6 |
                     GPIO_PIN_7 | GPIO_PIN_8 |
                     GPIO_PIN_9 | GPIO_PIN_10 |
                     GPIO_PIN_11;
    GPIO_Config(GPIOF, &gpioConfig);
    
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_0, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_1, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_2, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_3, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_4, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_6, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_7, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_8, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_9, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_10, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOF, GPIO_PIN_SOURCE_11, GPIO_AF_DMC);

    gpioConfig.pin = GPIO_PIN_1 | GPIO_PIN_2 |
                     GPIO_PIN_3 | GPIO_PIN_4 |
                     GPIO_PIN_5 | GPIO_PIN_6 |
                     GPIO_PIN_8 | GPIO_PIN_15;
    GPIO_Config(GPIOG, &gpioConfig);
    
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_1, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_2, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_3, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_4, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_5, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_6, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_8, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOG, GPIO_PIN_SOURCE_15, GPIO_AF_DMC);

    gpioConfig.pin = GPIO_PIN_3 | GPIO_PIN_8 |
                     GPIO_PIN_10 | GPIO_PIN_13 |
                     GPIO_PIN_15;
    GPIO_Config(GPIOH, &gpioConfig);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_DMC);
    
    gpioConfig.pin = GPIO_PIN_3;
    GPIO_Config(GPIOA, &gpioConfig);
    
    GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_3, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_5, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_8, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_10, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_13, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_15, GPIO_AF_DMC);

    gpioConfig.pin = GPIO_PIN_3 | GPIO_PIN_7 |
                     GPIO_PIN_8 | GPIO_PIN_9 |
                     GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_Config(GPIOI, &gpioConfig);
    
    GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_3, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_7, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_8, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_9, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_10, GPIO_AF_DMC);
    GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_11, GPIO_AF_DMC);
}

void SDRAM_Init(void)
{
    uint32_t sdramCapacity;
    DMC_Config_T dmcConfig;
    DMC_TimingConfig_T timingConfig;

    RCM_EnableAHB3PeriphClock(RCM_AHB3_PERIPH_EMMC);

    timingConfig.latencyCAS = DMC_CAS_LATENCY_3;        //!< Configure CAS latency period
    timingConfig.tARP       = DMC_AUTO_REFRESH_10;      //!< Configure auto refresh period
    timingConfig.tRAS       = DMC_RAS_MINIMUM_2;        //!< Configure line activation and precharging minimum time
    timingConfig.tCMD       = DMC_ATA_CMD_1;            //!< Configure active to active period
    timingConfig.tRCD       = DMC_DELAY_TIME_1;         //!< Configure RAS To CAS delay Time
    timingConfig.tRP        = DMC_PRECHARGE_1;          //!< Configure precharge period
    timingConfig.tWR        = DMC_NEXT_PRECHARGE_2;     //!< Configure time between the Last Data and The Next Precharge for write
    timingConfig.tXSR       = 3;                        //!< Configure XSR0
    timingConfig.tRFP       = 0x2F9;                    //!< Configure refresh Cycle

    dmcConfig.bankWidth     = DMC_BANK_WIDTH_1;         //!< Configure bank address width
    dmcConfig.clkPhase      = DMC_CLK_PHASE_REVERSE;    //!< Configure clock phase
    dmcConfig.rowWidth      = DMC_ROW_WIDTH_11;         //!< Configure row address width
    dmcConfig.colWidth      = DMC_COL_WIDTH_8;          //!< Configure column address width
    dmcConfig.timing        = timingConfig;

    DMC_Config(&dmcConfig);
    DMC_ConfigOpenBank(DMC_BANK_NUMBER_2);
    DMC_EnableAccelerateModule();

    DMC_Enable();

    /** sdramCapacity = row * col * 16bit * bank (16bit == 2Bytes) */
    sdramCapacity = (1 << (dmcConfig.rowWidth + 1)) * (1 << (dmcConfig.colWidth + 1)) * 2 * (1 << (dmcConfig.bankWidth + 1));
    
    printf("* Row Address Width    :%d bit\r\n", dmcConfig.rowWidth + 1);
    printf("* Column Address Width :%d bit\r\n", dmcConfig.colWidth + 1);
    printf("* Bank Address Width   :%d bit band addr\r\n", dmcConfig.bankWidth + 1);
    printf("* capacity             :%ld MByte\r\n", sdramCapacity / 1024 / 1024);
}

/*!
 * @brief       SDRAM 8bit Test
 *
 * @param       None
 *
 * @retval      None
 *
 */
void DMC_SDRAM_8BitTest(void)
{
    uint32_t WriteReadStatus=0;

    printf("%s \r\n", MESSAGE0);
    printf("    Test 8 bits read and write operation on the 2MB bytes space of external SDRAM\r\n");

    WriteReadStatus = DMC_SDRAM_WriteRead_8Bit();
    
    if (WriteReadStatus != 0)
    {
        printf("    Check failed, error address:0x%lX\r\n", WriteReadStatus);
    }
    else
    {
        printf("    Check passed\r\n");
    }
}


/*!
 * @brief       SDRAM 16bit Test
 *
 * @param       None
 *
 * @retval      None
 *
 */
void DMC_SDRAM_16BitTest(void)
{
    uint32_t WriteReadStatus=0;

    printf("%s \r\n", MESSAGE0);
    printf("    Test 16 bits read and write operation on the 2MB bytes space of external SDRAM\r\n");

    WriteReadStatus = DMC_SDRAM_WriteRead_16Bit();
    
    if (WriteReadStatus != 0)
    {
        printf("    Check failed, error address:0x%lX\r\n", WriteReadStatus);
    }
    else
    {
        printf("    Check passed\r\n");
    }
}

/*!
 * @brief       SDRAM 32bit Test
 *
 * @param       None
 *
 * @retval      None
 *
 */
void DMC_SDRAM_32BitTest(void)
{
    uint32_t WriteReadStatus=0;

    printf("%s \r\n", MESSAGE0);
    printf("    Test 32 bits read and write operation on the 2MB bytes space of external SDRAM\r\n");

    WriteReadStatus = DMC_SDRAM_WriteRead_32Bit();
    
    if (WriteReadStatus != 0)
    {
        printf("    Check failed, error address:0x%lX\r\n", WriteReadStatus);
    }
    else
    {
        printf("    Check passed\r\n");
    }
}

/*!
 * @brief       SDRAM 8bit Write Read
 *
 * @param       None
 *
 * @retval      None
 */
uint32_t DMC_SDRAM_WriteRead_8Bit(void)
{
    uint32_t test_end_addr = TEST_LENGTH;
    uint32_t Address = 0x00;
    uint32_t i = 0;

    /** Fill 0xFF */
    Address = 0x60000000;
    for (i = 0; Address < test_end_addr; i++, Address++)
    {
        *( uint8_t*) (Address) = 0xFF;
    }

    /** Write value to address */
    Address = SDRAM_START_ADDR;
    for (i = 0; Address < test_end_addr; i++, Address++)
    {
        *( uint8_t*) (Address) = (uint8_t)(0xAA + i);
    }

    /** Check */
    Address = SDRAM_START_ADDR;
    for (i = 0; Address < test_end_addr; i++, Address++)
    {
        if (*(uint8_t*) (Address) != (uint8_t)(0xAA + i))
        {
            return Address;
        }
    }

    return 0;
}

/*!
 * @brief       SDRAM 16bit Write Read
 *
 * @param       None
 *
 * @retval      None
 */
uint32_t DMC_SDRAM_WriteRead_16Bit(void)
{
    uint32_t test_end_addr = TEST_LENGTH;
    uint32_t Address = 0x60000000;
    uint32_t i = 0;

    /** Fill 0xFF */
    for (i = 0; Address < test_end_addr; i++, Address+=2)
    {
        *( uint16_t*) (Address) = 0xFFFF;
    }

    /** Write value to address */
    Address = SDRAM_START_ADDR;
    for (i = 0; Address < test_end_addr; i++, Address+=2)
    {
        *( uint16_t*) (Address) = (uint16_t)(0xAA55 + i);
    }

    /** Check */
    Address = SDRAM_START_ADDR;
    for (i = 0; Address < test_end_addr; i++, Address+=2)
    {
        if (*(uint16_t*) (Address) != (uint16_t)(0xAA55 + i))
        {
            return Address;
        }
    }

    return 0;
}

/*!
 * @brief       SDRAM 32bit Write Read
 *
 * @param       None
 *
 * @retval      None
 */
uint32_t DMC_SDRAM_WriteRead_32Bit(void)
{
    uint32_t test_end_addr = TEST_LENGTH;
    uint32_t Address = 0x60000000;
    uint32_t i = 0;

    /** Fill 0xFF */
    for (i = 0; Address < test_end_addr; i++, Address+=4)
    {
        *( uint32_t*) (Address) = 0xFFFFFFFF;
    }

    /** Write value to address */
    Address = SDRAM_START_ADDR;
    for (i = 0; i < test_end_addr; i++, Address+=4)
    {
        *( uint32_t*) (Address) = (uint32_t)(0xAA55AA55 + i);
    }

    /** Check */
    Address = SDRAM_START_ADDR;
    for (i = 0; i < test_end_addr; i++, Address+=4)
    {
        if (*(uint32_t*) (Address) != (uint32_t)(0xAA55AA55 + i))
        {
            return Address;
        }
    }

    return 0;
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 */
void Delay(void)
{
    tick = 0;

    while (tick < 500);
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
