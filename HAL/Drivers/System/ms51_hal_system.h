/**
 *******************************************************************************
 * @file    ms51_hal_system.h
 * @brief   MS51 HAL System Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details System HAL driver for MS51 series including:
 *          - Clock configuration
 *          - Power modes
 *          - System reset
 *          - Brown-out detection
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_SYSTEM_H
#define __MS51_HAL_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * Clock Source Definitions
 ******************************************************************************/

/**
 * @brief  System Clock Source enumeration
 */
typedef enum {
    HAL_SYSCLK_HIRC = 0,    /**< High-speed Internal RC (16/24MHz) */
    HAL_SYSCLK_LIRC = 1,    /**< Low-speed Internal RC (~10kHz) */
    HAL_SYSCLK_ECLK = 2     /**< External clock input */
} HAL_SysClkSrcTypeDef;

/**
 * @brief  HIRC Frequency Selection
 */
typedef enum {
    HAL_HIRC_16MHZ = 0,     /**< HIRC 16 MHz */
    HAL_HIRC_24MHZ = 1      /**< HIRC 24 MHz */
} HAL_HIRCFreqTypeDef;

/**
 * @brief  Clock Divider values
 */
typedef enum {
    HAL_CLKDIV_1   = 0,     /**< Fsys = Fclk / 1 */
    HAL_CLKDIV_2   = 1,     /**< Fsys = Fclk / 2 */
    HAL_CLKDIV_4   = 2,     /**< Fsys = Fclk / 4 */
    HAL_CLKDIV_8   = 3,     /**< Fsys = Fclk / 8 */
    HAL_CLKDIV_16  = 4,     /**< Fsys = Fclk / 16 */
    HAL_CLKDIV_32  = 5,     /**< Fsys = Fclk / 32 */
    HAL_CLKDIV_64  = 6,     /**< Fsys = Fclk / 64 */
    HAL_CLKDIV_128 = 7      /**< Fsys = Fclk / 128 */
} HAL_ClkDivTypeDef;

/*******************************************************************************
 * Power Mode Definitions
 ******************************************************************************/

/**
 * @brief  Power Mode enumeration
 */
typedef enum {
    HAL_POWERMODE_NORMAL = 0,   /**< Normal operation */
    HAL_POWERMODE_IDLE = 1,     /**< Idle mode (CPU stopped, peripherals running) */
    HAL_POWERMODE_POWERDOWN = 2 /**< Power-down mode (most clocks stopped) */
} HAL_PowerModeTypeDef;

/*******************************************************************************
 * Reset Source Definitions
 ******************************************************************************/

/**
 * @brief  Reset Source flags
 */
typedef enum {
    HAL_RESET_POR      = 0x01,  /**< Power-on reset */
    HAL_RESET_BOD      = 0x02,  /**< Brown-out reset */
    HAL_RESET_EXTERNAL = 0x04,  /**< External reset (NRST pin) */
    HAL_RESET_WDT      = 0x08,  /**< Watchdog timer reset */
    HAL_RESET_SOFTWARE = 0x10   /**< Software reset */
} HAL_ResetSourceTypeDef;

/*******************************************************************************
 * BOD (Brown-Out Detect) Definitions
 ******************************************************************************/

/**
 * @brief  BOD Voltage Level
 */
typedef enum {
    HAL_BOD_LEVEL_2V2 = 0,  /**< BOD at 2.2V */
    HAL_BOD_LEVEL_2V7 = 1,  /**< BOD at 2.7V */
    HAL_BOD_LEVEL_3V7 = 2,  /**< BOD at 3.7V */
    HAL_BOD_LEVEL_4V4 = 3   /**< BOD at 4.4V */
} HAL_BOD_LevelTypeDef;

/*******************************************************************************
 * Boot Area Definitions
 ******************************************************************************/

/**
 * @brief  Boot area selection
 */
typedef enum {
    HAL_BOOT_APROM = 0,     /**< Boot from APROM */
    HAL_BOOT_LDROM = 2      /**< Boot from LDROM */
} HAL_BootAreaTypeDef;

/*******************************************************************************
 * System Initialization Structure
 ******************************************************************************/

/**
 * @brief  System Clock Configuration Structure
 */
typedef struct {
    HAL_SysClkSrcTypeDef    ClockSource;    /**< System clock source */
    HAL_HIRCFreqTypeDef     HIRCFreq;       /**< HIRC frequency (if using HIRC) */
    HAL_ClkDivTypeDef       ClockDiv;       /**< Clock divider */
} HAL_System_ClkInitTypeDef;

/*******************************************************************************
 * System Function Prototypes
 ******************************************************************************/

/**
 * @brief  Configure system clock
 * @param  config Pointer to clock configuration structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_System_ClkInitTypeDef clkConfig;
 *   clkConfig.ClockSource = HAL_SYSCLK_HIRC;
 *   clkConfig.HIRCFreq = HAL_HIRC_24MHZ;
 *   clkConfig.ClockDiv = HAL_CLKDIV_1;
 *   HAL_System_ConfigClock(&clkConfig);
 */
HAL_StatusTypeDef HAL_System_ConfigClock(HAL_System_ClkInitTypeDef *config);

