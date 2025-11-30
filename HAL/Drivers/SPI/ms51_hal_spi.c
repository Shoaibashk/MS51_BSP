/**
 *******************************************************************************
 * @file    ms51_hal_spi.c
 * @brief   MS51 HAL SPI Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_SPI_MODULE_ENABLED

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize SPI peripheral
 */
HAL_StatusTypeDef HAL_SPI_Init(HAL_SPI_HandleTypeDef *hspi)
{
    uint8_t spcr = 0;
    
    if (hspi == NULL) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    
    /* Configure SPI pins */
    /* MOSI - P0.0, MISO - P0.1, SPICLK - P1.0, SS - P1.5 (default) */
    P0M1 &= ~0x03;  /* P0.0, P0.1 quasi-bidir */
    P0M2 &= ~0x03;
    P1M1 &= ~0x21;  /* P1.0, P1.5 */
    P1M2 &= ~0x21;
    
    if (hspi->Init.Mode == HAL_SPI_MODE_MASTER) {
        spcr |= 0x10;  /* MSTR = 1 */
        /* Configure MOSI and CLK as push-pull for master */
        P0M2 |= 0x01;  /* MOSI push-pull */
        P1M2 |= 0x01;  /* CLK push-pull */
    }
    
    if (hspi->Init.CPOL == HAL_SPI_CPOL_HIGH) {
        spcr |= 0x08;  /* CPOL = 1 */
    }
    
    if (hspi->Init.CPHA == HAL_SPI_CPHA_2EDGE) {
        spcr |= 0x04;  /* CPHA = 1 */
    }
    
    if (hspi->Init.DataOrder == HAL_SPI_LSB_FIRST) {
        spcr |= 0x20;  /* LSBFE = 1 */
    }
    
    /* Set clock divider */
    spcr |= (hspi->Init.ClockDiv & 0x03);
    
    /* Enable SS output in master mode if requested */
    if (hspi->Init.Mode == HAL_SPI_MODE_MASTER && hspi->Init.SSOutput) {
        spcr |= 0x80;  /* SSOE = 1 */
    }
    
    SPCR = spcr;
    
    hspi->Lock = HAL_UNLOCKED;
    hspi->TxComplete = 1;
    hspi->RxComplete = 1;
    
    return HAL_OK;
}

/**
 * @brief  De-initialize SPI peripheral
 */
HAL_StatusTypeDef HAL_SPI_DeInit(HAL_SPI_HandleTypeDef *hspi)
{
    HAL_SPI_Disable();
    HAL_SPI_DisableInterrupt();
    
    return HAL_OK;
}

/**
 * @brief  Simple SPI Master initialization
 */
HAL_StatusTypeDef HAL_SPI_MasterInit(HAL_SPI_ClkDivTypeDef clockDiv,
                                      HAL_SPI_CPOLTypeDef cpol,
                                      HAL_SPI_CPHATypeDef cpha)
{
    uint8_t spcr = 0x10;  /* MSTR = 1 */
    
    HAL_SFR_PAGE0();
    
    /* Configure pins */
    P0M1 &= ~0x03;
    P0M2 |= 0x01;   /* MOSI push-pull */
    P1M1 &= ~0x21;
    P1M2 |= 0x01;   /* CLK push-pull */
    
    if (cpol == HAL_SPI_CPOL_HIGH) {
        spcr |= 0x08;
    }
    
    if (cpha == HAL_SPI_CPHA_2EDGE) {
        spcr |= 0x04;
    }
    
    spcr |= (clockDiv & 0x03);
    
    SPCR = spcr;
    
    return HAL_OK;
}

/**
 * @brief  Enable SPI peripheral
 */
void HAL_SPI_Enable(void)
{
    HAL_SFR_PAGE0();
    SPCR |= 0x40;  /* SPIEN = 1 */
}

/**
 * @brief  Disable SPI peripheral
 */
void HAL_SPI_Disable(void)
{
    HAL_SFR_PAGE0();
    SPCR &= ~0x40;  /* SPIEN = 0 */
}

/**
 * @brief  Transmit and receive single byte
 */
uint8_t HAL_SPI_TransmitReceiveByte(HAL_SPI_HandleTypeDef *hspi, uint8_t txData)
{
    HAL_SFR_PAGE0();
    
    /* Clear flag */
    SPSR &= ~0x80;
    
    /* Write data to transmit */
    SPDR = txData;
    
    /* Wait for transfer complete */
    while (!(SPSR & 0x80));
    
    /* Return received data */
    return SPDR;
}

/**
 * @brief  Transmit data buffer
 */
