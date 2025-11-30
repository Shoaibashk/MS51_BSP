/**
 *******************************************************************************
 * @file    ms51_hal_uart.h
 * @brief   MS51 HAL UART Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details UART Hardware Abstraction Layer for MS51 series.
 *          Supports UART0 and UART1 with configurable baud rates.
 *
 * Features:
 *   - UART0 and UART1 support
 *   - Multiple baud rate generators (Timer1, Timer3)
 *   - Polling and interrupt-based transmission
 *   - Configurable word length, stop bits, parity
 *   - Printf support
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_UART_H
#define __MS51_HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * UART Instance Definitions
 ******************************************************************************/

/**
 * @brief  UART Instance enumeration
 */
typedef enum {
    HAL_UART0 = 0,      /**< UART0 instance */
    HAL_UART1 = 1       /**< UART1 instance */
} HAL_UART_InstanceTypeDef;

/*******************************************************************************
 * UART Baud Rate Generator Definitions
 ******************************************************************************/

/**
 * @brief  UART Baud Rate Generator source
 */
typedef enum {
    HAL_UART_BRG_TIMER1 = 0,    /**< Use Timer1 as baud rate generator */
    HAL_UART_BRG_TIMER3 = 1     /**< Use Timer3 as baud rate generator */
} HAL_UART_BRGTypeDef;

/*******************************************************************************
 * UART Mode Definitions
 ******************************************************************************/

/**
 * @brief  UART Mode enumeration
 * @note   MS51 UART supports 4 modes:
 *         - Mode 0: Synchronous, 8-bit shift register (fixed baud = Fsys/12)
 *         - Mode 1: 8-bit UART, variable baud rate
 *         - Mode 2: 9-bit UART, fixed baud rate (Fsys/32 or Fsys/64)
 *         - Mode 3: 9-bit UART, variable baud rate
 */
typedef enum {
    HAL_UART_MODE_0 = 0x00,     /**< Synchronous mode, 8-bit */
    HAL_UART_MODE_1 = 0x40,     /**< 8-bit UART, variable baud */
    HAL_UART_MODE_2 = 0x80,     /**< 9-bit UART, fixed baud */
    HAL_UART_MODE_3 = 0xC0      /**< 9-bit UART, variable baud */
} HAL_UART_ModeTypeDef;

/*******************************************************************************
 * UART Configuration Definitions
 ******************************************************************************/

/**
 * @brief  UART Word Length
 */
typedef enum {
    HAL_UART_WORDLENGTH_8B = 0, /**< 8-bit data */
    HAL_UART_WORDLENGTH_9B = 1  /**< 9-bit data (for modes 2 and 3) */
} HAL_UART_WordLengthTypeDef;

/**
 * @brief  UART Parity (simulated with 9th bit in mode 2/3)
 */
typedef enum {
    HAL_UART_PARITY_NONE = 0,   /**< No parity */
    HAL_UART_PARITY_ODD = 1,    /**< Odd parity */
    HAL_UART_PARITY_EVEN = 2    /**< Even parity */
} HAL_UART_ParityTypeDef;

/*******************************************************************************
 * UART Handle Structure
 ******************************************************************************/

/**
 * @brief  UART Initialization Structure
 */
typedef struct {
    uint32_t                    BaudRate;       /**< Baud rate value */
    HAL_UART_BRGTypeDef         BaudRateGen;    /**< Baud rate generator source */
    HAL_UART_ModeTypeDef        Mode;           /**< UART mode */
    HAL_UART_WordLengthTypeDef  WordLength;     /**< Word length */
    HAL_UART_ParityTypeDef      Parity;         /**< Parity mode */
    uint8_t                     EnableRx;       /**< Enable receive (HAL_ENABLE/HAL_DISABLE) */
} HAL_UART_InitTypeDef;

/**
 * @brief  UART Handle Structure
 */
typedef struct {
    HAL_UART_InstanceTypeDef    Instance;       /**< UART instance (UART0/UART1) */
    HAL_UART_InitTypeDef        Init;           /**< UART configuration */
    volatile uint8_t            *pTxBuffer;     /**< Pointer to TX buffer */
    volatile uint16_t           TxCount;        /**< TX data count */
    volatile uint8_t            *pRxBuffer;     /**< Pointer to RX buffer */
    volatile uint16_t           RxCount;        /**< RX data count */
    HAL_LockTypeDef             Lock;           /**< Locking object */
    volatile uint8_t            TxBusy;         /**< TX busy flag */
    volatile uint8_t            RxBusy;         /**< RX busy flag */
} HAL_UART_HandleTypeDef;

/*******************************************************************************
 * UART Callback Type
 ******************************************************************************/

/**
 * @brief  UART Callback function pointer type
 */
typedef void (*HAL_UART_CallbackTypeDef)(HAL_UART_HandleTypeDef *huart);

/*******************************************************************************
 * UART Common Baud Rates
 ******************************************************************************/
#define HAL_UART_BAUD_9600      9600UL
#define HAL_UART_BAUD_19200     19200UL
#define HAL_UART_BAUD_38400     38400UL
#define HAL_UART_BAUD_57600     57600UL
#define HAL_UART_BAUD_115200    115200UL

/*******************************************************************************
 * UART Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize UART peripheral
 * @param  huart Pointer to UART handle structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_UART_HandleTypeDef huart0;
 *   huart0.Instance = HAL_UART0;
 *   huart0.Init.BaudRate = HAL_UART_BAUD_115200;
 *   huart0.Init.BaudRateGen = HAL_UART_BRG_TIMER1;
 *   huart0.Init.Mode = HAL_UART_MODE_1;
 *   huart0.Init.EnableRx = HAL_ENABLE;
 *   HAL_UART_Init(&huart0);
 */
