/**
 * @file ms51_32k.h
 * @brief Device configuration for MS51 32KB family
 * 
 * This file defines device-specific constants for the 32KB MS51 family:
 * - MS51FC0AE
 * - MS51XC0BE
 * - MS51EB0AE
 * - MS51EC0AE
 * - MS51TC0AE
 * - MS51PC0AE
 *
 * @copyright Copyright (c) 2024-2025 MS51_BSP Contributors
 * @license SPDX-License-Identifier: Apache-2.0
 */

#ifndef MS51_32K_H
#define MS51_32K_H

/*----------------------------------------------------------------------------
 * Device Identification
 *----------------------------------------------------------------------------*/
#define MS51_DEVICE_FAMILY      "MS51_32K"
#define MS51_FLASH_SIZE_KB      32
#define MS51_RAM_SIZE_BYTES     1024

/*----------------------------------------------------------------------------
 * Memory Map
 *----------------------------------------------------------------------------*/
/* Flash Memory */
#define MS51_APROM_START        0x0000U
#define MS51_APROM_SIZE         0x7600U     /* 30 KB */
#define MS51_APROM_END          0x75FFU

#define MS51_LDROM_START        0x7600U
#define MS51_LDROM_SIZE         0x0A00U     /* 2.5 KB */
#define MS51_LDROM_END          0x7FFFU

#define MS51_CONFIG_START       0x7800U
#define MS51_CONFIG_SIZE        0x0008U     /* 8 bytes */
#define MS51_CONFIG_END         0x7807U

/* Data Flash (within APROM) */
#define MS51_DATAFLASH_SIZE     0x0800U     /* 2 KB max */

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
#define MS51_ADC_CHANNELS       12

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

#endif /* MS51_32K_H */
