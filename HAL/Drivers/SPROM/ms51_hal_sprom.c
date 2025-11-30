/**
 *******************************************************************************
 * @file    ms51_hal_sprom.c
 * @brief   MS51 HAL SPROM Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_SPROM_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/* XRAM buffer for read-modify-write operations */
#if defined(__C51__)
static uint8_t xdata sprom_buffer[HAL_SPROM_SIZE];
#elif defined(__ICC8051__)
static uint8_t __xdata sprom_buffer[HAL_SPROM_SIZE];
#elif defined(__SDCC__)
static uint8_t __xdata sprom_buffer[HAL_SPROM_SIZE];
#else
static uint8_t sprom_buffer[HAL_SPROM_SIZE];
#endif

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Enable SPROM access
 */
void HAL_SPROM_Enable(void)
{
    set_CHPCON_IAPEN;   /* Enable IAP */
    set_IAPUEN_SPMEN;   /* Enable SPROM read access */
}

/**
 * @brief  Disable SPROM access
 */
void HAL_SPROM_Disable(void)
{
    clr_IAPUEN_SPMEN;   /* Disable SPROM read access */
    clr_IAPUEN_SPUEN;   /* Disable SPROM write access */
    clr_CHPCON_IAPEN;   /* Disable IAP */
}

/**
 * @brief  Read a single byte from SPROM
 */
uint8_t HAL_SPROM_ReadByte(uint8_t address)
{
    uint8_t data;
    
    if (address >= HAL_SPROM_SIZE) {
        return 0xFF;
    }
    
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    
    /* SPROM is mapped at 0xFF80, read using code memory access */
#if defined(__C51__)
    data = *((uint8_t HAL_CODE *)(HAL_SPROM_BASE + address));
#elif defined(__ICC8051__)
    data = *((uint8_t __code *)(HAL_SPROM_BASE + address));
#elif defined(__SDCC__)
    data = *((uint8_t __code *)(HAL_SPROM_BASE + address));
#else
    /* Use IAP method for other compilers */
    IAPAL = 0x80 + address;
    IAPAH = 0x01;
    IAPCN = HAL_IAP_CMD_BYTE_READ_SPROM;
    set_IAPTRG_IAPGO;
    data = IAPFD;
#endif
    
    return data;
}

/**
 * @brief  Read multiple bytes from SPROM
 */
HAL_StatusTypeDef HAL_SPROM_Read(uint8_t address, uint8_t *pData, uint8_t size)
{
    uint8_t i;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    if ((uint16_t)address + (uint16_t)size > HAL_SPROM_SIZE) {
        return HAL_INVALID;
    }
    
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    
    for (i = 0; i < size; i++) {
#if defined(__C51__)
        pData[i] = *((uint8_t HAL_CODE *)(HAL_SPROM_BASE + address + i));
#elif defined(__ICC8051__)
        pData[i] = *((uint8_t __code *)(HAL_SPROM_BASE + address + i));
#elif defined(__SDCC__)
        pData[i] = *((uint8_t __code *)(HAL_SPROM_BASE + address + i));
#else
        IAPAL = 0x80 + address + i;
        IAPAH = 0x01;
        IAPCN = HAL_IAP_CMD_BYTE_READ_SPROM;
        set_IAPTRG_IAPGO;
        pData[i] = IAPFD;
#endif
    }
    
    return HAL_OK;
}

/**
 * @brief  Write a single byte to SPROM
 */
HAL_StatusTypeDef HAL_SPROM_WriteByte(uint8_t address, uint8_t data)
{
    return HAL_SPROM_Write(address, &data, 1);
}

/**
 * @brief  Write multiple bytes to SPROM
 */
