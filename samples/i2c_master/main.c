/*===========================================================================
 * @file    main.c
 * @brief   I2C Master Communication Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates I2C master mode communication.
 *   Scans for connected devices and performs read/write operations.
 * 
 * Hardware:
 *   - P1.3: SDA (with external pull-up)
 *   - P1.4: SCL (with external pull-up)
 *   - Connect I2C device (sensor, EEPROM, display, etc.)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_i2c.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define I2C_BUS_FREQ        100000      /* 100 kHz standard mode */
#define TEST_SLAVE_ADDR     0x50        /* Default: EEPROM AT24C02 */

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static i2c_handle_t g_i2c;

/*---------------------------------------------------------------------------
 * I2C Bus Scan
 *---------------------------------------------------------------------------*/
static void i2c_scan(void)
{
    uint8_t addr;
    uint8_t found = 0;
    hal_status_t status;

    SVC_CONSOLE_Printf("Scanning I2C bus...\r\n");
    SVC_CONSOLE_Printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");

    for (addr = 0; addr < 128; addr++) {
        if ((addr & 0x0F) == 0) {
            SVC_CONSOLE_Printf("%02X: ", addr);
        }

        /* Skip reserved addresses */
        if (addr < 0x08 || addr > 0x77) {
            SVC_CONSOLE_Printf("   ");
        } else {
            /* Try to start communication */
            status = DRV_I2C_Start(&g_i2c);
            if (status == HAL_OK) {
                status = DRV_I2C_WriteAddr(&g_i2c, addr << 1);
                DRV_I2C_Stop(&g_i2c);
                
                if (status == HAL_OK) {
                    SVC_CONSOLE_Printf("%02X ", addr);
                    found++;
                } else {
                    SVC_CONSOLE_Printf("-- ");
                }
            } else {
                SVC_CONSOLE_Printf("?? ");
            }
        }

        if ((addr & 0x0F) == 0x0F) {
            SVC_CONSOLE_Printf("\r\n");
        }
    }

    SVC_CONSOLE_Printf("\r\nFound %u device(s)\r\n", found);
}

/*---------------------------------------------------------------------------
 * Read single byte from device register
 *---------------------------------------------------------------------------*/
static hal_status_t i2c_read_reg(uint8_t slave_addr, uint8_t reg, uint8_t *data)
{
    hal_status_t status;

    /* Send register address */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, slave_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    status = DRV_I2C_WriteByte(&g_i2c, reg);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Repeated start for read */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, (slave_addr << 1) | 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Read with NACK */
    status = DRV_I2C_ReadByte(&g_i2c, data, 1);
    DRV_I2C_Stop(&g_i2c);

    return status;
}

/*---------------------------------------------------------------------------
 * Write single byte to device register
 *---------------------------------------------------------------------------*/
static hal_status_t i2c_write_reg(uint8_t slave_addr, uint8_t reg, uint8_t data)
{
    hal_status_t status;

    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, slave_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    status = DRV_I2C_WriteByte(&g_i2c, reg);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    status = DRV_I2C_WriteByte(&g_i2c, data);
    DRV_I2C_Stop(&g_i2c);

    return status;
}

/*---------------------------------------------------------------------------
 * Read multiple bytes from device
 *---------------------------------------------------------------------------*/
static hal_status_t i2c_read_burst(uint8_t slave_addr, uint8_t reg, 
                                    uint8_t *data, uint8_t len)
{
    hal_status_t status;
    uint8_t i;

    /* Send register address */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, slave_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    status = DRV_I2C_WriteByte(&g_i2c, reg);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Repeated start for read */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, (slave_addr << 1) | 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Read all bytes, NACK on last */
    for (i = 0; i < len; i++) {
        status = DRV_I2C_ReadByte(&g_i2c, &data[i], (i == len - 1) ? 1 : 0);
        if (status != HAL_OK) {
            DRV_I2C_Stop(&g_i2c);
            return status;
        }
    }

    DRV_I2C_Stop(&g_i2c);
    return HAL_OK;
}