HAL_StatusTypeDef HAL_SPI_Transmit(HAL_SPI_HandleTypeDef *hspi, uint8_t *pData,
                                    uint16_t size, uint32_t timeout)
{
    uint16_t i;
    uint32_t tickstart;
    
    if (hspi == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    tickstart = HAL_GetTick();
    
    for (i = 0; i < size; i++) {
        SPSR &= ~0x80;  /* Clear flag */
        SPDR = pData[i];
        
        /* Wait for transfer complete with timeout */
        while (!(SPSR & 0x80)) {
            if ((HAL_GetTick() - tickstart) > timeout) {
                return HAL_TIMEOUT;
            }
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Receive data buffer
 */
HAL_StatusTypeDef HAL_SPI_Receive(HAL_SPI_HandleTypeDef *hspi, uint8_t *pData,
                                   uint16_t size, uint32_t timeout)
{
    uint16_t i;
    uint32_t tickstart;
    
    if (hspi == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    tickstart = HAL_GetTick();
    
    for (i = 0; i < size; i++) {
        SPSR &= ~0x80;
        SPDR = 0xFF;  /* Send dummy byte */
        
        while (!(SPSR & 0x80)) {
            if ((HAL_GetTick() - tickstart) > timeout) {
                return HAL_TIMEOUT;
            }
        }
        
        pData[i] = SPDR;
    }
    
    return HAL_OK;
}

/**
 * @brief  Transmit and receive data
 */
HAL_StatusTypeDef HAL_SPI_TransmitReceive(HAL_SPI_HandleTypeDef *hspi,
                                           uint8_t *pTxData, uint8_t *pRxData,
                                           uint16_t size, uint32_t timeout)
{
    uint16_t i;
    uint32_t tickstart;
    
    if (hspi == NULL || pTxData == NULL || pRxData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    HAL_SFR_PAGE0();
    tickstart = HAL_GetTick();
    
    for (i = 0; i < size; i++) {
        SPSR &= ~0x80;
        SPDR = pTxData[i];
        
        while (!(SPSR & 0x80)) {
            if ((HAL_GetTick() - tickstart) > timeout) {
                return HAL_TIMEOUT;
            }
        }
        
        pRxData[i] = SPDR;
    }
    
    return HAL_OK;
}

/**
 * @brief  Check if SPI transfer is complete
 */
uint8_t HAL_SPI_IsTransferComplete(void)
{
    return (SPSR & 0x80) ? 1 : 0;
}

/**
 * @brief  Wait for SPI transfer complete
 */
HAL_StatusTypeDef HAL_SPI_WaitComplete(uint32_t timeout)
{
    uint32_t tickstart = HAL_GetTick();
    
    while (!(SPSR & 0x80)) {
        if ((HAL_GetTick() - tickstart) > timeout) {
            return HAL_TIMEOUT;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Clear SPI interrupt flag
 */
void HAL_SPI_ClearFlag(void)
{
    SPSR &= ~0x80;
}

/**
 * @brief  Enable SPI interrupt
 */
void HAL_SPI_EnableInterrupt(void)
{
    EIE |= 0x40;  /* ESPI = 1 */
}

/**
 * @brief  Disable SPI interrupt
 */
void HAL_SPI_DisableInterrupt(void)
{
    EIE &= ~0x40;  /* ESPI = 0 */
}

/**
 * @brief  Assert SS pin (drive low)
 */
void HAL_SPI_AssertSS(void)
{
    P1 &= ~0x20;  /* P1.5 = 0 */
}

/**
 * @brief  Deassert SS pin (drive high)
 */
void HAL_SPI_DeassertSS(void)
{
    P1 |= 0x20;   /* P1.5 = 1 */
}

/*******************************************************************************
 * Weak Callback Functions
 ******************************************************************************/

__attribute__((weak)) void HAL_SPI_TxCpltCallback(HAL_SPI_HandleTypeDef *hspi)
{
    HAL_UNUSED(hspi);
}

__attribute__((weak)) void HAL_SPI_RxCpltCallback(HAL_SPI_HandleTypeDef *hspi)
{
    HAL_UNUSED(hspi);
}

__attribute__((weak)) void HAL_SPI_TxRxCpltCallback(HAL_SPI_HandleTypeDef *hspi)
{
    HAL_UNUSED(hspi);
}

__attribute__((weak)) void HAL_SPI_ErrorCallback(HAL_SPI_HandleTypeDef *hspi)
{
    HAL_UNUSED(hspi);
}

#endif /* HAL_SPI_MODULE_ENABLED */
