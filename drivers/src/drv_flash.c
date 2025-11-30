/*===========================================================================
 * @file    drv_flash.c
 * @brief   Flash / IAP Driver Implementation
 * @version 2.0.0
 * @date    2025-11-30
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_flash.h"

/*---------------------------------------------------------------------------
 * Local Helpers
 *---------------------------------------------------------------------------*/
#ifndef MS51_PAGE_SIZE
#define MS51_PAGE_SIZE  128U
#endif

#define LOBYTE(x)   ((uint8_t)((x) & 0xFFU))
#define HIBYTE(x)   ((uint8_t)(((x) >> 8) & 0xFFU))

typedef struct {
    uint8_t read_cmd;
    uint8_t prog_cmd;
    uint8_t erase_cmd;
} flash_cmd_table_t;

static const flash_cmd_table_t flash_cmds[] = {
    { 0x00U, 0x21U, 0x22U },   /* FLASH_REGION_APROM */
    { 0x40U, 0x61U, 0x62U },   /* FLASH_REGION_LDROM */
    { 0x00U, 0x21U, 0x22U }    /* FLASH_REGION_DATAFLASH (mirrors APROM) */
};

static drv_status_t flash_check_region(flash_region_t region);
static void flash_enable_region(flash_region_t region);
static void flash_disable_region(flash_region_t region);
static void flash_set_address(uint32_t address);

/*===========================================================================
 * Public API
 *===========================================================================*/

drv_status_t DRV_FLASH_Init(flash_handle_t *handle, const flash_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);

    if (flash_check_region(config->region) != DRV_OK) {
        return DRV_INVALID_PARAM;
    }

    handle->config = *config;
    if (handle->config.page_size == 0U) {
        handle->config.page_size = MS51_PAGE_SIZE;
    }

    return DRV_OK;
}

drv_status_t DRV_FLASH_Read(flash_handle_t *handle, uint32_t address, uint8_t *data, uint16_t length)
{
    uint16_t i;

    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(data);
    if (length == 0U) {
        return DRV_OK;
    }

    if (flash_check_region(handle->config.region) != DRV_OK) {
        return DRV_INVALID_PARAM;
    }

    set_CHPCON_IAPEN;
    flash_enable_region(handle->config.region);
    IAPCN = flash_cmds[handle->config.region].read_cmd;

    flash_set_address(address);
    for (i = 0; i < length; i++) {
        set_IAPTRG_IAPGO;
        data[i] = IAPFD;

        IAPAL++;
        if (IAPAL == 0U) {
            IAPAH++;
        }
    }

    flash_disable_region(handle->config.region);
    clr_CHPCON_IAPEN;

    return DRV_OK;
}

drv_status_t DRV_FLASH_Write(flash_handle_t *handle, uint32_t address, const uint8_t *data, uint16_t length)
{
    uint16_t i;

    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(data);
    if (length == 0U) {
        return DRV_OK;
    }

    if (flash_check_region(handle->config.region) != DRV_OK) {
        return DRV_INVALID_PARAM;
    }

    set_CHPCON_IAPEN;
    flash_enable_region(handle->config.region);
    IAPCN = flash_cmds[handle->config.region].prog_cmd;

    flash_set_address(address);
    for (i = 0; i < length; i++) {
        IAPFD = data[i];
        set_IAPTRG_IAPGO_WDCLR;

        IAPAL++;
        if (IAPAL == 0U) {
            IAPAH++;
        }
    }

    flash_disable_region(handle->config.region);
    clr_CHPCON_IAPEN;

    return DRV_OK;
}

drv_status_t DRV_FLASH_Erase(flash_handle_t *handle, uint32_t address, uint16_t length)
{
    uint16_t pages;
    uint16_t page_size;
    uint16_t page;

    DRV_CHECK_PTR(handle);
    if (length == 0U) {
        return DRV_INVALID_PARAM;
    }

    if (flash_check_region(handle->config.region) != DRV_OK) {
        return DRV_INVALID_PARAM;
    }

    page_size = handle->config.page_size;
    if ((length % page_size) != 0U) {
        return DRV_INVALID_PARAM;
    }

    pages = length / page_size;

    set_CHPCON_IAPEN;
    flash_enable_region(handle->config.region);
    IAPCN = flash_cmds[handle->config.region].erase_cmd;
    IAPFD = 0xFFU;

    for (page = 0; page < pages; page++) {
        uint32_t addr = address + ((uint32_t)page * page_size);
        flash_set_address(addr);
        set_IAPTRG_IAPGO_WDCLR;
    }

    flash_disable_region(handle->config.region);
    clr_CHPCON_IAPEN;

    return DRV_OK;
}

drv_status_t DRV_FLASH_BlankCheck(flash_handle_t *handle, uint32_t address, uint16_t length)
{
    uint16_t i;

    DRV_CHECK_PTR(handle);
    if (length == 0U) {
        return DRV_INVALID_PARAM;
    }

    if (flash_check_region(handle->config.region) != DRV_OK) {
        return DRV_INVALID_PARAM;
    }

    set_CHPCON_IAPEN;
    flash_enable_region(handle->config.region);
    IAPCN = flash_cmds[handle->config.region].read_cmd;

    flash_set_address(address);
    for (i = 0; i < length; i++) {
        set_IAPTRG_IAPGO;
        if (IAPFD != 0xFFU) {
            flash_disable_region(handle->config.region);
            clr_CHPCON_IAPEN;
            return DRV_ERROR;
        }
        IAPAL++;
        if (IAPAL == 0U) {
            IAPAH++;
        }
    }

    flash_disable_region(handle->config.region);
    clr_CHPCON_IAPEN;

    return DRV_OK;
}

/*===========================================================================
 * Private Helpers
 *===========================================================================*/

static drv_status_t flash_check_region(flash_region_t region)
{
    if (region > FLASH_REGION_DATAFLASH) {
        return DRV_INVALID_PARAM;
    }
    return DRV_OK;
}

static void flash_enable_region(flash_region_t region)
{
    switch (region) {
        case FLASH_REGION_APROM:
        case FLASH_REGION_DATAFLASH:
            set_IAPUEN_APUEN;
            break;
        case FLASH_REGION_LDROM:
            set_IAPUEN_LDUEN;
            break;
        default:
            break;
    }
}

static void flash_disable_region(flash_region_t region)
{
    switch (region) {
        case FLASH_REGION_APROM:
        case FLASH_REGION_DATAFLASH:
            clr_IAPUEN_APUEN;
            break;
        case FLASH_REGION_LDROM:
            clr_IAPUEN_LDUEN;
            break;
        default:
            break;
    }
}

static void flash_set_address(uint32_t address)
{
    IAPAL = LOBYTE(address);
    IAPAH = HIBYTE(address);
}
