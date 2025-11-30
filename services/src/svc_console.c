/*===========================================================================
 * @file    svc_console.c
 * @brief   Console Service Implementation
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "svc_console.h"
#include "drv_uart.h"
#include <string.h>

/*---------------------------------------------------------------------------
 * Local Variables
 *---------------------------------------------------------------------------*/
static uart_handle_t g_console_uart;
static console_uart_t g_console_port = CONSOLE_UART_0;
static uint8_t g_echo_enable = 0;
static uint8_t g_newline_crlf = 1;
static uint8_t g_initialized = 0;

/*---------------------------------------------------------------------------
 * Helper Functions
 *---------------------------------------------------------------------------*/
static void console_put_digit(uint8_t digit)
{
    if (digit < 10) {
        SVC_CONSOLE_PutChar('0' + digit);
    } else {
        SVC_CONSOLE_PutChar('A' + digit - 10);
    }
}

static void console_put_uint(uint32_t value, uint8_t base, uint8_t width, char pad)
{
    char buf[12];
    int8_t i = 0;
    int8_t j;
    
    if (value == 0) {
        buf[i++] = '0';
    } else {
        while (value > 0) {
            uint8_t digit = (uint8_t)(value % base);
            buf[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            value /= base;
        }
    }
    
    /* Pad if needed */
    while (i < width) {
        buf[i++] = pad;
    }
    
    /* Print in reverse */
    for (j = i - 1; j >= 0; j--) {
        SVC_CONSOLE_PutChar(buf[j]);
    }
}

static void console_put_int(int32_t value, uint8_t width, char pad)
{
    if (value < 0) {
        SVC_CONSOLE_PutChar('-');
        value = -value;
        if (width > 0) width--;
    }
    console_put_uint((uint32_t)value, 10, width, pad);
}

/*===========================================================================
 * Public API Implementation
 *===========================================================================*/

drv_status_t SVC_CONSOLE_Init(const console_config_t *config)
{
    uart_config_t uart_cfg;
    drv_status_t status;

    if (config != NULL) {
        g_console_port = config->uart;
        g_echo_enable = config->echo_enable;
        g_newline_crlf = config->newline_crlf;
        uart_cfg.baudrate = config->baudrate;
    } else {
        g_console_port = CONSOLE_UART_0;
        g_echo_enable = 0;
        g_newline_crlf = 1;
        uart_cfg.baudrate = 115200;
    }

    uart_cfg.uart = (uart_id_t)g_console_port;
    uart_cfg.data_bits = UART_DATA_8BIT;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.stop_bits = UART_STOP_1BIT;
    uart_cfg.int_enable = 0;
    uart_cfg.callback = NULL;

    status = DRV_UART_Init(&g_console_uart, &uart_cfg);
    if (status != DRV_OK) {
        return status;
    }

    g_initialized = 1;
    return DRV_OK;
}

drv_status_t SVC_CONSOLE_DeInit(void)
{
    if (g_initialized) {
        DRV_UART_DeInit(&g_console_uart);
        g_initialized = 0;
    }
    return DRV_OK;
}

void SVC_CONSOLE_PutChar(char ch)
{
    if (!g_initialized) return;
    
    /* Convert LF to CRLF if enabled */
    if (ch == '\n' && g_newline_crlf) {
        DRV_UART_WriteByte(&g_console_uart, '\r');
    }
    DRV_UART_WriteByte(&g_console_uart, (uint8_t)ch);
}

void SVC_CONSOLE_PutString(const char *str)
{
    if (!g_initialized || str == NULL) return;
    
    while (*str) {
        SVC_CONSOLE_PutChar(*str++);
    }
}

int SVC_CONSOLE_Printf(const char *format, ...)
{
    va_list args;
    int count;
    
    va_start(args, format);
    count = SVC_CONSOLE_VPrintf(format, args);
    va_end(args);
    
    return count;
}

int SVC_CONSOLE_VPrintf(const char *format, va_list args)
{
    int count = 0;
    char ch;
    uint8_t width;
    char pad;
    
    if (!g_initialized || format == NULL) return 0;
    
    while ((ch = *format++) != '\0') {
        if (ch != '%') {
            SVC_CONSOLE_PutChar(ch);
            count++;
            continue;
        }
        
        /* Parse format specifier */
        width = 0;
        pad = ' ';
        
        ch = *format++;
        if (ch == '\0') break;
        
        /* Check for zero padding */
        if (ch == '0') {
            pad = '0';
            ch = *format++;
        }
        
        /* Parse width */
        while (ch >= '0' && ch <= '9') {
            width = width * 10 + (ch - '0');
            ch = *format++;
        }
        
        switch (ch) {
            case 'd':
            case 'i':
                console_put_int(va_arg(args, int), width, pad);
                count += (width > 0) ? width : 1;
                break;
                
            case 'u':
                console_put_uint(va_arg(args, unsigned int), 10, width, pad);
                count += (width > 0) ? width : 1;
                break;
                
            case 'x':
            case 'X':
                console_put_uint(va_arg(args, unsigned int), 16, width, pad);
                count += (width > 0) ? width : 1;
                break;
                
            case 'c':
                SVC_CONSOLE_PutChar((char)va_arg(args, int));
                count++;
                break;
                
            case 's': {
                const char *s = va_arg(args, const char *);
                if (s == NULL) s = "(null)";
                while (*s) {
                    SVC_CONSOLE_PutChar(*s++);
                    count++;
                }
                break;
            }
                
            case '%':
                SVC_CONSOLE_PutChar('%');
                count++;
                break;
                
            default:
                SVC_CONSOLE_PutChar('%');
                SVC_CONSOLE_PutChar(ch);
                count += 2;
                break;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------
 * Console Input
 *---------------------------------------------------------------------------*/
char SVC_CONSOLE_GetChar(void)
{
    uint8_t ch;
    
    if (!g_initialized) return '\0';
    
    DRV_UART_ReadByte(&g_console_uart, &ch, DRV_TIMEOUT_FOREVER);
    
    if (g_echo_enable) {
        SVC_CONSOLE_PutChar((char)ch);
    }
    
    return (char)ch;
}

uint8_t SVC_CONSOLE_IsCharAvailable(void)
{
    if (!g_initialized) return 0;
    return DRV_UART_IsRxAvailable(&g_console_uart);
}

uint16_t SVC_CONSOLE_GetLine(char *buffer, uint16_t max_len)
{
    uint16_t count = 0;
    char ch;
    
    if (!g_initialized || buffer == NULL || max_len == 0) return 0;
    
    while (count < max_len - 1) {
        ch = SVC_CONSOLE_GetChar();
        
        /* Handle backspace */
        if (ch == '\b' || ch == 0x7F) {
            if (count > 0) {
                count--;
                if (g_echo_enable) {
                    SVC_CONSOLE_PutString("\b \b");
                }
            }
            continue;
        }
        
        /* Handle newline */
        if (ch == '\r' || ch == '\n') {
            if (g_echo_enable) {
                SVC_CONSOLE_PutString("\r\n");
            }
            break;
        }
        
        buffer[count++] = ch;
    }
    
    buffer[count] = '\0';
    return count;
}

/*---------------------------------------------------------------------------
 * Debug Helpers
 *---------------------------------------------------------------------------*/
void SVC_CONSOLE_HexDump_Impl(const uint8_t *data, uint16_t len, uint16_t addr)
{
    uint16_t i, j;
    
    if (!g_initialized || data == NULL) return;
    
    for (i = 0; i < len; i += 16) {
        /* Print address */
        SVC_CONSOLE_Printf("%04X: ", addr + i);
        
        /* Print hex bytes */
        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                SVC_CONSOLE_Printf("%02X ", data[i + j]);
            } else {
                SVC_CONSOLE_PutString("   ");
            }
            if (j == 7) SVC_CONSOLE_PutChar(' ');
        }
        
        /* Print ASCII */
        SVC_CONSOLE_PutString(" |");
        for (j = 0; j < 16 && i + j < len; j++) {
            char ch = (char)data[i + j];
            if (ch >= 0x20 && ch < 0x7F) {
                SVC_CONSOLE_PutChar(ch);
            } else {
                SVC_CONSOLE_PutChar('.');
            }
        }
        SVC_CONSOLE_PutString("|\n");
    }
}

/* Wrapper that defaults addr to 0 when called with 2 args via macro */
void SVC_CONSOLE_HexDump(const uint8_t *data, uint16_t len, ...)
{
    SVC_CONSOLE_HexDump_Impl(data, len, 0);
}

void SVC_CONSOLE_DebugPrint(const char *file, int line, const char *msg)
{
    if (!g_initialized) return;
    
    /* Extract filename from path */
    const char *p = file;
    while (*file) {
        if (*file == '/' || *file == '\\') {
            p = file + 1;
        }
        file++;
    }
    
    SVC_CONSOLE_Printf("[%s:%d] %s\n", p, line, msg);
}
