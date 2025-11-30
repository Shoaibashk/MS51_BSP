/*===========================================================================
 * @file    svc_system.c
 * @brief   System Service Implementation
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "svc_system.h"
#include "drv_wdt.h"

/*---------------------------------------------------------------------------
 * Local Variables
 *---------------------------------------------------------------------------*/
static system_config_t g_config;
static uint32_t g_sys_clock = HIRC_24MHZ;
static reset_source_t g_reset_source = RESET_SRC_UNKNOWN;
static wdt_handle_t g_wdt_handle;
static uint8_t g_wdt_enabled = 0;
static uint8_t g_initialized = 0;

/*---------------------------------------------------------------------------
 * Clock Frequency Table
 *---------------------------------------------------------------------------*/
static const uint32_t g_clock_freq[] = {
    24000000UL,     /* HIRC 24 MHz */
    16000000UL,     /* HIRC 16 MHz */
    16600000UL,     /* HIRC 16.6 MHz */
    10000UL,        /* LIRC 10 kHz */
    0               /* ECLK - external, set by user */
};

/*===========================================================================
 * Public API Implementation
 *===========================================================================*/

drv_status_t SVC_SYSTEM_Init(const system_config_t *config)
{
    drv_status_t status;

    /* Determine reset source from power-on flags */
    if (PCON & 0x10) {
        g_reset_source = RESET_SRC_POR;
    } else if (WDCON & 0x08) {
        g_reset_source = RESET_SRC_WDT;
        WDCON &= ~0x08;  /* Clear WDT reset flag */
    } else if (BODCON0 & 0x08) {
        g_reset_source = RESET_SRC_BOD;
    } else {
        g_reset_source = RESET_SRC_EXTERNAL;
    }

    if (config != NULL) {
        g_config = *config;
    } else {
        g_config.clock_source = CLK_SRC_HIRC_24MHZ;
        g_config.clock_divider = 0;
        g_config.wdt_enable = 0;
        g_config.wdt_timeout_ms = 1000;
    }

    /* Configure clock source */
    status = SVC_SYSTEM_SetClockSource(g_config.clock_source);
    if (status != DRV_OK) return status;

    /* Configure clock divider */
    status = SVC_SYSTEM_SetClockDivider(g_config.clock_divider);
    if (status != DRV_OK) return status;

    /* Initialize watchdog if enabled */
    if (g_config.wdt_enable) {
        status = SVC_SYSTEM_WDT_Start(g_config.wdt_timeout_ms);
        if (status != DRV_OK) return status;
    }

    g_initialized = 1;
    return DRV_OK;
}

drv_status_t SVC_SYSTEM_DeInit(void)
{
    if (g_wdt_enabled) {
        SVC_SYSTEM_WDT_Stop();
    }
    g_initialized = 0;
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Reset Control
 *---------------------------------------------------------------------------*/
void SVC_SYSTEM_Reset(void)
{
    HAL_ENTER_CRITICAL();
    
    /* Trigger software reset via CHPCON */
    TA = 0xAA;
    TA = 0x55;
    CHPCON |= 0x80;  /* SWRST = 1 */
    
    HAL_EXIT_CRITICAL();
    
    /* Never reaches here */
    while (1);
}

void SVC_SYSTEM_ResetToAPROM(void)
{
    HAL_SYS_ResetToAPROM();
}

void SVC_SYSTEM_ResetToLDROM(void)
{
    HAL_SYS_ResetToLDROM();
}

reset_source_t SVC_SYSTEM_GetResetSource(void)
{
    return g_reset_source;
}

void SVC_SYSTEM_ClearResetSource(void)
{
    g_reset_source = RESET_SRC_UNKNOWN;
    
    /* Clear all reset flags */
    PCON &= ~0x10;      /* Clear POF */
    WDCON &= ~0x08;     /* Clear WDTRF */
}

/*---------------------------------------------------------------------------
 * Clock Management
 *---------------------------------------------------------------------------*/
drv_status_t SVC_SYSTEM_SetClockSource(clock_source_t source)
{
    uint8_t cken_val = CKEN;
    uint8_t ckswt_val;
    
    switch (source) {
        case CLK_SRC_HIRC_24MHZ:
            HAL_SYS_SetHIRC(HIRC_24MHZ);
            g_sys_clock = HIRC_24MHZ;
            break;
            
        case CLK_SRC_HIRC_16MHZ:
            HAL_SYS_SetHIRC(HIRC_16MHZ);
            g_sys_clock = HIRC_16MHZ;
            break;
            
        case CLK_SRC_HIRC_166MHZ:
            HAL_SYS_SetHIRC(HIRC_166MHZ);
            g_sys_clock = HIRC_166MHZ;
            break;
            
        case CLK_SRC_LIRC:
            cken_val |= 0x02;  /* Enable LIRC */
            HAL_TA_WRITE(CKEN, cken_val);
            
            ckswt_val = 0x01;  /* Switch to LIRC */
            HAL_TA_WRITE(CKSWT, ckswt_val);
            
            g_sys_clock = 10000UL;
            break;
            
        case CLK_SRC_ECLK:
            cken_val |= 0x40;  /* Enable external clock */
            HAL_TA_WRITE(CKEN, cken_val);
            
            ckswt_val = 0x04;  /* Switch to ECLK */
            HAL_TA_WRITE(CKSWT, ckswt_val);
            
            /* External clock frequency should be set by user */
            break;
            
        default:
            return DRV_INVALID_PARAM;
    }
    
    g_config.clock_source = source;
    return DRV_OK;
}

uint32_t SVC_SYSTEM_GetClockHz(void)
{
    return g_sys_clock >> g_config.clock_divider;
}

drv_status_t SVC_SYSTEM_SetClockDivider(uint8_t divider)
{
    if (divider > 7) {
        return DRV_INVALID_PARAM;
    }
    
    CKDIV = divider;
    g_config.clock_divider = divider;
    
    return DRV_OK;
}

drv_status_t SVC_SYSTEM_EnableClockOutput(uint8_t divider)
{
    /* Configure P1.0 for clock output */
    P1M1 &= ~0x01;  /* Clear P1.0 mode bits */
    P1M2 |= 0x01;   /* Push-pull output */
    
    /* Set divider in CKDIV high nibble */
    CKDIV = (CKDIV & 0x0F) | ((divider & 0x0F) << 4);
    
    /* Enable clock output */
    CKCON |= 0x02;  /* CLOEN = 1 */
    
    return DRV_OK;
}

drv_status_t SVC_SYSTEM_DisableClockOutput(void)
{
    CKCON &= ~0x02;  /* CLOEN = 0 */
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Watchdog Management
 *---------------------------------------------------------------------------*/
drv_status_t SVC_SYSTEM_WDT_Start(uint16_t timeout_ms)
{
    wdt_config_t wdt_cfg;
    drv_status_t status;

    /* Map timeout to WDT prescaler */
    /* WDT timeout = (2^(17+prescaler)) / Fsys */
    /* At 24MHz: prescaler 0 = 5.5ms, 1 = 11ms, 2 = 22ms, etc. */
    if (timeout_ms <= 11) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_1;
    } else if (timeout_ms <= 22) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_2;
    } else if (timeout_ms <= 44) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_4;
    } else if (timeout_ms <= 88) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_8;
    } else if (timeout_ms <= 175) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_16;
    } else if (timeout_ms <= 350) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_32;
    } else if (timeout_ms <= 700) {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_64;
    } else {
        wdt_cfg.prescaler = WDT_PRESCALE_DIV_128;
    }

    wdt_cfg.reset_enable = 1;
    wdt_cfg.int_enable = 0;
    wdt_cfg.callback = NULL;

    status = DRV_WDT_Init(&g_wdt_handle, &wdt_cfg);
    if (status != DRV_OK) return status;

    status = DRV_WDT_Start(&g_wdt_handle);
    if (status == DRV_OK) {
        g_wdt_enabled = 1;
    }

    return status;
}

