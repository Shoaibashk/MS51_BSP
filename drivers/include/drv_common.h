/*===========================================================================
 * @file    drv_common.h
 * @brief   Common Driver Definitions
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_COMMON_H
#define DRV_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*---------------------------------------------------------------------------
 * Driver Status Codes
 *---------------------------------------------------------------------------*/
typedef enum {
    DRV_OK              =  0,   /**< Operation successful */
    DRV_ERROR           = -1,   /**< General error */
    DRV_BUSY            = -2,   /**< Resource is busy */
    DRV_TIMEOUT         = -3,   /**< Operation timed out */
    DRV_INVALID_PARAM   = -4,   /**< Invalid parameter */
    DRV_NOT_SUPPORTED   = -5,   /**< Feature not supported */
    DRV_NOT_INIT        = -6,   /**< Not initialized */
    DRV_NO_DATA         = -7,   /**< No data available */
    DRV_OVERRUN         = -8,   /**< Buffer overrun */
    DRV_ERROR_RESOURCE  = -9    /**< Resource not available */
} drv_status_t;

/* Status code aliases for compatibility */
#define DRV_ERROR_PARAMETER     DRV_INVALID_PARAM
#define DRV_SUCCESS             DRV_OK

/*---------------------------------------------------------------------------
 * Callback Types
 *---------------------------------------------------------------------------*/

/**
 * @brief Generic driver callback function type
 * @param event  Event code (driver-specific)
 * @param param  Event parameter (driver-specific)
 */
typedef void (*drv_callback_t)(uint8_t event, void *param);

/*---------------------------------------------------------------------------
 * Common Timeout Values
 *---------------------------------------------------------------------------*/
#define DRV_TIMEOUT_NONE        0           /**< No timeout (non-blocking) */
#define DRV_TIMEOUT_FOREVER     0xFFFFFFFF  /**< Wait forever */

/*---------------------------------------------------------------------------
 * Utility Macros
 *---------------------------------------------------------------------------*/

/**
 * @brief Check if pointer is NULL and return error
 */
#define DRV_CHECK_PTR(ptr)  do { if ((ptr) == NULL) return DRV_INVALID_PARAM; } while(0)

/**
 * @brief Check condition and return error if false
 */
#define DRV_CHECK_COND(cond, err)  do { if (!(cond)) return (err); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* DRV_COMMON_H */
