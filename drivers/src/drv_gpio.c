/**
 * @file drv_gpio.c
 * @brief GPIO Driver Implementation for MS51 Series
 * @details Provides handle-based and direct GPIO control functions
 * @version 2.0.0
 * @date 2024
 * 
 * @copyright Copyright (c) 2024
 * SPDX-License-Identifier: Apache-2.0
 */

#include "drv_gpio.h"
#include "hal_sfr.h"

/*===========================================================================*/
/* Private Variables                                                          */
/*===========================================================================*/

/** Callback storage for pin interrupts (8 pins max per port) */
static drv_callback_t gpio_callbacks[8] = {0};

/*===========================================================================*/
/* Private Functions                                                          */
/*===========================================================================*/

/**
 * @brief Set GPIO pin mode (internal helper)
 * @param port GPIO port
 * @param pin Pin number (0-7)
 * @param mode Pin mode
 */
static void gpio_set_mode(gpio_port_t port, uint8_t pin, gpio_mode_t mode)
{
    uint8_t sfr_page;
    volatile uint8_t *px_mode;
    uint8_t mask;
    
    /* Save current SFR page */
    sfr_page = HAL_SFR_PageGet();
    HAL_SFR_PageSet(0);
    
    /* Get port mode register pointer */
    switch (port) {
        case GPIO_PORT_0:
            px_mode = &P0M1;
            break;
        case GPIO_PORT_1:
            px_mode = &P1M1;
            break;
        case GPIO_PORT_2:
            px_mode = &P2M1;
            break;
        case GPIO_PORT_3:
            px_mode = &P3M1;
            break;
        case GPIO_PORT_4:
            px_mode = &P4M1;
            break;
        case GPIO_PORT_5:
            px_mode = &P5M1;
            break;
        default:
            HAL_SFR_PageSet(sfr_page);
            return;
    }
    
    mask = (1U << pin);
    
    /* Configure mode:
     * PxM1.n  PxM2.n  Mode
     *   0       0     Quasi-bidirectional
     *   0       1     Push-pull output
     *   1       0     Input only (high-impedance)
     *   1       1     Open-drain output
     */
    switch (mode) {
        case GPIO_MODE_QUASI_BIDIR:
            *(px_mode) &= ~mask;        /* PxM1.n = 0 */
            *(px_mode + 1) &= ~mask;    /* PxM2.n = 0 */
            break;
            
        case GPIO_MODE_PUSH_PULL:
            *(px_mode) &= ~mask;        /* PxM1.n = 0 */
            *(px_mode + 1) |= mask;     /* PxM2.n = 1 */
            break;
            
        case GPIO_MODE_INPUT:
        case GPIO_MODE_INPUT_PULLUP:
            *(px_mode) |= mask;         /* PxM1.n = 1 */
            *(px_mode + 1) &= ~mask;    /* PxM2.n = 0 */
            break;
            
        case GPIO_MODE_OPEN_DRAIN:
            *(px_mode) |= mask;         /* PxM1.n = 1 */
            *(px_mode + 1) |= mask;     /* PxM2.n = 1 */
            break;
            
        default:
            break;
    }
    
    /* Restore SFR page */
    HAL_SFR_PageSet(sfr_page);
}

/**
 * @brief Get port data register pointer
 * @param port GPIO port
 * @return Pointer to port data register
 */
static volatile uint8_t* gpio_get_port_reg(gpio_port_t port)
{
    switch (port) {
        case GPIO_PORT_0: return &P0;
        case GPIO_PORT_1: return &P1;
        case GPIO_PORT_2: return &P2;
        case GPIO_PORT_3: return &P3;
        case GPIO_PORT_4: return &P4;
        case GPIO_PORT_5: return &P5;
        default: return NULL;
    }
}

/*===========================================================================*/
/* Handle-based API Implementation                                            */
/*===========================================================================*/

/**
 * @brief Initialize GPIO with handle
 */
