/*===========================================================================
 * @file    main.c
 * @brief   I2C EEPROM Read/Write Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates I2C EEPROM (AT24C02/AT24C08/AT24C32) access.
 *   Reads, writes, and verifies data in EEPROM memory.
 * 
 * Hardware:
 *   - P1.3: SDA (with external pull-up)
 *   - P1.4: SCL (with external pull-up)
 *   - AT24Cxx EEPROM at address 0x50
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
 * EEPROM Configuration (adjust for your device)
 *---------------------------------------------------------------------------*/
#define EEPROM_ADDR         0x50        /* A0=A1=A2=GND */
#define EEPROM_PAGE_SIZE    8           /* AT24C02=8, AT24C32=32 */
#define EEPROM_TOTAL_SIZE   256         /* AT24C02=256, AT24C32=4096 */

/* For larger EEPROMs (>256 bytes), use 16-bit addressing */
#define EEPROM_16BIT_ADDR   0           /* Set to 1 for AT24C32/64/128 */

/*---------------------------------------------------------------------------
 * Test Configuration
 *---------------------------------------------------------------------------*/
#define TEST_ADDRESS        0x00
#define TEST_SIZE           32

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static i2c_handle_t g_i2c;

/*---------------------------------------------------------------------------
 * EEPROM Byte Write
 *---------------------------------------------------------------------------*/
