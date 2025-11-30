/**
 *******************************************************************************
 * @file    ms51_hal_system.c
 * @brief   MS51 HAL System Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_SYSTEM_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/* Store current system clock frequency */
static uint32_t g_sysClkFreq = 16000000UL;  /* Default to 16MHz HIRC */

/*******************************************************************************
 * Private Constants - HIRC Trim values
 ******************************************************************************/
#define HIRC_16MHZ  0
#define HIRC_24MHZ  1

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Configure system clock
 */
HAL_StatusTypeDef HAL_System_ConfigClock(HAL_System_ClkInitTypeDef *config)
{
    if (config == NULL) {
        return HAL_INVALID;
    }
    
    /* Configure HIRC frequency if using HIRC */
    if (config->ClockSource == HAL_SYSCLK_HIRC) {
        HAL_System_SetHIRC(config->HIRCFreq);
    }
    
    /* Set clock source */
    switch (config->ClockSource) {
        case HAL_SYSCLK_HIRC:
            /* HIRC is default, ensure it's enabled */
            HAL_TA_SET_BIT(CKEN, 0x20);  /* HIRCEN = 1 */
            /* Wait for HIRC stable */
            while (!(CKSWT & 0x20));
            /* Switch to HIRC */
            HAL_TA_WRITE(CKSWT, (CKSWT & 0xF8) | 0x00);
            break;
            
        case HAL_SYSCLK_LIRC:
            /* LIRC is always on */
            while (!(CKSWT & 0x10));  /* Wait for LIRC stable */
            /* Switch to LIRC */
            HAL_TA_WRITE(CKSWT, (CKSWT & 0xF8) | 0x03);
            g_sysClkFreq = 10000UL;  /* ~10kHz */
            break;
            
        case HAL_SYSCLK_ECLK:
            /* Enable external clock */
            HAL_TA_SET_BIT(CKEN, 0xC0);  /* EXTEN1:0 = 11 */
            /* Wait for ECLK stable */
            while (!(CKSWT & 0x08));
            /* Switch to ECLK */
            HAL_TA_WRITE(CKSWT, (CKSWT & 0xF8) | 0x01);
            break;
            
        default:
            return HAL_INVALID;
    }
    
    /* Set clock divider */
    HAL_System_SetClockDiv(config->ClockDiv);
    
    return HAL_OK;
}

/**
 * @brief  Set HIRC frequency
 */
HAL_StatusTypeDef HAL_System_SetHIRC(HAL_HIRCFreqTypeDef freq)
{
    uint8_t hircTrim;
    
    /* Read HIRC trim value from ROM */
    HAL_TA_SET_BIT(CHPCON, 0x01);  /* IAPEN = 1 */
    
    IAPCN = 0x04;  /* Read UID */
    
    if (freq == HAL_HIRC_24MHZ) {
        /* Read 24MHz trim value from 0x0038 */
        IAPAH = 0x00;
        IAPAL = 0x38;
        HAL_TA_SET_BIT(IAPTRG, 0x01);
        RCTRIM0 = IAPFD;
        
        IAPAL = 0x39;
        HAL_TA_SET_BIT(IAPTRG, 0x01);
        RCTRIM1 = IAPFD;
        
        g_sysClkFreq = 24000000UL;
    } else {
        /* Read 16MHz trim value from 0x0030 */
        IAPAH = 0x00;
        IAPAL = 0x30;
        HAL_TA_SET_BIT(IAPTRG, 0x01);
        RCTRIM0 = IAPFD;
        
        IAPAL = 0x31;
        HAL_TA_SET_BIT(IAPTRG, 0x01);
        RCTRIM1 = IAPFD;
        
        g_sysClkFreq = 16000000UL;
    }
    
    HAL_TA_CLEAR_BIT(CHPCON, 0x01);  /* IAPEN = 0 */
    
    return HAL_OK;
}

/**
 * @brief  Set system clock divider
 */
HAL_StatusTypeDef HAL_System_SetClockDiv(HAL_ClkDivTypeDef div)
{
    CKDIV = (uint8_t)div;
    
    /* Update system clock frequency: Fsys = Fclk / (1 << div) */
    g_sysClkFreq = g_sysClkFreq / (1UL << div);
    
    return HAL_OK;
}

/**
 * @brief  Get current system clock frequency
 */
uint32_t HAL_System_GetSysClkFreq(void)
{
    return g_sysClkFreq;
}

/**
 * @brief  Enable clock output on P1.7
 */
HAL_StatusTypeDef HAL_System_ClockOutput(uint8_t enable, uint8_t divider)
{
    if (enable) {
        /* Configure P1.7 as push-pull output */
        P1M1 &= ~0x80;
        P1M2 |= 0x80;
        
        /* Enable clock output */
        CKCON |= 0x02;  /* CLOEN = 1 */
        
        /* Set divider (if supported) */
        /* Note: Clock output is Fsys/2 on MS51 */
    } else {
        CKCON &= ~0x02;  /* CLOEN = 0 */
    }
    
    return HAL_OK;
}

/**
 * @brief  Enter idle mode
 */
void HAL_System_EnterIdle(void)
{
    PCON |= 0x01;  /* Set IDLE bit */
    HAL_NOP();
    HAL_NOP();
}

/**
 * @brief  Enter power-down mode
 */
void HAL_System_EnterPowerDown(void)
{
    PCON |= 0x02;  /* Set PD bit */
    HAL_NOP();
    HAL_NOP();
}

/**
 * @brief  Configure wake-up sources
 */
