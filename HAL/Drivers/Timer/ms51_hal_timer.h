/**
 *******************************************************************************
 * @file    ms51_hal_timer.h
 * @brief   MS51 HAL Timer Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details Timer Hardware Abstraction Layer for MS51 series.
 *          Supports Timer0, Timer1, Timer2, and Timer3.
 *
 * Features:
 *   - Timer/Counter modes
 *   - Delay generation
 *   - Auto-reload mode
 *   - Interrupt support
 *   - Capture mode (Timer2)
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_TIMER_H
#define __MS51_HAL_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * Timer Instance Definitions
 ******************************************************************************/

/**
 * @brief  Timer Instance enumeration
 */
typedef enum {
    HAL_TIMER0 = 0,     /**< Timer 0 */
    HAL_TIMER1 = 1,     /**< Timer 1 */
    HAL_TIMER2 = 2,     /**< Timer 2 */
    HAL_TIMER3 = 3      /**< Timer 3 */
} HAL_Timer_InstanceTypeDef;

/*******************************************************************************
 * Timer Mode Definitions
 ******************************************************************************/

/**
 * @brief  Timer0/Timer1 Mode enumeration
 */
typedef enum {
    HAL_TIMER_MODE0 = 0x00,     /**< 13-bit timer/counter */
    HAL_TIMER_MODE1 = 0x01,     /**< 16-bit timer/counter */
    HAL_TIMER_MODE2 = 0x02,     /**< 8-bit auto-reload */
    HAL_TIMER_MODE3 = 0x03      /**< Timer0: two 8-bit timers, Timer1: stopped */
} HAL_Timer_ModeTypeDef;

/**
 * @brief  Timer2 Mode enumeration
 */
typedef enum {
    HAL_TIMER2_TIMER = 0,       /**< Timer mode */
    HAL_TIMER2_COUNTER = 1,     /**< Counter mode */
    HAL_TIMER2_CAPTURE = 2,     /**< Capture mode */
    HAL_TIMER2_AUTORELOAD = 3   /**< Auto-reload mode */
} HAL_Timer2_ModeTypeDef;

/*******************************************************************************
 * Timer Clock Source Definitions
 ******************************************************************************/

/**
 * @brief  Timer Clock Source
 */
typedef enum {
    HAL_TIMER_CLK_FSYS = 0,     /**< System clock (Fsys) */
    HAL_TIMER_CLK_FSYS_DIV12 = 1 /**< System clock / 12 */
} HAL_Timer_ClkSrcTypeDef;

/**
 * @brief  Timer2 Clock Divider
 */
typedef enum {
    HAL_TIMER2_DIV_1 = 0,       /**< Fsys / 1 */
    HAL_TIMER2_DIV_4 = 1,       /**< Fsys / 4 */
    HAL_TIMER2_DIV_16 = 2,      /**< Fsys / 16 */
    HAL_TIMER2_DIV_32 = 3,      /**< Fsys / 32 */
    HAL_TIMER2_DIV_64 = 4,      /**< Fsys / 64 */
    HAL_TIMER2_DIV_128 = 5,     /**< Fsys / 128 */
    HAL_TIMER2_DIV_256 = 6,     /**< Fsys / 256 */
    HAL_TIMER2_DIV_512 = 7      /**< Fsys / 512 */
} HAL_Timer2_DivTypeDef;

/**
 * @brief  Timer3 Clock Divider
 */
typedef enum {
    HAL_TIMER3_DIV_1 = 0,       /**< Fsys / 1 */
    HAL_TIMER3_DIV_2 = 1,       /**< Fsys / 2 */
    HAL_TIMER3_DIV_4 = 2,       /**< Fsys / 4 */
    HAL_TIMER3_DIV_8 = 3,       /**< Fsys / 8 */
    HAL_TIMER3_DIV_16 = 4,      /**< Fsys / 16 */
    HAL_TIMER3_DIV_32 = 5,      /**< Fsys / 32 */
    HAL_TIMER3_DIV_64 = 6,      /**< Fsys / 64 */
    HAL_TIMER3_DIV_128 = 7      /**< Fsys / 128 */
} HAL_Timer3_DivTypeDef;

/*******************************************************************************
 * Timer Initialization Structures
 ******************************************************************************/

/**
 * @brief  Timer0/Timer1 Initialization Structure
 */
typedef struct {
    HAL_Timer_ModeTypeDef       Mode;           /**< Timer mode */
    HAL_Timer_ClkSrcTypeDef     ClockSource;    /**< Clock source */
    uint16_t                    Period;         /**< Timer period/reload value */
    uint8_t                     EnableGate;     /**< Gate control enable */
    uint8_t                     CounterMode;    /**< Counter mode (external input) */
} HAL_Timer01_InitTypeDef;

/**
 * @brief  Timer2 Initialization Structure
 */
typedef struct {
    HAL_Timer2_ModeTypeDef      Mode;           /**< Timer2 mode */
    HAL_Timer2_DivTypeDef       Divider;        /**< Clock divider */
    uint16_t                    Period;         /**< Timer period/reload value */
} HAL_Timer2_InitTypeDef;

/**
 * @brief  Timer3 Initialization Structure
 */
typedef struct {
    HAL_Timer3_DivTypeDef       Divider;        /**< Clock divider */
    uint16_t                    Period;         /**< Timer period/reload value */
} HAL_Timer3_InitTypeDef;

