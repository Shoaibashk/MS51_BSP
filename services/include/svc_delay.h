/*===========================================================================
 * @file    svc_delay.h
 * @brief   Delay Service Interface
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Provides blocking and non-blocking delay functions using
 *          hardware timers or software loops.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef SVC_DELAY_H
#define SVC_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Delay Configuration
 *---------------------------------------------------------------------------*/
typedef enum {
    DELAY_TIMER_NONE = 0,   /**< Use software loop (no timer) */
    DELAY_TIMER_0    = 1,   /**< Use Timer 0 */
    DELAY_TIMER_1    = 2,   /**< Use Timer 1 */
    DELAY_TIMER_2    = 3,   /**< Use Timer 2 */
    DELAY_TIMER_3    = 4    /**< Use Timer 3 */
} delay_timer_t;

typedef struct {
    delay_timer_t timer;        /**< Timer to use for delays */
    uint32_t      sys_clock;    /**< System clock frequency (Hz) */
} delay_config_t;

/*---------------------------------------------------------------------------
 * System Tick Support
 *---------------------------------------------------------------------------*/
/**
 * @brief  Initialize delay service
 * @param  config  Delay configuration
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t SVC_DELAY_Init(const delay_config_t *config);

/**
 * @brief  De-initialize delay service
 * @return DRV_OK on success
 */
drv_status_t SVC_DELAY_DeInit(void);

/**
 * @brief  Get system tick count (1ms resolution)
 * @return Current tick count in milliseconds
 */
uint32_t SVC_DELAY_GetTick(void);

/**
 * @brief  Set system tick count
 * @param  tick  New tick value
 */
void SVC_DELAY_SetTick(uint32_t tick);

/*---------------------------------------------------------------------------
 * Blocking Delays
 *---------------------------------------------------------------------------*/
/**
 * @brief  Delay for specified microseconds
 * @param  us  Microseconds to delay
 * @note   Accuracy depends on system clock and implementation
 */
void SVC_DELAY_Us(uint16_t us);

/**
 * @brief  Delay for specified milliseconds
 * @param  ms  Milliseconds to delay
 */
void SVC_DELAY_Ms(uint16_t ms);

/**
 * @brief  Delay for specified seconds
 * @param  sec  Seconds to delay
 */
void SVC_DELAY_Sec(uint16_t sec);

/*---------------------------------------------------------------------------
 * Non-Blocking Timeout Support
 *---------------------------------------------------------------------------*/
/**
 * @brief  Check if timeout has elapsed since start tick
 * @param  start_tick  Starting tick value from SVC_DELAY_GetTick()
 * @param  timeout_ms  Timeout duration in milliseconds
 * @return true if timeout elapsed, false otherwise
 */
uint8_t SVC_DELAY_IsTimeout(uint32_t start_tick, uint32_t timeout_ms);

/**
 * @brief  Get elapsed time since start tick
 * @param  start_tick  Starting tick value
 * @return Elapsed time in milliseconds
 */
uint32_t SVC_DELAY_GetElapsed(uint32_t start_tick);

/*---------------------------------------------------------------------------
 * Simple Software Delays (No Timer Required)
 *---------------------------------------------------------------------------*/
/**
 * @brief  Simple NOP-based microsecond delay
 * @param  us  Microseconds to delay (approximate)
 * @note   Less accurate than timer-based delays
 */
void SVC_DELAY_SoftUs(uint16_t us);

/**
 * @brief  Simple loop-based millisecond delay  
 * @param  ms  Milliseconds to delay (approximate)
 */
void SVC_DELAY_SoftMs(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif /* SVC_DELAY_H */
