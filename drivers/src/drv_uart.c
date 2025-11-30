/*===========================================================================
 * @file    drv_uart.c
 * @brief   UART Driver Implementation
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "drv_uart.h"
#include <string.h>
#include <stdarg.h>

/*---------------------------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------------------------*/
static uart_handle_t *uart0_handle = NULL;
static uart_handle_t *uart1_handle = NULL;

/*---------------------------------------------------------------------------
 * Private Macros
 *---------------------------------------------------------------------------*/

/* System clock assumption (can be overridden) */
#ifndef FSYS_HZ
#define FSYS_HZ     24000000UL
#endif

/*---------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
static void uart_config_timer1(uint32_t baudrate);
static void uart_config_timer3(uint32_t baudrate);
static void uart0_config_pins(void);
static void uart1_config_pins(void);

/*===========================================================================
 * Public Functions
 *===========================================================================*/

/**
 * @brief  Initialize UART peripheral
 */
drv_status_t DRV_UART_Init(uart_handle_t *handle, const uart_config_t *config)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(config);
    
    /* Clear handle */
    memset(handle, 0, sizeof(uart_handle_t));
    handle->instance = config->instance;
    handle->rx_callback = config->rx_callback;
    handle->tx_callback = config->tx_callback;
    
    if (config->instance == UART_0) {
        /* Store handle reference for ISR */
        uart0_handle = handle;
        
        /* Configure UART0 pins */
        uart0_config_pins();
        
        /* Configure baud rate generator */
        if (config->timer == UART_TIMER_1) {
            uart_config_timer1(config->baudrate);
            /* T1 as baud rate generator for UART0 */
            clr_T3CON_BRCK;
        } else {
            uart_config_timer3(config->baudrate);
            /* T3 as baud rate generator for UART0 */
            set_T3CON_BRCK;
        }
        
        /* Configure UART0 for 8-bit mode, RX enabled */
        SCON = 0x50;  /* Mode 1 (8-bit UART), REN=1 */
        
        /* Clear TI/RI flags */
        clr_SCON_TI;
        clr_SCON_RI;
        
        /* Enable interrupt if needed */
        if (config->mode == UART_MODE_INTERRUPT) {
            set_IE_ES;  /* Enable UART0 interrupt */
        }
        
    } else if (config->instance == UART_1) {
        /* Store handle reference for ISR */
        uart1_handle = handle;
        
        /* Configure UART1 pins */
        uart1_config_pins();
        
        /* Configure Timer3 for UART1 baud rate */
        uart_config_timer3(config->baudrate);
        
        /* Set SM1_1 for 8-bit UART mode */
        set_S1CON_SM1_1;
        
        /* Enable reception */
        set_S1CON_REN_1;
        
        /* Clear flags */
        clr_S1CON_TI_1;
        clr_S1CON_RI_1;
        
        /* Enable interrupt if needed */
        if (config->mode == UART_MODE_INTERRUPT) {
            set_EIE_ES_1;  /* Enable UART1 interrupt */
        }
    } else {
        return DRV_INVALID_PARAM;
    }
    
    return DRV_OK;
}

/**
 * @brief  De-initialize UART peripheral
 */
drv_status_t DRV_UART_DeInit(uart_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    if (handle->instance == UART_0) {
        /* Disable UART0 interrupt */
        clr_IE_ES;
        
        /* Reset SCON */
        SCON = 0x00;
        
        uart0_handle = NULL;
        
    } else if (handle->instance == UART_1) {
        /* Disable UART1 interrupt */
        clr_EIE_ES_1;
        
        /* Reset S1CON */
        S1CON = 0x00;
        
        uart1_handle = NULL;
    }
    
    return DRV_OK;
}

/**
 * @brief  Register event callback
 */
drv_status_t DRV_UART_RegisterCallback(uart_handle_t *handle, drv_callback_t callback)
{
    DRV_CHECK_PTR(handle);
    
    handle->callback = callback;
    
    return DRV_OK;
}

/**
 * @brief  Transmit data (blocking)
 */
