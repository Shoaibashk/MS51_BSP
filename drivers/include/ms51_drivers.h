/**
 * @file ms51_drivers.h
 * @brief Master Driver Include File for MS51 Series
 * @details Include this single header to access all MS51 driver APIs
 * @version 2.0.0
 * @date 2025
 * 
 * @copyright Copyright (c) 2025 Nuvoton Technology Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MS51_DRIVERS_H
#define MS51_DRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/* Common Driver Definitions                                                  */
/*===========================================================================*/
#include "drv_common.h"

/*===========================================================================*/
/* Peripheral Drivers                                                         */
/*===========================================================================*/
#include "drv_gpio.h"       /**< GPIO - General Purpose I/O */
#include "drv_uart.h"       /**< UART - Asynchronous serial */
#include "drv_timer.h"      /**< Timer - Timer0/1/2/3 and WKT */
#include "drv_adc.h"        /**< ADC - 12-bit analog-to-digital converter */
#include "drv_pwm.h"        /**< PWM - 6-channel pulse width modulation */
#include "drv_spi.h"        /**< SPI - Serial peripheral interface */
#include "drv_i2c.h"        /**< I2C - Inter-integrated circuit */
#include "drv_wdt.h"        /**< WDT - Watchdog timer */
#include "drv_flash.h"      /**< Flash - IAP/APROM/DataFlash access */

/*===========================================================================*/
/* Version Information                                                        */
/*===========================================================================*/
#define MS51_DRV_VERSION_MAJOR  2
#define MS51_DRV_VERSION_MINOR  0
#define MS51_DRV_VERSION_PATCH  0
#define MS51_DRV_VERSION        ((MS51_DRV_VERSION_MAJOR << 16) | \
                                 (MS51_DRV_VERSION_MINOR << 8)  | \
                                 MS51_DRV_VERSION_PATCH)

/**
 * @brief Get driver version as string
 * @return Version string "X.Y.Z"
 */
#define MS51_DRV_VERSION_STRING "2.0.0"

#ifdef __cplusplus
}
#endif

#endif /* MS51_DRIVERS_H */
