/*===========================================================================
 * @file    svc_power.c
 * @brief   Power Management Service Implementation
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "svc_power.h"
#include "ms51_hal.h"

/*---------------------------------------------------------------------------
 * Local Variables
 *---------------------------------------------------------------------------*/
static power_config_t g_config;
static wakeup_source_t g_last_wakeup = WAKEUP_SRC_NONE;
static uint8_t g_initialized = 0;

/*===========================================================================
 * Public API Implementation
 *===========================================================================*/

drv_status_t SVC_POWER_Init(const power_config_t *config)
{
    if (config != NULL) {
        g_config = *config;
    } else {
        g_config.wakeup_sources = WAKEUP_SRC_ALL;
        g_config.wkt_config.clock = WKT_CLK_LIRC;
        g_config.wkt_config.period_ms = 1000;
        g_config.gpio_retain = 1;
        g_config.bod_enable = 0;
        g_config.lvr_enable = 1;
    }

    /* Configure BOD if enabled */
    if (g_config.bod_enable) {
        SVC_POWER_EnableBOD();
    } else {
        SVC_POWER_DisableBOD();
    }

    g_last_wakeup = WAKEUP_SRC_NONE;
    g_initialized = 1;
    
    return DRV_OK;
}

drv_status_t SVC_POWER_DeInit(void)
{
    SVC_POWER_StopWKT();
    g_initialized = 0;
    return DRV_OK;
}

drv_status_t SVC_POWER_EnterMode(power_mode_t mode)
{
    switch (mode) {
        case POWER_MODE_NORMAL:
            return DRV_OK;
            
        case POWER_MODE_IDLE:
            SVC_POWER_EnterIdle();
            return DRV_OK;
            
        case POWER_MODE_POWERDOWN:
            SVC_POWER_EnterPowerDown();
            return DRV_OK;
            
        default:
            return DRV_INVALID_PARAM;
    }
}

void SVC_POWER_EnterIdle(void)
{
    set_PCON_IDLE;
    /* Returns here after any interrupt */
}

void SVC_POWER_EnterPowerDown(void)
{
    /* Clear any pending wake-up flags */
    WKCON &= ~0x10;  /* Clear WKTF */
    
    set_PCON_PD;
    
    /* Returns here after wake-up */
    /* Determine wake-up source */
    if (WKCON & 0x10) {
        g_last_wakeup = WAKEUP_SRC_TIMER;
    } else if (PIF != 0) {
        g_last_wakeup = WAKEUP_SRC_PIN;
    } else {
        g_last_wakeup = WAKEUP_SRC_NONE;
    }
}

wakeup_source_t SVC_POWER_GetWakeupSource(void)
{
    return g_last_wakeup;
}

/*---------------------------------------------------------------------------
 * Wake-up Timer Control
 *---------------------------------------------------------------------------*/
drv_status_t SVC_POWER_ConfigWKT(const wkt_config_t *config)
{
    uint16_t reload;
    
    DRV_CHECK_PTR(config);
    
    g_config.wkt_config = *config;
    
    /* Calculate reload value */
    if (config->clock == WKT_CLK_LIRC) {
        /* LIRC ~10 kHz, so 10 counts = 1ms */
        reload = (uint16_t)(config->period_ms * 10);
    } else {
        /* LXT 32.768 kHz, ~33 counts = 1ms */
        reload = (uint16_t)((uint32_t)config->period_ms * 33 / 1);
    }
    
    /* Limit to 16-bit range */
    if (reload > 0xFF00) {
        reload = 0xFF00;
    }
    
    /* Configure WKT */
    WKCON &= ~0x07;  /* Clear clock source bits */
    
    if (config->clock == WKT_CLK_LXT) {
        WKCON |= 0x02;  /* Select LXT */
    }
    
    /* Set reload value (RWK register) */
    RWK = (uint8_t)(256 - (reload & 0xFF));
    
    return DRV_OK;
}

drv_status_t SVC_POWER_StartWKT(void)
{
    WKCON |= 0x08;  /* Enable WKT */
    EIE1 |= 0x04;   /* Enable WKT interrupt */
    
    return DRV_OK;
}

drv_status_t SVC_POWER_StopWKT(void)
{
    WKCON &= ~0x08;  /* Disable WKT */
    EIE1 &= ~0x04;   /* Disable WKT interrupt */
    
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Brown-Out Detection
 *---------------------------------------------------------------------------*/
drv_status_t SVC_POWER_ConfigBOD(bod_level_t level, uint8_t reset)
{
    uint8_t bodcon0_val;
    
    /* Read current BODCON0 */
    bodcon0_val = BODCON0 & ~0x07;  /* Clear BOD level bits */
    
    /* Set BOD level */
    bodcon0_val |= (level & 0x03);
    
    /* Set reset or interrupt mode */
    if (reset) {
        bodcon0_val |= 0x04;  /* BORF = 1, reset on BOD */
    }
    
    /* Write to TA-protected register */
    HAL_TA_WRITE(BODCON0, bodcon0_val);
    
    return DRV_OK;
}

drv_status_t SVC_POWER_EnableBOD(void)
{
    uint8_t val = BODCON0 | 0x80;  /* BODEN = 1 */
    HAL_TA_WRITE(BODCON0, val);
    
    return DRV_OK;
}

drv_status_t SVC_POWER_DisableBOD(void)
{
    uint8_t val = BODCON0 & ~0x80;  /* BODEN = 0 */
    HAL_TA_WRITE(BODCON0, val);
    
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Power-on Reset Control
 *---------------------------------------------------------------------------*/
uint8_t SVC_POWER_IsPOR(void)
{
    return (PCON & 0x10) ? 1U : 0U;  /* POF bit */
}

void SVC_POWER_ClearPOR(void)
{
    PCON &= ~0x10;  /* Clear POF bit */
}