HAL_StatusTypeDef HAL_System_ConfigWakeup(uint8_t wktEnable, uint8_t bodEnable, 
                                           uint8_t pinEnable)
{
    /* Configure wake-up timer if enabled */
    if (wktEnable) {
        EIE1 |= 0x04;  /* EWKT = 1 */
    } else {
        EIE1 &= ~0x04;
    }
    
    /* Configure BOD wake-up if enabled */
    if (bodEnable) {
        EBOD = 1;
    } else {
        EBOD = 0;
    }
    
    /* Configure pin interrupt wake-up if enabled */
    if (pinEnable) {
        EIE |= 0x02;  /* EPI = 1 */
    } else {
        EIE &= ~0x02;
    }
    
    return HAL_OK;
}

/**
 * @brief  Perform software system reset
 */
void HAL_System_Reset(HAL_BootAreaTypeDef bootArea)
{
    uint8_t bootConfig;
    
    bootConfig = 0x80 | (uint8_t)bootArea;
    
    HAL_TA_WRITE(CHPCON, bootConfig);
    
    /* Should not reach here */
    while (1);
}

/**
 * @brief  Get reset source flags
 */
uint8_t HAL_System_GetResetSource(void)
{
    uint8_t source = 0;
    
    /* Check AUXR1 for reset flags */
    if (AUXR1 & 0x80) source |= HAL_RESET_SOFTWARE;
    if (AUXR1 & 0x40) source |= HAL_RESET_EXTERNAL;
    if (AUXR1 & 0x20) source |= HAL_RESET_POR;
    
    /* Check WDCON for WDT reset */
    if (WDCON & 0x08) source |= HAL_RESET_WDT;
    
    /* Check BODCON0 for BOD reset */
    if (BODCON0 & 0x02) source |= HAL_RESET_BOD;
    
    return source;
}

/**
 * @brief  Clear reset source flags
 */
void HAL_System_ClearResetSource(void)
{
    AUXR1 &= ~0xE0;
    HAL_TA_CLEAR_BIT(WDCON, 0x08);
    HAL_TA_CLEAR_BIT(BODCON0, 0x02);
}

/**
 * @brief  Configure Brown-Out Detection
 */
HAL_StatusTypeDef HAL_System_ConfigBOD(uint8_t enable, HAL_BOD_LevelTypeDef level, 
                                        uint8_t reset)
{
    uint8_t bodcon0_val = 0;
    
    if (enable) {
        bodcon0_val = 0x80;  /* BODEN = 1 */
        bodcon0_val |= ((level & 0x03) << 4);  /* BOV1:BOV0 */
        
        if (reset) {
            bodcon0_val |= 0x04;  /* BORST = 1 */
        } else {
            bodcon0_val |= 0x01;  /* BOS = 1 (BOD sampling mode) */
        }
    }
    
    HAL_TA_WRITE(BODCON0, bodcon0_val);
    
    return HAL_OK;
}

/**
 * @brief  Get BOD detection flag
 */
uint8_t HAL_System_GetBODFlag(void)
{
    return (BODCON0 & 0x08) ? 1 : 0;  /* BOF bit */
}

/**
 * @brief  Clear BOD detection flag
 */
void HAL_System_ClearBODFlag(void)
{
    HAL_TA_CLEAR_BIT(BODCON0, 0x08);
}

/**
 * @brief  Read device unique ID
 */
HAL_StatusTypeDef HAL_System_ReadUID(uint8_t *uid)
{
    uint8_t i;
    
    if (uid == NULL) {
        return HAL_INVALID;
    }
    
    /* Enable IAP */
    HAL_TA_SET_BIT(CHPCON, 0x01);
    
    IAPCN = 0x04;  /* Read UID command */
    IAPAH = 0x00;
    
    for (i = 0; i < 9; i++) {
        IAPAL = i;
        HAL_TA_SET_BIT(IAPTRG, 0x01);
        uid[i] = IAPFD;
    }
    
    /* Disable IAP */
    HAL_TA_CLEAR_BIT(CHPCON, 0x01);
    
    return HAL_OK;
}

/**
 * @brief  Read device configuration bytes
 */
HAL_StatusTypeDef HAL_System_ReadConfig(uint8_t *config)
{
    uint8_t i;
    
    if (config == NULL) {
        return HAL_INVALID;
    }
    
    /* Enable IAP */
    HAL_TA_SET_BIT(CHPCON, 0x01);
    
    IAPCN = 0x03;  /* Read CONFIG command */
    IAPAH = 0x00;
    
    for (i = 0; i < 5; i++) {
        IAPAL = i;
        HAL_TA_SET_BIT(IAPTRG, 0x01);
        config[i] = IAPFD;
    }
    
    /* Disable IAP */
    HAL_TA_CLEAR_BIT(CHPCON, 0x01);
    
    return HAL_OK;
}

/**
 * @brief  Get device part number
 */
uint32_t HAL_System_GetDeviceID(void)
{
    uint8_t uid[9];
    uint32_t deviceId;
    
    HAL_System_ReadUID(uid);
    
    /* Device ID is typically at UID offset 0-3 */
    deviceId = ((uint32_t)uid[0] << 24) | ((uint32_t)uid[1] << 16) |
               ((uint32_t)uid[2] << 8) | uid[3];
    
    return deviceId;
}

/**
 * @brief  Enable global interrupts
 */
void HAL_System_EnableInterrupts(void)
{
    EA = 1;
}

/**
 * @brief  Disable global interrupts
 */
uint8_t HAL_System_DisableInterrupts(void)
{
    uint8_t state = EA;
    EA = 0;
    return state;
}

/**
 * @brief  Restore interrupt state
 */
void HAL_System_RestoreInterrupts(uint8_t state)
{
    EA = state;
}

/**
 * @brief  Software delay
 */
void HAL_System_Delay(uint16_t count)
{
    uint16_t i;
    for (i = 0; i < count; i++) {
        HAL_NOP();
    }
}

#endif /* HAL_SYSTEM_MODULE_ENABLED */
