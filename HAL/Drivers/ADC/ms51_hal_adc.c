/**
 *******************************************************************************
 * @file    ms51_hal_adc.c
 * @brief   MS51 HAL ADC Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_ADC_MODULE_ENABLED

/*******************************************************************************
 * Private Defines
 ******************************************************************************/

/* IAP commands for reading UID area (bandgap calibration) */
#define IAP_READ_UID    0x04

/* Bandgap reference voltage in millivolts (typical 1.22V) */
#define BANDGAP_VOLTAGE_MV  1220

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize ADC peripheral
 */
HAL_StatusTypeDef HAL_ADC_Init(HAL_ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    
    /* Configure clock divider */
    ADCCON1 &= ~0x70;
    ADCCON1 |= ((hadc->Init.ClockDiv & 0x07) << 4);
    
    /* Configure acquisition time */
    ADCCON2 &= ~0x0E;
    ADCCON2 |= ((hadc->Init.AcqTime & 0x07) << 1);
    
    /* Configure trigger source */
    ADCCON0 &= ~0x30;  /* Clear ETGSEL bits */
    
    if (hadc->Init.Trigger != HAL_ADC_TRIG_SOFTWARE) {
        /* Enable external trigger */
        ADCCON1 |= 0x02;  /* ADCEX = 1 */
        
        switch (hadc->Init.Trigger) {
            case HAL_ADC_TRIG_PWM_CH0:
                ADCCON0 |= 0x00;  /* ETGSEL = 00 */
                break;
            case HAL_ADC_TRIG_PWM_CH2:
                ADCCON0 |= 0x10;  /* ETGSEL = 01 */
                break;
            case HAL_ADC_TRIG_PWM_CH4:
                ADCCON0 |= 0x20;  /* ETGSEL = 10 */
                break;
            case HAL_ADC_TRIG_STADC:
                ADCCON0 |= 0x30;  /* ETGSEL = 11 */
                break;
            default:
                break;
        }
        
        /* Configure trigger type */
        ADCCON1 &= ~0x0C;
        ADCCON1 |= ((hadc->Init.TriggerType & 0x03) << 2);
    } else {
        ADCCON1 &= ~0x02;  /* ADCEX = 0 */
    }
    
    hadc->Lock = HAL_UNLOCKED;
    hadc->ConvComplete = 0;
    
    return HAL_OK;
}

/**
 * @brief  De-initialize ADC peripheral
 */
HAL_StatusTypeDef HAL_ADC_DeInit(HAL_ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Disable();
    HAL_ADC_DisableInterrupt();
    
    return HAL_OK;
}

/**
 * @brief  Simple ADC initialization
 */
HAL_StatusTypeDef HAL_ADC_SimpleInit(HAL_ADC_ClkDivTypeDef clockDiv, 
                                      HAL_ADC_AcqTimeTypeDef acqTime)
{
    HAL_SFR_PAGE0();
    
    /* Configure clock divider */
    ADCCON1 &= ~0x70;
    ADCCON1 |= ((clockDiv & 0x07) << 4);
    
    /* Configure acquisition time */
    ADCCON2 &= ~0x0E;
    ADCCON2 |= ((acqTime & 0x07) << 1);
    
    /* Software trigger mode */
    ADCCON1 &= ~0x02;
    
    return HAL_OK;
}

/**
 * @brief  Enable ADC peripheral
 */
void HAL_ADC_Enable(void)
{
    HAL_SFR_PAGE0();
    ADCCON1 |= 0x01;  /* ADCEN = 1 */
}

/**
 * @brief  Disable ADC peripheral
 */
void HAL_ADC_Disable(void)
{
    HAL_SFR_PAGE0();
    ADCCON1 &= ~0x01;  /* ADCEN = 0 */
}

