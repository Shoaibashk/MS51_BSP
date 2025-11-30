/**
 *******************************************************************************
 * @file    main.c
 * @brief   ADC VDD Measurement Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details Measures VDD supply voltage using internal bandgap reference.
 *          The bandgap voltage is approximately 1.22V.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - UART0: TXD on P0.6, RXD on P0.7
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
    uint16_t bgAdc;
    uint16_t bgCal;
    uint16_t vdd_mv;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Initialize UART for printf */
    HAL_UART_InitPrintf(SYSCLK_FREQ);
    
    /* Initialize ADC */
    HAL_ADC_SimpleInit(HAL_ADC_DIV_2, HAL_ADC_AQT_5);
    HAL_ADC_Enable();
    
    /* Read bandgap calibration value from flash */
    bgCal = HAL_ADC_ReadBandgapCalibration();
    
    /* Print welcome message */
    printf("\n\r");
    printf("MS51 HAL VDD Measurement Example\n\r");
    printf("Bandgap Calibration: %u\n\r", bgCal);
    printf("\n\r");
    
    /* Main loop - measure VDD periodically */
    while (1) {
        /* Read bandgap voltage via ADC */
        bgAdc = HAL_ADC_ReadBandgap();
        
        /* Calculate VDD from bandgap reading */
        vdd_mv = HAL_ADC_CalculateVDD(bgAdc);
        
        /* Print results */
        printf("Bandgap ADC: %u, VDD: %u mV\n\r", bgAdc, vdd_mv);
        
        /* Delay 1 second */
        HAL_Delay(1000);
    }
}
