/*===========================================================================
 * @file    svc_delay.c
 * @brief   Delay Service Implementation
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "svc_delay.h"
#include "drv_timer.h"

/*---------------------------------------------------------------------------
 * Local Variables
 *---------------------------------------------------------------------------*/
static volatile uint32_t g_tick_count = 0;
static uint32_t g_sys_clock = HIRC_24MHZ;
static delay_timer_t g_delay_timer = DELAY_TIMER_NONE;
static timer_handle_t g_timer_handle;

/*---------------------------------------------------------------------------
 * Timer Callback for System Tick
 *---------------------------------------------------------------------------*/
static void delay_timer_callback(uint8_t event, void *param)
{
    (void)event;
    (void)param;
    g_tick_count++;
}

/*===========================================================================
 * Public API Implementation
 *===========================================================================*/

drv_status_t SVC_DELAY_Init(const delay_config_t *config)
{
    timer_config_t timer_cfg;
    drv_status_t status;

    if (config != NULL) {
        g_delay_timer = config->timer;
        g_sys_clock = config->sys_clock;
    } else {
        g_delay_timer = DELAY_TIMER_NONE;
        g_sys_clock = HIRC_24MHZ;
    }

    g_tick_count = 0;

    /* If using hardware timer for ticks, configure it */
    if (g_delay_timer != DELAY_TIMER_NONE) {
        timer_cfg.timer = (timer_id_t)(g_delay_timer - 1);
        timer_cfg.mode = TIMER_MODE_AUTO_RELOAD;
        timer_cfg.clock_div = TIMER_CLK_DIV_12;
        timer_cfg.reload_value = (uint16_t)(g_sys_clock / 12 / 1000); /* 1ms tick */
        timer_cfg.int_enable = 1;
        timer_cfg.callback = delay_timer_callback;
        timer_cfg.callback_param = NULL;

        status = DRV_TIMER_Init(&g_timer_handle, &timer_cfg);
        if (status != DRV_OK) {
            return status;
        }

        status = DRV_TIMER_Start(&g_timer_handle);
        if (status != DRV_OK) {
            return status;
        }
    }

    return DRV_OK;
}

drv_status_t SVC_DELAY_DeInit(void)
{
    if (g_delay_timer != DELAY_TIMER_NONE) {
        DRV_TIMER_Stop(&g_timer_handle);
        DRV_TIMER_DeInit(&g_timer_handle);
    }
    
    g_tick_count = 0;
    g_delay_timer = DELAY_TIMER_NONE;
    
    return DRV_OK;
}

uint32_t SVC_DELAY_GetTick(void)
{
    return g_tick_count;
}

void SVC_DELAY_SetTick(uint32_t tick)
{
    g_tick_count = tick;
}

/*---------------------------------------------------------------------------
 * Blocking Delays
 *---------------------------------------------------------------------------*/
void SVC_DELAY_Us(uint16_t us)
{
    /* Calculate loop count based on system clock */
    /* At 24MHz, ~6 cycles per loop iteration */
    uint16_t loops = (uint16_t)((g_sys_clock / 1000000UL) * us / 6);
    
    while (loops--) {
        CALL_NOP;
    }
}

void SVC_DELAY_Ms(uint16_t ms)
{
    if (g_delay_timer != DELAY_TIMER_NONE) {
        /* Use hardware timer ticks */
        uint32_t start = g_tick_count;
        while ((g_tick_count - start) < ms) {
            /* Wait for tick count to reach target */
        }
    } else {
        /* Use software delay */
        while (ms--) {
            SVC_DELAY_Us(1000);
        }
    }
}

void SVC_DELAY_Sec(uint16_t sec)
{
    while (sec--) {
        SVC_DELAY_Ms(1000);
    }
}

/*---------------------------------------------------------------------------
 * Non-Blocking Timeout Support
 *---------------------------------------------------------------------------*/
uint8_t SVC_DELAY_IsTimeout(uint32_t start_tick, uint32_t timeout_ms)
{
    uint32_t elapsed = g_tick_count - start_tick;
    return (elapsed >= timeout_ms) ? 1U : 0U;
}

uint32_t SVC_DELAY_GetElapsed(uint32_t start_tick)
{
    return g_tick_count - start_tick;
}

/*---------------------------------------------------------------------------
 * Simple Software Delays
 *---------------------------------------------------------------------------*/
void SVC_DELAY_SoftUs(uint16_t us)
{
    /* Simple NOP-based delay */
    /* Adjust multiplier based on system clock */
    uint16_t loops = (uint16_t)(us * (g_sys_clock / 6000000UL));
    
    while (loops--) {
        CALL_NOP;
        CALL_NOP;
    }
}

void SVC_DELAY_SoftMs(uint16_t ms)
{
    while (ms--) {
        SVC_DELAY_SoftUs(1000);
    }
}
