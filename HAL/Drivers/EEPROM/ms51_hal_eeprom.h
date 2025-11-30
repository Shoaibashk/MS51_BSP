/**
 *******************************************************************************
 * @file    ms51_hal_eeprom.h
 * @brief   MS51 HAL EEPROM Emulation Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details EEPROM Emulation using APROM Data Flash area.
 *          Provides EEPROM-like read/write operations with automatic
 *          page management and wear leveling considerations.
 *
 * Features:
 *   - Byte-level read/write operations
 *   - Array read/write operations
 *   - Automatic page erase and rewrite
 *   - Write verification
 *   - Configurable data flash area
 *
 * @note    Data flash area must be configured in CONFIG bytes
 * @note    Default data flash area starts at 0x3800 for 16K devices
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_EEPROM_H
#define __MS51_HAL_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * EEPROM Configuration
 ******************************************************************************/

/**
 * @brief  Default Data Flash base address
 * @note   For MS51_16K: 0x3800 (last 2KB of APROM)
 *         For MS51_8K:  0x1C00
 *         For MS51_4K:  0x0C00
 */
#ifndef HAL_EEPROM_BASE_ADDRESS
#define HAL_EEPROM_BASE_ADDRESS     0x3800U
#endif

/**
 * @brief  Default Data Flash size
 */
#ifndef HAL_EEPROM_SIZE
#define HAL_EEPROM_SIZE             2048U   /**< 2KB data flash area */
#endif

/**
 * @brief  Page size for EEPROM operations
 */
#define HAL_EEPROM_PAGE_SIZE        128U

/*******************************************************************************
 * IAP Commands for APROM/Data Flash
 ******************************************************************************/

#define HAL_IAP_BYTE_READ_APROM     0x00    /**< Read byte from APROM */
#define HAL_IAP_BYTE_PROGRAM_APROM  0x21    /**< Program byte to APROM */
#define HAL_IAP_PAGE_ERASE_APROM    0x22    /**< Erase APROM page */

/*******************************************************************************
 * EEPROM Handle Structure
 ******************************************************************************/

/**
 * @brief  EEPROM Handle Structure
 */
typedef struct {
    uint16_t        BaseAddress;    /**< Base address of data flash area */
    uint16_t        Size;           /**< Size of data flash area in bytes */
    uint8_t         PageSize;       /**< Page size (128 bytes for MS51) */
    uint8_t         Initialized;    /**< Initialization flag */
} HAL_EEPROM_HandleTypeDef;

/*******************************************************************************
 * EEPROM Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize EEPROM emulation with default settings
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Uses default base address (0x3800) and size (2KB)
 * 
 * @example
 *   HAL_EEPROM_Init();  // Initialize with defaults
 */
HAL_StatusTypeDef HAL_EEPROM_Init(void);

/**
 * @brief  Initialize EEPROM emulation with custom settings
 * @param  heeprom Pointer to EEPROM handle
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_EEPROM_HandleTypeDef heeprom;
 *   heeprom.BaseAddress = 0x3800;
 *   heeprom.Size = 1024;
 *   HAL_EEPROM_InitEx(&heeprom);
 */
HAL_StatusTypeDef HAL_EEPROM_InitEx(HAL_EEPROM_HandleTypeDef *heeprom);

/**
 * @brief  Read a single byte from EEPROM
 * @param  address Offset within EEPROM area (0 to Size-1)
 * @retval uint8_t Read byte value
 * 
 * @example
 *   uint8_t data = HAL_EEPROM_ReadByte(0x0010);
 */
uint8_t HAL_EEPROM_ReadByte(uint16_t address);

/**
 * @brief  Read multiple bytes from EEPROM
 * @param  address Start offset within EEPROM area
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to read
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   uint8_t buffer[64];
 *   HAL_EEPROM_Read(0x0000, buffer, 64);
 */
