/*===========================================================================
 * @file    drv_adc.h
 * @brief   ADC Driver Interface
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details 12-bit ADC driver for MS51 microcontrollers.
 *          Supports single conversion, continuous mode, and various triggers.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_ADC_H
#define DRV_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * ADC Channel Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    ADC_CH_0  = 0,      /**< Channel 0 (AIN0) */
    ADC_CH_1  = 1,      /**< Channel 1 (AIN1) */
    ADC_CH_2  = 2,      /**< Channel 2 (AIN2) */
    ADC_CH_3  = 3,      /**< Channel 3 (AIN3) */
    ADC_CH_4  = 4,      /**< Channel 4 (AIN4) */
    ADC_CH_5  = 5,      /**< Channel 5 (AIN5) */
    ADC_CH_6  = 6,      /**< Channel 6 (AIN6) */
    ADC_CH_7  = 7,      /**< Channel 7 (AIN7) */
    ADC_CH_BG = 8,      /**< Bandgap voltage reference */
    ADC_CH_MAX
} adc_channel_t;

/*---------------------------------------------------------------------------
 * ADC Trigger Source
 *---------------------------------------------------------------------------*/
typedef enum {
    ADC_TRIG_SW   = 0,      /**< Software trigger */
    ADC_TRIG_PWM  = 1,      /**< PWM trigger */
    ADC_TRIG_GPIO = 2       /**< GPIO trigger (STADC pin) */
} adc_trigger_t;

/*---------------------------------------------------------------------------
 * ADC Clock Divider
 *---------------------------------------------------------------------------*/
typedef enum {
    ADC_CLK_DIV_1   = 0,    /**< Fsys / 1 */
    ADC_CLK_DIV_2   = 1,    /**< Fsys / 2 */
    ADC_CLK_DIV_4   = 2,    /**< Fsys / 4 */
    ADC_CLK_DIV_8   = 3,    /**< Fsys / 8 */
    ADC_CLK_DIV_16  = 4,    /**< Fsys / 16 */
    ADC_CLK_DIV_32  = 5,    /**< Fsys / 32 */
    ADC_CLK_DIV_64  = 6,    /**< Fsys / 64 */
    ADC_CLK_DIV_128 = 7     /**< Fsys / 128 */
} adc_clock_div_t;

/*---------------------------------------------------------------------------
 * ADC Reference Voltage
 *---------------------------------------------------------------------------*/
typedef enum {
    ADC_REF_VDD    = 0,     /**< VDD as reference */
    ADC_REF_AREF   = 1      /**< AREF pin as reference */
} adc_reference_t;

/*---------------------------------------------------------------------------
 * ADC Mode
 *---------------------------------------------------------------------------*/
typedef enum {
    ADC_MODE_SINGLE     = 0,    /**< Single conversion */
    ADC_MODE_CONTINUOUS = 1     /**< Continuous conversion */
} adc_mode_t;

/*---------------------------------------------------------------------------
 * ADC Configuration Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    adc_channel_t   channel;        /**< Input channel */
    adc_trigger_t   trigger;        /**< Trigger source */
    adc_clock_div_t clock_div;      /**< Clock divider */
    adc_reference_t reference;      /**< Reference voltage */
    adc_mode_t      mode;           /**< Conversion mode */
    uint8_t         enable_int;     /**< Enable interrupt */
    drv_callback_t  callback;       /**< Conversion complete callback */
} adc_config_t;

/*---------------------------------------------------------------------------
 * ADC Compare Configuration
 *---------------------------------------------------------------------------*/
typedef struct {
    uint16_t        threshold;      /**< Compare threshold (12-bit) */
    uint8_t         above;          /**< 1: interrupt if above, 0: below */
    drv_callback_t  callback;       /**< Compare match callback */
} adc_compare_config_t;

/*---------------------------------------------------------------------------
 * ADC Events
 *---------------------------------------------------------------------------*/
typedef enum {
    ADC_EVT_COMPLETE = 0,       /**< Conversion complete */
    ADC_EVT_COMPARE  = 1        /**< Compare threshold crossed */
} adc_event_t;

/*---------------------------------------------------------------------------
 * ADC Handle Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    adc_channel_t    channel;       /**< Current channel */
    drv_callback_t   callback;      /**< Event callback */
    drv_callback_t   cmp_callback;  /**< Compare callback */
    volatile uint16_t result;       /**< Last conversion result */
    volatile uint8_t  busy;         /**< Conversion in progress */
} adc_handle_t;

/*===========================================================================
 * Default Configuration Macros
 *===========================================================================*/

/**
 * @brief Default ADC configuration
 */
