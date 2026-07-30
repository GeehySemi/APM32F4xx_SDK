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
#include "data.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

#define SDRAM_ADDRESS  0x60000000

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

/* Private typedef ********************************************************/

typedef  void (*pFunction)(void);

/* Private variables ******************************************************/

volatile uint32_t tick = 0;

__IO uint32_t JumpAddress;
volatile pFunction Jump_To_Application;

/* Private function prototypes ********************************************/

#define Jump_to_App()         \
    do {                       \
        JumpAddress = *(__IO uint32_t *)(SDRAM_ADDRESS + 4);  \
        Jump_To_Application = (pFunction)JumpAddress;             \
        __set_MSP(*(__IO uint32_t *)SDRAM_ADDRESS);            \
        Jump_To_Application();                                  \
    } while(0)

void SDRAM_Init(void);
void SDRAM_GPIOConfig(void);

uint32_t Memory_WriteRead_32Bit(uint32_t start_addr, uint32_t *buf, uint32_t length);

/* External variables *****************************************************/

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
    USART_Config_T usartConfig;

    /** Configures COM port. */
    usartConfig.baudRate = 115200;
    usartConfig.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfig.mode = USART_MODE_TX;
    usartConfig.parity = USART_PARITY_NONE;
    usartConfig.stopBits = USART_STOP_BIT_1;
    usartConfig.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfig);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /** SDRAM Clock: HCLK / SDRAMPSC */
    RCM_ConfigSDRAM(RCM_SDRAM_DIV_4);

    SDRAM_GPIOConfig();
    SDRAM_Init();

    printf("This is an example running on the APM32F407 that executes a program on external SDRAM.\r\n\r\n");
    printf("Press the KEY1 button to write data into SDRAM.\r\n\r\n");

    while (1)
    {
        if (BOARD_PBGetState(BUTTON_KEY1) == RESET)
        {
            if (Memory_WriteRead_32Bit(SDRAM_START_ADDR, data, sizeof(data)) == 0)
            {
                printf("Write successful, now jumping to the program running in SDRAM! \r\n\r\n");
                Jump_to_App();
            }
            else
            {
                printf("Write failed, please check whether the wiring is correct or if the module is damaged! \r\n\r\n");
            }
        }
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

/*!
 * @brief       SDRAM Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SDRAM_Init(void)
{
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
    DMC_ConfigOpenBank(DMC_BANK_NUMBER_1);
    DMC_ConfigOpenBank(DMC_BANK_NUMBER_2);
    DMC_ConfigOpenBank(DMC_BANK_NUMBER_3);
    DMC_ConfigOpenBank(DMC_BANK_NUMBER_4);
    DMC_EnableAccelerateModule();

    DMC_Enable();
}

/*!
 * @brief Writes and verifies a block of 32-bit data in memory.
 *
 * This function performs three sequential operations on a block of memory starting from
 * the provided start_addr. First, it initializes the memory block by writing 0xFFFFFFFF
 * to each 32-bit word for the specified length. Next, it writes the data from the provided
 * buffer (buf) into the same memory block. Finally, it reads back the memory content and
 * compares it with the original buffer data. If a mismatch is detected at any address,
 * the function returns that specific address. If all data is successfully verified, it returns 0.
 *
 * @param   start_addr   The starting (byte) address of the memory block to be written and verified.
 * @param   buf          A pointer to an array containing the 32-bit data to be written into memory.
 * @param   length       The number of 32-bit words to write and verify.
 *
 * @retval  0            Data verification successful; all memory content matches the buffer.
 * @retval  non-zero     The address where the data verification failed.
 */

uint32_t Memory_WriteRead_32Bit(uint32_t start_addr, uint32_t *buf, uint32_t length)
{
    uint32_t i;
    uint32_t addr;

    for (i = 0, addr = start_addr; i < length; i++, addr += 4)
    {
        *(volatile uint32_t *)addr = 0xFFFFFFFF;
    }

    for (i = 0, addr = start_addr; i < length; i++, addr += 4)
    {
        *(volatile uint32_t *)addr = buf[i];
    }

    for (i = 0, addr = start_addr; i < length; i++, addr += 4)
    {
        if (*(volatile uint32_t *)addr != buf[i])
        {
            return addr;
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
