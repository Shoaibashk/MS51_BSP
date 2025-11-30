/*===========================================================================
 * @file    drv_pwm.h
 * @brief   PWM Driver Interface
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details 6-channel PWM driver for MS51 microcontrollers.
 *          Supports independent, complementary, and center-aligned modes.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_PWM_H
#define DRV_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * PWM Channel Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_CH_0  = 0,      /**< PWM channel 0 */
    PWM_CH_1  = 1,      /**< PWM channel 1 */
    PWM_CH_2  = 2,      /**< PWM channel 2 */
    PWM_CH_3  = 3,      /**< PWM channel 3 */
    PWM_CH_4  = 4,      /**< PWM channel 4 */
    PWM_CH_5  = 5,      /**< PWM channel 5 */
    PWM_CH_ALL = 0xFF   /**< All channels */
} pwm_channel_t;

/*---------------------------------------------------------------------------
 * PWM Mode
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_MODE_INDEPENDENT    = 0,    /**< Independent mode */
    PWM_MODE_COMPLEMENTARY  = 1,    /**< Complementary mode with dead time */
    PWM_MODE_SYNCHRONIZED   = 2     /**< Synchronized output */
} pwm_mode_t;

/*---------------------------------------------------------------------------
 * PWM Alignment
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_ALIGN_EDGE   = 0,       /**< Edge-aligned (up counter) */
    PWM_ALIGN_CENTER = 1        /**< Center-aligned (up-down counter) */
} pwm_alignment_t;

/*---------------------------------------------------------------------------
 * PWM Output Polarity
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_POL_ACTIVE_HIGH = 0,    /**< Active high */
    PWM_POL_ACTIVE_LOW  = 1     /**< Active low (inverted) */
} pwm_polarity_t;

/*---------------------------------------------------------------------------
 * PWM Clock Source
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_CLK_FSYS   = 0,     /**< System clock */
    PWM_CLK_TIMER1 = 1      /**< Timer1 overflow */
} pwm_clock_t;

/*---------------------------------------------------------------------------
 * PWM Clock Prescaler
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_PSC_1   = 0,        /**< Prescaler 1:1 */
    PWM_PSC_2   = 1,        /**< Prescaler 1:2 */
    PWM_PSC_4   = 2,        /**< Prescaler 1:4 */
    PWM_PSC_8   = 3,        /**< Prescaler 1:8 */
    PWM_PSC_16  = 4,        /**< Prescaler 1:16 */
    PWM_PSC_32  = 5,        /**< Prescaler 1:32 */
    PWM_PSC_64  = 6,        /**< Prescaler 1:64 */
    PWM_PSC_128 = 7         /**< Prescaler 1:128 */
} pwm_prescaler_t;

/*---------------------------------------------------------------------------
 * PWM Configuration Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    pwm_mode_t      mode;           /**< PWM mode */
    pwm_alignment_t alignment;      /**< Alignment mode */
    pwm_clock_t     clock;          /**< Clock source */
    pwm_prescaler_t prescaler;      /**< Clock prescaler */
    uint16_t        period;         /**< Period value (PWMPH:PWMPL) */
    uint8_t         enable_int;     /**< Enable period interrupt */
    drv_callback_t  callback;       /**< Period interrupt callback */
} pwm_config_t;

/*---------------------------------------------------------------------------
 * PWM Channel Configuration
 *---------------------------------------------------------------------------*/
typedef struct {
    pwm_channel_t   channel;        /**< Channel number */
    uint16_t        duty;           /**< Duty cycle value */
    pwm_polarity_t  polarity;       /**< Output polarity */
    uint8_t         enable;         /**< Enable channel output */
} pwm_channel_config_t;

/*---------------------------------------------------------------------------
 * PWM Dead Time Configuration
 *---------------------------------------------------------------------------*/
typedef struct {
    uint8_t rising_edge;        /**< Dead time on rising edge (0-255) */
    uint8_t falling_edge;       /**< Dead time on falling edge (0-255) */
} pwm_deadtime_config_t;

/*---------------------------------------------------------------------------
 * PWM Events
 *---------------------------------------------------------------------------*/
typedef enum {
    PWM_EVT_PERIOD = 0,     /**< Period match (counter = 0) */
    PWM_EVT_DUTY   = 1,     /**< Duty match */
    PWM_EVT_FAULT  = 2      /**< Fault brake triggered */
} pwm_event_t;

/*---------------------------------------------------------------------------
 * PWM Handle Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    uint16_t        period;         /**< Current period */
    drv_callback_t  callback;       /**< Event callback */
    volatile uint8_t running;       /**< Running flag */
    uint8_t         enabled_ch;     /**< Enabled channels bitmask */
} pwm_handle_t;

/*===========================================================================
 * Default Configuration Macros
 *===========================================================================*/

/**
 * @brief Default PWM configuration (1kHz @ 24MHz)
 */
