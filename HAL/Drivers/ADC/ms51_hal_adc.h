/**
 *******************************************************************************
 * @file    ms51_hal_adc.h
 * @brief   MS51 HAL ADC Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details ADC Hardware Abstraction Layer for MS51 series.
 *          12-bit SAR ADC with up to 8 channels.
 *
 * Features:
 *   - 8 external ADC channels (AIN0-AIN7)
 *   - Internal bandgap reference
 *   - Single and continuous conversion modes
 *   - Hardware trigger from PWM/STADC
 *   - Compare mode
 *   - Interrupt support
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_ADC_H
#define __MS51_HAL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * ADC Channel Definitions
 ******************************************************************************/

/**
 * @brief  ADC Channel enumeration
 */
typedef enum {
    HAL_ADC_CHANNEL_0  = 0,     /**< AIN0 - P0.7 */
    HAL_ADC_CHANNEL_1  = 1,     /**< AIN1 - P3.0 */
    HAL_ADC_CHANNEL_2  = 2,     /**< AIN2 - P0.6 */
    HAL_ADC_CHANNEL_3  = 3,     /**< AIN3 - P0.5 */
    HAL_ADC_CHANNEL_4  = 4,     /**< AIN4 - P0.4 */
    HAL_ADC_CHANNEL_5  = 5,     /**< AIN5 - P0.3 */
    HAL_ADC_CHANNEL_6  = 6,     /**< AIN6 - P1.1 */
    HAL_ADC_CHANNEL_7  = 7,     /**< AIN7 - P1.7 */
    HAL_ADC_CHANNEL_BANDGAP = 8 /**< Internal bandgap reference */
} HAL_ADC_ChannelTypeDef;

/*******************************************************************************
 * ADC Conversion Mode Definitions
 ******************************************************************************/

/**
 * @brief  ADC Conversion Mode
 */
typedef enum {
    HAL_ADC_MODE_SINGLE = 0,        /**< Single conversion mode */
    HAL_ADC_MODE_CONTINUOUS = 1     /**< Continuous conversion mode */
} HAL_ADC_ModeTypeDef;

/*******************************************************************************
 * ADC Trigger Source Definitions
 ******************************************************************************/

/**
 * @brief  ADC External Trigger Source
 */
typedef enum {
    HAL_ADC_TRIG_SOFTWARE = 0,      /**< Software trigger (ADCS bit) */
    HAL_ADC_TRIG_PWM_CH0  = 1,      /**< PWM0 CH0 trigger */
    HAL_ADC_TRIG_PWM_CH2  = 2,      /**< PWM0 CH2 trigger */
    HAL_ADC_TRIG_PWM_CH4  = 3,      /**< PWM0 CH4 trigger */
    HAL_ADC_TRIG_STADC    = 4       /**< External STADC pin trigger */
} HAL_ADC_TriggerTypeDef;

/**
 * @brief  ADC Trigger Type (for hardware trigger)
 */
typedef enum {
    HAL_ADC_TRIGTYPE_FALLING = 0,   /**< Falling edge trigger */
    HAL_ADC_TRIGTYPE_RISING = 1,    /**< Rising edge trigger */
    HAL_ADC_TRIGTYPE_CENTER = 2,    /**< PWM center trigger */
    HAL_ADC_TRIGTYPE_END = 3        /**< PWM period end trigger */
} HAL_ADC_TrigTypeTypeDef;

/*******************************************************************************
 * ADC Clock Configuration
 ******************************************************************************/

/**
 * @brief  ADC Clock Divider
 */
typedef enum {
    HAL_ADC_DIV_1   = 0,    /**< ADC clock = Fsys / 1 */
    HAL_ADC_DIV_2   = 1,    /**< ADC clock = Fsys / 2 */
    HAL_ADC_DIV_4   = 2,    /**< ADC clock = Fsys / 4 */
    HAL_ADC_DIV_8   = 3,    /**< ADC clock = Fsys / 8 */
    HAL_ADC_DIV_16  = 4,    /**< ADC clock = Fsys / 16 */
    HAL_ADC_DIV_32  = 5,    /**< ADC clock = Fsys / 32 */
    HAL_ADC_DIV_64  = 6,    /**< ADC clock = Fsys / 64 */
    HAL_ADC_DIV_128 = 7     /**< ADC clock = Fsys / 128 */
} HAL_ADC_ClkDivTypeDef;

/**
 * @brief  ADC Acquisition Time
 */
