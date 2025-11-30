/**
 *******************************************************************************
 * @file    ms51_hal_gpio.h
 * @brief   MS51 HAL GPIO Driver Header
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details GPIO Hardware Abstraction Layer for MS51 series.
 *          Provides unified API for GPIO configuration and control.
 *
 * Features:
 *   - Pin mode configuration (Input, Output, Quasi-bidirectional, Open-drain)
 *   - Pin read/write operations
 *   - Pin toggle
 *   - Schmitt trigger and slew rate configuration
 *   - Pin interrupt configuration
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_GPIO_H
#define __MS51_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ms51_hal.h"

/*******************************************************************************
 * GPIO Port Definitions
 ******************************************************************************/

/**
 * @brief  GPIO Port enumeration
 */
typedef enum {
    HAL_GPIO_PORT0 = 0,     /**< GPIO Port 0 */
    HAL_GPIO_PORT1 = 1,     /**< GPIO Port 1 */
    HAL_GPIO_PORT2 = 2,     /**< GPIO Port 2 (Only P2.0 available) */
    HAL_GPIO_PORT3 = 3      /**< GPIO Port 3 (Only P3.0 available) */
} HAL_GPIO_PortTypeDef;

/*******************************************************************************
 * GPIO Pin Definitions
 ******************************************************************************/

/**
 * @brief  GPIO Pin enumeration
 */
typedef enum {
    HAL_GPIO_PIN_0 = 0x01,  /**< Pin 0 */
    HAL_GPIO_PIN_1 = 0x02,  /**< Pin 1 */
    HAL_GPIO_PIN_2 = 0x04,  /**< Pin 2 */
    HAL_GPIO_PIN_3 = 0x08,  /**< Pin 3 */
    HAL_GPIO_PIN_4 = 0x10,  /**< Pin 4 */
    HAL_GPIO_PIN_5 = 0x20,  /**< Pin 5 */
    HAL_GPIO_PIN_6 = 0x40,  /**< Pin 6 */
    HAL_GPIO_PIN_7 = 0x80,  /**< Pin 7 */
    HAL_GPIO_PIN_ALL = 0xFF /**< All pins */
} HAL_GPIO_PinTypeDef;

/*******************************************************************************
 * GPIO Mode Definitions
 ******************************************************************************/

/**
 * @brief  GPIO Pin Mode enumeration
 * @note   MS51 has 4 GPIO modes controlled by P0M1/P0M2 (and similar for other ports)
 *         
 *         Mode Configuration Table:
 *         | PxM1.n | PxM2.n | Mode                  |
 *         |--------|--------|-----------------------|
 *         |   0    |   0    | Quasi-bidirectional   |
 *         |   0    |   1    | Push-pull output      |
 *         |   1    |   0    | Input only (Hi-Z)     |
 *         |   1    |   1    | Open-drain output     |
 */
typedef enum {
    HAL_GPIO_MODE_QUASI      = 0x00,    /**< Quasi-bidirectional mode */
    HAL_GPIO_MODE_PUSHPULL   = 0x01,    /**< Push-pull output mode */
    HAL_GPIO_MODE_INPUT      = 0x02,    /**< Input only (high impedance) */
    HAL_GPIO_MODE_OPENDRAIN  = 0x03     /**< Open-drain output mode */
} HAL_GPIO_ModeTypeDef;

/*******************************************************************************
 * GPIO Pin State Definitions
 ******************************************************************************/

/**
 * @brief  GPIO Pin State enumeration
 */
typedef enum {
    HAL_GPIO_PIN_RESET = 0, /**< Pin is low (0) */
    HAL_GPIO_PIN_SET = 1    /**< Pin is high (1) */
} HAL_GPIO_PinStateTypeDef;

/*******************************************************************************
 * GPIO Schmitt Trigger Definitions
 ******************************************************************************/

/**
 * @brief  GPIO Schmitt Trigger configuration
 */
typedef enum {
    HAL_GPIO_SCHMITT_DISABLE = 0,   /**< Schmitt trigger disabled (TTL input) */
    HAL_GPIO_SCHMITT_ENABLE = 1     /**< Schmitt trigger enabled */
} HAL_GPIO_SchmittTypeDef;

/*******************************************************************************
 * GPIO Interrupt Configuration
 ******************************************************************************/

/**
 * @brief  GPIO Interrupt Trigger Mode
 */
