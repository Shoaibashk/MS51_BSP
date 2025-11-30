/**
 * @file ms51_8k.h
 * @brief Device configuration for MS51 8KB family
 * 
 * This file defines device-specific constants for the 8KB MS51 family:
 * - MS51DA9AE
 * - MS51BA9AE
 *
 * @copyright Copyright (c) 2024-2025 MS51_BSP Contributors
 * @license SPDX-License-Identifier: Apache-2.0
 */

#ifndef MS51_8K_H
#define MS51_8K_H

/*----------------------------------------------------------------------------
 * Device Identification
 *----------------------------------------------------------------------------*/
#define MS51_DEVICE_FAMILY      "MS51_8K"
#define MS51_FLASH_SIZE_KB      8
#define MS51_RAM_SIZE_BYTES     256

/*----------------------------------------------------------------------------
 * Memory Map
 *----------------------------------------------------------------------------*/
/* Flash Memory */
#define MS51_APROM_START        0x0000U
#define MS51_APROM_SIZE         0x1600U     /* 5.5 KB */
#define MS51_APROM_END          0x15FFU

#define MS51_LDROM_START        0x1600U
#define MS51_LDROM_SIZE         0x0A00U     /* 2.5 KB */
#define MS51_LDROM_END          0x1FFFU

#define MS51_CONFIG_START       0x1800U
#define MS51_CONFIG_SIZE        0x0008U     /* 8 bytes */
#define MS51_CONFIG_END         0x1807U

/* Data Flash (within APROM) */
#define MS51_DATAFLASH_SIZE     0x0200U     /* 512 bytes max */

/* RAM Memory */
#define MS51_IRAM_START         0x00U
#define MS51_IRAM_SIZE          0x100U      /* 256 bytes */
#define MS51_IRAM_END           0xFFU

/* No XRAM on 8KB devices */
#define MS51_HAS_XRAM           0

/*----------------------------------------------------------------------------
 * Peripheral Availability
 *----------------------------------------------------------------------------*/
#define MS51_HAS_ADC            1
#define MS51_ADC_CHANNELS       8

#define MS51_HAS_PWM            1
#define MS51_PWM_CHANNELS       6

#define MS51_HAS_UART           1
#define MS51_UART_COUNT         1

#define MS51_HAS_SPI            1
#define MS51_SPI_COUNT          1

#define MS51_HAS_I2C            1
#define MS51_I2C_COUNT          1

#define MS51_HAS_TIMER          1
#define MS51_TIMER_COUNT        3   /* Timer0, Timer1, Timer2 */

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
#define MS51_GPIO_PORTS         4   /* P0, P1, P2, P3 */
#define MS51_GPIO_PINS_P0       8
#define MS51_GPIO_PINS_P1       8
#define MS51_GPIO_PINS_P2       1   /* Limited pins */
#define MS51_GPIO_PINS_P3       1   /* Limited pins */

#endif /* MS51_8K_H */
