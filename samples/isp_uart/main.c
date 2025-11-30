/*===========================================================================
 * @file    main.c
 * @brief   ISP UART Bootloader Sample
 * @version 1.0.0
 * @date    2025
 * 
 * Description:
 *   Simple ISP (In-System Programming) bootloader via UART.
 *   Receives commands and data to program application flash.
 * 
 *   This code runs in LDROM and programs APROM.
 * 
 * Protocol:
 *   - Sync: 0x5A 0xA5
 *   - Command + Length + Address + Data + Checksum
 *   - Commands: Read, Write, Erase, Run, GetInfo
 * 
 * Hardware:
 *   - UART0: RXD/TXD for ISP communication
 *   - P1.4: LED (optional, status indicator)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp.
 *===========================================================================*/

#include "ms51_hal.h"
#include "drv_gpio.h"
#include "drv_uart.h"
#include "drv_flash.h"
#include "svc_delay.h"

/*---------------------------------------------------------------------------
 * ISP Protocol Definitions
 *---------------------------------------------------------------------------*/
#define ISP_SYNC1           0x5A
#define ISP_SYNC2           0xA5

#define ISP_CMD_CONNECT     0x01
#define ISP_CMD_GET_INFO    0x02
#define ISP_CMD_ERASE       0x03
#define ISP_CMD_WRITE       0x04
#define ISP_CMD_READ        0x05
#define ISP_CMD_RUN         0x06
#define ISP_CMD_RESET       0x07

#define ISP_ACK_OK          0x00
#define ISP_ACK_ERROR       0x01
#define ISP_ACK_CHECKSUM    0x02
#define ISP_ACK_ADDR_ERR    0x03
#define ISP_ACK_CMD_ERR     0x04

#define ISP_TIMEOUT_MS      5000
#define ISP_PAGE_SIZE       128

/*---------------------------------------------------------------------------
 * Device Info
 *---------------------------------------------------------------------------*/
#define DEVICE_ID           0x51FB      /* MS51FB9AE example */
#define APROM_SIZE          16384
#define LDROM_SIZE          4096
#define PAGE_SIZE           128

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
static uart_handle_t g_uart;
static gpio_handle_t g_led;
static uint8_t g_buffer[ISP_PAGE_SIZE + 16];

/*---------------------------------------------------------------------------
 * LED Control
 *---------------------------------------------------------------------------*/
static void led_on(void)  { DRV_GPIO_Write(&g_led, 0); }
static void led_off(void) { DRV_GPIO_Write(&g_led, 1); }
static void led_toggle(void) { DRV_GPIO_Toggle(&g_led); }

/*---------------------------------------------------------------------------
 * UART Low-Level Functions
 *---------------------------------------------------------------------------*/
static uint8_t uart_getc_timeout(uint16_t timeout_ms)
{
    uint8_t data = 0;
    uint16_t count = 0;

    while (!DRV_UART_IsRxReady(&g_uart)) {
        SVC_DELAY_Ms(1);
        count++;
        if (count >= timeout_ms) {
            return 0xFF;
        }
    }

    DRV_UART_Receive(&g_uart, &data, 1);
    return data;
}

static void uart_putc(uint8_t data)
{
    DRV_UART_Send(&g_uart, &data, 1);
}

static void uart_send(const uint8_t *data, uint16_t len)
{
    DRV_UART_Send(&g_uart, data, len);
}

/*---------------------------------------------------------------------------
 * Wait for Sync Pattern
 *---------------------------------------------------------------------------*/
static int wait_sync(void)
{
    uint8_t byte1, byte2;

    byte1 = uart_getc_timeout(ISP_TIMEOUT_MS);
    if (byte1 != ISP_SYNC1) return -1;

    byte2 = uart_getc_timeout(100);
    if (byte2 != ISP_SYNC2) return -1;

    return 0;
}

/*---------------------------------------------------------------------------
 * Send Response
 *---------------------------------------------------------------------------*/
