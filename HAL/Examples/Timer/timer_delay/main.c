/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Timer Delay Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates using Timer-based delay functions
 *          for precise timing control.
 *
 * Hardware:
 *   - LED: P1.2 (connected to LED, active low)
 *
 * This example blinks an LED with various timing patterns using:
 *   - HAL_Timer_DelayMs() for millisecond delays
 *   - HAL_Timer_DelayUs() for microsecond delays
 *   - HAL_Delay() for simple delays
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Define LED pin */
#define LED_PORT    HAL_GPIO_PORT1
#define LED_PIN     HAL_GPIO_PIN_2

/* System clock frequency */
#define SYSCLK      24000000UL

/**
 * @brief  Blink LED with fast pattern (using microsecond delays)
 */
void BlinkFast(uint8_t count)
{
    uint8_t i;
    
    for (i = 0; i < count; i++) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);  /* LED ON */
        HAL_Timer_DelayUs(SYSCLK, 50000);          /* 50ms ON */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);  /* LED OFF */
        HAL_Timer_DelayUs(SYSCLK, 50000);          /* 50ms OFF */
    }
}

/**
 * @brief  Blink LED with slow pattern (using millisecond delays)
 */
void BlinkSlow(uint8_t count)
{
    uint8_t i;
    
    for (i = 0; i < count; i++) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);  /* LED ON */
        HAL_Timer_DelayMs(SYSCLK, 500);            /* 500ms ON */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);  /* LED OFF */
        HAL_Timer_DelayMs(SYSCLK, 500);            /* 500ms OFF */
    }
}

/**
 * @brief  SOS pattern using Morse code timing
 */
void BlinkSOS(void)
{
    uint8_t i;
    
    printf("SOS Pattern: ");
    
    /* S: three short blinks */
    for (i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);
        HAL_Delay(100);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
        HAL_Delay(100);
    }
    
    HAL_Delay(200);  /* Pause between letters */
    
    /* O: three long blinks */
    for (i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);
        HAL_Delay(300);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
        HAL_Delay(100);
    }
    
    HAL_Delay(200);  /* Pause between letters */
    
    /* S: three short blinks */
    for (i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);
        HAL_Delay(100);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
        HAL_Delay(100);
    }
    
    printf("Done\r\n");
}

/**
 * @brief  Main function - Timer Delay example
 */
void main(void)
{
    HAL_GPIO_InitTypeDef gpio_init;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(SYSCLK);
    
    printf("\r\n=== MS51 HAL Timer Delay Example ===\r\n");
    
    /* Configure LED pin as output */
    gpio_init.Pin = LED_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    HAL_GPIO_Init(LED_PORT, &gpio_init);
    
    /* Start with LED OFF */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
    
    printf("LED configured on P1.2\r\n\r\n");
    
    /* Main loop */
    while (1) {
        /* Fast blink pattern (5 times) */
        printf("Fast blink (50ms on/off)...\r\n");
        BlinkFast(5);
        HAL_Delay(1000);
        
        /* Slow blink pattern (3 times) */
        printf("Slow blink (500ms on/off)...\r\n");
        BlinkSlow(3);
        HAL_Delay(1000);
        
        /* SOS pattern */
        BlinkSOS();
        HAL_Delay(2000);
        
        printf("--- Pattern complete, repeating ---\r\n\r\n");
    }
}
