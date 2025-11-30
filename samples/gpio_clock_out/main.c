/*===========================================================================
 * @file    main.c
 * @brief   GPIO Clock Output Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates clock output functionality on P1.0.
 *   Outputs divided system clock for external use or debugging.
 * 
 * Hardware:
 *   - Clock output on P1.0 (CLKO)
 *   - Connect oscilloscope or frequency counter to measure
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
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
    uint8_t divider = 0;

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
    SVC_CONSOLE_Printf("  GPIO Clock Output Demo\r\n");
    SVC_CONSOLE_Printf("  Output: P1.0 (CLKO)\r\n");
    SVC_CONSOLE_Printf("  FSYS: %lu Hz\r\n", SVC_SYSTEM_GetClockHz());
    SVC_CONSOLE_Printf("========================================\r\n");

    SVC_CONSOLE_Printf("\r\nClock output frequency table:\r\n");
    SVC_CONSOLE_Printf("  Div 0: %lu Hz (FSYS/1)\r\n", SVC_SYSTEM_GetClockHz() / 1);
    SVC_CONSOLE_Printf("  Div 1: %lu Hz (FSYS/2)\r\n", SVC_SYSTEM_GetClockHz() / 2);
    SVC_CONSOLE_Printf("  Div 2: %lu Hz (FSYS/4)\r\n", SVC_SYSTEM_GetClockHz() / 4);
    SVC_CONSOLE_Printf("  Div 3: %lu Hz (FSYS/8)\r\n", SVC_SYSTEM_GetClockHz() / 8);
    SVC_CONSOLE_Printf("  Div 4: %lu Hz (FSYS/16)\r\n", SVC_SYSTEM_GetClockHz() / 16);
    SVC_CONSOLE_Printf("  Div 5: %lu Hz (FSYS/32)\r\n", SVC_SYSTEM_GetClockHz() / 32);
    SVC_CONSOLE_Printf("  Div 6: %lu Hz (FSYS/64)\r\n", SVC_SYSTEM_GetClockHz() / 64);
    SVC_CONSOLE_Printf("  Div 7: %lu Hz (FSYS/128)\r\n", SVC_SYSTEM_GetClockHz() / 128);

    /* Enable clock output with initial divider */
    SVC_SYSTEM_EnableClockOutput(divider);
    SVC_CONSOLE_Printf("\r\nClock output enabled, divider: %u\r\n", divider);

    SVC_CONSOLE_Printf("\r\nPress any key to cycle through dividers...\r\n");

    /* Main loop - cycle through clock dividers */
    while (1) {
        /* Wait for key press */
        if (SVC_CONSOLE_IsRxReady()) {
            SVC_CONSOLE_GetChar(0);  /* Clear the character */

            /* Increment divider */
            divider = (divider + 1) & 0x07;

            /* Update clock output */
            SVC_SYSTEM_EnableClockOutput(divider);

            /* Calculate and display frequency */
            uint32_t freq = SVC_SYSTEM_GetClockHz() >> divider;
            SVC_CONSOLE_Printf("Divider: %u, Output: %lu Hz\r\n", divider, freq);
        }

        SVC_DELAY_Ms(10);
    }
}
