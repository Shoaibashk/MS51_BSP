/**
 *******************************************************************************
 * @file    ms51_hal_timer.c
 * @brief   MS51 HAL Timer Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_TIMER_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint8_t HAL_DATA g_timer0_th, g_timer0_tl;
static uint8_t HAL_DATA g_timer1_th, g_timer1_tl;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize Timer0
 */
HAL_StatusTypeDef HAL_Timer0_Init(HAL_Timer01_InitTypeDef *init)
{
    if (init == NULL) {
        return HAL_INVALID;
    }
    
    /* Stop timer first */
    TR0 = 0;
    
    /* Clear mode bits for Timer0 */
    TMOD &= 0xF0;
    
    /* Set mode */
    TMOD |= (init->Mode & 0x03);
    
    /* Set clock source */
    if (init->ClockSource == HAL_TIMER_CLK_FSYS) {
        CKCON |= 0x08;  /* T0M = 1, Fsys */
    } else {
        CKCON &= ~0x08; /* T0M = 0, Fsys/12 */
    }
    
    /* Set gate control */
    if (init->EnableGate) {
        TMOD |= 0x08;   /* GATE0 = 1 */
    }
    
    /* Set counter mode */
    if (init->CounterMode) {
        TMOD |= 0x04;   /* C/T0 = 1, counter mode */
    }
    
    /* Set period/reload value */
    if (init->Mode == HAL_TIMER_MODE2) {
        /* 8-bit auto-reload */
        TH0 = (uint8_t)(256 - init->Period);
        TL0 = TH0;
    } else {
        /* 16-bit or 13-bit */
        TH0 = HAL_HIBYTE(65536 - init->Period);
        TL0 = HAL_LOBYTE(65536 - init->Period);
    }
    
    /* Store reload values */
    g_timer0_th = TH0;
    g_timer0_tl = TL0;
    
    /* Clear flag */
    TF0 = 0;
    
    return HAL_OK;
}

/**
 * @brief  Initialize Timer1
 */
HAL_StatusTypeDef HAL_Timer1_Init(HAL_Timer01_InitTypeDef *init)
{
    if (init == NULL) {
        return HAL_INVALID;
    }
    
    /* Stop timer first */
    TR1 = 0;
    
    /* Clear mode bits for Timer1 */
    TMOD &= 0x0F;
    
    /* Set mode */
    TMOD |= ((init->Mode & 0x03) << 4);
    
    /* Set clock source */
    if (init->ClockSource == HAL_TIMER_CLK_FSYS) {
        CKCON |= 0x10;  /* T1M = 1, Fsys */
    } else {
        CKCON &= ~0x10; /* T1M = 0, Fsys/12 */
    }
    
    /* Set gate control */
    if (init->EnableGate) {
        TMOD |= 0x80;   /* GATE1 = 1 */
    }
    
    /* Set counter mode */
    if (init->CounterMode) {
        TMOD |= 0x40;   /* C/T1 = 1, counter mode */
    }
    
    /* Set period/reload value */
    if (init->Mode == HAL_TIMER_MODE2) {
        /* 8-bit auto-reload */
        TH1 = (uint8_t)(256 - init->Period);
        TL1 = TH1;
    } else {
        /* 16-bit or 13-bit */
        TH1 = HAL_HIBYTE(65536 - init->Period);
        TL1 = HAL_LOBYTE(65536 - init->Period);
    }
    
    /* Store reload values */
    g_timer1_th = TH1;
    g_timer1_tl = TL1;
    
    /* Clear flag */
    TF1 = 0;
    
    return HAL_OK;
}

/**
 * @brief  Initialize Timer2
 */