#define ADC_DEFAULT_CONFIG {                \
    .channel    = ADC_CH_0,                 \
    .trigger    = ADC_TRIG_SW,              \
    .clock_div  = ADC_CLK_DIV_8,            \
    .reference  = ADC_REF_VDD,              \
    .mode       = ADC_MODE_SINGLE,          \
    .enable_int = 0,                        \
    .callback   = NULL                      \
}

/*===========================================================================
 * Public API Functions
 *===========================================================================*/

/**
 * @brief  Initialize ADC peripheral
 * @param  handle  Pointer to ADC handle
 * @param  config  Pointer to ADC configuration
 * @return DRV_OK on success, error code otherwise
 * 
 * @code
 *   adc_handle_t adc;
 *   adc_config_t config = ADC_DEFAULT_CONFIG;
 *   config.channel = ADC_CH_0;
 *   DRV_ADC_Init(&adc, &config);
 * @endcode
 */
drv_status_t DRV_ADC_Init(adc_handle_t *handle, const adc_config_t *config);

/**
 * @brief  De-initialize ADC peripheral
 * @param  handle  Pointer to ADC handle
 * @return DRV_OK on success
 */
drv_status_t DRV_ADC_DeInit(adc_handle_t *handle);

/**
 * @brief  Set ADC input channel
 * @param  handle   Pointer to ADC handle
 * @param  channel  ADC channel
 * @return DRV_OK on success
 */
drv_status_t DRV_ADC_SetChannel(adc_handle_t *handle, adc_channel_t channel);

/**
 * @brief  Start ADC conversion (blocking)
 * @param  handle  Pointer to ADC handle
 * @param  result  Pointer to store result (12-bit)
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t DRV_ADC_Convert(adc_handle_t *handle, uint16_t *result);

/**
 * @brief  Start ADC conversion (non-blocking, interrupt)
 * @param  handle  Pointer to ADC handle
 * @return DRV_OK on success, DRV_BUSY if busy
 */
drv_status_t DRV_ADC_StartConvert(adc_handle_t *handle);

/**
 * @brief  Get last conversion result
 * @param  handle  Pointer to ADC handle
 * @return 12-bit ADC result
 */
uint16_t DRV_ADC_GetResult(adc_handle_t *handle);

/**
 * @brief  Check if conversion is complete
 * @param  handle  Pointer to ADC handle
 * @return 1 if complete, 0 if busy
 */
uint8_t DRV_ADC_IsComplete(adc_handle_t *handle);

/**
 * @brief  Configure ADC compare function
 * @param  handle  Pointer to ADC handle
 * @param  config  Pointer to compare configuration
 * @return DRV_OK on success
 */
drv_status_t DRV_ADC_ConfigCompare(adc_handle_t *handle, 
                                    const adc_compare_config_t *config);

/**
 * @brief  Enable ADC compare function
 * @param  handle  Pointer to ADC handle
 * @return DRV_OK on success
 */
drv_status_t DRV_ADC_EnableCompare(adc_handle_t *handle);

/**
 * @brief  Disable ADC compare function
 * @param  handle  Pointer to ADC handle
 * @return DRV_OK on success
 */
drv_status_t DRV_ADC_DisableCompare(adc_handle_t *handle);

/**
 * @brief  Register ADC callback
 * @param  handle    Pointer to ADC handle
 * @param  callback  Callback function
 * @return DRV_OK on success
 */
drv_status_t DRV_ADC_RegisterCallback(adc_handle_t *handle, drv_callback_t callback);

/*---------------------------------------------------------------------------
 * Bandgap / VDD Measurement
 *---------------------------------------------------------------------------*/

/**
 * @brief  Read bandgap voltage (internal reference)
 * @param  handle  Pointer to ADC handle
 * @return Bandgap ADC reading (12-bit)
 */
uint16_t DRV_ADC_ReadBandgap(adc_handle_t *handle);

/**
 * @brief  Calculate VDD voltage from bandgap
 * @param  bandgap_reading  Bandgap ADC reading
 * @return VDD voltage in millivolts
 * 
 * @note Formula: VDD = (Bandgap_Voltage * 4096) / bandgap_reading
 *       Bandgap is typically 1.22V
 */
uint16_t DRV_ADC_CalculateVDD(uint16_t bandgap_reading);

/**
 * @brief  Convert ADC reading to millivolts
 * @param  adc_value  12-bit ADC reading
 * @param  vdd_mv     VDD voltage in millivolts
 * @return Voltage in millivolts
 */
uint16_t DRV_ADC_ToMillivolts(uint16_t adc_value, uint16_t vdd_mv);

#ifdef __cplusplus
}
#endif

#endif /* DRV_ADC_H */
