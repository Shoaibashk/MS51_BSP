/**
 *******************************************************************************
 * @file    main.c
 * @brief   GPIO Blink Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details Simple LED blink example using HAL GPIO driver.
 *          LED on P1.2 blinks at approximately 1Hz.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - LED connected to P1.2 (active low)
 *
 * Build:
 *   Keil:  C51 main.c
 *   SDCC:  sdcc -mmcs51 main.c
 *   IAR:   icc8051 main.c
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* LED is on P1.2 (active low on development board) */
#define LED_PORT    HAL_GPIO_PORT1
#define LED_PIN     HAL_GPIO_PIN_2

/**
 * @brief  Main program entry
 */
void main(void)
{
    HAL_GPIO_InitTypeDef gpio_init;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Configure LED pin as push-pull output */
    gpio_init.Pin = LED_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    gpio_init.Schmitt = HAL_GPIO_SCHMITT_DISABLE;
    HAL_GPIO_Init(LED_PORT, &gpio_init);
    
    /* Main loop - blink LED */
    while (1) {
        /* Turn LED ON (active low) */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, HAL_GPIO_PIN_RESET);
        HAL_Delay(500);  /* 500ms delay */
        
        /* Turn LED OFF */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, HAL_GPIO_PIN_SET);
        HAL_Delay(500);  /* 500ms delay */
    }
}
