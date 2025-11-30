/**
 *******************************************************************************
 * @file    ms51_hal_gpio.c
 * @brief   MS51 HAL GPIO Driver Implementation
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

#ifdef HAL_GPIO_MODULE_ENABLED

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief  Get pin position from pin mask
 */
static uint8_t GPIO_GetPinPosition(HAL_GPIO_PinTypeDef pin)
{
    uint8_t pos = 0;
    uint8_t mask = (uint8_t)pin;
    
    while ((mask & 0x01) == 0 && pos < 8) {
        mask >>= 1;
        pos++;
    }
    return pos;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief  Initialize GPIO pin(s)
 */
HAL_StatusTypeDef HAL_GPIO_Init(HAL_GPIO_PortTypeDef port, HAL_GPIO_InitTypeDef *init)
{
    uint8_t pinMask;
    uint8_t i;
    
    /* Parameter validation */
    if (init == NULL) {
        return HAL_INVALID;
    }
    
    pinMask = (uint8_t)init->Pin;
    
    /* Configure each pin in the mask */
    for (i = 0; i < 8; i++) {
        if (pinMask & (1 << i)) {
            /* Set pin mode */
            HAL_GPIO_SetMode(port, (HAL_GPIO_PinTypeDef)(1 << i), init->Mode);
            
            /* Set Schmitt trigger */
            HAL_GPIO_SetSchmitt(port, (HAL_GPIO_PinTypeDef)(1 << i), init->Schmitt);
        }
    }
    
    return HAL_OK;
}

/**
 * @brief  De-initialize GPIO pin(s) to default state (Quasi-bidirectional)
 */
HAL_StatusTypeDef HAL_GPIO_DeInit(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin)
{
    return HAL_GPIO_SetMode(port, pin, HAL_GPIO_MODE_QUASI);
}

/**
 * @brief  Set GPIO pin mode
 */
HAL_StatusTypeDef HAL_GPIO_SetMode(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                                    HAL_GPIO_ModeTypeDef mode)
{
    uint8_t pinMask = (uint8_t)pin;
    uint8_t m1_set, m1_clr, m2_set, m2_clr;
    
    /* Determine M1 and M2 register values based on mode */
    switch (mode) {
        case HAL_GPIO_MODE_QUASI:
            /* M1=0, M2=0 */
            m1_clr = pinMask;
            m2_clr = pinMask;
            m1_set = 0;
            m2_set = 0;
            break;
            
        case HAL_GPIO_MODE_PUSHPULL:
            /* M1=0, M2=1 */
            m1_clr = pinMask;
            m2_set = pinMask;
            m1_set = 0;
            m2_clr = 0;
            break;
            
        case HAL_GPIO_MODE_INPUT:
            /* M1=1, M2=0 */
            m1_set = pinMask;
            m2_clr = pinMask;
            m1_clr = 0;
            m2_set = 0;
            break;
            
        case HAL_GPIO_MODE_OPENDRAIN:
            /* M1=1, M2=1 */
            m1_set = pinMask;
            m2_set = pinMask;
            m1_clr = 0;
            m2_clr = 0;
            break;
            
        default:
            return HAL_INVALID;
    }
    
    /* Ensure we're on SFR page 0 for port mode registers */
    HAL_SFR_PAGE0();
    
    /* Apply mode configuration to the appropriate port */
    switch (port) {
        case HAL_GPIO_PORT0:
            P0M1 = (P0M1 & ~m1_clr) | m1_set;
            P0M2 = (P0M2 & ~m2_clr) | m2_set;
            break;
            
        case HAL_GPIO_PORT1:
            P1M1 = (P1M1 & ~m1_clr) | m1_set;
            P1M2 = (P1M2 & ~m2_clr) | m2_set;
            break;
            
        case HAL_GPIO_PORT2:
            /* P2 only has P2.0 */
            if (pin != HAL_GPIO_PIN_0) {
                return HAL_INVALID;
            }
            /* P2.0 configuration is through P2S register */
            break;
            
        case HAL_GPIO_PORT3:
            /* P3 only has P3.0 */
            if (pin != HAL_GPIO_PIN_0) {
                return HAL_INVALID;
            }
            P3M1 = (P3M1 & ~m1_clr) | m1_set;
            P3M2 = (P3M2 & ~m2_clr) | m2_set;
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Read GPIO pin state
 */
HAL_GPIO_PinStateTypeDef HAL_GPIO_ReadPin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin)
{
    uint8_t portValue;
    
    portValue = HAL_GPIO_ReadPort(port);
    
    if (portValue & (uint8_t)pin) {
        return HAL_GPIO_PIN_SET;
    }
    return HAL_GPIO_PIN_RESET;
}

/**
 * @brief  Write GPIO pin state
 */
void HAL_GPIO_WritePin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                       HAL_GPIO_PinStateTypeDef state)
{
    switch (port) {
        case HAL_GPIO_PORT0:
            if (state == HAL_GPIO_PIN_SET) {
                P0 |= (uint8_t)pin;
            } else {
                P0 &= ~(uint8_t)pin;
            }
            break;
            
        case HAL_GPIO_PORT1:
            if (state == HAL_GPIO_PIN_SET) {
                P1 |= (uint8_t)pin;
            } else {
                P1 &= ~(uint8_t)pin;
            }
            break;
            
        case HAL_GPIO_PORT2:
            if (state == HAL_GPIO_PIN_SET) {
                P2 |= (uint8_t)pin;
            } else {
                P2 &= ~(uint8_t)pin;
            }
            break;
            
        case HAL_GPIO_PORT3:
            if (state == HAL_GPIO_PIN_SET) {
                P3 |= (uint8_t)pin;
            } else {
                P3 &= ~(uint8_t)pin;
            }
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Toggle GPIO pin state
 */
void HAL_GPIO_TogglePin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin)
{
    switch (port) {
        case HAL_GPIO_PORT0:
            P0 ^= (uint8_t)pin;
            break;
            
        case HAL_GPIO_PORT1:
            P1 ^= (uint8_t)pin;
            break;
            
        case HAL_GPIO_PORT2:
            P2 ^= (uint8_t)pin;
            break;
            
        case HAL_GPIO_PORT3:
            P3 ^= (uint8_t)pin;
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Read entire GPIO port value
 */
uint8_t HAL_GPIO_ReadPort(HAL_GPIO_PortTypeDef port)
{
    switch (port) {
        case HAL_GPIO_PORT0:
            return P0;
            
        case HAL_GPIO_PORT1:
            return P1;
            
        case HAL_GPIO_PORT2:
            return P2;
            
        case HAL_GPIO_PORT3:
            return P3;
            
        default:
            return 0;
    }
}

/**
 * @brief  Write entire GPIO port value
 */
void HAL_GPIO_WritePort(HAL_GPIO_PortTypeDef port, uint8_t value)
{
    switch (port) {
        case HAL_GPIO_PORT0:
            P0 = value;
            break;
            
        case HAL_GPIO_PORT1:
            P1 = value;
            break;
            
        case HAL_GPIO_PORT2:
            P2 = value;
            break;
            
        case HAL_GPIO_PORT3:
            P3 = value;
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Enable/Disable Schmitt trigger for GPIO pin
 */
HAL_StatusTypeDef HAL_GPIO_SetSchmitt(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                                       uint8_t enable)
{
    uint8_t pinMask = (uint8_t)pin;
    
    /* Switch to SFR page 1 for Schmitt trigger registers */
    HAL_SFR_PAGE1();
    
    switch (port) {
        case HAL_GPIO_PORT0:
            if (enable) {
                /* P0S register on page 1, address 0xB1 */
                P0M1 |= pinMask;  /* P0S shares address with P0M1 on page 1 */
            } else {
                P0M1 &= ~pinMask;
            }
            break;
            
        case HAL_GPIO_PORT1:
            if (enable) {
                P1M1 |= pinMask;  /* P1S shares address with P1M1 on page 1 */
            } else {
                P1M1 &= ~pinMask;
            }
            break;
            
        case HAL_GPIO_PORT3:
            if (enable) {
                P3M1 |= pinMask;  /* P3S shares address with P3M1 on page 1 */
            } else {
                P3M1 &= ~pinMask;
            }
            break;
            
        default:
            HAL_SFR_PAGE0();
            return HAL_INVALID;
    }
    
    /* Switch back to SFR page 0 */
    HAL_SFR_PAGE0();
    
    return HAL_OK;
}

/**
 * @brief  Configure GPIO pin interrupt
 */
HAL_StatusTypeDef HAL_GPIO_ConfigInterrupt(HAL_GPIO_PortTypeDef port, 
                                            HAL_GPIO_IntChannelTypeDef channel,
                                            HAL_GPIO_IntModeTypeDef mode, 
                                            uint8_t enable)
{
    uint8_t channelBit = (1 << channel);
    
    if (!enable) {
        /* Disable interrupt for this channel */
        PINEN &= ~channelBit;
        PIPEN &= ~channelBit;
        return HAL_OK;
    }
    
    /* Configure port selection */
    PICON &= 0xFC;  /* Clear port selection bits */
    PICON |= (uint8_t)port & 0x03;
    
    /* Configure edge/level detection based on channel */
    switch (mode) {
        case HAL_GPIO_INT_LOWLEVEL:
            /* Edge mode = 0, negative enable = 1, positive enable = 0 */
            PICON &= ~(channelBit << 2);  /* Clear edge bit if applicable */
            PINEN |= channelBit;
            PIPEN &= ~channelBit;
            break;
            
        case HAL_GPIO_INT_HIGHLEVEL:
            /* Edge mode = 0, negative enable = 0, positive enable = 1 */
            PICON &= ~(channelBit << 2);
            PINEN &= ~channelBit;
            PIPEN |= channelBit;
            break;
            
        case HAL_GPIO_INT_FALLING:
            /* Edge mode = 1, negative enable = 1, positive enable = 0 */
            if (channel >= 6) {
                PICON |= 0x80;  /* PIT67 = 1 */
            } else if (channel >= 4) {
                PICON |= 0x40;  /* PIT45 = 1 */
            } else if (channel == 3) {
                PICON |= 0x20;  /* PIT3 = 1 */
            } else if (channel == 2) {
                PICON |= 0x10;  /* PIT2 = 1 */
            } else if (channel == 1) {
                PICON |= 0x08;  /* PIT1 = 1 */
            } else {
                PICON |= 0x04;  /* PIT0 = 1 */
            }
            PINEN |= channelBit;
            PIPEN &= ~channelBit;
            break;
            
        case HAL_GPIO_INT_RISING:
            /* Edge mode = 1, negative enable = 0, positive enable = 1 */
            if (channel >= 6) {
                PICON |= 0x80;
            } else if (channel >= 4) {
                PICON |= 0x40;
            } else if (channel == 3) {
                PICON |= 0x20;
            } else if (channel == 2) {
                PICON |= 0x10;
            } else if (channel == 1) {
                PICON |= 0x08;
            } else {
                PICON |= 0x04;
            }
            PINEN &= ~channelBit;
            PIPEN |= channelBit;
            break;
            
        case HAL_GPIO_INT_BOTH:
            /* Edge mode = 1, both enables = 1 */
            if (channel >= 6) {
                PICON |= 0x80;
            } else if (channel >= 4) {
                PICON |= 0x40;
            } else if (channel == 3) {
                PICON |= 0x20;
            } else if (channel == 2) {
                PICON |= 0x10;
            } else if (channel == 1) {
                PICON |= 0x08;
            } else {
                PICON |= 0x04;
            }
            PINEN |= channelBit;
            PIPEN |= channelBit;
            break;
            
        default:
            return HAL_INVALID;
    }
    
    return HAL_OK;
}

/**
 * @brief  Enable GPIO pin interrupt (global)
 */
void HAL_GPIO_EnableInterrupt(void)
{
    EIE |= 0x02;  /* Set EPI bit */
}

/**
 * @brief  Disable GPIO pin interrupt (global)
 */
void HAL_GPIO_DisableInterrupt(void)
{
    EIE &= ~0x02;  /* Clear EPI bit */
}

/**
 * @brief  Get GPIO interrupt flag
 */
uint8_t HAL_GPIO_GetInterruptFlag(HAL_GPIO_IntChannelTypeDef channel)
{
    return (PIF & (1 << channel)) ? 1 : 0;
}

/**
 * @brief  Clear GPIO interrupt flag
 */
void HAL_GPIO_ClearInterruptFlag(HAL_GPIO_IntChannelTypeDef channel)
{
    PIF &= ~(1 << channel);
}

/**
 * @brief  Set all GPIO pins to quasi-bidirectional mode
 */
void HAL_GPIO_SetAllQuasi(void)
{
    HAL_SFR_PAGE0();
    
    P0M1 = 0x00;
    P0M2 = 0x00;
    P1M1 = 0x00;
    P1M2 = 0x00;
    P3M1 = 0x00;
    P3M2 = 0x00;
}

/**
 * @brief  Set all GPIO pins to push-pull mode
 */
void HAL_GPIO_SetAllPushPull(void)
{
    HAL_SFR_PAGE0();
    
    P0M1 = 0x00;
    P0M2 = 0xFF;
    P1M1 = 0x00;
    P1M2 = 0xFF;
    P3M1 = 0x00;
    P3M2 = 0xFF;
}

/**
 * @brief  Set all GPIO pins to input mode
 */
void HAL_GPIO_SetAllInput(void)
{
    HAL_SFR_PAGE0();
    
    P0M1 = 0xFF;
    P0M2 = 0x00;
    P1M1 = 0xFF;
    P1M2 = 0x00;
    P3M1 = 0xFF;
    P3M2 = 0x00;
}

#endif /* HAL_GPIO_MODULE_ENABLED */
