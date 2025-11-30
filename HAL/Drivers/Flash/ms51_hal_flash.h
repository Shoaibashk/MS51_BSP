/**
 *******************************************************************************
 * @file    ms51_hal_flash.h
 * @brief   MS51 HAL Flash/IAP Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details Flash/IAP Hardware Abstraction Layer for MS51 series.
 *          Supports APROM, LDROM, and Data Flash operations.
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_FLASH_H
#define __MS51_HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * Flash Memory Area Definitions
 ******************************************************************************/

/**
 * @brief  Flash Memory Area enumeration
 */
typedef enum {
    HAL_FLASH_AREA_APROM = 0,       /**< Application ROM */
    HAL_FLASH_AREA_LDROM = 1,       /**< Loader ROM */
    HAL_FLASH_AREA_CONFIG = 2,      /**< Configuration bytes */
    HAL_FLASH_AREA_UID = 3          /**< Unique ID area */
} HAL_Flash_AreaTypeDef;

/*******************************************************************************
 * Flash Constants
 ******************************************************************************/

#define HAL_FLASH_PAGE_SIZE     128U    /**< Flash page size in bytes */

/* Flash base addresses */
#define HAL_APROM_BASE          0x0000U
#define HAL_LDROM_BASE          0x0000U /* LDROM starts at 0 when LDROM is active */
#define HAL_CONFIG_BASE         0x0000U /* CONFIG bytes address space */

/* Memory sizes (16K variant) */
#define HAL_APROM_SIZE          0x4000U /* 16KB */
#define HAL_LDROM_SIZE          0x0800U /* 2KB */

/*******************************************************************************
 * IAP Command Definitions
 ******************************************************************************/

/**
 * @brief  IAP Command enumeration
 */
typedef enum {
    HAL_IAP_CMD_IDLE         = 0x00,    /**< Idle */
    HAL_IAP_CMD_BYTE_READ    = 0x00,    /**< Read byte from flash */
    HAL_IAP_CMD_BYTE_PROGRAM = 0x21,    /**< Program byte to flash */
    HAL_IAP_CMD_PAGE_ERASE   = 0x22,    /**< Erase page */
    HAL_IAP_CMD_READ_UID     = 0x04,    /**< Read unique ID */
    HAL_IAP_CMD_READ_CONFIG  = 0x03,    /**< Read configuration bytes */
    HAL_IAP_CMD_WRITE_CONFIG = 0x27,    /**< Write configuration bytes */
    HAL_IAP_CMD_READ_CID     = 0x0B     /**< Read company ID */
} HAL_IAP_CmdTypeDef;

/*******************************************************************************
 * Flash Function Prototypes
 ******************************************************************************/

/**
 * @brief  Enable IAP (In-Application Programming)
 * @retval None
 */
void HAL_Flash_EnableIAP(void);

/**
 * @brief  Disable IAP
 * @retval None
 */
void HAL_Flash_DisableIAP(void);

/**
 * @brief  Erase a flash page
 * @param  area Flash memory area
 * @param  address Page address (must be page-aligned)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_Flash_ErasePage(HAL_FLASH_AREA_APROM, 0x3F80);  // Erase last page
 */
HAL_StatusTypeDef HAL_Flash_ErasePage(HAL_Flash_AreaTypeDef area, uint16_t address);

/**
 * @brief  Read byte from flash
 * @param  area Flash memory area
 * @param  address Address to read from
 * @retval uint8_t Read byte value
 */
uint8_t HAL_Flash_ReadByte(HAL_Flash_AreaTypeDef area, uint16_t address);

/**
 * @brief  Write byte to flash
 * @param  area Flash memory area
 * @param  address Address to write to
 * @param  data Byte to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   The page must be erased before writing
 * 
 * @example
 *   HAL_Flash_WriteByte(HAL_FLASH_AREA_APROM, 0x3F80, 0xAB);
 */
