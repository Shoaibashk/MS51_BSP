/*===========================================================================
 * @file    main.c
 * @brief   Watchdog Reset Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates Watchdog Timer in reset mode.
 *   LED blinks with regular WDT feeds, then simulates a hang.
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

/* WDT timeout period */
#define WDT_TIMEOUT_MS  1600    /* ~1.6 seconds */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static wdt_handle_t g_wdt;

/*---------------------------------------------------------------------------
 * LED Control
 *---------------------------------------------------------------------------*/
static void led_on(void)  { DRV_GPIO_Write(&g_led, 0); }
static void led_off(void) { DRV_GPIO_Write(&g_led, 1); }
static void led_toggle(void) { DRV_GPIO_Toggle(&g_led); }

/*---------------------------------------------------------------------------
 * Check Reset Source
 *---------------------------------------------------------------------------*/
static void check_reset_source(void)
{
    uint8_t reset_src;

    /* Read and display reset source */
    reset_src = SVC_SYSTEM_GetResetSource();

    SVC_CONSOLE_Printf("Reset Source: ");
    if (reset_src & RESET_SRC_POR) {
        SVC_CONSOLE_Printf("Power-On ");
    }
    if (reset_src & RESET_SRC_WDT) {
        SVC_CONSOLE_Printf("Watchdog ");
    }
    if (reset_src & RESET_SRC_EXT) {
        SVC_CONSOLE_Printf("External ");
    }
    if (reset_src & RESET_SRC_BOD) {
        SVC_CONSOLE_Printf("Brown-Out ");
    }
    if (reset_src & RESET_SRC_SW) {
        SVC_CONSOLE_Printf("Software ");
    }
    SVC_CONSOLE_Printf("\r\n");

    /* Clear reset flags */
    SVC_SYSTEM_ClearResetSource();
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
    uint8_t blink_count = 0;

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
    SVC_CONSOLE_Printf("  Watchdog Reset Demo\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Check what caused this reset */
    check_reset_source();

    /* Configure LED */
    gpio_cfg.port = LED_PORT;
    gpio_cfg.pin = LED_PIN;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &gpio_cfg);
    led_off();

    /* Configure WDT in reset mode */
    wdt_cfg.timeout_ms = WDT_TIMEOUT_MS;
    wdt_cfg.mode = WDT_MODE_RESET;
    wdt_cfg.int_enable = 0;
    wdt_cfg.callback = NULL;
    DRV_WDT_Init(&g_wdt, &wdt_cfg);

    SVC_CONSOLE_Printf("\r\nWDT configured: %u ms timeout\r\n", WDT_TIMEOUT_MS);
    SVC_CONSOLE_Printf("LED will blink 10 times, then stop feeding WDT...\r\n");

    /* Enable WDT */
    DRV_WDT_Enable(&g_wdt);

    while (1) {
        /* Blink LED */
        led_toggle();
        SVC_DELAY_Ms(200);

        blink_count++;

        if (blink_count < 20) {
            /* Normal operation: feed watchdog */
            DRV_WDT_Feed(&g_wdt);
            
            if ((blink_count & 1) == 0) {
                SVC_CONSOLE_Printf("Blink %u - WDT fed\r\n", blink_count / 2);
            }
        } else if (blink_count == 20) {
            SVC_CONSOLE_Printf("\r\n*** SIMULATING HANG ***\r\n");
            SVC_CONSOLE_Printf("Stopping WDT feed...\r\n");
            SVC_CONSOLE_Printf("System will reset in ~%u ms\r\n", WDT_TIMEOUT_MS);
        } else {
            /* Stop feeding - WDT will reset system */
            SVC_CONSOLE_Printf(".");
        }
    }
}
