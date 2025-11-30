/*===========================================================================
 * @file    drv_uart.h
 * @brief   UART Driver Interface
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details Asynchronous UART driver for MS51 microcontrollers.
 *          Supports UART0 and UART1 with interrupt-driven or polling modes.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_UART_H
#define DRV_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * UART Instance Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_0 = 0,     /**< UART0 (P0.6=TXD0, P0.7=RXD0 by default) */
    UART_1 = 1      /**< UART1 (P1.6=TXD1, P1.7=RXD1 by default) */
} uart_instance_t;

/* Compatibility aliases */
#define UART_PORT_0     UART_0
#define UART_PORT_1     UART_1

/*---------------------------------------------------------------------------
 * UART Baud Rate Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_BAUD_9600   = 9600,
    UART_BAUD_19200  = 19200,
    UART_BAUD_38400  = 38400,
    UART_BAUD_57600  = 57600,
    UART_BAUD_115200 = 115200
} uart_baudrate_t;

/*---------------------------------------------------------------------------
 * UART Data Bits
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_DATA_8BIT = 0,     /**< 8 data bits (Mode 1) */
    UART_DATA_9BIT = 1      /**< 9 data bits (Mode 2/3) */
} uart_databits_t;

/*---------------------------------------------------------------------------
 * UART Stop Bits
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_STOP_1BIT = 0,     /**< 1 stop bit */
    UART_STOP_2BIT = 1      /**< 2 stop bits (not all modes) */
} uart_stopbits_t;

/* Compatibility aliases */
#define UART_STOPBITS_1     UART_STOP_1BIT
#define UART_STOPBITS_2     UART_STOP_2BIT

/*---------------------------------------------------------------------------
 * UART Parity
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_PARITY_NONE = 0,   /**< No parity */
    UART_PARITY_ODD  = 1,   /**< Odd parity (via 9th bit) */
    UART_PARITY_EVEN = 2    /**< Even parity (via 9th bit) */
} uart_parity_t;

/*---------------------------------------------------------------------------
 * UART Transfer Mode
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_MODE_POLLING   = 0,    /**< Polling mode */
    UART_MODE_INTERRUPT = 1     /**< Interrupt-driven mode */
} uart_mode_t;

/*---------------------------------------------------------------------------
 * UART Timer Selection (for baud rate generator)
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_TIMER_1 = 1,       /**< Timer 1 as baud rate generator */
    UART_TIMER_3 = 3        /**< Timer 3 as baud rate generator */
} uart_timer_t;

/*---------------------------------------------------------------------------
 * UART RX Callback Type (for per-byte callbacks)
 *---------------------------------------------------------------------------*/
typedef void (*uart_rx_callback_t)(uint8_t data);
typedef void (*uart_tx_callback_t)(void);

/*---------------------------------------------------------------------------
 * UART Configuration Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    uart_instance_t instance;       /**< UART instance (0 or 1) */
    uint32_t        baudrate;       /**< Baud rate */
    uart_databits_t databits;       /**< Data bits */
    uart_stopbits_t stopbits;       /**< Stop bits */
    uart_parity_t   parity;         /**< Parity */
    uart_mode_t     mode;           /**< Transfer mode */
    uart_timer_t    timer;          /**< Timer for baud rate generation */
    uint8_t         rx_int_enable;  /**< Enable RX interrupt */
    uint8_t         tx_int_enable;  /**< Enable TX interrupt */
    uart_rx_callback_t rx_callback; /**< RX byte callback */
    uart_tx_callback_t tx_callback; /**< TX complete callback */
} uart_config_t;

/* Compatibility: 'port' as alias for 'instance' */
#define uart_port   instance

/*---------------------------------------------------------------------------
 * UART Events (for callbacks)
 *---------------------------------------------------------------------------*/
typedef enum {
    UART_EVT_TX_COMPLETE = 0,   /**< Transmission complete */
    UART_EVT_RX_DATA     = 1,   /**< Data received */
    UART_EVT_ERROR       = 2    /**< Error occurred */
} uart_event_t;

/*---------------------------------------------------------------------------
 * UART Handle Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    uart_instance_t instance;       /**< UART instance */
    drv_callback_t  callback;       /**< Event callback */
    uart_rx_callback_t rx_callback; /**< RX byte callback */
    uart_tx_callback_t tx_callback; /**< TX complete callback */
    volatile uint8_t *tx_buffer;    /**< TX buffer pointer */
    volatile uint16_t tx_length;    /**< TX buffer length */
    volatile uint16_t tx_count;     /**< TX bytes sent */
    volatile uint8_t *rx_buffer;    /**< RX buffer pointer */
    volatile uint16_t rx_length;    /**< RX buffer length */
    volatile uint16_t rx_count;     /**< RX bytes received */
    volatile uint8_t  tx_busy;      /**< TX in progress flag */
    volatile uint8_t  rx_busy;      /**< RX in progress flag */
} uart_handle_t;

/*===========================================================================
 * Default Configuration Macros
 *===========================================================================*/

/**
 * @brief Default UART0 configuration (115200, 8N1)
 */
#define UART0_DEFAULT_CONFIG {              \
    .instance = UART_0,                     \
    .baudrate = 115200,                     \
    .databits = UART_DATA_8BIT,             \
    .stopbits = UART_STOP_1BIT,             \
    .parity   = UART_PARITY_NONE,           \
    .mode     = UART_MODE_POLLING,          \
    .timer    = UART_TIMER_3                \
}

