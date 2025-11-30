/*===========================================================================
 * @file    drv_i2c.c
 * @brief   I2C Driver Implementation
 * @version 2.0.0
 * @date    2025-11-30
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_i2c.h"
#include <string.h>

/*---------------------------------------------------------------------------
 * Local Definitions
 *---------------------------------------------------------------------------*/
#define I2C_STATUS_START            0x08U
#define I2C_STATUS_REP_START        0x10U
#define I2C_STATUS_SLA_W_ACK        0x18U
#define I2C_STATUS_SLA_W_NACK       0x20U
#define I2C_STATUS_DATA_W_ACK       0x28U
#define I2C_STATUS_DATA_W_NACK      0x30U
#define I2C_STATUS_SLA_R_ACK        0x40U
#define I2C_STATUS_SLA_R_NACK       0x48U
#define I2C_STATUS_DATA_R_ACK       0x50U
#define I2C_STATUS_DATA_R_NACK      0x58U

#define I2C_TIMEOUT_DEFAULT         0xFFFFUL

/*---------------------------------------------------------------------------
 * Static Data
 *---------------------------------------------------------------------------*/
static i2c_handle_t *i2c0_handle = NULL;

/*---------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------*/
static void i2c_config_pins(void);
static uint8_t i2c_calc_divider(uint32_t bus_hz);
static drv_status_t i2c_wait_si(uint32_t timeout);
static void i2c_send_stop(void);
static drv_status_t i2c_send_byte(uint8_t data, uint32_t timeout);

/*===========================================================================
 * Public Functions
 *===========================================================================*/

drv_status_t DRV_I2C_Init(i2c_handle_t *handle, const i2c_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);

    memset(handle, 0, sizeof(i2c_handle_t));
    handle->config = *config;
    handle->callback = config->callback;

    i2c_config_pins();

    HAL_SFR_PAGE0();
    I2CON = 0x00;
    I2CLK = i2c_calc_divider(config->bus_hz ? config->bus_hz : 100000UL);
    I2ADDR = (config->own_address & 0xFEU);
    I2DAT = 0x00;

    if (config->general_call) {
        I2ADDR |= 0x01U;
    }

    set_I2CON_I2CEN;

    i2c0_handle = handle;
    return DRV_OK;
}

drv_status_t DRV_I2C_DeInit(i2c_handle_t *handle)
{
    DRV_CHECK_PTR(handle);

    HAL_SFR_PAGE0();
    clr_I2CON_I2CEN;
    handle->busy = 0;
    i2c0_handle = NULL;

    return DRV_OK;
}

drv_status_t DRV_I2C_RegisterCallback(i2c_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    handle->callback = callback;
    return DRV_OK;
}

