/*===========================================================================
 * @file    main.c
 * @brief   UART Printf Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates UART printf functionality using the Console service.
 *   Supports various format specifiers and debug output.
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
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint8_t  u8_val = 0xAB;
    uint16_t u16_val = 0x1234;
    uint32_t u32_val = 0xDEADBEEF;
    int8_t   i8_val = -42;
    int16_t  i16_val = -1000;
    uint32_t counter = 0;
    uint8_t  uid[12];
    uint8_t  test_data[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

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

    /* Print header */
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  MS51 UART Printf Demo\r\n");
    SVC_CONSOLE_Printf("  UART0 @ 115200 baud\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Print system info */
    SVC_CONSOLE_Printf("--- System Information ---\r\n");
    SVC_CONSOLE_Printf("BSP Version: 0x%08lX\r\n", SVC_SYSTEM_GetBSPVersion());
    SVC_CONSOLE_Printf("System Clock: %lu Hz\r\n", SVC_SYSTEM_GetClockHz());
    SVC_CONSOLE_Printf("Reset Source: %d\r\n", (int)SVC_SYSTEM_GetResetSource());
    SVC_CONSOLE_Printf("\r\n");

    /* Print Device UID */
    SVC_SYSTEM_GetUID(uid);
    SVC_CONSOLE_Printf("Device UID:\r\n");
    SVC_CONSOLE_HexDump(uid, 12);
    SVC_CONSOLE_Printf("\r\n");

    /* Demonstrate format specifiers */
    SVC_CONSOLE_Printf("--- Format Specifier Demo ---\r\n");
    SVC_CONSOLE_Printf("Unsigned 8-bit:  %%u = %u\r\n", u8_val);
    SVC_CONSOLE_Printf("Unsigned 16-bit: %%u = %u\r\n", u16_val);
    SVC_CONSOLE_Printf("Unsigned 32-bit: %%lu = %lu\r\n", u32_val);
    SVC_CONSOLE_Printf("Signed 8-bit:    %%d = %d\r\n", i8_val);
    SVC_CONSOLE_Printf("Signed 16-bit:   %%d = %d\r\n", i16_val);
    SVC_CONSOLE_Printf("Hex lowercase:   %%x = %x\r\n", u16_val);
    SVC_CONSOLE_Printf("Hex uppercase:   %%X = %X\r\n", u16_val);
    SVC_CONSOLE_Printf("Hex 32-bit:      %%lX = %lX\r\n", u32_val);
    SVC_CONSOLE_Printf("Character:       %%c = %c\r\n", 'A');
    SVC_CONSOLE_Printf("String:          %%s = %s\r\n", "Hello");
    SVC_CONSOLE_Printf("Percent:         %%%% = %%\r\n");
    SVC_CONSOLE_Printf("\r\n");

    /* Hex dump demo */
    SVC_CONSOLE_Printf("--- Hex Dump Demo ---\r\n");
    SVC_CONSOLE_Printf("Test data (16 bytes):\r\n");
    SVC_CONSOLE_HexDump(test_data, 16);
    SVC_CONSOLE_Printf("\r\n");

    /* Debug macros demo */
    SVC_CONSOLE_Printf("--- Debug Macros Demo ---\r\n");
    DBG_PRINT("This is DBG_PRINT\r\n");
    DBG_INFO("Information message");
    DBG_WARN("Warning message");
    DBG_ERROR("Error message");
    SVC_CONSOLE_Printf("\r\n");

    /* Continuous output */
    SVC_CONSOLE_Printf("--- Continuous Counter ---\r\n");
    while (1) {
        SVC_CONSOLE_Printf("Counter: %lu, Uptime: %lu ms\r\n", 
            counter++, SVC_DELAY_GetTicks());
        SVC_DELAY_Ms(1000);
    }
}
