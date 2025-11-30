/*===========================================================================
 * @file    drv_gpio.h
 * @brief   GPIO Driver Interface
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details General Purpose I/O driver for MS51 microcontrollers.
 *          Supports all GPIO modes: quasi-bidirectional, push-pull,
 *          input-only, and open-drain.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*---------------------------------------------------------------------------
 * GPIO Port Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    GPIO_PORT_0 = 0,    /**< Port 0 (8 pins: P0.0-P0.7) */
    GPIO_PORT_1 = 1,    /**< Port 1 (8 pins: P1.0-P1.7) */
    GPIO_PORT_2 = 2,    /**< Port 2 (1 pin: P2.0) */
    GPIO_PORT_3 = 3     /**< Port 3 (1 pin: P3.0) */
} gpio_port_t;

/*---------------------------------------------------------------------------
 * GPIO Pin Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    GPIO_PIN_0 = 0x01,  /**< Pin 0 */
    GPIO_PIN_1 = 0x02,  /**< Pin 1 */
    GPIO_PIN_2 = 0x04,  /**< Pin 2 */
    GPIO_PIN_3 = 0x08,  /**< Pin 3 */
    GPIO_PIN_4 = 0x10,  /**< Pin 4 */
    GPIO_PIN_5 = 0x20,  /**< Pin 5 */
    GPIO_PIN_6 = 0x40,  /**< Pin 6 */
    GPIO_PIN_7 = 0x80,  /**< Pin 7 */
    GPIO_PIN_ALL = 0xFF /**< All pins */
} gpio_pin_t;

/*---------------------------------------------------------------------------
 * GPIO Mode Definitions
 *---------------------------------------------------------------------------*/
typedef enum {
    GPIO_MODE_QUASI       = 0,  /**< Quasi-bidirectional (default) */
    GPIO_MODE_QUASI_BIDIR = 0,  /**< Alias for QUASI */
    GPIO_MODE_PUSH_PULL   = 1,  /**< Push-pull output */
    GPIO_MODE_OUTPUT      = 1,  /**< Alias for PUSH_PULL */
    GPIO_MODE_INPUT       = 2,  /**< Input only (high-impedance) */
    GPIO_MODE_INPUT_ONLY  = 2,  /**< Alias for INPUT */
    GPIO_MODE_INPUT_PULLUP= 2,  /**< Input with pull-up (set PULL_UP separately) */
    GPIO_MODE_OPEN_DRAIN  = 3   /**< Open-drain */
} gpio_mode_t;

/*---------------------------------------------------------------------------
 * GPIO Pull-up Configuration (for Schmitt trigger inputs)
 *---------------------------------------------------------------------------*/
typedef enum {
    GPIO_PULL_NONE = 0,         /**< No pull-up/pull-down */
    GPIO_PULL_UP   = 1          /**< Pull-up enabled (via Schmitt trigger) */
} gpio_pull_t;

/*---------------------------------------------------------------------------
 * GPIO Interrupt Edge
 *---------------------------------------------------------------------------*/
typedef enum {
    GPIO_INT_DISABLE = 0,       /**< Interrupt disabled */
    GPIO_INT_NONE    = 0,       /**< Alias for DISABLE */
    GPIO_INT_FALLING = 1,       /**< Falling edge trigger */
    GPIO_INT_RISING  = 2,       /**< Rising edge trigger */
    GPIO_INT_BOTH    = 3,       /**< Both edges trigger */
    GPIO_INT_LOW     = 4        /**< Low level trigger */
} gpio_int_edge_t;

/*---------------------------------------------------------------------------
 * GPIO Callback Type
 *---------------------------------------------------------------------------*/
typedef void (*gpio_callback_t)(void *context);

/*---------------------------------------------------------------------------
 * GPIO Configuration Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    gpio_port_t     port;           /**< Port number */
    uint8_t         pin;            /**< Pin number (0-7) */
    gpio_mode_t     mode;           /**< Pin mode */
    gpio_pull_t     pull;           /**< Pull-up configuration */
    gpio_int_edge_t int_edge;       /**< Interrupt edge */
    uint8_t         init_value;     /**< Initial output value (0 or 1) */
    gpio_callback_t callback;       /**< Interrupt callback */
} gpio_config_t;

/*---------------------------------------------------------------------------
 * GPIO Handle Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    gpio_port_t     port;           /**< Port number */
    uint8_t         pin;            /**< Pin number (0-7) */
    gpio_mode_t     mode;           /**< Pin mode */
    gpio_callback_t callback;       /**< Event callback */
    void           *context;        /**< User context for callback */
    uint8_t         int_enabled;    /**< Interrupt enabled flag */
} gpio_handle_t;

/*---------------------------------------------------------------------------
 * GPIO Events (for callbacks)
 *---------------------------------------------------------------------------*/
