/**
 * @file board_generic.h
 * @brief Generic development board configuration
 * 
 * This file provides a template board configuration that can be
 * customized for specific development boards or custom hardware.
 *
 * @copyright Copyright (c) 2024-2025 MS51_BSP Contributors
 * @license SPDX-License-Identifier: Apache-2.0
 */

#ifndef BOARD_GENERIC_H
#define BOARD_GENERIC_H

/*----------------------------------------------------------------------------
 * Board Identification
 *----------------------------------------------------------------------------*/
#define BOARD_NAME              "Generic MS51 Board"
#define BOARD_VERSION           "1.0.0"

/*----------------------------------------------------------------------------
 * Target Device Selection
 * Uncomment ONE of the following to select your target device family
 *----------------------------------------------------------------------------*/
/* #define MS51_8K */         /* 8KB devices: MS51DA9AE, MS51BA9AE */
/* #define MS51_16K */        /* 16KB devices: MS51FB9AE, MS51XB9AE, MS51XB9BE */
#define MS51_32K              /* 32KB devices: MS51FC0AE, MS51XC0BE, etc. */

/*----------------------------------------------------------------------------
 * System Clock Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_SYSCLK_HZ         24000000UL  /* 24 MHz HIRC */

/*----------------------------------------------------------------------------
 * LED Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_LED_COUNT         1
#define BOARD_LED0_PORT         GPIO_PORT_1
#define BOARD_LED0_PIN          0
#define BOARD_LED0_ACTIVE_LOW   1           /* LED on when pin is LOW */

/*----------------------------------------------------------------------------
 * Button Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_BUTTON_COUNT      1
#define BOARD_BTN0_PORT         GPIO_PORT_1
#define BOARD_BTN0_PIN          1
#define BOARD_BTN0_ACTIVE_LOW   1           /* Button pressed when pin is LOW */

/*----------------------------------------------------------------------------
 * UART Console Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_CONSOLE_UART      0           /* UART0 */
#define BOARD_CONSOLE_BAUDRATE  115200UL
#define BOARD_CONSOLE_TX_PORT   GPIO_PORT_0
#define BOARD_CONSOLE_TX_PIN    6           /* P0.6 = TXD0 */
#define BOARD_CONSOLE_RX_PORT   GPIO_PORT_0
#define BOARD_CONSOLE_RX_PIN    7           /* P0.7 = RXD0 */

/*----------------------------------------------------------------------------
 * I2C Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_I2C_SCL_PORT      GPIO_PORT_1
#define BOARD_I2C_SCL_PIN       3           /* P1.3 = SCL */
#define BOARD_I2C_SDA_PORT      GPIO_PORT_1
#define BOARD_I2C_SDA_PIN       4           /* P1.4 = SDA */

/*----------------------------------------------------------------------------
 * SPI Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_SPI_MOSI_PORT     GPIO_PORT_0
#define BOARD_SPI_MOSI_PIN      0           /* P0.0 = MOSI */
#define BOARD_SPI_MISO_PORT     GPIO_PORT_0
#define BOARD_SPI_MISO_PIN      1           /* P0.1 = MISO */
#define BOARD_SPI_SCK_PORT      GPIO_PORT_0
#define BOARD_SPI_SCK_PIN       2           /* P0.2 = SPCLK */
#define BOARD_SPI_SS_PORT       GPIO_PORT_0
#define BOARD_SPI_SS_PIN        3           /* P0.3 = SS */

/*----------------------------------------------------------------------------
 * PWM Output Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_PWM0_PORT         GPIO_PORT_1
#define BOARD_PWM0_PIN          2           /* P1.2 = PWM0 */

/*----------------------------------------------------------------------------
 * ADC Input Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_ADC0_PORT         GPIO_PORT_1
#define BOARD_ADC0_PIN          7           /* P1.7 = AIN7 */

/*----------------------------------------------------------------------------
 * External Interrupt Configuration
 *----------------------------------------------------------------------------*/
#define BOARD_INT0_PORT         GPIO_PORT_3
#define BOARD_INT0_PIN          0           /* P3.0 = INT0 */
#define BOARD_INT1_PORT         GPIO_PORT_3
#define BOARD_INT1_PIN          1           /* P3.1 = INT1 */

/*----------------------------------------------------------------------------
 * Board Initialization Macro
 *----------------------------------------------------------------------------*/
#define BOARD_INIT() do { \
    /* Add board-specific initialization here */ \
} while(0)

#endif /* BOARD_GENERIC_H */