/**
 * @brief  Set HIRC frequency (16MHz or 24MHz)
 * @param  freq HIRC frequency selection
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_System_SetHIRC(HAL_HIRC_24MHZ);
 */
HAL_StatusTypeDef HAL_System_SetHIRC(HAL_HIRCFreqTypeDef freq);

/**
 * @brief  Set system clock divider
 * @param  div Clock divider value
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_System_SetClockDiv(HAL_ClkDivTypeDef div);

/**
 * @brief  Get current system clock frequency
 * @retval uint32_t System clock frequency in Hz
 */
uint32_t HAL_System_GetSysClkFreq(void);

/**
 * @brief  Enable clock output on P1.7
 * @param  enable HAL_ENABLE or HAL_DISABLE
 * @param  divider Clock output divider (0-7)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_System_ClockOutput(uint8_t enable, uint8_t divider);

/*******************************************************************************
 * Power Management Functions
 ******************************************************************************/

/**
 * @brief  Enter idle mode
 * @retval None
 * 
 * @note   CPU stops, peripherals continue running
 *         Wake-up by any enabled interrupt
 */
void HAL_System_EnterIdle(void);

/**
 * @brief  Enter power-down mode
 * @retval None
 * 
 * @note   Most clocks stopped, minimal power consumption
 *         Wake-up by WKT, BOD, or enabled interrupts
 */
void HAL_System_EnterPowerDown(void);

/**
 * @brief  Configure wake-up sources for power-down mode
 * @param  wktEnable Enable wake-up timer
 * @param  bodEnable Enable BOD wake-up
 * @param  pinEnable Enable pin interrupt wake-up
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_System_ConfigWakeup(uint8_t wktEnable, uint8_t bodEnable, 
                                           uint8_t pinEnable);

/*******************************************************************************
 * Reset Functions
 ******************************************************************************/

/**
 * @brief  Perform software system reset
 * @param  bootArea Area to boot from after reset
 * @retval None (does not return)
 * 
 * @example
 *   HAL_System_Reset(HAL_BOOT_APROM);  // Reset and boot from APROM
 */
void HAL_System_Reset(HAL_BootAreaTypeDef bootArea);

/**
 * @brief  Get reset source flags
 * @retval uint8_t Reset source flags
 * 
 * @note   Returns combination of HAL_RESET_xxx flags
 */
uint8_t HAL_System_GetResetSource(void);

/**
 * @brief  Clear reset source flags
 * @retval None
 */
void HAL_System_ClearResetSource(void);

/*******************************************************************************
 * BOD (Brown-Out Detect) Functions
 ******************************************************************************/

/**
 * @brief  Configure Brown-Out Detection
 * @param  enable Enable/disable BOD
 * @param  level BOD voltage level
 * @param  reset Enable BOD reset (HAL_ENABLE) or interrupt (HAL_DISABLE)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_System_ConfigBOD(HAL_ENABLE, HAL_BOD_LEVEL_2V7, HAL_ENABLE);
 */
HAL_StatusTypeDef HAL_System_ConfigBOD(uint8_t enable, HAL_BOD_LevelTypeDef level, 
                                        uint8_t reset);

/**
 * @brief  Get BOD detection flag
 * @retval uint8_t 1 if BOD detected, 0 otherwise
 */
uint8_t HAL_System_GetBODFlag(void);

/**
 * @brief  Clear BOD detection flag
 * @retval None
 */
void HAL_System_ClearBODFlag(void);

/*******************************************************************************
 * Chip Information Functions
 ******************************************************************************/

/**
 * @brief  Read device unique ID
 * @param  uid Array to store UID (9 bytes)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_System_ReadUID(uint8_t *uid);

/**
 * @brief  Read device configuration bytes
 * @param  config Array to store config (5 bytes)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_System_ReadConfig(uint8_t *config);

/**
 * @brief  Get device part number
 * @retval uint32_t Device part number
 */
uint32_t HAL_System_GetDeviceID(void);

/*******************************************************************************
 * Interrupt Control Functions
 ******************************************************************************/

/**
 * @brief  Enable global interrupts
 * @retval None
 */
void HAL_System_EnableInterrupts(void);

/**
 * @brief  Disable global interrupts
 * @retval uint8_t Previous interrupt state (EA value)
 */
uint8_t HAL_System_DisableInterrupts(void);

/**
 * @brief  Restore interrupt state
 * @param  state Previous interrupt state from DisableInterrupts
 * @retval None
 */
void HAL_System_RestoreInterrupts(uint8_t state);

/*******************************************************************************
 * Delay Functions
 ******************************************************************************/

/**
 * @brief  Software delay (approximate)
 * @param  count Delay count
 * @retval None
 */
void HAL_System_Delay(uint16_t count);

/*******************************************************************************
 * Convenience Macros
 ******************************************************************************/

/**
 * @brief  Enter critical section (disable interrupts)
 */
#define HAL_ENTER_CRITICAL()    uint8_t _int_state = HAL_System_DisableInterrupts()

/**
 * @brief  Exit critical section (restore interrupts)
 */
#define HAL_EXIT_CRITICAL()     HAL_System_RestoreInterrupts(_int_state)

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_SYSTEM_H */