#define PWM_DEFAULT_CONFIG {                \
    .mode       = PWM_MODE_INDEPENDENT,     \
    .alignment  = PWM_ALIGN_EDGE,           \
    .clock      = PWM_CLK_FSYS,             \
    .prescaler  = PWM_PSC_1,                \
    .period     = 24000,                    \
    .enable_int = 0,                        \
    .callback   = NULL                      \
}

/*===========================================================================
 * Public API Functions
 *===========================================================================*/

/**
 * @brief  Initialize PWM peripheral
 * @param  handle  Pointer to PWM handle
 * @param  config  Pointer to PWM configuration
 * @return DRV_OK on success, error code otherwise
 * 
 * @code
 *   pwm_handle_t pwm;
 *   pwm_config_t config = PWM_DEFAULT_CONFIG;
 *   DRV_PWM_Init(&pwm, &config);
 * @endcode
 */
drv_status_t DRV_PWM_Init(pwm_handle_t *handle, const pwm_config_t *config);

/**
 * @brief  De-initialize PWM peripheral
 * @param  handle  Pointer to PWM handle
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_DeInit(pwm_handle_t *handle);

/**
 * @brief  Configure PWM channel
 * @param  handle  Pointer to PWM handle
 * @param  config  Pointer to channel configuration
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_ConfigChannel(pwm_handle_t *handle, 
                                    const pwm_channel_config_t *config);

/**
 * @brief  Set channel duty cycle
 * @param  handle   Pointer to PWM handle
 * @param  channel  PWM channel
 * @param  duty     Duty cycle value (0 to period)
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_SetDuty(pwm_handle_t *handle, pwm_channel_t channel, 
                              uint16_t duty);

/**
 * @brief  Set channel duty cycle as percentage
 * @param  handle   Pointer to PWM handle
 * @param  channel  PWM channel
 * @param  percent  Duty cycle percentage (0-100)
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_SetDutyPercent(pwm_handle_t *handle, pwm_channel_t channel, 
                                     uint8_t percent);

/**
 * @brief  Set PWM period
 * @param  handle  Pointer to PWM handle
 * @param  period  Period value
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_SetPeriod(pwm_handle_t *handle, uint16_t period);

/**
 * @brief  Start PWM output
 * @param  handle  Pointer to PWM handle
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_Start(pwm_handle_t *handle);

/**
 * @brief  Stop PWM output
 * @param  handle  Pointer to PWM handle
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_Stop(pwm_handle_t *handle);

/**
 * @brief  Enable PWM channel output
 * @param  handle   Pointer to PWM handle
 * @param  channel  PWM channel
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_EnableChannel(pwm_handle_t *handle, pwm_channel_t channel);

/**
 * @brief  Disable PWM channel output
 * @param  handle   Pointer to PWM handle
 * @param  channel  PWM channel
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_DisableChannel(pwm_handle_t *handle, pwm_channel_t channel);

/**
 * @brief  Configure dead time for complementary mode
 * @param  handle  Pointer to PWM handle
 * @param  config  Pointer to dead time configuration
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_ConfigDeadTime(pwm_handle_t *handle, 
                                     const pwm_deadtime_config_t *config);

/**
 * @brief  Enable fault brake
 * @param  handle  Pointer to PWM handle
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_EnableBrake(pwm_handle_t *handle);

/**
 * @brief  Disable fault brake
 * @param  handle  Pointer to PWM handle
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_DisableBrake(pwm_handle_t *handle);

/**
 * @brief  Trigger brake (software fault)
 * @param  handle  Pointer to PWM handle
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_TriggerBrake(pwm_handle_t *handle);

/**
 * @brief  Register PWM callback
 * @param  handle    Pointer to PWM handle
 * @param  callback  Callback function
 * @return DRV_OK on success
 */
drv_status_t DRV_PWM_RegisterCallback(pwm_handle_t *handle, drv_callback_t callback);

/*---------------------------------------------------------------------------
 * Helper Macros for Frequency Calculation
 *---------------------------------------------------------------------------*/

/**
 * @brief Calculate period value for desired frequency
 * @param fsys      System clock frequency in Hz
 * @param prescaler Prescaler value (1, 2, 4, 8, 16, 32, 64, 128)
 * @param freq_hz   Desired PWM frequency in Hz
 * @return Period value
 */
#define PWM_CALC_PERIOD(fsys, prescaler, freq_hz)  \
    ((fsys) / ((prescaler) * (freq_hz)))

/**
 * @brief Calculate duty value for percentage
 * @param period  Period value
 * @param percent Percentage (0-100)
 * @return Duty value
 */
#define PWM_CALC_DUTY(period, percent)  \
    (((uint32_t)(period) * (percent)) / 100)

#ifdef __cplusplus
}
#endif

#endif /* DRV_PWM_H */
