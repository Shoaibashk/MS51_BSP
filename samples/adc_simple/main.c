/*===========================================================================
 * @file    main.c
 * @brief   ADC Simple Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates basic ADC single-channel conversion.
 *   Reads analog voltage on P1.7 (AIN7) and displays result.
 * 
 * Hardware:
 *   - Potentiometer or voltage source on P1.7 (0-VDD)
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
#define ADC_CHANNEL     ADC_CH_7    /* P1.7 */
#define SAMPLE_MS       500         /* Sample interval */
#define VDD_MV          3300        /* Assumed VDD in mV */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static adc_handle_t g_adc;

/*---------------------------------------------------------------------------
 * Convert ADC value to millivolts
 *---------------------------------------------------------------------------*/
static uint16_t adc_to_mv(uint16_t adc_value, uint16_t vdd_mv)
{
    return (uint32_t)adc_value * vdd_mv / 4096;
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
    uint32_t sample_count = 0;

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
    SVC_CONSOLE_Printf("  ADC Simple Conversion Demo\r\n");
    SVC_CONSOLE_Printf("  Channel: AIN7 (P1.7)\r\n");
    SVC_CONSOLE_Printf("  Resolution: 12-bit (0-4095)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure P1.7 as analog input */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 7;
    gpio_cfg.mode = GPIO_MODE_INPUT_ONLY;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 0;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&adc_pin, &gpio_cfg);

    /* Disable digital input on analog pin */
    AINDIDS |= 0x80;    /* Disable AIN7 digital input */

    /* Initialize ADC */
    adc_cfg.channel = ADC_CHANNEL;
    adc_cfg.trigger = ADC_TRIG_SOFTWARE;
    adc_cfg.alignment = ADC_ALIGN_RIGHT;
    adc_cfg.int_enable = 0;
    adc_cfg.callback = NULL;

    if (DRV_ADC_Init(&g_adc, &adc_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("ADC init failed!\r\n");
        while (1);
    }

    SVC_CONSOLE_Printf("\r\nSampling every %u ms...\r\n", SAMPLE_MS);
    SVC_CONSOLE_Printf("Apply voltage (0 - %u mV) to P1.7\r\n\r\n", VDD_MV);

    SVC_CONSOLE_Printf("Sample | Raw   | Voltage\r\n");
    SVC_CONSOLE_Printf("-------|-------|--------\r\n");

    /* Main loop - continuous sampling */
    while (1) {
        /* Read ADC */
        adc_value = DRV_ADC_ReadBlocking(&g_adc, ADC_CHANNEL);
        
        /* Convert to millivolts */
        voltage = adc_to_mv(adc_value, VDD_MV);

        /* Display result */
        sample_count++;
        SVC_CONSOLE_Printf("%6lu | %4u  | %4u mV\r\n", 
            sample_count, adc_value, voltage);

        /* Wait for next sample */
        SVC_DELAY_Ms(SAMPLE_MS);
    }
}