HAL_StatusTypeDef HAL_Flash_WriteByte(HAL_Flash_AreaTypeDef area, 
                                       uint16_t address, uint8_t data);

/**
 * @brief  Read multiple bytes from flash
 * @param  area Flash memory area
 * @param  address Start address
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to read
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Flash_Read(HAL_Flash_AreaTypeDef area, uint16_t address,
                                  uint8_t *pData, uint16_t size);

/**
 * @brief  Write multiple bytes to flash
 * @param  area Flash memory area
 * @param  address Start address
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   The pages must be erased before writing
 */
HAL_StatusTypeDef HAL_Flash_Write(HAL_Flash_AreaTypeDef area, uint16_t address,
                                   uint8_t *pData, uint16_t size);

/**
 * @brief  Read unique device ID
 * @param  uid Pointer to buffer (9 bytes)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Flash_ReadUID(uint8_t *uid);

/**
 * @brief  Read configuration bytes
 * @param  config Pointer to buffer (5 bytes)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Flash_ReadConfig(uint8_t *config);

/**
 * @brief  Write configuration bytes
 * @param  config Pointer to config data (5 bytes)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @warning Incorrect configuration can brick the device!
 */
HAL_StatusTypeDef HAL_Flash_WriteConfig(uint8_t *config);

/**
 * @brief  Read Company ID
 * @retval uint8_t Company ID byte
 */
uint8_t HAL_Flash_ReadCID(void);

/**
 * @brief  Read Device ID
 * @retval uint8_t Device ID byte
 */
uint8_t HAL_Flash_ReadDID(void);

/**
 * @brief  Read bandgap calibration value
 * @retval uint16_t Bandgap calibration value (12-bit)
 */
uint16_t HAL_Flash_ReadBandgapCalibration(void);

/**
 * @brief  Read HIRC trim value (16MHz)
 * @param  trim Pointer to store trim bytes (2 bytes)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Flash_ReadHIRCTrim16M(uint8_t *trim);

/**
 * @brief  Read HIRC trim value (24MHz)
 * @param  trim Pointer to store trim bytes (2 bytes)
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_Flash_ReadHIRCTrim24M(uint8_t *trim);

/**
 * @brief  Verify flash content
 * @param  area Flash memory area
 * @param  address Start address
 * @param  pData Pointer to expected data
 * @param  size Number of bytes to verify
 * @retval HAL_StatusTypeDef HAL_OK if verification passed
 */
HAL_StatusTypeDef HAL_Flash_Verify(HAL_Flash_AreaTypeDef area, uint16_t address,
                                    uint8_t *pData, uint16_t size);

/**
 * @brief  Get flash page number from address
 * @param  address Flash address
 * @retval uint8_t Page number
 */
uint8_t HAL_Flash_GetPage(uint16_t address);

/**
 * @brief  Check if address is page-aligned
 * @param  address Address to check
 * @retval uint8_t 1 if aligned, 0 otherwise
 */
uint8_t HAL_Flash_IsPageAligned(uint16_t address);

/*******************************************************************************
 * Data Flash Convenience Functions
 ******************************************************************************/

/**
 * @brief  Initialize data flash area
 * @param  startPage Start page for data flash
 * @param  pageCount Number of pages
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_DataFlash_Init(uint8_t startPage, uint8_t pageCount);

/**
 * @brief  Erase data flash
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_DataFlash_Erase(void);

/**
 * @brief  Write to data flash
 * @param  offset Offset within data flash area
 * @param  pData Pointer to data
 * @param  size Number of bytes
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_DataFlash_Write(uint16_t offset, uint8_t *pData, 
                                       uint16_t size);

/**
 * @brief  Read from data flash
 * @param  offset Offset within data flash area
 * @param  pData Pointer to buffer
 * @param  size Number of bytes
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_DataFlash_Read(uint16_t offset, uint8_t *pData, 
                                      uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_FLASH_H */
