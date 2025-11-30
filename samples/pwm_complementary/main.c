/*===========================================================================
 * @file    main.c
 * @brief   PWM Complementary Mode with Dead-Time Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates PWM complementary mode for half-bridge motor drive.
 *   PWM0 and PWM1 output complementary signals with dead-time insertion.
 * 
 * Hardware:
 *   - PWM0 on P1.2 (high-side)
 *   - PWM1 on P1.1 (low-side, inverted)
 *   - Connect to half-bridge driver or oscilloscope
 *   - DO NOT connect directly to motor without proper driver circuit!
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_pwm.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define PWM_PERIOD          1000    /* PWM period counts */
#define PWM_FREQ_HZ         20000   /* 20 kHz for motor drive */
#define DEAD_TIME_NS        500     /* 500ns dead-time */
#define INITIAL_DUTY        50      /* 50% initial duty */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static pwm_handle_t g_pwm0;
static pwm_handle_t g_pwm1;

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    gpio_handle_t pin0, pin1;
    pwm_config_t pwm_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint8_t duty = INITIAL_DUTY;
    uint8_t dead_time = 16;     /* Dead-time in PWM clocks */

    /* Initialize system at 24MHz */
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
    SVC_CONSOLE_Printf("  PWM Complementary Mode Demo\r\n");
    SVC_CONSOLE_Printf("  For Half-Bridge Motor Drive\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("PWM0 (P1.2): High-side output\r\n");
    SVC_CONSOLE_Printf("PWM1 (P1.1): Low-side output (inverted)\r\n");
    SVC_CONSOLE_Printf("Dead-time: %u ns\r\n", DEAD_TIME_NS);
    SVC_CONSOLE_Printf("\r\n");

    /* Configure PWM output pins */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 2;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&pin0, &gpio_cfg);

    gpio_cfg.pin = 1;
    DRV_GPIO_Init(&pin1, &gpio_cfg);

    /* Initialize PWM0 (high-side) */
    pwm_cfg.channel = PWM_CH_0;
    pwm_cfg.clock_source = PWM_CLK_FSYS;
    pwm_cfg.mode = PWM_MODE_COMPLEMENTARY;
    pwm_cfg.alignment = PWM_ALIGN_EDGE;
    pwm_cfg.prescaler = 0;          /* No prescaler for high frequency */
    pwm_cfg.period = PWM_PERIOD;
    pwm_cfg.duty = (uint16_t)PWM_PERIOD * duty / 100;
    pwm_cfg.invert = 0;
    pwm_cfg.dead_time = dead_time;

    if (DRV_PWM_Init(&g_pwm0, &pwm_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("PWM0 init failed!\r\n");
        while (1);
    }

    /* Initialize PWM1 (low-side, auto-configured in complementary mode) */
    pwm_cfg.channel = PWM_CH_1;
    pwm_cfg.invert = 1;             /* Inverted output */

    if (DRV_PWM_Init(&g_pwm1, &pwm_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("PWM1 init failed!\r\n");
        while (1);
    }

    /* Start PWM */
    DRV_PWM_Start(&g_pwm0);
    DRV_PWM_Start(&g_pwm1);

    SVC_CONSOLE_Printf("Complementary PWM started.\r\n");
    SVC_CONSOLE_Printf("Initial duty: %u%%\r\n\r\n", duty);
    SVC_CONSOLE_Printf("Controls:\r\n");
    SVC_CONSOLE_Printf("  '+'/'-': Adjust duty +/-5%%\r\n");
    SVC_CONSOLE_Printf("  'd'/'D': Adjust dead-time\r\n");
    SVC_CONSOLE_Printf("  's': Stop outputs\r\n");
    SVC_CONSOLE_Printf("  'g': Go (start outputs)\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Main loop */
    while (1) {
        int ch = SVC_CONSOLE_GetChar(0);

        switch (ch) {
            case '+':
            case '=':
                if (duty < 95) {
                    duty += 5;
                    DRV_PWM_SetDutyPercent(&g_pwm0, duty);
                    DRV_PWM_SetDutyPercent(&g_pwm1, duty);
                    SVC_CONSOLE_Printf("Duty: %u%%\r\n", duty);
                }
                break;

            case '-':
            case '_':
                if (duty > 5) {
                    duty -= 5;
                    DRV_PWM_SetDutyPercent(&g_pwm0, duty);
                    DRV_PWM_SetDutyPercent(&g_pwm1, duty);
                    SVC_CONSOLE_Printf("Duty: %u%%\r\n", duty);
                }
                break;

            case 'D':
                /* Increase dead-time */
                if (dead_time < 64) {
                    dead_time += 8;
                    DRV_PWM_SetDeadTime(&g_pwm0, dead_time);
                    SVC_CONSOLE_Printf("Dead-time: %u clocks\r\n", dead_time);
                }
                break;

            case 'd':
                /* Decrease dead-time */
                if (dead_time > 8) {
                    dead_time -= 8;
                    DRV_PWM_SetDeadTime(&g_pwm0, dead_time);
                    SVC_CONSOLE_Printf("Dead-time: %u clocks\r\n", dead_time);
                }
                break;

            case 's':
            case 'S':
                DRV_PWM_Stop(&g_pwm0);
                DRV_PWM_Stop(&g_pwm1);
                SVC_CONSOLE_Printf("PWM outputs stopped.\r\n");
                break;

            case 'g':
            case 'G':
                DRV_PWM_Start(&g_pwm0);
                DRV_PWM_Start(&g_pwm1);
                SVC_CONSOLE_Printf("PWM outputs started.\r\n");
                break;
        }

        SVC_DELAY_Ms(10);
    }
}