typedef enum {
    HAL_GPIO_INT_LOWLEVEL    = 0x00,    /**< Low level trigger */
    HAL_GPIO_INT_HIGHLEVEL   = 0x01,    /**< High level trigger */
    HAL_GPIO_INT_FALLING     = 0x02,    /**< Falling edge trigger */
    HAL_GPIO_INT_RISING      = 0x03,    /**< Rising edge trigger */
    HAL_GPIO_INT_BOTH        = 0x04     /**< Both edges trigger */
} HAL_GPIO_IntModeTypeDef;

/**
 * @brief  GPIO Interrupt Channel (0-7 for different pin groups)
 */
typedef enum {
    HAL_GPIO_INT_CHANNEL0 = 0,  /**< Interrupt channel 0 */
    HAL_GPIO_INT_CHANNEL1 = 1,  /**< Interrupt channel 1 */
    HAL_GPIO_INT_CHANNEL2 = 2,  /**< Interrupt channel 2 */
    HAL_GPIO_INT_CHANNEL3 = 3,  /**< Interrupt channel 3 */
    HAL_GPIO_INT_CHANNEL4 = 4,  /**< Interrupt channel 4 */
    HAL_GPIO_INT_CHANNEL5 = 5,  /**< Interrupt channel 5 */
    HAL_GPIO_INT_CHANNEL6 = 6,  /**< Interrupt channel 6 */
    HAL_GPIO_INT_CHANNEL7 = 7   /**< Interrupt channel 7 */
} HAL_GPIO_IntChannelTypeDef;

/*******************************************************************************
 * GPIO Initialization Structure
 ******************************************************************************/

/**
 * @brief  GPIO Initialization Structure
 */
typedef struct {
    HAL_GPIO_PinTypeDef     Pin;        /**< Pin(s) to configure */
    HAL_GPIO_ModeTypeDef    Mode;       /**< Pin mode */
    HAL_GPIO_SchmittTypeDef Schmitt;    /**< Schmitt trigger enable */
} HAL_GPIO_InitTypeDef;

/*******************************************************************************
 * GPIO Function Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize GPIO pin(s)
 * @param  port GPIO port (HAL_GPIO_PORT0 to HAL_GPIO_PORT3)
 * @param  init Pointer to HAL_GPIO_InitTypeDef structure
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_GPIO_InitTypeDef gpio_init;
 *   gpio_init.Pin = HAL_GPIO_PIN_2;
 *   gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
 *   gpio_init.Schmitt = HAL_GPIO_SCHMITT_DISABLE;
 *   HAL_GPIO_Init(HAL_GPIO_PORT1, &gpio_init);
 */
HAL_StatusTypeDef HAL_GPIO_Init(HAL_GPIO_PortTypeDef port, HAL_GPIO_InitTypeDef *init);

/**
 * @brief  De-initialize GPIO pin(s) to default state
 * @param  port GPIO port
 * @param  pin Pin(s) to de-initialize
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_GPIO_DeInit(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin);

/**
 * @brief  Set GPIO pin mode
 * @param  port GPIO port
 * @param  pin Pin to configure
 * @param  mode Pin mode
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @example
 *   HAL_GPIO_SetMode(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, HAL_GPIO_MODE_PUSHPULL);
 */
HAL_StatusTypeDef HAL_GPIO_SetMode(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                                    HAL_GPIO_ModeTypeDef mode);

/**
 * @brief  Read GPIO pin state
 * @param  port GPIO port
 * @param  pin Pin to read
 * @retval HAL_GPIO_PinStateTypeDef Pin state (HAL_GPIO_PIN_SET or HAL_GPIO_PIN_RESET)
 * 
 * @example
 *   if (HAL_GPIO_ReadPin(HAL_GPIO_PORT1, HAL_GPIO_PIN_3) == HAL_GPIO_PIN_SET) {
 *       // Pin is high
 *   }
 */
HAL_GPIO_PinStateTypeDef HAL_GPIO_ReadPin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin);

/**
 * @brief  Write GPIO pin state
 * @param  port GPIO port
 * @param  pin Pin to write
 * @param  state Pin state to set
 * @retval None
 * 
 * @example
 *   HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, HAL_GPIO_PIN_SET);
 */
void HAL_GPIO_WritePin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                       HAL_GPIO_PinStateTypeDef state);

/**
 * @brief  Toggle GPIO pin state
 * @param  port GPIO port
 * @param  pin Pin to toggle
 * @retval None
 * 
 * @example
 *   HAL_GPIO_TogglePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2);  // Toggle LED
 */
void HAL_GPIO_TogglePin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin);

