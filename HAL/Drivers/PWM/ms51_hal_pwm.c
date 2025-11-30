/**
 *******************************************************************************
 * @file    ms51_hal_pwm.c
 * @brief   MS51 HAL PWM Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_PWM_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint16_t g_pwmPeriod = 0;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize PWM peripheral
 */
HAL_StatusTypeDef HAL_PWM_Init(HAL_PWM_HandleTypeDef *hpwm)
{
    if (hpwm == NULL) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    
    /* Stop PWM first */
    PWMRUN = 0;
    
    /* Clear PWM counter */
    CLRPWM = 1;
    
    /* Set PWM mode */
    PWMCON1 &= ~0xC0;  /* Clear mode bits */
    PWMCON1 |= ((hpwm->Init.Mode & 0x03) << 6);
    
    /* Set PWM type */
    if (hpwm->Init.Type == HAL_PWM_TYPE_CENTER) {
        PWMCON1 |= 0x10;  /* PWMTYP = 1 */
    } else {
        PWMCON1 &= ~0x10;
    }
    
    /* Set clock source */
    if (hpwm->Init.ClockSource == HAL_PWM_CLK_TIMER1) {
        CKCON |= 0x40;    /* PWMCKS = 1 */
    } else {
        CKCON &= ~0x40;   /* PWMCKS = 0 */
    }
    
    /* Set clock divider */
    PWMCON1 &= ~0x07;
    PWMCON1 |= (hpwm->Init.ClockDiv & 0x07);
    
    /* Set period */
    g_pwmPeriod = hpwm->Init.Period;
    PWMPH = HAL_HIBYTE(hpwm->Init.Period);
    PWMPL = HAL_LOBYTE(hpwm->Init.Period);
    
    hpwm->Lock = HAL_UNLOCKED;
    
    return HAL_OK;
}

/**
 * @brief  De-initialize PWM peripheral
 */
HAL_StatusTypeDef HAL_PWM_DeInit(HAL_PWM_HandleTypeDef *hpwm)
{
    HAL_PWM_Stop();
    HAL_PWM_DisableInterrupt();
    
    return HAL_OK;
}

/**
 * @brief  Simple PWM initialization
 */
HAL_StatusTypeDef HAL_PWM_SimpleInit(HAL_PWM_DivTypeDef clockDiv, uint16_t period)
{
    HAL_SFR_PAGE0();
    
    /* Stop and clear PWM */
    PWMRUN = 0;
    CLRPWM = 1;
    
    /* Set independent mode, edge-aligned */
    PWMCON1 = 0x00;
    
    /* Set divider */
    PWMCON1 |= (clockDiv & 0x07);
    
    /* Use Fsys as clock source */
    CKCON &= ~0x40;
    
    /* Set period */
    g_pwmPeriod = period;
    PWMPH = HAL_HIBYTE(period);
    PWMPL = HAL_LOBYTE(period);
    
    return HAL_OK;
}

/**
 * @brief  Configure PWM channel
 */
HAL_StatusTypeDef HAL_PWM_ConfigChannel(HAL_PWM_ChannelInitTypeDef *config)
{
    if (config == NULL) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    
    /* Set duty cycle */
    HAL_PWM_SetDuty(config->Channel, config->DutyCycle);
    
    /* Set polarity */
    HAL_PWM_SetPolarity(config->Channel, config->Polarity);
    
    /* Enable/disable output */
    if (config->OutputEnable) {
        HAL_PWM_EnableOutput(config->Channel);
    } else {
        HAL_PWM_DisableOutput(config->Channel);
    }
    
    return HAL_OK;
}

/**
 * @brief  Start PWM
 */
void HAL_PWM_Start(void)
{
    HAL_SFR_PAGE0();
    PWMRUN = 1;
}

/**
 * @brief  Stop PWM
 */
void HAL_PWM_Stop(void)
{
    HAL_SFR_PAGE0();
    PWMRUN = 0;
}

/**
 * @brief  Set PWM period
 */
void HAL_PWM_SetPeriod(uint16_t period)
{
    HAL_SFR_PAGE0();
    g_pwmPeriod = period;
    PWMPH = HAL_HIBYTE(period);
    PWMPL = HAL_LOBYTE(period);
}

/**
 * @brief  Set PWM duty cycle for a channel
 */
