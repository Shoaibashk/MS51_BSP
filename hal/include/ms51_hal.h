/*===========================================================================
 * @file    ms51_hal.h
 * @brief   MS51 Hardware Abstraction Layer - Main Header
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details This is the main HAL header file. Include this file in your
 *          application to get access to all HAL functionality.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_HAL_H
#define MS51_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 * Version Information
 *---------------------------------------------------------------------------*/
#define MS51_HAL_VERSION_MAJOR  2
#define MS51_HAL_VERSION_MINOR  0
#define MS51_HAL_VERSION_PATCH  0
#define MS51_HAL_VERSION        ((MS51_HAL_VERSION_MAJOR << 16) | \
                                 (MS51_HAL_VERSION_MINOR << 8)  | \
                                 (MS51_HAL_VERSION_PATCH))

/*---------------------------------------------------------------------------
 * Device Selection
 * Define one of these in your project settings:
 *   - MS51_8K   (MS51DA9AE, MS51BA9AE)
 *   - MS51_16K  (MS51FB9AE, MS51XB9AE, MS51XB9BE)
 *   - MS51_32K  (MS51FC0AE, MS51XC0BE, etc.)
 *---------------------------------------------------------------------------*/
#if !defined(MS51_8K) && !defined(MS51_16K) && !defined(MS51_32K)
    #define MS51_16K    /* Default to 16K variant */
#endif

/*---------------------------------------------------------------------------
 * Include Core HAL Headers
 *---------------------------------------------------------------------------*/
#include "ms51_types.h"         /* Type definitions */
#include "ms51_compiler.h"      /* Compiler abstraction */

/* Device-specific SFR definitions */
#if defined(MS51_8K)
    #include "ms51_sfr_8k.h"
#elif defined(MS51_16K)
    #include "ms51_sfr_16k.h"
#elif defined(MS51_32K)
    #include "ms51_sfr_32k.h"
#endif

#include "ms51_sfr_bits.h"      /* Bit manipulation macros */

/*---------------------------------------------------------------------------
 * System Configuration
 *---------------------------------------------------------------------------*/
/* Default system clock (can be overridden in project) */
#ifndef SYSTEM_CLOCK_HZ
    #define SYSTEM_CLOCK_HZ     24000000UL  /* 24 MHz */
#endif

/* HIRC frequency options */
#define HIRC_24MHZ      24000000UL
#define HIRC_16MHZ      16000000UL
#define HIRC_166MHZ     16600000UL  /* 16.6 MHz for specific baud rates */

/*---------------------------------------------------------------------------
 * Critical Section Macros
 *---------------------------------------------------------------------------*/
/**
 * @brief Enter critical section (disable interrupts)
 * @note  Saves EA state for proper nesting
 */
#define HAL_ENTER_CRITICAL()    { uint8_t _ea_save = EA; EA = 0

/**
 * @brief Exit critical section (restore interrupts)
 */
#define HAL_EXIT_CRITICAL()     EA = _ea_save; }

/**
 * @brief Simple interrupt disable/enable
 */
#define HAL_DISABLE_INTERRUPTS()    (EA = 0)
#define HAL_ENABLE_INTERRUPTS()     (EA = 1)

/*---------------------------------------------------------------------------
 * SFR Access Macros
 *---------------------------------------------------------------------------*/
/**
 * @brief Direct SFR read/write
 */
#define HAL_SFR_READ(reg)           (reg)
#define HAL_SFR_WRITE(reg, val)     ((reg) = (val))

/**
 * @brief Bit manipulation on SFR
 */
#define HAL_SFR_BIT_SET(reg, mask)      ((reg) |= (mask))
#define HAL_SFR_BIT_CLR(reg, mask)      ((reg) &= ~(mask))
#define HAL_SFR_BIT_TOGGLE(reg, mask)   ((reg) ^= (mask))
#define HAL_SFR_BIT_GET(reg, mask)      (((reg) & (mask)) != 0)

/**
 * @brief TA (Timed Access) protected register unlock sequence
 * @note  Some SFRs require TA unlock before modification
 */
#define HAL_TA_UNLOCK()     do { TA = 0xAA; TA = 0x55; } while(0)

/**
 * @brief Write to TA-protected register
 */
#define HAL_TA_WRITE(reg, val) do { \
    uint8_t _ea = EA; \
    EA = 0; \
    HAL_TA_UNLOCK(); \
    (reg) = (val); \
    EA = _ea; \
} while(0)

/*---------------------------------------------------------------------------
 * SFR Page Selection
 *---------------------------------------------------------------------------*/
/**
 * @brief Select SFR page 0 (default)
 */
#define HAL_SFR_PAGE0()     do { clr_SFRS_SFRPAGE; } while(0)

/**
 * @brief Select SFR page 1
 */
#define HAL_SFR_PAGE1()     do { set_SFRS_SFRPAGE; } while(0)

/*---------------------------------------------------------------------------
 * NOP and Delay
 *---------------------------------------------------------------------------*/
#define HAL_NOP()           CALL_NOP

/**
 * @brief Short inline delay (approximate cycles)
 */
#define HAL_DELAY_CYCLES(n) do { \
    uint8_t _i = (n); \
    while (_i--) { HAL_NOP(); } \
} while(0)

/*---------------------------------------------------------------------------
 * Common Bit Definitions
 *---------------------------------------------------------------------------*/
#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

/*---------------------------------------------------------------------------
 * System Functions
 *---------------------------------------------------------------------------*/
/**
 * @brief  Set HIRC frequency
 * @param  freq  HIRC_24MHZ, HIRC_16MHZ, or HIRC_166MHZ
 */
void HAL_SYS_SetHIRC(uint32_t freq);

/**
 * @brief  Software reset to APROM
 */
void HAL_SYS_ResetToAPROM(void);

/**
 * @brief  Software reset to LDROM
 */
void HAL_SYS_ResetToLDROM(void);

/**
 * @brief  Get current system clock frequency
 * @return System clock in Hz
 */
uint32_t HAL_SYS_GetClock(void);

/**
 * @brief  Enter idle mode
 * @note   CPU stops, peripherals continue running
 */
void HAL_SYS_EnterIdle(void);

/**
 * @brief  Enter power-down mode
 * @note   Most peripherals stop, minimal power consumption
 */
void HAL_SYS_EnterPowerDown(void);

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
extern BIT BIT_TMP;
extern DATA_MEM uint8_t TA_REG_TMP;
extern DATA_MEM uint8_t BYTE_TMP;
extern DATA_MEM uint8_t SFRS_TMP;

#ifdef __cplusplus
}
#endif

#endif /* MS51_HAL_H */
