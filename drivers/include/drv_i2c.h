/*===========================================================================
 * @file    drv_i2c.h
 * @brief   I2C Driver Interface
 * @version 2.0.0
 * @date    2025-11-30
 *
 * @details High-level I2C master/slave driver for MS51 devices. Provides
 *          blocking transfers with error/status reporting.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_I2C_H
#define DRV_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Enumerations
 *---------------------------------------------------------------------------*/
typedef enum {
    I2C_ROLE_MASTER = 0,
    I2C_ROLE_SLAVE  = 1
} i2c_role_t;

typedef enum {
    I2C_EVENT_TX_DONE = 0,
    I2C_EVENT_RX_DONE = 1,
    I2C_EVENT_ERROR   = 2
} i2c_event_t;

/*---------------------------------------------------------------------------
 * Configuration / Handle
 *---------------------------------------------------------------------------*/
typedef struct {
    i2c_role_t   role;
    uint32_t     bus_hz;          /* Desired bus speed (Hz) */
    uint8_t      own_address;     /* 7-bit address for slave mode */
    uint8_t      general_call;    /* Respond to general call */
    drv_callback_t callback;      /* Optional IRQ callback */
} i2c_config_t;

typedef struct {
    i2c_config_t config;
    drv_callback_t callback;
    volatile uint8_t busy;
} i2c_handle_t;

/*---------------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------------*/
drv_status_t DRV_I2C_Init(i2c_handle_t *handle, const i2c_config_t *config);
drv_status_t DRV_I2C_DeInit(i2c_handle_t *handle);
drv_status_t DRV_I2C_RegisterCallback(i2c_handle_t *handle, drv_callback_t callback);
drv_status_t DRV_I2C_MasterTransfer(i2c_handle_t *handle,
                                    uint8_t address,
                                    const uint8_t *tx_buf,
                                    uint16_t tx_len,
                                    uint8_t *rx_buf,
                                    uint16_t rx_len,
                                    uint32_t timeout);
drv_status_t DRV_I2C_MasterWrite(i2c_handle_t *handle,
                                 uint8_t address,
                                 const uint8_t *tx_buf,
                                 uint16_t tx_len,
                                 uint32_t timeout);
drv_status_t DRV_I2C_MasterRead(i2c_handle_t *handle,
                                uint8_t address,
                                uint8_t *rx_buf,
                                uint16_t rx_len,
                                uint32_t timeout);
uint8_t DRV_I2C_IsBusy(i2c_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* DRV_I2C_H */
