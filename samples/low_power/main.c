/*===========================================================================
 * @file    main.c
 * @brief   Low Power Sample - Using Power Service
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   This sample demonstrates low-power modes with periodic wake-up.
 *   The device enters Power-Down mode and wakes every 2 seconds via WKT.
 *   An LED blinks briefly on each wake-up to indicate activity.
 * 
 * Hardware:
 *   - LED connected to P1.0 (active low)
 *   - UART0 for debug output (optional)
 *   - Any MS51 device
 * 
 * Power Consumption:
 *   - Active (24MHz): ~4.5 mA
 *   - Power-Down: ~1 µA
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"
#include "svc_console.h"
#include "svc_system.h"
#include "svc_power.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define LED_PORT        GPIO_PORT_1
#define LED_PIN         0
#define WAKEUP_PERIOD_MS    2000    /* Wake every 2 seconds */
#define LED_FLASH_MS        50      /* LED on time each wake */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static uint32_t g_wakeup_count = 0;

/*---------------------------------------------------------------------------
 * Function Prototypes
 *---------------------------------------------------------------------------*/
static void led_flash(void);

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    console_config_t con_cfg;
    power_config_t pwr_cfg;
    gpio_config_t led_cfg;
    drv_status_t status;
    wakeup_source_t wakeup_src;

    /* Initialize system */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Initialize console (optional - comment out to save power) */
    con_cfg.port = UART_PORT_0;
    con_cfg.baudrate = 115200;
    con_cfg.echo_enable = 0;
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    /* Configure LED GPIO */
    led_cfg.port = LED_PORT;
    led_cfg.pin = LED_PIN;
    led_cfg.mode = GPIO_MODE_PUSH_PULL;
    led_cfg.pull = GPIO_PULL_NONE;
    led_cfg.int_edge = GPIO_INT_DISABLE;
    led_cfg.init_value = 1;
    led_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &led_cfg);
    DRV_GPIO_Write(&g_led, 1);  /* LED off initially */

    /* Configure power management */
    pwr_cfg.default_mode = POWER_MODE_RUN;
    pwr_cfg.wakeup_mask = WAKEUP_SRC_WKT;
    pwr_cfg.bod_enable = 0;     /* Disable BOD to save power */
    pwr_cfg.wkt_enable = 1;
    pwr_cfg.wkt_period_ms = WAKEUP_PERIOD_MS;
    
    status = SVC_POWER_Init(&pwr_cfg);
    if (status != DRV_OK) {
        SVC_CONSOLE_Printf("Power init failed: %d\r\n", status);
        while (1);
    }

    /* Print startup message */
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  MS51 Low Power Demo\r\n");
    SVC_CONSOLE_Printf("  Wake-up period: %u ms\r\n", WAKEUP_PERIOD_MS);
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Check reset source */
    if (SVC_POWER_IsPOR()) {
        SVC_CONSOLE_Printf("Power-on reset detected\r\n");
        SVC_POWER_ClearPORFlag();
    }

    SVC_CONSOLE_Printf("Entering low-power loop...\r\n");
    SVC_CONSOLE_Printf("LED will flash every %u seconds\r\n", 
        WAKEUP_PERIOD_MS / 1000);
    SVC_CONSOLE_Printf("\r\n");

    /* Configure WKT */
    SVC_POWER_WKT_Configure(WAKEUP_PERIOD_MS);
    SVC_POWER_WKT_Start();

    /* Main low-power loop */
    while (1) {
        /* Flush UART before sleep */
        SVC_CONSOLE_Flush();

        /* Enter Power-Down mode */
        SVC_POWER_EnterPowerDown();

        /* --- Device is now in Power-Down mode --- */
        /* --- Wake-up occurs here --- */

        /* Check wake-up source */
        wakeup_src = SVC_POWER_GetWakeupSource();
        g_wakeup_count++;

        if (wakeup_src & WAKEUP_SRC_WKT) {
            /* WKT wake-up - do periodic task */
            led_flash();

            /* Print wake-up info (uses power, comment out in production) */
            SVC_CONSOLE_Printf("Wake #%lu (WKT)\r\n", g_wakeup_count);
        }

        /* Other wake-up sources can be handled here */
        if (wakeup_src & WAKEUP_SRC_GPIO) {
            SVC_CONSOLE_Printf("Wake #%lu (GPIO)\r\n", g_wakeup_count);
        }
    }
}

/*---------------------------------------------------------------------------
 * Flash LED briefly
 *---------------------------------------------------------------------------*/
static void led_flash(void)
{
    DRV_GPIO_Write(&g_led, 0);  /* LED on */
    SVC_DELAY_Ms(LED_FLASH_MS);
    DRV_GPIO_Write(&g_led, 1);  /* LED off */
}
