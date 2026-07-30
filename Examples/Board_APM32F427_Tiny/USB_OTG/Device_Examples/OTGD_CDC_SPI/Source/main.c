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

/* Private includes *******************************************************/
#include "board_delay.h"
#include "log_printf.h"
#include "usb_device_user.h"
#include "usbd_cdc.h"
#include "usbd_cdc_vcp.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_spi.h"
#include "apm32f4xx_usart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Private macro **********************************************************/
#define DEBUG_USART    USART1
#define SPI_FRAME_MAX_LEN               32U
#define CDC_REPORT_MAX_LEN              512U
#define SPI_TRANSFER_TIMEOUT_MS         100U
#define SPI_ERROR_TIMEOUT_FLAG          0xFFFEU

/* Private typedef ********************************************************/
/**
 * @brief User application state
 */
typedef enum
{
    USER_APP_INIT,
    USER_APP_RUN,
} USER_APP_STATE_T;

/* Private variables ******************************************************/
static const char* tag = "main";
static volatile uint8_t spiTransferBusy;
static volatile uint8_t spiMasterTxIndex;
static volatile uint8_t spiMasterRxIndex;
static volatile uint8_t spiSlaveTxIndex;
static volatile uint8_t spiSlaveRxIndex;
static volatile uint8_t spiMasterTxDone;
static volatile uint8_t spiMasterRxDone;
static volatile uint8_t spiSlaveTxDone;
static volatile uint8_t spiSlaveRxDone;
static volatile uint8_t spiTransferDone;
static volatile uint8_t spiErrorPending;
static volatile uint16_t spiErrorFlag;
static volatile uint32_t spiTransferStartTick;

static uint8_t spiTransferLen;
static uint8_t spiMasterTxBuffer[SPI_FRAME_MAX_LEN];
static uint8_t spiMasterRxBuffer[SPI_FRAME_MAX_LEN];
static uint8_t spiSlaveTxBuffer[SPI_FRAME_MAX_LEN];
static uint8_t spiSlaveRxBuffer[SPI_FRAME_MAX_LEN];

static uint8_t cdcReportBuffer[CDC_REPORT_MAX_LEN];
static uint8_t cdcSendBuffer[CDC_REPORT_MAX_LEN];
static uint16_t cdcReportLen;
static uint8_t cdcReportPending;

/* Private function prototypes ********************************************/
static void SPI_InterruptBridgeConfig(void);
static void APP_QueueTextReport(const char *text);
static uint8_t APP_NormalizeRxLength(uint32_t rawLength);
static void APP_PrepareSlaveTx(uint8_t length);
static void APP_StartSpiTransfer(uint8_t length);
static uint16_t APP_AppendHexLine(char *buffer, uint16_t offset, const char *title, uint8_t *data, uint8_t length);
static void APP_BuildSpiReport(void);
static void APP_BuildSpiErrorReport(void);
static void APP_CheckSpiTransferDone(void);
static uint8_t APP_IsUsbCdcTxIdle(void);
static void APP_ServicePendingReport(void);
static void APP_AbortSpiTransfer(uint16_t errorFlag);
void APP_SPI_IRQHandler(SPI_T *spi);

/* External variables *****************************************************/
extern USBD_INFO_T gUsbDeviceFS;

extern USBD_CDC_VCP_INFO_T gUsbVCP;

extern uint8_t cdcRxBuffer[USBD_CDC_RX_BUF_LEN];

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
    USART_Config_T usartConfigStruct;

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate      = 115200;
    usartConfigStruct.mode          = USART_MODE_TX_RX;
    usartConfigStruct.parity        = USART_PARITY_NONE;
    usartConfigStruct.stopBits      = USART_STOP_BIT_1;
    usartConfigStruct.wordLength    = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow  = USART_HARDWARE_FLOW_NONE;

    BOARD_COMInit(COM1, &usartConfigStruct);

    BOARD_DelayConfig();
    SPI_InterruptBridgeConfig();

    /* Initialize USB device */
    USB_DeviceInit();

    LOGI(tag, "USB Device CDC + SPI interrupt example\n");

    while (1)
    {
        USB_DevUserApplication();
    }
}

