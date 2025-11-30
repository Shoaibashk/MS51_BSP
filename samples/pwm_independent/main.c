/*===========================================================================
 * @file    main.c
 * @brief   PWM Independent Mode Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates PWM independent mode with multiple channels.
 *   Each PWM channel operates independently with different duty cycles.
 * 
 * Hardware:
 *   - PWM0 on P1.2 (10% duty)
 *   - PWM2 on P1.0 (30% duty)
 *   - PWM3 on P0.0 (50% duty)
 *   - PWM4 on P0.1 (70% duty)
 *   - Connect LEDs or oscilloscope to observe outputs
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
#define PWM_PERIOD      1000    /* PWM period (counts) */
#define PWM_FREQ_HZ     1000    /* Target 1 kHz */

/*---------------------------------------------------------------------------
 * PWM Channel Configuration
 *---------------------------------------------------------------------------*/
static const struct {
    pwm_channel_t channel;
    uint8_t port;
    uint8_t pin;
    uint8_t duty_percent;
    const char *name;
} g_pwm_channels[] = {
    { PWM_CH_0, GPIO_PORT_1, 2, 10, "PWM0 (P1.2)" },
    { PWM_CH_2, GPIO_PORT_1, 0, 30, "PWM2 (P1.0)" },
    { PWM_CH_3, GPIO_PORT_0, 0, 50, "PWM3 (P0.0)" },
    { PWM_CH_4, GPIO_PORT_0, 1, 70, "PWM4 (P0.1)" },
};

#define NUM_PWM_CHANNELS (sizeof(g_pwm_channels) / sizeof(g_pwm_channels[0]))

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static pwm_handle_t g_pwm[NUM_PWM_CHANNELS];

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    pwm_config_t pwm_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint8_t i;
    uint8_t duty_offset = 0;

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
    SVC_CONSOLE_Printf("  PWM Independent Mode Demo\r\n");
    SVC_CONSOLE_Printf("  Frequency: %u Hz\r\n", PWM_FREQ_HZ);
    SVC_CONSOLE_Printf("  Period: %u counts\r\n", PWM_PERIOD);
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure PWM output pins */
    for (i = 0; i < NUM_PWM_CHANNELS; i++) {
        gpio_cfg.port = g_pwm_channels[i].port;
        gpio_cfg.pin = g_pwm_channels[i].pin;
        gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
        gpio_cfg.pull = GPIO_PULL_NONE;
        gpio_cfg.int_edge = GPIO_INT_DISABLE;
        gpio_cfg.init_value = 1;
        gpio_cfg.callback = NULL;
        
        gpio_handle_t pin;
        DRV_GPIO_Init(&pin, &gpio_cfg);
    }

    /* Initialize each PWM channel */
    for (i = 0; i < NUM_PWM_CHANNELS; i++) {
        pwm_cfg.channel = g_pwm_channels[i].channel;
        pwm_cfg.clock_source = PWM_CLK_FSYS;
        pwm_cfg.mode = PWM_MODE_INDEPENDENT;
        pwm_cfg.alignment = PWM_ALIGN_EDGE;
        pwm_cfg.prescaler = 23;     /* FSYS/24 = 1MHz */
        pwm_cfg.period = PWM_PERIOD;
        pwm_cfg.duty = (uint16_t)PWM_PERIOD * g_pwm_channels[i].duty_percent / 100;
        pwm_cfg.invert = 0;
        pwm_cfg.dead_time = 0;

        if (DRV_PWM_Init(&g_pwm[i], &pwm_cfg) != DRV_OK) {
            SVC_CONSOLE_Printf("PWM%u init failed!\r\n", i);
            while (1);
        }

        SVC_CONSOLE_Printf("  %s: %u%% duty\r\n", 
            g_pwm_channels[i].name, g_pwm_channels[i].duty_percent);
    }

    /* Start all PWM channels */
    for (i = 0; i < NUM_PWM_CHANNELS; i++) {
        DRV_PWM_Start(&g_pwm[i]);
    }

    SVC_CONSOLE_Printf("\r\nPWM outputs started.\r\n");
    SVC_CONSOLE_Printf("Press '+'/'-' to adjust duty, 'r' to reset.\r\n\r\n");

    /* Main loop - allow interactive duty adjustment */
    while (1) {
        int ch = SVC_CONSOLE_GetChar(0);

        if (ch == '+' || ch == '=') {
            /* Increase duty by 10% */
            if (duty_offset < 90) {
                duty_offset += 10;
            }
            SVC_CONSOLE_Printf("Duty offset: +%u%%\r\n", duty_offset);

            /* Update all channels */
            for (i = 0; i < NUM_PWM_CHANNELS; i++) {
                uint8_t new_duty = g_pwm_channels[i].duty_percent + duty_offset;
                if (new_duty > 100) new_duty = 100;
                DRV_PWM_SetDutyPercent(&g_pwm[i], new_duty);
                SVC_CONSOLE_Printf("  %s: %u%%\r\n", g_pwm_channels[i].name, new_duty);
            }
        } else if (ch == '-' || ch == '_') {
            /* Decrease duty by 10% */
            if (duty_offset >= 10) {
                duty_offset -= 10;
            } else if (duty_offset > 0) {
                duty_offset = 0;
            }
            SVC_CONSOLE_Printf("Duty offset: +%u%%\r\n", duty_offset);

            /* Update all channels */
            for (i = 0; i < NUM_PWM_CHANNELS; i++) {
                uint8_t new_duty = g_pwm_channels[i].duty_percent + duty_offset;
                if (new_duty > 100) new_duty = 100;
                DRV_PWM_SetDutyPercent(&g_pwm[i], new_duty);
                SVC_CONSOLE_Printf("  %s: %u%%\r\n", g_pwm_channels[i].name, new_duty);
            }
        } else if (ch == 'r' || ch == 'R') {
            /* Reset to original duty */
            duty_offset = 0;
            SVC_CONSOLE_Printf("Reset to original duty cycles.\r\n");

            for (i = 0; i < NUM_PWM_CHANNELS; i++) {
                DRV_PWM_SetDutyPercent(&g_pwm[i], g_pwm_channels[i].duty_percent);
                SVC_CONSOLE_Printf("  %s: %u%%\r\n", 
                    g_pwm_channels[i].name, g_pwm_channels[i].duty_percent);
            }
        }

        SVC_DELAY_Ms(10);
    }
}
