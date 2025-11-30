/**
 *******************************************************************************
 * @file    ms51_hal_conf.h
 * @brief   MS51 HAL Configuration File Template
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This file is used to enable/disable HAL modules.
 *          Copy this file to your project and modify as needed.
 *
 *******************************************************************************
 */

#ifndef __MS51_HAL_CONF_H
#define __MS51_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Device Selection
 * Uncomment the device variant you are using
 ******************************************************************************/
/* 16K Flash Variants */
/* #define MS51FB9AE */     /* 16K Flash, 1K SRAM, 18 GPIO */
/* #define MS51XB9AE */     /* 16K Flash, 1K SRAM, 18 GPIO */
/* #define MS51XB9BE */     /* 16K Flash, 1K SRAM, 18 GPIO */
#define MS51_16K            /* Generic 16K variant */

/* 8K Flash Variants */
/* #define MS51DA9AE */     /* 8K Flash, 1K SRAM, 18 GPIO */
/* #define MS51BA9AE */     /* 8K Flash, 1K SRAM, 18 GPIO */
/* #define MS51_8K */       /* Generic 8K variant */

/* 4K Flash Variants */
/* #define MS51FC0AE */     /* 4K Flash, 256B SRAM */
/* #define MS51PC0AE */     /* 4K Flash, 256B SRAM */
/* #define MS51EC0AE */     /* 4K Flash, 256B SRAM */
/* #define MS51_4K */       /* Generic 4K variant */

/*******************************************************************************
 * System Clock Configuration
 ******************************************************************************/

/**
 * @brief  System Clock Frequency in Hz
 * @note   Default is 24MHz (HIRC)
 */
#define HAL_SYSCLK_FREQ     24000000UL

/**
 * @brief  HIRC Frequency Selection
 *         - HAL_HIRC_24MHZ: 24 MHz (default)
 *         - HAL_HIRC_16MHZ: 16 MHz
 */
#define HAL_HIRC_FREQ       HAL_HIRC_24MHZ

/*******************************************************************************
 * HAL Module Enable/Disable
 * Comment out modules not used to reduce code size
 ******************************************************************************/

#define HAL_GPIO_MODULE_ENABLED       /**< Enable GPIO HAL Module */
#define HAL_UART_MODULE_ENABLED       /**< Enable UART HAL Module */
#define HAL_TIMER_MODULE_ENABLED      /**< Enable Timer HAL Module */
#define HAL_ADC_MODULE_ENABLED        /**< Enable ADC HAL Module */
#define HAL_I2C_MODULE_ENABLED        /**< Enable I2C HAL Module */
#define HAL_SPI_MODULE_ENABLED        /**< Enable SPI HAL Module */
#define HAL_PWM_MODULE_ENABLED        /**< Enable PWM HAL Module */
#define HAL_WDT_MODULE_ENABLED        /**< Enable Watchdog HAL Module */
#define HAL_SYSTEM_MODULE_ENABLED     /**< Enable System HAL Module */
#define HAL_FLASH_MODULE_ENABLED      /**< Enable Flash/IAP HAL Module */
#define HAL_BOD_MODULE_ENABLED        /**< Enable BOD (Brown-out Detector) HAL Module */
#define HAL_WKT_MODULE_ENABLED        /**< Enable WKT (Wake-up Timer) HAL Module */

/*******************************************************************************
 * Debug Configuration
 ******************************************************************************/

/**
 * @brief  Enable HAL debug mode
 *         When enabled, HAL functions will perform parameter validation
 */
#define HAL_DEBUG_ENABLED

/**
 * @brief  Enable printf support
 */
#define HAL_PRINTF_ENABLED

/**
 * @brief  Default UART port for printf
 */
#define HAL_PRINTF_UART     HAL_UART0

/*******************************************************************************
 * Timeout Configuration
 ******************************************************************************/

/**
 * @brief  Default timeout value in milliseconds
 */
#define HAL_DEFAULT_TIMEOUT     1000U

/**
 * @brief  Maximum timeout value
 */
#define HAL_MAX_TIMEOUT         0xFFFFFFFFUL

/*******************************************************************************
 * Hardware Pin Configuration
 ******************************************************************************/

/**
 * @brief  Default LED Pin Configuration
 */
#define HAL_LED_PORT        HAL_GPIO_PORT1
#define HAL_LED_PIN         HAL_GPIO_PIN_2

/**
 * @brief  Default UART0 Pin Configuration
 */
#define HAL_UART0_TX_PORT   HAL_GPIO_PORT0
#define HAL_UART0_TX_PIN    HAL_GPIO_PIN_6
#define HAL_UART0_RX_PORT   HAL_GPIO_PORT0
#define HAL_UART0_RX_PIN    HAL_GPIO_PIN_7

/**
 * @brief  Default I2C Pin Configuration
 */
#define HAL_I2C_SDA_PORT    HAL_GPIO_PORT1
#define HAL_I2C_SDA_PIN     HAL_GPIO_PIN_4
#define HAL_I2C_SCL_PORT    HAL_GPIO_PORT1
#define HAL_I2C_SCL_PIN     HAL_GPIO_PIN_3

/**
 * @brief  Default SPI Pin Configuration
 */
#define HAL_SPI_MOSI_PORT   HAL_GPIO_PORT0
#define HAL_SPI_MOSI_PIN    HAL_GPIO_PIN_0
#define HAL_SPI_MISO_PORT   HAL_GPIO_PORT0
#define HAL_SPI_MISO_PIN    HAL_GPIO_PIN_1
#define HAL_SPI_CLK_PORT    HAL_GPIO_PORT1
#define HAL_SPI_CLK_PIN     HAL_GPIO_PIN_0
#define HAL_SPI_SS_PORT     HAL_GPIO_PORT1
#define HAL_SPI_SS_PIN      HAL_GPIO_PIN_5

/*******************************************************************************
 * Assert Configuration
 ******************************************************************************/

#ifdef HAL_DEBUG_ENABLED
    /**
     * @brief  HAL Assert macro for parameter validation
     * @param  expr Expression to evaluate
     */
    #define HAL_ASSERT(expr)  do { \
        if (!(expr)) { \
            HAL_AssertFailed(__FILE__, __LINE__); \
        } \
    } while(0)
    
    void HAL_AssertFailed(const char *file, uint32_t line);
#else
    #define HAL_ASSERT(expr)  ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MS51_HAL_CONF_H */
