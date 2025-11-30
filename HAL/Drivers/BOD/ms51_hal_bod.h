/**
 *******************************************************************************
 * @file    ms51_hal_bod.h
 * @brief   MS51 HAL Brown-out Detector (BOD) Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details Brown-out Detector Hardware Abstraction Layer for MS51 series.
 *          Provides voltage monitoring and system reset/interrupt capabilities.
 *
 * Features:
 *   - Multiple voltage threshold levels (1.8V to 4.4V)
 *   - Reset or interrupt on brown-out event
 *   - Low-power BOD mode support
 *   - Configurable detection modes
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_BOD_H
#define __MS51_HAL_BOD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * BOD Voltage Level Definitions
 ******************************************************************************/

/**
 * @brief  BOD Voltage Threshold enumeration
 */
typedef enum {
    HAL_BOD_LEVEL_4V4 = 0x00,   /**< BOD threshold 4.4V */
    HAL_BOD_LEVEL_3V7 = 0x10,   /**< BOD threshold 3.7V */
    HAL_BOD_LEVEL_3V0 = 0x20,   /**< BOD threshold 3.0V */
    HAL_BOD_LEVEL_2V7 = 0x30,   /**< BOD threshold 2.7V */
    HAL_BOD_LEVEL_2V4 = 0x40,   /**< BOD threshold 2.4V */
    HAL_BOD_LEVEL_2V0 = 0x50,   /**< BOD threshold 2.0V */
    HAL_BOD_LEVEL_1V8 = 0x70    /**< BOD threshold 1.8V */
} HAL_BOD_LevelTypeDef;

/*******************************************************************************
 * BOD Reset Mode Definitions
 ******************************************************************************/

/**
 * @brief  BOD Reset Mode enumeration
 */
typedef enum {
    HAL_BOD_RESET_DISABLE = 0x00,   /**< BOD reset disabled */
    HAL_BOD_RESET_ENABLE  = 0x04    /**< BOD reset enabled */
} HAL_BOD_ResetTypeDef;

/*******************************************************************************
 * Low Power BOD Mode Definitions
 ******************************************************************************/

/**
 * @brief  Low Power BOD Mode enumeration
 */
typedef enum {
    HAL_BOD_LP_MODE_NORMAL = 0x00,  /**< Normal mode (BOD always on) */
    HAL_BOD_LP_MODE_1      = 0x02,  /**< Low power mode 1 (BOD every 1.6ms) */
    HAL_BOD_LP_MODE_2      = 0x04,  /**< Low power mode 2 (BOD every 6.4ms) */
    HAL_BOD_LP_MODE_3      = 0x06   /**< Low power mode 3 (BOD every 25.6ms) */
} HAL_BOD_LowPowerModeTypeDef;

/*******************************************************************************
 * BOD State Definitions
 ******************************************************************************/

/**
 * @brief  BOD State enumeration
 */
typedef enum {
    HAL_BOD_STATE_DISABLE = 0,      /**< BOD disabled */
    HAL_BOD_STATE_ENABLE  = 1       /**< BOD enabled */
} HAL_BOD_StateTypeDef;

/*******************************************************************************
 * BOD Initialization Structure
 ******************************************************************************/

/**
 * @brief  BOD Initialization Structure
 */
typedef struct {
    HAL_BOD_StateTypeDef        State;          /**< BOD enable state */
    HAL_BOD_LevelTypeDef        Level;          /**< BOD voltage threshold */
    HAL_BOD_ResetTypeDef        ResetMode;      /**< BOD reset mode */
    HAL_BOD_LowPowerModeTypeDef LowPowerMode;   /**< Low power BOD mode */
} HAL_BOD_InitTypeDef;

/*******************************************************************************
 * BOD Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize BOD with specified configuration
 * @param  init Pointer to BOD initialization structure
 * @retval HAL_StatusTypeDef HAL status
 *
 * @example
 *   HAL_BOD_InitTypeDef bod_init;
 *   bod_init.State = HAL_BOD_STATE_ENABLE;
 *   bod_init.Level = HAL_BOD_LEVEL_2V4;
 *   bod_init.ResetMode = HAL_BOD_RESET_ENABLE;
 *   bod_init.LowPowerMode = HAL_BOD_LP_MODE_NORMAL;
 *   HAL_BOD_Init(&bod_init);
 */
HAL_StatusTypeDef HAL_BOD_Init(HAL_BOD_InitTypeDef *init);

/**
 * @brief  De-Initialize BOD (disable BOD)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_BOD_DeInit(void);

/**
 * @brief  Enable BOD
 * @retval None
 */
void HAL_BOD_Enable(void);

/**
 * @brief  Disable BOD
 * @retval None
 */
void HAL_BOD_Disable(void);

/**
 * @brief  Set BOD voltage threshold level
 * @param  level BOD voltage level
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_BOD_SetLevel(HAL_BOD_LevelTypeDef level);

/**
 * @brief  Get current BOD voltage level setting
 * @retval HAL_BOD_LevelTypeDef Current BOD level
 */
HAL_BOD_LevelTypeDef HAL_BOD_GetLevel(void);

/**
 * @brief  Enable BOD reset function
 * @retval None
 */
void HAL_BOD_EnableReset(void);

/**
 * @brief  Disable BOD reset function
 * @retval None
 */
void HAL_BOD_DisableReset(void);

/**
 * @brief  Configure Low Power BOD mode
 * @param  mode Low power mode selection
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_BOD_SetLowPowerMode(HAL_BOD_LowPowerModeTypeDef mode);

/**
 * @brief  Enable BOD interrupt
 * @retval None
 */
void HAL_BOD_EnableInterrupt(void);

/**
 * @brief  Disable BOD interrupt
 * @retval None
 */
void HAL_BOD_DisableInterrupt(void);

/**
 * @brief  Get BOD status flag
 * @retval uint8_t BOD status (1 = brown-out detected, 0 = normal)
 */
uint8_t HAL_BOD_GetStatus(void);

/**
 * @brief  Clear BOD flag
 * @retval None
 */
void HAL_BOD_ClearFlag(void);

/**
 * @brief  Check if last reset was caused by BOD
 * @retval uint8_t 1 if BOD reset occurred, 0 otherwise
 */
uint8_t HAL_BOD_WasResetSource(void);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_BOD_H */
