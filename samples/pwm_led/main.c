/*===========================================================================
 * @file    main.c
 * @brief   PWM LED Dimmer Sample - Using PWM Driver
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   This sample demonstrates PWM usage to create a breathing LED effect.
 *   The LED brightness gradually increases and decreases in a smooth cycle.
 * 
 * Hardware:
 *   - LED connected to P0.0 (PWM0 output)
 *   - Any MS51 device with PWM
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_pwm.h"
#include "svc_delay.h"
#include "svc_console.h"
#include "svc_system.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define PWM_PERIOD      1000    /* PWM period (counts) */
#define STEP_SIZE       10      /* Brightness step size */
#define STEP_DELAY_MS   20      /* Delay between steps (ms) */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static pwm_handle_t g_pwm0;

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    console_config_t con_cfg;
    pwm_config_t pwm_cfg;
    drv_status_t status;
    uint16_t duty = 0;
    int8_t direction = 1;  /* 1 = increasing, -1 = decreasing */

    /* Initialize system */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Initialize console for debug output */
    con_cfg.port = UART_PORT_0;
    con_cfg.baudrate = 115200;
    con_cfg.echo_enable = 0;
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  MS51 PWM LED Dimmer Demo\r\n");
    SVC_CONSOLE_Printf("  Output: P0.0 (PWM0)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Configure PWM0 */
    pwm_cfg.channel = PWM_CH_0;
    pwm_cfg.clock_source = PWM_CLK_FSYS;
    pwm_cfg.mode = PWM_MODE_INDEPENDENT;
    pwm_cfg.alignment = PWM_ALIGN_EDGE;
    pwm_cfg.prescaler = 23;       /* FSYS/24 = 1MHz PWM clock */
    pwm_cfg.period = PWM_PERIOD;  /* 1MHz / 1000 = 1kHz PWM frequency */
    pwm_cfg.duty = 0;             /* Start at 0% */
    pwm_cfg.invert = 0;
    pwm_cfg.dead_time = 0;

    status = DRV_PWM_Init(&g_pwm0, &pwm_cfg);
    if (status != DRV_OK) {
        SVC_CONSOLE_Printf("PWM init failed: %d\r\n", status);
        while (1);
    }

    /* Start PWM output */
    status = DRV_PWM_Start(&g_pwm0);
    if (status != DRV_OK) {
        SVC_CONSOLE_Printf("PWM start failed: %d\r\n", status);
        while (1);
    }

    SVC_CONSOLE_Printf("PWM running at 1 kHz\r\n");
    SVC_CONSOLE_Printf("Breathing effect started...\r\n");

    /* Main loop - breathing LED effect */
    while (1) {
        /* Update duty cycle */
        duty = (uint16_t)((int16_t)duty + (direction * STEP_SIZE));
        
        /* Clamp and reverse direction at limits */
        if (duty >= PWM_PERIOD) {
            duty = PWM_PERIOD;
            direction = -1;
        } else if (duty == 0 || (direction < 0 && duty < STEP_SIZE)) {
            duty = 0;
            direction = 1;
        }

        /* Set new duty cycle */
        DRV_PWM_SetDuty(&g_pwm0, duty);

        /* Small delay for smooth effect */
        SVC_DELAY_Ms(STEP_DELAY_MS);
    }
}