HAL_StatusTypeDef HAL_EEPROM_Read(uint16_t address, uint8_t *pData, uint16_t size);

/**
 * @brief  Write a single byte to EEPROM
 * @param  address Offset within EEPROM area
 * @param  data Byte to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   This performs read-modify-write on the containing page
 * @warning Flash endurance is typically 100,000 cycles per page
 * 
 * @example
 *   HAL_EEPROM_WriteByte(0x0010, 0xAB);
 */
HAL_StatusTypeDef HAL_EEPROM_WriteByte(uint16_t address, uint8_t data);

/**
 * @brief  Write multiple bytes to EEPROM
 * @param  address Start offset within EEPROM area
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Automatically handles page boundaries
 * 
 * @example
 *   uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
 *   HAL_EEPROM_Write(0x0000, data, 4);
 */
HAL_StatusTypeDef HAL_EEPROM_Write(uint16_t address, uint8_t *pData, uint16_t size);

/**
 * @brief  Erase entire EEPROM area
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @warning This erases all data in the EEPROM area!
 */
HAL_StatusTypeDef HAL_EEPROM_Erase(void);

/**
 * @brief  Erase a single page in EEPROM area
 * @param  pageNum Page number (0 to (Size/128)-1)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_EEPROM_ErasePage(uint8_t pageNum);

/**
 * @brief  Verify EEPROM content matches expected data
 * @param  address Start offset
 * @param  pData Pointer to expected data
 * @param  size Number of bytes to verify
 * @retval HAL_StatusTypeDef HAL_OK if verification passed
 */
HAL_StatusTypeDef HAL_EEPROM_Verify(uint16_t address, uint8_t *pData, uint16_t size);

/**
 * @brief  Write and verify data to EEPROM
 * @param  address Start offset
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to write
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_EEPROM_WriteVerify(uint16_t address, uint8_t *pData, uint16_t size);

/**
 * @brief  Update a single byte (only write if different)
 * @param  address Offset within EEPROM area
 * @param  data Byte to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Reduces flash wear by only writing if value is different
 */
HAL_StatusTypeDef HAL_EEPROM_Update(uint16_t address, uint8_t data);

/**
 * @brief  Update multiple bytes (only write pages that changed)
 * @param  address Start offset
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Reduces flash wear by comparing before writing
 */
HAL_StatusTypeDef HAL_EEPROM_UpdateArray(uint16_t address, uint8_t *pData, uint16_t size);

/**
 * @brief  Read 16-bit value from EEPROM (little-endian)
 * @param  address Offset within EEPROM area
 * @retval uint16_t Read value
 */
uint16_t HAL_EEPROM_ReadU16(uint16_t address);

/**
 * @brief  Write 16-bit value to EEPROM (little-endian)
 * @param  address Offset within EEPROM area
 * @param  data Value to write
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_EEPROM_WriteU16(uint16_t address, uint16_t data);

/**
 * @brief  Read 32-bit value from EEPROM (little-endian)
 * @param  address Offset within EEPROM area
 * @retval uint32_t Read value
 */
uint32_t HAL_EEPROM_ReadU32(uint16_t address);

/**
 * @brief  Write 32-bit value to EEPROM (little-endian)
 * @param  address Offset within EEPROM area
 * @param  data Value to write
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_EEPROM_WriteU32(uint16_t address, uint32_t data);

/**
 * @brief  Get remaining free space in EEPROM
 * @retval uint16_t Number of bytes available
 */
uint16_t HAL_EEPROM_GetFreeSpace(void);

/**
 * @brief  Get EEPROM base address
 * @retval uint16_t Base address in flash
 */
uint16_t HAL_EEPROM_GetBaseAddress(void);

/**
 * @brief  Check if address is within valid EEPROM range
 * @param  address Address to check
 * @retval uint8_t 1 if valid, 0 otherwise
 */
uint8_t HAL_EEPROM_IsValidAddress(uint16_t address);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_EEPROM_H */
