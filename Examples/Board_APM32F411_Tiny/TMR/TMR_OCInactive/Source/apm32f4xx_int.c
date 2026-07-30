/*!
 * @file        apm32f4xx_int.c
 *
 * @brief       Main Interrupt Service Routines
 *
 * @version     V1.0.4
 *
 * @date        2025-02-15
 *
 * @attention
 *
 *  Copyright (C) 2021-2025 Geehy Semiconductor
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
#include "apm32f4xx_int.h"

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles SVCall exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief   This function handles PendSV_Handler exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief   This function handles SysTick Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SysTick_Handler(void)
{
    BOARD_DelayIRQHandler();
}

/*!
 * @brief       TMR3 Handler
 *
 * @param       None
 *
 * @retval      None
 */
void TMR3_IRQHandler(void)
{
    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC1))
    {
        /* Clear the TMR3 CC1 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC1);

        /* delay 800us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_1, 0);
    }

    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC2))
    {
        /* Clear the TMR3 CC2 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC2);

        /* delay 600us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_2, 0);
    }

    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC3))
    {
        /* Clear the TMR3 CC3 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC3);

        /* delay 400us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_3, 0);
    }

    if (TMR_ReadIntFlag(TMR3, TMR_INT_CC4))
    {
        /* Clear the TMR3 CC4 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CC4);

        /* delay 200us generate falling edge */
        GPIO_WriteBitValue(GPIOA, GPIO_PIN_4, 0);
    }
}
