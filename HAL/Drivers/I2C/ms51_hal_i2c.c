/**
 *******************************************************************************
 * @file    ms51_hal_i2c.c
 * @brief   MS51 HAL I2C Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_I2C_MODULE_ENABLED

/*******************************************************************************
 * Private Defines
 ******************************************************************************/
#define I2C_WRITE   0
#define I2C_READ    1

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize I2C peripheral
 */
HAL_StatusTypeDef HAL_I2C_Init(HAL_I2C_HandleTypeDef *hi2c)
{
    uint8_t i2cclk;
    
    if (hi2c == NULL) {
        return HAL_INVALID;
    }
    
    /* Configure I2C pins (P1.3=SCL, P1.4=SDA) as open-drain */
    P1M1 |= 0x18;   /* P1.3, P1.4 */
    P1M2 |= 0x18;
    
    /* Calculate clock divider: I2CLK = (Fsys / (4 * I2C_freq)) - 1 */
    i2cclk = (uint8_t)((HAL_SYSCLK_FREQ / (4 * hi2c->Init.ClockSpeed)) - 1);
    I2CLK = i2cclk;
    
    /* Configure slave address if slave mode */
    if (hi2c->Init.Mode == HAL_I2C_MODE_SLAVE) {
        I2ADDR = (hi2c->Init.SlaveAddr << 1);
        if (hi2c->Init.GeneralCall) {
            I2ADDR |= 0x01;  /* Enable general call */
        }
    }
    
    /* Enable I2C */
    I2CEN = 1;
    
    hi2c->Lock = HAL_UNLOCKED;
    hi2c->State = 0;
    
    return HAL_OK;
}

/**
 * @brief  De-initialize I2C peripheral
 */
HAL_StatusTypeDef HAL_I2C_DeInit(HAL_I2C_HandleTypeDef *hi2c)
{
    I2CEN = 0;
    HAL_I2C_DisableInterrupt();
    
    return HAL_OK;
}

/**
 * @brief  Simple I2C Master initialization
 */
HAL_StatusTypeDef HAL_I2C_MasterInit(uint32_t clockSpeed)
{
    uint8_t i2cclk;
    
    /* Configure I2C pins as open-drain */
    P1M1 |= 0x18;
    P1M2 |= 0x18;
    
    /* Calculate and set clock divider */
    i2cclk = (uint8_t)((HAL_SYSCLK_FREQ / (4 * clockSpeed)) - 1);
    I2CLK = i2cclk;
    
    /* Enable I2C */
    I2CEN = 1;
    
    return HAL_OK;
}

/**
 * @brief  Send START condition
 */
void HAL_I2C_Start(void)
{
    STA = 1;    /* Set START condition */
    SI = 0;     /* Clear interrupt flag */
}

/**
 * @brief  Send STOP condition
 */
void HAL_I2C_Stop(void)
{
    STO = 1;    /* Set STOP condition */
    SI = 0;     /* Clear interrupt flag */
}

/**
 * @brief  Wait for I2C interrupt
 */