drv_status_t SVC_SYSTEM_WDT_Stop(void)
{
    drv_status_t status;
    
    status = DRV_WDT_Stop(&g_wdt_handle);
    if (status == DRV_OK) {
        g_wdt_enabled = 0;
    }
    
    return status;
}

void SVC_SYSTEM_WDT_Kick(void)
{
    if (g_wdt_enabled) {
        DRV_WDT_Kick(&g_wdt_handle);
    }
}

uint8_t SVC_SYSTEM_WDT_IsReset(void)
{
    return (g_reset_source == RESET_SRC_WDT) ? 1U : 0U;
}

/*---------------------------------------------------------------------------
 * Device Information
 *---------------------------------------------------------------------------*/
drv_status_t SVC_SYSTEM_GetUID(uint8_t *uid)
{
    uint8_t i;
    
    DRV_CHECK_PTR(uid);
    
    /* UID is stored at IAP address 0x00-0x0B */
    for (i = 0; i < 12; i++) {
        /* Set IAP address */
        IAPAL = i;
        IAPAH = 0x00;
        
        /* Set command: Read UID */
        IAPCN = 0x04;
        
        /* Trigger IAP */
        TA = 0xAA;
        TA = 0x55;
        IAPTRG |= 0x01;
        
        /* Wait for completion */
        while (IAPTRG & 0x01);
        
        /* Read result */
        uid[i] = IAPFD;
    }
    
    return DRV_OK;
}

drv_status_t SVC_SYSTEM_GetUCID(uint8_t *ucid)
{
    uint8_t i;
    
    DRV_CHECK_PTR(ucid);
    
    /* UCID is stored at IAP address 0x20-0x27 */
    for (i = 0; i < 8; i++) {
        /* Set IAP address */
        IAPAL = 0x20 + i;
        IAPAH = 0x00;
        
        /* Set command: Read UCID */
        IAPCN = 0x04;
        
        /* Trigger IAP */
        TA = 0xAA;
        TA = 0x55;
        IAPTRG |= 0x01;
        
        /* Wait for completion */
        while (IAPTRG & 0x01);
        
        /* Read result */
        ucid[i] = IAPFD;
    }
    
    return DRV_OK;
}

uint32_t SVC_SYSTEM_GetDeviceID(void)
{
    /* Device ID is at address 0x00 of CONFIG */
    uint32_t device_id = 0;
    
    /* Set IAP address */
    IAPAL = 0x00;
    IAPAH = 0x00;
    
    /* Set command: Read Device ID */
    IAPCN = 0x0C;
    
    /* Trigger IAP */
    TA = 0xAA;
    TA = 0x55;
    IAPTRG |= 0x01;
    
    /* Wait for completion */
    while (IAPTRG & 0x01);
    
    device_id = IAPFD;
    
    return device_id;
}

uint32_t SVC_SYSTEM_GetBSPVersion(void)
{
    return MS51_HAL_VERSION;
}

/*---------------------------------------------------------------------------
 * Interrupt Management
 *---------------------------------------------------------------------------*/
void SVC_SYSTEM_EnableInterrupts(void)
{
    EA = 1;
}

void SVC_SYSTEM_DisableInterrupts(void)
{
    EA = 0;
}

uint8_t SVC_SYSTEM_EnterCritical(void)
{
    uint8_t state = EA;
    EA = 0;
    return state;
}

void SVC_SYSTEM_ExitCritical(uint8_t state)
{
    EA = state;
}
