/*===========================================================================
 * @file    drv_spi.h
 * @brief   SPI Driver Interface
 * @version 2.0.0
 * @date    2025-11-30
 *
 * @details Flexible SPI driver for MS51 microcontrollers. Supports blocking
 *          and interrupt-driven transfers with configurable clock polarity,
 *          phase, and data order.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_SPI_H
#define DRV_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Enumerations
 *---------------------------------------------------------------------------*/
typedef enum {
    SPI_MODE0 = 0,   /* CPOL = 0, CPHA = 0 */
    SPI_MODE1 = 1,   /* CPOL = 0, CPHA = 1 */
    SPI_MODE2 = 2,   /* CPOL = 1, CPHA = 0 */
    SPI_MODE3 = 3    /* CPOL = 1, CPHA = 1 */
} spi_mode_t;

typedef enum {
    SPI_ROLE_MASTER = 0,
    SPI_ROLE_SLAVE  = 1
} spi_role_t;

typedef enum {
    SPI_BIT_ORDER_MSB = 0,
    SPI_BIT_ORDER_LSB = 1
} spi_bit_order_t;

typedef enum {
    SPI_CLK_DIV2 = 0,
    SPI_CLK_DIV4 = 1,
    SPI_CLK_DIV8 = 2,
    SPI_CLK_DIV16 = 3
} spi_clock_div_t;

typedef enum {
    SPI_EVT_TRANSFER_DONE = 0,
    SPI_EVT_TRANSFER_ERROR = 1
} spi_event_t;

/*---------------------------------------------------------------------------
 * Configuration / Handle
 *---------------------------------------------------------------------------*/
typedef struct {
    spi_role_t       role;
    spi_mode_t       mode;
    spi_bit_order_t  bit_order;
    spi_clock_div_t  clock_div;
    uint8_t          hw_cs;        /* 1: enable SS as auto chip-select */
    drv_callback_t   callback;     /* Optional event callback */
} spi_config_t;

typedef struct {
    spi_config_t config;
    const uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint16_t length;
    uint16_t index;
    volatile uint8_t busy;
    drv_callback_t callback;
} spi_handle_t;

/*---------------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------------*/
drv_status_t DRV_SPI_Init(spi_handle_t *handle, const spi_config_t *config);
drv_status_t DRV_SPI_DeInit(spi_handle_t *handle);
drv_status_t DRV_SPI_RegisterCallback(spi_handle_t *handle, drv_callback_t callback);
drv_status_t DRV_SPI_Transfer(spi_handle_t *handle,
                              const uint8_t *tx_data,
                              uint8_t *rx_data,
                              uint16_t length,
                              uint32_t timeout);
drv_status_t DRV_SPI_TransferIT(spi_handle_t *handle,
                                const uint8_t *tx_data,
                                uint8_t *rx_data,
                                uint16_t length);
drv_status_t DRV_SPI_WriteByte(spi_handle_t *handle, uint8_t byte);
uint8_t      DRV_SPI_ReadByte(spi_handle_t *handle, uint8_t dummy);
uint8_t      DRV_SPI_IsBusy(spi_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* DRV_SPI_H */