drv_status_t DRV_UART_Transmit(uart_handle_t *handle, const uint8_t *data, 
                               uint16_t length, uint32_t timeout)
{
    uint16_t i;
    
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(data);
    
    if (handle->instance == UART_0) {
        for (i = 0; i < length; i++) {
            SBUF = data[i];
            
            /* Wait for transmission complete */
            while (!TI) {
                /* Could add timeout check here */
            }
            clr_SCON_TI;
        }
    } else {
        for (i = 0; i < length; i++) {
            S1BUF = data[i];
            
            /* Wait for transmission complete */
            while (!(S1CON & 0x02)) {  /* TI_1 */
                /* Could add timeout check here */
            }
            clr_S1CON_TI_1;
        }
    }
    
    return DRV_OK;
}

/**
 * @brief  Receive data (blocking)
 */
drv_status_t DRV_UART_Receive(uart_handle_t *handle, uint8_t *data, 
                              uint16_t length, uint32_t timeout)
{
    uint16_t i;
    uint32_t count;
    
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(data);
    
    if (handle->instance == UART_0) {
        for (i = 0; i < length; i++) {
            count = timeout;
            
            /* Wait for data */
            while (!RI) {
                if (timeout && (--count == 0)) {
                    return DRV_TIMEOUT;
                }
            }
            
            data[i] = SBUF;
            clr_SCON_RI;
        }
    } else {
        for (i = 0; i < length; i++) {
            count = timeout;
            
            /* Wait for data */
            while (!(S1CON & 0x01)) {  /* RI_1 */
                if (timeout && (--count == 0)) {
                    return DRV_TIMEOUT;
                }
            }
            
            data[i] = S1BUF;
            clr_S1CON_RI_1;
        }
    }
    
    return DRV_OK;
}

/**
 * @brief  Start interrupt-driven transmission
 */
drv_status_t DRV_UART_TransmitIT(uart_handle_t *handle, const uint8_t *data, 
                                  uint16_t length)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(data);
    
    if (handle->tx_busy) {
        return DRV_BUSY;
    }
    
    handle->tx_buffer = (volatile uint8_t *)data;
    handle->tx_length = length;
    handle->tx_count = 0;
    handle->tx_busy = 1;
    
    /* Start transmission with first byte */
    if (handle->instance == UART_0) {
        set_IE_ES;  /* Enable interrupt */
        SBUF = handle->tx_buffer[handle->tx_count++];
    } else {
        set_EIE_ES_1;
        S1BUF = handle->tx_buffer[handle->tx_count++];
    }
    
    return DRV_OK;
}

/**
 * @brief  Start interrupt-driven reception
 */
drv_status_t DRV_UART_ReceiveIT(uart_handle_t *handle, uint8_t *data, 
                                 uint16_t length)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(data);
    
    if (handle->rx_busy) {
        return DRV_BUSY;
    }
    
    handle->rx_buffer = (volatile uint8_t *)data;
    handle->rx_length = length;
    handle->rx_count = 0;
    handle->rx_busy = 1;
    
    /* Enable receive interrupt */
    if (handle->instance == UART_0) {
        set_IE_ES;
    } else {
        set_EIE_ES_1;
    }
    
    return DRV_OK;
}

/**
 * @brief  Transmit single byte (blocking)
 */
drv_status_t DRV_UART_TransmitByte(uart_handle_t *handle, uint8_t byte)
{
    DRV_CHECK_PTR(handle);
    
    if (handle->instance == UART_0) {
        SBUF = byte;
        while (!TI);
        clr_SCON_TI;
    } else {
        S1BUF = byte;
        while (!(S1CON & 0x02));
        clr_S1CON_TI_1;
    }
    
    return DRV_OK;
}

/**
 * @brief  Receive single byte (blocking with timeout)
 */
drv_status_t DRV_UART_ReceiveByte(uart_handle_t *handle, uint8_t *byte, 
                                   uint32_t timeout)
{
    uint32_t count = timeout;
    
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(byte);
    
    if (handle->instance == UART_0) {
        while (!RI) {
            if (timeout && (--count == 0)) {
                return DRV_TIMEOUT;
            }
        }
        *byte = SBUF;
        clr_SCON_RI;
    } else {
        while (!(S1CON & 0x01)) {
            if (timeout && (--count == 0)) {
                return DRV_TIMEOUT;
            }
        }
        *byte = S1BUF;
        clr_S1CON_RI_1;
    }
    
    return DRV_OK;
}

