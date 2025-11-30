/*===========================================================================
 * @file    drv_adc.c
 * @brief   ADC Driver Implementation
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_adc.h"

/*---------------------------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------------------------*/
static adc_handle_t *adc_handle_ptr = NULL;

/*---------------------------------------------------------------------------
 * Private Constants
 *---------------------------------------------------------------------------*/
#define BANDGAP_VOLTAGE_MV  1220    /* Typical bandgap = 1.22V */

/*===========================================================================
 * Public Functions
 *===========================================================================*/

/**
 * @brief  Initialize ADC peripheral
 */
drv_status_t DRV_ADC_Init(adc_handle_t *handle, const adc_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    /* Validate channel */
    if (config->channel >= ADC_CH_MAX) {
        return DRV_INVALID_PARAM;
    }
    
    /* Initialize handle */
    handle->channel = config->channel;
    handle->callback = config->callback;
    handle->cmp_callback = NULL;
    handle->result = 0;
    handle->busy = 0;
    
    /* Store handle reference for ISR */
    adc_handle_ptr = handle;
    
    /* Enable ADC */
    ADCCON1 |= 0x01;  /* ADCEN = 1 */
    
    /* Configure clock divider */
    ADCCON1 = (ADCCON1 & 0xF1) | ((config->clock_div & 0x07) << 1);
    
    /* Configure trigger source */
    switch (config->trigger) {
        case ADC_TRIG_SW:
            clr_ADCCON0_ADCS;
            break;
        case ADC_TRIG_PWM:
            /* PWM trigger configuration */
            /* Set ADCCON0.6:4 for PWM trigger selection */
            break;
        case ADC_TRIG_GPIO:
            /* GPIO trigger (STADC pin) */
            set_ADCCON0_ADCS;
            break;
    }
    
    /* Select channel */
    ADCCON0 = (ADCCON0 & 0xF0) | (config->channel & 0x0F);
    
    /* Configure GPIO for analog input if not bandgap */
    if (config->channel != ADC_CH_BG) {
        /* Disable digital input path for analog channels */
        switch (config->channel) {
            case ADC_CH_0: AINDIDS |= 0x01; break;
            case ADC_CH_1: AINDIDS |= 0x02; break;
            case ADC_CH_2: AINDIDS |= 0x04; break;
            case ADC_CH_3: AINDIDS |= 0x08; break;
            case ADC_CH_4: AINDIDS |= 0x10; break;
            case ADC_CH_5: AINDIDS |= 0x20; break;
            case ADC_CH_6: AINDIDS |= 0x40; break;
            case ADC_CH_7: AINDIDS |= 0x80; break;
            default: break;
        }
    }
    
    /* Enable interrupt if requested */
    if (config->enable_int) {
        clr_ADCCON0_ADCF;  /* Clear flag */
        set_IE_EADC;       /* Enable ADC interrupt */
    }
    
    return DRV_OK;
}

/**
 * @brief  De-initialize ADC peripheral
 */
drv_status_t DRV_ADC_DeInit(adc_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Disable ADC interrupt */
    clr_IE_EADC;
    
    /* Disable ADC */
    ADCCON1 &= ~0x01;  /* ADCEN = 0 */
    
    /* Re-enable digital inputs */
    AINDIDS = 0x00;
    
    adc_handle_ptr = NULL;
    
    return DRV_OK;
}

/**
 * @brief  Set ADC input channel
 */
drv_status_t DRV_ADC_SetChannel(adc_handle_t *handle, adc_channel_t channel)
{
    DRV_CHECK_PTR(handle);
    
    if (channel >= ADC_CH_MAX) {
        return DRV_INVALID_PARAM;
    }
    
    handle->channel = channel;
    ADCCON0 = (ADCCON0 & 0xF0) | (channel & 0x0F);
    
    /* Configure digital input disable for analog channel */
    if (channel != ADC_CH_BG) {
        AINDIDS |= (1 << channel);
    }
    
    return DRV_OK;
}

/**
 * @brief  Start ADC conversion (blocking)
 */
drv_status_t DRV_ADC_Convert(adc_handle_t *handle, uint16_t *result)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(result);
    
    if (handle->busy) {
        return DRV_BUSY;
    }
    
    handle->busy = 1;
    
    /* Clear flag */
    clr_ADCCON0_ADCF;
    
    /* Start conversion */
    set_ADCCON0_ADCS;
    
    /* Wait for conversion complete */
    while (!(ADCCON0 & 0x80)) {
        /* ADCF flag check */
    }
    
    /* Clear flag */
    clr_ADCCON0_ADCF;
    
    /* Read result (12-bit) */
    handle->result = ((uint16_t)ADCRH << 4) | ((ADCRL & 0xF0) >> 4);
    *result = handle->result;
    
    handle->busy = 0;
    
    return DRV_OK;
}

/**
 * @brief  Start ADC conversion (non-blocking)
 */