/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
void USB_DevUserApplication(void)
{
    static uint8_t userAppState = USER_APP_INIT;
    static uint8_t interval = 1U;
    
    switch (userAppState)
    {
        case USER_APP_INIT:
            interval = USBD_CDC_ReadInterval(&gUsbDeviceFS);
            APP_QueueTextReport("SPL USB CDC + SPI interrupt bridge ready.\r\nSend 1-32 bytes to trigger one SPI full-duplex transaction.\r\n");
            userAppState = USER_APP_RUN;
            break;

        case USER_APP_RUN:
            APP_ServicePendingReport();

            if ((spiTransferBusy != 0U) && ((BOARD_ReadTick() - spiTransferStartTick) > SPI_TRANSFER_TIMEOUT_MS))
            {
                APP_AbortSpiTransfer(SPI_ERROR_TIMEOUT_FLAG);
            }

            if ((spiTransferDone != 0U) && (cdcReportPending == 0U))
            {
                spiTransferDone = 0U;
                APP_BuildSpiReport();
            }

            if ((spiErrorPending != 0U) && (cdcReportPending == 0U))
            {
                spiErrorPending = 0U;
                APP_BuildSpiErrorReport();
            }

            if (gUsbVCP.state == USBD_CDC_VCP_REV_UPDATE)
            {
                uint8_t rxLength;

                gUsbVCP.state = USBD_CDC_VCP_IDLE;
                rxLength = APP_NormalizeRxLength(gUsbVCP.rxUpdateLen);

                if (rxLength == 0U)
                {
                    APP_QueueTextReport("Ignore empty frame. Please send 1-32 bytes.\r\n");
                }
                else if ((spiTransferBusy != 0U) || (cdcReportPending != 0U))
                {
                    APP_QueueTextReport("Previous SPI transaction is still being processed.\r\n");
                }
                else
                {
                    memcpy(spiMasterTxBuffer, cdcRxBuffer, rxLength);
                    APP_PrepareSlaveTx(rxLength);
                    APP_StartSpiTransfer(rxLength);
                }
            }
            
            BOARD_DelayMs(interval);
            break;
    }
}

static void SPI_InterruptBridgeConfig(void)
{
    GPIO_Config_T gpioConfig;
    SPI_Config_T spiConfig;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | RCM_AHB1_PERIPH_GPIOB | RCM_AHB1_PERIPH_GPIOC);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1 | RCM_APB2_PERIPH_SYSCFG);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_SPI2);

    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_3, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_5, GPIO_AF_SPI1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_15, GPIO_AF_SPI1);

    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.speed = GPIO_SPEED_100MHz;
    gpioConfig.otype = GPIO_OTYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_UP;
    GPIO_Config(GPIOB, &gpioConfig);

    gpioConfig.pin = GPIO_PIN_15;
    GPIO_Config(GPIOA, &gpioConfig);

    SPI_ConfigStructInit(&spiConfig);
    spiConfig.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    spiConfig.mode = SPI_MODE_MASTER;
    spiConfig.length = SPI_DATA_LENGTH_8B;
    spiConfig.polarity = SPI_CLKPOL_HIGH;
    spiConfig.phase = SPI_CLKPHA_1EDGE;
    spiConfig.nss = SPI_NSS_HARD;
    spiConfig.baudrateDiv = SPI_BAUDRATE_DIV_256;
    spiConfig.firstBit = SPI_FIRSTBIT_MSB;
    spiConfig.crcPolynomial = 7U;
    SPI_Config(SPI1, &spiConfig);
    SPI_DisableCRC(SPI1);
    SPI_EnableSSOutput(SPI1);

    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_13, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_2, GPIO_AF_SPI2);
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_3, GPIO_AF_SPI2);

    gpioConfig.pin = GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_Config(GPIOB, &gpioConfig);

    gpioConfig.pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_Config(GPIOC, &gpioConfig);

    SPI_ConfigStructInit(&spiConfig);
    spiConfig.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    spiConfig.mode = SPI_MODE_SLAVE;
    spiConfig.length = SPI_DATA_LENGTH_8B;
    spiConfig.polarity = SPI_CLKPOL_HIGH;
    spiConfig.phase = SPI_CLKPHA_1EDGE;
    spiConfig.nss = SPI_NSS_HARD;
    spiConfig.baudrateDiv = SPI_BAUDRATE_DIV_256;
    spiConfig.firstBit = SPI_FIRSTBIT_MSB;
    spiConfig.crcPolynomial = 7U;
    SPI_Config(SPI2, &spiConfig);
    SPI_DisableCRC(SPI2);

    SPI_I2S_DisableInterrupt(SPI1, SPI_I2S_INT_TXBE | SPI_I2S_INT_RXBNE | SPI_I2S_INT_ERR);
    SPI_I2S_DisableInterrupt(SPI2, SPI_I2S_INT_TXBE | SPI_I2S_INT_RXBNE | SPI_I2S_INT_ERR);

    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_4);
    NVIC_EnableIRQRequest(SPI1_IRQn, 1, 0);
    NVIC_EnableIRQRequest(SPI2_IRQn, 2, 0);

    SPI_Enable(SPI1);
    SPI_Enable(SPI2);
}

