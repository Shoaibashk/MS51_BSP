/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Project Template - IAR Embedded Workbench
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details Template project for IAR Embedded Workbench for 8051.
 *          Copy this folder to start a new project.
 *
 * Build Instructions:
 *   1. Create new IAR project for 8051
 *   2. Select target device: MS51FB9AE (or your variant)
 *   3. Add HAL source files to project
 *   4. Configure include paths
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
    HAL_UART_InitPrintf(SYSCLK_FREQ);
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
    printf("IAR Embedded Workbench\n\r");
    printf("\n\r");
    
    /* Enable global interrupts */
    HAL_EnableInterrupts();
    
    /* Main loop */
    while (1) {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(500);
        printf("LED Toggle\n\r");
    }
}

/*******************************************************************************
 * Interrupt Service Routines
 ******************************************************************************/

#if 0  /* Enable and customize as needed */

#pragma vector = TIMER0_VECTOR
__interrupt void Timer0_ISR(void)
{
    HAL_Timer_ClearFlag(HAL_TIMER0);
    /* Your code here */
}

#pragma vector = UART0_VECTOR
__interrupt void UART0_ISR(void)
{
    /* Your code here */
}

#endif
