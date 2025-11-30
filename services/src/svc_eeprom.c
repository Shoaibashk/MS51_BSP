/*===========================================================================
 * @file    svc_eeprom.c
 * @brief   EEPROM Emulation Service Implementation
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "svc_eeprom.h"
#include "drv_flash.h"

/*---------------------------------------------------------------------------
 * Local Variables
 *---------------------------------------------------------------------------*/
static flash_handle_t g_flash_handle;
static eeprom_config_t g_config;
static uint32_t g_write_count = 0;
static uint8_t g_initialized = 0;

/*---------------------------------------------------------------------------
 * CRC-8 Calculation
 *---------------------------------------------------------------------------*/
static uint8_t calc_crc8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0xFF;
    uint16_t i;
    uint8_t j;
    
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

/*===========================================================================
 * Public API Implementation
 *===========================================================================*/

drv_status_t SVC_EEPROM_Init(const eeprom_config_t *config)
{
    flash_config_t flash_cfg;
    drv_status_t status;

    if (config != NULL) {
        g_config = *config;
    } else {
        g_config.base_address = EEPROM_BASE_ADDR;
        g_config.size = EEPROM_SIZE;
        g_config.page_size = EEPROM_PAGE_SIZE;
        g_config.wear_leveling = 0;
    }

    flash_cfg.region = FLASH_REGION_DATAFLASH;
    flash_cfg.page_size = g_config.page_size;

    status = DRV_FLASH_Init(&g_flash_handle, &flash_cfg);
    if (status != DRV_OK) {
        return status;
    }

    g_write_count = 0;
    g_initialized = 1;
    
    return DRV_OK;
}

drv_status_t SVC_EEPROM_DeInit(void)
{
    g_initialized = 0;
    return DRV_OK;
}

drv_status_t SVC_EEPROM_ReadByte(uint16_t address, uint8_t *data)
{
    if (!g_initialized) return DRV_NOT_INIT;
    DRV_CHECK_PTR(data);
    
    if (address >= g_config.size) {
        return DRV_INVALID_PARAM;
    }

    return DRV_FLASH_Read(&g_flash_handle, 
                          g_config.base_address + address,
                          data, 1);
}

drv_status_t SVC_EEPROM_WriteByte(uint16_t address, uint8_t data)
{
    uint8_t current;
    drv_status_t status;
    uint16_t page_offset;
    uint16_t page_addr;
    uint8_t page_buffer[EEPROM_PAGE_SIZE];
    uint16_t i;

    if (!g_initialized) return DRV_NOT_INIT;
    
    if (address >= g_config.size) {
        return DRV_INVALID_PARAM;
    }

    /* Read current value */
    status = SVC_EEPROM_ReadByte(address, &current);
    if (status != DRV_OK) return status;

    /* Check if write is needed */
    if (current == data) {
        return DRV_OK;
    }

    /* Check if we can just program (all bits going 1->0) */
    if ((current & data) == data) {
        status = DRV_FLASH_Write(&g_flash_handle,
                                 g_config.base_address + address,
                                 &data, 1);
        if (status == DRV_OK) g_write_count++;
        return status;
    }

    /* Need to erase page first - read entire page */
    page_offset = address % g_config.page_size;
    page_addr = address - page_offset;

    status = DRV_FLASH_Read(&g_flash_handle,
                            g_config.base_address + page_addr,
                            page_buffer, g_config.page_size);
    if (status != DRV_OK) return status;

    /* Update the byte in buffer */
    page_buffer[page_offset] = data;

    /* Erase page */
    status = DRV_FLASH_Erase(&g_flash_handle,
                             g_config.base_address + page_addr,
                             g_config.page_size);
    if (status != DRV_OK) return status;

    /* Reprogram page */
    status = DRV_FLASH_Write(&g_flash_handle,
                             g_config.base_address + page_addr,
                             page_buffer, g_config.page_size);
    if (status == DRV_OK) g_write_count++;
    
    return status;
}

drv_status_t SVC_EEPROM_Read(uint16_t address, uint8_t *data, uint16_t length)
{
    if (!g_initialized) return DRV_NOT_INIT;
    DRV_CHECK_PTR(data);
    
    if (address + length > g_config.size) {
        return DRV_INVALID_PARAM;
    }

    return DRV_FLASH_Read(&g_flash_handle,
                          g_config.base_address + address,
                          data, length);
}

