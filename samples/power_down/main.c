/*===========================================================================
 * @file    main.c
 * @brief   Power Down Mode with Wakeup Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates power management features:
 *   - Power-Down mode entry
 *   - Wakeup via external interrupt (button)
 *   - Wakeup via WDT interrupt
 *   - Idle mode for moderate power saving
 * 
 * Hardware:
 *   - P1.4: LED (active low)
 *   - P3.0: Button (active low, external pull-up)
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
#include "svc_power.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define LED_PORT        GPIO_PORT_1
#define LED_PIN         4

#define BUTTON_PORT     GPIO_PORT_3
#define BUTTON_PIN      0

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static gpio_handle_t g_button;
static volatile uint8_t g_wakeup_source = 0;

/*---------------------------------------------------------------------------
 * Wakeup Source Flags
 *---------------------------------------------------------------------------*/
#define WAKEUP_BUTTON   0x01
#define WAKEUP_WDT      0x02

/*---------------------------------------------------------------------------
 * LED Control
 *---------------------------------------------------------------------------*/
static void led_on(void)  { DRV_GPIO_Write(&g_led, 0); }
static void led_off(void) { DRV_GPIO_Write(&g_led, 1); }

/*---------------------------------------------------------------------------
 * Button Interrupt Callback (Wakeup)
 *---------------------------------------------------------------------------*/
static void button_callback(void *context)
{
    (void)context;
    g_wakeup_source |= WAKEUP_BUTTON;
}

/*---------------------------------------------------------------------------
 * WDT Interrupt Callback (Wakeup)
 *---------------------------------------------------------------------------*/
static void wdt_wakeup_callback(void *context)
{
    (void)context;
    g_wakeup_source |= WAKEUP_WDT;
}

/*---------------------------------------------------------------------------
 * Blink LED Pattern
 *---------------------------------------------------------------------------*/
static void blink_pattern(uint8_t count, uint16_t on_ms, uint16_t off_ms)
{
    uint8_t i;
    for (i = 0; i < count; i++) {
        led_on();
        SVC_DELAY_Ms(on_ms);
        led_off();
        SVC_DELAY_Ms(off_ms);
    }
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
    power_config_t pwr_cfg;
    wdt_handle_t wdt;
    uint32_t wakeup_count = 0;
    char cmd;

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

    /* Initialize power service */
    pwr_cfg.bod_enable = 0;
    pwr_cfg.lvr_enable = 1;
    pwr_cfg.wakeup_sources = PWR_WAKEUP_PIN | PWR_WAKEUP_WDT;
    SVC_POWER_Init(&pwr_cfg);

    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  Power-Down Mode Demo\r\n");
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
    led_off();

    /* Configure button with interrupt for wakeup */
    gpio_cfg.port = BUTTON_PORT;
    gpio_cfg.pin = BUTTON_PIN;
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_cfg.pull = GPIO_PULL_UP;
    gpio_cfg.int_edge = GPIO_INT_FALLING;
    gpio_cfg.callback = button_callback;
    DRV_GPIO_Init(&g_button, &gpio_cfg);
    DRV_GPIO_IntEnable(&g_button);

    /* Configure WDT for periodic wakeup (used in some modes) */
    wdt_cfg.timeout_ms = 3200;      /* ~3.2 seconds */
    wdt_cfg.mode = WDT_MODE_INTERRUPT;
    wdt_cfg.int_enable = 1;
    wdt_cfg.callback = wdt_wakeup_callback;
    DRV_WDT_Init(&wdt, &wdt_cfg);

    /* Enable global interrupts */
    HAL_INT_Enable();

    SVC_CONSOLE_Printf("\r\nWakeup sources:\r\n");
    SVC_CONSOLE_Printf("  - Button on P3.0 (INT0)\r\n");
    SVC_CONSOLE_Printf("  - WDT interrupt (3.2s period)\r\n");
    SVC_CONSOLE_Printf("\r\nCommands:\r\n");
    SVC_CONSOLE_Printf("  'p' - Enter Power-Down mode\r\n");
    SVC_CONSOLE_Printf("  'i' - Enter Idle mode\r\n");
    SVC_CONSOLE_Printf("  'w' - Enable WDT wakeup\r\n");
    SVC_CONSOLE_Printf("  'b' - Blink LED\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Initial blink to show we're running */
    blink_pattern(3, 100, 100);

    while (1) {
        /* Check for wakeup events */
        if (g_wakeup_source) {
            wakeup_count++;
            
            SVC_CONSOLE_Printf("\r\n*** WAKEUP #%lu ***\r\n", wakeup_count);
            
            if (g_wakeup_source & WAKEUP_BUTTON) {
                SVC_CONSOLE_Printf("Source: Button\r\n");
                blink_pattern(2, 50, 50);
            }
            if (g_wakeup_source & WAKEUP_WDT) {
                SVC_CONSOLE_Printf("Source: WDT\r\n");
                blink_pattern(3, 50, 50);
                DRV_WDT_Feed(&wdt);
                DRV_WDT_ClearInt(&wdt);
            }
            
            g_wakeup_source = 0;
        }

        /* Check for commands */
        if (SVC_CONSOLE_Available()) {
            cmd = SVC_CONSOLE_Getc();

            switch (cmd) {
                case 'p':
                case 'P':
                    SVC_CONSOLE_Printf("Entering Power-Down mode...\r\n");
                    SVC_CONSOLE_Printf("Press button to wake up.\r\n");
                    SVC_DELAY_Ms(100);  /* Allow UART to finish */
                    
                    /* Enter power-down */
                    SVC_POWER_EnterPowerDown();
                    
                    /* We wake up here */
                    SVC_CONSOLE_Printf("Awake!\r\n");
                    break;

                case 'i':
                case 'I':
                    SVC_CONSOLE_Printf("Entering Idle mode...\r\n");
                    SVC_CONSOLE_Printf("Any interrupt will wake up.\r\n");
                    SVC_DELAY_Ms(100);
                    
                    /* Enter idle */
                    SVC_POWER_EnterIdle();
                    
                    SVC_CONSOLE_Printf("Awake!\r\n");
                    break;

                case 'w':
                case 'W':
                    SVC_CONSOLE_Printf("Enabling WDT wakeup...\r\n");
                    DRV_WDT_Enable(&wdt);
                    SVC_CONSOLE_Printf("WDT will wake from Power-Down every ~3.2s\r\n");
                    break;

                case 'b':
                case 'B':
                    blink_pattern(5, 100, 100);
                    break;

                default:
                    break;
            }
        }

        SVC_DELAY_Ms(10);
    }
}
