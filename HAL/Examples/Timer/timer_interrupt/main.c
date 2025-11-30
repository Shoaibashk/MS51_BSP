/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Timer Interrupt Example
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates Timer0 and Timer1 interrupt-based
 *          periodic tasks using the HAL Timer driver.
 *
 *          - Timer0: 1ms interrupt for LED1 toggle (500ms period)
 *          - Timer1: 1ms interrupt for LED2 toggle (250ms period)
 *
 * Hardware:
 *   - LED1: P1.2 (toggle every 500ms via Timer0)
 *   - LED2: P1.3 (toggle every 250ms via Timer1)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Timer interrupt counters */
static volatile uint16_t HAL_DATA timer0_count = 0;
static volatile uint16_t HAL_DATA timer1_count = 0;

/* Timer reload values for 1ms interrupt at 24MHz with Fsys/12 */
#define TIMER_RELOAD_1MS_H  (0xF8)  /* High byte */
#define TIMER_RELOAD_1MS_L  (0x30)  /* Low byte */

/**
 * @brief  Timer0 Interrupt Handler - 1ms tick
 */
void Timer0_ISR(void) HAL_INTERRUPT(1)
{
    /* Reload timer for next 1ms interval */
    TL0 = TIMER_RELOAD_1MS_L;
    TH0 = TIMER_RELOAD_1MS_H;
    
    /* Increment counter */
    timer0_count++;
    
    /* Toggle LED1 every 500ms */
    if (timer0_count >= 500) {
        timer0_count = 0;
        HAL_GPIO_TogglePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2);
    }
}

/**
 * @brief  Timer1 Interrupt Handler - 1ms tick
 */
void Timer1_ISR(void) HAL_INTERRUPT(3)
{
    /* Reload timer for next 1ms interval */
    TL1 = TIMER_RELOAD_1MS_L;
    TH1 = TIMER_RELOAD_1MS_H;
    
    /* Increment counter */
    timer1_count++;
    
    /* Toggle LED2 every 250ms */
    if (timer1_count >= 250) {
        timer1_count = 0;
        HAL_GPIO_TogglePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_3);
    }
}

/**
 * @brief  Main function - Timer Interrupt example
 */
void main(void)
{
    HAL_Timer01_InitTypeDef timer0_init;
    HAL_Timer01_InitTypeDef timer1_init;
    HAL_GPIO_InitTypeDef gpio_init;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(24000000UL);
    
    printf("\r\n=== MS51 HAL Timer Interrupt Example ===\r\n");
    
    /* Configure LED1 (P1.2) as output */
    gpio_init.Pin = HAL_GPIO_PIN_2;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    HAL_GPIO_Init(HAL_GPIO_PORT1, &gpio_init);
    
    /* Configure LED2 (P1.3) as output */
    gpio_init.Pin = HAL_GPIO_PIN_3;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    HAL_GPIO_Init(HAL_GPIO_PORT1, &gpio_init);
    
    /* Initialize LEDs to OFF */
    HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, 1);
    HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_3, 1);
    
    /* Configure Timer0 for 1ms interrupt */
    timer0_init.Mode = HAL_TIMER_MODE1;         /* 16-bit timer mode */
    timer0_init.ClockSource = HAL_TIMER_CLK_FSYS_DIV12;
    timer0_init.Period = 0xF830;                /* 1ms at 24MHz/12 */
    timer0_init.EnableGate = 0;
    timer0_init.CounterMode = 0;
    
    if (HAL_Timer0_Init(&timer0_init) != HAL_OK) {
        printf("Timer0 initialization failed!\r\n");
        while (1);
    }
    
    /* Configure Timer1 for 1ms interrupt */
    timer1_init.Mode = HAL_TIMER_MODE1;         /* 16-bit timer mode */
    timer1_init.ClockSource = HAL_TIMER_CLK_FSYS_DIV12;
    timer1_init.Period = 0xF830;                /* 1ms at 24MHz/12 */
    timer1_init.EnableGate = 0;
    timer1_init.CounterMode = 0;
    
    if (HAL_Timer1_Init(&timer1_init) != HAL_OK) {
        printf("Timer1 initialization failed!\r\n");
        while (1);
    }
    
    /* Enable Timer0 and Timer1 interrupts */
    HAL_Timer_EnableInterrupt(HAL_TIMER0);
    HAL_Timer_EnableInterrupt(HAL_TIMER1);
    
    /* Start both timers */
    HAL_Timer_Start(HAL_TIMER0);
    HAL_Timer_Start(HAL_TIMER1);
    
    /* Enable global interrupts */
    HAL_EnableInterrupts();
    
    printf("Timer0: 1ms tick, LED1 toggle every 500ms\r\n");
    printf("Timer1: 1ms tick, LED2 toggle every 250ms\r\n");
    printf("Timers running...\r\n\r\n");
    
    /* Main loop - nothing to do, interrupts handle everything */
    while (1) {
        /* CPU can do other tasks here */
        HAL_NOP();
    }
}