/**
 * @brief  Read entire GPIO port value
 * @param  port GPIO port
 * @retval uint8_t Port value (8-bit)
 * 
 * @example
 *   uint8_t port_val = HAL_GPIO_ReadPort(HAL_GPIO_PORT0);
 */
uint8_t HAL_GPIO_ReadPort(HAL_GPIO_PortTypeDef port);

/**
 * @brief  Write entire GPIO port value
 * @param  port GPIO port
 * @param  value Value to write (8-bit)
 * @retval None
 * 
 * @example
 *   HAL_GPIO_WritePort(HAL_GPIO_PORT0, 0x55);  // Write pattern to port
 */
void HAL_GPIO_WritePort(HAL_GPIO_PortTypeDef port, uint8_t value);

/**
 * @brief  Enable Schmitt trigger for GPIO pin
 * @param  port GPIO port
 * @param  pin Pin to configure
 * @param  enable HAL_ENABLE or HAL_DISABLE
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef HAL_GPIO_SetSchmitt(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                                       uint8_t enable);

/**
 * @brief  Configure GPIO pin interrupt
 * @param  port GPIO port (for port selection)
 * @param  channel Interrupt channel (0-7)
 * @param  mode Interrupt trigger mode
 * @param  enable HAL_ENABLE or HAL_DISABLE
 * @retval HAL_StatusTypeDef HAL status
 * 
 * @note   MS51 has 8 pin interrupt channels that can be mapped to different pins
 * 
 * @example
 *   // Configure falling edge interrupt on channel 0
 *   HAL_GPIO_ConfigInterrupt(HAL_GPIO_PORT0, HAL_GPIO_INT_CHANNEL0, 
 *                            HAL_GPIO_INT_FALLING, HAL_ENABLE);
 */
HAL_StatusTypeDef HAL_GPIO_ConfigInterrupt(HAL_GPIO_PortTypeDef port, 
                                            HAL_GPIO_IntChannelTypeDef channel,
                                            HAL_GPIO_IntModeTypeDef mode, 
                                            uint8_t enable);

/**
 * @brief  Enable GPIO pin interrupt
 * @retval None
 */
void HAL_GPIO_EnableInterrupt(void);

/**
 * @brief  Disable GPIO pin interrupt
 * @retval None
 */
void HAL_GPIO_DisableInterrupt(void);

/**
 * @brief  Get GPIO interrupt flag
 * @param  channel Interrupt channel
 * @retval uint8_t 1 if flag is set, 0 otherwise
 */
uint8_t HAL_GPIO_GetInterruptFlag(HAL_GPIO_IntChannelTypeDef channel);

/**
 * @brief  Clear GPIO interrupt flag
 * @param  channel Interrupt channel
 * @retval None
 */
void HAL_GPIO_ClearInterruptFlag(HAL_GPIO_IntChannelTypeDef channel);

/**
 * @brief  Set all GPIO pins to quasi-bidirectional mode (default reset state)
 * @retval None
 */
void HAL_GPIO_SetAllQuasi(void);

/**
 * @brief  Set all GPIO pins to push-pull mode
 * @retval None
 */
void HAL_GPIO_SetAllPushPull(void);

/**
 * @brief  Set all GPIO pins to input mode
 * @retval None
 */
void HAL_GPIO_SetAllInput(void);

/*******************************************************************************
 * GPIO Convenience Macros
 ******************************************************************************/

/**
 * @brief  Quick pin configuration macros for common operations
 */
#define HAL_GPIO_SET_PIN(port, pin)    HAL_GPIO_WritePin(port, pin, HAL_GPIO_PIN_SET)
#define HAL_GPIO_CLR_PIN(port, pin)    HAL_GPIO_WritePin(port, pin, HAL_GPIO_PIN_RESET)
#define HAL_GPIO_TGL_PIN(port, pin)    HAL_GPIO_TogglePin(port, pin)
#define HAL_GPIO_GET_PIN(port, pin)    HAL_GPIO_ReadPin(port, pin)

/**
 * @brief  LED control macros (assuming LED on P1.2)
 */
#define HAL_LED_ON()    HAL_GPIO_WritePin(HAL_LED_PORT, HAL_LED_PIN, HAL_GPIO_PIN_RESET)
#define HAL_LED_OFF()   HAL_GPIO_WritePin(HAL_LED_PORT, HAL_LED_PIN, HAL_GPIO_PIN_SET)
#define HAL_LED_TOGGLE() HAL_GPIO_TogglePin(HAL_LED_PORT, HAL_LED_PIN)

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_GPIO_H */
