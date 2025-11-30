/*===========================================================================
 * @file    main.c
 * @brief   Data Flash EEPROM Emulation Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates using on-chip data flash as EEPROM.
 *   Stores configuration data that persists across resets.
 * 
 * Hardware:
 *   - No external components required
 *   - UART0 for console output
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_flash.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"
#include "svc_eeprom.h"

/*---------------------------------------------------------------------------
 * Configuration Data Structure
 *---------------------------------------------------------------------------*/
#define CONFIG_MAGIC        0xA55A
#define CONFIG_VERSION      1

typedef struct {
    uint16_t magic;
    uint8_t version;
    uint8_t led_brightness;
    uint16_t servo_center;
    int8_t temp_offset;
    uint8_t flags;
    uint32_t boot_count;
    uint16_t checksum;
} config_data_t;

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static config_data_t g_config;

/*---------------------------------------------------------------------------
 * Calculate Checksum
 *---------------------------------------------------------------------------*/
static uint16_t calc_checksum(const config_data_t *cfg)
{
    const uint8_t *ptr = (const uint8_t *)cfg;
    uint16_t sum = 0;
    uint8_t i;

    /* Sum all bytes except checksum field */
    for (i = 0; i < sizeof(config_data_t) - sizeof(uint16_t); i++) {
        sum += ptr[i];
    }

    return sum;
}

/*---------------------------------------------------------------------------
 * Initialize Default Configuration
 *---------------------------------------------------------------------------*/
static void config_defaults(config_data_t *cfg)
{
    cfg->magic = CONFIG_MAGIC;
    cfg->version = CONFIG_VERSION;
    cfg->led_brightness = 50;
    cfg->servo_center = 1500;
    cfg->temp_offset = 0;
    cfg->flags = 0x01;
    cfg->boot_count = 0;
    cfg->checksum = calc_checksum(cfg);
}

/*---------------------------------------------------------------------------
 * Load Configuration from EEPROM
 *---------------------------------------------------------------------------*/
static int config_load(config_data_t *cfg)
{
    hal_status_t status;
    uint16_t checksum;

    status = SVC_EEPROM_Read(0, (uint8_t *)cfg, sizeof(config_data_t));
    if (status != HAL_OK) {
        return -1;
    }

    /* Verify magic number */
    if (cfg->magic != CONFIG_MAGIC) {
        return -2;
    }

    /* Verify checksum */
    checksum = calc_checksum(cfg);
    if (checksum != cfg->checksum) {
        return -3;
    }

    return 0;
}

/*---------------------------------------------------------------------------
 * Save Configuration to EEPROM
 *---------------------------------------------------------------------------*/
static int config_save(config_data_t *cfg)
{
    hal_status_t status;

    /* Update checksum */
    cfg->checksum = calc_checksum(cfg);

    status = SVC_EEPROM_Write(0, (const uint8_t *)cfg, sizeof(config_data_t));
    if (status != HAL_OK) {
        return -1;
    }

    return 0;
}

/*---------------------------------------------------------------------------
 * Display Configuration
 *---------------------------------------------------------------------------*/
