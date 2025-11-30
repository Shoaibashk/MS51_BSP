# MS51 BSP Sample Code

This directory contains sample applications demonstrating the new layered MS51 BSP architecture.

## Architecture Overview

All samples use the layered architecture:
```
┌─────────────────────────────────────┐
│         Application Sample          │
├─────────────────────────────────────┤
│      Service Layer (SVC_xxx)        │
├─────────────────────────────────────┤
│      Driver Layer (DRV_xxx)         │
├─────────────────────────────────────┤
│        HAL Layer (HAL_xxx)          │
├─────────────────────────────────────┤
│           Hardware                  │
└─────────────────────────────────────┘
```

## Sample Categories

### GPIO Samples

| Sample | Description |
|--------|-------------|
| [gpio_input_output](gpio_input_output/) | Basic GPIO input/output, all pin modes |
| [gpio_pin_interrupt](gpio_pin_interrupt/) | Pin interrupt with power-down wakeup |
| [gpio_clock_out](gpio_clock_out/) | Clock output on GPIO pin |

### UART Samples

| Sample | Description |
|--------|-------------|
| [uart_printf](uart_printf/) | Printf via console service |
| [uart_interrupt](uart_interrupt/) | Interrupt-driven UART with ring buffer |

### Timer Samples

| Sample | Description |
|--------|-------------|
| [timer_interrupt](timer_interrupt/) | Timer with callback ISR |
| [timer_capture](timer_capture/) | Input capture for frequency measurement |

### ADC Samples

| Sample | Description |
|--------|-------------|
| [adc_simple](adc_simple/) | Basic ADC with bandgap VDD measurement |
| [adc_multi_channel](adc_multi_channel/) | Multi-channel scanning |
| [adc_compare](adc_compare/) | Hardware compare with interrupt |

### PWM Samples

| Sample | Description |
|--------|-------------|
| [pwm_independent](pwm_independent/) | Multiple independent PWM channels |
| [pwm_complementary](pwm_complementary/) | Complementary with dead-time |

### SPI Samples

| Sample | Description |
|--------|-------------|
| [spi_master](spi_master/) | SPI master with loopback test |
| [spi_flash](spi_flash/) | W25Qxx SPI Flash read/write |

### I2C Samples

| Sample | Description |
|--------|-------------|
| [i2c_master](i2c_master/) | I2C master with bus scan |
| [i2c_eeprom](i2c_eeprom/) | AT24Cxx EEPROM access |

### Watchdog Samples

| Sample | Description |
|--------|-------------|
| [wdt_reset](wdt_reset/) | WDT reset mode demonstration |
| [wdt_interrupt](wdt_interrupt/) | WDT interrupt mode |

### Flash/EEPROM Samples

| Sample | Description |
|--------|-------------|
| [flash_dataflash](flash_dataflash/) | Data flash as EEPROM emulation |

### Power Management Samples

| Sample | Description |
|--------|-------------|
| [power_down](power_down/) | Power-Down mode with wakeup sources |

### Bootloader Samples

| Sample | Description |
|--------|-------------|
| [isp_uart](isp_uart/) | UART ISP bootloader |

## Building Samples

### Keil uVision

1. Open project file in Keil
2. Select target MCU variant (MS51FB9AE, etc.)
3. Include required source files:
   - `hal/ms51_hal.c`
   - Required driver files from `drivers/`
   - Required service files from `services/`
   - Sample `main.c`
4. Build (F7)

### IAR Embedded Workbench

1. Create new project
2. Add source files as above
3. Configure device in Options
4. Build

### SDCC

```bash
sdcc -mmcs51 \
    -I../../hal \
    -I../../drivers \
    -I../../services \
    ../../hal/ms51_hal.c \
    ../../drivers/drv_gpio.c \
    ../../services/svc_delay.c \
    main.c \
    -o sample.ihx
```

## Hardware Setup

### Default Pin Assignments

| Function | Pin | Notes |
|----------|-----|-------|
| LED | P1.4 | Active low |
| Button | P3.0 | Active low, needs pull-up |
| UART0 RX | P0.7 | |
| UART0 TX | P0.6 | |
| I2C SDA | P1.3 | Needs 4.7kΩ pull-up |
| I2C SCL | P1.4 | Needs 4.7kΩ pull-up |
| SPI CLK | P1.0 | |
| SPI MOSI | P0.0 | |
| SPI MISO | P0.1 | |
| SPI SS | P1.5 | Software controlled |

## Common Code Pattern

All samples follow this initialization pattern:

```c
#include "ms51_hal.h"
#include "drv_xxx.h"
#include "svc_xxx.h"

int main(void)
{
    system_config_t sys_cfg;
    xxx_config_t xxx_cfg;

    /* 1. System initialization */
    sys_cfg.clock_source = CLK_SRC_HIRC_24MHZ;
    sys_cfg.clock_divider = 0;
    sys_cfg.wdt_enable = 0;
    SVC_SYSTEM_Init(&sys_cfg);

    /* 2. Delay service (used by many peripherals) */
    SVC_DELAY_Init(NULL);

    /* 3. Console for debug output */
    SVC_CONSOLE_Init(&con_cfg);

    /* 4. Peripheral initialization */
    DRV_XXX_Init(&handle, &xxx_cfg);

    /* 5. Enable global interrupts if needed */
    HAL_INT_Enable();

    /* 6. Main loop */
    while (1) {
        /* Application code */
    }
}
```

## Console Output

Most samples output debug information via UART0 at 115200 baud.
Connect a USB-to-Serial adapter to view output in a terminal.

## License

SPDX-License-Identifier: Apache-2.0
Copyright (c) 2025 Nuvoton Technology Corp.
