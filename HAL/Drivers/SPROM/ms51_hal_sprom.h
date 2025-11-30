/**
 *******************************************************************************
 * @file    ms51_hal_sprom.h
 * @brief   MS51 HAL SPROM Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details SPROM (Security Protected ROM) Hardware Abstraction Layer for MS51.
 *          The SPROM is a 127-byte protected memory region that can be used
 *          for storing sensitive data like encryption keys or license info.
 *
 * Features:
 *   - Read SPROM bytes
 *   - Write SPROM as EEPROM (with read-modify-write)
 *   - Page erase
 *   - Array read/write operations
 *   - Data verification
 *
 * @note    SPROM size is 127 bytes (0x00-0x7E), last byte is reserved for lock bit
 * @note    SPROM base address when mapped: 0xFF80
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_SPROM_H
#define __MS51_HAL_SPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * SPROM Constants
 ******************************************************************************/

#define HAL_SPROM_SIZE          127U        /**< SPROM size in bytes (excluding lock byte) */
#define HAL_SPROM_BASE          0xFF80U     /**< SPROM base address when mapped */
#define HAL_SPROM_LOCK_ADDR     0x7FU       /**< Lock byte address within SPROM */

/*******************************************************************************
 * IAP Commands for SPROM
 ******************************************************************************/

#define HAL_IAP_CMD_BYTE_READ_SPROM     0x00    /**< Read byte from SPROM */
#define HAL_IAP_CMD_BYTE_PROGRAM_SPROM  0x61    /**< Program byte to SPROM */
#define HAL_IAP_CMD_PAGE_ERASE_SPROM    0x62    /**< Erase SPROM page */

/*******************************************************************************
 * SPROM Function Prototypes
 ******************************************************************************/

/**
 * @brief  Enable SPROM access
 * @retval None
 * @note   Must be called before any SPROM operations
 */
void HAL_SPROM_Enable(void);

/**
 * @brief  Disable SPROM access
 * @retval None
 */
void HAL_SPROM_Disable(void);

/**
 * @brief  Read a single byte from SPROM
 * @param  address SPROM address (0x00-0x7E)
 * @retval uint8_t Read byte value
 * 
 * @example
 *   uint8_t data = HAL_SPROM_ReadByte(0x00);  // Read first byte
 */
uint8_t HAL_SPROM_ReadByte(uint8_t address);

/**
 * @brief  Read multiple bytes from SPROM
 * @param  address Start address (0x00-0x7E)
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to read (max 127)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   uint8_t buffer[16];
 *   HAL_SPROM_Read(0x00, buffer, 16);  // Read 16 bytes from start
 */
HAL_StatusTypeDef HAL_SPROM_Read(uint8_t address, uint8_t *pData, uint8_t size);

/**
 * @brief  Write a single byte to SPROM
 * @param  address SPROM address (0x00-0x7E)
 * @param  data Byte to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   This performs read-modify-write on entire SPROM page
 * @warning Writing is slow as it requires page erase and full rewrite
 * 
 * @example
 *   HAL_SPROM_WriteByte(0x00, 0xAB);  // Write 0xAB to address 0
 */
HAL_StatusTypeDef HAL_SPROM_WriteByte(uint8_t address, uint8_t data);

/**
 * @brief  Write multiple bytes to SPROM
 * @param  address Start address (0x00-0x7E)
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   This performs read-modify-write on entire SPROM page
 * 
 * @example
 *   uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
 *   HAL_SPROM_Write(0x00, data, 4);  // Write 4 bytes starting at address 0
 */
HAL_StatusTypeDef HAL_SPROM_Write(uint8_t address, uint8_t *pData, uint8_t size);

/**
 * @brief  Erase entire SPROM page
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @warning This erases all 127 bytes of SPROM data!
 */
HAL_StatusTypeDef HAL_SPROM_Erase(void);

/**
 * @brief  Verify SPROM content matches expected data
 * @param  address Start address
 * @param  pData Pointer to expected data
 * @param  size Number of bytes to verify
 * @retval HAL_StatusTypeDef HAL_OK if verification passed
 */
HAL_StatusTypeDef HAL_SPROM_Verify(uint8_t address, uint8_t *pData, uint8_t size);

/**
 * @brief  Check if SPROM is locked
 * @retval uint8_t 1 if locked, 0 if unlocked
 * 
 * @note   When SPROM is locked, it cannot be read or modified from outside
 */
uint8_t HAL_SPROM_IsLocked(void);

/**
 * @brief  Lock SPROM (prevent external access)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @warning Once locked, SPROM cannot be unlocked except by full chip erase!
 * @warning Use with extreme caution!
 */
HAL_StatusTypeDef HAL_SPROM_Lock(void);

/**
 * @brief  Write and verify data to SPROM
 * @param  address Start address
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to write
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   This function writes data and then verifies it was written correctly
 */
HAL_StatusTypeDef HAL_SPROM_WriteVerify(uint8_t address, uint8_t *pData, uint8_t size);

/**
 * @brief  Fill SPROM with a pattern
 * @param  pattern Pattern byte to fill
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Useful for initialization or clearing SPROM
 */
HAL_StatusTypeDef HAL_SPROM_Fill(uint8_t pattern);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_SPROM_H */
