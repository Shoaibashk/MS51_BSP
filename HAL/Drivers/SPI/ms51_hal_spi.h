/**
 *******************************************************************************
 * @file    ms51_hal_spi.h
 * @brief   MS51 HAL SPI Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details SPI Hardware Abstraction Layer for MS51 series.
 *          Supports Master and Slave modes.
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_SPI_H
#define __MS51_HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * SPI Mode Definitions
 ******************************************************************************/

/**
 * @brief  SPI Mode (Master/Slave)
 */
typedef enum {
    HAL_SPI_MODE_SLAVE = 0,     /**< SPI Slave mode */
    HAL_SPI_MODE_MASTER = 1     /**< SPI Master mode */
} HAL_SPI_ModeTypeDef;

/**
 * @brief  SPI Clock Polarity
 */
typedef enum {
    HAL_SPI_CPOL_LOW = 0,       /**< Clock idle low */
    HAL_SPI_CPOL_HIGH = 1       /**< Clock idle high */
} HAL_SPI_CPOLTypeDef;

/**
 * @brief  SPI Clock Phase
 */
typedef enum {
    HAL_SPI_CPHA_1EDGE = 0,     /**< Data sampled on first clock edge */
    HAL_SPI_CPHA_2EDGE = 1      /**< Data sampled on second clock edge */
} HAL_SPI_CPHATypeDef;

/**
 * @brief  SPI Data Order
 */
typedef enum {
    HAL_SPI_MSB_FIRST = 0,      /**< MSB transmitted first */
    HAL_SPI_LSB_FIRST = 1       /**< LSB transmitted first */
} HAL_SPI_DataOrderTypeDef;

/**
 * @brief  SPI Clock Divider
 */
typedef enum {
    HAL_SPI_DIV_2   = 0,        /**< Fsys / 2 */
    HAL_SPI_DIV_4   = 1,        /**< Fsys / 4 */
    HAL_SPI_DIV_8   = 2,        /**< Fsys / 8 */
    HAL_SPI_DIV_16  = 3         /**< Fsys / 16 */
} HAL_SPI_ClkDivTypeDef;

/*******************************************************************************
 * SPI Initialization Structure
 ******************************************************************************/

/**
 * @brief  SPI Initialization Structure
 */
typedef struct {
    HAL_SPI_ModeTypeDef         Mode;           /**< SPI mode (Master/Slave) */
    HAL_SPI_CPOLTypeDef         CPOL;           /**< Clock polarity */
    HAL_SPI_CPHATypeDef         CPHA;           /**< Clock phase */
    HAL_SPI_DataOrderTypeDef    DataOrder;      /**< Data order (MSB/LSB first) */
    HAL_SPI_ClkDivTypeDef       ClockDiv;       /**< Clock divider */
    uint8_t                     SSOutput;       /**< Enable SS output (master mode) */
} HAL_SPI_InitTypeDef;

/**
 * @brief  SPI Handle Structure
 */
typedef struct {
    HAL_SPI_InitTypeDef         Init;           /**< SPI configuration */
    volatile uint8_t            *pTxBuffer;     /**< TX buffer pointer */
    volatile uint8_t            *pRxBuffer;     /**< RX buffer pointer */
    volatile uint16_t           TxCount;        /**< TX counter */
    volatile uint16_t           RxCount;        /**< RX counter */
    volatile uint16_t           TxSize;         /**< TX buffer size */
    volatile uint16_t           RxSize;         /**< RX buffer size */
    volatile uint8_t            TxComplete;     /**< TX complete flag */
    volatile uint8_t            RxComplete;     /**< RX complete flag */
    HAL_LockTypeDef             Lock;           /**< Lock object */
} HAL_SPI_HandleTypeDef;

/*******************************************************************************
 * SPI Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize SPI peripheral
 * @param  hspi Pointer to SPI handle structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_SPI_HandleTypeDef hspi;
 *   hspi.Init.Mode = HAL_SPI_MODE_MASTER;
 *   hspi.Init.CPOL = HAL_SPI_CPOL_LOW;
 *   hspi.Init.CPHA = HAL_SPI_CPHA_1EDGE;
 *   hspi.Init.DataOrder = HAL_SPI_MSB_FIRST;
 *   hspi.Init.ClockDiv = HAL_SPI_DIV_8;
 *   HAL_SPI_Init(&hspi);
 */
HAL_StatusTypeDef HAL_SPI_Init(HAL_SPI_HandleTypeDef *hspi);

/**
 * @brief  De-initialize SPI peripheral
 * @param  hspi Pointer to SPI handle structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_SPI_DeInit(HAL_SPI_HandleTypeDef *hspi);

/**
 * @brief  Simple SPI Master initialization
 * @param  clockDiv Clock divider
 * @param  cpol Clock polarity
 * @param  cpha Clock phase
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_SPI_MasterInit(HAL_SPI_ClkDivTypeDef clockDiv,
                                      HAL_SPI_CPOLTypeDef cpol,
                                      HAL_SPI_CPHATypeDef cpha);

/**
 * @brief  Enable SPI peripheral
 * @retval None
 */
