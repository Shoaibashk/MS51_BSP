/*===========================================================================
 * @file    main.c
 * @brief   SPI Master Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Demonstrates SPI master mode communication.
 *   Sends data to SPI slave and receives response.
 * 
 * Hardware:
 *   - P1.5: SS (Slave Select, directly controlled)
 *   - P1.0: SPCLK (SPI Clock)
 *   - P0.0: MOSI (Master Out, Slave In)
 *   - P0.1: MISO (Master In, Slave Out)
 *   - Connect to SPI slave device
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
 * Pin Definitions
 *---------------------------------------------------------------------------*/
#define SS_PORT     GPIO_PORT_1
#define SS_PIN      5

/*---------------------------------------------------------------------------
 * Test Data
 *---------------------------------------------------------------------------*/
static uint8_t g_tx_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
static uint8_t g_rx_data[5];

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static gpio_handle_t g_ss_pin;
static spi_handle_t g_spi;

/*---------------------------------------------------------------------------
 * SS Control Functions
 *---------------------------------------------------------------------------*/
static void ss_assert(void)
{
    DRV_GPIO_Write(&g_ss_pin, 0);   /* SS low = active */
}

static void ss_deassert(void)
{
    DRV_GPIO_Write(&g_ss_pin, 1);   /* SS high = inactive */
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
    drv_status_t status;
    uint8_t i;
    uint32_t transfer_count = 0;

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
    SVC_CONSOLE_Printf("  SPI Master Demo\r\n");
    SVC_CONSOLE_Printf("  Mode: CPOL=0, CPHA=0 (Mode 0)\r\n");
    SVC_CONSOLE_Printf("========================================\r\n");

    /* Configure SS pin as push-pull output */
    gpio_cfg.port = SS_PORT;
    gpio_cfg.pin = SS_PIN;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_ss_pin, &gpio_cfg);
    ss_deassert();  /* Start with SS inactive */

    /* Configure SPI pins (SPCLK, MOSI, MISO) */
    /* These are typically configured by the SPI driver */

    /* Initialize SPI master */
    spi_cfg.mode = SPI_MODE_MASTER;
    spi_cfg.cpol = SPI_CPOL_0;          /* Clock idle low */
    spi_cfg.cpha = SPI_CPHA_0;          /* Sample on leading edge */
    spi_cfg.clock_div = SPI_CLK_DIV_4;  /* FSYS/4 = 6MHz */
    spi_cfg.lsb_first = 0;              /* MSB first */
    spi_cfg.int_enable = 0;
    spi_cfg.callback = NULL;

    status = DRV_SPI_Init(&g_spi, &spi_cfg);
    if (status != DRV_OK) {
        SVC_CONSOLE_Printf("SPI init failed: %d\r\n", status);
        while (1);
    }

    SVC_CONSOLE_Printf("\r\nSPI initialized.\r\n");
    SVC_CONSOLE_Printf("Press any key to perform transfer...\r\n\r\n");

    /* Main loop */
    while (1) {
        /* Wait for key press */
        if (SVC_CONSOLE_IsRxReady()) {
            SVC_CONSOLE_GetChar(0);
            transfer_count++;

            SVC_CONSOLE_Printf("--- Transfer #%lu ---\r\n", transfer_count);

            /* Show TX data */
            SVC_CONSOLE_Printf("TX: ");
            for (i = 0; i < sizeof(g_tx_data); i++) {
                SVC_CONSOLE_Printf("%02X ", g_tx_data[i]);
            }
            SVC_CONSOLE_Printf("\r\n");

            /* Perform SPI transfer */
            ss_assert();
            status = DRV_SPI_TransferBuffer(&g_spi, g_tx_data, g_rx_data, sizeof(g_tx_data));
            ss_deassert();

            if (status != DRV_OK) {
                SVC_CONSOLE_Printf("Transfer failed: %d\r\n", status);
            } else {
                /* Show RX data */
                SVC_CONSOLE_Printf("RX: ");
                for (i = 0; i < sizeof(g_rx_data); i++) {
                    SVC_CONSOLE_Printf("%02X ", g_rx_data[i]);
                }
                SVC_CONSOLE_Printf("\r\n");
            }

            SVC_CONSOLE_Printf("\r\n");
        }

        SVC_DELAY_Ms(10);
    }
}
