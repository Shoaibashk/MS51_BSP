/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL SPI Slave Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates SPI slave mode communication using the
 *          HAL SPI driver with interrupt.
 *
 * Hardware Connection:
 *   - P0.0 -> SPICLK (Clock from master)
 *   - P0.1 -> MISO   (Slave Out to Master)
 *   - P0.2 -> MOSI   (Master Out to Slave)
 *   - P1.0 -> SS     (Slave Select from master)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Data buffers */
static uint8_t HAL_XDATA rxBuffer[32];
static uint8_t HAL_XDATA rxIndex = 0;
static uint8_t HAL_XDATA dataReady = 0;

/**
 * @brief  SPI Interrupt Handler
 */
void SPI_ISR(void) HAL_INTERRUPT(9)
{
    uint8_t rxData;
    
    /* Check SPI interrupt flag */
    if (HAL_SPI_GetFlag()) {
        /* Read received data */
        rxData = HAL_SPI_ReceiveData();
        
        /* Store in buffer */
        if (rxIndex < 32) {
            rxBuffer[rxIndex++] = rxData;
        }
        
        /* Prepare response data (echo back +1) */
        HAL_SPI_SendData(rxData + 1);
        
        /* Check for end of packet (0xFF) */
        if (rxData == 0xFF) {
            dataReady = 1;
        }
        
        /* Clear interrupt flag */
        HAL_SPI_ClearFlag();
    }
}

/**
 * @brief  Main function - SPI Slave example
 */
void main(void)
{
    uint8_t i;
    HAL_SPI_InitTypeDef spi_init;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(24000000UL);
    
    printf("\r\n=== MS51 HAL SPI Slave Example ===\r\n");
    
    /* Configure SPI as slave */
    spi_init.Mode = HAL_SPI_MODE_SLAVE;
    spi_init.ClockPolarity = HAL_SPI_CPOL_LOW;
    spi_init.ClockPhase = HAL_SPI_CPHA_1EDGE;
    spi_init.DataSize = HAL_SPI_DATASIZE_8BIT;
    spi_init.BaudRatePrescaler = HAL_SPI_BAUDRATE_DIV4;
    spi_init.FirstBit = HAL_SPI_FIRSTBIT_MSB;
    
    /* Initialize SPI */
    if (HAL_SPI_Init(&spi_init) != HAL_OK) {
        printf("SPI initialization failed!\r\n");
        while (1);
    }
    
    /* Enable SPI interrupt */
    HAL_SPI_EnableInterrupt();
    
    /* Enable global interrupts */
    HAL_EnableInterrupts();
    
    printf("SPI Slave initialized\r\n");
    printf("Waiting for data from master...\r\n\r\n");
    
    /* Main loop */
    while (1) {
        /* Check if data packet received */
        if (dataReady) {
            printf("Received %d bytes: ", rxIndex);
            for (i = 0; i < rxIndex; i++) {
                printf("0x%02X ", rxBuffer[i]);
            }
            printf("\r\n");
            
            /* Reset for next packet */
            rxIndex = 0;
            dataReady = 0;
        }
        
        /* Small delay */
        HAL_Delay(10);
    }
}
