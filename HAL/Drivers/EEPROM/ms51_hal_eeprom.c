/**
 *******************************************************************************
 * @file    ms51_hal_eeprom.c
 * @brief   MS51 HAL EEPROM Emulation Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_EEPROM_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/* EEPROM handle with default configuration */
static HAL_EEPROM_HandleTypeDef hEeprom = {
    .BaseAddress = HAL_EEPROM_BASE_ADDRESS,
    .Size = HAL_EEPROM_SIZE,
    .PageSize = HAL_EEPROM_PAGE_SIZE,
    .Initialized = 0
};

/* XRAM buffer for read-modify-write operations */
#if defined(__C51__)
static uint8_t xdata eeprom_page_buffer[HAL_EEPROM_PAGE_SIZE];
#elif defined(__ICC8051__)
static uint8_t __xdata eeprom_page_buffer[HAL_EEPROM_PAGE_SIZE];
#elif defined(__SDCC__)
static uint8_t __xdata eeprom_page_buffer[HAL_EEPROM_PAGE_SIZE];
#else
static uint8_t eeprom_page_buffer[HAL_EEPROM_PAGE_SIZE];
#endif

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief  Read a byte directly from flash
 */
static uint8_t EEPROM_ReadFlashByte(uint16_t addr)
{
#if defined(__C51__)
    return *((uint8_t HAL_CODE *)addr);
#elif defined(__ICC8051__)
    return *((uint8_t __code *)addr);
#elif defined(__SDCC__)
    return *((uint8_t __code *)addr);
#else
    return *((uint8_t *)addr);
#endif
}

/**
 * @brief  Write data to a single page (internal use)
 */
static HAL_StatusTypeDef EEPROM_WriteToPage(uint16_t pageAddr, uint8_t offset, 
                                             const uint8_t *pData, uint8_t num)
{
    uint8_t i;
    uint8_t verifyData;
    uint8_t needErase = 0;
    
    /* Read current page content into buffer */
    for (i = 0; i < HAL_EEPROM_PAGE_SIZE; i++) {
        eeprom_page_buffer[i] = EEPROM_ReadFlashByte(pageAddr + i);
    }
    
    /* Check if we can write without erase (only changing 1s to 0s) */
    for (i = 0; i < num; i++) {
        if ((eeprom_page_buffer[offset + i] & pData[i]) != pData[i]) {
            needErase = 1;
            break;
        }
    }
    
    /* Also check if data is already the same */
    for (i = 0; i < num; i++) {
        if (eeprom_page_buffer[offset + i] != pData[i]) {
            break;
        }
    }
    if (i == num) {
        return HAL_OK;  /* Data already matches, no write needed */
    }
    
    /* Update buffer with new data */
    for (i = 0; i < num; i++) {
        eeprom_page_buffer[offset + i] = pData[i];
    }
    
    /* Enable IAP */
    set_CHPCON_IAPEN;
    set_IAPUEN_APUEN;
    
    if (needErase) {
        /* Erase the page */
        IAPAL = (uint8_t)(pageAddr & 0xFF);
        IAPAH = (uint8_t)(pageAddr >> 8);
        IAPFD = 0xFF;
        IAPCN = HAL_IAP_PAGE_ERASE_APROM;
        set_IAPTRG_IAPGO;
    }
    
    /* Program all bytes */
    IAPAL = (uint8_t)(pageAddr & 0xFF);
    IAPAH = (uint8_t)(pageAddr >> 8);
    
    for (i = 0; i < HAL_EEPROM_PAGE_SIZE; i++) {
        /* Skip if byte is already 0xFF and we didn't erase */
        if (!needErase && eeprom_page_buffer[i] == EEPROM_ReadFlashByte(pageAddr + i)) {
            IAPAL++;
            continue;
        }
        
        /* Program byte */
        IAPCN = HAL_IAP_BYTE_PROGRAM_APROM;
        IAPFD = eeprom_page_buffer[i];
        set_IAPTRG_IAPGO;
        
        /* Verify byte */
        IAPCN = HAL_IAP_BYTE_READ_APROM;
        IAPFD = 0xFF;
        set_IAPTRG_IAPGO;
        verifyData = IAPFD;
        
        if (verifyData != eeprom_page_buffer[i]) {
            clr_IAPUEN_APUEN;
            clr_CHPCON_IAPEN;
            return HAL_ERROR;
        }
        
        IAPAL++;
    }
    
    clr_IAPUEN_APUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize EEPROM emulation with default settings
 */
HAL_StatusTypeDef HAL_EEPROM_Init(void)
{
    hEeprom.BaseAddress = HAL_EEPROM_BASE_ADDRESS;
    hEeprom.Size = HAL_EEPROM_SIZE;
    hEeprom.PageSize = HAL_EEPROM_PAGE_SIZE;
    hEeprom.Initialized = 1;
    
    return HAL_OK;
}

/**
 * @brief  Initialize EEPROM emulation with custom settings
 */
HAL_StatusTypeDef HAL_EEPROM_InitEx(HAL_EEPROM_HandleTypeDef *heeprom)
{
    if (heeprom == NULL) {
        return HAL_INVALID;
    }
    
    hEeprom.BaseAddress = heeprom->BaseAddress;
    hEeprom.Size = heeprom->Size;
    hEeprom.PageSize = HAL_EEPROM_PAGE_SIZE;  /* Fixed for MS51 */
    hEeprom.Initialized = 1;
    
    return HAL_OK;
}

/**
 * @brief  Read a single byte from EEPROM
 */
uint8_t HAL_EEPROM_ReadByte(uint16_t address)
{
    if (address >= hEeprom.Size) {
        return 0xFF;
    }
    
    return EEPROM_ReadFlashByte(hEeprom.BaseAddress + address);
}

/**
 * @brief  Read multiple bytes from EEPROM
 */
HAL_StatusTypeDef HAL_EEPROM_Read(uint16_t address, uint8_t *pData, uint16_t size)
{
    uint16_t i;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    if ((uint32_t)address + (uint32_t)size > hEeprom.Size) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        pData[i] = EEPROM_ReadFlashByte(hEeprom.BaseAddress + address + i);
    }
    
    return HAL_OK;
}

