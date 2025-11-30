/*===========================================================================
 * @file    main.c
 * @brief   Timer Interrupt Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates timer interrupt functionality using Timer0.
 *   LED toggles at configurable intervals using timer interrupts.
 *   Multiple timers can run simultaneously.
 * 
 * Hardware:
 *   - LED on P1.0
 *   - UART0 for debug output
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_timer.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define LED_PORT        GPIO_PORT_1
#define LED_PIN         0

#define TIMER0_PERIOD_MS    500     /* LED toggle period */
#define TIMER2_PERIOD_MS    100     /* Fast tick for counting */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static timer_handle_t g_timer0;
static timer_handle_t g_timer2;

static volatile uint32_t g_timer0_count = 0;
static volatile uint32_t g_timer2_count = 0;

/*---------------------------------------------------------------------------
 * Timer Callbacks
 *---------------------------------------------------------------------------*/
static void timer0_callback(timer_id_t id)
{
    /* Toggle LED */
    DRV_GPIO_Toggle(&g_led);
    g_timer0_count++;
}

static void timer2_callback(timer_id_t id)
{
    /* Fast counter */
    g_timer2_count++;
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    timer_config_t tim_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint32_t last_print = 0;

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
    SVC_CONSOLE_Printf("  Timer Interrupt Demo\r\n");
    SVC_CONSOLE_Printf("  Timer0: %u ms (LED toggle)\r\n", TIMER0_PERIOD_MS);
    SVC_CONSOLE_Printf("  Timer2: %u ms (fast counter)\r\n", TIMER2_PERIOD_MS);
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure LED as push-pull output */
    gpio_cfg.port = LED_PORT;
    gpio_cfg.pin = LED_PIN;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &gpio_cfg);
    DRV_GPIO_Write(&g_led, 1);  /* LED off initially */

    /* Configure Timer 0 - 500ms period for LED */
    tim_cfg.id = TIMER_0;
    tim_cfg.mode = TIMER_MODE_1;            /* 16-bit timer */
    tim_cfg.clock_source = TIMER_CLK_FSYS_12;
    tim_cfg.period_us = TIMER0_PERIOD_MS * 1000UL;
    tim_cfg.auto_reload = 1;
    tim_cfg.int_enable = 1;
    tim_cfg.callback = timer0_callback;
    
    if (DRV_TIMER_Init(&g_timer0, &tim_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("Timer0 init failed!\r\n");
        while (1);
    }

    /* Configure Timer 2 - 100ms period for counting */
    tim_cfg.id = TIMER_2;
    tim_cfg.mode = TIMER_MODE_1;
    tim_cfg.clock_source = TIMER_CLK_FSYS_12;
    tim_cfg.period_us = TIMER2_PERIOD_MS * 1000UL;
    tim_cfg.auto_reload = 1;
    tim_cfg.int_enable = 1;
    tim_cfg.callback = timer2_callback;

    if (DRV_TIMER_Init(&g_timer2, &tim_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("Timer2 init failed!\r\n");
        while (1);
    }

    /* Enable global interrupts */
    SVC_SYSTEM_EnableInterrupts();

    /* Start timers */
    DRV_TIMER_Start(&g_timer0);
    DRV_TIMER_Start(&g_timer2);

    SVC_CONSOLE_Printf("\r\nTimers started. LED should toggle every %u ms.\r\n", 
        TIMER0_PERIOD_MS);
    SVC_CONSOLE_Printf("Statistics printed every second.\r\n\r\n");

    /* Main loop - print statistics */
    while (1) {
        uint32_t now = SVC_DELAY_GetTicks();

        /* Print every second */
        if (now - last_print >= 1000) {
            last_print = now;

            SVC_CONSOLE_Printf("Uptime: %lu ms | T0 count: %lu | T2 count: %lu\r\n",
                now, g_timer0_count, g_timer2_count);
        }

        SVC_DELAY_Ms(10);
    }
}
