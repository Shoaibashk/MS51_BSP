/*===========================================================================
 * @file    svc_power.h
 * @brief   Power Management Service Interface
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Provides power management functions including idle mode,
 *          power-down mode, and wake-up configuration.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef SVC_POWER_H
#define SVC_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Power Modes
 *---------------------------------------------------------------------------*/
typedef enum {
    POWER_MODE_NORMAL    = 0,   /**< Normal run mode */
    POWER_MODE_IDLE      = 1,   /**< Idle mode (CPU stops, peripherals run) */
    POWER_MODE_POWERDOWN = 2    /**< Power-down mode (minimal power) */
} power_mode_t;

/*---------------------------------------------------------------------------
 * Wake-up Sources
 *---------------------------------------------------------------------------*/
typedef enum {
    WAKEUP_SRC_NONE      = 0x00,    /**< No wake-up source */
    WAKEUP_SRC_INT0      = 0x01,    /**< External INT0 */
    WAKEUP_SRC_INT1      = 0x02,    /**< External INT1 */
    WAKEUP_SRC_TIMER     = 0x04,    /**< Wake-up timer */
    WAKEUP_SRC_I2C       = 0x08,    /**< I2C address match */
    WAKEUP_SRC_UART      = 0x10,    /**< UART start bit */
    WAKEUP_SRC_BOD       = 0x20,    /**< Brown-out detector */
    WAKEUP_SRC_PIN       = 0x40,    /**< Pin interrupt */
    WAKEUP_SRC_WDT       = 0x80,    /**< Watchdog timer */
    WAKEUP_SRC_ALL       = 0xFF     /**< All sources */
} wakeup_source_t;

/* Compatibility aliases */
#define PWR_WAKEUP_NONE     WAKEUP_SRC_NONE
#define PWR_WAKEUP_INT0     WAKEUP_SRC_INT0
#define PWR_WAKEUP_INT1     WAKEUP_SRC_INT1
#define PWR_WAKEUP_TIMER    WAKEUP_SRC_TIMER
#define PWR_WAKEUP_I2C      WAKEUP_SRC_I2C
#define PWR_WAKEUP_UART     WAKEUP_SRC_UART
#define PWR_WAKEUP_BOD      WAKEUP_SRC_BOD
#define PWR_WAKEUP_PIN      WAKEUP_SRC_PIN
#define PWR_WAKEUP_WDT      WAKEUP_SRC_WDT
#define PWR_WAKEUP_ALL      WAKEUP_SRC_ALL

/*---------------------------------------------------------------------------
 * Wake-up Timer Configuration
 *---------------------------------------------------------------------------*/
typedef enum {
    WKT_CLK_LIRC = 0,       /**< Use LIRC (10 kHz) */
    WKT_CLK_LXT  = 1        /**< Use external 32.768 kHz crystal */
} wkt_clock_t;

typedef struct {
    wkt_clock_t clock;      /**< Clock source */
    uint16_t    period_ms;  /**< Wake-up period in milliseconds */
} wkt_config_t;

/*---------------------------------------------------------------------------
 * Power Configuration
 *---------------------------------------------------------------------------*/
typedef struct {
    uint8_t         wakeup_sources; /**< Enabled wake-up sources (OR of wakeup_source_t) */
    wkt_config_t    wkt_config;     /**< Wake-up timer configuration */
    uint8_t         gpio_retain;    /**< Retain GPIO states in power-down */
    uint8_t         bod_enable;     /**< Enable brown-out detector */
    uint8_t         lvr_enable;     /**< Enable low-voltage reset */
} power_config_t;

/*---------------------------------------------------------------------------
 * Power Management API
 *---------------------------------------------------------------------------*/
/**
 * @brief  Initialize power management service
 * @param  config  Power configuration (NULL for defaults)
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_Init(const power_config_t *config);

/**
 * @brief  De-initialize power management service
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_DeInit(void);

/**
 * @brief  Enter specified power mode
 * @param  mode  Power mode to enter
 * @return DRV_OK on wake-up, DRV_ERROR on failure
 */
drv_status_t SVC_POWER_EnterMode(power_mode_t mode);

/**
 * @brief  Enter idle mode
 * @note   CPU stops, peripherals continue running
 *         Wakes on any enabled interrupt
 */
void SVC_POWER_EnterIdle(void);

/**
 * @brief  Enter power-down mode
 * @note   Minimal power consumption, most peripherals stopped
 *         Wakes on configured wake-up sources only
 */
void SVC_POWER_EnterPowerDown(void);

/**
 * @brief  Get last wake-up source
 * @return Wake-up source that caused last exit from power-down
 */
wakeup_source_t SVC_POWER_GetWakeupSource(void);

/*---------------------------------------------------------------------------
 * Wake-up Timer Control
 *---------------------------------------------------------------------------*/
/**
 * @brief  Configure wake-up timer
 * @param  config  Wake-up timer configuration
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_ConfigWKT(const wkt_config_t *config);

/**
 * @brief  Start wake-up timer
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_StartWKT(void);

/**
 * @brief  Stop wake-up timer
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_StopWKT(void);

/*---------------------------------------------------------------------------
 * Brown-Out Detection
 *---------------------------------------------------------------------------*/
typedef enum {
    BOD_LEVEL_2_2V = 0,     /**< BOD at 2.2V */
    BOD_LEVEL_2_7V = 1,     /**< BOD at 2.7V */
    BOD_LEVEL_3_7V = 2,     /**< BOD at 3.7V */
    BOD_LEVEL_4_4V = 3      /**< BOD at 4.4V */
} bod_level_t;

/**
 * @brief  Configure brown-out detector
 * @param  level    BOD voltage level
 * @param  reset    Enable reset on BOD (1=reset, 0=interrupt)
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_ConfigBOD(bod_level_t level, uint8_t reset);

/**
 * @brief  Enable brown-out detector
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_EnableBOD(void);

/**
 * @brief  Disable brown-out detector
 * @return DRV_OK on success
 */
drv_status_t SVC_POWER_DisableBOD(void);

/*---------------------------------------------------------------------------
 * Power-on Reset Control
 *---------------------------------------------------------------------------*/
/**
 * @brief  Check if power-on reset occurred
 * @return 1 if POR occurred, 0 otherwise
 */
uint8_t SVC_POWER_IsPOR(void);

/**
 * @brief  Clear power-on reset flag
 */
void SVC_POWER_ClearPOR(void);

#ifdef __cplusplus
}
#endif

#endif /* SVC_POWER_H */
