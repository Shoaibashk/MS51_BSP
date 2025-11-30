/**
 *******************************************************************************
 * @file    ms51_hal_i2c.h
 * @brief   MS51 HAL I2C Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details I2C Hardware Abstraction Layer for MS51 series.
 *          Supports Master and Slave modes.
 *
 * Features:
 *   - Master transmit/receive
 *   - Slave transmit/receive
 *   - Standard mode (100kHz) and Fast mode (400kHz)
 *   - 7-bit and 10-bit addressing
 *   - Interrupt and polling modes
 *   - Timeout support
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_I2C_H
#define __MS51_HAL_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * I2C Mode Definitions
 ******************************************************************************/

/**
 * @brief  I2C Mode enumeration
 */
typedef enum {
    HAL_I2C_MODE_MASTER = 0,    /**< I2C Master mode */
    HAL_I2C_MODE_SLAVE = 1      /**< I2C Slave mode */
} HAL_I2C_ModeTypeDef;

/*******************************************************************************
 * I2C Speed Definitions
 ******************************************************************************/

/**
 * @brief  I2C Speed enumeration
 */
typedef enum {
    HAL_I2C_SPEED_STANDARD = 100000UL,  /**< Standard mode (100kHz) */
    HAL_I2C_SPEED_FAST = 400000UL       /**< Fast mode (400kHz) */
} HAL_I2C_SpeedTypeDef;

/*******************************************************************************
 * I2C Status Codes
 ******************************************************************************/

/**
 * @brief  I2C Master Status Codes
 */
typedef enum {
    HAL_I2C_STAT_BUS_ERROR       = 0x00,    /**< Bus error */
    HAL_I2C_STAT_START           = 0x08,    /**< START transmitted */
    HAL_I2C_STAT_RSTART          = 0x10,    /**< Repeated START transmitted */
    HAL_I2C_STAT_TX_ADDR_ACK     = 0x18,    /**< Slave address + W, ACK received */
    HAL_I2C_STAT_TX_ADDR_NACK    = 0x20,    /**< Slave address + W, NACK received */
    HAL_I2C_STAT_TX_DATA_ACK     = 0x28,    /**< Data byte sent, ACK received */
    HAL_I2C_STAT_TX_DATA_NACK    = 0x30,    /**< Data byte sent, NACK received */
    HAL_I2C_STAT_ARB_LOST        = 0x38,    /**< Arbitration lost */
    HAL_I2C_STAT_RX_ADDR_ACK     = 0x40,    /**< Slave address + R, ACK received */
    HAL_I2C_STAT_RX_ADDR_NACK    = 0x48,    /**< Slave address + R, NACK received */
    HAL_I2C_STAT_RX_DATA_ACK     = 0x50,    /**< Data received, ACK returned */
    HAL_I2C_STAT_RX_DATA_NACK    = 0x58     /**< Data received, NACK returned */
} HAL_I2C_StatusCodeTypeDef;

/*******************************************************************************
 * I2C Initialization Structure
 ******************************************************************************/

/**
 * @brief  I2C Initialization Structure
 */
typedef struct {
    HAL_I2C_ModeTypeDef     Mode;           /**< I2C mode (Master/Slave) */
    uint32_t                ClockSpeed;     /**< I2C clock speed in Hz */
    uint8_t                 SlaveAddr;      /**< Own address (for slave mode) */
    uint8_t                 GeneralCall;    /**< General call enable */
} HAL_I2C_InitTypeDef;

/**
 * @brief  I2C Handle Structure
 */
typedef struct {
    HAL_I2C_InitTypeDef     Init;           /**< I2C configuration */
    volatile uint8_t        *pBuffer;       /**< Data buffer pointer */
    volatile uint16_t       BufferSize;     /**< Buffer size */
    volatile uint16_t       BufferIndex;    /**< Current buffer index */
    volatile uint8_t        SlaveAddr;      /**< Target slave address */
    volatile uint8_t        State;          /**< Current state */
    HAL_LockTypeDef         Lock;           /**< Lock object */
} HAL_I2C_HandleTypeDef;

/*******************************************************************************
 * I2C Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize I2C peripheral
 * @param  hi2c Pointer to I2C handle structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_I2C_HandleTypeDef hi2c;
 *   hi2c.Init.Mode = HAL_I2C_MODE_MASTER;
 *   hi2c.Init.ClockSpeed = HAL_I2C_SPEED_STANDARD;
 *   HAL_I2C_Init(&hi2c);
 */
HAL_StatusTypeDef HAL_I2C_Init(HAL_I2C_HandleTypeDef *hi2c);