/**
 * @brief  Check if TX is busy
 */
uint8_t DRV_UART_IsTxBusy(uart_handle_t *handle)
{
    if (handle == NULL) return 0;
    return handle->tx_busy;
}

/**
 * @brief  Check if RX has data available
 */
uint8_t DRV_UART_IsRxReady(uart_handle_t *handle)
{
    if (handle == NULL) return 0;
    
    if (handle->instance == UART_0) {
        return RI ? 1 : 0;
    } else {
        return (S1CON & 0x01) ? 1 : 0;
    }
}

/**
 * @brief  Abort ongoing transmission
 */
drv_status_t DRV_UART_AbortTransmit(uart_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    handle->tx_busy = 0;
    handle->tx_count = 0;
    handle->tx_length = 0;
    
    return DRV_OK;
}

/**
 * @brief  Abort ongoing reception
 */
drv_status_t DRV_UART_AbortReceive(uart_handle_t *handle)
{
    DRV_CHECK_PTR(handle);
    
    handle->rx_busy = 0;
    handle->rx_count = 0;
    handle->rx_length = 0;
    
    return DRV_OK;
}

/**
 * @brief  Send null-terminated string
 */
drv_status_t DRV_UART_SendString(uart_handle_t *handle, const char *str)
{
    DRV_CHECK_PTR(handle);
    DRV_CHECK_PTR(str);
    
    while (*str) {
        DRV_UART_TransmitByte(handle, *str++);
    }
    
    return DRV_OK;
}

/**
 * @brief  Send formatted string (simplified printf)
 */
int DRV_UART_Printf(uart_handle_t *handle, const char *format, ...)
{
    char buffer[128];
    va_list args;
    int len;
    
    if (handle == NULL || format == NULL) {
        return -1;
    }
    
    va_start(args, format);
    
    /* Note: vsprintf not available in all embedded compilers */
    /* This is a simplified implementation */
    len = 0;
    (void)args;
    (void)buffer;
    
    /* Send the format string as-is for now */
    /* Real implementation would use vsnprintf */
    DRV_UART_SendString(handle, format);
    
    va_end(args);
    
    return len;
}

/*===========================================================================
 * Private Functions
 *===========================================================================*/

/**
 * @brief  Configure Timer1 for UART baud rate
 */
static void uart_config_timer1(uint32_t baudrate)
{
    uint16_t timer_reload;
    
    /* Timer1 in Mode 2 (8-bit auto-reload) */
    TMOD = (TMOD & 0x0F) | 0x20;
    
    /* Calculate reload value: 256 - (FSYS / 32 / 12 / baudrate) */
    /* SMOD=1 doubles baud rate */
    set_PCON_SMOD;
    
    timer_reload = (uint16_t)(256 - (FSYS_HZ / 32 / baudrate));
    
    TH1 = (uint8_t)timer_reload;
    TL1 = (uint8_t)timer_reload;
    
    /* Start Timer1 */
    set_TCON_TR1;
}

/**
 * @brief  Configure Timer3 for UART baud rate
 */
static void uart_config_timer3(uint32_t baudrate)
{
    uint16_t timer_reload;
    
    /* T3 in Mode 0 (auto-reload 16-bit timer) */
    T3CON = 0x00;
    
    /* Calculate reload value: 65536 - (FSYS / 32 / baudrate) */
    timer_reload = (uint16_t)(65536 - (FSYS_HZ / 32 / baudrate));
    
    RH3 = (uint8_t)(timer_reload >> 8);   /* High byte */
    RL3 = (uint8_t)(timer_reload & 0xFF); /* Low byte */
    
    /* Start Timer3 */
    set_T3CON_TR3;
}

/**
 * @brief  Configure UART0 pins
 */
static void uart0_config_pins(void)
{
    /* Default UART0 pins: P0.6=TXD0, P0.7=RXD0 */
    /* Set TXD0 as push-pull output */
    P0M1 &= ~0x40;
    P0M2 |=  0x40;
    
    /* Set RXD0 as input */
    P0M1 |=  0x80;
    P0M2 &= ~0x80;
}

/**
 * @brief  Configure UART1 pins
 */
