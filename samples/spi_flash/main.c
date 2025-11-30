/*===========================================================================
 * @file    main.c
 * @brief   SPI Flash Read/Write Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates SPI Flash memory access (W25Qxx, MX25L, etc.).
 *   Reads JEDEC ID, erases sector, writes and reads data.
 * 
 * Hardware:
 *   - P1.5: SS (Slave Select)
 *   - P1.0: SPCLK
 *   - P0.0: MOSI
 *   - P0.1: MISO
 *   - SPI Flash (W25Q32, W25Q64, MX25L6433F, etc.)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_spi.h"
#include "svc_delay.h"
#include "svc_system.h"
#include "svc_console.h"

/*---------------------------------------------------------------------------
 * SPI Flash Commands
 *---------------------------------------------------------------------------*/
#define CMD_READ_JEDEC_ID   0x9F
#define CMD_READ_STATUS     0x05
#define CMD_WRITE_ENABLE    0x06
#define CMD_WRITE_DISABLE   0x04
#define CMD_SECTOR_ERASE    0x20
#define CMD_PAGE_PROGRAM    0x02
#define CMD_READ_DATA       0x03
#define CMD_FAST_READ       0x0B

/* Status register bits */
#define STATUS_BUSY         0x01
#define STATUS_WEL          0x02

/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#define SS_PORT             GPIO_PORT_1
#define SS_PIN              5

#define TEST_ADDRESS        0x000000    /* Start of flash */
#define TEST_SIZE           32

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_ss;
static spi_handle_t g_spi;
static uint8_t g_buffer[TEST_SIZE];

/*---------------------------------------------------------------------------
 * SS Control
 *---------------------------------------------------------------------------*/
static void ss_low(void)  { DRV_GPIO_Write(&g_ss, 0); }
static void ss_high(void) { DRV_GPIO_Write(&g_ss, 1); }

/*---------------------------------------------------------------------------
 * Read JEDEC ID (Manufacturer, Device ID)
 *---------------------------------------------------------------------------*/
static void flash_read_jedec_id(uint8_t *mfr, uint8_t *type, uint8_t *capacity)
{
    uint8_t cmd = CMD_READ_JEDEC_ID;
    uint8_t id[3];

    ss_low();
    DRV_SPI_SendByte(&g_spi, cmd);
    id[0] = DRV_SPI_ReceiveByte(&g_spi);
    id[1] = DRV_SPI_ReceiveByte(&g_spi);
    id[2] = DRV_SPI_ReceiveByte(&g_spi);
    ss_high();

    *mfr = id[0];
    *type = id[1];
    *capacity = id[2];
}

/*---------------------------------------------------------------------------
 * Read Status Register
 *---------------------------------------------------------------------------*/
static uint8_t flash_read_status(void)
{
    uint8_t status;

    ss_low();
    DRV_SPI_SendByte(&g_spi, CMD_READ_STATUS);
    status = DRV_SPI_ReceiveByte(&g_spi);
    ss_high();

    return status;
}

/*---------------------------------------------------------------------------
 * Wait for Flash Ready
 *---------------------------------------------------------------------------*/
static void flash_wait_ready(void)
{
    uint16_t timeout = 10000;
    while ((flash_read_status() & STATUS_BUSY) && timeout--) {
        SVC_DELAY_Ms(1);
    }
}

/*---------------------------------------------------------------------------
 * Write Enable
 *---------------------------------------------------------------------------*/
static void flash_write_enable(void)
{
    ss_low();
    DRV_SPI_SendByte(&g_spi, CMD_WRITE_ENABLE);
    ss_high();
}

/*---------------------------------------------------------------------------
 * Sector Erase (4KB)
 *---------------------------------------------------------------------------*/
static void flash_sector_erase(uint32_t address)
{
    flash_write_enable();

    ss_low();
    DRV_SPI_SendByte(&g_spi, CMD_SECTOR_ERASE);
    DRV_SPI_SendByte(&g_spi, (address >> 16) & 0xFF);
    DRV_SPI_SendByte(&g_spi, (address >> 8) & 0xFF);
    DRV_SPI_SendByte(&g_spi, address & 0xFF);
    ss_high();

    flash_wait_ready();
}

/*---------------------------------------------------------------------------
 * Page Program (up to 256 bytes)
 *---------------------------------------------------------------------------*/
static void flash_page_program(uint32_t address, const uint8_t *data, uint16_t len)
{
    uint16_t i;

    flash_write_enable();

    ss_low();
    DRV_SPI_SendByte(&g_spi, CMD_PAGE_PROGRAM);
    DRV_SPI_SendByte(&g_spi, (address >> 16) & 0xFF);
    DRV_SPI_SendByte(&g_spi, (address >> 8) & 0xFF);
    DRV_SPI_SendByte(&g_spi, address & 0xFF);
    
    for (i = 0; i < len; i++) {
        DRV_SPI_SendByte(&g_spi, data[i]);
    }
    ss_high();

    flash_wait_ready();
}

