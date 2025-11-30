/*===========================================================================
 * @file    drv_wdt.c
 * @brief   Watchdog Timer Driver Implementation
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_wdt.h"

/*---------------------------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------------------------*/
static wdt_handle_t *wdt_handle_ptr = NULL;

/*===========================================================================
 * Public Functions
 *===========================================================================*/

/**
 * @brief  Initialize Watchdog Timer
 */
drv_status_t DRV_WDT_Init(wdt_handle_t *handle, const wdt_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    /* Initialize handle */
    handle->timeout = config->timeout;
    handle->callback = config->callback;
    handle->enabled = 0;
    
    wdt_handle_ptr = handle;
    
    /* Configure timeout period */
    /* WDCON[2:0] = WPS[2:0] */
    TA_REG_UNLOCK();
    WDCON = (WDCON & 0xF8) | (config->timeout & 0x07);
    
    /* Configure idle mode behavior */
    if (config->enable_idle) {
        TA_REG_UNLOCK();
        set_WDCON_WIDPD;  /* Run in idle */
    } else {
        TA_REG_UNLOCK();
        clr_WDCON_WIDPD;
    }
    
    /* Configure interrupt mode */
    if (config->mode == WDT_MODE_INTERRUPT) {
        set_EIE_EWDT;  /* Enable WDT interrupt */
    }
    
    return DRV_OK;
}

/**
 * @brief  Start Watchdog Timer
 */
drv_status_t DRV_WDT_Start(wdt_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Clear WDT counter first */
    TA_REG_UNLOCK();
    set_WDCON_WDCLR;
    
    /* Enable WDT - WARNING: Cannot be disabled! */
    TA_REG_UNLOCK();
    set_WDCON_WDTR;
    
    handle->enabled = 1;
    
    return DRV_OK;
}

/**
 * @brief  Feed Watchdog Timer
 */
drv_status_t DRV_WDT_Feed(wdt_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    if (!handle->enabled) {
        return DRV_NOT_INIT;
    }
    
    /* Clear WDT counter (TA-protected) */
    TA_REG_UNLOCK();
    set_WDCON_WDCLR;
    
    return DRV_OK;
}

/**
 * @brief  Set WDT timeout period
 */
drv_status_t DRV_WDT_SetTimeout(wdt_handle_t *handle, wdt_timeout_t timeout)
{
    DRV_CHECK_PTR(handle);
    
    if (timeout > WDT_TIMEOUT_1600MS) {
        return DRV_INVALID_PARAM;
    }
    
    handle->timeout = timeout;
    
    /* Update timeout (TA-protected) */
    TA_REG_UNLOCK();
    WDCON = (WDCON & 0xF8) | (timeout & 0x07);
    
    return DRV_OK;
}

/**
 * @brief  Check if last reset was caused by WDT
 */
uint8_t DRV_WDT_GetResetFlag(void)
{
    return (WDCON & 0x20) ? 1 : 0;  /* WDTRF flag */
}

/**
 * @brief  Clear WDT reset flag
 */
void DRV_WDT_ClearResetFlag(void)
{
    TA_REG_UNLOCK();
    clr_WDCON_WDTRF;
}

/**
 * @brief  Register WDT callback
 */
drv_status_t DRV_WDT_RegisterCallback(wdt_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    handle->callback = callback;
    return DRV_OK;
}

/*===========================================================================
 * WDT Interrupt Service Routine
 *===========================================================================*/

/**
 * @brief  WDT Timeout Interrupt ISR
 * @note   This occurs before reset in interrupt mode
 */
#if defined(COMPILER_KEIL)
void WDT_ISR(void) interrupt 10
#elif defined(COMPILER_IAR)
#pragma vector = 0x53
__interrupt void WDT_ISR(void)
#elif defined(COMPILER_SDCC)
void WDT_ISR(void) __interrupt(10)
#endif
#if !defined(COMPILER_GCC)
{
    wdt_handle_t *handle = wdt_handle_ptr;
    
    /* Clear WDT interrupt flag */
    TA_REG_UNLOCK();
    clr_WDCON_WDTF;
    
    if (handle && handle->callback) {
        /* User can attempt recovery or log before reset */
        handle->callback(0, NULL);
    }
    
    /* If we return, WDT will reset the MCU */
}
#endif