HAL_StatusTypeDef HAL_UART_Init(HAL_UART_HandleTypeDef *huart);

/**
 * @brief  De-Initialize UART peripheral
 * @param  huart Pointer to UART handle structure
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_UART_DeInit(HAL_UART_HandleTypeDef *huart);

/**
 * @brief  Simple UART initialization for common use cases
 * @param  instance UART instance (HAL_UART0 or HAL_UART1)
 * @param  sysclk System clock frequency in Hz
 * @param  baudrate Desired baud rate
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_UART_SimpleInit(HAL_UART0, 24000000UL, 115200UL);
 */
HAL_StatusTypeDef HAL_UART_SimpleInit(HAL_UART_InstanceTypeDef instance, 
                                       uint32_t sysclk, uint32_t baudrate);

/**
 * @brief  Transmit single byte via UART (blocking)
 * @param  huart Pointer to UART handle structure
 * @param  data Byte to transmit
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_UART_TransmitByte(HAL_UART_HandleTypeDef *huart, uint8_t data);

/**
 * @brief  Receive single byte via UART (blocking)
 * @param  huart Pointer to UART handle structure
 * @param  pData Pointer to store received byte
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_UART_ReceiveByte(HAL_UART_HandleTypeDef *huart, uint8_t *pData, 
                                        uint32_t timeout);

/**
 * @brief  Transmit data buffer via UART (blocking)
 * @param  huart Pointer to UART handle structure
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to transmit
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   uint8_t buffer[] = "Hello World!";
 *   HAL_UART_Transmit(&huart0, buffer, sizeof(buffer)-1, 1000);
 */
HAL_StatusTypeDef HAL_UART_Transmit(HAL_UART_HandleTypeDef *huart, uint8_t *pData, 
                                     uint16_t size, uint32_t timeout);

/**
 * @brief  Receive data buffer via UART (blocking)
 * @param  huart Pointer to UART handle structure
 * @param  pData Pointer to data buffer
 * @param  size Number of bytes to receive
 * @param  timeout Timeout in milliseconds
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_UART_Receive(HAL_UART_HandleTypeDef *huart, uint8_t *pData, 
                                    uint16_t size, uint32_t timeout);

/**
 * @brief  Print null-terminated string via UART
 * @param  huart Pointer to UART handle structure
 * @param  str Null-terminated string
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_UART_PrintString(HAL_UART_HandleTypeDef *huart, const char *str);

/**
 * @brief  Enable UART receive interrupt
 * @param  instance UART instance
 * @retval None
 */
void HAL_UART_EnableRxInterrupt(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Disable UART receive interrupt
 * @param  instance UART instance
 * @retval None
 */
void HAL_UART_DisableRxInterrupt(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Enable UART transmit interrupt
 * @param  instance UART instance
 * @retval None
 */
void HAL_UART_EnableTxInterrupt(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Disable UART transmit interrupt
 * @param  instance UART instance
 * @retval None
 */
void HAL_UART_DisableTxInterrupt(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Check if UART receive buffer has data
 * @param  instance UART instance
 * @retval uint8_t 1 if data available, 0 otherwise
 */
uint8_t HAL_UART_IsRxReady(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Check if UART transmit buffer is empty (ready to transmit)
 * @param  instance UART instance
 * @retval uint8_t 1 if ready, 0 otherwise
 */
uint8_t HAL_UART_IsTxReady(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Enable printf on UART
 * @param  instance UART instance to use for printf
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_UART_EnablePrintf(HAL_UART_InstanceTypeDef instance);

/**
 * @brief  Quick initialization for UART0 with printf support at 115200 baud
 * @param  sysclk System clock frequency (e.g., 24000000UL for 24MHz)
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   This is a convenience function that sets up UART0 with:
 *         - 115200 baud rate
 *         - 8-bit data, no parity
 *         - Timer1 as baud rate generator
 *         - Printf enabled
 * 
 * @example
 *   HAL_UART_InitPrintf(24000000UL);  // For 24MHz system clock
 *   printf("Hello World!\n");
 */
HAL_StatusTypeDef HAL_UART_InitPrintf(uint32_t sysclk);

/*******************************************************************************
 * UART Interrupt Handler (to be called from ISR)
 ******************************************************************************/

/**
 * @brief  UART0 interrupt handler
 * @param  huart Pointer to UART handle structure
 * @note   Call this from UART0 ISR
 */
void HAL_UART0_IRQHandler(HAL_UART_HandleTypeDef *huart);

/**
 * @brief  UART1 interrupt handler
 * @param  huart Pointer to UART handle structure
 * @note   Call this from UART1 ISR
 */
void HAL_UART1_IRQHandler(HAL_UART_HandleTypeDef *huart);

/*******************************************************************************
 * UART Callback Registration
 ******************************************************************************/

/**
 * @brief  Register UART RX complete callback
 * @param  huart Pointer to UART handle structure
 * @param  callback Callback function pointer
 * @retval None
 */
void HAL_UART_RegisterRxCallback(HAL_UART_HandleTypeDef *huart, 
                                  HAL_UART_CallbackTypeDef callback);

/**
 * @brief  Register UART TX complete callback
 * @param  huart Pointer to UART handle structure
 * @param  callback Callback function pointer
 * @retval None
 */
void HAL_UART_RegisterTxCallback(HAL_UART_HandleTypeDef *huart, 
                                  HAL_UART_CallbackTypeDef callback);

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_UART_H */
