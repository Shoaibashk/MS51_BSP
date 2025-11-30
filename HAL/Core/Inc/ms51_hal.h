/**
 *******************************************************************************
 * @file    ms51_hal.h
 * @brief   MS51 Hardware Abstraction Layer (HAL) Main Header File
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This is the main header file for the MS51 HAL library.
 *          Include this file to access all HAL drivers and functions.
 *
 * @note    This HAL library is designed to be compatible with:
 *          - Keil C51 Compiler
 *          - SDCC (Small Device C Compiler)
 *          - IAR Embedded Workbench for 8051
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_H
#define __MS51_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Compiler Detection and Configuration
 ******************************************************************************/
#if defined(__C51__)
    /* Keil C51 Compiler */
    #define HAL_COMPILER_KEIL
    #define HAL_DATA        data
    #define HAL_IDATA       idata
    #define HAL_XDATA       xdata
    #define HAL_CODE        code
    #define HAL_BIT         bit
    #define HAL_INTERRUPT(n) interrupt n
    #define HAL_USING(n)    using n
    #define HAL_NOP()       _nop_()
    
#elif defined(__ICC8051__)
    /* IAR Embedded Workbench for 8051 */
    #define HAL_COMPILER_IAR
    #define HAL_DATA        __data
    #define HAL_IDATA       __idata
    #define HAL_XDATA       __xdata
    #define HAL_CODE        __code
    #define HAL_BIT         __bit
    #define HAL_INTERRUPT(n) 
    #define HAL_USING(n)
    #define HAL_NOP()       __asm("nop")
    
#elif defined(__SDCC__)
    /* SDCC Compiler */
    #define HAL_COMPILER_SDCC
    #define HAL_DATA        __data
    #define HAL_IDATA       __idata
    #define HAL_XDATA       __xdata
    #define HAL_CODE        __code
    #define HAL_BIT         __bit
    #define HAL_INTERRUPT(n) __interrupt(n)
    #define HAL_USING(n)    __using(n)
    #define HAL_NOP()       __asm__("nop;")
    
#else
    #error "Unsupported compiler! Please use Keil C51, SDCC, or IAR for 8051."
#endif

/*******************************************************************************
 * Standard Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#if defined(HAL_COMPILER_KEIL)
    #include <absacc.h>
    #include <intrins.h>
#elif defined(HAL_COMPILER_IAR)
    #include <stdlib.h>
    #include <stdbool.h>
#elif defined(HAL_COMPILER_SDCC)
    #include <stdint.h>
#endif

/*******************************************************************************
 * HAL Version Information
 ******************************************************************************/
#define HAL_VERSION_MAIN    (0x01U)  /**< Major version number */
#define HAL_VERSION_SUB1    (0x00U)  /**< Minor version number */
#define HAL_VERSION_SUB2    (0x00U)  /**< Patch version number */
#define HAL_VERSION_RC      (0x00U)  /**< Release candidate */

/**
 * @brief  HAL Version structure
 */
typedef struct {
    uint8_t major;      /**< Major version */
    uint8_t minor;      /**< Minor version */
    uint8_t patch;      /**< Patch version */
    uint8_t rc;         /**< Release candidate */
} HAL_VersionTypeDef;

/*******************************************************************************
 * HAL Type Definitions
 ******************************************************************************/

/* Standard integer types - ensure consistency across compilers */
#ifndef _STDINT_H
typedef unsigned char   uint8_t;
typedef unsigned int    uint16_t;
typedef unsigned long   uint32_t;
typedef signed char     int8_t;
typedef signed int      int16_t;
typedef signed long     int32_t;
#endif

/* Boolean type */
typedef enum {
    HAL_FALSE = 0U,
    HAL_TRUE = 1U
} HAL_BoolTypeDef;

/*******************************************************************************
 * HAL Status Definitions
 ******************************************************************************/

/**
 * @brief  HAL Status enumeration
 */
