/*===========================================================================
 * @file    drv_timer.h
 * @brief   Timer Driver Interface
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details Timer driver for MS51 microcontrollers.
 *          Supports Timer0, Timer1, Timer2, Timer3, and Wakeup Timer.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_TIMER_H
#define DRV_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Timer Instance Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    TIMER_0 = 0,    /**< Timer 0 (8-bit or 16-bit) */
    TIMER_1 = 1,    /**< Timer 1 (8-bit or 16-bit) */
    TIMER_2 = 2,    /**< Timer 2 (16-bit auto-reload) */
    TIMER_3 = 3,    /**< Timer 3 (16-bit, baud rate or PWM) */
    TIMER_WKT = 4   /**< Wakeup Timer */
} timer_instance_t;

/*---------------------------------------------------------------------------
 * Timer Mode Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    TIMER_MODE_0 = 0,   /**< Mode 0: 13-bit timer/counter */
    TIMER_MODE_1 = 1,   /**< Mode 1: 16-bit timer/counter */
    TIMER_MODE_2 = 2,   /**< Mode 2: 8-bit auto-reload */
    TIMER_MODE_3 = 3    /**< Mode 3: Split 8-bit timers (T0 only) */
} timer_mode_t;

/*---------------------------------------------------------------------------
 * Timer Clock Source
 *---------------------------------------------------------------------------*/
typedef enum {
    TIMER_CLK_FSYS      = 0,    /**< System clock (Fsys) */
    TIMER_CLK_FSYS_DIV  = 1,    /**< Fsys divided (depends on timer) */
    TIMER_CLK_EXT       = 2,    /**< External clock input */
    TIMER_CLK_LIRC      = 3     /**< LIRC 10kHz (WKT only) */
} timer_clock_t;

/*---------------------------------------------------------------------------
 * Timer Prescaler
 *---------------------------------------------------------------------------*/
typedef enum {
    TIMER_PSC_1    = 0,     /**< Prescaler 1:1 */
    TIMER_PSC_12   = 1,     /**< Prescaler 1:12 (legacy 8051) */
    TIMER_PSC_32   = 5,     /**< Prescaler 1:32 */
    TIMER_PSC_64   = 6,     /**< Prescaler 1:64 */
    TIMER_PSC_128  = 7,     /**< Prescaler 1:128 */
    TIMER_PSC_256  = 8,     /**< Prescaler 1:256 */
    TIMER_PSC_512  = 9,     /**< Prescaler 1:512 */
    TIMER_PSC_1024 = 10     /**< Prescaler 1:1024 */
} timer_prescaler_t;

/*---------------------------------------------------------------------------
 * Timer Configuration Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    timer_instance_t    instance;   /**< Timer instance */
    timer_mode_t        mode;       /**< Timer mode */
    timer_clock_t       clock;      /**< Clock source */
    timer_prescaler_t   prescaler;  /**< Clock prescaler */
    uint16_t            period;     /**< Timer period value */
    uint8_t             auto_reload;/**< Enable auto-reload */
    uint8_t             enable_int; /**< Enable interrupt */
    drv_callback_t      callback;   /**< Overflow callback */
} timer_config_t;

/*---------------------------------------------------------------------------
 * Timer Events
 *---------------------------------------------------------------------------*/
typedef enum {
    TIMER_EVT_OVERFLOW = 0,     /**< Timer overflow/period match */
    TIMER_EVT_CAPTURE  = 1      /**< Capture event (T2 only) */
} timer_event_t;

/*---------------------------------------------------------------------------
 * Timer Handle Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    timer_instance_t instance;      /**< Timer instance */
    drv_callback_t   callback;      /**< Event callback */
    volatile uint32_t overflow_cnt; /**< Overflow counter */
    volatile uint8_t  running;      /**< Running flag */
} timer_handle_t;

/*===========================================================================
 * Default Configuration Macros
 *===========================================================================*/

/**
 * @brief Default 1ms tick timer configuration (Timer0)
 */
#define TIMER0_1MS_CONFIG {                 \
    .instance    = TIMER_0,                 \
    .mode        = TIMER_MODE_1,            \
    .clock       = TIMER_CLK_FSYS,          \
    .prescaler   = TIMER_PSC_12,            \
    .period      = 65536 - 2000,            \
    .auto_reload = 0,                       \
    .enable_int  = 1,                       \
    .callback    = NULL                     \
}

