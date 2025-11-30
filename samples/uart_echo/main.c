/*===========================================================================
 * @file    main.c
 * @brief   UART Echo Sample - Using UART Driver and Console Service
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   This sample demonstrates UART usage with the console service.
 *   Characters received are echoed back with additional debug info.
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
#include "drv_gpio.h"
#include "drv_uart.h"
#include "svc_delay.h"
#include "svc_console.h"
#include "svc_system.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define BUFFER_SIZE     64

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    console_config_t con_cfg;
    char buffer[BUFFER_SIZE];
    uint32_t rx_count = 0;
    int ch;

    /* Initialize system */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Initialize console service */
    con_cfg.port = UART_PORT_0;
    con_cfg.baudrate = 115200;
    con_cfg.echo_enable = 0;      /* We'll echo manually */
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    /* Print welcome message */
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  MS51 UART Echo Demo\r\n");
    SVC_CONSOLE_Printf("  BSP Version: 0x%08lX\r\n", SVC_SYSTEM_GetBSPVersion());
    SVC_CONSOLE_Printf("  Clock: %lu Hz\r\n", SVC_SYSTEM_GetClockHz());
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("Type characters to echo. Enter a line for line mode.\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Main loop */
    while (1) {
        /* Check for received character */
        ch = SVC_CONSOLE_GetChar(0);  /* Non-blocking */
        
        if (ch >= 0) {
            rx_count++;
            
            /* Echo the character */
            SVC_CONSOLE_PutChar((char)ch);
            
            /* Handle special characters */
            if (ch == '\r' || ch == '\n') {
                SVC_CONSOLE_NewLine();
                SVC_CONSOLE_Printf("[RX count: %lu]\r\n", rx_count);
            }
            
            /* Line input demo on 'L' key */
            if (ch == 'L' || ch == 'l') {
                SVC_CONSOLE_Printf("\r\nEnter a line: ");
                if (SVC_CONSOLE_GetLine(buffer, BUFFER_SIZE, 10000)) {
                    SVC_CONSOLE_Printf("You entered: %s\r\n", buffer);
                    SVC_CONSOLE_Printf("Length: %u bytes\r\n", 
                        (uint16_t)strlen((const char*)buffer));
                    SVC_CONSOLE_Printf("Hex dump:\r\n");
                    SVC_CONSOLE_HexDump((uint8_t*)buffer, 
                        (uint16_t)strlen((const char*)buffer));
                } else {
                    SVC_CONSOLE_Printf("\r\nTimeout!\r\n");
                }
            }
            
            /* System info on 'I' key */
            if (ch == 'I' || ch == 'i') {
                uint8_t uid[12];
                SVC_CONSOLE_Printf("\r\n--- System Info ---\r\n");
                SVC_CONSOLE_Printf("Uptime: %lu ms\r\n", SVC_DELAY_GetTicks());
                SVC_CONSOLE_Printf("Reset source: %d\r\n", 
                    (int)SVC_SYSTEM_GetResetSource());
                
                SVC_SYSTEM_GetUID(uid);
                SVC_CONSOLE_Printf("Device UID:\r\n");
                SVC_CONSOLE_HexDump(uid, 12);
                SVC_CONSOLE_Printf("-------------------\r\n");
            }
        }
        
        /* Small delay to reduce CPU usage */
        SVC_DELAY_Ms(1);
    }
}

/*---------------------------------------------------------------------------
 * Helper function - strlen implementation
 *---------------------------------------------------------------------------*/
static uint16_t strlen(const char *s)
{
    uint16_t len = 0;
    while (*s++) len++;
    return len;
}
