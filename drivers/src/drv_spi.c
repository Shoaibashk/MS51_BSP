/*===========================================================================
 * @file    drv_spi.c
 * @brief   SPI Driver Implementation
 * @version 2.0.0
 * @date    2025-11-30
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_spi.h"
#include <string.h>

/*---------------------------------------------------------------------------
 * Local Definitions
 *---------------------------------------------------------------------------*/
#define SPCR_SSOE      0x80U
#define SPCR_SPIEN     0x40U
#define SPCR_LSBFE     0x20U
#define SPCR_MSTR      0x10U
#define SPCR_CPOL      0x08U
#define SPCR_CPHA      0x04U
#define SPCR_SPR_MASK  0x03U

#define SPSR_SPIF      0x80U
#define SPSR_WCOL      0x40U
#define SPSR_SPIOVF    0x20U
#define SPSR_MODF      0x10U
#define SPSR_DISMODF   0x08U

#define SPI_TIMEOUT_DEFAULT   0xFFFFUL

/*---------------------------------------------------------------------------
 * Static Data
 *---------------------------------------------------------------------------*/
static spi_handle_t *spi0_handle = NULL;

/*---------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------*/
static void spi_config_pins(const spi_config_t *config);
static void spi_apply_mode(const spi_config_t *config);
static drv_status_t spi_wait_flag(uint32_t timeout);

/*===========================================================================
 * Public Functions
 *===========================================================================*/

drv_status_t DRV_SPI_Init(spi_handle_t *handle, const spi_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);

    memset(handle, 0, sizeof(spi_handle_t));
    handle->config = *config;
    handle->callback = config->callback;

    HAL_SFR_PAGE0();
    spi_config_pins(config);

    /* Reset SPI registers */
    SPCR = 0x00;
    SPSR = 0x00;

    spi_apply_mode(config);

    if (config->clock_div <= SPI_CLK_DIV16) {
        SPCR = (SPCR & ~SPCR_SPR_MASK) | (config->clock_div & SPCR_SPR_MASK);
    }

    if (config->bit_order == SPI_BIT_ORDER_LSB) {
        SPCR |= SPCR_LSBFE;
    } else {
        SPCR &= ~SPCR_LSBFE;
    }

    if (config->role == SPI_ROLE_MASTER) {
        SPCR |= SPCR_MSTR;
        SPSR |= SPSR_DISMODF;    /* Avoid mode fault when using GPIO for SS */
    } else {
        SPCR &= ~SPCR_MSTR;
    }

    if (config->hw_cs) {
        SPCR |= SPCR_SSOE;
    } else {
        SPCR &= ~SPCR_SSOE;
    }

    SPCR |= SPCR_SPIEN;

    spi0_handle = handle;
    return DRV_OK;
}

drv_status_t DRV_SPI_DeInit(spi_handle_t *handle)
{
    DRV_CHECK_PTR(handle);

    HAL_SFR_PAGE0();
    SPCR &= ~SPCR_SPIEN;
    SPSR = 0x00;

    spi0_handle = NULL;
    handle->busy = 0;

    return DRV_OK;
}

drv_status_t DRV_SPI_RegisterCallback(spi_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    handle->callback = callback;
    return DRV_OK;
}

drv_status_t DRV_SPI_Transfer(spi_handle_t *handle,
                              const uint8_t *tx_data,
                              uint8_t *rx_data,
                              uint16_t length,
                              uint32_t timeout)
{
    uint16_t i;

    DRV_CHECK_PTR(handle);
    if (length == 0) {
        return DRV_OK;
    }

    if (handle->busy) {
        return DRV_BUSY;
    }

    if (timeout == 0) {
        timeout = SPI_TIMEOUT_DEFAULT;
    }

    handle->busy = 1;
    HAL_SFR_PAGE0();

    for (i = 0; i < length; i++) {
        uint8_t value = tx_data ? tx_data[i] : 0xFFU;
        SPSR &= ~(SPSR_SPIF | SPSR_WCOL | SPSR_SPIOVF | SPSR_MODF);
        SPDR = value;

        if (spi_wait_flag(timeout) != DRV_OK) {
            handle->busy = 0;
            return DRV_TIMEOUT;
        }

        if (SPSR & (SPSR_WCOL | SPSR_MODF)) {
            handle->busy = 0;
            return DRV_ERROR;
        }

        if (rx_data) {
            rx_data[i] = SPDR;
        } else {
            (void)SPDR;
        }
    }

    handle->busy = 0;
    return DRV_OK;
}

