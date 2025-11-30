/**
 *******************************************************************************
 * @file    ms51_hal_bod.c
 * @brief   MS51 HAL Brown-out Detector (BOD) Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal_bod.h"

#ifdef HAL_BOD_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static HAL_BOD_InitTypeDef g_bodConfig;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize BOD with specified configuration
 */
HAL_StatusTypeDef HAL_BOD_Init(HAL_BOD_InitTypeDef *init)
{
    uint8_t bodcon_val;
    
    if (init == NULL) {
        return HAL_ERROR;
    }
    
    /* Store configuration */
    g_bodConfig = *init;
    
    /* Select SFR page 0 */
    SFRS = 0;
    
    /* Build BODCON0 value */
    bodcon_val = init->Level | init->ResetMode;
    
    /* Set enable bit if enabled */
    if (init->State == HAL_BOD_STATE_ENABLE) {
        bodcon_val |= 0x80;  /* Set BOD enable bit */
    }
    
    /* Write to protected BODCON0 register */
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = bodcon_val;
    
    /* Configure low power mode if specified */
    if (init->LowPowerMode != HAL_BOD_LP_MODE_NORMAL) {
        TA = 0xAA;
        TA = 0x55;
        BODCON1 = init->LowPowerMode;
    }
    
    return HAL_OK;
}

/**
 * @brief  De-Initialize BOD (disable BOD)
 */
HAL_StatusTypeDef HAL_BOD_DeInit(void)
{
    /* Select SFR page 0 */
    SFRS = 0;
    
    /* Disable BOD by writing 0 to BODCON0 */
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = 0x00;
    
    /* Clear low power mode */
    TA = 0xAA;
    TA = 0x55;
    BODCON1 = 0x00;
    
    /* Disable BOD interrupt */
    IE &= ~0x20;  /* Clear bit 5 (EBOD) */
    
    return HAL_OK;
}

/**
 * @brief  Enable BOD
 */
void HAL_BOD_Enable(void)
{
    uint8_t temp;
    
    SFRS = 0;
    temp = BODCON0 | 0x80;  /* Set BOD enable bit */
    
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = temp;
}

/**
 * @brief  Disable BOD
 */
void HAL_BOD_Disable(void)
{
    uint8_t temp;
    
    SFRS = 0;
    temp = BODCON0 & ~0x80;  /* Clear BOD enable bit */
    
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = temp;
}

/**
 * @brief  Set BOD voltage threshold level
 */
HAL_StatusTypeDef HAL_BOD_SetLevel(HAL_BOD_LevelTypeDef level)
{
    uint8_t temp;
    
    SFRS = 0;
    temp = (BODCON0 & 0x8F) | level;  /* Clear level bits and set new level */
    
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = temp;
    
    return HAL_OK;
}

/**
 * @brief  Get current BOD voltage level setting
 */
HAL_BOD_LevelTypeDef HAL_BOD_GetLevel(void)
{
    SFRS = 0;
    return (HAL_BOD_LevelTypeDef)(BODCON0 & 0x70);
}

/**
 * @brief  Enable BOD reset function
 */
void HAL_BOD_EnableReset(void)
{
    uint8_t temp;
    
    SFRS = 0;
    temp = BODCON0 | 0x04;  /* Set BOD reset enable bit */
    
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = temp;
}

/**
 * @brief  Disable BOD reset function
 */
void HAL_BOD_DisableReset(void)
{
    uint8_t temp;
    
    SFRS = 0;
    temp = BODCON0 & ~0x04;  /* Clear BOD reset enable bit */
    
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = temp;
}

/**
 * @brief  Configure Low Power BOD mode
 */
HAL_StatusTypeDef HAL_BOD_SetLowPowerMode(HAL_BOD_LowPowerModeTypeDef mode)
{
    SFRS = 0;
    
    TA = 0xAA;
    TA = 0x55;
    BODCON1 = mode;
    
    return HAL_OK;
}

/**
 * @brief  Enable BOD interrupt
 */
void HAL_BOD_EnableInterrupt(void)
{
    IE |= 0x20;  /* Set bit 5 (EBOD) */
}

/**
 * @brief  Disable BOD interrupt
 */
void HAL_BOD_DisableInterrupt(void)
{
    IE &= ~0x20;  /* Clear bit 5 (EBOD) */
}

/**
 * @brief  Get BOD status flag
 */
uint8_t HAL_BOD_GetStatus(void)
{
    SFRS = 0;
    return (BODCON0 & 0x08) ? 1 : 0;  /* Check BOF bit */
}

/**
 * @brief  Clear BOD flag
 */
void HAL_BOD_ClearFlag(void)
{
    uint8_t temp;
    
    SFRS = 0;
    temp = BODCON0 & ~0x08;  /* Clear BOF bit */
    
    TA = 0xAA;
    TA = 0x55;
    BODCON0 = temp;
}

/**
 * @brief  Check if last reset was caused by BOD
 */
uint8_t HAL_BOD_WasResetSource(void)
{
    /* Check PCON.BOF bit for BOD reset flag */
    return (PCON & 0x10) ? 1 : 0;
}

#endif /* HAL_BOD_MODULE_ENABLED */
