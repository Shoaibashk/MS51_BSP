/**
 *******************************************************************************
 * @file    ms51_hal_wkt.c
 * @brief   MS51 HAL Wake-up Timer (WKT) Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal_wkt.h"

#ifdef HAL_WKT_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static HAL_WKT_InitTypeDef g_wktConfig;

/*******************************************************************************
 * Private Macros
 ******************************************************************************/

/* WKT control bits */
#define WKT_RUN_BIT     0x08    /* WKTR bit in WKCON */
#define WKT_FLAG_BIT    0x10    /* WKTF bit in WKCON */

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize WKT with specified configuration
 */
HAL_StatusTypeDef HAL_WKT_Init(HAL_WKT_InitTypeDef *init)
{
    if (init == NULL) {
        return HAL_ERROR;
    }
    
    /* Store configuration */
    g_wktConfig = *init;
    
    /* Select SFR page 0 */
    SFRS = 0;
    
    /* Clear WKT control and set divider */
    WKCON = (WKCON & 0xF8) | (init->Divider & 0x07);
    
    /* Set reload value */
    RWK = init->ReloadValue;
    
    /* Enable interrupt if requested */
    if (init->EnableInterrupt) {
        HAL_WKT_EnableInterrupt();
    }
    
    return HAL_OK;
}

/**
 * @brief  De-Initialize WKT (disable WKT)
 */
HAL_StatusTypeDef HAL_WKT_DeInit(void)
{
    /* Select SFR page 0 */
    SFRS = 0;
    
    /* Stop WKT */
    WKCON &= ~WKT_RUN_BIT;
    
    /* Clear reload value */
    RWK = 0;
    
    /* Disable WKT interrupt */
    HAL_WKT_DisableInterrupt();
    
    return HAL_OK;
}

/**
 * @brief  Start WKT
 */
void HAL_WKT_Start(void)
{
    SFRS = 0;
    WKCON |= WKT_RUN_BIT;  /* Set WKTR bit */
}

/**
 * @brief  Stop WKT
 */
void HAL_WKT_Stop(void)
{
    SFRS = 0;
    WKCON &= ~WKT_RUN_BIT;  /* Clear WKTR bit */
}

/**
 * @brief  Set WKT reload value
 */
void HAL_WKT_SetReload(uint8_t value)
{
    SFRS = 0;
    RWK = value;
}

/**
 * @brief  Set WKT clock divider
 */
HAL_StatusTypeDef HAL_WKT_SetDivider(HAL_WKT_DivTypeDef divider)
{
    SFRS = 0;
    WKCON = (WKCON & 0xF8) | (divider & 0x07);
    return HAL_OK;
}

/**
 * @brief  Enable WKT interrupt
 */
void HAL_WKT_EnableInterrupt(void)
{
    /* Enable WKT interrupt (EIE.EWKT bit) */
    SFRS = 0;
    EIE |= 0x02;  /* Set EWKT bit */
}

/**
 * @brief  Disable WKT interrupt
 */
void HAL_WKT_DisableInterrupt(void)
{
    /* Disable WKT interrupt */
    SFRS = 0;
    EIE &= ~0x02;  /* Clear EWKT bit */
}

/**
 * @brief  Get WKT interrupt flag
 */
uint8_t HAL_WKT_GetFlag(void)
{
    SFRS = 0;
    return (WKCON & WKT_FLAG_BIT) ? 1 : 0;
}

/**
 * @brief  Clear WKT interrupt flag
 */
void HAL_WKT_ClearFlag(void)
{
    SFRS = 0;
    WKCON &= ~WKT_FLAG_BIT;  /* Clear WKTF bit */
}

/**
 * @brief  Configure WKT for millisecond-level wake-up
 */
HAL_StatusTypeDef HAL_WKT_ConfigMs(uint16_t ms)
{
    uint8_t ticks;
    
    if (ms < 10 || ms > 1000) {
        return HAL_ERROR;
    }
    
    SFRS = 0;
    
    /* Use divider 64 for ~6.4ms per tick */
    WKCON = (WKCON & 0xF8) | HAL_WKT_DIV_64;
    
    /* Calculate reload value */
    /* With divider 64, each tick is ~6.4ms at 10kHz LIRC */
    ticks = (uint8_t)(ms / 6);
    if (ticks > 255) {
        ticks = 255;
    }
    
    RWK = 256 - ticks;
    
    /* Enable WKT interrupt */
    HAL_WKT_EnableInterrupt();
    
    /* Start WKT */
    WKCON |= WKT_RUN_BIT;
    
    return HAL_OK;
}

/**
 * @brief  Configure WKT for second-level wake-up
 */
HAL_StatusTypeDef HAL_WKT_ConfigSeconds(uint8_t seconds)
{
    uint8_t ticks;
    
    if (seconds < 1 || seconds > 50) {
        return HAL_ERROR;
    }
    
    SFRS = 0;
    
    /* Use divider 2048 for ~204.8ms per tick */
    WKCON = (WKCON & 0xF8) | HAL_WKT_DIV_2048;
    
    /* Calculate reload value */
    /* With divider 2048, each tick is ~204.8ms at 10kHz LIRC */
    ticks = (uint8_t)(seconds * 5);  /* 5 ticks per second */
    if (ticks > 255) {
        ticks = 255;
    }
    
    RWK = 256 - ticks;
    
    /* Enable WKT interrupt */
    HAL_WKT_EnableInterrupt();
    
    /* Start WKT */
    WKCON |= WKT_RUN_BIT;
    
    return HAL_OK;
}

#endif /* HAL_WKT_MODULE_ENABLED */
