/**
 *******************************************************************************
 * @file    main.c
 * @brief   ADC Single Conversion Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details ADC single conversion example.
 *          Reads analog voltage on AIN4 (P0.4) and displays via UART.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - Potentiometer connected to P0.4 (AIN4)
 *   - UART0: TXD on P0.6, RXD on P0.7
 *
 *******************************************************************************
 */

#include "ms51_hal.h"
#include <stdio.h>

/* System clock frequency */
#define SYSCLK_FREQ     24000000UL

/* ADC channel to read */
#define ADC_CHANNEL     HAL_ADC_CHANNEL_4

/**
 * @brief  Main program entry
 */
void main(void)
{
    uint16_t adcValue;
    uint32_t voltage_mv;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Initialize UART for printf */
    HAL_UART_InitPrintf(SYSCLK_FREQ);
    
    /* Initialize ADC */
    HAL_ADC_SimpleInit(HAL_ADC_DIV_2, HAL_ADC_AQT_5);
    
    /* Configure ADC channel */
    HAL_ADC_ConfigChannel(ADC_CHANNEL);
    
    /* Enable ADC */
    HAL_ADC_Enable();
    
    /* Print welcome message */
    printf("\n\r");
    printf("MS51 HAL ADC Example\n\r");
    printf("Reading AIN4 (P0.4)\n\r");
    printf("\n\r");
    
    /* Main loop - read and display ADC value */
    while (1) {
        /* Perform ADC conversion */
        adcValue = HAL_ADC_Read(ADC_CHANNEL);
        
        /* Convert to voltage (assuming 3.3V reference) */
        voltage_mv = HAL_ADC_TO_VOLTAGE_MV(adcValue);
        
        /* Print results */
        printf("ADC: %u, Voltage: %lu mV\n\r", 
               adcValue, (unsigned long)voltage_mv);
        
        /* Delay 500ms */
        HAL_Delay(500);
    }
}