typedef enum {
    HAL_ADC_AQT_0 = 0,      /**< 6 ADC clocks */
    HAL_ADC_AQT_1 = 1,      /**< 10 ADC clocks */
    HAL_ADC_AQT_2 = 2,      /**< 14 ADC clocks */
    HAL_ADC_AQT_3 = 3,      /**< 18 ADC clocks */
    HAL_ADC_AQT_4 = 4,      /**< 22 ADC clocks */
    HAL_ADC_AQT_5 = 5,      /**< 26 ADC clocks */
    HAL_ADC_AQT_6 = 6,      /**< 30 ADC clocks */
    HAL_ADC_AQT_7 = 7       /**< 34 ADC clocks */
} HAL_ADC_AcqTimeTypeDef;

/*******************************************************************************
 * ADC Compare Mode
 ******************************************************************************/

/**
 * @brief  ADC Compare Mode
 */
typedef enum {
    HAL_ADC_CMP_DISABLE = 0,        /**< Compare disabled */
    HAL_ADC_CMP_LESS = 1,           /**< Interrupt if result < threshold */
    HAL_ADC_CMP_GREATER = 2         /**< Interrupt if result >= threshold */
} HAL_ADC_CompareModeTypeDef;

/*******************************************************************************
 * ADC Initialization Structure
 ******************************************************************************/

/**
 * @brief  ADC Initialization Structure
 */
typedef struct {
    HAL_ADC_ModeTypeDef         Mode;           /**< Conversion mode */
    HAL_ADC_ClkDivTypeDef       ClockDiv;       /**< Clock divider */
    HAL_ADC_AcqTimeTypeDef      AcqTime;        /**< Acquisition time */
    HAL_ADC_TriggerTypeDef      Trigger;        /**< Trigger source */
    HAL_ADC_TrigTypeTypeDef     TriggerType;    /**< Trigger type for HW trigger */
    uint8_t                     EnableAlign;    /**< Left/Right alignment */
} HAL_ADC_InitTypeDef;

/**
 * @brief  ADC Handle Structure
 */
typedef struct {
    HAL_ADC_InitTypeDef         Init;           /**< ADC configuration */
    volatile uint16_t           LastResult;     /**< Last conversion result */
    volatile uint8_t            ConvComplete;   /**< Conversion complete flag */
    HAL_LockTypeDef             Lock;           /**< Lock object */
} HAL_ADC_HandleTypeDef;

/*******************************************************************************
 * ADC Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize ADC peripheral
 * @param  hadc Pointer to ADC handle structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_ADC_HandleTypeDef hadc;
 *   hadc.Init.Mode = HAL_ADC_MODE_SINGLE;
 *   hadc.Init.ClockDiv = HAL_ADC_DIV_2;
 *   hadc.Init.AcqTime = HAL_ADC_AQT_5;
 *   hadc.Init.Trigger = HAL_ADC_TRIG_SOFTWARE;
 *   HAL_ADC_Init(&hadc);
 */
HAL_StatusTypeDef HAL_ADC_Init(HAL_ADC_HandleTypeDef *hadc);

/**
 * @brief  De-initialize ADC peripheral
 * @param  hadc Pointer to ADC handle structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_DeInit(HAL_ADC_HandleTypeDef *hadc);

/**
 * @brief  Simple ADC initialization
 * @param  clockDiv Clock divider
 * @param  acqTime Acquisition time
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_SimpleInit(HAL_ADC_ClkDivTypeDef clockDiv, 
                                      HAL_ADC_AcqTimeTypeDef acqTime);

/**
 * @brief  Enable ADC peripheral
 * @retval None
 */
void HAL_ADC_Enable(void);

/**
 * @brief  Disable ADC peripheral
 * @retval None
 */
void HAL_ADC_Disable(void);

/**
 * @brief  Configure ADC channel for input
 * @param  channel ADC channel to configure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   This configures the corresponding GPIO pin as analog input
 */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(HAL_ADC_ChannelTypeDef channel);

/**
 * @brief  Select ADC channel
 * @param  channel ADC channel to select
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_SelectChannel(HAL_ADC_ChannelTypeDef channel);

/**
 * @brief  Start ADC conversion (software trigger)
 * @retval None
 */
void HAL_ADC_StartConversion(void);

/**
 * @brief  Check if ADC conversion is complete
 * @retval uint8_t 1 if complete, 0 otherwise
 */
uint8_t HAL_ADC_IsConversionComplete(void);

/**
 * @brief  Get ADC conversion result
 * @retval uint16_t 12-bit ADC result
 */
uint16_t HAL_ADC_GetResult(void);

/**
 * @brief  Perform single ADC conversion (blocking)
 * @param  channel ADC channel to convert
 * @retval uint16_t 12-bit ADC result
 * 
 * @example
 *   uint16_t adcValue = HAL_ADC_Read(HAL_ADC_CHANNEL_4);
 */
