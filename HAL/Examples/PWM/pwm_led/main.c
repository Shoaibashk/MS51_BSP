/**
 *******************************************************************************
 * @file    main.c
 * @brief   PWM LED Dimmer Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details PWM example for LED brightness control.
 *          Implements a breathing LED effect using PWM.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - LED connected to P1.2 (PWM0)
 *
 *******************************************************************************
 */

#include "ms51_hal.h"
#include <stdio.h>

/* System clock frequency */
#define SYSCLK_FREQ     24000000UL

/* PWM Configuration */
#define PWM_PERIOD      1000    /* 12-bit period value */
#define PWM_CHANNEL     HAL_PWM_CHANNEL_0

/* Breathing effect parameters */
#define BREATH_STEP     10      /* Brightness change per step */
#define BREATH_DELAY    20      /* Delay between steps (ms) */

/**
 * @brief  Main program entry
 */
void main(void)
{
    HAL_PWM_HandleTypeDef hpwm;
    HAL_PWM_ChannelInitTypeDef pwmChannel;
    uint16_t duty = 0;
    int8_t direction = 1;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(SYSCLK_FREQ);
    
    printf("\n\rMS51 HAL PWM LED Dimmer Example\n\r");
    printf("Breathing LED on P1.2 (PWM0)\n\r\n\r");
    
    /* Initialize PWM */
    hpwm.Init.Mode = HAL_PWM_MODE_INDEPENDENT;
    hpwm.Init.Type = HAL_PWM_TYPE_EDGE;
    hpwm.Init.ClockSource = HAL_PWM_CLK_FSYS;
    hpwm.Init.ClockDiv = HAL_PWM_DIV_8;  /* 24MHz / 8 = 3MHz */
    hpwm.Init.Period = PWM_PERIOD;        /* 3MHz / 1000 = 3kHz */
    HAL_PWM_Init(&hpwm);
    
    /* Configure PWM channel 0 */
    pwmChannel.Channel = PWM_CHANNEL;
    pwmChannel.DutyCycle = 0;
    pwmChannel.OutputEnable = HAL_ENABLE;
    pwmChannel.Polarity = 1;  /* Inverted for active-low LED */
    HAL_PWM_ConfigChannel(&pwmChannel);
    
    /* Start PWM */
    HAL_PWM_Start();
    
    printf("PWM started. Period: %u, Frequency: ~%lu Hz\n\r", 
           PWM_PERIOD, (unsigned long)(SYSCLK_FREQ / 8 / PWM_PERIOD));
    
    /* Main loop - breathing effect */
    while (1) {
        /* Update duty cycle */
        HAL_PWM_SetDuty(PWM_CHANNEL, duty);
        HAL_PWM_LoadValues();
        
        /* Update brightness direction */
        duty += (direction * BREATH_STEP);
        
        /* Reverse direction at limits */
        if (duty >= PWM_PERIOD) {
            duty = PWM_PERIOD;
            direction = -1;
        } else if (duty == 0 || (direction < 0 && duty < BREATH_STEP)) {
            duty = 0;
            direction = 1;
        }
        
        /* Delay for smooth breathing effect */
        HAL_Delay(BREATH_DELAY);
    }
}