HAL_StatusTypeDef HAL_I2C_WaitFlag(uint32_t timeout)
{
    uint32_t tickstart = HAL_GetTick();
    
    while (!SI) {
        if ((HAL_GetTick() - tickstart) > timeout) {
            return HAL_TIMEOUT;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Get I2C status code
 */
uint8_t HAL_I2C_GetStatus(void)
{
    return I2STAT;
}

/**
 * @brief  Send byte via I2C
 */
void HAL_I2C_WriteByte(uint8_t data)
{
    I2DAT = data;
    SI = 0;
}

/**
 * @brief  Read byte from I2C
 */
uint8_t HAL_I2C_ReadByte(void)
{
    return I2DAT;
}

/**
 * @brief  Send ACK after receiving
 */
void HAL_I2C_SendACK(void)
{
    AA = 1;
    SI = 0;
}

/**
 * @brief  Send NACK after receiving
 */
void HAL_I2C_SendNACK(void)
{
    AA = 0;
    SI = 0;
}

/**
 * @brief  Clear I2C interrupt flag
 */
void HAL_I2C_ClearFlag(void)
{
    SI = 0;
}

/**
 * @brief  Enable I2C interrupt
 */
void HAL_I2C_EnableInterrupt(void)
{
    EIE |= 0x01;  /* EI2C = 1 */
}

/**
 * @brief  Disable I2C interrupt
 */
void HAL_I2C_DisableInterrupt(void)
{
    EIE &= ~0x01;  /* EI2C = 0 */
}

/**
 * @brief  Master transmit data to slave
 */
HAL_StatusTypeDef HAL_I2C_MasterTransmit(HAL_I2C_HandleTypeDef *hi2c,
                                          uint8_t slaveAddr,
                                          uint8_t *pData, uint16_t size,
                                          uint32_t timeout)
{
    uint16_t i;
    uint8_t status;
    
    if (hi2c == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    /* Send START */
    HAL_I2C_Start();
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x08 && status != 0x10) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Send slave address + Write */
    HAL_I2C_WriteByte((slaveAddr << 1) | I2C_WRITE);
    STA = 0;  /* Clear START */
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x18) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Send data bytes */
    for (i = 0; i < size; i++) {
        HAL_I2C_WriteByte(pData[i]);
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
        
        status = HAL_I2C_GetStatus();
        if (status != 0x28) {
            HAL_I2C_Stop();
            return HAL_ERROR;
        }
    }
    
    /* Send STOP */
    HAL_I2C_Stop();
    
    return HAL_OK;
}

/**
 * @brief  Master receive data from slave
 */
HAL_StatusTypeDef HAL_I2C_MasterReceive(HAL_I2C_HandleTypeDef *hi2c,
                                         uint8_t slaveAddr,
                                         uint8_t *pData, uint16_t size,
                                         uint32_t timeout)
{
    uint16_t i;
    uint8_t status;
    
    if (hi2c == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    /* Send START */
    HAL_I2C_Start();
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x08 && status != 0x10) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Send slave address + Read */
    HAL_I2C_WriteByte((slaveAddr << 1) | I2C_READ);
    STA = 0;
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x40) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Receive data bytes */
    for (i = 0; i < size; i++) {
        if (i < (size - 1)) {
            HAL_I2C_SendACK();  /* ACK for more bytes */
        } else {
            HAL_I2C_SendNACK(); /* NACK for last byte */
        }
        
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
        
        pData[i] = HAL_I2C_ReadByte();
    }
    
    /* Send STOP */
    HAL_I2C_Stop();
    
    return HAL_OK;
}

/**
 * @brief  Write to memory/register of slave device
 */
HAL_StatusTypeDef HAL_I2C_Mem_Write(HAL_I2C_HandleTypeDef *hi2c,
                                     uint8_t slaveAddr,
                                     uint16_t memAddr, uint8_t memAddrSize,
                                     uint8_t *pData, uint16_t size,
                                     uint32_t timeout)
{
    uint16_t i;
    uint8_t status;
    
    if (hi2c == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    /* Send START */
    HAL_I2C_Start();
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    /* Send slave address + Write */
    HAL_I2C_WriteByte((slaveAddr << 1) | I2C_WRITE);
    STA = 0;
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x18) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Send memory address */
    if (memAddrSize == 2) {
        HAL_I2C_WriteByte((uint8_t)(memAddr >> 8));
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
    }
    
    HAL_I2C_WriteByte((uint8_t)(memAddr & 0xFF));
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    /* Send data */
    for (i = 0; i < size; i++) {
        HAL_I2C_WriteByte(pData[i]);
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
    }
    
    /* Send STOP */
    HAL_I2C_Stop();
    
    return HAL_OK;
}

/**
 * @brief  Read from memory/register of slave device
 */
HAL_StatusTypeDef HAL_I2C_Mem_Read(HAL_I2C_HandleTypeDef *hi2c,
                                    uint8_t slaveAddr,
                                    uint16_t memAddr, uint8_t memAddrSize,
                                    uint8_t *pData, uint16_t size,
                                    uint32_t timeout)
{
    uint16_t i;
    uint8_t status;
    
    if (hi2c == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    /* Send START */
    HAL_I2C_Start();
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    /* Send slave address + Write (to write memory address) */
    HAL_I2C_WriteByte((slaveAddr << 1) | I2C_WRITE);
    STA = 0;
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x18) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Send memory address */
    if (memAddrSize == 2) {
        HAL_I2C_WriteByte((uint8_t)(memAddr >> 8));
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
    }
    
    HAL_I2C_WriteByte((uint8_t)(memAddr & 0xFF));
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    /* Send repeated START */
    HAL_I2C_Start();
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    /* Send slave address + Read */
    HAL_I2C_WriteByte((slaveAddr << 1) | I2C_READ);
    STA = 0;
    if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
        return HAL_TIMEOUT;
    }
    
    status = HAL_I2C_GetStatus();
    if (status != 0x40) {
        HAL_I2C_Stop();
        return HAL_ERROR;
    }
    
    /* Receive data */
    for (i = 0; i < size; i++) {
        if (i < (size - 1)) {
            HAL_I2C_SendACK();
        } else {
            HAL_I2C_SendNACK();
        }
        
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
        
        pData[i] = HAL_I2C_ReadByte();
    }
    
    /* Send STOP */
    HAL_I2C_Stop();
    
    return HAL_OK;
}

/**
 * @brief  Check if slave device is ready
 */
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(HAL_I2C_HandleTypeDef *hi2c,
                                         uint8_t slaveAddr,
                                         uint8_t retries, uint32_t timeout)
{
    uint8_t i;
    uint8_t status;
    
    for (i = 0; i < retries; i++) {
        /* Send START */
        HAL_I2C_Start();
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            continue;
        }
        
        /* Send slave address */
        HAL_I2C_WriteByte((slaveAddr << 1) | I2C_WRITE);
        STA = 0;
        if (HAL_I2C_WaitFlag(timeout) != HAL_OK) {
            HAL_I2C_Stop();
            continue;
        }
        
        status = HAL_I2C_GetStatus();
        HAL_I2C_Stop();
        
        if (status == 0x18) {
            return HAL_OK;  /* Device responded with ACK */
        }
    }
    
    return HAL_ERROR;
}

/*******************************************************************************
 * Weak Callback Functions
 ******************************************************************************/

__attribute__((weak)) void HAL_I2C_MasterTxCpltCallback(HAL_I2C_HandleTypeDef *hi2c)
{
    HAL_UNUSED(hi2c);
}

__attribute__((weak)) void HAL_I2C_MasterRxCpltCallback(HAL_I2C_HandleTypeDef *hi2c)
{
    HAL_UNUSED(hi2c);
}

__attribute__((weak)) void HAL_I2C_ErrorCallback(HAL_I2C_HandleTypeDef *hi2c)
{
    HAL_UNUSED(hi2c);
}

#endif /* HAL_I2C_MODULE_ENABLED */