/**
 * @brief  Write a single byte to EEPROM
 */
HAL_StatusTypeDef HAL_EEPROM_WriteByte(uint16_t address, uint8_t data)
{
    uint16_t pageAddr;
    uint8_t offset;
    
    if (address >= hEeprom.Size) {
        return HAL_INVALID;
    }
    
    /* Calculate page address and offset */
    pageAddr = ((hEeprom.BaseAddress + address) / HAL_EEPROM_PAGE_SIZE) * HAL_EEPROM_PAGE_SIZE;
    offset = (hEeprom.BaseAddress + address) % HAL_EEPROM_PAGE_SIZE;
    
    return EEPROM_WriteToPage(pageAddr, offset, &data, 1);
}

/**
 * @brief  Write multiple bytes to EEPROM
 */
HAL_StatusTypeDef HAL_EEPROM_Write(uint16_t address, uint8_t *pData, uint16_t size)
{
    uint16_t currentPage, endPage;
    uint16_t absAddr;
    uint8_t offset, bytesToWrite;
    HAL_StatusTypeDef status;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    if ((uint32_t)address + (uint32_t)size > hEeprom.Size) {
        return HAL_INVALID;
    }
    
    absAddr = hEeprom.BaseAddress + address;
    currentPage = absAddr / HAL_EEPROM_PAGE_SIZE;
    endPage = (hEeprom.BaseAddress + address + size - 1) / HAL_EEPROM_PAGE_SIZE;
    
    while (currentPage <= endPage) {
        offset = (uint8_t)(absAddr % HAL_EEPROM_PAGE_SIZE);
        bytesToWrite = HAL_EEPROM_PAGE_SIZE - offset;
        
        if (bytesToWrite > size) {
            bytesToWrite = (uint8_t)size;
        }
        
        status = EEPROM_WriteToPage(currentPage * HAL_EEPROM_PAGE_SIZE, offset, pData, bytesToWrite);
        if (status != HAL_OK) {
            return status;
        }
        
        absAddr += bytesToWrite;
        pData += bytesToWrite;
        size -= bytesToWrite;
        currentPage++;
    }
    
    return HAL_OK;
}

/**
 * @brief  Erase entire EEPROM area
 */
