/**
 *******************************************************************************
 * @file    ms51_hal_wdt.c
 * @brief   MS51 HAL Watchdog Timer Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_WDT_MODULE_ENABLED

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize Watchdog Timer
 */
HAL_StatusTypeDef HAL_WDT_Init(HAL_WDT_InitTypeDef *init)
{
    uint8_t wdcon = 0;
    
    if (init == NULL) {
        return HAL_INVALID;
    }
    
    /* Clear WDT first */
    HAL_WDT_Clear();
    
    /* Set timeout period */
    wdcon = (init->Timeout & 0x07);
    
    /* Configure power-down mode */
    if (init->PowerDownEnable) {
        wdcon |= 0x10;  /* WIDPD = 1 */
    }
    
    /* Write WDCON (TA protected) */
    HAL_TA_WRITE(WDCON, wdcon);
    
    /* Note: WDTR (WDT enable) and reset enable are typically
       configured through CONFIG bits, not runtime */
    
    return HAL_OK;
}

/**
 * @brief  Start Watchdog Timer
 */
void HAL_WDT_Start(void)
{
    HAL_TA_SET_BIT(WDCON, 0x80);  /* WDTR = 1 */
}

/**
 * @brief  Stop Watchdog Timer
 */
void HAL_WDT_Stop(void)
{
    HAL_TA_CLEAR_BIT(WDCON, 0x80);  /* WDTR = 0 */
}

/**
 * @brief  Refresh (feed) the Watchdog Timer
 */
void HAL_WDT_Refresh(void)
{
    HAL_TA_SET_BIT(WDCON, 0x40);  /* WDCLR = 1 */
}

/**
 * @brief  Clear Watchdog Timer counter
 */
void HAL_WDT_Clear(void)
{
    HAL_TA_SET_BIT(WDCON, 0x40);  /* WDCLR = 1 */
}

/**
 * @brief  Check if WDT reset occurred
 */
uint8_t HAL_WDT_GetResetFlag(void)
{
    return (WDCON & 0x08) ? 1 : 0;  /* WDTRF bit */
}

/**
 * @brief  Clear WDT reset flag
 */
void HAL_WDT_ClearResetFlag(void)
{
    HAL_TA_CLEAR_BIT(WDCON, 0x08);  /* Clear WDTRF */
}

/**
 * @brief  Get WDT timeout flag
 */
uint8_t HAL_WDT_GetTimeoutFlag(void)
{
    return (WDCON & 0x20) ? 1 : 0;  /* WDTF bit */
}

/**
 * @brief  Clear WDT timeout flag
 */
void HAL_WDT_ClearTimeoutFlag(void)
{
    HAL_TA_CLEAR_BIT(WDCON, 0x20);  /* Clear WDTF */
}

/**
 * @brief  Enable WDT interrupt
 */
void HAL_WDT_EnableInterrupt(void)
{
    EIE |= 0x10;  /* EWDT = 1 */
}

/**
 * @brief  Disable WDT interrupt
 */
void HAL_WDT_DisableInterrupt(void)
{
    EIE &= ~0x10;  /* EWDT = 0 */
}

/**
 * @brief  Simple WDT initialization
 */
HAL_StatusTypeDef HAL_WDT_SimpleInit(HAL_WDT_TimeoutTypeDef timeout)
{
    HAL_WDT_InitTypeDef init;
    
    init.Timeout = timeout;
    init.ResetEnable = HAL_ENABLE;
    init.IdleEnable = HAL_DISABLE;
    init.PowerDownEnable = HAL_DISABLE;
    
    return HAL_WDT_Init(&init);
}

/*******************************************************************************
 * Weak Callback Function
 ******************************************************************************/

__attribute__((weak)) void HAL_WDT_TimeoutCallback(void)
{
    /* User implementation */
}

#endif /* HAL_WDT_MODULE_ENABLED */
