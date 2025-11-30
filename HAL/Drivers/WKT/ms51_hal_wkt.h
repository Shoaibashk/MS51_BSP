/**
 *******************************************************************************
 * @file    ms51_hal_wkt.h
 * @brief   MS51 HAL Wake-up Timer (WKT) Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details Wake-up Timer Hardware Abstraction Layer for MS51 series.
 *          Provides periodic wake-up from power-down mode.
 *
 * Features:
 *   - Configurable wake-up interval (ms to seconds)
 *   - Multiple clock divider options
 *   - Auto-reload capability
 *   - Interrupt support
 *   - Works in power-down mode
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_WKT_H
#define __MS51_HAL_WKT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * WKT Clock Divider Definitions
 ******************************************************************************/

/**
 * @brief  WKT Clock Divider enumeration
 * @note   Wake-up period = (256 - RWK) * Divider / 10000 seconds
 */
typedef enum {
    HAL_WKT_DIV_1    = 0,   /**< Divider = 1 (100us per tick @ 10kHz LIRC) */
    HAL_WKT_DIV_4    = 1,   /**< Divider = 4 (400us per tick) */
    HAL_WKT_DIV_16   = 2,   /**< Divider = 16 (1.6ms per tick) */
    HAL_WKT_DIV_64   = 3,   /**< Divider = 64 (6.4ms per tick) */
    HAL_WKT_DIV_256  = 4,   /**< Divider = 256 (25.6ms per tick) */
    HAL_WKT_DIV_512  = 5,   /**< Divider = 512 (51.2ms per tick) */
    HAL_WKT_DIV_1024 = 6,   /**< Divider = 1024 (102.4ms per tick) */
    HAL_WKT_DIV_2048 = 7    /**< Divider = 2048 (204.8ms per tick) */
} HAL_WKT_DivTypeDef;

/*******************************************************************************
 * WKT State Definitions
 ******************************************************************************/

/**
 * @brief  WKT State enumeration
 */
typedef enum {
    HAL_WKT_STATE_DISABLE = 0,  /**< WKT disabled */
    HAL_WKT_STATE_ENABLE  = 1   /**< WKT enabled */
} HAL_WKT_StateTypeDef;

/*******************************************************************************
 * WKT Initialization Structure
 ******************************************************************************/

/**
 * @brief  WKT Initialization Structure
 */
typedef struct {
    HAL_WKT_DivTypeDef  Divider;        /**< WKT clock divider */
    uint8_t             ReloadValue;    /**< WKT reload value (0-255) */
    uint8_t             EnableInterrupt; /**< Enable WKT interrupt */
} HAL_WKT_InitTypeDef;

/*******************************************************************************
 * WKT Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize WKT with specified configuration
 * @param  init Pointer to WKT initialization structure
 * @retval HAL_StatusTypeDef HAL status
 *
 * @note   Wake-up time calculation:
 *         Time = (256 - ReloadValue) * DividerValue / 10000 seconds
 *
 * @example
 *   // Configure WKT for ~1 second wake-up
 *   HAL_WKT_InitTypeDef wkt_init;
 *   wkt_init.Divider = HAL_WKT_DIV_2048;
 *   wkt_init.ReloadValue = 256 - 5;  // 5 ticks * 204.8ms = ~1 second
 *   wkt_init.EnableInterrupt = 1;
 *   HAL_WKT_Init(&wkt_init);
 */
HAL_StatusTypeDef HAL_WKT_Init(HAL_WKT_InitTypeDef *init);

/**
 * @brief  De-Initialize WKT (disable WKT)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_WKT_DeInit(void);

/**
 * @brief  Start WKT
 * @retval None
 */
void HAL_WKT_Start(void);

/**
 * @brief  Stop WKT
 * @retval None
 */
void HAL_WKT_Stop(void);

/**
 * @brief  Set WKT reload value
 * @param  value Reload value (0-255)
 * @retval None
 */
void HAL_WKT_SetReload(uint8_t value);

/**
 * @brief  Set WKT clock divider
 * @param  divider Divider value
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_WKT_SetDivider(HAL_WKT_DivTypeDef divider);

/**
 * @brief  Enable WKT interrupt
 * @retval None
 */
void HAL_WKT_EnableInterrupt(void);

/**
 * @brief  Disable WKT interrupt
 * @retval None
 */
void HAL_WKT_DisableInterrupt(void);

/**
 * @brief  Get WKT interrupt flag
 * @retval uint8_t 1 if flag set, 0 otherwise
 */
uint8_t HAL_WKT_GetFlag(void);

/**
 * @brief  Clear WKT interrupt flag
 * @retval None
 */
void HAL_WKT_ClearFlag(void);

/**
 * @brief  Configure WKT for millisecond-level wake-up
 * @param  ms Wake-up time in milliseconds (10-1000)
 * @retval HAL_StatusTypeDef HAL status
 *
 * @note   For longer wake-up times, use HAL_WKT_ConfigSeconds()
 * @example
 *   HAL_WKT_ConfigMs(200);  // 200ms wake-up interval
 */
HAL_StatusTypeDef HAL_WKT_ConfigMs(uint16_t ms);

/**
 * @brief  Configure WKT for second-level wake-up
 * @param  seconds Wake-up time in seconds (1-50)
 * @retval HAL_StatusTypeDef HAL status
 *
 * @example
 *   HAL_WKT_ConfigSeconds(5);  // 5 second wake-up interval
 */
HAL_StatusTypeDef HAL_WKT_ConfigSeconds(uint8_t seconds);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_WKT_H */