HAL_StatusTypeDef HAL_EEPROM_Erase(void)
{
    uint16_t pageAddr;
    uint8_t numPages;
    uint8_t i;
    
    numPages = (uint8_t)(hEeprom.Size / HAL_EEPROM_PAGE_SIZE);
    
    set_CHPCON_IAPEN;
    set_IAPUEN_APUEN;
    
    for (i = 0; i < numPages; i++) {
        pageAddr = hEeprom.BaseAddress + (i * HAL_EEPROM_PAGE_SIZE);
        
        IAPAL = (uint8_t)(pageAddr & 0xFF);
        IAPAH = (uint8_t)(pageAddr >> 8);
        IAPFD = 0xFF;
        IAPCN = HAL_IAP_PAGE_ERASE_APROM;
        set_IAPTRG_IAPGO;
    }
    
    clr_IAPUEN_APUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

/**
 * @brief  Erase a single page
 */
HAL_StatusTypeDef HAL_EEPROM_ErasePage(uint8_t pageNum)
{
    uint16_t pageAddr;
    
    if (pageNum >= (hEeprom.Size / HAL_EEPROM_PAGE_SIZE)) {
        return HAL_INVALID;
    }
    
    pageAddr = hEeprom.BaseAddress + (pageNum * HAL_EEPROM_PAGE_SIZE);
    
    set_CHPCON_IAPEN;
    set_IAPUEN_APUEN;
    
    IAPAL = (uint8_t)(pageAddr & 0xFF);
    IAPAH = (uint8_t)(pageAddr >> 8);
    IAPFD = 0xFF;
    IAPCN = HAL_IAP_PAGE_ERASE_APROM;
    set_IAPTRG_IAPGO;
    
    clr_IAPUEN_APUEN;
    clr_CHPCON_IAPEN;
    
    return HAL_OK;
}

/**
 * @brief  Verify EEPROM content
 */
HAL_StatusTypeDef HAL_EEPROM_Verify(uint16_t address, uint8_t *pData, uint16_t size)
{
    uint16_t i;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    if ((uint32_t)address + (uint32_t)size > hEeprom.Size) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        if (EEPROM_ReadFlashByte(hEeprom.BaseAddress + address + i) != pData[i]) {
            return HAL_ERROR;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Write and verify data
 */
HAL_StatusTypeDef HAL_EEPROM_WriteVerify(uint16_t address, uint8_t *pData, uint16_t size)
{
    HAL_StatusTypeDef status;
    
    status = HAL_EEPROM_Write(address, pData, size);
    if (status != HAL_OK) {
        return status;
    }
    
    return HAL_EEPROM_Verify(address, pData, size);
}

/**
 * @brief  Update a single byte
 */
HAL_StatusTypeDef HAL_EEPROM_Update(uint16_t address, uint8_t data)
{
    uint8_t currentValue;
    
    currentValue = HAL_EEPROM_ReadByte(address);
    
    if (currentValue == data) {
        return HAL_OK;  /* No change needed */
    }
    
    return HAL_EEPROM_WriteByte(address, data);
}

/**
 * @brief  Update multiple bytes
 */
HAL_StatusTypeDef HAL_EEPROM_UpdateArray(uint16_t address, uint8_t *pData, uint16_t size)
{
    uint16_t i;
    uint8_t needWrite = 0;
    
    if (pData == NULL) {
        return HAL_INVALID;
    }
    
    /* Check if any byte is different */
    for (i = 0; i < size; i++) {
        if (HAL_EEPROM_ReadByte(address + i) != pData[i]) {
            needWrite = 1;
            break;
        }
    }
    
    if (!needWrite) {
        return HAL_OK;  /* No changes needed */
    }
    
    return HAL_EEPROM_Write(address, pData, size);
}

/**
 * @brief  Read 16-bit value
 */
uint16_t HAL_EEPROM_ReadU16(uint16_t address)
{
    uint16_t value;
    
    value = HAL_EEPROM_ReadByte(address);
    value |= ((uint16_t)HAL_EEPROM_ReadByte(address + 1) << 8);
    
    return value;
}

/**
 * @brief  Write 16-bit value
 */
HAL_StatusTypeDef HAL_EEPROM_WriteU16(uint16_t address, uint16_t data)
{
    uint8_t buffer[2];
    
    buffer[0] = (uint8_t)(data & 0xFF);
    buffer[1] = (uint8_t)(data >> 8);
    
    return HAL_EEPROM_Write(address, buffer, 2);
}

/**
 * @brief  Read 32-bit value
 */
uint32_t HAL_EEPROM_ReadU32(uint16_t address)
{
    uint32_t value;
    
    value = HAL_EEPROM_ReadByte(address);
    value |= ((uint32_t)HAL_EEPROM_ReadByte(address + 1) << 8);
    value |= ((uint32_t)HAL_EEPROM_ReadByte(address + 2) << 16);
    value |= ((uint32_t)HAL_EEPROM_ReadByte(address + 3) << 24);
    
    return value;
}

/**
 * @brief  Write 32-bit value
 */
HAL_StatusTypeDef HAL_EEPROM_WriteU32(uint16_t address, uint32_t data)
{
    uint8_t buffer[4];
    
    buffer[0] = (uint8_t)(data & 0xFF);
    buffer[1] = (uint8_t)((data >> 8) & 0xFF);
    buffer[2] = (uint8_t)((data >> 16) & 0xFF);
    buffer[3] = (uint8_t)((data >> 24) & 0xFF);
    
    return HAL_EEPROM_Write(address, buffer, 4);
}

/**
 * @brief  Get remaining free space
 */
uint16_t HAL_EEPROM_GetFreeSpace(void)
{
    return hEeprom.Size;
}

/**
 * @brief  Get EEPROM base address
 */
uint16_t HAL_EEPROM_GetBaseAddress(void)
{
    return hEeprom.BaseAddress;
}

/**
 * @brief  Check if address is valid
 */
uint8_t HAL_EEPROM_IsValidAddress(uint16_t address)
{
    return (address < hEeprom.Size) ? 1 : 0;
}

#endif /* HAL_EEPROM_MODULE_ENABLED */
