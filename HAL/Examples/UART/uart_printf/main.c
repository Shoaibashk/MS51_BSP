/**
 *******************************************************************************
 * @file    main.c
 * @brief   UART Printf Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details UART printf example for debug output.
 *          Demonstrates serial communication at 115200 baud.
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

/**
 * @brief  Main program entry
 */
void main(void)
{
    uint32_t counter = 0;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Initialize UART0 for printf at 115200 baud */
    HAL_UART_InitPrintf(SYSCLK_FREQ);
    
    /* Print welcome message */
    printf("\n\r");
    printf("*******************************\n\r");
    printf("* MS51 HAL Library Demo       *\n\r");
    printf("* UART Printf Example         *\n\r");
    printf("* System Clock: 24 MHz        *\n\r");
    printf("*******************************\n\r");
    printf("\n\r");
    
    /* Main loop - print counter periodically */
    while (1) {
        printf("Counter: %lu\n\r", (unsigned long)counter);
        counter++;
        
        /* Delay 1 second */
        HAL_Delay(1000);
    }
}
