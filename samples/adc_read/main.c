/*===========================================================================
 * @file    main.c
 * @brief   ADC Read Sample - Using ADC Driver
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   This sample demonstrates ADC usage to read analog inputs and
 *   calculate supply voltage using the internal bandgap reference.
 * 
 * Hardware:
 *   - Potentiometer or analog sensor on P1.7 (ADC CH7)
 *   - UART0 for debug output
 *   - Any MS51 device with ADC
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_adc.h"
#include "svc_delay.h"
#include "svc_console.h"
#include "svc_system.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define ADC_INPUT_CHANNEL   ADC_CH_7    /* P1.7 */
#define SAMPLE_INTERVAL_MS  500

/* Bandgap reference voltage in mV */
#define BANDGAP_VOLTAGE_MV  1220

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static adc_handle_t g_adc;

/*---------------------------------------------------------------------------
 * Function Prototypes
 *---------------------------------------------------------------------------*/
static uint16_t read_vdd_mv(void);
static uint16_t read_adc_mv(uint16_t vdd_mv);

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    console_config_t con_cfg;
    adc_config_t adc_cfg;
    gpio_config_t gpio_cfg;
    gpio_handle_t adc_pin;
    drv_status_t status;
    uint16_t vdd_mv, adc_mv, adc_raw;
    uint32_t sample_count = 0;

    /* Initialize system */
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

    /* Configure P1.7 as ADC input (input only mode) */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 7;
    gpio_cfg.mode = GPIO_MODE_INPUT_ONLY;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 0;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&adc_pin, &gpio_cfg);

    /* Initialize ADC */
    adc_cfg.channel = ADC_INPUT_CHANNEL;
    adc_cfg.trigger = ADC_TRIG_SOFTWARE;
    adc_cfg.alignment = ADC_ALIGN_RIGHT;
    adc_cfg.int_enable = 0;
    adc_cfg.callback = NULL;

    status = DRV_ADC_Init(&g_adc, &adc_cfg);
    if (status != DRV_OK) {
        SVC_CONSOLE_Printf("ADC init failed: %d\r\n", status);
        while (1);
    }

    /* Print header */
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  MS51 ADC Demo\r\n");
    SVC_CONSOLE_Printf("  Input: P1.7 (ADC CH7)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("Sample |  VDD   |  Raw  | Voltage\r\n");
    SVC_CONSOLE_Printf("-------|--------|-------|----------\r\n");

    /* Main sampling loop */
    while (1) {
        /* Read VDD using bandgap reference */
        vdd_mv = read_vdd_mv();

        /* Read ADC input channel */
        DRV_ADC_SetChannel(&g_adc, ADC_INPUT_CHANNEL);
        adc_raw = DRV_ADC_ReadBlocking(&g_adc, ADC_INPUT_CHANNEL);
        adc_mv = read_adc_mv(vdd_mv);

        /* Print results */
        sample_count++;
        SVC_CONSOLE_Printf("%6lu | %4u mV | %4u | %4u mV\r\n",
            sample_count, vdd_mv, adc_raw, adc_mv);

        /* Wait for next sample */
        SVC_DELAY_Ms(SAMPLE_INTERVAL_MS);
    }
}

/*---------------------------------------------------------------------------
 * Read VDD voltage using bandgap reference
 *---------------------------------------------------------------------------*/
static uint16_t read_vdd_mv(void)
{
    uint16_t bandgap_raw;
    uint32_t vdd;

    /* Read bandgap channel */
    DRV_ADC_SetChannel(&g_adc, ADC_CH_BANDGAP);
    bandgap_raw = DRV_ADC_ReadBlocking(&g_adc, ADC_CH_BANDGAP);

    /* Calculate VDD: VDD = (4096 * Vbg) / ADC_result */
    /* Vbg = 1.22V = 1220mV */
    if (bandgap_raw > 0) {
        vdd = ((uint32_t)4096 * BANDGAP_VOLTAGE_MV) / bandgap_raw;
    } else {
        vdd = 0;
    }

    return (uint16_t)vdd;
}

/*---------------------------------------------------------------------------
 * Read ADC channel and convert to mV
 *---------------------------------------------------------------------------*/
static uint16_t read_adc_mv(uint16_t vdd_mv)
{
    uint16_t adc_raw;
    uint32_t voltage;

    /* Read input channel */
    adc_raw = DRV_ADC_ReadBlocking(&g_adc, ADC_INPUT_CHANNEL);

    /* Convert to mV: Vin = (ADC_result * VDD) / 4096 */
    voltage = ((uint32_t)adc_raw * vdd_mv) / 4096;

    return (uint16_t)voltage;
}
