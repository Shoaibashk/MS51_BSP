/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL SPI Master Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates SPI master mode communication using the
 *          HAL SPI driver. It sends and receives data using polling mode.
 *
 * Hardware Connection:
 *   - P0.0 -> SPICLK (Clock)
 *   - P0.1 -> MISO   (Master In, Slave Out)
 *   - P0.2 -> MOSI   (Master Out, Slave In)
 *   - P0.3 -> SS     (Slave Select)
 *
 * Note: Connect to an SPI slave device for testing.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Test data buffer */
static uint8_t HAL_XDATA txBuffer[8] = {0x55, 0xAA, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
static uint8_t HAL_XDATA rxBuffer[8];

/**
 * @brief  Main function - SPI Master example
 */
void main(void)
{
    uint8_t i;
    HAL_SPI_InitTypeDef spi_init;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(24000000UL);
    
    printf("\r\n=== MS51 HAL SPI Master Example ===\r\n");
    
    /* Configure SPI */
    spi_init.Mode = HAL_SPI_MODE_MASTER;
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
    
    printf("SPI initialized successfully\r\n");
    printf("Clock Polarity: Low\r\n");
    printf("Clock Phase: 1st Edge\r\n");
    printf("Data Size: 8-bit\r\n\r\n");
    
    /* Main loop */
    while (1) {
        printf("Sending data: ");
        for (i = 0; i < 8; i++) {
            printf("0x%02X ", txBuffer[i]);
        }
        printf("\r\n");
        
        /* Select slave */
        HAL_SPI_Select();
        
        /* Transmit and receive data */
        for (i = 0; i < 8; i++) {
            rxBuffer[i] = HAL_SPI_TransmitReceive(txBuffer[i]);
        }
        
        /* Deselect slave */
        HAL_SPI_Deselect();
        
        /* Print received data */
        printf("Received data: ");
        for (i = 0; i < 8; i++) {
            printf("0x%02X ", rxBuffer[i]);
        }
        printf("\r\n\r\n");
        
        /* Delay before next transmission */
        HAL_Delay(1000);
    }
}