drv_status_t DRV_GPIO_Init(gpio_handle_t *handle, const gpio_config_t *config)
{
    volatile uint8_t *port_reg;
    
    if (handle == NULL || config == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    /* Validate port and pin */
    if (config->port > GPIO_PORT_5 || config->pin > 7) {
        return DRV_ERROR_PARAMETER;
    }
    
    /* Initialize handle */
    handle->port = config->port;
    handle->pin = config->pin;
    handle->mode = config->mode;
    handle->callback = config->callback;
    handle->int_enabled = 0;
    
    /* Configure pin mode */
    gpio_set_mode(config->port, config->pin, config->mode);
    
    /* Set initial value for output modes */
    if (config->mode == GPIO_MODE_PUSH_PULL || 
        config->mode == GPIO_MODE_OPEN_DRAIN ||
        config->mode == GPIO_MODE_QUASI_BIDIR) {
        port_reg = gpio_get_port_reg(config->port);
        if (port_reg != NULL) {
            if (config->init_value) {
                *port_reg |= (1U << config->pin);
            } else {
                *port_reg &= ~(1U << config->pin);
            }
        }
    }
    
    /* For input with pull-up, set pin high */
    if (config->mode == GPIO_MODE_INPUT_PULLUP) {
        port_reg = gpio_get_port_reg(config->port);
        if (port_reg != NULL) {
            *port_reg |= (1U << config->pin);
        }
    }
    
    /* Store callback if provided */
    if (config->callback != NULL && config->pin < 8) {
        gpio_callbacks[config->pin] = config->callback;
    }
    
    return DRV_OK;
}

/**
 * @brief Deinitialize GPIO handle
 */
drv_status_t DRV_GPIO_DeInit(gpio_handle_t *handle)
{
    if (handle == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    /* Disable interrupt if enabled */
    if (handle->int_enabled) {
        DRV_GPIO_IntDisable(handle);
    }
    
    /* Reset pin to quasi-bidirectional (default) */
    gpio_set_mode(handle->port, handle->pin, GPIO_MODE_QUASI_BIDIR);
    
    /* Clear callback */
    if (handle->pin < 8) {
        gpio_callbacks[handle->pin] = NULL;
    }
    
    /* Clear handle */
    handle->port = GPIO_PORT_0;
    handle->pin = 0;
    handle->mode = GPIO_MODE_QUASI_BIDIR;
    handle->callback = NULL;
    handle->int_enabled = 0;
    
    return DRV_OK;
}

/**
 * @brief Write value to GPIO pin
 */
drv_status_t DRV_GPIO_Write(gpio_handle_t *handle, uint8_t value)
{
    volatile uint8_t *port_reg;
    
    if (handle == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    port_reg = gpio_get_port_reg(handle->port);
    if (port_reg == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    if (value) {
        *port_reg |= (1U << handle->pin);
    } else {
        *port_reg &= ~(1U << handle->pin);
    }
    
    return DRV_OK;
}

/**
 * @brief Read GPIO pin value
 */
uint8_t DRV_GPIO_Read(gpio_handle_t *handle)
{
    volatile uint8_t *port_reg;
    
    if (handle == NULL) {
        return 0;
    }
    
    port_reg = gpio_get_port_reg(handle->port);
    if (port_reg == NULL) {
        return 0;
    }
    
    return (*port_reg & (1U << handle->pin)) ? 1 : 0;
}

/**
 * @brief Toggle GPIO pin
 */
drv_status_t DRV_GPIO_Toggle(gpio_handle_t *handle)
{
    volatile uint8_t *port_reg;
    
    if (handle == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    port_reg = gpio_get_port_reg(handle->port);
    if (port_reg == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    *port_reg ^= (1U << handle->pin);
    
    return DRV_OK;
}

/**
 * @brief Enable GPIO interrupt
 */
drv_status_t DRV_GPIO_IntEnable(gpio_handle_t *handle)
{
    uint8_t sfr_page;
    
    if (handle == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    /* Only pins on Port 3.0 (INT0) and Port 3.1 (INT1) support external interrupts */
    if (handle->port != GPIO_PORT_3 || handle->pin > 1) {
        return DRV_ERROR_RESOURCE;
    }
    
    sfr_page = HAL_SFR_PageGet();
    HAL_SFR_PageSet(0);
    
    if (handle->pin == 0) {
        /* Configure INT0 */
        IT0 = 1;    /* Edge triggered */
        EX0 = 1;    /* Enable INT0 */
    } else {
        /* Configure INT1 */
        IT1 = 1;    /* Edge triggered */
        EX1 = 1;    /* Enable INT1 */
    }
    
    EA = 1;         /* Enable global interrupts */
    handle->int_enabled = 1;
    
    HAL_SFR_PageSet(sfr_page);
    
    return DRV_OK;
}

/**
 * @brief Disable GPIO interrupt
 */
drv_status_t DRV_GPIO_IntDisable(gpio_handle_t *handle)
{
    uint8_t sfr_page;
    
    if (handle == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    if (handle->port != GPIO_PORT_3 || handle->pin > 1) {
        return DRV_ERROR_RESOURCE;
    }
    
    sfr_page = HAL_SFR_PageGet();
    HAL_SFR_PageSet(0);
    
    if (handle->pin == 0) {
        EX0 = 0;    /* Disable INT0 */
    } else {
        EX1 = 0;    /* Disable INT1 */
    }
    
    handle->int_enabled = 0;
    
    HAL_SFR_PageSet(sfr_page);
    
    return DRV_OK;
}

/**
 * @brief Register callback for GPIO pin
 */
drv_status_t DRV_GPIO_RegisterCallback(gpio_handle_t *handle, gpio_callback_t callback, void *context)
{
    if (handle == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    handle->callback = callback;
    handle->context = context;
    
    /* Also store in global callback array for ISR access */
    if (handle->pin < 8) {
        gpio_callbacks[handle->pin] = (drv_callback_t)callback;
    }
    
    return DRV_OK;
}

/*===========================================================================*/
/* Direct API Implementation (Legacy/Convenience)                             */
/*===========================================================================*/

/**
 * @brief Initialize GPIO directly without handle
 */
drv_status_t DRV_GPIO_InitDirect(const gpio_config_t *config)
{
    gpio_handle_t temp_handle;
    return DRV_GPIO_Init(&temp_handle, config);
}

/**
 * @brief Write to GPIO pin directly
 */
drv_status_t DRV_GPIO_WriteDirect(gpio_port_t port, uint8_t pin, uint8_t value)
{
    volatile uint8_t *port_reg;
    
    if (port > GPIO_PORT_5 || pin > 7) {
        return DRV_ERROR_PARAMETER;
    }
    
    port_reg = gpio_get_port_reg(port);
    if (port_reg == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    if (value) {
        *port_reg |= (1U << pin);
    } else {
        *port_reg &= ~(1U << pin);
    }
    
    return DRV_OK;
}

/**
 * @brief Read GPIO pin directly
 */
uint8_t DRV_GPIO_ReadDirect(gpio_port_t port, uint8_t pin)
{
    volatile uint8_t *port_reg;
    
    if (port > GPIO_PORT_5 || pin > 7) {
        return 0;
    }
    
    port_reg = gpio_get_port_reg(port);
    if (port_reg == NULL) {
        return 0;
    }
    
    return (*port_reg & (1U << pin)) ? 1 : 0;
}

/**
 * @brief Toggle GPIO pin directly
 */
drv_status_t DRV_GPIO_ToggleDirect(gpio_port_t port, uint8_t pin)
{
    volatile uint8_t *port_reg;
    
    if (port > GPIO_PORT_5 || pin > 7) {
        return DRV_ERROR_PARAMETER;
    }
    
    port_reg = gpio_get_port_reg(port);
    if (port_reg == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    *port_reg ^= (1U << pin);
    
    return DRV_OK;
}

/**
 * @brief Write entire port directly
 */
drv_status_t DRV_GPIO_WritePortDirect(gpio_port_t port, uint8_t value)
{
    volatile uint8_t *port_reg;
    
    port_reg = gpio_get_port_reg(port);
    if (port_reg == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    *port_reg = value;
    
    return DRV_OK;
}

/**
 * @brief Read entire port directly
 */
uint8_t DRV_GPIO_ReadPortDirect(gpio_port_t port)
{
    volatile uint8_t *port_reg;
    
    port_reg = gpio_get_port_reg(port);
    if (port_reg == NULL) {
        return 0;
    }
    
    return *port_reg;
}

/**
 * @brief Set pin mode directly
 */
drv_status_t DRV_GPIO_SetModeDirect(gpio_port_t port, uint8_t pin, gpio_mode_t mode)
{
    if (port > GPIO_PORT_5 || pin > 7) {
        return DRV_ERROR_PARAMETER;
    }
    
    gpio_set_mode(port, pin, mode);
    
    return DRV_OK;
}

/**
 * @brief Configure pin interrupt directly
 */
drv_status_t DRV_GPIO_ConfigIntDirect(gpio_port_t port, uint8_t pin, 
                                       gpio_int_edge_t edge, drv_callback_t callback)
{
    uint8_t sfr_page;
    
    /* Only P3.0 (INT0) and P3.1 (INT1) support external interrupts */
    if (port != GPIO_PORT_3 || pin > 1) {
        return DRV_ERROR_RESOURCE;
    }
    
    if (callback == NULL) {
        return DRV_ERROR_PARAMETER;
    }
    
    /* Store callback */
    gpio_callbacks[pin] = callback;
    
    sfr_page = HAL_SFR_PageGet();
    HAL_SFR_PageSet(0);
    
    if (pin == 0) {
        /* Configure INT0 */
        switch (edge) {
            case GPIO_INT_FALLING:
            case GPIO_INT_RISING:
                IT0 = 1;    /* Edge triggered */
                break;
            case GPIO_INT_BOTH:
            case GPIO_INT_LOW:
                IT0 = 0;    /* Level triggered */
                break;
            default:
                HAL_SFR_PageSet(sfr_page);
                return DRV_ERROR_PARAMETER;
        }
        EX0 = 1;    /* Enable INT0 */
    } else {
        /* Configure INT1 */
        switch (edge) {
            case GPIO_INT_FALLING:
            case GPIO_INT_RISING:
                IT1 = 1;    /* Edge triggered */
                break;
            case GPIO_INT_BOTH:
            case GPIO_INT_LOW:
                IT1 = 0;    /* Level triggered */
                break;
            default:
                HAL_SFR_PageSet(sfr_page);
                return DRV_ERROR_PARAMETER;
        }
        EX1 = 1;    /* Enable INT1 */
    }
    
    EA = 1;     /* Enable global interrupts */
    
    HAL_SFR_PageSet(sfr_page);
    
    return DRV_OK;
}

/*===========================================================================*/
/* Interrupt Service Routines                                                  */
/*===========================================================================*/

/**
 * @brief INT0 External Interrupt Handler
 */
#if defined(__C51__) || defined(__CX51__)
void INT0_ISR(void) interrupt 0
#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = 0x03
__interrupt void INT0_ISR(void)
#elif defined(__SDCC)
void INT0_ISR(void) __interrupt(0)
#else
void INT0_ISR(void)
#endif
{
    if (gpio_callbacks[0] != NULL) {
        gpio_callbacks[0]();
    }
}

/**
 * @brief INT1 External Interrupt Handler
 */
#if defined(__C51__) || defined(__CX51__)
void INT1_ISR(void) interrupt 2
#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = 0x13
__interrupt void INT1_ISR(void)
#elif defined(__SDCC)
void INT1_ISR(void) __interrupt(2)
#else
void INT1_ISR(void)
#endif
{
    if (gpio_callbacks[1] != NULL) {
        gpio_callbacks[1]();
    }
}

/*===========================================================================*/
/* Pin Interrupt (PIT) Support                                                */
/*===========================================================================*/

/**
 * @brief Configure pin interrupt on any pin (PIT feature)
 */
drv_status_t DRV_GPIO_ConfigPinInt(gpio_port_t port, uint8_t pin, 
                                    gpio_int_edge_t edge, drv_callback_t callback)
{
    uint8_t sfr_page;
    uint8_t pinen_l, pinen_h;
    uint8_t pips_val;
    
    if (port > GPIO_PORT_5 || pin > 7) {
        return DRV_ERROR_PARAMETER;
    }
    
    /* Store callback for pin interrupt channel 0 (simplified) */
    gpio_callbacks[pin] = callback;
    
    sfr_page = HAL_SFR_PageGet();
    HAL_SFR_PageSet(0);
    
    /* Calculate PIPS value: port in upper nibble, pin in lower nibble */
    pips_val = (port << 4) | pin;
    
    /* Configure edge type and enable */
    switch (edge) {
        case GPIO_INT_FALLING:
            pinen_l = 0x01;  /* Falling edge enable */
            pinen_h = 0x00;
            break;
        case GPIO_INT_RISING:
            pinen_l = 0x00;
            pinen_h = 0x01;  /* Rising edge enable */
            break;
        case GPIO_INT_BOTH:
            pinen_l = 0x01;  /* Both edges */
            pinen_h = 0x01;
            break;
        case GPIO_INT_LOW:
            pinen_l = 0x01;  /* Low level */
            pinen_h = 0x00;
            break;
        default:
            HAL_SFR_PageSet(sfr_page);
            return DRV_ERROR_PARAMETER;
    }
    
    /* Configure PIT channel 0 (for simplicity) */
    HAL_SFR_PageSet(1);
    PIP0 = pips_val;
    PINEN = pinen_l;
    PIPEN = pinen_h;
    
    /* Enable PIT interrupt */
    HAL_SFR_PageSet(0);
    EIE0 |= 0x01;   /* Enable PIT interrupt */
    EA = 1;
    
    HAL_SFR_PageSet(sfr_page);
    
    return DRV_OK;
}

/**
 * @brief PIT Interrupt Handler
 */
#if defined(__C51__) || defined(__CX51__)
void PIT_ISR(void) interrupt 7
#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = 0x3B
__interrupt void PIT_ISR(void)
#elif defined(__SDCC)
void PIT_ISR(void) __interrupt(7)
#else
void PIT_ISR(void)
#endif
{
    uint8_t sfr_page;
    uint8_t pif;
    uint8_t i;
    
    sfr_page = HAL_SFR_PageGet();
    HAL_SFR_PageSet(0);
    
    /* Read and clear PIT flags */
    pif = PIF;
    PIF = 0;
    
    HAL_SFR_PageSet(sfr_page);
    
    /* Call callbacks for triggered channels */
    for (i = 0; i < 8; i++) {
        if ((pif & (1U << i)) && gpio_callbacks[i] != NULL) {
            gpio_callbacks[i]();
        }
    }
}
