/**
 *******************************************************************************
 * @file    ms51_hal_pwm.h
 * @brief   MS51 HAL PWM Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details PWM Hardware Abstraction Layer for MS51 series.
 *          6-channel PWM with complementary outputs and dead-time.
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_PWM_H
#define __MS51_HAL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * PWM Channel Definitions
 ******************************************************************************/

/**
 * @brief  PWM Channel enumeration
 */
typedef enum {
    HAL_PWM_CHANNEL_0 = 0,      /**< PWM Channel 0 */
    HAL_PWM_CHANNEL_1 = 1,      /**< PWM Channel 1 */
    HAL_PWM_CHANNEL_2 = 2,      /**< PWM Channel 2 */
    HAL_PWM_CHANNEL_3 = 3,      /**< PWM Channel 3 */
    HAL_PWM_CHANNEL_4 = 4,      /**< PWM Channel 4 */
    HAL_PWM_CHANNEL_5 = 5       /**< PWM Channel 5 */
} HAL_PWM_ChannelTypeDef;

/*******************************************************************************
 * PWM Mode Definitions
 ******************************************************************************/

/**
 * @brief  PWM Operating Mode
 */
typedef enum {
    HAL_PWM_MODE_INDEPENDENT = 0,   /**< Independent mode (6 channels) */
    HAL_PWM_MODE_COMPLEMENTARY = 1, /**< Complementary mode (3 pairs) */
    HAL_PWM_MODE_SYNCHRONIZED = 2,  /**< Synchronized mode */
    HAL_PWM_MODE_GROUPED = 3        /**< Grouped mode */
} HAL_PWM_ModeTypeDef;

/**
 * @brief  PWM Output Type
 */
typedef enum {
    HAL_PWM_TYPE_EDGE = 0,          /**< Edge-aligned PWM */
    HAL_PWM_TYPE_CENTER = 1         /**< Center-aligned PWM */
} HAL_PWM_TypeTypeDef;

/**
 * @brief  PWM Clock Source
 */
typedef enum {
    HAL_PWM_CLK_FSYS = 0,           /**< Fsys as clock source */
    HAL_PWM_CLK_TIMER1 = 1          /**< Timer1 overflow as clock */
} HAL_PWM_ClkSrcTypeDef;

/**
 * @brief  PWM Clock Divider
 */
typedef enum {
    HAL_PWM_DIV_1   = 0,            /**< PWM clock / 1 */
    HAL_PWM_DIV_2   = 1,            /**< PWM clock / 2 */
    HAL_PWM_DIV_4   = 2,            /**< PWM clock / 4 */
    HAL_PWM_DIV_8   = 3,            /**< PWM clock / 8 */
    HAL_PWM_DIV_16  = 4,            /**< PWM clock / 16 */
    HAL_PWM_DIV_32  = 5,            /**< PWM clock / 32 */
    HAL_PWM_DIV_64  = 6,            /**< PWM clock / 64 */
    HAL_PWM_DIV_128 = 7             /**< PWM clock / 128 */
} HAL_PWM_DivTypeDef;

/*******************************************************************************
 * PWM Initialization Structures
 ******************************************************************************/

/**
 * @brief  PWM Initialization Structure
 */
typedef struct {
    HAL_PWM_ModeTypeDef     Mode;           /**< PWM mode */
    HAL_PWM_TypeTypeDef     Type;           /**< PWM type (edge/center) */
    HAL_PWM_ClkSrcTypeDef   ClockSource;    /**< Clock source */
    HAL_PWM_DivTypeDef      ClockDiv;       /**< Clock divider */
    uint16_t                Period;         /**< PWM period (12-bit) */
} HAL_PWM_InitTypeDef;

/**
 * @brief  PWM Channel Configuration Structure
 */
typedef struct {
    HAL_PWM_ChannelTypeDef  Channel;        /**< PWM channel */
    uint16_t                DutyCycle;      /**< Duty cycle (12-bit) */
    uint8_t                 OutputEnable;   /**< Enable output */
    uint8_t                 Polarity;       /**< Output polarity (0=normal, 1=inverted) */
} HAL_PWM_ChannelInitTypeDef;

/**
 * @brief  PWM Handle Structure
 */
typedef struct {
    HAL_PWM_InitTypeDef     Init;           /**< PWM configuration */
    HAL_LockTypeDef         Lock;           /**< Lock object */
} HAL_PWM_HandleTypeDef;