/*---------------------------------------------------------------------------
 * Device Detection Demo
 *---------------------------------------------------------------------------*/
static void demo_device_detect(void)
{
    uint8_t data[6];
    hal_status_t status;

    SVC_CONSOLE_Printf("\r\n--- Common Device Detection ---\r\n");

    /* Try to detect BMP180/BMP280 (0x76 or 0x77) */
    status = i2c_read_reg(0x76, 0xD0, &data[0]);
    if (status == HAL_OK) {
        SVC_CONSOLE_Printf("BMP280/BME280 @ 0x76, ID=0x%02X\r\n", data[0]);
    }

    status = i2c_read_reg(0x77, 0xD0, &data[0]);
    if (status == HAL_OK) {
        SVC_CONSOLE_Printf("BMP280/BME280 @ 0x77, ID=0x%02X\r\n", data[0]);
    }

    /* Try to detect SHT30 (0x44 or 0x45) */
    status = DRV_I2C_Start(&g_i2c);
    if (status == HAL_OK) {
        status = DRV_I2C_WriteAddr(&g_i2c, 0x44 << 1);
        DRV_I2C_Stop(&g_i2c);
        if (status == HAL_OK) {
            SVC_CONSOLE_Printf("SHT30/SHT31 detected @ 0x44\r\n");
        }
    }

    /* Try to detect MPU6050 (0x68 or 0x69) */
    status = i2c_read_reg(0x68, 0x75, &data[0]);
    if (status == HAL_OK) {
        SVC_CONSOLE_Printf("MPU6050/MPU6500 @ 0x68, WHO_AM_I=0x%02X\r\n", data[0]);
    }

    /* Try to detect OLED SSD1306 (0x3C or 0x3D) */
    status = DRV_I2C_Start(&g_i2c);
    if (status == HAL_OK) {
        status = DRV_I2C_WriteAddr(&g_i2c, 0x3C << 1);
        DRV_I2C_Stop(&g_i2c);
        if (status == HAL_OK) {
            SVC_CONSOLE_Printf("OLED SSD1306 detected @ 0x3C\r\n");
        }
    }

    /* Try EEPROM at 0x50 */
    status = DRV_I2C_Start(&g_i2c);
    if (status == HAL_OK) {
        status = DRV_I2C_WriteAddr(&g_i2c, 0x50 << 1);
        DRV_I2C_Stop(&g_i2c);
        if (status == HAL_OK) {
            SVC_CONSOLE_Printf("EEPROM AT24Cxx detected @ 0x50\r\n");
        }
    }
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    i2c_config_t i2c_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;

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
    con_cfg.echo_enable = 0;
    con_cfg.newline_crlf = 1;
    SVC_CONSOLE_Init(&con_cfg);

    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("  I2C Master Demo\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("Hardware:\r\n");
    SVC_CONSOLE_Printf("  SDA: P1.3 (needs pull-up)\r\n");
    SVC_CONSOLE_Printf("  SCL: P1.4 (needs pull-up)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Initialize I2C */
    i2c_cfg.mode = I2C_MODE_MASTER;
    i2c_cfg.clock_freq = I2C_BUS_FREQ;
    i2c_cfg.slave_addr = 0;
    i2c_cfg.slave_addr_mask = 0;
    i2c_cfg.int_enable = 0;
    i2c_cfg.callback = NULL;
    DRV_I2C_Init(&g_i2c, &i2c_cfg);

    /* Scan I2C bus */
    SVC_CONSOLE_Printf("\r\n");
    i2c_scan();

    /* Detect common devices */
    demo_device_detect();

    SVC_CONSOLE_Printf("\r\n--- I2C Demo Complete ---\r\n");
    SVC_CONSOLE_Printf("Connect your I2C device and reset to scan again.\r\n");

    while (1) {
        SVC_DELAY_Ms(1000);
    }
}
