/*===========================================================================
 * @file    main.c
 * @brief   Timer Capture Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates Timer2 input capture functionality.
 *   Measures pulse width or frequency of external signal on P1.0.
 * 
 * Hardware:
 *   - P1.0 (T2/IC0): Timer2 capture input
 *   - Connect square wave signal (1Hz - 100kHz) to P1.0
 *   - UART0 for debug output
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_timer.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static timer_handle_t g_timer2;

static volatile uint16_t g_capture_value = 0;
static volatile uint16_t g_last_capture = 0;
static volatile uint16_t g_period = 0;
static volatile uint8_t g_capture_flag = 0;
static volatile uint32_t g_capture_count = 0;

/*---------------------------------------------------------------------------
 * Timer2 Capture Callback
 *---------------------------------------------------------------------------*/
static void timer2_capture_callback(timer_id_t id)
{
    uint16_t current;
    
    /* Read capture value from RCMP2H:RCMP2L */
    current = ((uint16_t)RCMP2H << 8) | RCMP2L;
    
    /* Calculate period (difference from last capture) */
    if (current >= g_last_capture) {
        g_period = current - g_last_capture;
    } else {
        /* Timer overflow occurred */
        g_period = (0xFFFF - g_last_capture) + current + 1;
    }
    
    g_last_capture = current;
    g_capture_value = current;
    g_capture_flag = 1;
    g_capture_count++;
}

/*---------------------------------------------------------------------------
 * Calculate frequency from period
 *---------------------------------------------------------------------------*/
static uint32_t calculate_frequency(uint16_t period)
{
    uint32_t timer_clk;
    
    if (period == 0) return 0;
    
    /* Timer2 clock = FSYS / 16 (with T2DIV) */
    timer_clk = SVC_SYSTEM_GetClockHz() / 16;
    
    return timer_clk / period;
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    timer_config_t tim_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint32_t last_print = 0;
    uint16_t period_copy;
    uint32_t freq;

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
    SVC_CONSOLE_Printf("  Timer2 Input Capture Demo\r\n");
    SVC_CONSOLE_Printf("  Capture Input: P1.0\r\n");
    SVC_CONSOLE_Printf("  Timer Clock: %lu Hz (FSYS/16)\r\n", 
        SVC_SYSTEM_GetClockHz() / 16);
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure P1.0 as input for capture */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 0;
    gpio_cfg.mode = GPIO_MODE_INPUT_ONLY;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 0;
    gpio_cfg.callback = NULL;
    
    gpio_handle_t capture_pin;
    DRV_GPIO_Init(&capture_pin, &gpio_cfg);

    /* Configure Timer2 for capture mode */
    /* Note: Capture mode configuration is device-specific */
    /* Using direct register access for capture setup */
    
    /* Timer2 in auto-reload mode, FSYS/16 clock */
    T2MOD = 0x00;
    T2CON = 0x00;
    
    /* Enable capture on rising edge */
    CAPCON0 = 0x10;     /* Enable capture channel 0 */
    CAPCON1 = 0x00;     /* Rising edge trigger */
    CAPCON2 = 0x00;
    
    /* Set timer2 clock divider */
    T2MOD |= 0x40;      /* T2DIV = 16 */
    
    /* Enable Timer2 */
    TR2 = 1;
    
    /* Enable capture interrupt */
    ECAP = 1;           /* Enable capture interrupt */
    
    /* Enable global interrupts */
    SVC_SYSTEM_EnableInterrupts();

    SVC_CONSOLE_Printf("\r\nCapture started. Apply signal to P1.0.\r\n");
    SVC_CONSOLE_Printf("Measurable range: 1 Hz - 100 kHz\r\n\r\n");

    /* Main loop - display measurements */
    while (1) {
        uint32_t now = SVC_DELAY_GetTicks();

        /* Update display every 500ms */
        if (now - last_print >= 500) {
            last_print = now;

            if (g_capture_flag) {
                /* Copy values with interrupts disabled */
                uint8_t crit = SVC_SYSTEM_EnterCritical();
                period_copy = g_period;
                g_capture_flag = 0;
                SVC_SYSTEM_ExitCritical(crit);

                /* Calculate frequency */
                freq = calculate_frequency(period_copy);

                SVC_CONSOLE_Printf("Period: %u ticks | Freq: %lu Hz | Captures: %lu\r\n",
                    period_copy, freq, g_capture_count);
            } else {
                SVC_CONSOLE_Printf("No signal detected (waiting for capture)...\r\n");
            }
        }

        SVC_DELAY_Ms(10);
    }
}

/*---------------------------------------------------------------------------
 * Capture Interrupt Handler
 * Note: This should be in the interrupt vector file
 *---------------------------------------------------------------------------*/
#if defined(__C51__)
void Capture_ISR(void) interrupt 12
#elif defined(__ICC8051__)
#pragma vector=0x63
__interrupt void Capture_ISR(void)
#elif defined(__SDCC__)
void Capture_ISR(void) __interrupt (12)
#endif
{
    PUSH_SFRS;
    
    if (CAPCON0 & 0x01) {   /* Check capture flag */
        /* Read capture value */
        g_capture_value = ((uint16_t)C0H << 8) | C0L;
        
        /* Calculate period */
        if (g_capture_value >= g_last_capture) {
            g_period = g_capture_value - g_last_capture;
        } else {
            g_period = (0xFFFF - g_last_capture) + g_capture_value + 1;
        }
        
        g_last_capture = g_capture_value;
        g_capture_flag = 1;
        g_capture_count++;
        
        /* Clear capture flag */
        CAPCON0 &= ~0x01;
    }
    
    POP_SFRS;
}
