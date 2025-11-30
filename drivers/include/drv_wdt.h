/*===========================================================================
 * @file    drv_wdt.h
 * @brief   Watchdog Timer Driver Interface
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details Watchdog Timer driver for MS51 microcontrollers.
 *          Supports watchdog reset and interrupt modes.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_WDT_H
#define DRV_WDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * WDT Timeout Period
 *---------------------------------------------------------------------------*/
typedef enum {
    WDT_TIMEOUT_6MS    = 0,     /**< 2^12 / LIRC ~6.5ms */
    WDT_TIMEOUT_25MS   = 1,     /**< 2^14 / LIRC ~26ms */
    WDT_TIMEOUT_50MS   = 2,     /**< 2^15 / LIRC ~52ms */
    WDT_TIMEOUT_100MS  = 3,     /**< 2^16 / LIRC ~104ms */
    WDT_TIMEOUT_200MS  = 4,     /**< 2^17 / LIRC ~209ms */
    WDT_TIMEOUT_400MS  = 5,     /**< 2^18 / LIRC ~419ms */
    WDT_TIMEOUT_800MS  = 6,     /**< 2^19 / LIRC ~839ms */
    WDT_TIMEOUT_1600MS = 7      /**< 2^20 / LIRC ~1.68s */
} wdt_timeout_t;

/*---------------------------------------------------------------------------
 * WDT Mode
 *---------------------------------------------------------------------------*/
typedef enum {
    WDT_MODE_RESET     = 0,     /**< Reset MCU on timeout */
    WDT_MODE_INTERRUPT = 1      /**< Interrupt on timeout (then reset) */
} wdt_mode_t;

/*---------------------------------------------------------------------------
 * WDT Configuration Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    wdt_timeout_t   timeout;        /**< Timeout period */
    wdt_mode_t      mode;           /**< WDT mode */
    uint8_t         enable_idle;    /**< Run WDT in idle mode */
    uint8_t         enable_pd;      /**< Run WDT in power-down mode */
    drv_callback_t  callback;       /**< Timeout callback (interrupt mode) */
} wdt_config_t;

/*---------------------------------------------------------------------------
 * WDT Handle Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    wdt_timeout_t   timeout;        /**< Current timeout setting */
    drv_callback_t  callback;       /**< Event callback */
    volatile uint8_t enabled;       /**< WDT enabled flag */
} wdt_handle_t;

/*===========================================================================
 * Default Configuration Macro
 *===========================================================================*/

/**
 * @brief Default WDT configuration (800ms reset mode)
 */
#define WDT_DEFAULT_CONFIG {                \
    .timeout     = WDT_TIMEOUT_800MS,       \
    .mode        = WDT_MODE_RESET,          \
    .enable_idle = 0,                       \
    .enable_pd   = 0,                       \
    .callback    = NULL                     \
}

/*===========================================================================
 * Public API Functions
 *===========================================================================*/

/**
 * @brief  Initialize Watchdog Timer
 * @param  handle  Pointer to WDT handle
 * @param  config  Pointer to WDT configuration
 * @return DRV_OK on success, error code otherwise
 * 
 * @warning Once WDT is enabled, it cannot be disabled except by reset!
 * 
 * @code
 *   wdt_handle_t wdt;
 *   wdt_config_t config = WDT_DEFAULT_CONFIG;
 *   DRV_WDT_Init(&wdt, &config);
 *   DRV_WDT_Start(&wdt);
 *   
 *   while (1) {
 *       // Application code
 *       DRV_WDT_Feed(&wdt);  // Must feed within timeout!
 *   }
 * @endcode
 */
drv_status_t DRV_WDT_Init(wdt_handle_t *handle, const wdt_config_t *config);

/**
 * @brief  Start Watchdog Timer
 * @param  handle  Pointer to WDT handle
 * @return DRV_OK on success
 * 
 * @warning WDT cannot be stopped once started!
 */
drv_status_t DRV_WDT_Start(wdt_handle_t *handle);

/**
 * @brief  Feed (reload) Watchdog Timer
 * @param  handle  Pointer to WDT handle
 * @return DRV_OK on success
 * 
 * @note Must be called periodically to prevent reset
 */
drv_status_t DRV_WDT_Feed(wdt_handle_t *handle);

/**
 * @brief  Set WDT timeout period
 * @param  handle   Pointer to WDT handle
 * @param  timeout  New timeout period
 * @return DRV_OK on success
 */
drv_status_t DRV_WDT_SetTimeout(wdt_handle_t *handle, wdt_timeout_t timeout);

/**
 * @brief  Check if last reset was caused by WDT
 * @return 1 if WDT reset occurred, 0 otherwise
 */
uint8_t DRV_WDT_GetResetFlag(void);

/**
 * @brief  Clear WDT reset flag
 */
void DRV_WDT_ClearResetFlag(void);

/**
 * @brief  Register WDT callback (for interrupt mode)
 * @param  handle    Pointer to WDT handle
 * @param  callback  Callback function
 * @return DRV_OK on success
 */
drv_status_t DRV_WDT_RegisterCallback(wdt_handle_t *handle, drv_callback_t callback);

/*---------------------------------------------------------------------------
 * Quick Feed Macro (for time-critical code)
 *---------------------------------------------------------------------------*/

/**
 * @brief Quick watchdog feed using TA-protected access
 */
#define DRV_WDT_FEED_QUICK()    do {    \
    TA_REG_UNLOCK();                    \
    set_WDCON_WDCLR;                    \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* DRV_WDT_H */