static void send_ack(uint8_t status)
{
    uint8_t resp[3];
    resp[0] = ISP_SYNC1;
    resp[1] = ISP_SYNC2;
    resp[2] = status;
    uart_send(resp, 3);
}

/*---------------------------------------------------------------------------
 * Handle Connect Command
 *---------------------------------------------------------------------------*/
static void handle_connect(void)
{
    send_ack(ISP_ACK_OK);
    led_on();
}

/*---------------------------------------------------------------------------
 * Handle Get Info Command
 *---------------------------------------------------------------------------*/
static void handle_get_info(void)
{
    uint8_t resp[12];

    resp[0] = ISP_SYNC1;
    resp[1] = ISP_SYNC2;
    resp[2] = ISP_ACK_OK;
    resp[3] = (DEVICE_ID >> 8) & 0xFF;
    resp[4] = DEVICE_ID & 0xFF;
    resp[5] = (APROM_SIZE >> 8) & 0xFF;
    resp[6] = APROM_SIZE & 0xFF;
    resp[7] = (LDROM_SIZE >> 8) & 0xFF;
    resp[8] = LDROM_SIZE & 0xFF;
    resp[9] = PAGE_SIZE;
    resp[10] = 0x01;    /* Version */
    resp[11] = 0x00;    /* Flags */

    uart_send(resp, 12);
}

/*---------------------------------------------------------------------------
 * Handle Erase Command
 *---------------------------------------------------------------------------*/
static void handle_erase(void)
{
    uint16_t addr;
    uint8_t byte1, byte2;
    hal_status_t status;

    /* Receive address (2 bytes) */
    byte1 = uart_getc_timeout(100);
    byte2 = uart_getc_timeout(100);
    addr = ((uint16_t)byte1 << 8) | byte2;

    /* Validate address */
    if (addr >= APROM_SIZE) {
        send_ack(ISP_ACK_ADDR_ERR);
        return;
    }

    /* Erase page */
    status = DRV_Flash_ErasePage(addr);
    if (status == HAL_OK) {
        send_ack(ISP_ACK_OK);
    } else {
        send_ack(ISP_ACK_ERROR);
    }
}

/*---------------------------------------------------------------------------
 * Handle Write Command
 *---------------------------------------------------------------------------*/
static void handle_write(void)
{
    uint16_t addr;
    uint8_t len;
    uint8_t checksum = 0;
    uint8_t byte1, byte2, i;
    hal_status_t status;

    /* Receive address (2 bytes) */
    byte1 = uart_getc_timeout(100);
    byte2 = uart_getc_timeout(100);
    addr = ((uint16_t)byte1 << 8) | byte2;
    checksum += byte1;
    checksum += byte2;

    /* Receive length */
    len = uart_getc_timeout(100);
    checksum += len;

    if (len > ISP_PAGE_SIZE || addr + len > APROM_SIZE) {
        send_ack(ISP_ACK_ADDR_ERR);
        return;
    }

    /* Receive data */
    for (i = 0; i < len; i++) {
        g_buffer[i] = uart_getc_timeout(100);
        checksum += g_buffer[i];
        led_toggle();
    }

    /* Receive and verify checksum */
    byte1 = uart_getc_timeout(100);
    if (byte1 != checksum) {
        send_ack(ISP_ACK_CHECKSUM);
        return;
    }

    /* Write to flash */
    status = DRV_Flash_Write(addr, g_buffer, len);
    if (status == HAL_OK) {
        send_ack(ISP_ACK_OK);
    } else {
        send_ack(ISP_ACK_ERROR);
    }
}

/*---------------------------------------------------------------------------
 * Handle Read Command
 *---------------------------------------------------------------------------*/
