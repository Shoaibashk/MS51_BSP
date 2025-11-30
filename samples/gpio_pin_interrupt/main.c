/*===========================================================================
 * @file    main.c
 * @brief   GPIO Pin Interrupt Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates GPIO pin interrupt functionality.
 *   - P1.3: Button with falling edge interrupt
 *   - P3.2: Button with both edge interrupt
 *   - P1.0: LED toggled on interrupt
 *   Device enters power-down between button presses.
 * 
 * Hardware:
 *   - LED on P1.0
 *   - Button on P1.3 (pull-up enabled)
 *   - Button on P3.2 (pull-down enabled)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"
#include "svc_power.h"

/*---------------------------------------------------------------------------
 * Pin Definitions
 *---------------------------------------------------------------------------*/
#define LED_PORT        GPIO_PORT_1
#define LED_PIN         0

#define BTN1_PORT       GPIO_PORT_1
#define BTN1_PIN        3

#define BTN2_PORT       GPIO_PORT_3
#define BTN2_PIN        2

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_led;
static gpio_handle_t g_btn1;
static gpio_handle_t g_btn2;

static volatile uint8_t g_int_flag = 0;
static volatile uint8_t g_int_source = 0;

/*---------------------------------------------------------------------------
 * Interrupt Callbacks
 *---------------------------------------------------------------------------*/
static void button1_callback(void *context)
{
    (void)context;  /* Unused */
    g_int_flag = 1;
    g_int_source = 1;
}

static void button2_callback(void *context)
{
    (void)context;  /* Unused */
    g_int_flag = 1;
    g_int_source = 2;
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    power_config_t pwr_cfg;
    uint32_t int_count = 0;

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

    /* Initialize power management */
    pwr_cfg.default_mode = POWER_MODE_RUN;
    pwr_cfg.wakeup_mask = WAKEUP_SRC_GPIO;
    pwr_cfg.bod_enable = 0;
    pwr_cfg.wkt_enable = 0;
    pwr_cfg.wkt_period_ms = 0;
    SVC_POWER_Init(&pwr_cfg);

    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  GPIO Pin Interrupt Demo\r\n");
    SVC_CONSOLE_Printf("  BTN1: P1.3 (falling edge)\r\n");
    SVC_CONSOLE_Printf("  BTN2: P3.2 (both edges)\r\n");
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
    DRV_GPIO_Write(&g_led, 1);  /* LED off */

    /* Configure Button 1 with falling edge interrupt */
    gpio_cfg.port = BTN1_PORT;
    gpio_cfg.pin = BTN1_PIN;
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_cfg.pull = GPIO_PULL_UP;
    gpio_cfg.int_edge = GPIO_INT_FALLING;
    gpio_cfg.callback = button1_callback;
    DRV_GPIO_Init(&g_btn1, &gpio_cfg);
    DRV_GPIO_IntEnable(&g_btn1);

    /* Configure Button 2 with both edge interrupt */
    gpio_cfg.port = BTN2_PORT;
    gpio_cfg.pin = BTN2_PIN;
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_cfg.pull = GPIO_PULL_NONE;  /* External pull-down */
    gpio_cfg.int_edge = GPIO_INT_BOTH;
    gpio_cfg.callback = button2_callback;
    DRV_GPIO_Init(&g_btn2, &gpio_cfg);
    DRV_GPIO_IntEnable(&g_btn2);

    /* Enable global interrupts */
    SVC_SYSTEM_EnableInterrupts();

    SVC_CONSOLE_Printf("Press buttons to trigger interrupts...\r\n");
    SVC_CONSOLE_Printf("Device will enter power-down between presses.\r\n");

    /* Main loop */
    while (1) {
        /* Flush console before sleep */
        SVC_CONSOLE_Flush();

        /* Enter power-down mode - GPIO interrupt will wake us */
        SVC_POWER_EnterPowerDown();

        /* Woke up - check interrupt flag */
        if (g_int_flag) {
            g_int_flag = 0;
            int_count++;

            /* Toggle LED */
            DRV_GPIO_Toggle(&g_led);

            /* Print interrupt source */
            if (g_int_source == 1) {
                SVC_CONSOLE_Printf("[%lu] BTN1 (P1.3) interrupt!\r\n", int_count);
            } else if (g_int_source == 2) {
                uint8_t state = DRV_GPIO_Read(&g_btn2);
                SVC_CONSOLE_Printf("[%lu] BTN2 (P3.2) %s edge!\r\n", 
                    int_count, state ? "rising" : "falling");
            }

            /* Small delay for debounce */
            SVC_DELAY_Ms(50);
        }
    }
}
