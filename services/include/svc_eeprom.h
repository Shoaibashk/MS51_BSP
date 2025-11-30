/*===========================================================================
 * @file    svc_eeprom.h
 * @brief   EEPROM Emulation Service Interface
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Provides EEPROM-like persistent storage using Data Flash.
 *          Supports wear leveling and data integrity checks.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef SVC_EEPROM_H
#define SVC_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * EEPROM Configuration
 *---------------------------------------------------------------------------*/
/**
 * @brief Default Data Flash base address
 * @note  Adjust based on device variant and APROM size
 */
#ifndef EEPROM_BASE_ADDR
    #if defined(MS51_8K)
        #define EEPROM_BASE_ADDR    0x1800  /* 8KB device: starts at 6KB */
    #elif defined(MS51_16K)
        #define EEPROM_BASE_ADDR    0x3800  /* 16KB device: starts at 14KB */
    #elif defined(MS51_32K)
        #define EEPROM_BASE_ADDR    0x7800  /* 32KB device: starts at 30KB */
    #else
        #define EEPROM_BASE_ADDR    0x3800  /* Default to 16KB */
    #endif
#endif

#ifndef EEPROM_SIZE
    #define EEPROM_SIZE         2048    /* 2KB EEPROM emulation area */
#endif

#ifndef EEPROM_PAGE_SIZE
    #define EEPROM_PAGE_SIZE    128     /* Flash page size */
#endif

typedef struct {
    uint16_t base_address;      /**< Base address in Data Flash (alias: start_addr) */
    uint16_t size;              /**< Total EEPROM size in bytes */
    uint16_t page_size;         /**< Flash page size */
    uint8_t  wear_leveling;     /**< Enable wear leveling (0=off, 1=on) */
} eeprom_config_t;

/* Backwards compatibility - 'start_addr' is alias for 'base_address' */
#define start_addr base_address

/*---------------------------------------------------------------------------
 * EEPROM API
 *---------------------------------------------------------------------------*/
/**
 * @brief  Initialize EEPROM emulation service
 * @param  config  EEPROM configuration (NULL for defaults)
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t SVC_EEPROM_Init(const eeprom_config_t *config);

/**
 * @brief  De-initialize EEPROM service
 * @return DRV_OK on success
 */
drv_status_t SVC_EEPROM_DeInit(void);

/**
 * @brief  Read byte from EEPROM
 * @param  address  EEPROM address (0 to EEPROM_SIZE-1)
 * @param  data     Pointer to store read byte
 * @return DRV_OK on success, DRV_INVALID_PARAM if address out of range
 */
drv_status_t SVC_EEPROM_ReadByte(uint16_t address, uint8_t *data);

/**
 * @brief  Write byte to EEPROM
 * @param  address  EEPROM address (0 to EEPROM_SIZE-1)
 * @param  data     Byte to write
 * @return DRV_OK on success, DRV_INVALID_PARAM if address out of range
 * @note   Erases page if necessary, may take several ms
 */
drv_status_t SVC_EEPROM_WriteByte(uint16_t address, uint8_t data);

/**
 * @brief  Read multiple bytes from EEPROM
 * @param  address  Starting EEPROM address
 * @param  data     Buffer to store read data
 * @param  length   Number of bytes to read
 * @return DRV_OK on success, DRV_INVALID_PARAM if address+length out of range
 */
drv_status_t SVC_EEPROM_Read(uint16_t address, uint8_t *data, uint16_t length);

/**
 * @brief  Write multiple bytes to EEPROM
 * @param  address  Starting EEPROM address
 * @param  data     Data to write
 * @param  length   Number of bytes to write
 * @return DRV_OK on success, DRV_INVALID_PARAM if address+length out of range
 */
drv_status_t SVC_EEPROM_Write(uint16_t address, const uint8_t *data, uint16_t length);

/**
 * @brief  Erase all EEPROM data (set to 0xFF)
 * @return DRV_OK on success
 */
drv_status_t SVC_EEPROM_EraseAll(void);

/**
 * @brief  Erase single EEPROM page
 * @param  page_num  Page number (0 to EEPROM_SIZE/PAGE_SIZE-1)
 * @return DRV_OK on success
 */
drv_status_t SVC_EEPROM_ErasePage(uint16_t page_num);

/*---------------------------------------------------------------------------
 * Structured Data Support
 *---------------------------------------------------------------------------*/
/**
 * @brief  Read structure from EEPROM with CRC check
 * @param  address  EEPROM address
 * @param  data     Pointer to structure
 * @param  size     Size of structure
 * @return DRV_OK on success, DRV_ERROR if CRC mismatch
 */
drv_status_t SVC_EEPROM_ReadStruct(uint16_t address, void *data, uint16_t size);

/**
 * @brief  Write structure to EEPROM with CRC
 * @param  address  EEPROM address  
 * @param  data     Pointer to structure
 * @param  size     Size of structure
 * @return DRV_OK on success
 */
drv_status_t SVC_EEPROM_WriteStruct(uint16_t address, const void *data, uint16_t size);

/*---------------------------------------------------------------------------
 * Status and Diagnostics
 *---------------------------------------------------------------------------*/
/**
 * @brief  Get EEPROM write count (for wear monitoring)
 * @return Total write cycles since last erase
 */
uint32_t SVC_EEPROM_GetWriteCount(void);

/**
 * @brief  Check EEPROM data integrity
 * @return DRV_OK if data valid, DRV_ERROR if corruption detected
 */
drv_status_t SVC_EEPROM_VerifyIntegrity(void);

#ifdef __cplusplus
}
#endif

#endif /* SVC_EEPROM_H */