static void handle_read(void)
{
    uint16_t addr;
    uint8_t len;
    uint8_t checksum = 0;
    uint8_t byte1, byte2, i;

    /* Receive address (2 bytes) */
    byte1 = uart_getc_timeout(100);
    byte2 = uart_getc_timeout(100);
    addr = ((uint16_t)byte1 << 8) | byte2;

    /* Receive length */
    len = uart_getc_timeout(100);

    if (addr + len > APROM_SIZE) {
        send_ack(ISP_ACK_ADDR_ERR);
        return;
    }

    /* Read from flash */
    DRV_Flash_Read(addr, g_buffer, len);

    /* Calculate checksum */
    for (i = 0; i < len; i++) {
        checksum += g_buffer[i];
    }

    /* Send response */
    uart_putc(ISP_SYNC1);
    uart_putc(ISP_SYNC2);
    uart_putc(ISP_ACK_OK);
    uart_putc(len);
    uart_send(g_buffer, len);
    uart_putc(checksum);
}

/*---------------------------------------------------------------------------
 * Handle Run Command (Jump to APROM)
 *---------------------------------------------------------------------------*/
static void handle_run(void)
{
    send_ack(ISP_ACK_OK);
    SVC_DELAY_Ms(10);

    /* Switch to APROM and reset */
    DRV_Flash_SetBootArea(FLASH_BOOT_APROM);
    SVC_DELAY_Ms(10);

    /* Software reset */
    HAL_SYS_Reset();
}

/*---------------------------------------------------------------------------
 * Process ISP Command
 *---------------------------------------------------------------------------*/
static void process_command(void)
{
    uint8_t cmd;

    /* Wait for sync */
    if (wait_sync() != 0) {
        return;
    }

    /* Get command */
    cmd = uart_getc_timeout(100);

    switch (cmd) {
        case ISP_CMD_CONNECT:
            handle_connect();
            break;

        case ISP_CMD_GET_INFO:
            handle_get_info();
            break;

        case ISP_CMD_ERASE:
            handle_erase();
            break;

        case ISP_CMD_WRITE:
            handle_write();
            break;

        case ISP_CMD_READ:
            handle_read();
            break;

        case ISP_CMD_RUN:
            handle_run();
            break;

        case ISP_CMD_RESET:
            send_ack(ISP_ACK_OK);
            SVC_DELAY_Ms(10);
            HAL_SYS_Reset();
            break;

        default:
            send_ack(ISP_ACK_CMD_ERR);
            break;
    }
}

/*---------------------------------------------------------------------------
 * Main Application (Bootloader)
 *---------------------------------------------------------------------------*/
int main(void)
{
    gpio_config_t gpio_cfg;
    uart_config_t uart_cfg;

    /* Initialize system at 24MHz */
    HAL_SYS_Init();
    HAL_SYS_SetClock(CLK_SRC_HIRC_24MHZ);

    /* Initialize delay service */
    SVC_DELAY_Init(NULL);

    /* Configure LED */
    gpio_cfg.port = GPIO_PORT_1;
    gpio_cfg.pin = 4;
    gpio_cfg.mode = GPIO_MODE_PUSH_PULL;
    gpio_cfg.pull = GPIO_PULL_NONE;
    gpio_cfg.int_edge = GPIO_INT_DISABLE;
    gpio_cfg.init_value = 1;
    gpio_cfg.callback = NULL;
    DRV_GPIO_Init(&g_led, &gpio_cfg);
    led_off();

    /* Configure UART */
    uart_cfg.port = UART_PORT_0;
    uart_cfg.baudrate = 115200;
    uart_cfg.data_bits = UART_DATA_8BIT;
    uart_cfg.stop_bits = UART_STOP_1BIT;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.flow_control = UART_FLOW_NONE;
    uart_cfg.int_enable = 0;
    uart_cfg.callback = NULL;
    DRV_UART_Init(&g_uart, &uart_cfg);

    /* Blink LED to indicate bootloader start */
    led_on();
    SVC_DELAY_Ms(100);
    led_off();
    SVC_DELAY_Ms(100);
    led_on();
    SVC_DELAY_Ms(100);
    led_off();

    /* Main ISP loop */
    while (1) {
        process_command();
    }
}