static void APP_QueueTextReport(const char *text)
{
    cdcReportLen = (uint16_t)strlen(text);
    if (cdcReportLen > CDC_REPORT_MAX_LEN)
    {
        cdcReportLen = CDC_REPORT_MAX_LEN;
    }
    memcpy(cdcReportBuffer, text, cdcReportLen);
    cdcReportPending = 1U;
}

static uint8_t APP_IsUsbCdcTxIdle(void)
{
    USBD_CDC_INFO_T *usbDevCDC;

    usbDevCDC = (USBD_CDC_INFO_T *)gUsbDeviceFS.devClass[gUsbDeviceFS.classID]->classData;

    if (usbDevCDC == NULL)
    {
        return 0U;
    }

    return (uint8_t)(usbDevCDC->cdcTx.state == USBD_CDC_XFER_IDLE);
}

static void APP_ServicePendingReport(void)
{
    if ((cdcReportPending != 0U) && (APP_IsUsbCdcTxIdle() != 0U))
    {
        memcpy(cdcSendBuffer, cdcReportBuffer, cdcReportLen);

        if (USBD_FS_CDC_ItfSend(cdcSendBuffer, cdcReportLen) == USBD_OK)
        {
            cdcReportPending = 0U;
        }
    }
}

static uint8_t APP_NormalizeRxLength(uint32_t rawLength)
{
    uint32_t validLength = rawLength;

    while (validLength > 0U)
    {
        if ((cdcRxBuffer[validLength - 1U] == '\r') || (cdcRxBuffer[validLength - 1U] == '\n'))
        {
            validLength--;
        }
        else
        {
            break;
        }
    }

    if (validLength > SPI_FRAME_MAX_LEN)
    {
        validLength = SPI_FRAME_MAX_LEN;
    }

    return (uint8_t)validLength;
}

static void APP_PrepareSlaveTx(uint8_t length)
{
    uint8_t index;

    memset(spiMasterRxBuffer, 0, sizeof(spiMasterRxBuffer));
    memset(spiSlaveRxBuffer, 0, sizeof(spiSlaveRxBuffer));

    for (index = 0U; index < length; index++)
    {
        spiSlaveTxBuffer[index] = (uint8_t)(spiMasterTxBuffer[index] ^ 0xA5U);
    }
}

