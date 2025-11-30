/*===========================================================================
 * @file    main.c
 * @brief   ADC Multi-Channel Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates multi-channel ADC scanning.
 *   Reads multiple analog inputs and internal references.
 * 
 * Hardware:
 *   - AIN0 (P1.7): Analog input 0
 *   - AIN1 (P3.0): Analog input 1
 *   - AIN2 (P0.7): Analog input 2
 *   - Bandgap: Internal 1.22V reference
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
#define NUM_CHANNELS    4
#define SAMPLE_MS       1000
#define BANDGAP_MV      1220    /* Bandgap reference voltage */

/*---------------------------------------------------------------------------
 * Channel Configuration
 *---------------------------------------------------------------------------*/
static const struct {
    adc_channel_t channel;
    const char *name;
    uint8_t port;
    uint8_t pin;
} g_channels[NUM_CHANNELS] = {
    { ADC_CH_0,      "AIN0 (P1.7)", GPIO_PORT_1, 7 },
    { ADC_CH_1,      "AIN1 (P3.0)", GPIO_PORT_3, 0 },
    { ADC_CH_2,      "AIN2 (P0.7)", GPIO_PORT_0, 7 },
    { ADC_CH_BANDGAP, "Bandgap",    0xFF, 0xFF }     /* Internal, no pin */
};

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static adc_handle_t g_adc;
static uint16_t g_results[NUM_CHANNELS];

/*---------------------------------------------------------------------------
 * Calculate VDD from bandgap reading
 *---------------------------------------------------------------------------*/
static uint16_t calculate_vdd(uint16_t bandgap_raw)
{
    if (bandgap_raw == 0) return 0;
    
    /* VDD = (4096 * Vbg) / ADC_result */
    return (uint32_t)4096 * BANDGAP_MV / bandgap_raw;
}

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
    gpio_handle_t pins[NUM_CHANNELS];
    adc_config_t adc_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint16_t vdd_mv;
    uint8_t i;
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
    SVC_CONSOLE_Printf("  ADC Multi-Channel Demo\r\n");
    SVC_CONSOLE_Printf("  Channels: %u + Bandgap\r\n", NUM_CHANNELS - 1);
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure analog input pins */
    for (i = 0; i < NUM_CHANNELS; i++) {
        if (g_channels[i].port != 0xFF) {
            gpio_cfg.port = g_channels[i].port;
            gpio_cfg.pin = g_channels[i].pin;
            gpio_cfg.mode = GPIO_MODE_INPUT_ONLY;
            gpio_cfg.pull = GPIO_PULL_NONE;
            gpio_cfg.int_edge = GPIO_INT_DISABLE;
            gpio_cfg.init_value = 0;
            gpio_cfg.callback = NULL;
            DRV_GPIO_Init(&pins[i], &gpio_cfg);
        }
    }

    /* Disable digital input on analog pins */
    AINDIDS = 0x87;     /* Disable AIN0, AIN1, AIN2, AIN7 digital inputs */

    /* Initialize ADC */
    adc_cfg.channel = ADC_CH_0;
    adc_cfg.trigger = ADC_TRIG_SOFTWARE;
    adc_cfg.alignment = ADC_ALIGN_RIGHT;
    adc_cfg.int_enable = 0;
    adc_cfg.callback = NULL;

    if (DRV_ADC_Init(&g_adc, &adc_cfg) != DRV_OK) {
        SVC_CONSOLE_Printf("ADC init failed!\r\n");
        while (1);
    }

    /* Set ADC timing for bandgap */
    ADCCON1 |= 0x30;    /* Clock divider for stable bandgap */
    ADCCON2 |= 0x0E;    /* Acquisition time */

    SVC_CONSOLE_Printf("\r\nChannels:\r\n");
    for (i = 0; i < NUM_CHANNELS; i++) {
        SVC_CONSOLE_Printf("  [%u] %s\r\n", i, g_channels[i].name);
    }
    SVC_CONSOLE_Printf("\r\nSampling every %u ms...\r\n\r\n", SAMPLE_MS);

    /* Main loop - scan all channels */
    while (1) {
        sample_count++;

        /* Read all channels */
        for (i = 0; i < NUM_CHANNELS; i++) {
            DRV_ADC_SetChannel(&g_adc, g_channels[i].channel);
            g_results[i] = DRV_ADC_ReadBlocking(&g_adc, g_channels[i].channel);
        }

        /* Calculate VDD from bandgap */
        vdd_mv = calculate_vdd(g_results[NUM_CHANNELS - 1]);

        /* Display header */
        SVC_CONSOLE_Printf("--- Sample %lu (VDD = %u mV) ---\r\n", 
            sample_count, vdd_mv);

        /* Display all channel readings */
        for (i = 0; i < NUM_CHANNELS; i++) {
            uint16_t mv;
            
            if (g_channels[i].channel == ADC_CH_BANDGAP) {
                /* Bandgap shows calculated voltage */
                mv = BANDGAP_MV;
            } else {
                mv = adc_to_mv(g_results[i], vdd_mv);
            }

            SVC_CONSOLE_Printf("  %s: %4u (%4u mV)\r\n",
                g_channels[i].name, g_results[i], mv);
        }

        SVC_CONSOLE_Printf("\r\n");

        /* Wait for next sample */
        SVC_DELAY_Ms(SAMPLE_MS);
    }
}
