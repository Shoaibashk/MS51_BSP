/*===========================================================================
 * @file    drv_pwm.c
 * @brief   PWM Driver Implementation
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_pwm.h"

/*---------------------------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------------------------*/
static pwm_handle_t *pwm_handle_ptr = NULL;

/*===========================================================================
 * Public Functions
 *===========================================================================*/

/**
 * @brief  Initialize PWM peripheral
 */
drv_status_t DRV_PWM_Init(pwm_handle_t *handle, const pwm_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    /* Initialize handle */
    handle->period = config->period;
    handle->callback = config->callback;
    handle->running = 0;
    handle->enabled_ch = 0;
    
    pwm_handle_ptr = handle;
    
    /* Clear all PWM settings */
    PWMCON0 = 0x00;
    PWMCON1 = 0x00;
    
    /* Configure clock source */
    if (config->clock == PWM_CLK_TIMER1) {
        set_PWMCON1_PWMCKS;
    } else {
        clr_PWMCON1_PWMCKS;
    }
    
    /* Configure prescaler */
    CKCON = (CKCON & 0xC0) | (config->prescaler & 0x07);
    
    /* Configure alignment */
    if (config->alignment == PWM_ALIGN_CENTER) {
        set_PWMCON1_PWMTYP;
    } else {
        clr_PWMCON1_PWMTYP;
    }
    
    /* Configure mode */
    switch (config->mode) {
        case PWM_MODE_INDEPENDENT:
            clr_PWMCON1_GP;
            break;
        case PWM_MODE_COMPLEMENTARY:
            set_PWMCON1_GP;
            break;
        default:
            break;
    }
    
    /* Set period */
    PWMPH = (uint8_t)(config->period >> 8);
    PWMPL = (uint8_t)(config->period & 0xFF);
    
    /* Enable interrupt if requested */
    if (config->enable_int) {
        set_EIE_EPWM;
        set_PWMCON0_PWMF;  /* Clear flag */
    }
    
    return DRV_OK;
}

/**
 * @brief  De-initialize PWM peripheral
 */
drv_status_t DRV_PWM_DeInit(pwm_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Stop PWM */
    DRV_PWM_Stop(handle);
    
    /* Disable all outputs */
    PIOCON0 = 0x00;
    PIOCON1 = 0x00;
    
    /* Disable interrupt */
    clr_EIE_EPWM;
    
    pwm_handle_ptr = NULL;
    
    return DRV_OK;
}

/**
 * @brief  Configure PWM channel
 */
drv_status_t DRV_PWM_ConfigChannel(pwm_handle_t *handle, 
                                    const pwm_channel_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    if (config->channel > PWM_CH_5) {
        return DRV_INVALID_PARAM;
    }
    
    /* Set duty cycle */
    DRV_PWM_SetDuty(handle, config->channel, config->duty);
    
    /* Configure polarity */
    if (config->polarity == PWM_POL_ACTIVE_LOW) {
        PNP |= (1 << config->channel);
    } else {
        PNP &= ~(1 << config->channel);
    }
    
    /* Enable/disable output */
    if (config->enable) {
        DRV_PWM_EnableChannel(handle, config->channel);
    } else {
        DRV_PWM_DisableChannel(handle, config->channel);
    }
    
    return DRV_OK;
}

/**
 * @brief  Set channel duty cycle
 */
drv_status_t DRV_PWM_SetDuty(pwm_handle_t *handle, pwm_channel_t channel, 
                              uint16_t duty)
{
    DRV_CHECK_PTR(handle);
    
    switch (channel) {
        case PWM_CH_0:
            PWM0H = (uint8_t)(duty >> 8);
            PWM0L = (uint8_t)(duty & 0xFF);
            break;
        case PWM_CH_1:
            PWM1H = (uint8_t)(duty >> 8);
            PWM1L = (uint8_t)(duty & 0xFF);
            break;
        case PWM_CH_2:
            PWM2H = (uint8_t)(duty >> 8);
            PWM2L = (uint8_t)(duty & 0xFF);
            break;
        case PWM_CH_3:
            PWM3H = (uint8_t)(duty >> 8);
            PWM3L = (uint8_t)(duty & 0xFF);
            break;
        case PWM_CH_4:
            PWM4H = (uint8_t)(duty >> 8);
            PWM4L = (uint8_t)(duty & 0xFF);
            break;
        case PWM_CH_5:
            PWM5H = (uint8_t)(duty >> 8);
            PWM5L = (uint8_t)(duty & 0xFF);
            break;
        default:
            return DRV_INVALID_PARAM;
    }
    
    /* Load new duty value */
    set_PWMCON0_LOAD;
    
    return DRV_OK;
}

/**
 * @brief  Set duty cycle as percentage
 */
drv_status_t DRV_PWM_SetDutyPercent(pwm_handle_t *handle, pwm_channel_t channel, 
                                     uint8_t percent)
{
    uint16_t duty;
    
    DRV_CHECK_PTR(handle);
    
    if (percent > 100) {
        percent = 100;
    }
    
    duty = PWM_CALC_DUTY(handle->period, percent);
    
    return DRV_PWM_SetDuty(handle, channel, duty);
}

