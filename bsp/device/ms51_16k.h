/**
 * @file ms51_16k.h
 * @brief Device configuration for MS51 16KB family
 * 
 * This file defines device-specific constants for the 16KB MS51 family:
 * - MS51FB9AE
 * - MS51XB9AE
 * - MS51XB9BE
 *
 * @copyright Copyright (c) 2024-2025 MS51_BSP Contributors
 * @license SPDX-License-Identifier: Apache-2.0
 */

#ifndef MS51_16K_H
#define MS51_16K_H

/*----------------------------------------------------------------------------
 * Device Identification
 *----------------------------------------------------------------------------*/
#define MS51_DEVICE_FAMILY      "MS51_16K"
#define MS51_FLASH_SIZE_KB      16
#define MS51_RAM_SIZE_BYTES     1024

/*----------------------------------------------------------------------------
 * Memory Map
 *----------------------------------------------------------------------------*/
/* Flash Memory */
#define MS51_APROM_START        0x0000U
#define MS51_APROM_SIZE         0x3600U     /* 13.5 KB */
#define MS51_APROM_END          0x35FFU

#define MS51_LDROM_START        0x3600U
#define MS51_LDROM_SIZE         0x0A00U     /* 2.5 KB */
#define MS51_LDROM_END          0x3FFFU

#define MS51_CONFIG_START       0x3800U
#define MS51_CONFIG_SIZE        0x0008U     /* 8 bytes */
#define MS51_CONFIG_END         0x3807U

/* Data Flash (within APROM) */
#define MS51_DATAFLASH_SIZE     0x0400U     /* 1 KB max */

/* RAM Memory */
#define MS51_IRAM_START         0x00U
#define MS51_IRAM_SIZE          0x100U      /* 256 bytes */
#define MS51_IRAM_END           0xFFU

#define MS51_HAS_XRAM           1
#define MS51_XRAM_START         0x0000U
#define MS51_XRAM_SIZE          0x0300U     /* 768 bytes */
#define MS51_XRAM_END           0x02FFU

/*----------------------------------------------------------------------------
 * Peripheral Availability
 *----------------------------------------------------------------------------*/
#define MS51_HAS_ADC            1
#define MS51_ADC_CHANNELS       8

#define MS51_HAS_PWM            1
#define MS51_PWM_CHANNELS       6

#define MS51_HAS_UART           1
#define MS51_UART_COUNT         2

#define MS51_HAS_SPI            1
#define MS51_SPI_COUNT          1

#define MS51_HAS_I2C            1
#define MS51_I2C_COUNT          1

#define MS51_HAS_TIMER          1
#define MS51_TIMER_COUNT        4   /* Timer0, Timer1, Timer2, Timer3 */

#define MS51_HAS_WDT            1

/*----------------------------------------------------------------------------
 * Clock Configuration
 *----------------------------------------------------------------------------*/
#define MS51_HIRC_FREQ_HZ       24000000UL  /* 24 MHz */
#define MS51_HIRC_DIV2_FREQ_HZ  12000000UL  /* 12 MHz */
#define MS51_LIRC_FREQ_HZ       10000UL     /* 10 kHz */

/*----------------------------------------------------------------------------
 * GPIO Configuration
 *----------------------------------------------------------------------------*/
#define MS51_GPIO_PORTS         6   /* P0, P1, P2, P3, P4, P5 */
#define MS51_GPIO_PINS_P0       8
#define MS51_GPIO_PINS_P1       8
#define MS51_GPIO_PINS_P2       8
#define MS51_GPIO_PINS_P3       8
#define MS51_GPIO_PINS_P4       4   /* Limited pins */
#define MS51_GPIO_PINS_P5       6   /* Limited pins */

#endif /* MS51_16K_H */
