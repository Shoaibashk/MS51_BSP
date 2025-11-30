/*===========================================================================
 * @file    main.c
 * @brief   ADC Compare Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates ADC hardware compare (window watchdog) functionality.
 *   Generates interrupt when ADC value goes above/below thresholds.
 *   Useful for monitoring voltage levels with minimal CPU overhead.
 * 
 * Hardware:
 *   - Potentiometer on P1.7 (AIN7)
 *   - LED on P1.0 (indicates threshold exceeded)
 *   - UART0 for debug output
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_adc.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define ADC_CHANNEL         ADC_CH_7    /* P1.7 */
#define VDD_MV              3300

/* Threshold values (12-bit: 0-4095) */
#define THRESHOLD_LOW       1024        /* ~0.825V at 3.3V VDD */
#define THRESHOLD_HIGH      3072        /* ~2.475V at 3.3V VDD */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static adc_handle_t g_adc;
static gpio_handle_t g_led;

static volatile uint8_t g_compare_flag = 0;
static volatile uint8_t g_above_high = 0;
static volatile uint8_t g_below_low = 0;
static volatile uint32_t g_compare_count = 0;

/*---------------------------------------------------------------------------
 * ADC Compare Callback
 *---------------------------------------------------------------------------*/
static void adc_compare_callback(uint16_t result)
{
    g_compare_flag = 1;
    g_compare_count++;
    
    if (result >= THRESHOLD_HIGH) {
        g_above_high = 1;
        g_below_low = 0;
    } else if (result <= THRESHOLD_LOW) {
        g_below_low = 1;
        g_above_high = 0;
    }
}

/*---------------------------------------------------------------------------
 * Convert ADC to mV
 *---------------------------------------------------------------------------*/
static uint16_t adc_to_mv(uint16_t adc_value)
{
    return (uint32_t)adc_value * VDD_MV / 4096;
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    gpio_handle_t adc_pin;
    adc_config_t adc_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint16_t adc_value, voltage;
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
    SVC_CONSOLE_Printf("  ADC Hardware Compare Demo\r\n");
    SVC_CONSOLE_Printf("  Channel: AIN7 (P1.7)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("Thresholds:\r\n");
    SVC_CONSOLE_Printf("  Low:  %u (%u mV)\r\n", THRESHOLD_LOW, adc_to_mv(THRESHOLD_LOW));
    SVC_CONSOLE_Printf("  High: %u (%u mV)\r\n", THRESHOLD_HIGH, adc_to_mv(THRESHOLD_HIGH));
    SVC_CONSOLE_Printf("\r\n");

    /* Configure LED */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 0;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &gpio_cfg);
    DRV_GPIO_Write(&g_led, 1);  /* LED off */

    /* Configure ADC input pin */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 7;
    gpio_cfg.mode = GPIO_MODE_INPUT_ONLY;
    gpio_cfg.pull = GPIO_PULL_NONE;
    DRV_GPIO_Init(&adc_pin, &gpio_cfg);

    /* Disable digital input */
    AINDIDS |= 0x80;

    /* Initialize ADC */
    adc_cfg.channel = ADC_CHANNEL;
    adc_cfg.trigger = ADC_TRIG_SOFTWARE;
    adc_cfg.alignment = ADC_ALIGN_RIGHT;
    adc_cfg.int_enable = 1;
    adc_cfg.callback = adc_compare_callback;

    if (DRV_ADC_Init(&g_adc, &adc_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("ADC init failed!\r\n");
        while (1);
    }

    /* Enable hardware compare */
    DRV_ADC_EnableCompare(&g_adc, THRESHOLD_HIGH, THRESHOLD_LOW);

    /* Enable global interrupts */
    SVC_SYSTEM_EnableInterrupts();

    SVC_CONSOLE_Printf("Adjust voltage on P1.7...\r\n");
    SVC_CONSOLE_Printf("LED will light when outside thresholds.\r\n\r\n");

    /* Main loop */
    while (1) {
        uint32_t now = SVC_DELAY_GetTicks();

        /* Read current ADC value */
        adc_value = DRV_ADC_ReadBlocking(&g_adc, ADC_CHANNEL);
        voltage = adc_to_mv(adc_value);

        /* Check compare flag */
        if (g_compare_flag) {
            g_compare_flag = 0;

            if (g_above_high) {
                DRV_GPIO_Write(&g_led, 0);  /* LED on */
                SVC_CONSOLE_Printf("*** HIGH threshold exceeded! ***\r\n");
            } else if (g_below_low) {
                DRV_GPIO_Write(&g_led, 0);  /* LED on */
                SVC_CONSOLE_Printf("*** LOW threshold exceeded! ***\r\n");
            }
        } else {
            /* Within normal range */
            DRV_GPIO_Write(&g_led, 1);  /* LED off */
        }

        /* Print status every 500ms */
        if (now - last_print >= 500) {
            last_print = now;

            const char *status;
            if (adc_value >= THRESHOLD_HIGH) {
                status = "ABOVE HIGH";
            } else if (adc_value <= THRESHOLD_LOW) {
                status = "BELOW LOW";
            } else {
                status = "NORMAL";
            }

            SVC_CONSOLE_Printf("ADC: %4u (%4u mV) [%s] Events: %lu\r\n",
                adc_value, voltage, status, g_compare_count);
        }

        SVC_DELAY_Ms(50);
    }
}
