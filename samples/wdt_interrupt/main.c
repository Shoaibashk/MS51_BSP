/*===========================================================================
 * @file    main.c
 * @brief   Watchdog Interrupt Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates Watchdog Timer in interrupt mode.
 *   WDT interrupt toggles LED and can be used for periodic wakeup.
 * 
 * Hardware:
 *   - P1.4: LED (active low)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_wdt.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define LED_PORT    GPIO_PORT_1
#define LED_PIN     4

#define WDT_TIMEOUT_MS  400     /* ~400 ms */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static wdt_handle_t g_wdt;
static volatile uint32_t g_wdt_count = 0;

/*---------------------------------------------------------------------------
 * LED Control
 *---------------------------------------------------------------------------*/
static void led_toggle(void) { DRV_GPIO_Toggle(&g_led); }

/*---------------------------------------------------------------------------
 * WDT Interrupt Callback
 *---------------------------------------------------------------------------*/
static void wdt_callback(void *context)
{
    (void)context;

    /* Toggle LED in ISR */
    led_toggle();

    /* Increment counter */
    g_wdt_count++;

    /* Clear WDT interrupt and restart */
    DRV_WDT_ClearInt(&g_wdt);
    DRV_WDT_Feed(&g_wdt);
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    wdt_config_t wdt_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint32_t last_count = 0;

    /* Initialize system at 24MHz */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Initialize console */
    con_cfg.port = UART_PORT_0;
    con_cfg.baudrate = 115200;
    con_cfg.echo_enable = 0;
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  Watchdog Interrupt Demo\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure LED */
    gpio_cfg.port = LED_PORT;
    gpio_cfg.pin = LED_PIN;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &gpio_cfg);
    DRV_GPIO_Write(&g_led, 1);  /* LED off */

    /* Configure WDT in interrupt mode */
    wdt_cfg.timeout_ms = WDT_TIMEOUT_MS;
    wdt_cfg.mode = WDT_MODE_INTERRUPT;
    wdt_cfg.int_enable = 1;
    wdt_cfg.callback = wdt_callback;
    DRV_WDT_Init(&g_wdt, &wdt_cfg);

    SVC_CONSOLE_Printf("\r\nWDT configured: %u ms period\r\n", WDT_TIMEOUT_MS);
    SVC_CONSOLE_Printf("LED will toggle on each WDT interrupt.\r\n\r\n");

    /* Enable WDT */
    DRV_WDT_Enable(&g_wdt);

    /* Enable global interrupts */
    HAL_INT_Enable();

    while (1) {
        /* Print status when count changes */
        if (g_wdt_count != last_count) {
            SVC_CONSOLE_Printf("WDT interrupt #%lu\r\n", g_wdt_count);
            last_count = g_wdt_count;
        }

        /* Main loop can do other work */
        SVC_DELAY_Ms(100);
    }
}