static hal_status_t eeprom_write_byte(uint16_t addr, uint8_t data)
{
    hal_status_t status;
    uint8_t dev_addr = EEPROM_ADDR;

#if !EEPROM_16BIT_ADDR
    /* For small EEPROMs, upper address bits in device address */
    dev_addr |= ((addr >> 8) & 0x07);
#endif

    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, dev_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

#if EEPROM_16BIT_ADDR
    /* High byte address first for large EEPROMs */
    status = DRV_I2C_WriteByte(&g_i2c, (addr >> 8) & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }
#endif

    /* Low byte address */
    status = DRV_I2C_WriteByte(&g_i2c, addr & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Data */
    status = DRV_I2C_WriteByte(&g_i2c, data);
    DRV_I2C_Stop(&g_i2c);

    /* Wait for write cycle (5-10ms typical) */
    SVC_DELAY_Ms(10);

    return status;
}

/*---------------------------------------------------------------------------
 * EEPROM Page Write
 *---------------------------------------------------------------------------*/
static hal_status_t eeprom_write_page(uint16_t addr, const uint8_t *data, uint8_t len)
{
    hal_status_t status;
    uint8_t dev_addr = EEPROM_ADDR;
    uint8_t i;

    /* Ensure we don't cross page boundary */
    if (len > EEPROM_PAGE_SIZE) {
        len = EEPROM_PAGE_SIZE;
    }

#if !EEPROM_16BIT_ADDR
    dev_addr |= ((addr >> 8) & 0x07);
#endif

    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, dev_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

#if EEPROM_16BIT_ADDR
    status = DRV_I2C_WriteByte(&g_i2c, (addr >> 8) & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }
#endif

    status = DRV_I2C_WriteByte(&g_i2c, addr & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Write data bytes */
    for (i = 0; i < len; i++) {
        status = DRV_I2C_WriteByte(&g_i2c, data[i]);
        if (status != HAL_OK) {
            DRV_I2C_Stop(&g_i2c);
            return status;
        }
    }

    DRV_I2C_Stop(&g_i2c);

    /* Wait for write cycle */
    SVC_DELAY_Ms(10);

    return HAL_OK;
}

/*---------------------------------------------------------------------------
 * EEPROM Byte Read
 *---------------------------------------------------------------------------*/
static hal_status_t eeprom_read_byte(uint16_t addr, uint8_t *data)
{
    hal_status_t status;
    uint8_t dev_addr = EEPROM_ADDR;

#if !EEPROM_16BIT_ADDR
    dev_addr |= ((addr >> 8) & 0x07);
#endif

    /* Set address */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, dev_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

#if EEPROM_16BIT_ADDR
    status = DRV_I2C_WriteByte(&g_i2c, (addr >> 8) & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }
#endif

    status = DRV_I2C_WriteByte(&g_i2c, addr & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Read */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, (dev_addr << 1) | 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    status = DRV_I2C_ReadByte(&g_i2c, data, 1);
    DRV_I2C_Stop(&g_i2c);

    return status;
}

/*---------------------------------------------------------------------------
 * EEPROM Sequential Read
 *---------------------------------------------------------------------------*/
static hal_status_t eeprom_read_seq(uint16_t addr, uint8_t *data, uint16_t len)
{
    hal_status_t status;
    uint8_t dev_addr = EEPROM_ADDR;
    uint16_t i;

#if !EEPROM_16BIT_ADDR
    dev_addr |= ((addr >> 8) & 0x07);
#endif

    /* Set address */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, dev_addr << 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

#if EEPROM_16BIT_ADDR
    status = DRV_I2C_WriteByte(&g_i2c, (addr >> 8) & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }
#endif

    status = DRV_I2C_WriteByte(&g_i2c, addr & 0xFF);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

    /* Sequential read */
    status = DRV_I2C_Start(&g_i2c);
    if (status != HAL_OK) return status;

    status = DRV_I2C_WriteAddr(&g_i2c, (dev_addr << 1) | 1);
    if (status != HAL_OK) {
        DRV_I2C_Stop(&g_i2c);
        return status;
    }

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
 * Write buffer with page boundary handling
 *---------------------------------------------------------------------------*/
static hal_status_t eeprom_write_buffer(uint16_t addr, const uint8_t *data, uint16_t len)
{
    hal_status_t status;
    uint16_t offset = 0;
    uint8_t chunk;
    uint8_t page_remain;

    while (offset < len) {
        /* Calculate bytes remaining in current page */
        page_remain = EEPROM_PAGE_SIZE - ((addr + offset) % EEPROM_PAGE_SIZE);
        chunk = (len - offset) < page_remain ? (len - offset) : page_remain;

        status = eeprom_write_page(addr + offset, &data[offset], chunk);
        if (status != HAL_OK) {
            return status;
        }

        offset += chunk;
    }

    return HAL_OK;
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    i2c_config_t i2c_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint8_t write_data[TEST_SIZE];
    uint8_t read_data[TEST_SIZE];
    uint8_t i;
    uint8_t errors;
    hal_status_t status;

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
    SVC_CONSOLE_Printf("  I2C EEPROM Demo (AT24Cxx)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");
    SVC_CONSOLE_Printf("Configuration:\r\n");
    SVC_CONSOLE_Printf("  Device Addr: 0x%02X\r\n", EEPROM_ADDR);
    SVC_CONSOLE_Printf("  Page Size:   %u bytes\r\n", EEPROM_PAGE_SIZE);
    SVC_CONSOLE_Printf("  Total Size:  %u bytes\r\n", EEPROM_TOTAL_SIZE);
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Initialize I2C */
    i2c_cfg.mode = I2C_MODE_MASTER;
    i2c_cfg.clock_freq = 100000;
    i2c_cfg.slave_addr = 0;
    i2c_cfg.slave_addr_mask = 0;
    i2c_cfg.int_enable = 0;
    i2c_cfg.callback = NULL;
    DRV_I2C_Init(&g_i2c, &i2c_cfg);

    /* Check if EEPROM is present */
    status = DRV_I2C_Start(&g_i2c);
    if (status == HAL_OK) {
        status = DRV_I2C_WriteAddr(&g_i2c, EEPROM_ADDR << 1);
        DRV_I2C_Stop(&g_i2c);
    }

    if (status != HAL_OK) {
        SVC_CONSOLE_Printf("\r\nERROR: EEPROM not found at 0x%02X!\r\n", EEPROM_ADDR);
        SVC_CONSOLE_Printf("Check connections and pull-up resistors.\r\n");
        while (1) { SVC_DELAY_Ms(1000); }
    }

    SVC_CONSOLE_Printf("\r\nEEPROM detected!\r\n");

    /* Prepare test data */
    for (i = 0; i < TEST_SIZE; i++) {
        write_data[i] = i ^ 0xAA;
    }

    /* Read original data */
    SVC_CONSOLE_Printf("\r\n--- Current EEPROM contents ---\r\n");
    status = eeprom_read_seq(TEST_ADDRESS, read_data, TEST_SIZE);
    if (status == HAL_OK) {
        SVC_CONSOLE_HexDump(read_data, TEST_SIZE);
    } else {
        SVC_CONSOLE_Printf("Read failed!\r\n");
    }

    /* Write test pattern */
    SVC_CONSOLE_Printf("\r\n--- Writing test pattern ---\r\n");
    SVC_CONSOLE_Printf("Pattern: 0xAA XOR index\r\n");
    status = eeprom_write_buffer(TEST_ADDRESS, write_data, TEST_SIZE);
    if (status == HAL_OK) {
        SVC_CONSOLE_Printf("Write complete.\r\n");
    } else {
        SVC_CONSOLE_Printf("Write failed!\r\n");
    }

    /* Read back and verify */
    SVC_CONSOLE_Printf("\r\n--- Reading back data ---\r\n");
    status = eeprom_read_seq(TEST_ADDRESS, read_data, TEST_SIZE);
    if (status == HAL_OK) {
        SVC_CONSOLE_HexDump(read_data, TEST_SIZE);
    }

    /* Verify */
    errors = 0;
    for (i = 0; i < TEST_SIZE; i++) {
        if (read_data[i] != write_data[i]) {
            errors++;
            SVC_CONSOLE_Printf("Mismatch @ %u: W=0x%02X R=0x%02X\r\n", 
                               i, write_data[i], read_data[i]);
        }
    }

    if (errors == 0) {
        SVC_CONSOLE_Printf("\r\n*** Verification PASSED! ***\r\n");
    } else {
        SVC_CONSOLE_Printf("\r\n*** Verification FAILED (%u errors) ***\r\n", errors);
    }

    /* Single byte read/write demo */
    SVC_CONSOLE_Printf("\r\n--- Single Byte Access Demo ---\r\n");
    
    SVC_CONSOLE_Printf("Writing 0x55 to address 0x00...\r\n");
    eeprom_write_byte(0x00, 0x55);

    uint8_t val;
    eeprom_read_byte(0x00, &val);
    SVC_CONSOLE_Printf("Read back: 0x%02X\r\n", val);

    SVC_CONSOLE_Printf("\r\n--- EEPROM Demo Complete ---\r\n");

    while (1) {
        SVC_DELAY_Ms(1000);
    }
}