void HAL_SPI_Enable(void);

/**
 * @brief  Disable SPI peripheral
 * @retval None
 */
void HAL_SPI_Disable(void);

/**
 * @brief  Transmit and receive single byte (blocking)
 * @param  hspi Pointer to SPI handle structure
 * @param  txData Byte to transmit
 * @retval uint8_t Received byte
 * 
 * @example
 *   uint8_t rxData = HAL_SPI_TransmitReceiveByte(&hspi, 0xAA);
 */
uint8_t HAL_SPI_TransmitReceiveByte(HAL_SPI_HandleTypeDef *hspi, uint8_t txData);

/**
 * @brief  Transmit data buffer (blocking)
 * @param  hspi Pointer to SPI handle structure
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to transmit
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_SPI_Transmit(HAL_SPI_HandleTypeDef *hspi, uint8_t *pData,
                                    uint16_t size, uint32_t timeout);

/**
 * @brief  Receive data buffer (blocking)
 * @param  hspi Pointer to SPI handle structure
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to receive
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_SPI_Receive(HAL_SPI_HandleTypeDef *hspi, uint8_t *pData,
                                   uint16_t size, uint32_t timeout);

/**
 * @brief  Transmit and receive data (blocking)
 * @param  hspi Pointer to SPI handle structure
 * @param  pTxData Pointer to TX data buffer
 * @param  pRxData Pointer to RX data buffer
 * @param  size Number of bytes
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_SPI_TransmitReceive(HAL_SPI_HandleTypeDef *hspi,
                                           uint8_t *pTxData, uint8_t *pRxData,
                                           uint16_t size, uint32_t timeout);

/**
 * @brief  Check if SPI transfer is complete
 * @retval uint8_t 1 if complete, 0 otherwise
 */
uint8_t HAL_SPI_IsTransferComplete(void);

/**
 * @brief  Wait for SPI transfer complete
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_SPI_WaitComplete(uint32_t timeout);

/**
 * @brief  Clear SPI interrupt flag
 * @retval None
 */
void HAL_SPI_ClearFlag(void);

/**
 * @brief  Enable SPI interrupt
 * @retval None
 */
void HAL_SPI_EnableInterrupt(void);

/**
 * @brief  Disable SPI interrupt
 * @retval None
 */
void HAL_SPI_DisableInterrupt(void);

/**
 * @brief  Assert SS pin (drive low for chip select)
 * @retval None
 */
void HAL_SPI_AssertSS(void);

/**
 * @brief  Deassert SS pin (drive high)
 * @retval None
 */
void HAL_SPI_DeassertSS(void);

/*******************************************************************************
 * SPI Callback Functions
 ******************************************************************************/

/**
 * @brief  SPI TX complete callback
 */
void HAL_SPI_TxCpltCallback(HAL_SPI_HandleTypeDef *hspi);

/**
 * @brief  SPI RX complete callback
 */
void HAL_SPI_RxCpltCallback(HAL_SPI_HandleTypeDef *hspi);

/**
 * @brief  SPI TX/RX complete callback
 */
void HAL_SPI_TxRxCpltCallback(HAL_SPI_HandleTypeDef *hspi);

/**
 * @brief  SPI Error callback
 */
void HAL_SPI_ErrorCallback(HAL_SPI_HandleTypeDef *hspi);

/*******************************************************************************
 * SPI Interrupt-Driven Functions
 ******************************************************************************/

/**
 * @brief  Transmit data using interrupt mode
 * @param  hspi Pointer to SPI handle structure
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to transmit
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Non-blocking function. HAL_SPI_TxCpltCallback is called on completion.
 */
HAL_StatusTypeDef HAL_SPI_Transmit_IT(HAL_SPI_HandleTypeDef *hspi, uint8_t *pData,
                                       uint16_t size);

/**
 * @brief  Receive data using interrupt mode
 * @param  hspi Pointer to SPI handle structure
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to receive
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Non-blocking function. HAL_SPI_RxCpltCallback is called on completion.
 */
HAL_StatusTypeDef HAL_SPI_Receive_IT(HAL_SPI_HandleTypeDef *hspi, uint8_t *pData,
                                      uint16_t size);

/**
 * @brief  Transmit and receive data using interrupt mode
 * @param  hspi Pointer to SPI handle structure
 * @param  pTxData Pointer to TX data buffer
 * @param  pRxData Pointer to RX data buffer
 * @param  size Number of bytes
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   Non-blocking function. HAL_SPI_TxRxCpltCallback is called on completion.
 */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(HAL_SPI_HandleTypeDef *hspi,
                                              uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t size);

/**
 * @brief  SPI interrupt handler
 * @param  hspi Pointer to SPI handle structure
 * @note   Call this from SPI ISR
 */
void HAL_SPI_IRQHandler(HAL_SPI_HandleTypeDef *hspi);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_SPI_H */
