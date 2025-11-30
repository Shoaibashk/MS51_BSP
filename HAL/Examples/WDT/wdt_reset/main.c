/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Watchdog Timer (WDT) Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates the Watchdog Timer functionality:
 *          - Initialize WDT with reset enabled
 *          - Regular watchdog feeding to prevent reset
 *          - Optional: Stop feeding to demonstrate WDT reset
 *
 * Hardware:
 *   - LED: P1.2 (blinks to show system is running)
 *
 * Note: If the WDT is not fed within the timeout period, the system
 *       will reset and start over.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* LED Configuration */
#define LED_PORT    HAL_GPIO_PORT1
#define LED_PIN     HAL_GPIO_PIN_2

/* Flag to simulate watchdog timeout */
static volatile uint8_t stopFeedingWDT = 0;
static volatile uint16_t loopCount = 0;

/**
 * @brief  Main function - WDT example
 */
void main(void)
{
    HAL_GPIO_InitTypeDef gpio_init;
    HAL_WDT_InitTypeDef wdt_init;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(24000000UL);
    
    printf("\r\n=== MS51 HAL Watchdog Timer Example ===\r\n");
    
    /* Check if reset was caused by WDT */
    if (HAL_WDT_WasResetSource()) {
        printf("!!! SYSTEM RESET BY WATCHDOG !!!\r\n");
        printf("Previous watchdog timeout caused reset\r\n\r\n");
        HAL_WDT_ClearResetFlag();
    } else {
        printf("Normal system startup\r\n\r\n");
    }
    
    /* Configure LED pin as output */
    gpio_init.Pin = LED_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    HAL_GPIO_Init(LED_PORT, &gpio_init);
    
    /* Start with LED OFF */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
    
    /* Configure Watchdog Timer */
    printf("Configuring Watchdog Timer...\r\n");
    
    wdt_init.Timeout = HAL_WDT_TIMEOUT_1_6S;  /* ~1.6 second timeout */
    wdt_init.EnableReset = HAL_ENABLE;         /* Enable reset on timeout */
    wdt_init.EnableInterrupt = HAL_DISABLE;    /* No interrupt */
    
    if (HAL_WDT_Init(&wdt_init) != HAL_OK) {
        printf("WDT initialization failed!\r\n");
        while (1);
    }
    
    printf("WDT configured with ~1.6 second timeout\r\n");
    printf("Reset on timeout: ENABLED\r\n\r\n");
    
    /* Start the watchdog */
    HAL_WDT_Start();
    printf("Watchdog started!\r\n");
    printf("System will reset if not fed within 1.6 seconds\r\n\r\n");
    
    /* Main loop */
    while (1) {
        /* Toggle LED to show system is alive */
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        
        loopCount++;
        printf("Loop %u - ", loopCount);
        
        /* Decide whether to feed the watchdog */
        if (!stopFeedingWDT) {
            /* Feed the watchdog to prevent reset */
            HAL_WDT_Refresh();
            printf("Watchdog fed OK\r\n");
            
            /* After 10 iterations, stop feeding to demonstrate reset */
            if (loopCount >= 10) {
                printf("\r\n*** STOPPING WATCHDOG FEED ***\r\n");
                printf("System will reset in ~1.6 seconds...\r\n\r\n");
                stopFeedingWDT = 1;
            }
        } else {
            printf("NOT feeding watchdog - waiting for reset...\r\n");
        }
        
        /* Delay 500ms (well within 1.6s WDT timeout) */
        HAL_Delay(500);
    }
}
