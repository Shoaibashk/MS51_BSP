/*===========================================================================
 * @file    main.c
 * @brief   UART Interrupt Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates UART interrupt-driven receive with ring buffer.
 *   Received characters are buffered and processed in main loop.
 * 
 * Hardware:
 *   - UART0 on P0.6 (RXD), P0.7 (TXD)
 *   - Connect to PC via USB-Serial adapter
 *   - 115200 baud, 8N1
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_uart.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Ring Buffer Configuration
 *---------------------------------------------------------------------------*/
#define RX_BUFFER_SIZE  64

static volatile uint8_t g_rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t g_rx_head = 0;
static volatile uint8_t g_rx_tail = 0;
static volatile uint8_t g_rx_count = 0;
static volatile uint32_t g_rx_total = 0;

/*---------------------------------------------------------------------------
 * UART RX Callback
 *---------------------------------------------------------------------------*/
static void uart_rx_callback(uint8_t data)
{
    /* Store in ring buffer if not full */
    if (g_rx_count < RX_BUFFER_SIZE) {
        g_rx_buffer[g_rx_head] = data;
        g_rx_head = (g_rx_head + 1) % RX_BUFFER_SIZE;
        g_rx_count++;
        g_rx_total++;
    }
    /* Buffer overflow - data is discarded */
}

/*---------------------------------------------------------------------------
 * Get character from ring buffer
 *---------------------------------------------------------------------------*/
static int get_rx_char(void)
{
    uint8_t critical;
    uint8_t data;

    if (g_rx_count == 0) {
        return -1;  /* Buffer empty */
    }

    critical = SVC_SYSTEM_EnterCritical();
    data = g_rx_buffer[g_rx_tail];
    g_rx_tail = (g_rx_tail + 1) % RX_BUFFER_SIZE;
    g_rx_count--;
    SVC_SYSTEM_ExitCritical(critical);

    return data;
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    uart_config_t uart_cfg;
    uart_handle_t uart0;
    console_config_t con_cfg;
    int ch;
    char line[64];
    uint8_t line_pos = 0;

    /* Initialize system at 24MHz */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Initialize UART0 with RX interrupt */
    uart_cfg.instance = UART_PORT_0;
    uart_cfg.baudrate = 115200;
    uart_cfg.databits = UART_DATA_8BIT;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.stopbits = UART_STOP_1BIT;
    uart_cfg.mode = UART_MODE_INTERRUPT;
    uart_cfg.timer = UART_TIMER_3;
    uart_cfg.tx_int_enable = 0;
    uart_cfg.rx_int_enable = 1;
    uart_cfg.rx_callback = uart_rx_callback;
    uart_cfg.tx_callback = NULL;
    DRV_UART_Init(&uart0, &uart_cfg);

    /* Also init console for printf (uses same UART0) */
    con_cfg.port = UART_PORT_0;
    con_cfg.baudrate = 115200;
    con_cfg.echo_enable = 0;
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    /* Enable global interrupts */
    SVC_SYSTEM_EnableInterrupts();

    /* Print header */
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  UART Interrupt Demo\r\n");
    SVC_CONSOLE_Printf("  Ring buffer size: %u bytes\r\n", RX_BUFFER_SIZE);
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("Type text and press Enter to echo.\r\n");
    SVC_CONSOLE_Printf("Commands: 'stats' - show statistics\r\n");
    SVC_CONSOLE_Printf("          'clear' - clear buffer\r\n");
    SVC_CONSOLE_Printf("\r\n> ");

    /* Main loop - process received characters */
    while (1) {
        /* Check for received character */
        ch = get_rx_char();
        
        if (ch >= 0) {
            /* Echo character */
            SVC_CONSOLE_PutChar((char)ch);

            /* Handle special characters */
            if (ch == '\r' || ch == '\n') {
                SVC_CONSOLE_NewLine();
                line[line_pos] = '\0';

                /* Process command */
                if (line_pos > 0) {
                    /* Check for commands */
                    if (line_pos == 5 && line[0] == 's' && line[1] == 't') {
                        /* "stats" command */
                        SVC_CONSOLE_Printf("--- Statistics ---\r\n");
                        SVC_CONSOLE_Printf("Total RX: %lu bytes\r\n", g_rx_total);
                        SVC_CONSOLE_Printf("Buffer used: %u/%u\r\n", 
                            g_rx_count, RX_BUFFER_SIZE);
                        SVC_CONSOLE_Printf("Uptime: %lu ms\r\n", SVC_DELAY_GetTicks());
                    } else if (line_pos == 5 && line[0] == 'c' && line[1] == 'l') {
                        /* "clear" command */
                        g_rx_total = 0;
                        SVC_CONSOLE_Printf("Statistics cleared.\r\n");
                    } else {
                        /* Echo the line back */
                        SVC_CONSOLE_Printf("Received %u chars: %s\r\n", 
                            line_pos, line);
                    }
                }

                line_pos = 0;
                SVC_CONSOLE_Printf("> ");
            } else if (ch == 0x08 || ch == 0x7F) {
                /* Backspace */
                if (line_pos > 0) {
                    line_pos--;
                    SVC_CONSOLE_Printf(" \b");  /* Erase character */
                }
            } else if (line_pos < sizeof(line) - 1) {
                /* Add to line buffer */
                line[line_pos++] = (char)ch;
            }
        }

        /* Small delay to reduce CPU usage */
        SVC_DELAY_Ms(1);
    }
}
