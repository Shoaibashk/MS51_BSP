/*===========================================================================
 * @file    main.c
 * @brief   Blink LED Sample - Using GPIO Driver
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   This sample demonstrates basic GPIO usage with the new driver layer.
 *   An LED connected to P1.0 blinks at 1 Hz (500ms on, 500ms off).
 * 
 * Hardware:
 *   - LED connected to P1.0 (active low)
 *   - Any MS51 device
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"
#include "svc_system.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define LED_PORT    GPIO_PORT_1
#define LED_PIN     0
#define BLINK_MS    500

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t led_cfg;
    system_config_t sys_cfg;
    drv_status_t status;

    /* Initialize system */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Configure LED GPIO */
    led_cfg.port = LED_PORT;
    led_cfg.pin = LED_PIN;
    led_cfg.mode = GPIO_MODE_PUSH_PULL;
    led_cfg.pull = GPIO_PULL_NONE;
    led_cfg.int_edge = GPIO_INT_DISABLE;
    led_cfg.init_value = 1;  /* LED off initially */
    led_cfg.callback = NULL;

    status = DRV_GPIO_Init(&g_led, &led_cfg);
    if (status != DRV_OK) {
        /* Error handling - in real app, indicate error somehow */
        while (1);
    }

    /* Main loop - blink LED forever */
    while (1) {
        DRV_GPIO_Write(&g_led, 0);  /* LED on (active low) */
        SVC_DELAY_Ms(BLINK_MS);

        DRV_GPIO_Write(&g_led, 1);  /* LED off */
        SVC_DELAY_Ms(BLINK_MS);
    }
}