drv_status_t DRV_SPI_TransferIT(spi_handle_t *handle,
                                const uint8_t *tx_data,
                                uint8_t *rx_data,
                                uint16_t length)
{
    DRV_CHECK_PTR(handle);
    if (length == 0) {
        return DRV_INVALID_PARAM;
    }

    if (handle->busy) {
        return DRV_BUSY;
    }

    handle->tx_buf = tx_data;
    handle->rx_buf = rx_data;
    handle->length = length;
    handle->index = 0;
    handle->busy = 1;

    HAL_SFR_PAGE0();
    SPSR &= ~(SPSR_SPIF | SPSR_WCOL | SPSR_SPIOVF | SPSR_MODF);

    /* Prime first byte */
    {
        uint8_t first = (tx_data != NULL) ? tx_data[0] : 0xFFU;
        handle->index = 1;
        SPDR = first;
    }

    set_EIE_ESPI;

    return DRV_OK;
}

drv_status_t DRV_SPI_WriteByte(spi_handle_t *handle, uint8_t byte)
{
    return DRV_SPI_Transfer(handle, &byte, NULL, 1, SPI_TIMEOUT_DEFAULT);
}

uint8_t DRV_SPI_ReadByte(spi_handle_t *handle, uint8_t dummy)
{
    uint8_t value = 0xFFU;
    DRV_SPI_Transfer(handle, &dummy, &value, 1, SPI_TIMEOUT_DEFAULT);
    return value;
}

uint8_t DRV_SPI_IsBusy(spi_handle_t *handle)
{
    if (handle == NULL) {
        return 0;
    }
    return handle->busy;
}

/*===========================================================================
 * Private Helpers
 *===========================================================================*/

static void spi_config_pins(const spi_config_t *config)
{
    if (config->role == SPI_ROLE_MASTER) {
        P05_PUSHPULL_MODE;  /* SS */
        P10_PUSHPULL_MODE;  /* SPCLK */
        P00_PUSHPULL_MODE;  /* MOSI */
        P01_INPUT_MODE;     /* MISO */
        P05 = 1;
    } else {
        P05_QUASI_MODE;
        P10_QUASI_MODE;
        P00_QUASI_MODE;
        P01_QUASI_MODE;
    }
}

static void spi_apply_mode(const spi_config_t *config)
{
    switch (config->mode) {
        case SPI_MODE0:
            SPCR &= ~(SPCR_CPOL | SPCR_CPHA);
            break;
        case SPI_MODE1:
            SPCR &= ~SPCR_CPOL;
            SPCR |= SPCR_CPHA;
            break;
        case SPI_MODE2:
            SPCR |= SPCR_CPOL;
            SPCR &= ~SPCR_CPHA;
            break;
        case SPI_MODE3:
        default:
            SPCR |= (SPCR_CPOL | SPCR_CPHA);
            break;
    }
}

static drv_status_t spi_wait_flag(uint32_t timeout)
{
    while (!(SPSR & SPSR_SPIF)) {
        if (--timeout == 0) {
            return DRV_TIMEOUT;
        }
    }

    SPSR &= ~SPSR_SPIF;
    return DRV_OK;
}

/*===========================================================================
 * Interrupt Handler
 *===========================================================================*/

#if defined(COMPILER_KEIL)
void SPI0_ISR(void) interrupt 9
#elif defined(COMPILER_IAR)
#pragma vector = 0x4B
__interrupt void SPI0_ISR(void)
#elif defined(COMPILER_SDCC)
void SPI0_ISR(void) __interrupt(9)
#endif
#if !defined(COMPILER_GCC)
{
    spi_handle_t *handle = spi0_handle;
    uint8_t status;

    HAL_SFR_PAGE0();
    status = SPSR;
    SPSR &= ~SPSR_SPIF;

    if ((status & (SPSR_WCOL | SPSR_MODF)) != 0U) {
        if (handle && handle->callback) {
            handle->busy = 0;
            handle->callback(SPI_EVT_TRANSFER_ERROR, NULL);
        }
        SPSR &= ~(SPSR_WCOL | SPSR_MODF);
        return;
    }

    if (handle == NULL || !handle->busy) {
        (void)SPDR;
        return;
    }

    if (handle->rx_buf && handle->index) {
        handle->rx_buf[handle->index - 1U] = SPDR;
    } else {
        (void)SPDR;
    }

    if (handle->index < handle->length) {
        uint8_t next = handle->tx_buf ? handle->tx_buf[handle->index] : 0xFFU;
        handle->index++;
        SPDR = next;
    } else {
        handle->busy = 0;
        clr_EIE_ESPI;

        if (handle->callback) {
            handle->callback(SPI_EVT_TRANSFER_DONE, NULL);
        }
    }
}
#endif