/**
 * @brief Default UART1 configuration (9600, 8N1)
 */
#define UART1_DEFAULT_CONFIG {              \
    .instance = UART_1,                     \
    .baudrate = 9600,                       \
    .databits = UART_DATA_8BIT,             \
    .stopbits = UART_STOP_1BIT,             \
    .parity   = UART_PARITY_NONE,           \
    .mode     = UART_MODE_POLLING,          \
    .timer    = UART_TIMER_3                \
}

/*===========================================================================
 * Public API Functions
 *===========================================================================*/

/**
 * @brief  Initialize UART peripheral
 * @param  handle   Pointer to UART handle
 * @param  config   Pointer to UART configuration
 * @return DRV_OK on success, error code otherwise
 * 
 * @code
 *   uart_handle_t uart0;
 *   uart_config_t config = UART0_DEFAULT_CONFIG;
 *   DRV_UART_Init(&uart0, &config);
 * @endcode
 */
drv_status_t DRV_UART_Init(uart_handle_t *handle, const uart_config_t *config);

/**
 * @brief  De-initialize UART peripheral
 * @param  handle  Pointer to UART handle
 * @return DRV_OK on success
 */
drv_status_t DRV_UART_DeInit(uart_handle_t *handle);

/**
 * @brief  Register event callback
 * @param  handle    Pointer to UART handle
 * @param  callback  Callback function
 * @return DRV_OK on success
 */
drv_status_t DRV_UART_RegisterCallback(uart_handle_t *handle, drv_callback_t callback);

/**
 * @brief  Transmit data (blocking)
 * @param  handle   Pointer to UART handle
 * @param  data     Data buffer to transmit
 * @param  length   Number of bytes to transmit
 * @param  timeout  Timeout in milliseconds (0 = no wait)
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t DRV_UART_Transmit(uart_handle_t *handle, const uint8_t *data, 
                               uint16_t length, uint32_t timeout);

/**
 * @brief  Receive data (blocking)
 * @param  handle   Pointer to UART handle
 * @param  data     Buffer to receive data
 * @param  length   Number of bytes to receive
 * @param  timeout  Timeout in milliseconds (0 = no wait)
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t DRV_UART_Receive(uart_handle_t *handle, uint8_t *data, 
                              uint16_t length, uint32_t timeout);

/**
 * @brief  Start interrupt-driven transmission
 * @param  handle  Pointer to UART handle
 * @param  data    Data buffer to transmit
 * @param  length  Number of bytes to transmit
 * @return DRV_OK on success, DRV_BUSY if busy
 */
drv_status_t DRV_UART_TransmitIT(uart_handle_t *handle, const uint8_t *data, 
                                  uint16_t length);

/**
 * @brief  Start interrupt-driven reception
 * @param  handle  Pointer to UART handle
 * @param  data    Buffer to receive data
 * @param  length  Number of bytes to receive
 * @return DRV_OK on success, DRV_BUSY if busy
 */
drv_status_t DRV_UART_ReceiveIT(uart_handle_t *handle, uint8_t *data, 
                                 uint16_t length);

/**
 * @brief  Transmit single byte (blocking)
 * @param  handle  Pointer to UART handle
 * @param  byte    Byte to transmit
 * @return DRV_OK on success
 */
drv_status_t DRV_UART_TransmitByte(uart_handle_t *handle, uint8_t byte);

/**
 * @brief  Receive single byte (blocking with timeout)
 * @param  handle   Pointer to UART handle
 * @param  byte     Pointer to store received byte
 * @param  timeout  Timeout count
 * @return DRV_OK on success, DRV_TIMEOUT if timed out
 */
drv_status_t DRV_UART_ReceiveByte(uart_handle_t *handle, uint8_t *byte, 
                                   uint32_t timeout);

/**
 * @brief  Check if TX is busy
 * @param  handle  Pointer to UART handle
 * @return 1 if busy, 0 if idle
 */
uint8_t DRV_UART_IsTxBusy(uart_handle_t *handle);

/**
 * @brief  Check if RX has data available
 * @param  handle  Pointer to UART handle
 * @return 1 if data available, 0 if empty
 */
uint8_t DRV_UART_IsRxReady(uart_handle_t *handle);

/**
 * @brief  Abort ongoing transmission
 * @param  handle  Pointer to UART handle
 * @return DRV_OK on success
 */
drv_status_t DRV_UART_AbortTransmit(uart_handle_t *handle);

/**
 * @brief  Abort ongoing reception
 * @param  handle  Pointer to UART handle
 * @return DRV_OK on success
 */
drv_status_t DRV_UART_AbortReceive(uart_handle_t *handle);

/*---------------------------------------------------------------------------
 * Printf Support
 *---------------------------------------------------------------------------*/

/**
 * @brief  Send formatted string (printf-style)
 * @param  handle  Pointer to UART handle
 * @param  format  Format string
 * @param  ...     Variable arguments
 * @return Number of characters sent
 * 
 * @code
 *   DRV_UART_Printf(&uart0, "Value: %d\r\n", value);
 * @endcode
 */
int DRV_UART_Printf(uart_handle_t *handle, const char *format, ...);

/**
 * @brief  Send null-terminated string
 * @param  handle  Pointer to UART handle
 * @param  str     String to send
 * @return DRV_OK on success
 */
drv_status_t DRV_UART_SendString(uart_handle_t *handle, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
