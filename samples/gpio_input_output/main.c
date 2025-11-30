/*===========================================================================
 * @file    main.c
 * @brief   GPIO Input/Output Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates GPIO configuration for input and output modes.
 *   - P1.0: LED output (push-pull)
 *   - P3.0: Button input with pull-up
 *   - LED toggles when button is pressed
 * 
 * Hardware:
 *   - LED on P1.0 (active low)
 *   - Button on P3.0 (active low with internal pull-up)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Pin Definitions
 *---------------------------------------------------------------------------*/
#define LED_PORT        GPIO_PORT_1
#define LED_PIN         0

#define BTN_PORT        GPIO_PORT_3
#define BTN_PIN         0

#define DEBOUNCE_MS     50

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static gpio_handle_t g_button;

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint8_t btn_state, btn_last = 1;
    uint32_t press_count = 0;

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
    SVC_CONSOLE_Printf("  GPIO Input/Output Demo\r\n");
    SVC_CONSOLE_Printf("  LED: P1.0, Button: P3.0\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure LED as push-pull output */
    gpio_cfg.port = LED_PORT;
    gpio_cfg.pin = LED_PIN;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;    /* LED off initially */
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &gpio_cfg);

    /* Configure button as input with pull-up */
    gpio_cfg.port = BTN_PORT;
    gpio_cfg.pin = BTN_PIN;
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_cfg.pull = GPIO_PULL_UP;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 0;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_button, &gpio_cfg);

    SVC_CONSOLE_Printf("Press button to toggle LED...\r\n");

    /* Main loop - poll button and toggle LED */
    while (1) {
        /* Read button state */
        btn_state = DRV_GPIO_Read(&g_button);

        /* Detect falling edge (button pressed) */
        if (btn_state == 0 && btn_last == 1) {
            /* Debounce */
            SVC_DELAY_Ms(DEBOUNCE_MS);
            btn_state = DRV_GPIO_Read(&g_button);
            
            if (btn_state == 0) {
                /* Toggle LED */
                DRV_GPIO_Toggle(&g_led);
                press_count++;
                SVC_CONSOLE_Printf("Button pressed! Count: %lu\r\n", press_count);
            }
        }

        btn_last = btn_state;
        SVC_DELAY_Ms(10);
    }
}
