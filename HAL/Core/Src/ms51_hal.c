/**
 *******************************************************************************
 * @file    ms51_hal.c
 * @brief   MS51 HAL Core Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/* System tick counter (incremented in timer interrupt) */
static volatile uint32_t g_halTick = 0;

/* HAL initialization flag */
static uint8_t g_halInitialized = 0;

/*******************************************************************************
 * HAL Core Functions
 ******************************************************************************/

/**
 * @brief  Initialize the HAL Library
 */
HAL_StatusTypeDef HAL_Init(void)
{
    if (g_halInitialized) {
        return HAL_OK;
    }
    
    /* Default to 24MHz HIRC */
#ifdef HAL_SYSTEM_MODULE_ENABLED
    HAL_System_SetHIRC(HAL_HIRC_24MHZ);
#endif
    
    /* Initialize system tick */
    g_halTick = 0;
    
    g_halInitialized = 1;
    
    return HAL_OK;
}

/**
 * @brief  De-Initialize the HAL Library
 */
HAL_StatusTypeDef HAL_DeInit(void)
{
    g_halInitialized = 0;
    g_halTick = 0;
    
    return HAL_OK;
}

/**
 * @brief  Get HAL Version
 */
void HAL_GetVersion(HAL_VersionTypeDef *version)
{
    if (version != NULL) {
        version->major = HAL_VERSION_MAIN;
        version->minor = HAL_VERSION_SUB1;
        version->patch = HAL_VERSION_SUB2;
        version->rc = HAL_VERSION_RC;
    }
}

/**
 * @brief  Increment system tick (call from timer ISR)
 */
void HAL_IncTick(void)
{
    g_halTick++;
}

/**
 * @brief  Get system tick count
 */
uint32_t HAL_GetTick(void)
{
    return g_halTick;
}

/**
 * @brief  Provides a blocking delay in milliseconds
 */
void HAL_Delay(uint16_t ms)
{
#ifdef HAL_TIMER_MODULE_ENABLED
    HAL_Timer_DelayMs(HAL_SYSCLK_FREQ, ms);
#else
    /* Fallback to software delay */
    uint16_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < (HAL_SYSCLK_FREQ / 10000UL); j++) {
            HAL_NOP();
        }
    }
#endif
}

/**
 * @brief  Provides a blocking delay in microseconds
 */
void HAL_DelayUs(uint16_t us)
{
#ifdef HAL_TIMER_MODULE_ENABLED
    HAL_Timer_DelayUs(HAL_SYSCLK_FREQ, us);
#else
    /* Fallback to software delay */
    uint16_t i;
    uint16_t count = (uint16_t)(HAL_SYSCLK_FREQ / 10000000UL) * us;
    for (i = 0; i < count; i++) {
        HAL_NOP();
    }
#endif
}

/**
 * @brief  Enable global interrupts
 */
void HAL_EnableInterrupts(void)
{
    EA = 1;
}

/**
 * @brief  Disable global interrupts
 */
void HAL_DisableInterrupts(void)
{
    EA = 0;
}

/**
 * @brief  Software system reset
 */
void HAL_SystemReset(void)
{
#ifdef HAL_SYSTEM_MODULE_ENABLED
    HAL_System_Reset(HAL_BOOT_APROM);
#else
    /* Direct register write */
    HAL_TA_WRITE(CHPCON, 0x80);
    while (1);
#endif
}

/**
 * @brief  Enter power-down mode
 */
void HAL_EnterPowerDown(void)
{
#ifdef HAL_SYSTEM_MODULE_ENABLED
    HAL_System_EnterPowerDown();
#else
    PCON |= 0x02;
    HAL_NOP();
    HAL_NOP();
#endif
}

/**
 * @brief  Enter idle mode
 */
void HAL_EnterIdle(void)
{
#ifdef HAL_SYSTEM_MODULE_ENABLED
    HAL_System_EnterIdle();
#else
    PCON |= 0x01;
    HAL_NOP();
    HAL_NOP();
#endif
}

/*******************************************************************************
 * Debug Functions
 ******************************************************************************/

#ifdef HAL_DEBUG_ENABLED
/**
 * @brief  Assert failed handler
 */
void HAL_AssertFailed(const char *file, uint32_t line)
{
    /* User can override this function */
    /* Default: disable interrupts and loop forever */
    HAL_DisableInterrupts();
    while (1) {
        /* Toggle LED if available */
#ifdef HAL_GPIO_MODULE_ENABLED
        HAL_LED_TOGGLE();
        HAL_Delay(100);
#endif
    }
}
#endif

/*******************************************************************************
 * Weak callback functions (can be overridden by user)
 ******************************************************************************/

/**
 * @brief  System tick handler callback
 */
__attribute__((weak)) void HAL_SysTickCallback(void)
{
    /* User implementation */
}

/**
 * @brief  Error handler callback
 */
__attribute__((weak)) void HAL_ErrorCallback(uint8_t errorCode)
{
    /* User implementation */
    HAL_UNUSED(errorCode);
}
