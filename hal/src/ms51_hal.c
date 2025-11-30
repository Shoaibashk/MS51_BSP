/*===========================================================================
 * @file    ms51_hal.c
 * @brief   MS51 HAL Implementation
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#include "ms51_hal.h"

/*---------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------*/
BIT BIT_TMP;
DATA_MEM uint8_t TA_REG_TMP;
DATA_MEM uint8_t BYTE_TMP;
DATA_MEM uint8_t SFRS_TMP;

/* System clock tracking */
static uint32_t g_system_clock = HIRC_24MHZ;

/*---------------------------------------------------------------------------
 * HIRC Trim Values
 *---------------------------------------------------------------------------*/
#define HIRC_24MHZ_TRIM     0x00
#define HIRC_16MHZ_TRIM     0x10
#define HIRC_166MHZ_TRIM    0x20

/*===========================================================================
 * System Functions Implementation
 *===========================================================================*/

/**
 * @brief  Set HIRC frequency
 * @param  freq  HIRC_24MHZ, HIRC_16MHZ, or HIRC_166MHZ
 */
void HAL_SYS_SetHIRC(uint32_t freq)
{
    uint8_t trim_value;
    
    switch (freq) {
        case HIRC_16MHZ:
            trim_value = HIRC_16MHZ_TRIM;
            g_system_clock = HIRC_16MHZ;
            break;
            
        case HIRC_166MHZ:
            trim_value = HIRC_166MHZ_TRIM;
            g_system_clock = HIRC_166MHZ;
            break;
            
        case HIRC_24MHZ:
        default:
            trim_value = HIRC_24MHZ_TRIM;
            g_system_clock = HIRC_24MHZ;
            break;
    }
    
    /* Apply HIRC trim setting */
    /* TA-protected write to RCTRIM0 */
    HAL_ENTER_CRITICAL();
    TA = 0xAA;
    TA = 0x55;
    RCTRIM0 = trim_value;
    HAL_EXIT_CRITICAL();
    
    /* Wait for HIRC stable */
    CALL_NOP;
    CALL_NOP;
    CALL_NOP;
    CALL_NOP;
}

/**
 * @brief  Software reset to APROM
 */
void HAL_SYS_ResetToAPROM(void)
{
    /* Clear boot selection bit (boot from APROM) */
    HAL_ENTER_CRITICAL();
    TA = 0xAA;
    TA = 0x55;
    CHPCON &= ~0x02;    /* BS = 0 */
    
    /* Trigger software reset */
    TA = 0xAA;
    TA = 0x55;
    CHPCON |= 0x80;     /* SWRST = 1 */
    HAL_EXIT_CRITICAL();
    
    /* Never reaches here */
    while (1);
}

/**
 * @brief  Software reset to LDROM
 */
void HAL_SYS_ResetToLDROM(void)
{
    /* Set boot selection bit (boot from LDROM) */
    HAL_ENTER_CRITICAL();
    TA = 0xAA;
    TA = 0x55;
    CHPCON |= 0x02;     /* BS = 1 */
    
    /* Trigger software reset */
    TA = 0xAA;
    TA = 0x55;
    CHPCON |= 0x80;     /* SWRST = 1 */
    HAL_EXIT_CRITICAL();
    
    /* Never reaches here */
    while (1);
}

/**
 * @brief  Get current system clock frequency
 * @return System clock in Hz
 */
uint32_t HAL_SYS_GetClock(void)
{
    return g_system_clock;
}

/**
 * @brief  Enter idle mode
 * @note   CPU stops, peripherals continue running
 */
void HAL_SYS_EnterIdle(void)
{
    set_PCON_IDLE;
}

/**
 * @brief  Enter power-down mode
 * @note   Most peripherals stop, minimal power consumption
 */
void HAL_SYS_EnterPowerDown(void)
{
    set_PCON_PD;
}

/*===========================================================================
 * Read APROM Byte
 *===========================================================================*/

#if defined(COMPILER_KEIL)
/**
 * @brief  Read a byte from APROM using code pointer
 * @param  u16_addr  Address in APROM
 * @return Byte value at the address
 */
uint8_t Read_APROM_BYTE(uint16_t code *u16_addr)
{
    return *u16_addr;
}

#elif defined(COMPILER_IAR)
uint8_t Read_APROM_BYTE(const uint16_t __code *u16_addr)
{
    return (uint8_t)(*u16_addr);
}

#elif defined(COMPILER_SDCC)
uint8_t Read_APROM_BYTE(const uint16_t __code *u16_addr)
{
    return (uint8_t)(*u16_addr);
}

#else
/* Host/Unit test stub */
uint8_t Read_APROM_BYTE(const uint16_t *u16_addr)
{
    (void)u16_addr;
    return 0xFF;
}
#endif
