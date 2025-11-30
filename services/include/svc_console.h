/*===========================================================================
 * @file    svc_console.h
 * @brief   Console Service Interface (Printf/Debug)
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Provides printf-style output and debug console functionality
 *          over UART.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef SVC_CONSOLE_H
#define SVC_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"
#include <stdarg.h>

/*---------------------------------------------------------------------------
 * Console Configuration
 *---------------------------------------------------------------------------*/
typedef enum {
    CONSOLE_UART_0 = 0,     /**< Use UART0 for console */
    CONSOLE_UART_1 = 1      /**< Use UART1 for console */
} console_uart_t;

/* Compatibility with driver UART port names */
#define UART_PORT_0     CONSOLE_UART_0
#define UART_PORT_1     CONSOLE_UART_1

typedef struct {
    console_uart_t uart;        /**< UART port to use (alias: port) */
    uint32_t       baudrate;    /**< Baud rate (default 115200) */
    uint8_t        echo_enable; /**< Enable input echo */
    uint8_t        newline_crlf;/**< Convert LF to CRLF on output */
} console_config_t;

/* Backwards compatibility - 'port' is alias for 'uart' */
#define port uart

/*---------------------------------------------------------------------------
 * Console API
 *---------------------------------------------------------------------------*/
/**
 * @brief  Initialize console service
 * @param  config  Console configuration
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t SVC_CONSOLE_Init(const console_config_t *config);

/**
 * @brief  De-initialize console service
 * @return DRV_OK on success
 */
drv_status_t SVC_CONSOLE_DeInit(void);

/**
 * @brief  Output a single character
 * @param  ch  Character to output
 */
void SVC_CONSOLE_PutChar(char ch);

/**
 * @brief  Output a string (null-terminated)
 * @param  str  String to output
 */
void SVC_CONSOLE_PutString(const char *str);

/**
 * @brief  Formatted print (printf-style)
 * @param  format  Format string
 * @param  ...     Variable arguments
 * @return Number of characters printed
 * 
 * @note   Supports: %d, %u, %x, %X, %s, %c, %%
 *         Optional: width, leading zeros (e.g., %04X)
 */
int SVC_CONSOLE_Printf(const char *format, ...);

/**
 * @brief  Formatted print with va_list
 * @param  format  Format string
 * @param  args    Variable argument list
 * @return Number of characters printed
 */
int SVC_CONSOLE_VPrintf(const char *format, va_list args);

/*---------------------------------------------------------------------------
 * Console Input
 *---------------------------------------------------------------------------*/
/**
 * @brief  Get a single character (blocking)
 * @return Character received
 */
char SVC_CONSOLE_GetChar(void);

/**
 * @brief  Check if character is available
 * @return 1 if character available, 0 otherwise
 */
uint8_t SVC_CONSOLE_IsCharAvailable(void);

/**
 * @brief  Get a line of input (blocking)
 * @param  buffer   Buffer to store input
 * @param  max_len  Maximum buffer length
 * @return Number of characters read (excluding null terminator)
 * @note   Terminates on newline or max_len-1 characters
 */
uint16_t SVC_CONSOLE_GetLine(char *buffer, uint16_t max_len);

/*---------------------------------------------------------------------------
 * Debug Helpers
 *---------------------------------------------------------------------------*/
/**
 * @brief  Print hexadecimal dump of data
 * @param  data   Pointer to data
 * @param  len    Length of data
 * @param  addr   Starting address for display (optional, default 0)
 */
void SVC_CONSOLE_HexDump(const uint8_t *data, uint16_t len, ...);

/* Macro to allow optional addr parameter */
#define SVC_CONSOLE_HexDump2(data, len)       SVC_CONSOLE_HexDump_Impl(data, len, 0)
#define SVC_CONSOLE_HexDump3(data, len, addr) SVC_CONSOLE_HexDump_Impl(data, len, addr)
void SVC_CONSOLE_HexDump_Impl(const uint8_t *data, uint16_t len, uint16_t addr);

/**
 * @brief  Print debug message with file/line info
 * @param  file   Source file name
 * @param  line   Line number
 * @param  msg    Message to print
 */
void SVC_CONSOLE_DebugPrint(const char *file, int line, const char *msg);

/*---------------------------------------------------------------------------
 * Convenience Macros
 *---------------------------------------------------------------------------*/
#ifdef DEBUG_ENABLE
    #define CONSOLE_DEBUG(msg)    SVC_CONSOLE_DebugPrint(__FILE__, __LINE__, msg)
    #define CONSOLE_PRINTF(...)   SVC_CONSOLE_Printf(__VA_ARGS__)
#else
    #define CONSOLE_DEBUG(msg)    ((void)0)
    #define CONSOLE_PRINTF(...)   ((void)0)
#endif

/*---------------------------------------------------------------------------
 * Compatibility Aliases
 *---------------------------------------------------------------------------*/
#define SVC_CONSOLE_Getc()          SVC_CONSOLE_GetChar()
#define SVC_CONSOLE_Putc(ch)        SVC_CONSOLE_PutChar(ch)
#define SVC_CONSOLE_Available()     SVC_CONSOLE_IsCharAvailable()
#define SVC_CONSOLE_Puts(str)       SVC_CONSOLE_PutString(str)

#ifdef __cplusplus
}
#endif

#endif /* SVC_CONSOLE_H */