HAL_StatusTypeDef HAL_Timer2_Init(HAL_Timer2_InitTypeDef *init)
{
    if (init == NULL) {
        return HAL_INVALID;
    }
    
    /* Stop timer first */
    TR2 = 0;
    
    /* Clear T2MOD */
    T2MOD = 0x00;
    
    /* Set divider */
    T2MOD |= ((init->Divider & 0x07) << 4);
    
    /* Set mode */
    switch (init->Mode) {
        case HAL_TIMER2_TIMER:
            T2CON = 0x00;
            break;
            
        case HAL_TIMER2_COUNTER:
            T2CON = 0x02;
            break;
            
        case HAL_TIMER2_CAPTURE:
            T2CON = 0x01;  /* CM_RL2 = 1 */
            break;
            
        case HAL_TIMER2_AUTORELOAD:
            T2CON = 0x00;
            T2MOD |= 0x80;  /* LDEN = 1 for auto-reload */
            break;
            
        default:
            return HAL_INVALID;
    }
    
    /* Set period/reload value */
    TH2 = HAL_HIBYTE(65536 - init->Period);
    TL2 = HAL_LOBYTE(65536 - init->Period);
    RCMP2H = TH2;
    RCMP2L = TL2;
    
    /* Clear flag */
    TF2 = 0;
    
    return HAL_OK;
}

/**
 * @brief  Initialize Timer3
 */
HAL_StatusTypeDef HAL_Timer3_Init(HAL_Timer3_InitTypeDef *init)
{
    if (init == NULL) {
        return HAL_INVALID;
    }
    
    /* Ensure page 0 */
    HAL_SFR_PAGE0();
    
    /* Stop timer and clear T3CON */
    T3CON &= ~0x08;  /* TR3 = 0 */
    
    /* Clear divider bits and set new divider */
    T3CON &= 0xF8;
    T3CON |= (init->Divider & 0x07);
    
    /* Set period/reload value */
    RH3 = HAL_HIBYTE(65536 - init->Period);
    RL3 = HAL_LOBYTE(65536 - init->Period);
    
    /* Clear flag */
    T3CON &= ~0x10;  /* TF3 = 0 */
    
    return HAL_OK;
}

/**
 * @brief  Start Timer
 */
