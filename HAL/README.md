# MS51 HAL Library

A comprehensive Hardware Abstraction Layer (HAL) library for Nuvoton MS51 series microcontrollers.

## Features

- **Multi-compiler support**: Keil C51, SDCC, and IAR Embedded Workbench
- **Complete peripheral drivers**: GPIO, UART, Timer, ADC, I2C, SPI, PWM, WDT, Flash, BOD, WKT
- **STM32-style API**: Familiar, consistent, and well-documented interface
- **Ready-to-use examples**: Working examples for all peripherals
- **Project templates**: Quick-start templates for each compiler
- **Power management**: Idle and power-down modes with wake-up timer

## Getting Started

1. **Copy HAL folder** to your project
2. **Include the main header**: `#include "ms51_hal.h"`
3. **Configure modules** in `ms51_hal_conf.h`
4. **Add source files** to your project
5. **Build and run!**

### Quick Example

```c
#include "ms51_hal.h"
#include <stdio.h>

void main(void)
{
    HAL_GPIO_InitTypeDef gpio;
    
    // Initialize HAL
    HAL_Init();
    
    // Configure LED on P1.2
    gpio.Pin = HAL_GPIO_PIN_2;
    gpio.Mode = HAL_GPIO_MODE_PUSHPULL;
    gpio.Schmitt = HAL_GPIO_SCHMITT_DISABLE;
    HAL_GPIO_Init(HAL_GPIO_PORT1, &gpio);
    
    // Initialize UART for printf
    HAL_UART_InitPrintf(24000000UL);
    
    printf("Hello, MS51 HAL!\n\r");
    
    while (1) {
        HAL_GPIO_TogglePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2);
        HAL_Delay(500);
    }
}
```

## Directory Structure

```
HAL/
├── Core/              # Core HAL files
│   ├── Inc/          # Headers
│   └── Src/          # Source files
├── Device/           # Device-specific headers
│   ├── MS51_16K/
│   ├── MS51_8K/
│   └── MS51_4K/
├── Drivers/          # Peripheral drivers
│   ├── GPIO/
│   ├── UART/
│   ├── Timer/
│   ├── ADC/
│   ├── I2C/
│   ├── SPI/
│   ├── PWM/
│   ├── WDT/
│   ├── Flash/
│   ├── System/
│   ├── BOD/          # Brown-out Detector
│   └── WKT/          # Wake-up Timer
├── Examples/         # Example projects
│   ├── GPIO/
│   ├── UART/
│   ├── Timer/
│   ├── ADC/
│   ├── I2C/
│   ├── SPI/
│   ├── PWM/
│   ├── WDT/
│   ├── BOD/
│   └── PowerManagement/
├── Templates/        # Project templates
└── Doc/              # Documentation
```

## Supported Devices

| Variant | Flash | SRAM | Package |
|---------|-------|------|---------|
| MS51FB9AE | 16K | 1K | TSSOP20 |
| MS51XB9AE | 16K | 1K | LQFP32 |
| MS51DA9AE | 8K | 1K | TSSOP20 |
| MS51BA9AE | 8K | 1K | SOP16 |
| MS51FC0AE | 4K | 256 | TSSOP20 |

## Documentation

- [Complete Documentation](Doc/README.md)
- [Migration Guide (BSP to HAL)](Doc/MIGRATION.md)

## Examples

| Example | Description |
|---------|-------------|
| gpio_blink | LED blinking |
| gpio_input | Button input |
| uart_printf | Debug output |
| uart_echo | Serial echo |
| adc_single | ADC reading |
| adc_vdd | VDD measurement |
| i2c_eeprom | EEPROM read/write |
| pwm_led | LED dimmer |
| spi_master | SPI master mode |
| spi_slave | SPI slave mode with interrupt |
| timer_interrupt | Timer interrupt example |
| timer_delay | Timer-based delays |
| wdt_reset | Watchdog timer reset |
| bod_detect | Brown-out detection |
| powerdown_wakeup | Power-down with wake-up timer |

## License

Apache-2.0 License

## Contributing

Contributions are welcome! Please read the [documentation](Doc/README.md) first.
