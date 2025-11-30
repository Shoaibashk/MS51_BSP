/*===========================================================================
 * @file    svc_system.h
 * @brief   System Service Interface
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Provides system-level services including reset, clock
 *          configuration, watchdog management, and device information.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef SVC_SYSTEM_H
#define SVC_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Reset Sources
 *---------------------------------------------------------------------------*/
typedef enum {
    RESET_SRC_UNKNOWN   = 0x00,     /**< Unknown reset source */
    RESET_SRC_POR       = 0x01,     /**< Power-on reset */
    RESET_SRC_EXTERNAL  = 0x02,     /**< External reset pin */
    RESET_SRC_WDT       = 0x04,     /**< Watchdog timeout reset */
    RESET_SRC_BOD       = 0x08,     /**< Brown-out reset */
    RESET_SRC_SOFTWARE  = 0x10      /**< Software reset */
} reset_source_t;

/* Compatibility aliases used in samples */
#define RESET_SRC_EXT   RESET_SRC_EXTERNAL
#define RESET_SRC_SW    RESET_SRC_SOFTWARE

/*---------------------------------------------------------------------------
 * Clock Sources
 *---------------------------------------------------------------------------*/
typedef enum {
    CLK_SRC_HIRC_24MHZ  = 0,    /**< Internal high-speed RC (24 MHz) */
    CLK_SRC_HIRC_16MHZ  = 1,    /**< Internal high-speed RC (16 MHz) */
    CLK_SRC_HIRC_166MHZ = 2,    /**< Internal high-speed RC (16.6 MHz) */
    CLK_SRC_LIRC        = 3,    /**< Internal low-speed RC (10 kHz) */
    CLK_SRC_ECLK        = 4     /**< External clock input */
} clock_source_t;

/*---------------------------------------------------------------------------
 * System Configuration
 *---------------------------------------------------------------------------*/
typedef struct {
    clock_source_t  clock_source;   /**< System clock source */
    uint8_t         clock_divider;  /**< Clock divider (0-7, CKDIV) */
    uint8_t         wdt_enable;     /**< Enable watchdog on init */
    uint16_t        wdt_timeout_ms; /**< Watchdog timeout in ms */
} system_config_t;

/*---------------------------------------------------------------------------
 * System Initialization
 *---------------------------------------------------------------------------*/
/**
 * @brief  Initialize system services
 * @param  config  System configuration (NULL for defaults)
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_Init(const system_config_t *config);

/**
 * @brief  De-initialize system services
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_DeInit(void);

/*---------------------------------------------------------------------------
 * Reset Control
 *---------------------------------------------------------------------------*/
/**
 * @brief  Perform software reset
 * @note   This function does not return
 */
void SVC_SYSTEM_Reset(void);

/**
 * @brief  Reset and boot to APROM
 * @note   This function does not return
 */
void SVC_SYSTEM_ResetToAPROM(void);

/**
 * @brief  Reset and boot to LDROM
 * @note   This function does not return
 */
void SVC_SYSTEM_ResetToLDROM(void);

/**
 * @brief  Get last reset source
 * @return Reset source that caused last system reset
 */
reset_source_t SVC_SYSTEM_GetResetSource(void);

/**
 * @brief  Clear reset source flags
 */
void SVC_SYSTEM_ClearResetSource(void);

/*---------------------------------------------------------------------------
 * Clock Management
 *---------------------------------------------------------------------------*/
/**
 * @brief  Set system clock source
 * @param  source  Clock source to use
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_SetClockSource(clock_source_t source);

/**
 * @brief  Get current system clock frequency
 * @return System clock in Hz
 */
uint32_t SVC_SYSTEM_GetClockHz(void);

/**
 * @brief  Set clock divider
 * @param  divider  Clock divider value (0-7)
 *         0 = Fsys/1, 1 = Fsys/2, ... 7 = Fsys/128
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_SetClockDivider(uint8_t divider);

/**
 * @brief  Enable clock output on P1.0
 * @param  divider  Output frequency = Fsys / (2 * (divider+1))
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_EnableClockOutput(uint8_t divider);

/**
 * @brief  Disable clock output
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_DisableClockOutput(void);

/*---------------------------------------------------------------------------
 * Watchdog Management
 *---------------------------------------------------------------------------*/
/**
 * @brief  Initialize and start watchdog
 * @param  timeout_ms  Timeout in milliseconds
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_WDT_Start(uint16_t timeout_ms);

/**
 * @brief  Stop watchdog timer
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_WDT_Stop(void);

/**
 * @brief  Kick (feed) watchdog timer
 * @note   Call periodically to prevent watchdog reset
 */
void SVC_SYSTEM_WDT_Kick(void);

/**
 * @brief  Check if last reset was from watchdog
 * @return 1 if WDT reset occurred, 0 otherwise
 */
uint8_t SVC_SYSTEM_WDT_IsReset(void);

/*---------------------------------------------------------------------------
 * Device Information
 *---------------------------------------------------------------------------*/
/**
 * @brief  Get device unique ID
 * @param  uid  Buffer to store 12-byte UID
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_GetUID(uint8_t *uid);

/**
 * @brief  Get device unique company ID
 * @param  ucid  Buffer to store 8-byte UCID
 * @return DRV_OK on success
 */
drv_status_t SVC_SYSTEM_GetUCID(uint8_t *ucid);

/**
 * @brief  Get device ID
 * @return Device ID value
 */
uint32_t SVC_SYSTEM_GetDeviceID(void);

/**
 * @brief  Get BSP version
 * @return Version as (major<<16 | minor<<8 | patch)
 */
uint32_t SVC_SYSTEM_GetBSPVersion(void);

/*---------------------------------------------------------------------------
 * Interrupt Management
 *---------------------------------------------------------------------------*/
/**
 * @brief  Enable global interrupts
 */
void SVC_SYSTEM_EnableInterrupts(void);

/**
 * @brief  Disable global interrupts
 */
void SVC_SYSTEM_DisableInterrupts(void);

/**
 * @brief  Enter critical section (save and disable interrupts)
 * @return Previous interrupt state
 */
uint8_t SVC_SYSTEM_EnterCritical(void);

/**
 * @brief  Exit critical section (restore interrupt state)
 * @param  state  State returned from SVC_SYSTEM_EnterCritical
 */
void SVC_SYSTEM_ExitCritical(uint8_t state);

#ifdef __cplusplus
}
#endif

#endif /* SVC_SYSTEM_H */
