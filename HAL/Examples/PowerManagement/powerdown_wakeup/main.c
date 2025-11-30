/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Power-Down Mode Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates low-power modes:
 *          - Idle mode (CPU stops, peripherals running)
 *          - Power-down mode (lowest power, wake by external interrupt)
 *          - Wake-up Timer for periodic wake from power-down
 *
 * Wake-up sources:
 *   - External interrupt on P3.0 (INT0)
 *   - Wake-up Timer (WKT)
 *
 * Hardware:
 *   - LED: P1.2 (shows power state)
 *   - Button: P3.0 (wake-up source, active low with external pull-up)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Hardware Configuration */
#define LED_PORT        HAL_GPIO_PORT1
#define LED_PIN         HAL_GPIO_PIN_2
#define BUTTON_PORT     HAL_GPIO_PORT3
#define BUTTON_PIN      HAL_GPIO_PIN_0

/* Wake-up source flag */
static volatile uint8_t wakeupSource = 0;
#define WAKEUP_NONE     0
#define WAKEUP_BUTTON   1
#define WAKEUP_WKT      2

/**
 * @brief  External Interrupt 0 Handler (Wake-up button)
 */
void INT0_ISR(void) HAL_INTERRUPT(0)
{
    wakeupSource = WAKEUP_BUTTON;
    /* INT0 flag is automatically cleared */
}

/**
 * @brief  Wake-up Timer Interrupt Handler
 */
void WKT_ISR(void) HAL_INTERRUPT(17)
{
    wakeupSource = WAKEUP_WKT;
    HAL_WKT_ClearFlag();
}

/**
 * @brief  Quick LED blink to indicate activity
 */
void BlinkLED(uint8_t count, uint16_t delay_ms)
{
    uint8_t i;
    for (i = 0; i < count; i++) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 0);  /* ON */
        HAL_Delay(delay_ms);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);  /* OFF */
        HAL_Delay(delay_ms);
    }
}

/**
 * @brief  Main function - Power Management example
 */
void main(void)
{
    HAL_GPIO_InitTypeDef gpio_init;
    HAL_WKT_InitTypeDef wkt_init;
    uint8_t cycleCount = 0;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(24000000UL);
    
    printf("\r\n=== MS51 HAL Power Management Example ===\r\n");
    
    /* Configure LED pin as output */
    gpio_init.Pin = LED_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    HAL_GPIO_Init(LED_PORT, &gpio_init);
    
    /* Configure Button pin as input with internal pull-up */
    gpio_init.Pin = BUTTON_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_INPUT;
    HAL_GPIO_Init(BUTTON_PORT, &gpio_init);
    
    /* LED OFF initially */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
    
    /* Configure External Interrupt 0 (falling edge on P3.0) */
    /* INT0 is mapped to P3.0 by default */
    IT0 = 1;    /* Falling edge triggered */
    EX0 = 1;    /* Enable INT0 */
    
    /* Configure Wake-up Timer for 5 second wake-up */
    printf("Configuring Wake-up Timer for 5 second interval...\r\n");
    
    wkt_init.Divider = HAL_WKT_DIV_2048;      /* ~204.8ms per tick */
    wkt_init.ReloadValue = 256 - 25;           /* 25 ticks = ~5 seconds */
    wkt_init.EnableInterrupt = 1;
    
    HAL_WKT_Init(&wkt_init);
    
    /* Enable global interrupts */
    HAL_EnableInterrupts();
    
    printf("Power management modes:\r\n");
    printf("  1. Normal: LED blinks fast\r\n");
    printf("  2. Idle: LED blinks slow (CPU halted)\r\n");
    printf("  3. Power-Down: LED off, wake by button or WKT\r\n\r\n");
    printf("Press button on P3.0 to wake from power-down\r\n");
    printf("WKT will also wake system every 5 seconds\r\n\r\n");
    
    /* Main loop */
    while (1) {
        cycleCount++;
        printf("\r\n=== Cycle %u ===\r\n", cycleCount);
        
        /* Phase 1: Normal operation (active mode) */
        printf("1. NORMAL MODE - Fast LED blink (3 sec)\r\n");
        BlinkLED(15, 100);  /* Fast blink for 3 seconds */
        
        /* Phase 2: Idle mode */
        printf("2. ENTERING IDLE MODE (3 sec)...\r\n");
        BlinkLED(3, 500);   /* Slow blink to indicate idle */
        
        HAL_EnterIdle();    /* CPU halts, peripherals continue */
        /* Wakes on any interrupt */
        
        printf("   Woke from Idle mode\r\n");
        BlinkLED(2, 100);   /* Quick blink on wake */
        
        /* Phase 3: Power-Down mode */
        printf("3. ENTERING POWER-DOWN MODE...\r\n");
        printf("   Wake sources: Button (P3.0) or WKT (5 sec)\r\n");
        HAL_Delay(100);     /* Let UART finish */
        
        /* LED OFF before power-down */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, 1);
        
        /* Start WKT for timed wake-up */
        HAL_WKT_Start();
        
        /* Clear wake-up source */
        wakeupSource = WAKEUP_NONE;
        
        /* Enter power-down mode */
        HAL_EnterPowerDown();
        /* CPU stops here until interrupt */
        
        /* Woke up! */
        HAL_WKT_Stop();
        
        /* Identify wake-up source */
        if (wakeupSource == WAKEUP_BUTTON) {
            printf("   Woke from Power-Down by BUTTON\r\n");
            BlinkLED(5, 100);   /* 5 fast blinks for button */
        } else if (wakeupSource == WAKEUP_WKT) {
            printf("   Woke from Power-Down by WKT\r\n");
            BlinkLED(3, 200);   /* 3 medium blinks for WKT */
        } else {
            printf("   Woke from Power-Down (unknown source)\r\n");
            BlinkLED(1, 500);
        }
        
        /* Small delay before next cycle */
        HAL_Delay(1000);
    }
}
