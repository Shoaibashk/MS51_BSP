/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL BOD (Brown-out Detector) Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates the Brown-out Detector functionality:
 *          - Configure BOD with voltage threshold
 *          - Enable BOD reset or interrupt
 *          - Monitor power supply voltage
 *
 * When the supply voltage drops below the configured threshold:
 *   - If reset is enabled: System will reset
 *   - If interrupt is enabled: BOD interrupt will fire
 *
 * Hardware:
 *   - LED: P1.2 (blinks to show system is running)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* LED Configuration */
#define LED_PORT    HAL_GPIO_PORT1
#define LED_PIN     HAL_GPIO_PIN_2

/* BOD interrupt flag */
static volatile uint8_t bodEventDetected = 0;

/**
 * @brief  BOD Interrupt Handler
 */
void BOD_ISR(void) HAL_INTERRUPT(8)
{
    /* Set flag for main loop to process */
    bodEventDetected = 1;
    
    /* Clear BOD flag */
    HAL_BOD_ClearFlag();
}

/**
 * @brief  Main function - BOD example
 */
void main(void)
{
    HAL_GPIO_InitTypeDef gpio_init;
    HAL_BOD_InitTypeDef bod_init;
    uint8_t i;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(24000000UL);
    
    printf("\r\n=== MS51 HAL BOD Example ===\r\n");
    
    /* Check if reset was caused by BOD */
    if (HAL_BOD_WasResetSource()) {
        printf("!!! SYSTEM RESET BY BROWN-OUT !!!\r\n");
        printf("Power supply dropped below threshold\r\n\r\n");
    } else {
        printf("Normal system startup\r\n\r\n");
    }
    
    /* Configure LED pin as output */
    gpio_init.Pin = LED_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    HAL_GPIO_Init(LED_PORT, &gpio_init);
    
    /* Configure BOD */
    printf("Configuring Brown-out Detector...\r\n");
    
    bod_init.State = HAL_BOD_STATE_ENABLE;
    bod_init.Level = HAL_BOD_LEVEL_2V4;           /* 2.4V threshold */
    bod_init.ResetMode = HAL_BOD_RESET_DISABLE;   /* Use interrupt instead */
    bod_init.LowPowerMode = HAL_BOD_LP_MODE_NORMAL;
    
    if (HAL_BOD_Init(&bod_init) != HAL_OK) {
        printf("BOD initialization failed!\r\n");
        while (1);
    }
    
    /* Enable BOD interrupt */
    HAL_BOD_EnableInterrupt();
    
    /* Enable global interrupts */
    HAL_EnableInterrupts();
    
    printf("BOD configured:\r\n");
    printf("  - Voltage threshold: 2.4V\r\n");
    printf("  - Reset mode: DISABLED (using interrupt)\r\n");
    printf("  - Low power mode: NORMAL\r\n\r\n");
    printf("Monitoring power supply voltage...\r\n\r\n");
    
    /* Main loop */
    while (1) {
        /* Check for BOD event */
        if (bodEventDetected) {
            bodEventDetected = 0;
            
            printf("\r\n!!! BROWN-OUT DETECTED !!!\r\n");
            printf("Supply voltage dropped below 2.4V\r\n");
            printf("Taking protective action...\r\n\r\n");
            
            /* Here you could:
             * - Save critical data to EEPROM
             * - Disable non-essential peripherals
             * - Enter low-power mode
             * - Signal an external power management IC
             */
            
            /* Fast blink LED to indicate BOD event */
            for (i = 0; i < 10; i++) {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_Delay(50);
            }
        }
        
        /* Normal operation - blink LED slowly */
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(500);
        
        /* Check BOD status periodically */
        if (HAL_BOD_GetStatus()) {
            printf("BOD: Low voltage detected!\r\n");
        }
    }
}
