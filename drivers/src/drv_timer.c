/*===========================================================================
 * @file    drv_timer.c
 * @brief   Timer Driver Implementation
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_timer.h"

/*---------------------------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------------------------*/
static timer_handle_t *timer0_handle = NULL;
static timer_handle_t *timer1_handle = NULL;
static timer_handle_t *timer2_handle = NULL;
static timer_handle_t *timer3_handle = NULL;
static timer_handle_t *wkt_handle = NULL;

/* Reload values storage */
static uint16_t timer0_reload = 0;
static uint16_t timer1_reload = 0;
static uint16_t timer2_reload = 0;
static uint16_t timer3_reload = 0;

/*---------------------------------------------------------------------------
 * System Clock Definition
 *---------------------------------------------------------------------------*/
#ifndef FSYS_HZ
#define FSYS_HZ     24000000UL
#endif

/*===========================================================================
 * Public Functions
 *===========================================================================*/

/**
 * @brief  Initialize timer peripheral
 */
drv_status_t DRV_Timer_Init(timer_handle_t *handle, const timer_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    /* Initialize handle */
    handle->instance = config->instance;
    handle->callback = config->callback;
    handle->overflow_cnt = 0;
    handle->running = 0;
    
    switch (config->instance) {
        case TIMER_0:
            timer0_handle = handle;
            timer0_reload = config->period;
            
            /* Set timer mode */
            TMOD = (TMOD & 0xF0) | (config->mode & 0x03);
            
            /* Configure clock divider */
            if (config->prescaler == TIMER_PSC_12) {
                clr_CKCON_T0M;  /* Fsys/12 */
            } else {
                set_CKCON_T0M;  /* Fsys/1 */
            }
            
            /* Load timer value */
            TL0 = (uint8_t)(config->period & 0xFF);
            TH0 = (uint8_t)(config->period >> 8);
            
            /* Enable interrupt if requested */
            if (config->enable_int) {
                set_IE_ET0;
            }
            break;
            
        case TIMER_1:
            timer1_handle = handle;
            timer1_reload = config->period;
            
            /* Set timer mode */
            TMOD = (TMOD & 0x0F) | ((config->mode & 0x03) << 4);
            
            /* Configure clock divider */
            if (config->prescaler == TIMER_PSC_12) {
                clr_CKCON_T1M;
            } else {
                set_CKCON_T1M;
            }
            
            /* Load timer value */
            TL1 = (uint8_t)(config->period & 0xFF);
            TH1 = (uint8_t)(config->period >> 8);
            
            /* Enable interrupt if requested */
            if (config->enable_int) {
                set_IE_ET1;
            }
            break;
            
        case TIMER_2:
            timer2_handle = handle;
            timer2_reload = config->period;
            
            /* Timer2 is always 16-bit auto-reload */
            /* Load reload registers */
            RCMP2L = (uint8_t)(config->period & 0xFF);
            RCMP2H = (uint8_t)(config->period >> 8);
            
            /* Load timer value */
            TL2 = (uint8_t)(config->period & 0xFF);
            TH2 = (uint8_t)(config->period >> 8);
            
            /* Clear flags */
            clr_T2CON_TF2;
            
            /* Enable interrupt if requested */
            if (config->enable_int) {
                set_EIE_ET2;
            }
            break;
            
        case TIMER_3:
            timer3_handle = handle;
            timer3_reload = config->period;
            
            /* Timer3 configuration */
            T3CON = 0x00;
            
            /* Load reload registers */
            RH3 = (uint8_t)(config->period >> 8);
            RL3 = (uint8_t)(config->period & 0xFF);
            
            /* Enable interrupt if requested */
            if (config->enable_int) {
                set_EIE1_ET3;
            }
            break;
            
        case TIMER_WKT:
            return DRV_WKT_Init(handle, config->period, config->callback);
            
        default:
            return DRV_INVALID_PARAM;
    }
    
    return DRV_OK;
}

/**
 * @brief  De-initialize timer peripheral
 */
drv_status_t DRV_Timer_DeInit(timer_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Stop timer first */
    DRV_Timer_Stop(handle);
    
    switch (handle->instance) {
        case TIMER_0:
            clr_IE_ET0;
            timer0_handle = NULL;
            break;
            
        case TIMER_1:
            clr_IE_ET1;
            timer1_handle = NULL;
            break;
            
        case TIMER_2:
            clr_EIE_ET2;
            timer2_handle = NULL;
            break;
            
        case TIMER_3:
            clr_EIE1_ET3;
            timer3_handle = NULL;
            break;
            
        case TIMER_WKT:
            DRV_WKT_Stop(handle);
            wkt_handle = NULL;
            break;
            
        default:
            break;
    }
    
    return DRV_OK;
}

/**
 * @brief  Start timer
 */
