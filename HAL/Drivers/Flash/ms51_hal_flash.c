/**
 *******************************************************************************
 * @file    ms51_hal_flash.c
 * @brief   MS51 HAL Flash/IAP Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_FLASH_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint16_t g_dataFlashBase = 0;
static uint16_t g_dataFlashSize = 0;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief  Trigger IAP operation
 */
static void IAP_Trigger(void)
{
    HAL_TA_SET_BIT(IAPTRG, 0x01);
}

/**
 * @brief  Wait for IAP operation to complete
 */
static void IAP_WaitComplete(void)
{
    /* IAP operation is typically fast, no explicit wait needed */
    HAL_NOP();
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Enable IAP (In-Application Programming)
 */
void HAL_Flash_EnableIAP(void)
{
    HAL_TA_SET_BIT(CHPCON, 0x01);  /* IAPEN = 1 */
}

/**
 * @brief  Disable IAP
 */
void HAL_Flash_DisableIAP(void)
{
    HAL_TA_CLEAR_BIT(CHPCON, 0x01);  /* IAPEN = 0 */
}

/**
 * @brief  Erase a flash page
 */
HAL_StatusTypeDef HAL_Flash_ErasePage(HAL_Flash_AreaTypeDef area, uint16_t address)
{
    uint8_t intState;
    
    /* Align to page boundary */
    address &= ~(HAL_FLASH_PAGE_SIZE - 1);
    
    /* Save and disable interrupts */
    intState = EA;
    EA = 0;
    
    /* Enable IAP */
    HAL_Flash_EnableIAP();
    
    /* Enable update */
    HAL_TA_SET_BIT(IAPUEN, 0x01);  /* APUEN = 1 for APROM */
    
    /* Set address */
    IAPAL = (uint8_t)(address & 0xFF);
    IAPAH = (uint8_t)((address >> 8) & 0xFF);
    
    /* Set page erase command */
    IAPCN = HAL_IAP_CMD_PAGE_ERASE;
    IAPFD = 0xFF;  /* Dummy data */
    
    /* Trigger IAP */
    IAP_Trigger();
    IAP_WaitComplete();
    
    /* Disable update */
    HAL_TA_CLEAR_BIT(IAPUEN, 0x01);
    
    /* Disable IAP */
    HAL_Flash_DisableIAP();
    
    /* Restore interrupts */
    EA = intState;
    
    return HAL_OK;
}

/**
 * @brief  Read byte from flash
 */
uint8_t HAL_Flash_ReadByte(HAL_Flash_AreaTypeDef area, uint16_t address)
{
    uint8_t data;
    uint8_t intState;
    
    /* Save and disable interrupts */
    intState = EA;
    EA = 0;
    
    /* Enable IAP */
    HAL_Flash_EnableIAP();
    
    /* Set address */
    IAPAL = (uint8_t)(address & 0xFF);
    IAPAH = (uint8_t)((address >> 8) & 0xFF);
    
    /* Set read command based on area */
    switch (area) {
        case HAL_FLASH_AREA_APROM:
        case HAL_FLASH_AREA_LDROM:
            IAPCN = HAL_IAP_CMD_BYTE_READ;
            break;
        case HAL_FLASH_AREA_CONFIG:
            IAPCN = HAL_IAP_CMD_READ_CONFIG;
            break;
        case HAL_FLASH_AREA_UID:
            IAPCN = HAL_IAP_CMD_READ_UID;
            break;
        default:
            IAPCN = HAL_IAP_CMD_BYTE_READ;
            break;
    }
    
    /* Trigger IAP */
    IAP_Trigger();
    IAP_WaitComplete();
    
    data = IAPFD;
    
    /* Disable IAP */
    HAL_Flash_DisableIAP();
    
    /* Restore interrupts */
    EA = intState;
    
    return data;
}

/**
 * @brief  Write byte to flash
 */
HAL_StatusTypeDef HAL_Flash_WriteByte(HAL_Flash_AreaTypeDef area, 
                                       uint16_t address, uint8_t data)
{
    uint8_t intState;
    
    /* Save and disable interrupts */
    intState = EA;
    EA = 0;
    
    /* Enable IAP */
    HAL_Flash_EnableIAP();
    
    /* Enable update */
    HAL_TA_SET_BIT(IAPUEN, 0x01);  /* APUEN = 1 */
    
    /* Set address */
    IAPAL = (uint8_t)(address & 0xFF);
    IAPAH = (uint8_t)((address >> 8) & 0xFF);
    
    /* Set program command */
    IAPCN = HAL_IAP_CMD_BYTE_PROGRAM;
    IAPFD = data;
    
    /* Trigger IAP */
    IAP_Trigger();
    IAP_WaitComplete();
    
    /* Disable update */
    HAL_TA_CLEAR_BIT(IAPUEN, 0x01);
    
    /* Disable IAP */
    HAL_Flash_DisableIAP();
    
    /* Restore interrupts */
    EA = intState;
    
    return HAL_OK;
}

/**
 * @brief  Read multiple bytes from flash
 */
HAL_StatusTypeDef HAL_Flash_Read(HAL_Flash_AreaTypeDef area, uint16_t address,
                                  uint8_t *pData, uint16_t size)
{
    uint16_t i;
    
    if (pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        pData[i] = HAL_Flash_ReadByte(area, address + i);
    }
    
    return HAL_OK;
}

/**
 * @brief  Write multiple bytes to flash
 */
HAL_StatusTypeDef HAL_Flash_Write(HAL_Flash_AreaTypeDef area, uint16_t address,
                                   uint8_t *pData, uint16_t size)
{
    uint16_t i;
    
    if (pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        HAL_Flash_WriteByte(area, address + i, pData[i]);
    }
    
    return HAL_OK;
}

/**
 * @brief  Read unique device ID
 */
HAL_StatusTypeDef HAL_Flash_ReadUID(uint8_t *uid)
{
    uint8_t i;
    
    if (uid == NULL) {
        return HAL_INVALID;
    }
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_UID;
    IAPAH = 0x00;
    
    for (i = 0; i < 9; i++) {
        IAPAL = i;
        IAP_Trigger();
        uid[i] = IAPFD;
    }
    
    HAL_Flash_DisableIAP();
    
    return HAL_OK;
}

/**
 * @brief  Read configuration bytes
 */
HAL_StatusTypeDef HAL_Flash_ReadConfig(uint8_t *config)
{
    uint8_t i;
    
    if (config == NULL) {
        return HAL_INVALID;
    }
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_CONFIG;
    IAPAH = 0x00;
    
    for (i = 0; i < 5; i++) {
        IAPAL = i;
        IAP_Trigger();
        config[i] = IAPFD;
    }
    
    HAL_Flash_DisableIAP();
    
    return HAL_OK;
}

/**
 * @brief  Write configuration bytes
 */
HAL_StatusTypeDef HAL_Flash_WriteConfig(uint8_t *config)
{
    uint8_t i;
    uint8_t intState;
    
    if (config == NULL) {
        return HAL_INVALID;
    }
    
    /* Disable interrupts */
    intState = EA;
    EA = 0;
    
    HAL_Flash_EnableIAP();
    
    /* Enable config update */
    HAL_TA_SET_BIT(IAPUEN, 0x04);  /* CFUEN = 1 */
    
    /* Erase config area first */
    IAPCN = HAL_IAP_CMD_PAGE_ERASE;
    IAPAL = 0x00;
    IAPAH = 0x00;
    IAPFD = 0xFF;
    IAP_Trigger();
    
    /* Write config bytes */
    IAPCN = HAL_IAP_CMD_BYTE_PROGRAM;
    IAPAH = 0x00;
    
    for (i = 0; i < 5; i++) {
        IAPAL = i;
        IAPFD = config[i];
        IAP_Trigger();
    }
    
    /* Disable config update */
    HAL_TA_CLEAR_BIT(IAPUEN, 0x04);
    
    HAL_Flash_DisableIAP();
    
    /* Restore interrupts */
    EA = intState;
    
    return HAL_OK;
}

/**
 * @brief  Read Company ID
 */
uint8_t HAL_Flash_ReadCID(void)
{
    uint8_t cid;
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_CID;
    IAPAL = 0x00;
    IAPAH = 0x00;
    IAP_Trigger();
    cid = IAPFD;
    
    HAL_Flash_DisableIAP();
    
    return cid;
}

/**
 * @brief  Read Device ID
 */
uint8_t HAL_Flash_ReadDID(void)
{
    uint8_t did;
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_CID;
    IAPAL = 0x01;
    IAPAH = 0x00;
    IAP_Trigger();
    did = IAPFD;
    
    HAL_Flash_DisableIAP();
    
    return did;
}

/**
 * @brief  Read bandgap calibration value
 */
uint16_t HAL_Flash_ReadBandgapCalibration(void)
{
    uint16_t bg;
    uint8_t bgH, bgL;
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_UID;
    IAPAH = 0x00;
    
    IAPAL = 0x0C;
    IAP_Trigger();
    bgH = IAPFD;
    
    IAPAL = 0x0D;
    IAP_Trigger();
    bgL = IAPFD & 0x0F;
    
    HAL_Flash_DisableIAP();
    
    bg = ((uint16_t)bgH << 4) | bgL;
    
    return bg;
}

/**
 * @brief  Read HIRC trim value (16MHz)
 */
HAL_StatusTypeDef HAL_Flash_ReadHIRCTrim16M(uint8_t *trim)
{
    if (trim == NULL) {
        return HAL_INVALID;
    }
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_UID;
    IAPAH = 0x00;
    
    IAPAL = 0x30;
    IAP_Trigger();
    trim[0] = IAPFD;
    
    IAPAL = 0x31;
    IAP_Trigger();
    trim[1] = IAPFD;
    
    HAL_Flash_DisableIAP();
    
    return HAL_OK;
}

/**
 * @brief  Read HIRC trim value (24MHz)
 */
HAL_StatusTypeDef HAL_Flash_ReadHIRCTrim24M(uint8_t *trim)
{
    if (trim == NULL) {
        return HAL_INVALID;
    }
    
    HAL_Flash_EnableIAP();
    
    IAPCN = HAL_IAP_CMD_READ_UID;
    IAPAH = 0x00;
    
    IAPAL = 0x38;
    IAP_Trigger();
    trim[0] = IAPFD;
    
    IAPAL = 0x39;
    IAP_Trigger();
    trim[1] = IAPFD;
    
    HAL_Flash_DisableIAP();
    
    return HAL_OK;
}

/**
 * @brief  Verify flash content
 */
HAL_StatusTypeDef HAL_Flash_Verify(HAL_Flash_AreaTypeDef area, uint16_t address,
                                    uint8_t *pData, uint16_t size)
{
    uint16_t i;
    uint8_t readByte;
    
    if (pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        readByte = HAL_Flash_ReadByte(area, address + i);
        if (readByte != pData[i]) {
            return HAL_ERROR;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Get flash page number from address
 */
uint8_t HAL_Flash_GetPage(uint16_t address)
{
    return (uint8_t)(address / HAL_FLASH_PAGE_SIZE);
}

/**
 * @brief  Check if address is page-aligned
 */
uint8_t HAL_Flash_IsPageAligned(uint16_t address)
{
    return (address & (HAL_FLASH_PAGE_SIZE - 1)) == 0 ? 1 : 0;
}

/*******************************************************************************
 * Data Flash Functions
 ******************************************************************************/

/**
 * @brief  Initialize data flash area
 */
HAL_StatusTypeDef HAL_DataFlash_Init(uint8_t startPage, uint8_t pageCount)
{
    if (pageCount == 0) {
        return HAL_INVALID;
    }
    
    g_dataFlashBase = (uint16_t)startPage * HAL_FLASH_PAGE_SIZE;
    g_dataFlashSize = (uint16_t)pageCount * HAL_FLASH_PAGE_SIZE;
    
    return HAL_OK;
}

/**
 * @brief  Erase data flash
 */
HAL_StatusTypeDef HAL_DataFlash_Erase(void)
{
    uint16_t addr;
    uint16_t endAddr;
    
    if (g_dataFlashSize == 0) {
        return HAL_ERROR;
    }
    
    endAddr = g_dataFlashBase + g_dataFlashSize;
    for (addr = g_dataFlashBase; addr < endAddr; addr += HAL_FLASH_PAGE_SIZE) {
        HAL_Flash_ErasePage(HAL_FLASH_AREA_APROM, addr);
    }
    
    return HAL_OK;
}

/**
 * @brief  Write to data flash
 */
HAL_StatusTypeDef HAL_DataFlash_Write(uint16_t offset, uint8_t *pData, 
                                       uint16_t size)
{
    if (g_dataFlashSize == 0 || (offset + size) > g_dataFlashSize) {
        return HAL_INVALID;
    }
    
    return HAL_Flash_Write(HAL_FLASH_AREA_APROM, g_dataFlashBase + offset, 
                          pData, size);
}

/**
 * @brief  Read from data flash
 */
HAL_StatusTypeDef HAL_DataFlash_Read(uint16_t offset, uint8_t *pData, 
                                      uint16_t size)
{
    if (g_dataFlashSize == 0 || (offset + size) > g_dataFlashSize) {
        return HAL_INVALID;
    }
    
    return HAL_Flash_Read(HAL_FLASH_AREA_APROM, g_dataFlashBase + offset, 
                         pData, size);
}

#endif /* HAL_FLASH_MODULE_ENABLED */
