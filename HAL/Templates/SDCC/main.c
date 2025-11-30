/**
 *******************************************************************************
 * @file    main.c
 * @brief   MS51 HAL Project Template - SDCC
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details Template project for SDCC (Small Device C Compiler).
 *          Copy this folder to start a new project.
 *
 * Build Instructions:
 *   1. Install SDCC (https://sdcc.sourceforge.net/)
 *   2. Use the provided Makefile: make
 *   3. Program the resulting .ihx file to the device
 *
 * Manual compilation (single file):
 *   sdcc -mmcs51 --model-small --code-size 16384 \
 *        -I../../Core/Inc -I../../Device/MS51_16K \
 *        -I../../Drivers/GPIO -I../../Drivers/UART \
 *        main.c
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
    printf("SDCC Compiler\n\r");
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

/* Timer0 ISR */
void Timer0_ISR(void) __interrupt(1)
{
    HAL_Timer_ClearFlag(HAL_TIMER0);
    /* Your code here */
}

/* UART0 ISR */
void UART0_ISR(void) __interrupt(4)
{
    /* Your code here */
}

#endif
