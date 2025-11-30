/**
 *******************************************************************************
 * @file    main.c
 * @brief   GPIO Input Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details GPIO input example using button to control LED.
 *          Button on P1.6, LED on P1.2.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - Button connected to P1.6 (active low)
 *   - LED connected to P1.2 (active low)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Pin definitions */
#define LED_PORT    HAL_GPIO_PORT1
#define LED_PIN     HAL_GPIO_PIN_2

#define BTN_PORT    HAL_GPIO_PORT1
#define BTN_PIN     HAL_GPIO_PIN_6

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
    
    /* Configure button pin as input with Schmitt trigger */
    gpio_init.Pin = BTN_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_INPUT;
    gpio_init.Schmitt = HAL_GPIO_SCHMITT_ENABLE;
    HAL_GPIO_Init(BTN_PORT, &gpio_init);
    
    /* Ensure LED is off initially */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, HAL_GPIO_PIN_SET);
    
    /* Main loop - read button, control LED */
    while (1) {
        /* Read button state */
        if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == HAL_GPIO_PIN_RESET) {
            /* Button pressed (active low) - turn LED ON */
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, HAL_GPIO_PIN_RESET);
        } else {
            /* Button released - turn LED OFF */
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, HAL_GPIO_PIN_SET);
        }
        
        /* Small delay for debouncing */
        HAL_DelayUs(1000);
    }
}