HAL_StatusTypeDef HAL_Timer_Start(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            TF0 = 0;
            TR0 = 1;
            break;
            
        case HAL_TIMER1:
            TF1 = 0;
            TR1 = 1;
            break;
            
        case HAL_TIMER2:
            TF2 = 0;
            TR2 = 1;
            break;
            
        case HAL_TIMER3:
            HAL_SFR_PAGE0();
            T3CON &= ~0x10;  /* Clear TF3 */
            T3CON |= 0x08;   /* TR3 = 1 */
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Stop Timer
 */
HAL_StatusTypeDef HAL_Timer_Stop(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            TR0 = 0;
            break;
            
        case HAL_TIMER1:
            TR1 = 0;
            break;
            
        case HAL_TIMER2:
            TR2 = 0;
            break;
            
        case HAL_TIMER3:
            HAL_SFR_PAGE0();
            T3CON &= ~0x08;  /* TR3 = 0 */
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Enable Timer interrupt
 */
void HAL_Timer_EnableInterrupt(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            ET0 = 1;
            break;
            
        case HAL_TIMER1:
            ET1 = 1;
            break;
            
        case HAL_TIMER2:
            EIE |= 0x80;  /* ET2 = 1 */
            break;
            
        case HAL_TIMER3:
            EIE1 |= 0x02; /* ET3 = 1 */
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Disable Timer interrupt
 */
void HAL_Timer_DisableInterrupt(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            ET0 = 0;
            break;
            
        case HAL_TIMER1:
            ET1 = 0;
            break;
            
        case HAL_TIMER2:
            EIE &= ~0x80;  /* ET2 = 0 */
            break;
            
        case HAL_TIMER3:
            EIE1 &= ~0x02; /* ET3 = 0 */
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Get Timer interrupt flag
 */
uint8_t HAL_Timer_GetFlag(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            return TF0;
            
        case HAL_TIMER1:
            return TF1;
            
        case HAL_TIMER2:
            return TF2;
            
        case HAL_TIMER3:
            HAL_SFR_PAGE0();
            return (T3CON & 0x10) ? 1 : 0;
            
        default:
            return 0;
    }
}

/**
 * @brief  Clear Timer interrupt flag
 */
void HAL_Timer_ClearFlag(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            TF0 = 0;
            break;
            
        case HAL_TIMER1:
            TF1 = 0;
            break;
            
        case HAL_TIMER2:
            TF2 = 0;
            break;
            
        case HAL_TIMER3:
            HAL_SFR_PAGE0();
            T3CON &= ~0x10;
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Set Timer period/reload value
 */
HAL_StatusTypeDef HAL_Timer_SetPeriod(HAL_Timer_InstanceTypeDef timer, uint16_t period)
{
    uint16_t reload = 65536 - period;
    
    switch (timer) {
        case HAL_TIMER0:
            g_timer0_th = HAL_HIBYTE(reload);
            g_timer0_tl = HAL_LOBYTE(reload);
            TH0 = g_timer0_th;
            TL0 = g_timer0_tl;
            break;
            
        case HAL_TIMER1:
            g_timer1_th = HAL_HIBYTE(reload);
            g_timer1_tl = HAL_LOBYTE(reload);
            TH1 = g_timer1_th;
            TL1 = g_timer1_tl;
            break;
            
        case HAL_TIMER2:
            TH2 = HAL_HIBYTE(reload);
            TL2 = HAL_LOBYTE(reload);
            RCMP2H = TH2;
            RCMP2L = TL2;
            break;
            
        case HAL_TIMER3:
            HAL_SFR_PAGE0();
            RH3 = HAL_HIBYTE(reload);
            RL3 = HAL_LOBYTE(reload);
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Get Timer current count value
 */
uint16_t HAL_Timer_GetCount(HAL_Timer_InstanceTypeDef timer)
{
    switch (timer) {
        case HAL_TIMER0:
            return HAL_MAKEWORD(TH0, TL0);
            
        case HAL_TIMER1:
            return HAL_MAKEWORD(TH1, TL1);
            
        case HAL_TIMER2:
            return HAL_MAKEWORD(TH2, TL2);
            
        case HAL_TIMER3:
            HAL_SFR_PAGE0();
            return HAL_MAKEWORD(RH3, RL3);
            
        default:
            return 0;
    }
}

/*******************************************************************************
 * Delay Functions
 ******************************************************************************/

/**
 * @brief  Blocking delay using Timer0 (milliseconds)
 */
void HAL_Timer_DelayMs(uint32_t sysclk, uint16_t ms)
{
    uint16_t i;
    uint16_t timerReload;
    
    /* Calculate timer reload for 1ms at Fsys/12 clock 
     * Formula: reload = 65536 - (sysclk / 12 / 1000)
     * At 24MHz: reload = 65536 - (24000000 / 12 / 1000) = 65536 - 2000 = 63536
     */
    timerReload = (uint16_t)(65536 - ((sysclk / 12) / 1000));
    
    /* Save timer state */
    TMOD &= 0xF0;
    TMOD |= 0x01;       /* Mode 1 (16-bit) */
    CKCON &= ~0x08;     /* T0M = 0, Fsys/12 */
    
    for (i = 0; i < ms; i++) {
        TH0 = HAL_HIBYTE(timerReload);
        TL0 = HAL_LOBYTE(timerReload);
        TF0 = 0;
        TR0 = 1;
        while (!TF0);
        TR0 = 0;
    }
}

/**
 * @brief  Blocking delay using Timer0 (microseconds)
 */
void HAL_Timer_DelayUs(uint32_t sysclk, uint16_t us)
{
    uint16_t timerReload;
    
    if (us == 0) return;
    
    /* Calculate timer reload for us at Fsys clock */
    timerReload = (uint16_t)(65536 - ((sysclk / 1000000UL) * us));
    
    /* Configure timer */
    TMOD &= 0xF0;
    TMOD |= 0x01;       /* Mode 1 (16-bit) */
    CKCON |= 0x08;      /* T0M = 1, Fsys */
    
    TH0 = HAL_HIBYTE(timerReload);
    TL0 = HAL_LOBYTE(timerReload);
    TF0 = 0;
    TR0 = 1;
    while (!TF0);
    TR0 = 0;
}

/**
 * @brief  Generic software delay loop
 */
void HAL_Timer_SoftDelay(uint16_t count)
{
    uint16_t i;
    for (i = 0; i < count; i++) {
        HAL_NOP();
    }
}

/**
 * @brief  Initialize Timer for interrupt-based delay
 */
HAL_StatusTypeDef HAL_Timer_InitInterruptDelay(HAL_Timer_InstanceTypeDef timer,
                                                uint32_t sysclk, uint32_t intervalUs)
{
    uint16_t reload;
    HAL_Timer01_InitTypeDef init;
    HAL_Timer2_InitTypeDef init2;
    
    /* Calculate reload value for given interval */
    /* Using Fsys/12 clock */
    reload = (uint16_t)((sysclk / 12) * intervalUs / 1000000UL);
    
    switch (timer) {
        case HAL_TIMER0:
            init.Mode = HAL_TIMER_MODE1;
            init.ClockSource = HAL_TIMER_CLK_FSYS_DIV12;
            init.Period = reload;
            init.EnableGate = 0;
            init.CounterMode = 0;
            HAL_Timer0_Init(&init);
            HAL_Timer_EnableInterrupt(HAL_TIMER0);
            break;
            
        case HAL_TIMER1:
            init.Mode = HAL_TIMER_MODE1;
            init.ClockSource = HAL_TIMER_CLK_FSYS_DIV12;
            init.Period = reload;
            init.EnableGate = 0;
            init.CounterMode = 0;
            HAL_Timer1_Init(&init);
            HAL_Timer_EnableInterrupt(HAL_TIMER1);
            break;
            
        case HAL_TIMER2:
            init2.Mode = HAL_TIMER2_AUTORELOAD;
            init2.Divider = HAL_TIMER2_DIV_1;
            init2.Period = reload * 12;  /* Adjust for Fsys clock */
            HAL_Timer2_Init(&init2);
            HAL_Timer_EnableInterrupt(HAL_TIMER2);
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Initialize Timer2 capture mode
 */
HAL_StatusTypeDef HAL_Timer2_CaptureInit(uint8_t captureChannel, uint8_t edgeMode)
{
    /* Stop Timer2 first */
    TR2 = 0;
    
    /* Set capture mode */
    T2CON = 0x01;  /* CM_RL2 = 1 for capture mode */
    
    /* Configure capture channel */
    switch (captureChannel) {
        case 0:
            CAPCON0 |= 0x10;  /* CAPEN0 = 1 */
            CAPCON1 &= ~0x03;
            CAPCON1 |= (edgeMode & 0x03);
            break;
            
        case 1:
            CAPCON0 |= 0x20;  /* CAPEN1 = 1 */
            CAPCON1 &= ~0x0C;
            CAPCON1 |= ((edgeMode & 0x03) << 2);
            break;
            
        case 2:
            CAPCON0 |= 0x40;  /* CAPEN2 = 1 */
            CAPCON1 &= ~0x30;
            CAPCON1 |= ((edgeMode & 0x03) << 4);
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Get Timer2 capture value
 */
uint16_t HAL_Timer2_GetCaptureValue(uint8_t captureChannel)
{
    switch (captureChannel) {
        case 0:
            return HAL_MAKEWORD(C0H, C0L);
            
        case 1:
            return HAL_MAKEWORD(C1H, C1L);
            
        case 2:
            return HAL_MAKEWORD(C2H, C2L);
            
        default:
            return 0;
    }
}

#endif /* HAL_TIMER_MODULE_ENABLED */