static void APP_StartSpiTransfer(uint8_t length)
{
    spiTransferLen = length;
    spiTransferBusy = 1U;
    spiTransferDone = 0U;
    spiErrorPending = 0U;
    spiErrorFlag = 0U;
    spiMasterTxIndex = 0U;
    spiMasterRxIndex = 0U;
    spiSlaveTxIndex = 0U;
    spiSlaveRxIndex = 0U;
    spiMasterTxDone = 0U;
    spiMasterRxDone = 0U;
    spiSlaveTxDone = 0U;
    spiSlaveRxDone = 0U;
    spiTransferStartTick = BOARD_ReadTick();

    while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE) == SET)
    {
        (void)SPI_I2S_RxData(SPI1);
    }

    while (SPI_I2S_ReadStatusFlag(SPI2, SPI_FLAG_RXBNE) == SET)
    {
        (void)SPI_I2S_RxData(SPI2);
    }

    SPI_I2S_TxData(SPI2, spiSlaveTxBuffer[spiSlaveTxIndex++]);

    if (spiSlaveTxIndex >= spiTransferLen)
    {
        spiSlaveTxDone = 1U;
    }

    SPI_I2S_EnableInterrupt(SPI2, SPI_I2S_INT_RXBNE | SPI_I2S_INT_ERR);
    SPI_I2S_EnableInterrupt(SPI1, SPI_I2S_INT_RXBNE | SPI_I2S_INT_ERR);

    if (spiSlaveTxIndex < spiTransferLen)
    {
        SPI_I2S_EnableInterrupt(SPI2, SPI_I2S_INT_TXBE);
    }
    else
    {
        spiSlaveTxDone = 1U;
    }

    SPI_I2S_TxData(SPI1, spiMasterTxBuffer[spiMasterTxIndex++]);

    if (spiMasterTxIndex >= spiTransferLen)
    {
        spiMasterTxDone = 1U;
    }
    else
    {
        SPI_I2S_EnableInterrupt(SPI1, SPI_I2S_INT_TXBE);
    }
}

static uint16_t APP_AppendHexLine(char *buffer, uint16_t offset, const char *title, uint8_t *data, uint8_t length)
{
    uint8_t index;

    offset += (uint16_t)sprintf(&buffer[offset], "%s", title);

    for (index = 0U; index < length; index++)
    {
        offset += (uint16_t)sprintf(&buffer[offset], "%02X ", data[index]);
    }

    offset += (uint16_t)sprintf(&buffer[offset], "\r\n");

    return offset;
}

static void APP_BuildSpiReport(void)
{
    uint16_t offset = 0U;

    offset += (uint16_t)sprintf((char *)&cdcReportBuffer[offset], "SPI transfer done, len=%u\r\n", spiTransferLen);
    offset = APP_AppendHexLine((char *)cdcReportBuffer, offset, "USB->SPI1 TX : ", spiMasterTxBuffer, spiTransferLen);
    offset = APP_AppendHexLine((char *)cdcReportBuffer, offset, "SPI2 RX      : ", spiSlaveRxBuffer, spiTransferLen);
    offset = APP_AppendHexLine((char *)cdcReportBuffer, offset, "SPI2 TX      : ", spiSlaveTxBuffer, spiTransferLen);
    offset = APP_AppendHexLine((char *)cdcReportBuffer, offset, "SPI1 RX      : ", spiMasterRxBuffer, spiTransferLen);
    offset += (uint16_t)sprintf((char *)&cdcReportBuffer[offset], "\r\n");

    cdcReportLen = offset;
    cdcReportPending = 1U;
}

static void APP_BuildSpiErrorReport(void)
{
    if (spiErrorFlag == SPI_ERROR_TIMEOUT_FLAG)
    {
        cdcReportLen = (uint16_t)sprintf((char *)cdcReportBuffer, "SPI interrupt transfer timeout. Check SPI jumpers: PA15-PB12, PB3-PB13, PB4-PC2, PB5-PC3, GND-GND.\r\n");
    }
    else
    {
        cdcReportLen = (uint16_t)sprintf((char *)cdcReportBuffer, "SPI interrupt transfer error, flag=0x%04X\r\n", spiErrorFlag);
    }

    cdcReportPending = 1U;
}

static void APP_CheckSpiTransferDone(void)
{
    if ((spiMasterTxDone != 0U) && (spiMasterRxDone != 0U) && (spiSlaveTxDone != 0U) && (spiSlaveRxDone != 0U))
    {
        spiTransferBusy = 0U;
        spiTransferDone = 1U;
    }
}

static void APP_AbortSpiTransfer(uint16_t errorFlag)
{
    SPI_I2S_DisableInterrupt(SPI1, SPI_I2S_INT_TXBE | SPI_I2S_INT_RXBNE | SPI_I2S_INT_ERR);
    SPI_I2S_DisableInterrupt(SPI2, SPI_I2S_INT_TXBE | SPI_I2S_INT_RXBNE | SPI_I2S_INT_ERR);

    spiTransferBusy = 0U;
    spiTransferDone = 0U;
    spiMasterTxDone = 0U;
    spiMasterRxDone = 0U;
    spiSlaveTxDone = 0U;
    spiSlaveRxDone = 0U;
    spiErrorFlag = errorFlag;
    spiErrorPending = 1U;
}