drv_status_t DRV_ADC_StartConvert(adc_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    if (handle->busy) {
        return DRV_BUSY;
    }
    
    handle->busy = 1;
    
    /* Clear flag */
    clr_ADCCON0_ADCF;
    
    /* Start conversion */
    set_ADCCON0_ADCS;
    
    return DRV_OK;
}

/**
 * @brief  Get last conversion result
 */
uint16_t DRV_ADC_GetResult(adc_handle_t *handle)
{
    if (handle == NULL) {
        return 0;
    }
    
    /* Read from registers */
    return ((uint16_t)ADCRH << 4) | ((ADCRL & 0xF0) >> 4);
}

/**
 * @brief  Check if conversion is complete
 */
uint8_t DRV_ADC_IsComplete(adc_handle_t *handle)
{
    if (handle == NULL) {
        return 1;
    }
    
    return (ADCCON0 & 0x80) ? 1 : 0;  /* ADCF flag */
}

/**
 * @brief  Configure ADC compare function
 */
drv_status_t DRV_ADC_ConfigCompare(adc_handle_t *handle, 
                                    const adc_compare_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    handle->cmp_callback = config->callback;
    
    /* Set compare threshold */
    /* ADCMPH:ADCMPL = 12-bit threshold */
    ADCMPH = (uint8_t)(config->threshold >> 4);
    ADCMPL = (uint8_t)((config->threshold & 0x0F) << 4);
    
    /* Configure compare direction */
    if (config->above) {
        set_ADCCON2_ADCMPO;  /* Interrupt when result > threshold */
    } else {
        clr_ADCCON2_ADCMPO;  /* Interrupt when result <= threshold */
    }
    
    return DRV_OK;
}

/**
 * @brief  Enable ADC compare function
 */
drv_status_t DRV_ADC_EnableCompare(adc_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    set_ADCCON2_ADCMPEN;  /* Enable compare */
    
    return DRV_OK;
}

/**
 * @brief  Disable ADC compare function
 */
drv_status_t DRV_ADC_DisableCompare(adc_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    clr_ADCCON2_ADCMPEN;  /* Disable compare */
    
    return DRV_OK;
}

/**
 * @brief  Register ADC callback
 */
drv_status_t DRV_ADC_RegisterCallback(adc_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    handle->callback = callback;
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Bandgap / VDD Measurement
 *---------------------------------------------------------------------------*/

/**
 * @brief  Read bandgap voltage
 */
uint16_t DRV_ADC_ReadBandgap(adc_handle_t *handle)
{
    uint16_t result;
    adc_channel_t saved_channel;
    
    if (handle == NULL) {
        return 0;
    }
    
    /* Save current channel */
    saved_channel = handle->channel;
    
    /* Switch to bandgap channel */
    DRV_ADC_SetChannel(handle, ADC_CH_BG);
    
    /* Perform conversion */
    DRV_ADC_Convert(handle, &result);
    
    /* Restore channel */
    DRV_ADC_SetChannel(handle, saved_channel);
    
    return result;
}

/**
 * @brief  Calculate VDD from bandgap reading
 */
uint16_t DRV_ADC_CalculateVDD(uint16_t bandgap_reading)
{
    uint32_t vdd;
    
    if (bandgap_reading == 0) {
        return 0;
    }
    
    /* VDD = (Bandgap_Voltage * 4096) / ADC_reading */
    vdd = ((uint32_t)BANDGAP_VOLTAGE_MV * 4096UL) / bandgap_reading;
    
    return (uint16_t)vdd;
}

/**
 * @brief  Convert ADC reading to millivolts
 */
uint16_t DRV_ADC_ToMillivolts(uint16_t adc_value, uint16_t vdd_mv)
{
    uint32_t mv;
    
    /* Voltage = (ADC_value * VDD) / 4096 */
    mv = ((uint32_t)adc_value * vdd_mv) / 4096UL;
    
    return (uint16_t)mv;
}

/*===========================================================================
 * ADC Interrupt Service Routine
 *===========================================================================*/

/**
 * @brief  ADC Conversion Complete ISR
 */
#if defined(COMPILER_KEIL)
void ADC_ISR(void) interrupt 11
#elif defined(COMPILER_IAR)
#pragma vector = 0x5B
__interrupt void ADC_ISR(void)
#elif defined(COMPILER_SDCC)
void ADC_ISR(void) __interrupt(11)
#endif
#if !defined(COMPILER_GCC)
{
    adc_handle_t *handle = adc_handle_ptr;
    
    /* Clear interrupt flag */
    clr_ADCCON0_ADCF;
    
    if (handle) {
        /* Read result */
        handle->result = ((uint16_t)ADCRH << 4) | ((ADCRL & 0xF0) >> 4);
        handle->busy = 0;
        
        /* Call callback */
        if (handle->callback) {
            handle->callback(ADC_EVT_COMPLETE, &handle->result);
        }
    }
}
#endif