/*******************************************************************************
 * Timer Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize Timer0
 * @param  init Pointer to Timer0 initialization structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer0_Init(HAL_Timer01_InitTypeDef *init);

/**
 * @brief  Initialize Timer1
 * @param  init Pointer to Timer1 initialization structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer1_Init(HAL_Timer01_InitTypeDef *init);

/**
 * @brief  Initialize Timer2
 * @param  init Pointer to Timer2 initialization structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer2_Init(HAL_Timer2_InitTypeDef *init);

/**
 * @brief  Initialize Timer3
 * @param  init Pointer to Timer3 initialization structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer3_Init(HAL_Timer3_InitTypeDef *init);

/**
 * @brief  Start Timer
 * @param  timer Timer instance
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer_Start(HAL_Timer_InstanceTypeDef timer);

/**
 * @brief  Stop Timer
 * @param  timer Timer instance
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer_Stop(HAL_Timer_InstanceTypeDef timer);

/**
 * @brief  Enable Timer interrupt
 * @param  timer Timer instance
 * @retval None
 */
void HAL_Timer_EnableInterrupt(HAL_Timer_InstanceTypeDef timer);

/**
 * @brief  Disable Timer interrupt
 * @param  timer Timer instance
 * @retval None
 */
void HAL_Timer_DisableInterrupt(HAL_Timer_InstanceTypeDef timer);

/**
 * @brief  Get Timer interrupt flag
 * @param  timer Timer instance
 * @retval uint8_t 1 if flag set, 0 otherwise
 */
uint8_t HAL_Timer_GetFlag(HAL_Timer_InstanceTypeDef timer);

/**
 * @brief  Clear Timer interrupt flag
 * @param  timer Timer instance
 * @retval None
 */
void HAL_Timer_ClearFlag(HAL_Timer_InstanceTypeDef timer);

/**
 * @brief  Set Timer period/reload value
 * @param  timer Timer instance
 * @param  period Period value
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer_SetPeriod(HAL_Timer_InstanceTypeDef timer, uint16_t period);

/**
 * @brief  Get Timer current count value
 * @param  timer Timer instance
 * @retval uint16_t Current timer count
 */
uint16_t HAL_Timer_GetCount(HAL_Timer_InstanceTypeDef timer);

/*******************************************************************************
 * Delay Functions
 ******************************************************************************/

/**
 * @brief  Blocking delay using Timer0
 * @param  sysclk System clock frequency in Hz
 * @param  ms Delay in milliseconds
 * @retval None
 * 
 * @note   This function uses Timer0 in Mode 1 (16-bit) with Fsys/12 clock
 *         Maximum delay depends on system clock:
 *         - At 24MHz: max ~32ms per loop
 *         - At 8MHz: max ~98ms per loop
 * 
 * @example
 *   HAL_Timer_DelayMs(24000000UL, 100);  // 100ms delay at 24MHz
 */
void HAL_Timer_DelayMs(uint32_t sysclk, uint16_t ms);

/**
 * @brief  Blocking delay using Timer0
 * @param  sysclk System clock frequency in Hz
 * @param  us Delay in microseconds
 * @retval None
 * 
 * @example
 *   HAL_Timer_DelayUs(24000000UL, 500);  // 500us delay at 24MHz
 */
void HAL_Timer_DelayUs(uint32_t sysclk, uint16_t us);

/**
 * @brief  Generic delay function using loop
 * @param  count Delay count
 * @retval None
 * 
 * @note   This is a software delay loop, timing is approximate
 */
void HAL_Timer_SoftDelay(uint16_t count);

/**
 * @brief  Initialize Timer for interrupt-based delay
 * @param  timer Timer instance (recommended: Timer0 or Timer2)
 * @param  sysclk System clock frequency
 * @param  intervalUs Interrupt interval in microseconds
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   // Set up 1ms tick using Timer0
 *   HAL_Timer_InitInterruptDelay(HAL_TIMER0, 24000000UL, 1000);
 */
HAL_StatusTypeDef HAL_Timer_InitInterruptDelay(HAL_Timer_InstanceTypeDef timer,
                                                uint32_t sysclk, uint32_t intervalUs);

/*******************************************************************************
 * Timer2 Capture Functions
 ******************************************************************************/

/**
 * @brief  Initialize Timer2 capture mode
 * @param  captureChannel Capture channel (0, 1, or 2)
 * @param  edgeMode Edge detection mode
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Timer2_CaptureInit(uint8_t captureChannel, uint8_t edgeMode);

/**
 * @brief  Get Timer2 capture value
 * @param  captureChannel Capture channel
 * @retval uint16_t Captured value
 */
uint16_t HAL_Timer2_GetCaptureValue(uint8_t captureChannel);

/*******************************************************************************
 * Convenience Macros
 ******************************************************************************/

/**
 * @brief  Calculate timer reload value for given period
 * @param  fsys System clock frequency
 * @param  div Clock divider
 * @param  period_us Period in microseconds
 */
#define HAL_TIMER_CALC_RELOAD(fsys, div, period_us) \
    (65536 - ((fsys) / (div) * (period_us) / 1000000UL))

/**
 * @brief  Quick delay macros
 */
#define HAL_DELAY_MS(ms)    HAL_Timer_DelayMs(HAL_SYSCLK_FREQ, ms)
#define HAL_DELAY_US(us)    HAL_Timer_DelayUs(HAL_SYSCLK_FREQ, us)

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_TIMER_H */
