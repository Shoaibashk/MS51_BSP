/**
 *******************************************************************************
 * @file    ms51_hal_uart.c
 * @brief   MS51 HAL UART Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_UART_MODULE_ENABLED

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/* Printf output flag */
static uint8_t g_printfEnabled = 0;
static HAL_UART_InstanceTypeDef g_printfUart = HAL_UART0;

/* Callback function pointers */
static HAL_UART_CallbackTypeDef g_uart0RxCallback = NULL;
static HAL_UART_CallbackTypeDef g_uart0TxCallback = NULL;
static HAL_UART_CallbackTypeDef g_uart1RxCallback = NULL;
static HAL_UART_CallbackTypeDef g_uart1TxCallback = NULL;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize UART peripheral
 */
HAL_StatusTypeDef HAL_UART_Init(HAL_UART_HandleTypeDef *huart)
{
    uint32_t sysclk;
    uint16_t timerReload;
    
    if (huart == NULL) {
        return HAL_INVALID;
    }
    
    /* Get system clock from HAL config */
    sysclk = HAL_SYSCLK_FREQ;
    
    if (huart->Instance == HAL_UART0) {
        /* Configure UART0 */
        SCON = 0x50;  /* Mode 1, REN = 1 */
        
        if (huart->Init.BaudRateGen == HAL_UART_BRG_TIMER1) {
            /* Use Timer1 as baud rate generator */
            TMOD |= 0x20;      /* Timer1 Mode 2 (8-bit auto-reload) */
            PCON |= 0x80;      /* SMOD = 1, double baud rate */
            CKCON |= 0x10;     /* T1M = 1, Timer1 clock = Fsys */
            T3CON &= ~0x20;    /* BRCK = 0, use Timer1 for UART0 */
            
            /* Calculate Timer1 reload value */
            TH1 = (uint8_t)(256 - (sysclk / 16 / huart->Init.BaudRate));
            TL1 = TH1;
            
            TR1 = 1;           /* Start Timer1 */
        } else {
            /* Use Timer3 as baud rate generator */
            PCON |= 0x80;      /* SMOD = 1 */
            T3CON = 0x00;      /* Clear T3CON */
            T3CON |= 0x20;     /* BRCK = 1, use Timer3 for UART0 */
            
            /* Calculate Timer3 reload value (16-bit) */
            timerReload = (uint16_t)(65536 - (sysclk / 16 / huart->Init.BaudRate));
            RH3 = HAL_HIBYTE(timerReload);
            RL3 = HAL_LOBYTE(timerReload);
            
            T3CON |= 0x08;     /* TR3 = 1, start Timer3 */
        }
        
        if (huart->Init.EnableRx) {
            REN = 1;           /* Enable receiver */
        }
        
    } else if (huart->Instance == HAL_UART1) {
        /* Configure UART1 */
        SCON_1 = 0x50;     /* Mode 1, REN_1 = 1 */
        T3CON = 0x80;      /* UART1 Mode 1, Timer3 as baud rate generator */
        
        /* Calculate Timer3 reload value */
        timerReload = (uint16_t)(65536 - (sysclk / 16 / huart->Init.BaudRate));
        RH3 = HAL_HIBYTE(timerReload);
        RL3 = HAL_LOBYTE(timerReload);
        
        T3CON |= 0x08;     /* TR3 = 1, start Timer3 */
        
        if (huart->Init.EnableRx) {
            REN_1 = 1;     /* Enable receiver */
        }
    } else {
        return HAL_INVALID;
    }
    
    huart->Lock = HAL_UNLOCKED;
    huart->TxBusy = 0;
    huart->RxBusy = 0;
    
    return HAL_OK;
}

/**
 * @brief  De-Initialize UART peripheral
 */
HAL_StatusTypeDef HAL_UART_DeInit(HAL_UART_HandleTypeDef *huart)
{
    if (huart == NULL) {
        return HAL_INVALID;
    }
    
    if (huart->Instance == HAL_UART0) {
        SCON = 0x00;
        ES = 0;  /* Disable UART0 interrupt */
    } else if (huart->Instance == HAL_UART1) {
        SCON_1 = 0x00;
        EIE1 &= ~0x01;  /* Disable UART1 interrupt */
    }
    
    return HAL_OK;
}

/**
 * @brief  Simple UART initialization
 */