static void uart1_config_pins(void)
{
    /* Default UART1 pins: P1.6=TXD1, P1.7=RXD1 */
    /* Set TXD1 as push-pull output */
    P1M1 &= ~0x40;
    P1M2 |=  0x40;
    
    /* Set RXD1 as input */
    P1M1 |=  0x80;
    P1M2 &= ~0x80;
}

/*===========================================================================
 * UART Interrupt Service Routines
 *===========================================================================*/

/**
 * @brief  UART0 Interrupt Handler
 */
#if defined(COMPILER_KEIL)
void UART0_ISR(void) interrupt 4
#elif defined(COMPILER_IAR)
#pragma vector = 0x23
__interrupt void UART0_ISR(void)
#elif defined(COMPILER_SDCC)
void UART0_ISR(void) __interrupt(4)
#endif
#if !defined(COMPILER_GCC)
{
    uart_handle_t *handle = uart0_handle;
    
    if (handle == NULL) {
        clr_SCON_TI;
        clr_SCON_RI;
        return;
    }
    
    /* Transmit interrupt */
    if (TI) {
        clr_SCON_TI;
        
        if (handle->tx_busy) {
            if (handle->tx_count < handle->tx_length) {
                /* Send next byte */
                SBUF = handle->tx_buffer[handle->tx_count++];
            } else {
                /* Transmission complete */
                handle->tx_busy = 0;
                
                if (handle->tx_callback) {
                    handle->tx_callback();
                } else if (handle->callback) {
                    handle->callback(UART_EVT_TX_COMPLETE, NULL);
                }
            }
        }
    }
    
    /* Receive interrupt */
    if (RI) {
        clr_SCON_RI;
        
        uint8_t byte = SBUF;
        
        if (handle->rx_busy) {
            handle->rx_buffer[handle->rx_count++] = byte;
            
            if (handle->rx_count >= handle->rx_length) {
                handle->rx_busy = 0;
                
                if (handle->callback) {
                    handle->callback(UART_EVT_RX_DATA, NULL);
                }
            }
        } else {
            /* Call per-byte RX callback if registered */
            if (handle->rx_callback) {
                handle->rx_callback(byte);
            } else if (handle->callback) {
                handle->callback(UART_EVT_RX_DATA, &byte);
            }
        }
    }
}
#endif

/**
 * @brief  UART1 Interrupt Handler
 */
#if defined(COMPILER_KEIL)
void UART1_ISR(void) interrupt 15
#elif defined(COMPILER_IAR)
#pragma vector = 0x7B
__interrupt void UART1_ISR(void)
#elif defined(COMPILER_SDCC)
void UART1_ISR(void) __interrupt(15)
#endif
#if !defined(COMPILER_GCC)
{
    uart_handle_t *handle = uart1_handle;
    
    if (handle == NULL) {
        clr_S1CON_TI_1;
        clr_S1CON_RI_1;
        return;
    }
    
    /* Transmit interrupt */
    if (S1CON & 0x02) {  /* TI_1 */
        clr_S1CON_TI_1;
        
        if (handle->tx_busy) {
            if (handle->tx_count < handle->tx_length) {
                S1BUF = handle->tx_buffer[handle->tx_count++];
            } else {
                handle->tx_busy = 0;
                
                if (handle->tx_callback) {
                    handle->tx_callback();
                } else if (handle->callback) {
                    handle->callback(UART_EVT_TX_COMPLETE, NULL);
                }
            }
        }
    }
    
    /* Receive interrupt */
    if (S1CON & 0x01) {  /* RI_1 */
        clr_S1CON_RI_1;
        
        uint8_t byte = S1BUF;
        
        if (handle->rx_busy) {
            handle->rx_buffer[handle->rx_count++] = byte;
            
            if (handle->rx_count >= handle->rx_length) {
                handle->rx_busy = 0;
                
                if (handle->callback) {
                    handle->callback(UART_EVT_RX_DATA, NULL);
                }
            }
        } else {
            /* Call per-byte RX callback if registered */
            if (handle->rx_callback) {
                handle->rx_callback(byte);
            } else if (handle->callback) {
                handle->callback(UART_EVT_RX_DATA, &byte);
            }
        }
    }
}
#endif