drv_status_t DRV_I2C_MasterTransfer(i2c_handle_t *handle,
                                    uint8_t address,
                                    const uint8_t *tx_buf,
                                    uint16_t tx_len,
                                    uint8_t *rx_buf,
                                    uint16_t rx_len,
                                    uint32_t timeout)
{
    uint16_t i;
    drv_status_t status;

    DRV_CHECK_PTR(handle);
    if (handle->config.role != I2C_ROLE_MASTER) {
        return DRV_NOT_SUPPORTED;
    }

    if ((tx_len == 0) && (rx_len == 0)) {
        return DRV_INVALID_PARAM;
    }

    if (handle->busy) {
        return DRV_BUSY;
    }

    if (timeout == 0) {
        timeout = I2C_TIMEOUT_DEFAULT;
    }

    handle->busy = 1;
    HAL_SFR_PAGE0();

    /* Start condition */
    set_I2CON_STA;
    clr_I2CON_SI;
    status = i2c_wait_si(timeout);
    if (status != DRV_OK || I2STAT != I2C_STATUS_START) {
        i2c_send_stop();
        handle->busy = 0;
        return DRV_ERROR;
    }

    /* Send slave address with write if TX data present */
    if (tx_len > 0) {
        I2DAT = (uint8_t)((address << 1) & 0xFEU);
        clr_I2CON_STA;
        clr_I2CON_SI;
        status = i2c_wait_si(timeout);
        if (status != DRV_OK) {
            i2c_send_stop();
            handle->busy = 0;
            return status;
        }
        if (I2STAT != I2C_STATUS_SLA_W_ACK) {
            i2c_send_stop();
            handle->busy = 0;
            return DRV_ERROR;
        }

        for (i = 0; i < tx_len; i++) {
            status = i2c_send_byte(tx_buf ? tx_buf[i] : 0xFFU, timeout);
            if (status != DRV_OK) {
                i2c_send_stop();
                handle->busy = 0;
                return status;
            }
        }
    }

    /* Handle read phase */
    if (rx_len > 0U) {
        /* Issue repeated start if write phase happened */
        set_I2CON_STA;
        clr_I2CON_SI;
        status = i2c_wait_si(timeout);
        if (status != DRV_OK || I2STAT != I2C_STATUS_REP_START) {
            i2c_send_stop();
            handle->busy = 0;
            return DRV_ERROR;
        }

        I2DAT = (uint8_t)((address << 1) | 0x01U);
        clr_I2CON_STA;
        clr_I2CON_SI;
        status = i2c_wait_si(timeout);
        if (status != DRV_OK) {
            i2c_send_stop();
            handle->busy = 0;
            return status;
        }
        if (I2STAT != I2C_STATUS_SLA_R_ACK) {
            i2c_send_stop();
            handle->busy = 0;
            return DRV_ERROR;
        }

        for (i = 0; i < rx_len; i++) {
            if (i == (rx_len - 1U)) {
                clr_I2CON_AA;   /* NACK for last byte */
            } else {
                set_I2CON_AA;
            }

            clr_I2CON_SI;
            status = i2c_wait_si(timeout);
            if (status != DRV_OK) {
                i2c_send_stop();
                handle->busy = 0;
                return status;
            }

            if (rx_buf) {
                rx_buf[i] = I2DAT;
            }
        }
    }

    i2c_send_stop();
    handle->busy = 0;

    if (handle->callback) {
        handle->callback(rx_len ? I2C_EVENT_RX_DONE : I2C_EVENT_TX_DONE, NULL);
    }

    return DRV_OK;
}

drv_status_t DRV_I2C_MasterWrite(i2c_handle_t *handle,
                                 uint8_t address,
                                 const uint8_t *tx_buf,
                                 uint16_t tx_len,
                                 uint32_t timeout)
{
    return DRV_I2C_MasterTransfer(handle, address, tx_buf, tx_len, NULL, 0, timeout);
}

drv_status_t DRV_I2C_MasterRead(i2c_handle_t *handle,
                                uint8_t address,
                                uint8_t *rx_buf,
                                uint16_t rx_len,
                                uint32_t timeout)
{
    return DRV_I2C_MasterTransfer(handle, address, NULL, 0, rx_buf, rx_len, timeout);
}

uint8_t DRV_I2C_IsBusy(i2c_handle_t *handle)
{
    if (handle == NULL) {
        return 0;
    }
    return handle->busy;
}

/*===========================================================================
 * Private Helpers
 *===========================================================================*/

static void i2c_config_pins(void)
{
    P13_OPENDRAIN_MODE;   /* SCL */
    P14_OPENDRAIN_MODE;   /* SDA */
    P13 = 1;
    P14 = 1;
}

static uint8_t i2c_calc_divider(uint32_t bus_hz)
{
    uint32_t fsys = HAL_SYS_GetClock();
    uint32_t value;

    if (bus_hz == 0U || bus_hz > (fsys / 4U)) {
        bus_hz = 100000UL;
    }

    value = (fsys / (4UL * bus_hz)) - 1UL;
    if (value > 0xFFUL) {
        value = 0xFFUL;
    }

    if (value == 0U) {
        value = 1U;
    }

    return (uint8_t)value;
}

static drv_status_t i2c_wait_si(uint32_t timeout)
{
    while (!SI) {
        if (--timeout == 0U) {
            return DRV_TIMEOUT;
        }
    }
    return DRV_OK;
}

static void i2c_send_stop(void)
{
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);
}

static drv_status_t i2c_send_byte(uint8_t data, uint32_t timeout)
{
    drv_status_t status;

    I2DAT = data;
    clr_I2CON_SI;
    status = i2c_wait_si(timeout);
    if (status != DRV_OK) {
        return status;
    }

    if (I2STAT != I2C_STATUS_DATA_W_ACK) {
        return DRV_ERROR;
    }

    return DRV_OK;
}