HAL_StatusTypeDef HAL_SPROM_Write(uint8_t address, uint8_t *pData, uint8_t size)
{
    uint8_t i;
    uint8_t verifyData;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    if ((uint16_t)address + (uint16_t)size > HAL_SPROM_SIZE) {
        return HAL_INVALID;
    }
    
    /* Enable IAP and SPROM access */
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    set_IAPUEN_SPUEN;
    
    /* Read entire SPROM into buffer */
    for (i = 0; i < HAL_SPROM_SIZE; i++) {
#if defined(__C51__)
        sprom_buffer[i] = *((uint8_t HAL_CODE *)(HAL_SPROM_BASE + i));
#elif defined(__ICC8051__)
        sprom_buffer[i] = *((uint8_t __code *)(HAL_SPROM_BASE + i));
#elif defined(__SDCC__)
        sprom_buffer[i] = *((uint8_t __code *)(HAL_SPROM_BASE + i));
#else
        sprom_buffer[i] = HAL_SPROM_ReadByte(i);
#endif
    }
    
    /* Modify data in buffer */
    for (i = 0; i < size; i++) {
        sprom_buffer[address + i] = pData[i];
    }
    
    /* Erase SPROM page */
    IAPCN = HAL_IAP_CMD_PAGE_ERASE_SPROM;
    IAPAL = 0x80;
    IAPAH = 0x01;
    IAPFD = 0xFF;
    set_IAPTRG_IAPGO;
    
    /* Program all bytes back */
    IAPAL = 0x80;
    for (i = 0; i < HAL_SPROM_SIZE; i++) {
        /* Program byte */
        IAPCN = HAL_IAP_CMD_BYTE_PROGRAM_SPROM;
        IAPFD = sprom_buffer[i];
        set_IAPTRG_IAPGO;
        
        /* Verify byte */
        IAPCN = HAL_IAP_CMD_BYTE_READ_SPROM;
        IAPFD = 0xFF;
        set_IAPTRG_IAPGO;
        verifyData = IAPFD;
        
        if (verifyData != sprom_buffer[i]) {
            clr_IAPUEN_SPUEN;
            clr_CHPCON_IAPEN;
            return HAL_ERROR;
        }
        
        IAPAL++;
    }
    
    clr_IAPUEN_SPUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

/**
 * @brief  Erase entire SPROM page
 */
HAL_StatusTypeDef HAL_SPROM_Erase(void)
{
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    set_IAPUEN_SPUEN;
    
    IAPCN = HAL_IAP_CMD_PAGE_ERASE_SPROM;
    IAPAL = 0x80;
    IAPAH = 0x01;
    IAPFD = 0xFF;
    set_IAPTRG_IAPGO;
    
    clr_IAPUEN_SPUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

/**
 * @brief  Verify SPROM content matches expected data
 */
HAL_StatusTypeDef HAL_SPROM_Verify(uint8_t address, uint8_t *pData, uint8_t size)
{
    uint8_t i;
    uint8_t readData;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    if ((uint16_t)address + (uint16_t)size > HAL_SPROM_SIZE) {
        return HAL_INVALID;
    }
    
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    
    for (i = 0; i < size; i++) {
        readData = HAL_SPROM_ReadByte(address + i);
        if (readData != pData[i]) {
            return HAL_ERROR;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Check if SPROM is locked
 */
uint8_t HAL_SPROM_IsLocked(void)
{
    uint8_t lockByte;
    
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    
    /* Read lock byte at address 0x7F (127th byte) */
#if defined(__C51__)
    lockByte = *((uint8_t HAL_CODE *)(HAL_SPROM_BASE + HAL_SPROM_LOCK_ADDR));
#elif defined(__ICC8051__)
    lockByte = *((uint8_t __code *)(HAL_SPROM_BASE + HAL_SPROM_LOCK_ADDR));
#elif defined(__SDCC__)
    lockByte = *((uint8_t __code *)(HAL_SPROM_BASE + HAL_SPROM_LOCK_ADDR));
#else
    IAPAL = 0xFF;
    IAPAH = 0x01;
    IAPCN = HAL_IAP_CMD_BYTE_READ_SPROM;
    set_IAPTRG_IAPGO;
    lockByte = IAPFD;
#endif
    
    /* If lock byte is 0xFF, SPROM is unlocked */
    return (lockByte != 0xFF) ? 1 : 0;
}

/**
 * @brief  Lock SPROM
 */
HAL_StatusTypeDef HAL_SPROM_Lock(void)
{
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    set_IAPUEN_SPUEN;
    
    /* Write any non-0xFF value to lock byte */
    IAPAL = 0xFF;  /* Address 0x7F in SPROM space (0x80 + 0x7F) */
    IAPAH = 0x01;
    IAPCN = HAL_IAP_CMD_BYTE_PROGRAM_SPROM;
    IAPFD = 0x00;  /* Write 0x00 to lock */
    set_IAPTRG_IAPGO;
    
    clr_IAPUEN_SPUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

/**
 * @brief  Write and verify data to SPROM
 */
HAL_StatusTypeDef HAL_SPROM_WriteVerify(uint8_t address, uint8_t *pData, uint8_t size)
{
    HAL_StatusTypeDef status;
    
    status = HAL_SPROM_Write(address, pData, size);
    if (status != HAL_OK) {
        return status;
    }
    
    return HAL_SPROM_Verify(address, pData, size);
}

/**
 * @brief  Fill SPROM with a pattern
 */
HAL_StatusTypeDef HAL_SPROM_Fill(uint8_t pattern)
{
    uint8_t i;
    uint8_t verifyData;
    
    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    set_IAPUEN_SPUEN;
    
    /* Erase SPROM */
    IAPCN = HAL_IAP_CMD_PAGE_ERASE_SPROM;
    IAPAL = 0x80;
    IAPAH = 0x01;
    IAPFD = 0xFF;
    set_IAPTRG_IAPGO;
    
    /* Program pattern to all bytes */
    IAPAL = 0x80;
    for (i = 0; i < HAL_SPROM_SIZE; i++) {
        IAPCN = HAL_IAP_CMD_BYTE_PROGRAM_SPROM;
        IAPFD = pattern;
        set_IAPTRG_IAPGO;
        
        /* Verify */
        IAPCN = HAL_IAP_CMD_BYTE_READ_SPROM;
        IAPFD = 0xFF;
        set_IAPTRG_IAPGO;
        verifyData = IAPFD;
        
        if (verifyData != pattern) {
            clr_IAPUEN_SPUEN;
            clr_CHPCON_IAPEN;
            return HAL_ERROR;
        }
        
        IAPAL++;
    }
    
    clr_IAPUEN_SPUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

#endif /* HAL_SPROM_MODULE_ENABLED */
