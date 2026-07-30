/*!
 * @file        apm32f4xx.h
 *
 * @brief       CMSIS Cortex-M4 Device Peripheral Access Layer Header File.
 *
 * @details     This file contains all the peripheral register's definitions, bits definitions and memory mapping
 *
 * @version     V1.0.6
 *
 * @date        2025-12-15
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

#ifndef __APM32F4XX_H
#define __APM32F4XX_H

#ifdef __cplusplus
  extern "C" {
#endif /* __cplusplus */

/** @addtogroup CMSIS
  @{
*/

/** @defgroup APM32F4xx
  * @brief Peripheral Access Layer
  @{
*/

/** @defgroup HSE_Macros
  @{
*/

/**
 * @brief Define Value of the External oscillator in Hz
 */
#ifndef HSE_VALUE
#define HSE_VALUE                   ((uint32_t)8000000)
#endif

/** Time out for HSE start up */
#define HSE_STARTUP_TIMEOUT         ((uint16_t)0x05000)

/** Value of the Internal oscillator in Hz */
#define HSI_VALUE                   ((uint32_t)16000000)

/**
  * @}
  */

/** @defgroup APM32F4xx_StdPeripheral_Library_Version
  @{
*/

/**
  * @brief APM32 Family
  */
#if !defined  (APM32F4)
#define APM32F4
#endif /* APM32F4 */

/* Uncomment the line below according to the target APM32 device used in your
   application
  */
#if !defined (APM32F405xx) && !defined (APM32F407xx) && !defined (APM32F415xx) && \
    !defined (APM32F417xx) && !defined (APM32F411xx) && !defined (APM32F423xx) && \
    !defined (APM32F425xx) && !defined (APM32F427xx)
    /* #define APM32F405xx */   /*!< APM32F405RG, APM32F405VG and APM32F405ZG Devices */
    /* #define APM32F407xx */   /*!< APM32F407VG, APM32F407VE, APM32F407ZG, APM32F407ZE, APM32F407IG and APM32F407IE Devices */
    /* #define APM32F415xx */   /*!< APM32F415VG, APM32F415VE, APM32F415ZG, APM32F415ZE, APM32F415IG and APM32F415IE Devices */
    /* #define APM32F417xx */   /*!< APM32F417VG, APM32F417VE, APM32F417ZG, APM32F417ZE, APM32F417IG and APM32F417IE Devices */
    /* #define APM32F411xx */   /*!< APM32F411CC, APM32F411CE, APM32F411RC, APM32F411RE, APM32F411VC and APM32F411VE Devices */
    /* #define APM32F423xx */   /*!< APM32F423ZG, APM32F423VG, APM32F423RG and APM32F423CG Devices */
    /* #define APM32F425xx */   /*!< APM32F425ZG, APM32F425VG, APM32F425RG and APM32F425CG Devices */
    /* #define APM32F427xx */   /*!< APM32F427ZG, APM32F427VG, APM32F427RG and APM32F427CG Devices */
#endif

/**
 * @brief Library_Version_Number_Macros
 */
#define __APM32F4XX_STDPERIPH_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __APM32F4XX_STDPERIPH_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __APM32F4XX_STDPERIPH_VERSION_SUB2   (0x06) /*!< [15:8]  sub2 version */
#define __APM32F4XX_STDPERIPH_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __APM32F4XX_STDPERIPH_VERSION        ((__APM32F4XX_STDPERIPH_VERSION_MAIN << 24)\
                                             |(__APM32F4XX_STDPERIPH_VERSION_SUB1 << 16)\
                                             |(__APM32F4XX_STDPERIPH_VERSION_SUB2 << 8)\
                                             |(__APM32F4XX_STDPERIPH_VERSION_RC))

/**
  * @}
  */

/** @addtogroup Device_Included
  * @{
  */

#if defined(APM32F405xx)
  #include "apm32f405xx.h"