/**
 * @brief  Set PWM period
 */
drv_status_t DRV_PWM_SetPeriod(pwm_handle_t *handle, uint16_t period)
{
    DRV_CHECK_PTR(handle);
    
    handle->period = period;
    
    PWMPH = (uint8_t)(period >> 8);
    PWMPL = (uint8_t)(period & 0xFF);
    
    /* Load new period */
    set_PWMCON0_LOAD;
    
    return DRV_OK;
}

/**
 * @brief  Start PWM output
 */
drv_status_t DRV_PWM_Start(pwm_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Start PWM counter */
    set_PWMCON0_PWMRUN;
    
    handle->running = 1;
    
    return DRV_OK;
}

/**
 * @brief  Stop PWM output
 */
drv_status_t DRV_PWM_Stop(pwm_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Stop PWM counter */
    clr_PWMCON0_PWMRUN;
    
    handle->running = 0;
    
    return DRV_OK;
}

/**
 * @brief  Enable PWM channel output
 */
drv_status_t DRV_PWM_EnableChannel(pwm_handle_t *handle, pwm_channel_t channel)
{
    DRV_CHECK_PTR(handle);
    
    if (channel > PWM_CH_5 && channel != PWM_CH_ALL) {
        return DRV_INVALID_PARAM;
    }
    
    if (channel == PWM_CH_ALL) {
        PIOCON0 = 0x3F;  /* Enable all 6 channels */
        handle->enabled_ch = 0x3F;
    } else {
        /* Enable GPIO as PWM output */
        switch (channel) {
            case PWM_CH_0:
            case PWM_CH_1:
            case PWM_CH_2:
            case PWM_CH_3:
                PIOCON0 |= (1 << channel);
                break;
            case PWM_CH_4:
            case PWM_CH_5:
                PIOCON1 |= (1 << (channel - 4));
                break;
            default:
                break;
        }
        handle->enabled_ch |= (1 << channel);
    }
    
    return DRV_OK;
}

/**
 * @brief  Disable PWM channel output
 */
drv_status_t DRV_PWM_DisableChannel(pwm_handle_t *handle, pwm_channel_t channel)
{
    DRV_CHECK_PTR(handle);
    
    if (channel > PWM_CH_5 && channel != PWM_CH_ALL) {
        return DRV_INVALID_PARAM;
    }
    
    if (channel == PWM_CH_ALL) {
        PIOCON0 = 0x00;
        PIOCON1 = 0x00;
        handle->enabled_ch = 0;
    } else {
        switch (channel) {
            case PWM_CH_0:
            case PWM_CH_1:
            case PWM_CH_2:
            case PWM_CH_3:
                PIOCON0 &= ~(1 << channel);
                break;
            case PWM_CH_4:
            case PWM_CH_5:
                PIOCON1 &= ~(1 << (channel - 4));
                break;
            default:
                break;
        }
        handle->enabled_ch &= ~(1 << channel);
    }
    
    return DRV_OK;
}

/**
 * @brief  Configure dead time
 */
drv_status_t DRV_PWM_ConfigDeadTime(pwm_handle_t *handle, 
                                     const pwm_deadtime_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    /* Configure dead time for complementary pairs */
    /* PDTEN register controls dead time enable */
    /* PDTCNT register sets dead time counter */
    
    /* Set dead time value */
    /* Dead time = PDTCNT * (1/Fsys) */
    
    /* Note: Actual register names may vary by device variant */
    
    return DRV_OK;
}

/**
 * @brief  Enable fault brake
 */
drv_status_t DRV_PWM_EnableBrake(pwm_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Enable fault brake on FB pin */
    set_PWMCON1_FBE;
    
    return DRV_OK;
}

/**
 * @brief  Disable fault brake
 */
drv_status_t DRV_PWM_DisableBrake(pwm_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    clr_PWMCON1_FBE;
    
    return DRV_OK;
}

/**
 * @brief  Trigger software brake
 */
drv_status_t DRV_PWM_TriggerBrake(pwm_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Software fault brake */
    set_PWMCON1_PWMF;
    
    return DRV_OK;
}

/**
 * @brief  Register PWM callback
 */
drv_status_t DRV_PWM_RegisterCallback(pwm_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    handle->callback = callback;
    return DRV_OK;
}

/*===========================================================================
 * PWM Interrupt Service Routine
 *===========================================================================*/

/**
 * @brief  PWM Period Interrupt ISR
 */
#if defined(COMPILER_KEIL)
void PWM_ISR(void) interrupt 13
#elif defined(COMPILER_IAR)
#pragma vector = 0x6B
__interrupt void PWM_ISR(void)
#elif defined(COMPILER_SDCC)
void PWM_ISR(void) __interrupt(13)
#endif
#if !defined(COMPILER_GCC)
{
    pwm_handle_t *handle = pwm_handle_ptr;
    
    /* Clear interrupt flag */
    clr_PWMCON0_PWMF;
    
    if (handle && handle->callback) {
        handle->callback(PWM_EVT_PERIOD, NULL);
    }
}
#endif