/*******************************************************************************
 * PWM Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize PWM peripheral
 * @param  hpwm Pointer to PWM handle structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_Init(HAL_PWM_HandleTypeDef *hpwm);

/**
 * @brief  De-initialize PWM peripheral
 * @param  hpwm Pointer to PWM handle structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_DeInit(HAL_PWM_HandleTypeDef *hpwm);

/**
 * @brief  Simple PWM initialization
 * @param  clockDiv Clock divider
 * @param  period PWM period (12-bit value)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   // Initialize PWM with 1kHz frequency at 24MHz system clock
 *   // Period = 24MHz / 1 / 1kHz = 24000 (but max is 4095)
 *   // Use divider /8: 24MHz / 8 / 1kHz = 3000
 *   HAL_PWM_SimpleInit(HAL_PWM_DIV_8, 3000);
 */
HAL_StatusTypeDef HAL_PWM_SimpleInit(HAL_PWM_DivTypeDef clockDiv, uint16_t period);

/**
 * @brief  Configure PWM channel
 * @param  config Pointer to channel configuration structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_ConfigChannel(HAL_PWM_ChannelInitTypeDef *config);

/**
 * @brief  Start PWM
 * @retval None
 */
void HAL_PWM_Start(void);

/**
 * @brief  Stop PWM
 * @retval None
 */
void HAL_PWM_Stop(void);

/**
 * @brief  Set PWM period
 * @param  period PWM period (12-bit value)
 * @retval None
 */
void HAL_PWM_SetPeriod(uint16_t period);

/**
 * @brief  Set PWM duty cycle for a channel
 * @param  channel PWM channel
 * @param  duty Duty cycle (12-bit value)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   // Set 50% duty cycle (assuming period = 1000)
 *   HAL_PWM_SetDuty(HAL_PWM_CHANNEL_0, 500);
 */
HAL_StatusTypeDef HAL_PWM_SetDuty(HAL_PWM_ChannelTypeDef channel, uint16_t duty);

/**
 * @brief  Set PWM duty cycle as percentage
 * @param  channel PWM channel
 * @param  percent Duty cycle in percent (0-100)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_PWM_SetDutyPercent(HAL_PWM_CHANNEL_0, 75);  // 75% duty
 */
HAL_StatusTypeDef HAL_PWM_SetDutyPercent(HAL_PWM_ChannelTypeDef channel, 
                                          uint8_t percent);

/**
 * @brief  Enable PWM channel output
 * @param  channel PWM channel
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_EnableOutput(HAL_PWM_ChannelTypeDef channel);

/**
 * @brief  Disable PWM channel output
 * @param  channel PWM channel
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_DisableOutput(HAL_PWM_ChannelTypeDef channel);

/**
 * @brief  Set PWM output polarity
 * @param  channel PWM channel
 * @param  polarity 0 = normal, 1 = inverted
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_SetPolarity(HAL_PWM_ChannelTypeDef channel, 
                                       uint8_t polarity);

/**
 * @brief  Enable dead-time for complementary channels
 * @param  pair Channel pair (0=CH0/CH1, 1=CH2/CH3, 2=CH4/CH5)
 * @param  deadtime Dead-time value
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_SetDeadTime(uint8_t pair, uint8_t deadtime);

/**
 * @brief  Load new PWM values
 * @note   Call after updating duty cycles for synchronized update
 * @retval None
 */
void HAL_PWM_LoadValues(void);

/**
 * @brief  Enable PWM interrupt
 * @retval None
 */
void HAL_PWM_EnableInterrupt(void);

/**
 * @brief  Disable PWM interrupt
 * @retval None
 */
void HAL_PWM_DisableInterrupt(void);

/**
 * @brief  Get PWM interrupt flag
 * @retval uint8_t 1 if flag set, 0 otherwise
 */
uint8_t HAL_PWM_GetFlag(void);

/**
 * @brief  Clear PWM interrupt flag
 * @retval None
 */
void HAL_PWM_ClearFlag(void);

/**
 * @brief  Configure fault brake function
 * @param  enable Enable/disable fault brake
 * @param  source Fault brake source
 * @param  level Fault brake trigger level
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_PWM_ConfigFaultBrake(uint8_t enable, uint8_t source, 
                                            uint8_t level);

/*******************************************************************************
 * PWM Callback
 ******************************************************************************/

/**
 * @brief  PWM period complete callback
 */
void HAL_PWM_PeriodElapsedCallback(HAL_PWM_HandleTypeDef *hpwm);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_PWM_H */
