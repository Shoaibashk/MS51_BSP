/**
 *******************************************************************************
 * @file    ms51_hal_wdt.h
 * @brief   MS51 HAL Watchdog Timer Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details Watchdog Timer HAL for MS51 series.
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_WDT_H
#define __MS51_HAL_WDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * WDT Timeout Period Definitions
 ******************************************************************************/

/**
 * @brief  WDT Timeout Period
 * @note   Timeout = WDT_period * (LIRC period) where LIRC ~= 10kHz
 *         WDT periods: 2^(4+n) * LIRC cycles
 */
typedef enum {
    HAL_WDT_TIMEOUT_2POW4  = 0,     /**< 2^4 = 16 LIRC cycles (~1.6ms) */
    HAL_WDT_TIMEOUT_2POW5  = 1,     /**< 2^5 = 32 LIRC cycles (~3.2ms) */
    HAL_WDT_TIMEOUT_2POW6  = 2,     /**< 2^6 = 64 LIRC cycles (~6.4ms) */
    HAL_WDT_TIMEOUT_2POW7  = 3,     /**< 2^7 = 128 LIRC cycles (~12.8ms) */
    HAL_WDT_TIMEOUT_2POW8  = 4,     /**< 2^8 = 256 LIRC cycles (~25.6ms) */
    HAL_WDT_TIMEOUT_2POW9  = 5,     /**< 2^9 = 512 LIRC cycles (~51.2ms) */
    HAL_WDT_TIMEOUT_2POW10 = 6,     /**< 2^10 = 1024 LIRC cycles (~102.4ms) */
    HAL_WDT_TIMEOUT_2POW11 = 7      /**< 2^11 = 2048 LIRC cycles (~204.8ms) */
} HAL_WDT_TimeoutTypeDef;

/*******************************************************************************
 * WDT Initialization Structure
 ******************************************************************************/

/**
 * @brief  WDT Initialization Structure
 */
typedef struct {
    HAL_WDT_TimeoutTypeDef  Timeout;        /**< Watchdog timeout period */
    uint8_t                 ResetEnable;    /**< Enable reset on timeout */
    uint8_t                 IdleEnable;     /**< Run WDT in idle mode */
    uint8_t                 PowerDownEnable; /**< Run WDT in power-down mode */
} HAL_WDT_InitTypeDef;

/*******************************************************************************
 * WDT Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize Watchdog Timer
 * @param  init Pointer to WDT initialization structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_WDT_InitTypeDef wdt_init;
 *   wdt_init.Timeout = HAL_WDT_TIMEOUT_2POW10;  // ~100ms
 *   wdt_init.ResetEnable = HAL_ENABLE;
 *   wdt_init.IdleEnable = HAL_DISABLE;
 *   wdt_init.PowerDownEnable = HAL_DISABLE;
 *   HAL_WDT_Init(&wdt_init);
 */
HAL_StatusTypeDef HAL_WDT_Init(HAL_WDT_InitTypeDef *init);

/**
 * @brief  Start Watchdog Timer
 * @retval None
 */
void HAL_WDT_Start(void);

/**
 * @brief  Stop Watchdog Timer
 * @retval None
 * @note   WDT cannot be stopped once started if configured via CONFIG bits
 */
void HAL_WDT_Stop(void);

/**
 * @brief  Refresh (feed) the Watchdog Timer
 * @retval None
 * 
 * @note   Call this function periodically to prevent WDT reset
 * 
 * @example
 *   while (1) {
 *       // Main loop code
 *       HAL_WDT_Refresh();
 *   }
 */
void HAL_WDT_Refresh(void);

/**
 * @brief  Clear Watchdog Timer counter
 * @retval None
 */
void HAL_WDT_Clear(void);

/**
 * @brief  Check if WDT reset occurred
 * @retval uint8_t 1 if WDT reset occurred, 0 otherwise
 */
uint8_t HAL_WDT_GetResetFlag(void);

/**
 * @brief  Clear WDT reset flag
 * @retval None
 */
void HAL_WDT_ClearResetFlag(void);

/**
 * @brief  Get WDT timeout flag
 * @retval uint8_t 1 if timeout occurred, 0 otherwise
 */
uint8_t HAL_WDT_GetTimeoutFlag(void);

/**
 * @brief  Clear WDT timeout flag
 * @retval None
 */
void HAL_WDT_ClearTimeoutFlag(void);

/**
 * @brief  Enable WDT interrupt
 * @retval None
 */
void HAL_WDT_EnableInterrupt(void);

/**
 * @brief  Disable WDT interrupt
 * @retval None
 */
void HAL_WDT_DisableInterrupt(void);

/**
 * @brief  Simple WDT initialization with default settings
 * @param  timeout WDT timeout period
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_WDT_SimpleInit(HAL_WDT_TIMEOUT_2POW10);  // ~100ms timeout
 *   HAL_WDT_Start();
 */
HAL_StatusTypeDef HAL_WDT_SimpleInit(HAL_WDT_TimeoutTypeDef timeout);

/*******************************************************************************
 * WDT Callback
 ******************************************************************************/

/**
 * @brief  WDT timeout callback (before reset)
 * @note   Called from WDT ISR if interrupt is enabled
 */
void HAL_WDT_TimeoutCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_WDT_H */
