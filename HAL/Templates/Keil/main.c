/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Project Template - Keil C51
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details Template project for Keil C51 compiler.
 *          Copy this folder to start a new project.
 *
 * Build Instructions:
 *   1. Create new Keil project
 *   2. Select target device: MS51FB9AE (or your variant)
 *   3. Add HAL source files to project
 *   4. Add include paths for HAL headers
 *   5. Build and download
 *
 *******************************************************************************
 */

#include "ms51_hal.h"
#include <stdio.h>

/* System clock frequency */
#define SYSCLK_FREQ     24000000UL

/* Application-specific definitions */
#define LED_PORT        HAL_GPIO_PORT1
#define LED_PIN         HAL_GPIO_PIN_2

/**
 * @brief  System Clock Configuration
 * @note   Configure HIRC to 24MHz
 */
void SystemClock_Config(void)
{
#ifdef HAL_SYSTEM_MODULE_ENABLED
    HAL_System_SetHIRC(HAL_HIRC_24MHZ);
#endif
}

/**
 * @brief  GPIO Configuration
 */
void GPIO_Config(void)
{
    HAL_GPIO_InitTypeDef gpio_init;
    
    /* Configure LED pin as push-pull output */
    gpio_init.Pin = LED_PIN;
    gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
    gpio_init.Schmitt = HAL_GPIO_SCHMITT_DISABLE;
    HAL_GPIO_Init(LED_PORT, &gpio_init);
}

/**
 * @brief  UART Configuration
 */
void UART_Config(void)
{
    /* Initialize UART0 for printf at 115200 baud */
    HAL_UART_InitPrintf(SYSCLK_FREQ);
}

/**
 * @brief  Error Handler
 */
void Error_Handler(void)
{
    /* Turn off LED and loop forever */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, HAL_GPIO_PIN_SET);
    
    while (1) {
        /* Toggle LED rapidly to indicate error */
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(100);
    }
}

/**
 * @brief  Main program entry
 */
void main(void)
{
    /* Initialize HAL */
    HAL_Init();
    
    /* Configure system clock */
    SystemClock_Config();
    
    /* Configure peripherals */
    GPIO_Config();
    UART_Config();
    
    /* Print welcome message */
    printf("\n\r");
    printf("MS51 HAL Library Template\n\r");
    printf("Keil C51 Compiler\n\r");
    printf("System Clock: %lu Hz\n\r", (unsigned long)SYSCLK_FREQ);
    printf("\n\r");
    
    /* Enable global interrupts */
    HAL_EnableInterrupts();
    
    /* Main loop */
    while (1) {
        /* Toggle LED */
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        
        /* Delay 500ms */
        HAL_Delay(500);
        
        /* Print status */
        printf("LED Toggle\n\r");
    }
}

/*******************************************************************************
 * Interrupt Service Routines
 ******************************************************************************/

#if 0  /* Enable and customize as needed */

/* Timer0 ISR */
void Timer0_ISR(void) interrupt 1
{
    HAL_Timer_ClearFlag(HAL_TIMER0);
    /* Your code here */
}

/* UART0 ISR */
void UART0_ISR(void) interrupt 4
{
    /* Your code here */
}

/* ADC ISR */
void ADC_ISR(void) interrupt 11
{
    HAL_ADC_ClearFlag();
    /* Your code here */
}

#endif