HAL_StatusTypeDef HAL_PWM_SetDuty(HAL_PWM_ChannelTypeDef channel, uint16_t duty)
{
    HAL_SFR_PAGE0();
    
    switch (channel) {
        case HAL_PWM_CHANNEL_0:
            PWM0H = HAL_HIBYTE(duty);
            PWM0L = HAL_LOBYTE(duty);
            break;
            
        case HAL_PWM_CHANNEL_1:
            PWM1H = HAL_HIBYTE(duty);
            PWM1L = HAL_LOBYTE(duty);
            break;
            
        case HAL_PWM_CHANNEL_2:
            PWM2H = HAL_HIBYTE(duty);
            PWM2L = HAL_LOBYTE(duty);
            break;
            
        case HAL_PWM_CHANNEL_3:
            PWM3H = HAL_HIBYTE(duty);
            PWM3L = HAL_LOBYTE(duty);
            break;
            
        case HAL_PWM_CHANNEL_4:
            HAL_SFR_PAGE1();
            PWM0H = HAL_HIBYTE(duty);  /* PWM4H on page 1 */
            PWM0L = HAL_LOBYTE(duty);  /* PWM4L on page 1 */
            HAL_SFR_PAGE0();
            break;
            
        case HAL_PWM_CHANNEL_5:
            HAL_SFR_PAGE1();
            PWM1H = HAL_HIBYTE(duty);  /* PWM5H on page 1 */
            PWM1L = HAL_LOBYTE(duty);  /* PWM5L on page 1 */
            HAL_SFR_PAGE0();
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Set PWM duty cycle as percentage
 */
HAL_StatusTypeDef HAL_PWM_SetDutyPercent(HAL_PWM_ChannelTypeDef channel, 
                                          uint8_t percent)
{
    uint32_t duty;
    
    if (percent > 100) {
        percent = 100;
    }
    
    duty = ((uint32_t)g_pwmPeriod * percent) / 100;
    
    return HAL_PWM_SetDuty(channel, (uint16_t)duty);
}

/**
 * @brief  Enable PWM channel output
 */
HAL_StatusTypeDef HAL_PWM_EnableOutput(HAL_PWM_ChannelTypeDef channel)
{
    HAL_SFR_PAGE0();
    
    switch (channel) {
        case HAL_PWM_CHANNEL_0:
            PIOCON0 |= 0x01;  /* PIO00 = 1 */
            break;
        case HAL_PWM_CHANNEL_1:
            PIOCON0 |= 0x02;  /* PIO01 = 1 */
            break;
        case HAL_PWM_CHANNEL_2:
            PIOCON0 |= 0x04;  /* PIO02 = 1 */
            break;
        case HAL_PWM_CHANNEL_3:
            PIOCON0 |= 0x08;  /* PIO03 = 1 */
            break;
        case HAL_PWM_CHANNEL_4:
            PIOCON0 |= 0x10;  /* PIO04 = 1 */
            break;
        case HAL_PWM_CHANNEL_5:
            PIOCON0 |= 0x20;  /* PIO05 = 1 */
            break;
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Disable PWM channel output
 */
HAL_StatusTypeDef HAL_PWM_DisableOutput(HAL_PWM_ChannelTypeDef channel)
{
    HAL_SFR_PAGE0();
    
    switch (channel) {
        case HAL_PWM_CHANNEL_0:
            PIOCON0 &= ~0x01;
            break;
        case HAL_PWM_CHANNEL_1:
            PIOCON0 &= ~0x02;
            break;
        case HAL_PWM_CHANNEL_2:
            PIOCON0 &= ~0x04;
            break;
        case HAL_PWM_CHANNEL_3:
            PIOCON0 &= ~0x08;
            break;
        case HAL_PWM_CHANNEL_4:
            PIOCON0 &= ~0x10;
            break;
        case HAL_PWM_CHANNEL_5:
            PIOCON0 &= ~0x20;
            break;
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Set PWM output polarity
 */
HAL_StatusTypeDef HAL_PWM_SetPolarity(HAL_PWM_ChannelTypeDef channel, 
                                       uint8_t polarity)
{
    uint8_t mask = (1 << channel);
    
    if (channel > HAL_PWM_CHANNEL_5) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    
    if (polarity) {
        PNP |= mask;
    } else {
        PNP &= ~mask;
    }
    
    return HAL_OK;
}

/**
 * @brief  Enable dead-time for complementary channels
 */
HAL_StatusTypeDef HAL_PWM_SetDeadTime(uint8_t pair, uint8_t deadtime)
{
    if (pair > 2) {
        return HAL_INVALID;
    }
    
    /* Set dead-time value */
    HAL_TA_WRITE(PDTCNT, deadtime);
    
    /* Enable dead-time for the pair */
    switch (pair) {
        case 0:
            HAL_TA_SET_BIT(PDTEN, 0x01);  /* PDT01EN */
            break;
        case 1:
            HAL_TA_SET_BIT(PDTEN, 0x02);  /* PDT23EN */
            break;
        case 2:
            HAL_TA_SET_BIT(PDTEN, 0x04);  /* PDT45EN */
            break;
    }
    
    return HAL_OK;
}

/**
 * @brief  Load new PWM values
 */
void HAL_PWM_LoadValues(void)
{
    HAL_SFR_PAGE0();
    LOAD = 1;
}

/**
 * @brief  Enable PWM interrupt
 */
void HAL_PWM_EnableInterrupt(void)
{
    EIE |= 0x08;  /* EPWM = 1 */
}

/**
 * @brief  Disable PWM interrupt
 */
void HAL_PWM_DisableInterrupt(void)
{
    EIE &= ~0x08;  /* EPWM = 0 */
}

/**
 * @brief  Get PWM interrupt flag
 */
uint8_t HAL_PWM_GetFlag(void)
{
    return PWMF;
}

/**
 * @brief  Clear PWM interrupt flag
 */
void HAL_PWM_ClearFlag(void)
{
    PWMF = 0;
}

/**
 * @brief  Configure fault brake function
 */
HAL_StatusTypeDef HAL_PWM_ConfigFaultBrake(uint8_t enable, uint8_t source, 
                                            uint8_t level)
{
    if (enable) {
        /* Configure fault brake input level */
        if (level) {
            FBD |= 0x40;  /* FBINLS = 1 (high level triggers) */
        } else {
            FBD &= ~0x40; /* FBINLS = 0 (low level triggers) */
        }
        
        /* Enable fault brake */
        PWMCON1 |= 0x08;  /* FBINEN = 1 */
    } else {
        PWMCON1 &= ~0x08; /* FBINEN = 0 */
    }
    
    return HAL_OK;
}

/*******************************************************************************
 * Weak Callback Function
 ******************************************************************************/

__attribute__((weak)) void HAL_PWM_PeriodElapsedCallback(HAL_PWM_HandleTypeDef *hpwm)
{
    HAL_UNUSED(hpwm);
}

#endif /* HAL_PWM_MODULE_ENABLED */