uint16_t HAL_ADC_Read(HAL_ADC_ChannelTypeDef channel);

/**
 * @brief  Perform single ADC conversion with timeout
 * @param  channel ADC channel to convert
 * @param  result Pointer to store result
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_ReadTimeout(HAL_ADC_ChannelTypeDef channel, 
                                       uint16_t *result, uint32_t timeout);

/**
 * @brief  Read multiple ADC channels
 * @param  channels Array of channels to read
 * @param  results Array to store results
 * @param  count Number of channels
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_ReadMultiple(HAL_ADC_ChannelTypeDef *channels,
                                        uint16_t *results, uint8_t count);

/**
 * @brief  Read internal bandgap voltage
 * @retval uint16_t Bandgap ADC value
 * 
 * @note   Can be used to calculate VDD voltage
 */
uint16_t HAL_ADC_ReadBandgap(void);

/**
 * @brief  Calculate VDD voltage using bandgap reference
 * @param  bandgapADC Bandgap ADC reading
 * @retval uint16_t VDD voltage in millivolts
 * 
 * @note   Bandgap voltage is approximately 1.22V
 */
uint16_t HAL_ADC_CalculateVDD(uint16_t bandgapADC);

/**
 * @brief  Configure ADC compare mode
 * @param  mode Compare mode
 * @param  threshold Compare threshold (12-bit value)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_ConfigCompare(HAL_ADC_CompareModeTypeDef mode, 
                                         uint16_t threshold);

/**
 * @brief  Enable ADC interrupt
 * @retval None
 */
void HAL_ADC_EnableInterrupt(void);

/**
 * @brief  Disable ADC interrupt
 * @retval None
 */
void HAL_ADC_DisableInterrupt(void);

/**
 * @brief  Get ADC interrupt flag
 * @retval uint8_t 1 if flag set, 0 otherwise
 */
uint8_t HAL_ADC_GetFlag(void);

/**
 * @brief  Clear ADC interrupt flag
 * @retval None
 */
void HAL_ADC_ClearFlag(void);

/**
 * @brief  Read calibration data from flash
 * @retval uint16_t Bandgap calibration value
 */
uint16_t HAL_ADC_ReadBandgapCalibration(void);

/*******************************************************************************
 * ADC Interrupt-Driven Functions
 ******************************************************************************/

/**
 * @brief  Start ADC conversion with interrupt
 * @param  hadc Pointer to ADC handle
 * @param  channel ADC channel to convert
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Non-blocking function. HAL_ADC_ConvCompleteCallback is called on completion.
 */
HAL_StatusTypeDef HAL_ADC_Start_IT(HAL_ADC_HandleTypeDef *hadc, 
                                    HAL_ADC_ChannelTypeDef channel);

/**
 * @brief  Stop ADC conversion
 * @param  hadc Pointer to ADC handle
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_Stop_IT(HAL_ADC_HandleTypeDef *hadc);

/**
 * @brief  ADC interrupt handler
 * @param  hadc Pointer to ADC handle
 * @note   Call this from ADC ISR
 */
void HAL_ADC_IRQHandler(HAL_ADC_HandleTypeDef *hadc);

/**
 * @brief  Start continuous ADC conversion with interrupt
 * @param  hadc Pointer to ADC handle
 * @param  channel ADC channel to convert
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_ADC_StartContinuous_IT(HAL_ADC_HandleTypeDef *hadc,
                                              HAL_ADC_ChannelTypeDef channel);

/*******************************************************************************
 * ADC Callback
 ******************************************************************************/

/**
 * @brief  ADC conversion complete callback
 * @param  hadc Pointer to ADC handle
 * @note   Called from ADC ISR when conversion is complete
 */
void HAL_ADC_ConvCompleteCallback(HAL_ADC_HandleTypeDef *hadc);

/**
 * @brief  ADC compare match callback
 * @param  hadc Pointer to ADC handle
 * @note   Called when ADC result matches compare threshold
 */
void HAL_ADC_CompareMatchCallback(HAL_ADC_HandleTypeDef *hadc);

/*******************************************************************************
 * ADC Convenience Macros
 ******************************************************************************/

/**
 * @brief  Convert ADC value to voltage (assuming 3.3V reference)
 */
#define HAL_ADC_TO_VOLTAGE_MV(adc_val)  ((uint32_t)(adc_val) * 3300 / 4096)

/**
 * @brief  Convert ADC value to percentage (0-100%)
 */
#define HAL_ADC_TO_PERCENT(adc_val)     ((uint32_t)(adc_val) * 100 / 4096)

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_ADC_H */