typedef enum {
    HAL_OK       = 0x00U,   /**< Operation completed successfully */
    HAL_ERROR    = 0x01U,   /**< Operation error */
    HAL_BUSY     = 0x02U,   /**< Operation busy */
    HAL_TIMEOUT  = 0x03U,   /**< Operation timeout */
    HAL_INVALID  = 0x04U    /**< Invalid parameter */
} HAL_StatusTypeDef;

/**
 * @brief  HAL Lock enumeration
 */
typedef enum {
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/*******************************************************************************
 * HAL Common Macros
 ******************************************************************************/

/* Enable/Disable states */
#define HAL_DISABLE     0U
#define HAL_ENABLE      1U

/* Bit manipulation macros */
#define HAL_SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define HAL_CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define HAL_READ_BIT(REG, BIT)    ((REG) & (BIT))
#define HAL_TOGGLE_BIT(REG, BIT)  ((REG) ^= (BIT))
#define HAL_MODIFY_REG(REG, CLEARMASK, SETMASK) \
    ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))

/* Bit position macros */
#define HAL_BIT0    0x01U
#define HAL_BIT1    0x02U
#define HAL_BIT2    0x04U
#define HAL_BIT3    0x08U
#define HAL_BIT4    0x10U
#define HAL_BIT5    0x20U
#define HAL_BIT6    0x40U
#define HAL_BIT7    0x80U

/* Byte manipulation macros */
#define HAL_HIBYTE(val)       ((uint8_t)((val) >> 8))
#define HAL_LOBYTE(val)       ((uint8_t)((val) & 0xFF))
#define HAL_MAKEWORD(hi, lo)  ((((uint16_t)(hi)) << 8) + (uint16_t)(lo))

/* NULL definition */
#ifndef NULL
#define NULL    ((void *)0)
#endif

/* Unused parameter macro */
#define HAL_UNUSED(x)   ((void)(x))

/*******************************************************************************
 * HAL Module Configuration
 * Enable/disable HAL modules by defining these macros
 ******************************************************************************/
#include "ms51_hal_conf.h"

/*******************************************************************************
 * Device Selection
 ******************************************************************************/
#if defined(MS51_16K) || defined(MS51FB9AE) || defined(MS51XB9AE) || defined(MS51XB9BE)
    #include "ms51_16k_hal.h"
#elif defined(MS51_8K) || defined(MS51DA9AE) || defined(MS51BA9AE)
    #include "ms51_8k_hal.h"
#elif defined(MS51_4K) || defined(MS51FC0AE) || defined(MS51PC0AE) || defined(MS51EC0AE)
    #include "ms51_4k_hal.h"
#else
    /* Default to 16K device if not specified */
    #define MS51_16K
    #include "ms51_16k_hal.h"
#endif

/*******************************************************************************
 * HAL Driver Includes
 ******************************************************************************/

/* GPIO HAL Driver */
#ifdef HAL_GPIO_MODULE_ENABLED
    #include "ms51_hal_gpio.h"
#endif

/* UART HAL Driver */
#ifdef HAL_UART_MODULE_ENABLED
    #include "ms51_hal_uart.h"
#endif

/* Timer HAL Driver */
#ifdef HAL_TIMER_MODULE_ENABLED
    #include "ms51_hal_timer.h"
#endif

/* ADC HAL Driver */
#ifdef HAL_ADC_MODULE_ENABLED
    #include "ms51_hal_adc.h"
#endif

/* I2C HAL Driver */
#ifdef HAL_I2C_MODULE_ENABLED
    #include "ms51_hal_i2c.h"
#endif

/* SPI HAL Driver */
#ifdef HAL_SPI_MODULE_ENABLED
    #include "ms51_hal_spi.h"
#endif

/* PWM HAL Driver */
#ifdef HAL_PWM_MODULE_ENABLED
    #include "ms51_hal_pwm.h"
#endif

/* WDT HAL Driver */
#ifdef HAL_WDT_MODULE_ENABLED
    #include "ms51_hal_wdt.h"
#endif