/*===========================================================================
 * Public API Functions
 *===========================================================================*/

/**
 * @brief  Initialize timer peripheral
 * @param  handle  Pointer to timer handle
 * @param  config  Pointer to timer configuration
 * @return DRV_OK on success, error code otherwise
 * 
 * @code
 *   timer_handle_t timer0;
 *   timer_config_t config = TIMER0_1MS_CONFIG;
 *   config.callback = my_timer_callback;
 *   DRV_Timer_Init(&timer0, &config);
 * @endcode
 */
drv_status_t DRV_Timer_Init(timer_handle_t *handle, const timer_config_t *config);

/**
 * @brief  De-initialize timer peripheral
 * @param  handle  Pointer to timer handle
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer_DeInit(timer_handle_t *handle);

/**
 * @brief  Start timer
 * @param  handle  Pointer to timer handle
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer_Start(timer_handle_t *handle);

/**
 * @brief  Stop timer
 * @param  handle  Pointer to timer handle
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer_Stop(timer_handle_t *handle);

/**
 * @brief  Reset timer counter to reload value
 * @param  handle  Pointer to timer handle
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer_Reset(timer_handle_t *handle);

/**
 * @brief  Get current timer count
 * @param  handle  Pointer to timer handle
 * @return Current count value
 */
uint16_t DRV_Timer_GetCount(timer_handle_t *handle);

/**
 * @brief  Set timer period/reload value
 * @param  handle  Pointer to timer handle
 * @param  period  New period value
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer_SetPeriod(timer_handle_t *handle, uint16_t period);

/**
 * @brief  Get overflow count
 * @param  handle  Pointer to timer handle
 * @return Number of overflows since start
 */
uint32_t DRV_Timer_GetOverflows(timer_handle_t *handle);

/**
 * @brief  Register timer callback
 * @param  handle    Pointer to timer handle
 * @param  callback  Callback function
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer_RegisterCallback(timer_handle_t *handle, drv_callback_t callback);

/*---------------------------------------------------------------------------
 * Timer2 Capture Functions
 *---------------------------------------------------------------------------*/

/**
 * @brief  Initialize Timer2 in capture mode
 * @param  handle   Pointer to timer handle
 * @param  callback Capture event callback
 * @return DRV_OK on success
 */
drv_status_t DRV_Timer2_InitCapture(timer_handle_t *handle, drv_callback_t callback);

/**
 * @brief  Get last capture value
 * @param  handle  Pointer to timer handle
 * @return Captured value
 */
uint16_t DRV_Timer2_GetCapture(timer_handle_t *handle);

/*---------------------------------------------------------------------------
 * Wakeup Timer Functions
 *---------------------------------------------------------------------------*/

/**
 * @brief  Initialize Wakeup Timer
 * @param  handle    Pointer to timer handle
 * @param  period_ms Wakeup period in milliseconds
 * @param  callback  Wakeup callback
 * @return DRV_OK on success
 */
drv_status_t DRV_WKT_Init(timer_handle_t *handle, uint32_t period_ms, 
                          drv_callback_t callback);

/**
 * @brief  Start Wakeup Timer
 * @param  handle  Pointer to timer handle
 * @return DRV_OK on success
 */
drv_status_t DRV_WKT_Start(timer_handle_t *handle);

/**
 * @brief  Stop Wakeup Timer
 * @param  handle  Pointer to timer handle
 * @return DRV_OK on success
 */
drv_status_t DRV_WKT_Stop(timer_handle_t *handle);

/*---------------------------------------------------------------------------
 * Delay Functions (using Timer)
 *---------------------------------------------------------------------------*/

/**
 * @brief  Blocking delay in microseconds
 * @param  us  Delay in microseconds
 * 
 * @note Uses Timer0 for timing. Accuracy depends on system clock.
 */
void DRV_Timer_DelayUs(uint16_t us);

/**
 * @brief  Blocking delay in milliseconds
 * @param  ms  Delay in milliseconds
 */
void DRV_Timer_DelayMs(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif /* DRV_TIMER_H */