HAL_StatusTypeDef HAL_UART_SimpleInit(HAL_UART_InstanceTypeDef instance, 
                                       uint32_t sysclk, uint32_t baudrate)
{
    uint16_t timerReload;
    
    if (instance == HAL_UART0) {
        /* UART0 with Timer1 */
        SCON = 0x50;           /* Mode 1, REN = 1 */
        TMOD |= 0x20;          /* Timer1 Mode 2 */
        PCON |= 0x80;          /* SMOD = 1 */
        CKCON |= 0x10;         /* T1M = 1 */
        T3CON &= ~0x20;        /* Use Timer1 */
        
        TH1 = (uint8_t)(256 - (sysclk / 16 / baudrate));
        TL1 = TH1;
        TR1 = 1;
        
    } else if (instance == HAL_UART1) {
        /* UART1 with Timer3 */
        SCON_1 = 0x50;
        T3CON = 0x80;
        
        timerReload = (uint16_t)(65536 - (sysclk / 16 / baudrate));
        RH3 = HAL_HIBYTE(timerReload);
        RL3 = HAL_LOBYTE(timerReload);
        
        T3CON |= 0x08;
    } else {
        return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Transmit single byte via UART (blocking)
 */
HAL_StatusTypeDef HAL_UART_TransmitByte(HAL_UART_HandleTypeDef *huart, uint8_t data)
{
    if (huart == NULL) {
        return HAL_INVALID;
    }
    
    if (huart->Instance == HAL_UART0) {
        SBUF = data;
        while (!TI);
        TI = 0;
    } else if (huart->Instance == HAL_UART1) {
        SBUF_1 = data;
        while (!TI_1);
        TI_1 = 0;
    } else {
        return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Receive single byte via UART (blocking)
 */
HAL_StatusTypeDef HAL_UART_ReceiveByte(HAL_UART_HandleTypeDef *huart, uint8_t *pData, 
                                        uint32_t timeout)
{
    uint32_t tickstart;
    
    if (huart == NULL || pData == NULL) {
        return HAL_INVALID;
    }
    
    tickstart = HAL_GetTick();
    
    if (huart->Instance == HAL_UART0) {
        while (!RI) {
            if ((HAL_GetTick() - tickstart) > timeout) {
                return HAL_TIMEOUT;
            }
        }
        *pData = SBUF;
        RI = 0;
    } else if (huart->Instance == HAL_UART1) {
        while (!RI_1) {
            if ((HAL_GetTick() - tickstart) > timeout) {
                return HAL_TIMEOUT;
            }
        }
        *pData = SBUF_1;
        RI_1 = 0;
    } else {
        return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Transmit data buffer via UART (blocking)
 */
HAL_StatusTypeDef HAL_UART_Transmit(HAL_UART_HandleTypeDef *huart, uint8_t *pData, 
                                     uint16_t size, uint32_t timeout)
{
    uint16_t i;
    HAL_StatusTypeDef status;
    
    if (huart == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        status = HAL_UART_TransmitByte(huart, pData[i]);
        if (status != HAL_OK) {
            return status;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Receive data buffer via UART (blocking)
 */
HAL_StatusTypeDef HAL_UART_Receive(HAL_UART_HandleTypeDef *huart, uint8_t *pData, 
                                    uint16_t size, uint32_t timeout)
{
    uint16_t i;
    HAL_StatusTypeDef status;
    
    if (huart == NULL || pData == NULL || size == 0) {
        return HAL_INVALID;
    }
    
    for (i = 0; i < size; i++) {
        status = HAL_UART_ReceiveByte(huart, &pData[i], timeout);
        if (status != HAL_OK) {
            return status;
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  Print null-terminated string via UART
 */
HAL_StatusTypeDef HAL_UART_PrintString(HAL_UART_HandleTypeDef *huart, const char *str)
{
    if (huart == NULL || str == NULL) {
        return HAL_INVALID;
    }
    
    while (*str) {
        HAL_UART_TransmitByte(huart, (uint8_t)*str++);
    }
    
    return HAL_OK;
}

/**
 * @brief  Enable UART receive interrupt
 */
void HAL_UART_EnableRxInterrupt(HAL_UART_InstanceTypeDef instance)
{
    if (instance == HAL_UART0) {
        ES = 1;  /* Enable UART0 interrupt */
    } else if (instance == HAL_UART1) {
        EIE1 |= 0x01;  /* Enable UART1 interrupt */
    }
}

/**
 * @brief  Disable UART receive interrupt
 */
void HAL_UART_DisableRxInterrupt(HAL_UART_InstanceTypeDef instance)
{
    if (instance == HAL_UART0) {
        ES = 0;
    } else if (instance == HAL_UART1) {
        EIE1 &= ~0x01;
    }
}

/**
 * @brief  Enable UART transmit interrupt
 */
void HAL_UART_EnableTxInterrupt(HAL_UART_InstanceTypeDef instance)
{
    /* TX interrupt is same as RX in 8051 */
    HAL_UART_EnableRxInterrupt(instance);
}

/**
 * @brief  Disable UART transmit interrupt
 */
void HAL_UART_DisableTxInterrupt(HAL_UART_InstanceTypeDef instance)
{
    HAL_UART_DisableRxInterrupt(instance);
}

/**
 * @brief  Check if UART receive buffer has data
 */
uint8_t HAL_UART_IsRxReady(HAL_UART_InstanceTypeDef instance)
{
    if (instance == HAL_UART0) {
        return RI ? 1 : 0;
    } else if (instance == HAL_UART1) {
        return RI_1 ? 1 : 0;
    }
    return 0;
}

/**
 * @brief  Check if UART transmit buffer is empty
 */
uint8_t HAL_UART_IsTxReady(HAL_UART_InstanceTypeDef instance)
{
    if (instance == HAL_UART0) {
        return TI ? 1 : 0;
    } else if (instance == HAL_UART1) {
        return TI_1 ? 1 : 0;
    }
    return 0;
}

/**
 * @brief  Enable printf on UART
 */
HAL_StatusTypeDef HAL_UART_EnablePrintf(HAL_UART_InstanceTypeDef instance)
{
    g_printfEnabled = 1;
    g_printfUart = instance;
    
    /* Set TI to enable printf */
    if (instance == HAL_UART0) {
        TI = 1;
    } else if (instance == HAL_UART1) {
        TI_1 = 1;
    }
    
    return HAL_OK;
}

/**
 * @brief  Quick initialization for UART with printf support
 */
HAL_StatusTypeDef HAL_UART_InitPrintf(uint32_t sysclk)
{
    HAL_StatusTypeDef status;
    
    /* Configure P0.6 (TXD) as push-pull output */
    P0M1 &= ~0x40;
    P0M2 |= 0x40;
    
    /* Initialize UART0 at 115200 baud */
    status = HAL_UART_SimpleInit(HAL_UART0, sysclk, 115200);
    if (status != HAL_OK) {
        return status;
    }
    
    /* Enable printf */
    return HAL_UART_EnablePrintf(HAL_UART0);
}

/**
 * @brief  UART0 interrupt handler
 */
void HAL_UART0_IRQHandler(HAL_UART_HandleTypeDef *huart)
{
    if (RI) {
        RI = 0;
        if (g_uart0RxCallback != NULL) {
            g_uart0RxCallback(huart);
        }
    }
    
    if (TI) {
        if (!g_printfEnabled || g_printfUart != HAL_UART0) {
            TI = 0;
        }
        if (g_uart0TxCallback != NULL) {
            g_uart0TxCallback(huart);
        }
    }
}

/**
 * @brief  UART1 interrupt handler
 */
void HAL_UART1_IRQHandler(HAL_UART_HandleTypeDef *huart)
{
    if (RI_1) {
        RI_1 = 0;
        if (g_uart1RxCallback != NULL) {
            g_uart1RxCallback(huart);
        }
    }
    
    if (TI_1) {
        if (!g_printfEnabled || g_printfUart != HAL_UART1) {
            TI_1 = 0;
        }
        if (g_uart1TxCallback != NULL) {
            g_uart1TxCallback(huart);
        }
    }
}

/**
 * @brief  Register UART RX complete callback
 */
void HAL_UART_RegisterRxCallback(HAL_UART_HandleTypeDef *huart, 
                                  HAL_UART_CallbackTypeDef callback)
{
    if (huart->Instance == HAL_UART0) {
        g_uart0RxCallback = callback;
    } else if (huart->Instance == HAL_UART1) {
        g_uart1RxCallback = callback;
    }
}

/**
 * @brief  Register UART TX complete callback
 */
void HAL_UART_RegisterTxCallback(HAL_UART_HandleTypeDef *huart, 
                                  HAL_UART_CallbackTypeDef callback)
{
    if (huart->Instance == HAL_UART0) {
        g_uart0TxCallback = callback;
    } else if (huart->Instance == HAL_UART1) {
        g_uart1TxCallback = callback;
    }
}

/*******************************************************************************
 * Printf Support - putchar override
 ******************************************************************************/

#if defined(HAL_COMPILER_KEIL) || defined(HAL_COMPILER_IAR)
/**
 * @brief  putchar implementation for printf support
 */
char putchar(char c)
{
    if (g_printfEnabled) {
        if (g_printfUart == HAL_UART0) {
            TI = 0;
            SBUF = c;
            while (!TI);
        } else {
            TI_1 = 0;
            SBUF_1 = c;
            while (!TI_1);
        }
    }
    return c;
}

#elif defined(HAL_COMPILER_SDCC)
/**
 * @brief  putchar implementation for SDCC printf support
 */
int putchar(int c)
{
    if (g_printfEnabled) {
        if (g_printfUart == HAL_UART0) {
            TI = 0;
            SBUF = (uint8_t)c;
            while (!TI);
        } else {
            TI_1 = 0;
            SBUF_1 = (uint8_t)c;
            while (!TI_1);
        }
    }
    return c;
}
#endif

#endif /* HAL_UART_MODULE_ENABLED */