drv_status_t DRV_Timer_Start(timer_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    switch (handle->instance) {
        case TIMER_0:
            set_TCON_TR0;
            break;
            
        case TIMER_1:
            set_TCON_TR1;
            break;
            
        case TIMER_2:
            set_T2CON_TR2;
            break;
            
        case TIMER_3:
            set_T3CON_TR3;
            break;
            
        case TIMER_WKT:
            return DRV_WKT_Start(handle);
            
        default:
            return DRV_INVALID_PARAM;
    }
    
    handle->running = 1;
    return DRV_OK;
}

/**
 * @brief  Stop timer
 */
drv_status_t DRV_Timer_Stop(timer_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    switch (handle->instance) {
        case TIMER_0:
            clr_TCON_TR0;
            break;
            
        case TIMER_1:
            clr_TCON_TR1;
            break;
            
        case TIMER_2:
            clr_T2CON_TR2;
            break;
            
        case TIMER_3:
            clr_T3CON_TR3;
            break;
            
        case TIMER_WKT:
            return DRV_WKT_Stop(handle);
            
        default:
            return DRV_INVALID_PARAM;
    }
    
    handle->running = 0;
    return DRV_OK;
}

/**
 * @brief  Reset timer counter
 */
drv_status_t DRV_Timer_Reset(timer_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    switch (handle->instance) {
        case TIMER_0:
            TL0 = (uint8_t)(timer0_reload & 0xFF);
            TH0 = (uint8_t)(timer0_reload >> 8);
            break;
            
        case TIMER_1:
            TL1 = (uint8_t)(timer1_reload & 0xFF);
            TH1 = (uint8_t)(timer1_reload >> 8);
            break;
            
        case TIMER_2:
            TL2 = (uint8_t)(timer2_reload & 0xFF);
            TH2 = (uint8_t)(timer2_reload >> 8);
            break;
            
        case TIMER_3:
            /* Timer3 uses auto-reload, just write current value */
            break;
            
        default:
            return DRV_INVALID_PARAM;
    }
    
    handle->overflow_cnt = 0;
    return DRV_OK;
}

/**
 * @brief  Get current timer count
 */
uint16_t DRV_Timer_GetCount(timer_handle_t *handle)
{
    uint16_t count = 0;
    
    if (handle == NULL) {
        return 0;
    }
    
    switch (handle->instance) {
        case TIMER_0:
            count = (uint16_t)TL0 | ((uint16_t)TH0 << 8);
            break;
            
        case TIMER_1:
            count = (uint16_t)TL1 | ((uint16_t)TH1 << 8);
            break;
            
        case TIMER_2:
            count = (uint16_t)TL2 | ((uint16_t)TH2 << 8);
            break;
            
        case TIMER_3:
            /* Timer3 count not directly readable in some modes */
            break;
            
        default:
            break;
    }
    
    return count;
}

/**
 * @brief  Set timer period
 */
drv_status_t DRV_Timer_SetPeriod(timer_handle_t *handle, uint16_t period)
{
    DRV_CHECK_PTR(handle);
    
    switch (handle->instance) {
        case TIMER_0:
            timer0_reload = period;
            TL0 = (uint8_t)(period & 0xFF);
            TH0 = (uint8_t)(period >> 8);
            break;
            
        case TIMER_1:
            timer1_reload = period;
            TL1 = (uint8_t)(period & 0xFF);
            TH1 = (uint8_t)(period >> 8);
            break;
            
        case TIMER_2:
            timer2_reload = period;
            RCMP2L = (uint8_t)(period & 0xFF);
            RCMP2H = (uint8_t)(period >> 8);
            break;
            
        case TIMER_3:
            timer3_reload = period;
            RH3 = (uint8_t)(period >> 8);
            RL3 = (uint8_t)(period & 0xFF);
            break;
            
        default:
            return DRV_INVALID_PARAM;
    }
    
    return DRV_OK;
}

/**
 * @brief  Get overflow count
 */
uint32_t DRV_Timer_GetOverflows(timer_handle_t *handle)
{
    if (handle == NULL) {
        return 0;
    }
    return handle->overflow_cnt;
}

/**
 * @brief  Register timer callback
 */