drv_status_t SVC_EEPROM_Write(uint16_t address, const uint8_t *data, uint16_t length)
{
    uint16_t i;
    drv_status_t status;

    if (!g_initialized) return DRV_NOT_INIT;
    DRV_CHECK_PTR(data);
    
    if (address + length > g_config.size) {
        return DRV_INVALID_PARAM;
    }

    /* Write byte by byte to handle page boundaries and wear leveling */
    for (i = 0; i < length; i++) {
        status = SVC_EEPROM_WriteByte(address + i, data[i]);
        if (status != DRV_OK) return status;
    }

    return DRV_OK;
}

drv_status_t SVC_EEPROM_EraseAll(void)
{
    uint16_t pages;
    uint16_t i;
    drv_status_t status;

    if (!g_initialized) return DRV_NOT_INIT;

    pages = g_config.size / g_config.page_size;
    
    for (i = 0; i < pages; i++) {
        status = SVC_EEPROM_ErasePage(i);
        if (status != DRV_OK) return status;
    }

    g_write_count = 0;
    return DRV_OK;
}

drv_status_t SVC_EEPROM_ErasePage(uint16_t page_num)
{
    uint16_t max_pages;
    uint32_t page_addr;

    if (!g_initialized) return DRV_NOT_INIT;

    max_pages = g_config.size / g_config.page_size;
    if (page_num >= max_pages) {
        return DRV_INVALID_PARAM;
    }

    page_addr = g_config.base_address + ((uint32_t)page_num * g_config.page_size);
    
    return DRV_FLASH_Erase(&g_flash_handle, page_addr, g_config.page_size);
}

/*---------------------------------------------------------------------------
 * Structured Data Support
 *---------------------------------------------------------------------------*/
drv_status_t SVC_EEPROM_ReadStruct(uint16_t address, void *data, uint16_t size)
{
    uint8_t stored_crc;
    uint8_t calc_crc;
    drv_status_t status;

    if (!g_initialized) return DRV_NOT_INIT;
    DRV_CHECK_PTR(data);

    if (address + size + 1 > g_config.size) {
        return DRV_INVALID_PARAM;
    }

    /* Read data */
    status = SVC_EEPROM_Read(address, (uint8_t *)data, size);
    if (status != DRV_OK) return status;

    /* Read stored CRC */
    status = SVC_EEPROM_ReadByte(address + size, &stored_crc);
    if (status != DRV_OK) return status;

    /* Verify CRC */
    calc_crc = calc_crc8((uint8_t *)data, size);
    if (calc_crc != stored_crc) {
        return DRV_ERROR;
    }

    return DRV_OK;
}

drv_status_t SVC_EEPROM_WriteStruct(uint16_t address, const void *data, uint16_t size)
{
    uint8_t crc;
    drv_status_t status;

    if (!g_initialized) return DRV_NOT_INIT;
    DRV_CHECK_PTR(data);

    if (address + size + 1 > g_config.size) {
        return DRV_INVALID_PARAM;
    }

    /* Write data */
    status = SVC_EEPROM_Write(address, (const uint8_t *)data, size);
    if (status != DRV_OK) return status;

    /* Calculate and write CRC */
    crc = calc_crc8((const uint8_t *)data, size);
    return SVC_EEPROM_WriteByte(address + size, crc);
}

/*---------------------------------------------------------------------------
 * Status and Diagnostics
 *---------------------------------------------------------------------------*/
uint32_t SVC_EEPROM_GetWriteCount(void)
{
    return g_write_count;
}

drv_status_t SVC_EEPROM_VerifyIntegrity(void)
{
    /* Simple check: verify we can read all addresses */
    uint8_t dummy;
    uint16_t i;
    drv_status_t status;

    if (!g_initialized) return DRV_NOT_INIT;

    for (i = 0; i < g_config.size; i += g_config.page_size) {
        status = SVC_EEPROM_ReadByte(i, &dummy);
        if (status != DRV_OK) return DRV_ERROR;
    }

    return DRV_OK;
}