/**
 * @brief  Configure ADC channel for input
 */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(HAL_ADC_ChannelTypeDef channel)
{
    HAL_SFR_PAGE0();
    
    /* Disable digital input for the corresponding pin */
    switch (channel) {
        case HAL_ADC_CHANNEL_0:  /* AIN0 - P0.7 */
            AINDIDS |= 0x04;
            P0M1 |= 0x80;  /* Input mode */
            P0M2 &= ~0x80;
            break;
            
        case HAL_ADC_CHANNEL_1:  /* AIN1 - P3.0 */
            AINDIDS |= 0x02;
            P3M1 |= 0x01;
            P3M2 &= ~0x01;
            break;
            
        case HAL_ADC_CHANNEL_2:  /* AIN2 - P0.6 */
            AINDIDS |= 0x08;
            P0M1 |= 0x40;
            P0M2 &= ~0x40;
            break;
            
        case HAL_ADC_CHANNEL_3:  /* AIN3 - P0.5 */
            AINDIDS |= 0x10;
            P0M1 |= 0x20;
            P0M2 &= ~0x20;
            break;
            
        case HAL_ADC_CHANNEL_4:  /* AIN4 - P0.4 */
            AINDIDS |= 0x20;
            P0M1 |= 0x10;
            P0M2 &= ~0x10;
            break;
            
        case HAL_ADC_CHANNEL_5:  /* AIN5 - P0.3 */
            AINDIDS |= 0x40;
            P0M1 |= 0x08;
            P0M2 &= ~0x08;
            break;
            
        case HAL_ADC_CHANNEL_6:  /* AIN6 - P1.1 */
            AINDIDS |= 0x80;
            P1M1 |= 0x02;
            P1M2 &= ~0x02;
            break;
            
        case HAL_ADC_CHANNEL_7:  /* AIN7 - P1.7 */
            AINDIDS |= 0x01;
            P1M1 |= 0x80;
            P1M2 &= ~0x80;
            break;
            
        case HAL_ADC_CHANNEL_BANDGAP:
            /* No GPIO configuration needed for bandgap */
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Select ADC channel
 */
HAL_StatusTypeDef HAL_ADC_SelectChannel(HAL_ADC_ChannelTypeDef channel)
{
    if (channel > HAL_ADC_CHANNEL_BANDGAP) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    
    /* Clear channel selection bits and set new channel */
    ADCCON0 &= ~0x0F;
    ADCCON0 |= (channel & 0x0F);
    
    return HAL_OK;
}

/**
 * @brief  Start ADC conversion
 */
void HAL_ADC_StartConversion(void)
{
    HAL_SFR_PAGE0();
    ADCF = 0;           /* Clear conversion complete flag */
    ADCS = 1;           /* Start conversion */
}

/**
 * @brief  Check if ADC conversion is complete
 */
uint8_t HAL_ADC_IsConversionComplete(void)
{
    return (ADCCON0 & 0x80) ? 1 : 0;  /* Check ADCF bit */
}

/**
 * @brief  Get ADC conversion result
 */
uint16_t HAL_ADC_GetResult(void)
{
    uint16_t result;
    
    HAL_SFR_PAGE0();
    
    /* Read 12-bit result */
    result = (uint16_t)ADCRH << 4;
    result |= (ADCRL & 0x0F);
    
    return result;
}

/**
 * @brief  Perform single ADC conversion (blocking)
 */
uint16_t HAL_ADC_Read(HAL_ADC_ChannelTypeDef channel)
{
    /* Select channel */
    HAL_ADC_SelectChannel(channel);
    
    /* Enable ADC */
    HAL_ADC_Enable();
    
    /* Clear flag and start conversion */
    ADCF = 0;
    ADCS = 1;
    
    /* Wait for conversion complete */
    while (!(ADCCON0 & 0x80));
    
    return HAL_ADC_GetResult();
}

/**
 * @brief  Perform single ADC conversion with timeout
 */
HAL_StatusTypeDef HAL_ADC_ReadTimeout(HAL_ADC_ChannelTypeDef channel, 
                                       uint16_t *result, uint32_t timeout)
{
    uint32_t tickstart;
    
    if (result == NULL) {
        return HAL_INVALID;
    }
    
    /* Select channel */
    HAL_ADC_SelectChannel(channel);
    
    /* Enable ADC */
    HAL_ADC_Enable();
    
    /* Start conversion */
    ADCF = 0;
    ADCS = 1;
    
    tickstart = HAL_GetTick();
    
    /* Wait for conversion complete with timeout */
    while (!(ADCCON0 & 0x80)) {
        if ((HAL_GetTick() - tickstart) > timeout) {
            return HAL_TIMEOUT;
        }
    }
    
    *result = HAL_ADC_GetResult();
    
    return HAL_OK;
}

/**
 * @brief  Read multiple ADC channels
 */
HAL_StatusTypeDef HAL_ADC_ReadMultiple(HAL_ADC_ChannelTypeDef *channels,
                                        uint16_t *results, uint8_t count)
{
    uint8_t i;
    
    if (channels == NULL || results == NULL || count == 0) {
        return HAL_INVALID;
    }
    
    /* Enable ADC */
    HAL_ADC_Enable();
    
    for (i = 0; i < count; i++) {
        results[i] = HAL_ADC_Read(channels[i]);
    }
    
    return HAL_OK;
}

/**
 * @brief  Read internal bandgap voltage
 */
uint16_t HAL_ADC_ReadBandgap(void)
{
    return HAL_ADC_Read(HAL_ADC_CHANNEL_BANDGAP);
}

/**
 * @brief  Calculate VDD voltage using bandgap reference
 */
uint16_t HAL_ADC_CalculateVDD(uint16_t bandgapADC)
{
    uint32_t vdd;
    
    if (bandgapADC == 0) {
        return 0;
    }
    
    /* VDD = (Bandgap Voltage * 4096) / ADC reading */
    /* Bandgap = 1.22V = 1220mV (typical) */
    vdd = ((uint32_t)BANDGAP_VOLTAGE_MV * 4096UL) / bandgapADC;
    
    return (uint16_t)vdd;
}

/**
 * @brief  Configure ADC compare mode
 */
HAL_StatusTypeDef HAL_ADC_ConfigCompare(HAL_ADC_CompareModeTypeDef mode, 
                                         uint16_t threshold)
{
    HAL_SFR_PAGE0();
    
    if (mode == HAL_ADC_CMP_DISABLE) {
        ADCCON2 &= ~0x20;  /* ADCMPEN = 0 */
        return HAL_OK;
    }
    
    /* Set compare threshold */
    ADCMPL = threshold & 0x0F;
    ADCMPH = (threshold >> 4) & 0xFF;
    
    /* Configure compare mode */
    if (mode == HAL_ADC_CMP_GREATER) {
        ADCCON2 |= 0x40;   /* ADCMPOP = 1 for >= comparison */
    } else {
        ADCCON2 &= ~0x40;  /* ADCMPOP = 0 for < comparison */
    }
    
    /* Enable compare */
    ADCCON2 |= 0x20;  /* ADCMPEN = 1 */
    
    return HAL_OK;
}

/**
 * @brief  Enable ADC interrupt
 */
void HAL_ADC_EnableInterrupt(void)
{
    EADC = 1;  /* Enable ADC interrupt */
}

/**
 * @brief  Disable ADC interrupt
 */
void HAL_ADC_DisableInterrupt(void)
{
    EADC = 0;  /* Disable ADC interrupt */
}

/**
 * @brief  Get ADC interrupt flag
 */
uint8_t HAL_ADC_GetFlag(void)
{
    return (ADCCON0 & 0x80) ? 1 : 0;
}

/**
 * @brief  Clear ADC interrupt flag
 */
void HAL_ADC_ClearFlag(void)
{
    ADCF = 0;
}

/**
 * @brief  Read bandgap calibration data from UID area
 */
uint16_t HAL_ADC_ReadBandgapCalibration(void)
{
    uint8_t bgHigh, bgLow;
    uint16_t bgValue;
    
    /* Enable IAP */
    HAL_TA_SET_BIT(CHPCON, 0x01);  /* IAPEN = 1 */
    
    /* Set read UID command */
    IAPCN = IAP_READ_UID;
    
    /* Read low 4 bits from address 0x000D */
    IAPAL = 0x0D;
    IAPAH = 0x00;
    HAL_TA_SET_BIT(IAPTRG, 0x01);  /* Trigger IAP */
    bgLow = IAPFD & 0x0F;
    
    /* Read high 8 bits from address 0x000C */
    IAPAL = 0x0C;
    HAL_TA_SET_BIT(IAPTRG, 0x01);  /* Trigger IAP */
    bgHigh = IAPFD;
    
    /* Disable IAP */
    HAL_TA_CLEAR_BIT(CHPCON, 0x01);  /* IAPEN = 0 */
    
    bgValue = ((uint16_t)bgHigh << 4) | bgLow;
    
    return bgValue;
}

/**
 * @brief  ADC conversion complete callback (weak)
 */
__attribute__((weak)) void HAL_ADC_ConvCompleteCallback(HAL_ADC_HandleTypeDef *hadc)
{
    /* User implementation */
    HAL_UNUSED(hadc);
}

#endif /* HAL_ADC_MODULE_ENABLED */
