/**
 *******************************************************************************
 * @file    main.c
 * @brief   UART Echo Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details UART echo example - echoes received characters back.
 *          Demonstrates bidirectional UART communication.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - UART0: TXD on P0.6, RXD on P0.7
 *   - Connect to PC via USB-UART converter (115200,8,N,1)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"
#include <stdio.h>

/* System clock frequency */
#define SYSCLK_FREQ     24000000UL

/* UART handle */
HAL_UART_HandleTypeDef huart;

/**
 * @brief  Main program entry
 */
void main(void)
{
    uint8_t rxByte;
    HAL_StatusTypeDef status;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Initialize UART0 */
    huart.Instance = HAL_UART0;
    huart.Init.BaudRate = 115200;
    huart.Init.BaudRateGen = HAL_UART_BRG_TIMER1;
    huart.Init.Mode = HAL_UART_MODE_1;
    huart.Init.EnableRx = HAL_ENABLE;
    HAL_UART_Init(&huart);
    
    /* Enable printf for welcome message */
    HAL_UART_EnablePrintf(HAL_UART0);
    
    /* Print welcome message */
    printf("\n\r");
    printf("MS51 HAL UART Echo Example\n\r");
    printf("Type characters and they will be echoed back.\n\r");
    printf("\n\r");
    
    /* Main loop - echo received characters */
    while (1) {
        /* Wait for and receive a byte with timeout */
        status = HAL_UART_ReceiveByte(&huart, &rxByte, 100);
        
        if (status == HAL_OK) {
            /* Echo received byte */
            HAL_UART_TransmitByte(&huart, rxByte);
            
            /* Handle special characters */
            if (rxByte == '\r') {
                HAL_UART_TransmitByte(&huart, '\n');
            }
        }
        /* Timeout is normal - just continue waiting */
    }
}
