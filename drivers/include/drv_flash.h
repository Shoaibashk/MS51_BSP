/*===========================================================================
 * @file    drv_flash.h
 * @brief   Flash / IAP Driver Interface
 * @version 2.0.0
 * @date    2025-11-30
 *
 * @details Safe helpers for APROM/LDROM/DataFlash erase, program and read
 *          operations using the MS51 IAP controller.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_FLASH_H
#define DRV_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Enumerations
 *---------------------------------------------------------------------------*/
typedef enum {
    FLASH_REGION_APROM = 0,
    FLASH_REGION_LDROM = 1,
    FLASH_REGION_DATAFLASH = 2
} flash_region_t;

/*---------------------------------------------------------------------------
 * Configuration / Handle
 *---------------------------------------------------------------------------*/
typedef struct {
    flash_region_t region;
    uint16_t       page_size;     /* Defaults to device PAGE size (128B) */
} flash_config_t;

typedef struct {
    flash_config_t config;
} flash_handle_t;

/*---------------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------------*/
drv_status_t DRV_FLASH_Init(flash_handle_t *handle, const flash_config_t *config);
drv_status_t DRV_FLASH_Read(flash_handle_t *handle, uint32_t address, uint8_t *data, uint16_t length);
drv_status_t DRV_FLASH_Write(flash_handle_t *handle, uint32_t address, const uint8_t *data, uint16_t length);
drv_status_t DRV_FLASH_Erase(flash_handle_t *handle, uint32_t address, uint16_t length);
drv_status_t DRV_FLASH_BlankCheck(flash_handle_t *handle, uint32_t address, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* DRV_FLASH_H */