/*---------------------------------------------------------------------------
 * Read Data
 *---------------------------------------------------------------------------*/
static void flash_read_data(uint32_t address, uint8_t *data, uint16_t len)
{
    uint16_t i;

    ss_low();
    DRV_SPI_SendByte(&g_spi, CMD_READ_DATA);
    DRV_SPI_SendByte(&g_spi, (address >> 16) & 0xFF);
    DRV_SPI_SendByte(&g_spi, (address >> 8) & 0xFF);
    DRV_SPI_SendByte(&g_spi, address & 0xFF);
    
    for (i = 0; i < len; i++) {
        data[i] = DRV_SPI_ReceiveByte(&g_spi);
    }
    ss_high();
}

/*---------------------------------------------------------------------------
 * Main Application
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    spi_config_t spi_cfg;
    system_config_t sys_cfg;
    console_config_t con_cfg;
    uint8_t mfr, type, capacity;
    uint8_t i;
    uint8_t write_data[TEST_SIZE];

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
    SVC_CONSOLE_Printf("  SPI Flash Demo\r\n");
    SVC_CONSOLE_Printf("  (W25Qxx, MX25Lxxx compatible)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure SS pin */
    gpio_cfg.port = SS_PORT;
    gpio_cfg.pin = SS_PIN;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_ss, &gpio_cfg);
    ss_high();

    /* Initialize SPI */
    spi_cfg.mode = SPI_MODE_MASTER;
    spi_cfg.cpol = SPI_CPOL_0;
    spi_cfg.cpha = SPI_CPHA_0;
    spi_cfg.clock_div = SPI_CLK_DIV_4;
    spi_cfg.lsb_first = 0;
    spi_cfg.int_enable = 0;
    spi_cfg.callback = NULL;
    DRV_SPI_Init(&g_spi, &spi_cfg);

    /* Read and display JEDEC ID */
    flash_read_jedec_id(&mfr, &type, &capacity);
    SVC_CONSOLE_Printf("\r\nJEDEC ID:\r\n");
    SVC_CONSOLE_Printf("  Manufacturer: 0x%02X", mfr);
    if (mfr == 0xEF) SVC_CONSOLE_Printf(" (Winbond)");
    else if (mfr == 0xC2) SVC_CONSOLE_Printf(" (Macronix)");
    else if (mfr == 0x20) SVC_CONSOLE_Printf(" (Micron)");
    SVC_CONSOLE_Printf("\r\n");
    SVC_CONSOLE_Printf("  Device Type:  0x%02X\r\n", type);
    SVC_CONSOLE_Printf("  Capacity:     0x%02X", capacity);
    if (capacity == 0x16) SVC_CONSOLE_Printf(" (32Mbit)");
    else if (capacity == 0x17) SVC_CONSOLE_Printf(" (64Mbit)");
    else if (capacity == 0x18) SVC_CONSOLE_Printf(" (128Mbit)");
    SVC_CONSOLE_Printf("\r\n");

    /* Prepare test data */
    for (i = 0; i < TEST_SIZE; i++) {
        write_data[i] = i;
    }

    /* Erase sector */
    SVC_CONSOLE_Printf("\r\nErasing sector at 0x%06lX...\r\n", (uint32_t)TEST_ADDRESS);
    flash_sector_erase(TEST_ADDRESS);
    SVC_CONSOLE_Printf("Erase complete.\r\n");

    /* Read after erase (should be 0xFF) */
    flash_read_data(TEST_ADDRESS, g_buffer, TEST_SIZE);
    SVC_CONSOLE_Printf("\r\nData after erase:\r\n");
    SVC_CONSOLE_HexDump(g_buffer, TEST_SIZE);

    /* Write test data */
    SVC_CONSOLE_Printf("\r\nWriting test pattern...\r\n");
    flash_page_program(TEST_ADDRESS, write_data, TEST_SIZE);
    SVC_CONSOLE_Printf("Write complete.\r\n");

    /* Read back and verify */
    flash_read_data(TEST_ADDRESS, g_buffer, TEST_SIZE);
    SVC_CONSOLE_Printf("\r\nData after write:\r\n");
    SVC_CONSOLE_HexDump(g_buffer, TEST_SIZE);

    /* Verify */
    uint8_t errors = 0;
    for (i = 0; i < TEST_SIZE; i++) {
        if (g_buffer[i] != write_data[i]) {
            errors++;
        }
    }

    if (errors == 0) {
        SVC_CONSOLE_Printf("\r\nVerification PASSED!\r\n");
    } else {
        SVC_CONSOLE_Printf("\r\nVerification FAILED! (%u errors)\r\n", errors);
    }

    while (1) {
        SVC_DELAY_Ms(1000);
    }
}