/**
 * @brief  De-initialize I2C peripheral
 * @param  hi2c Pointer to I2C handle structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_I2C_DeInit(HAL_I2C_HandleTypeDef *hi2c);

/**
 * @brief  Simple I2C Master initialization
 * @param  clockSpeed I2C clock speed in Hz
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_I2C_MasterInit(uint32_t clockSpeed);

/**
 * @brief  Master transmit data to slave
 * @param  hi2c Pointer to I2C handle structure
 * @param  slaveAddr Slave address (7-bit, left-aligned)
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to transmit
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   uint8_t data[] = {0x01, 0x02, 0x03};
 *   HAL_I2C_MasterTransmit(&hi2c, 0x50, data, 3, 1000);
 */
HAL_StatusTypeDef HAL_I2C_MasterTransmit(HAL_I2C_HandleTypeDef *hi2c, 
                                          uint8_t slaveAddr,
                                          uint8_t *pData, uint16_t size, 
                                          uint32_t timeout);

/**
 * @brief  Master receive data from slave
 * @param  hi2c Pointer to I2C handle structure
 * @param  slaveAddr Slave address (7-bit, left-aligned)
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to receive
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_I2C_MasterReceive(HAL_I2C_HandleTypeDef *hi2c,
                                         uint8_t slaveAddr,
                                         uint8_t *pData, uint16_t size,
                                         uint32_t timeout);

/**
 * @brief  Write to memory/register of slave device
 * @param  hi2c Pointer to I2C handle structure
 * @param  slaveAddr Slave address
 * @param  memAddr Memory/register address
 * @param  memAddrSize Size of memory address (1 or 2 bytes)
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to write
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   // Write to EEPROM at address 0x0010
 *   uint8_t data = 0xAB;
 *   HAL_I2C_Mem_Write(&hi2c, 0xA0, 0x0010, 2, &data, 1, 1000);
 */
HAL_StatusTypeDef HAL_I2C_Mem_Write(HAL_I2C_HandleTypeDef *hi2c,
                                     uint8_t slaveAddr,
                                     uint16_t memAddr, uint8_t memAddrSize,
                                     uint8_t *pData, uint16_t size,
                                     uint32_t timeout);

/**
 * @brief  Read from memory/register of slave device
 * @param  hi2c Pointer to I2C handle structure
 * @param  slaveAddr Slave address
 * @param  memAddr Memory/register address
 * @param  memAddrSize Size of memory address (1 or 2 bytes)
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to read
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_I2C_Mem_Read(HAL_I2C_HandleTypeDef *hi2c,
                                    uint8_t slaveAddr,
                                    uint16_t memAddr, uint8_t memAddrSize,
                                    uint8_t *pData, uint16_t size,
                                    uint32_t timeout);

/**
 * @brief  Check if slave device is ready
 * @param  hi2c Pointer to I2C handle structure
 * @param  slaveAddr Slave address
 * @param  retries Number of retry attempts
 * @param  timeout Timeout per retry in milliseconds
 * @retval HAL_StatusTypeDef HAL_OK if device responds
 */
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(HAL_I2C_HandleTypeDef *hi2c,
                                         uint8_t slaveAddr,
                                         uint8_t retries, uint32_t timeout);

/**
 * @brief  Send START condition
 * @retval None
 */
void HAL_I2C_Start(void);

/**
 * @brief  Send STOP condition
 * @retval None
 */
void HAL_I2C_Stop(void);

/**
 * @brief  Wait for I2C interrupt (SI flag)
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_I2C_WaitFlag(uint32_t timeout);

/**
 * @brief  Get I2C status code
 * @retval uint8_t Status code from I2STAT register
 */
uint8_t HAL_I2C_GetStatus(void);

/**
 * @brief  Send byte via I2C
 * @param  data Byte to send
 * @retval None
 */
void HAL_I2C_WriteByte(uint8_t data);

/**
 * @brief  Read byte from I2C
 * @retval uint8_t Received byte
 */
uint8_t HAL_I2C_ReadByte(void);

/**
 * @brief  Send ACK after receiving
 * @retval None
 */
void HAL_I2C_SendACK(void);

/**
 * @brief  Send NACK after receiving
 * @retval None
 */
void HAL_I2C_SendNACK(void);

/**
 * @brief  Enable I2C interrupt
 * @retval None
 */
void HAL_I2C_EnableInterrupt(void);

/**
 * @brief  Disable I2C interrupt
 * @retval None
 */
void HAL_I2C_DisableInterrupt(void);

/**
 * @brief  Clear I2C interrupt flag (SI)
 * @retval None
 */
void HAL_I2C_ClearFlag(void);

/*******************************************************************************
 * I2C Callback Functions
 ******************************************************************************/

/**
 * @brief  I2C Master TX complete callback
 */
void HAL_I2C_MasterTxCpltCallback(HAL_I2C_HandleTypeDef *hi2c);

/**
 * @brief  I2C Master RX complete callback
 */
void HAL_I2C_MasterRxCpltCallback(HAL_I2C_HandleTypeDef *hi2c);

/**
 * @brief  I2C Error callback
 */
void HAL_I2C_ErrorCallback(HAL_I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_I2C_H */