typedef enum {
    GPIO_EVT_PIN_CHANGE = 0     /**< Pin state changed */
} gpio_event_t;

/*===========================================================================
 * Public API Functions (Handle-based)
 *===========================================================================*/

/**
 * @brief  Initialize GPIO pin with handle
 * @param  handle  Pointer to GPIO handle
 * @param  config  Pointer to GPIO configuration
 * @return DRV_OK on success, error code otherwise
 * 
 * @code
 *   gpio_handle_t led;
 *   gpio_config_t led_config = {
 *       .port = GPIO_PORT_1,
 *       .pin  = 4,
 *       .mode = GPIO_MODE_PUSH_PULL,
 *       .pull = GPIO_PULL_NONE,
 *       .int_edge = GPIO_INT_DISABLE,
 *       .callback = NULL
 *   };
 *   DRV_GPIO_Init(&led, &led_config);
 * @endcode
 */
drv_status_t DRV_GPIO_Init(gpio_handle_t *handle, const gpio_config_t *config);

/**
 * @brief  De-initialize GPIO pin to default state
 * @param  handle  Pointer to GPIO handle
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_DeInit(gpio_handle_t *handle);

/**
 * @brief  Write value to GPIO pin
 * @param  handle  Pointer to GPIO handle
 * @param  value   0 = low, non-zero = high
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_Write(gpio_handle_t *handle, uint8_t value);

/**
 * @brief  Read GPIO pin state
 * @param  handle  Pointer to GPIO handle
 * @return Pin state (0 or 1)
 */
uint8_t DRV_GPIO_Read(gpio_handle_t *handle);

/**
 * @brief  Toggle GPIO pin
 * @param  handle  Pointer to GPIO handle
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_Toggle(gpio_handle_t *handle);

/**
 * @brief  Enable interrupt for GPIO pin
 * @param  handle  Pointer to GPIO handle
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_IntEnable(gpio_handle_t *handle);

/**
 * @brief  Disable interrupt for GPIO pin
 * @param  handle  Pointer to GPIO handle
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_IntDisable(gpio_handle_t *handle);

/**
 * @brief  Register callback for GPIO pin
 * @param  handle    Pointer to GPIO handle
 * @param  callback  Callback function
 * @param  context   User context passed to callback
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_RegisterCallback(gpio_handle_t *handle, gpio_callback_t callback, void *context);

/* Compatibility aliases */
#define DRV_GPIO_EnableInterrupt    DRV_GPIO_IntEnable
#define DRV_GPIO_DisableInterrupt   DRV_GPIO_IntDisable

/*===========================================================================
 * Legacy API Functions (Direct access without handle)
 *===========================================================================*/

/**
 * @brief  Initialize GPIO pin(s) - legacy API
 * @param  config  Pointer to GPIO configuration
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t DRV_GPIO_InitLegacy(const gpio_config_t *config);

/**
 * @brief  Write value to GPIO pin(s) - direct access
 * @param  port   Port number
 * @param  pin    Pin mask
 * @param  value  0 = low, non-zero = high
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_WriteDirect(gpio_port_t port, uint8_t pin, uint8_t value);

/**
 * @brief  Read GPIO pin state - direct access
 * @param  port  Port number
 * @param  pin   Pin number (0-7)
 * @return Pin state (0 or 1), or 0xFF on error
 */
uint8_t DRV_GPIO_ReadDirect(gpio_port_t port, uint8_t pin);

/**
 * @brief  Toggle GPIO pin(s) - direct access
 * @param  port  Port number
 * @param  pin   Pin mask
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_ToggleDirect(gpio_port_t port, uint8_t pin);

/**
 * @brief  Write entire port value
 * @param  port   Port number
 * @param  value  8-bit port value
 * @return DRV_OK on success
 */
drv_status_t DRV_GPIO_WritePort(gpio_port_t port, uint8_t value);

/**
 * @brief  Read entire port value
 * @param  port  Port number
 * @return 8-bit port value
 */
uint8_t DRV_GPIO_ReadPort(gpio_port_t port);

/*---------------------------------------------------------------------------
 * Quick Pin Access Macros
 *---------------------------------------------------------------------------*/

/* LED on NuTiny board (P1.4) */
#define LED_PIN                 4
#define LED_PORT                GPIO_PORT_1
#define LED_ON()                DRV_GPIO_WriteDirect(LED_PORT, (1<<LED_PIN), 0)
#define LED_OFF()               DRV_GPIO_WriteDirect(LED_PORT, (1<<LED_PIN), 1)
#define LED_TOGGLE()            DRV_GPIO_ToggleDirect(LED_PORT, (1<<LED_PIN))

#ifdef __cplusplus
}
#endif

#endif /* DRV_GPIO_H */