drv_status_t DRV_Timer_RegisterCallback(timer_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    handle->callback = callback;
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Wakeup Timer Functions
 *---------------------------------------------------------------------------*/

/**
 * @brief  Initialize Wakeup Timer
 */
drv_status_t DRV_WKT_Init(timer_handle_t *handle, uint32_t period_ms, 
                          drv_callback_t callback)
{
    uint16_t wkt_count;
    
    DRV_CHECK_PTR(handle);
    
    handle->instance = TIMER_WKT;
    handle->callback = callback;
    handle->overflow_cnt = 0;
    handle->running = 0;
    
    wkt_handle = handle;
    
    /* Calculate WKT counter value */
    /* WKT uses LIRC (10kHz) - period in 100us units */
    wkt_count = (uint16_t)((period_ms * 10) & 0x3FFF);
    
    /* Configure WKT */
    WKCON = 0x00;  /* Clear configuration */
    
    /* Set WKT counter */
    RWK = (uint8_t)(wkt_count & 0xFF);
    /* High byte in WKCON[3:0] if needed */
    
    /* Enable WKT interrupt */
    set_EIE_EWKT;
    
    return DRV_OK;
}

/**
 * @brief  Start Wakeup Timer
 */
drv_status_t DRV_WKT_Start(timer_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Enable WKT */
    set_WKCON_WKTR;
    
    handle->running = 1;
    return DRV_OK;
}

/**
 * @brief  Stop Wakeup Timer
 */
drv_status_t DRV_WKT_Stop(timer_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    /* Disable WKT */
    clr_WKCON_WKTR;
    
    handle->running = 0;
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Delay Functions
 *---------------------------------------------------------------------------*/

/**
 * @brief  Blocking delay in microseconds
 */
void DRV_Timer_DelayUs(uint16_t us)
{
    /* Simple loop-based delay */
    /* Calibrated for 24MHz, adjust for other frequencies */
    uint16_t count = us * 4;  /* ~4 cycles per loop at 24MHz */
    
    while (count--) {
        NOP;
    }
}

/**
 * @brief  Blocking delay in milliseconds
 */
void DRV_Timer_DelayMs(uint16_t ms)
{
    while (ms--) {
        DRV_Timer_DelayUs(1000);
    }
}

/*===========================================================================
 * Timer Interrupt Service Routines
 *===========================================================================*/

/**
 * @brief  Timer0 Overflow ISR
 */
#if defined(COMPILER_KEIL)
void Timer0_ISR(void) interrupt 1
#elif defined(COMPILER_IAR)
#pragma vector = 0x0B
__interrupt void Timer0_ISR(void)
#elif defined(COMPILER_SDCC)
void Timer0_ISR(void) __interrupt(1)
#endif
#if !defined(COMPILER_GCC)
{
    timer_handle_t *handle = timer0_handle;
    
    /* Reload timer value (Mode 1 is not auto-reload) */
    TL0 = (uint8_t)(timer0_reload & 0xFF);
    TH0 = (uint8_t)(timer0_reload >> 8);
    
    if (handle) {
        handle->overflow_cnt++;
        
        if (handle->callback) {
            handle->callback(TIMER_EVT_OVERFLOW, NULL);
        }
    }
}
#endif

/**
 * @brief  Timer1 Overflow ISR
 */
#if defined(COMPILER_KEIL)
void Timer1_ISR(void) interrupt 3
#elif defined(COMPILER_IAR)
#pragma vector = 0x1B
__interrupt void Timer1_ISR(void)
#elif defined(COMPILER_SDCC)
void Timer1_ISR(void) __interrupt(3)
#endif
#if !defined(COMPILER_GCC)
{
    timer_handle_t *handle = timer1_handle;
    
    /* Reload timer value */
    TL1 = (uint8_t)(timer1_reload & 0xFF);
    TH1 = (uint8_t)(timer1_reload >> 8);
    
    if (handle) {
        handle->overflow_cnt++;
        
        if (handle->callback) {
            handle->callback(TIMER_EVT_OVERFLOW, NULL);
        }
    }
}
#endif

/**
 * @brief  Timer2 Overflow ISR
 */
#if defined(COMPILER_KEIL)
void Timer2_ISR(void) interrupt 5
#elif defined(COMPILER_IAR)
#pragma vector = 0x2B
__interrupt void Timer2_ISR(void)
#elif defined(COMPILER_SDCC)
void Timer2_ISR(void) __interrupt(5)
#endif
#if !defined(COMPILER_GCC)
{
    timer_handle_t *handle = timer2_handle;
    
    /* Clear overflow flag */
    clr_T2CON_TF2;
    
    if (handle) {
        handle->overflow_cnt++;
        
        if (handle->callback) {
            handle->callback(TIMER_EVT_OVERFLOW, NULL);
        }
    }
}
#endif

/**
 * @brief  Wakeup Timer ISR
 */
#if defined(COMPILER_KEIL)
void WKT_ISR(void) interrupt 17
#elif defined(COMPILER_IAR)
#pragma vector = 0x8B
__interrupt void WKT_ISR(void)
#elif defined(COMPILER_SDCC)
void WKT_ISR(void) __interrupt(17)
#endif
#if !defined(COMPILER_GCC)
{
    timer_handle_t *handle = wkt_handle;
    
    /* Clear WKT flag */
    clr_WKCON_WKTF;
    
    if (handle) {
        handle->overflow_cnt++;
        
        if (handle->callback) {
            handle->callback(TIMER_EVT_OVERFLOW, NULL);
        }
    }
}
#endif