#elif defined(APM32F407xx)
  #include "apm32f407xx.h"
#elif defined(APM32F415xx)
  #include "apm32f415xx.h"
#elif defined(APM32F417xx)
  #include "apm32f417xx.h"
#elif defined(APM32F411xx)
  #include "apm32f411xx.h"
#elif defined(APM32F423xx)
  #include "apm32f423xx.h"
#elif defined(APM32F425xx)
  #include "apm32f425xx.h"
#elif defined(APM32F427xx)
  #include "apm32f427xx.h"
#else
 #error "Please select first the target APM32F4xx device used in your application (in apm32f4xx.h file)"
#endif

/**
  * @}
  */

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
  #ifndef __weak
    #define __weak  __attribute__((weak))
  #endif
  #ifndef __packed
    #define __packed  __attribute__((packed))
  #endif
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

/** @defgroup Exported_Types
  @{
*/

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;
typedef __I int16_t vsc16;
typedef __I int8_t vsc8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t vuc8;

#ifndef __IM
#define __IM   __I
#endif
#ifndef __OM
#define __OM   __O
#endif
#ifndef __IOM
#define __IOM  __IO
#endif

enum {BIT_RESET, BIT_SET};
enum {RESET, SET};
enum {DISABLE, ENABLE};
enum {ERROR, SUCCESS};

#ifndef NULL
#define NULL   ((void *)0)
#endif

#if defined (__CC_ARM )
#pragma anon_unions
#endif

/**
  * @}
  */

/** @defgroup Exported_Macros
  @{
*/

/* Define one bit mask */
#define BIT0                    ((uint32_t)0x00000001)
#define BIT1                    ((uint32_t)0x00000002)
#define BIT2                    ((uint32_t)0x00000004)
#define BIT3                    ((uint32_t)0x00000008)
#define BIT4                    ((uint32_t)0x00000010)
#define BIT5                    ((uint32_t)0x00000020)
#define BIT6                    ((uint32_t)0x00000040)
#define BIT7                    ((uint32_t)0x00000080)
#define BIT8                    ((uint32_t)0x00000100)
#define BIT9                    ((uint32_t)0x00000200)
#define BIT10                   ((uint32_t)0x00000400)
#define BIT11                   ((uint32_t)0x00000800)
#define BIT12                   ((uint32_t)0x00001000)
#define BIT13                   ((uint32_t)0x00002000)
#define BIT14                   ((uint32_t)0x00004000)
#define BIT15                   ((uint32_t)0x00008000)
#define BIT16                   ((uint32_t)0x00010000)
#define BIT17                   ((uint32_t)0x00020000)
#define BIT18                   ((uint32_t)0x00040000)
#define BIT19                   ((uint32_t)0x00080000)
#define BIT20                   ((uint32_t)0x00100000)
#define BIT21                   ((uint32_t)0x00200000)
#define BIT22                   ((uint32_t)0x00400000)
#define BIT23                   ((uint32_t)0x00800000)
#define BIT24                   ((uint32_t)0x01000000)
#define BIT25                   ((uint32_t)0x02000000)
#define BIT26                   ((uint32_t)0x04000000)
#define BIT27                   ((uint32_t)0x08000000)
#define BIT28                   ((uint32_t)0x10000000)
#define BIT29                   ((uint32_t)0x20000000)
#define BIT30                   ((uint32_t)0x40000000)
#define BIT31                   ((uint32_t)0x80000000)

#define UNUSED(X)               (void)X      /* To avoid gcc/g++ warnings */

#define SET_BIT(REG, BIT)       ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)      ((REG) & (BIT))

#define CLEAR_REG(REG)          ((REG) = (0x0))

#define WRITE_REG(REG, VAL)     ((REG) = (VAL))

#define READ_REG(REG)           ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __APM32F4XX_H */

/**@} end of group APM32F4xx */
/**@} end of group CMSIS */