/* System HAL Driver */
#ifdef HAL_SYSTEM_MODULE_ENABLED
    #include "ms51_hal_system.h"
#endif

/* Flash HAL Driver */
#ifdef HAL_FLASH_MODULE_ENABLED
    #include "ms51_hal_flash.h"
#endif

/* BOD HAL Driver */
#ifdef HAL_BOD_MODULE_ENABLED
    #include "ms51_hal_bod.h"
#endif

/* WKT HAL Driver */
#ifdef HAL_WKT_MODULE_ENABLED
    #include "ms51_hal_wkt.h"
#endif

/* SPROM HAL Driver */
#ifdef HAL_SPROM_MODULE_ENABLED
    #include "ms51_hal_sprom.h"
#endif

/* EEPROM HAL Driver */
#ifdef HAL_EEPROM_MODULE_ENABLED
    #include "ms51_hal_eeprom.h"
#endif

/*******************************************************************************
 * HAL Core Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize the HAL Library
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Init(void);

/**
 * @brief  De-Initialize the HAL Library
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_DeInit(void);

/**
 * @brief  Get HAL Version
 * @param  version Pointer to HAL_VersionTypeDef structure
 * @retval None
 */
void HAL_GetVersion(HAL_VersionTypeDef *version);

/**
 * @brief  Provides a blocking delay in milliseconds
 * @param  ms Delay duration in milliseconds
 * @retval None
 */
void HAL_Delay(uint16_t ms);

/**
 * @brief  Provides a blocking delay in microseconds
 * @param  us Delay duration in microseconds
 * @retval None
 */
void HAL_DelayUs(uint16_t us);

/**
 * @brief  Get system tick count
 * @retval uint32_t Current system tick value
 */
uint32_t HAL_GetTick(void);

/**
 * @brief  Enable global interrupts
 * @retval None
 */
void HAL_EnableInterrupts(void);

/**
 * @brief  Disable global interrupts
 * @retval None
 */
void HAL_DisableInterrupts(void);

/**
 * @brief  Software system reset
 * @retval None (function does not return)
 */
void HAL_SystemReset(void);

/**
 * @brief  Enter power-down mode
 * @retval None
 */
void HAL_EnterPowerDown(void);

/**
 * @brief  Enter idle mode
 * @retval None
 */
void HAL_EnterIdle(void);

/*******************************************************************************
 * Protected Access Macro for TA-Protected Registers
 ******************************************************************************/

/**
 * @brief  Macro for writing to TA-protected SFR registers
 * @note   MS51 has Time-Access (TA) protection for critical registers
 */
#define HAL_TA_WRITE(reg, val)  do { \
    uint8_t _ea_tmp = EA; \
    EA = 0; \
    TA = 0xAA; \
    TA = 0x55; \
    reg = (val); \
    EA = _ea_tmp; \
} while(0)

/**
 * @brief  Macro for setting bits in TA-protected SFR registers
 */
#define HAL_TA_SET_BIT(reg, bit)  do { \
    uint8_t _ea_tmp = EA; \
    EA = 0; \
    TA = 0xAA; \
    TA = 0x55; \
    reg |= (bit); \
    EA = _ea_tmp; \
} while(0)

/**
 * @brief  Macro for clearing bits in TA-protected SFR registers
 */
#define HAL_TA_CLEAR_BIT(reg, bit)  do { \
    uint8_t _ea_tmp = EA; \
    EA = 0; \
    TA = 0xAA; \
    TA = 0x55; \
    reg &= ~(bit); \
    EA = _ea_tmp; \
} while(0)

/*******************************************************************************
 * SFR Page Selection Macros
 ******************************************************************************/

/**
 * @brief  Select SFR Page 0
 */
#define HAL_SFR_PAGE0()  HAL_TA_WRITE(SFRS, 0x00)

/**
 * @brief  Select SFR Page 1
 */
#define HAL_SFR_PAGE1()  HAL_TA_WRITE(SFRS, 0x01)

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_H */