void APP_SPI_IRQHandler(SPI_T *spi)
{
    if (SPI_I2S_ReadIntFlag(spi, SPI_I2S_INT_RXBNE) == SET)
    {
        uint8_t data = (uint8_t)SPI_I2S_RxData(spi);

        if (spi == SPI1)
        {
            if (spiMasterRxIndex < spiTransferLen)
            {
                spiMasterRxBuffer[spiMasterRxIndex++] = data;
            }

            if (spiMasterRxIndex >= spiTransferLen)
            {
                SPI_I2S_DisableInterrupt(SPI1, SPI_I2S_INT_RXBNE);
                spiMasterRxDone = 1U;
                APP_CheckSpiTransferDone();
            }
        }
        else
        {
            if (spiSlaveRxIndex < spiTransferLen)
            {
                spiSlaveRxBuffer[spiSlaveRxIndex++] = data;
            }

            if (spiSlaveRxIndex >= spiTransferLen)
            {
                SPI_I2S_DisableInterrupt(SPI2, SPI_I2S_INT_RXBNE);
                spiSlaveRxDone = 1U;
                APP_CheckSpiTransferDone();
            }
        }
    }

    if (SPI_I2S_ReadIntFlag(spi, SPI_I2S_INT_TXBE) == SET)
    {
        if (spi == SPI1)
        {
            if (spiMasterTxIndex < spiTransferLen)
            {
                SPI_I2S_TxData(SPI1, spiMasterTxBuffer[spiMasterTxIndex++]);

                if (spiMasterTxIndex >= spiTransferLen)
                {
                    SPI_I2S_DisableInterrupt(SPI1, SPI_I2S_INT_TXBE);
                    spiMasterTxDone = 1U;
                    APP_CheckSpiTransferDone();
                }
            }
            else
            {
                SPI_I2S_DisableInterrupt(SPI1, SPI_I2S_INT_TXBE);
                spiMasterTxDone = 1U;
                APP_CheckSpiTransferDone();
            }
        }
        else
        {
            if (spiSlaveTxIndex < spiTransferLen)
            {
                SPI_I2S_TxData(SPI2, spiSlaveTxBuffer[spiSlaveTxIndex++]);

                if (spiSlaveTxIndex >= spiTransferLen)
                {
                    SPI_I2S_DisableInterrupt(SPI2, SPI_I2S_INT_TXBE);
                    spiSlaveTxDone = 1U;
                    APP_CheckSpiTransferDone();
                }
            }
            else
            {
                SPI_I2S_DisableInterrupt(SPI2, SPI_I2S_INT_TXBE);
                spiSlaveTxDone = 1U;
                APP_CheckSpiTransferDone();
            }
        }
    }

    if (SPI_I2S_ReadIntFlag(spi, SPI_I2S_INT_OVR) == SET)
    {
        if (spi == SPI1)
        {
            (void)SPI_I2S_RxData(SPI1);
            (void)SPI_I2S_ReadIntFlag(SPI1, SPI_I2S_INT_OVR);
        }
        else
        {
            (void)SPI_I2S_RxData(SPI2);
            (void)SPI_I2S_ReadIntFlag(SPI2, SPI_I2S_INT_OVR);
        }

        APP_AbortSpiTransfer(SPI_FLAG_OVR);
    }

    if (SPI_I2S_ReadIntFlag(spi, SPI_INT_ME) == SET)
    {
        if (spi == SPI1)
        {
            (void)SPI_I2S_ReadIntFlag(SPI1, SPI_INT_ME);
            SPI_Enable(SPI1);
        }
        else
        {
            (void)SPI_I2S_ReadIntFlag(SPI2, SPI_INT_ME);
            SPI_Enable(SPI2);
        }

        APP_AbortSpiTransfer(SPI_FLAG_ME);
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