static void config_display(const config_data_t *cfg)
{
    SVC_CONSOLE_Printf("Current Configuration:\r\n");
    SVC_CONSOLE_Printf("  Magic:       0x%04X\r\n", cfg->magic);
    SVC_CONSOLE_Printf("  Version:     %u\r\n", cfg->version);
    SVC_CONSOLE_Printf("  Brightness:  %u%%\r\n", cfg->led_brightness);
    SVC_CONSOLE_Printf("  Servo Ctr:   %u us\r\n", cfg->servo_center);
    SVC_CONSOLE_Printf("  Temp Offset: %d C\r\n", (int)cfg->temp_offset);
    SVC_CONSOLE_Printf("  Flags:       0x%02X\r\n", cfg->flags);
    SVC_CONSOLE_Printf("  Boot Count:  %lu\r\n", cfg->boot_count);
    SVC_CONSOLE_Printf("  Checksum:    0x%04X\r\n", cfg->checksum);
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    system_config_t sys_cfg;
    console_config_t con_cfg;
    eeprom_config_t ee_cfg;
    int result;
    char cmd;

    /* Initialize system at 24MHz */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    sys_cfg.wdt_timeout_ms = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Initialize console */
    con_cfg.port = UART_PORT_0;
    con_cfg.baudrate = 115200;
    con_cfg.echo_enable = 1;
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    /* Initialize EEPROM service */
    ee_cfg.start_addr = 0;
    ee_cfg.size = 256;
    ee_cfg.page_size = 128;
    SVC_EEPROM_Init(&ee_cfg);

    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  Data Flash EEPROM Demo\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Try to load configuration */
    result = config_load(&g_config);
    
    if (result == 0) {
        SVC_CONSOLE_Printf("\r\nConfiguration loaded successfully.\r\n");
        g_config.boot_count++;
        config_save(&g_config);
    } else {
        SVC_CONSOLE_Printf("\r\nNo valid configuration found.\r\n");
        if (result == -2) {
            SVC_CONSOLE_Printf("Invalid magic number.\r\n");
        } else if (result == -3) {
            SVC_CONSOLE_Printf("Checksum mismatch.\r\n");
        }
        SVC_CONSOLE_Printf("Loading defaults...\r\n");
        config_defaults(&g_config);
        g_config.boot_count = 1;
        config_save(&g_config);
    }

    SVC_CONSOLE_Printf("\r\n");
    config_display(&g_config);

    SVC_CONSOLE_Printf("\r\nCommands:\r\n");
    SVC_CONSOLE_Printf("  'b' - Increase brightness\r\n");
    SVC_CONSOLE_Printf("  'd' - Decrease brightness\r\n");
    SVC_CONSOLE_Printf("  's' - Save configuration\r\n");
    SVC_CONSOLE_Printf("  'r' - Reload configuration\r\n");
    SVC_CONSOLE_Printf("  'f' - Factory reset\r\n");
    SVC_CONSOLE_Printf("  '?' - Show configuration\r\n");
    SVC_CONSOLE_Printf("\r\n");

    while (1) {
        if (SVC_CONSOLE_Available()) {
            cmd = SVC_CONSOLE_Getc();

            switch (cmd) {
                case 'b':
                case 'B':
                    if (g_config.led_brightness < 100) {
                        g_config.led_brightness += 10;
                        SVC_CONSOLE_Printf("Brightness: %u%%\r\n", 
                                           g_config.led_brightness);
                    }
                    break;

                case 'd':
                case 'D':
                    if (g_config.led_brightness > 0) {
                        g_config.led_brightness -= 10;
                        SVC_CONSOLE_Printf("Brightness: %u%%\r\n", 
                                           g_config.led_brightness);
                    }
                    break;

                case 's':
                case 'S':
                    SVC_CONSOLE_Printf("Saving...\r\n");
                    if (config_save(&g_config) == 0) {
                        SVC_CONSOLE_Printf("Configuration saved.\r\n");
                    } else {
                        SVC_CONSOLE_Printf("Save failed!\r\n");
                    }
                    break;

                case 'r':
                case 'R':
                    SVC_CONSOLE_Printf("Reloading...\r\n");
                    if (config_load(&g_config) == 0) {
                        SVC_CONSOLE_Printf("Configuration reloaded.\r\n");
                        config_display(&g_config);
                    } else {
                        SVC_CONSOLE_Printf("Reload failed!\r\n");
                    }
                    break;

                case 'f':
                case 'F':
                    SVC_CONSOLE_Printf("Factory reset...\r\n");
                    config_defaults(&g_config);
                    config_save(&g_config);
                    SVC_CONSOLE_Printf("Done.\r\n");
                    config_display(&g_config);
                    break;

                case '?':
                    config_display(&g_config);
                    break;

                default:
                    break;
            }
        }

        SVC_DELAY_Ms(10);
    }
}
