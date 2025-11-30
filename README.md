# MS51 Board Support Package (BSP)

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)
[![Version](https://img.shields.io/badge/version-2.1.0-green.svg)](CHANGELOG.md)

A modern, layered Board Support Package for the Nuvoton MS51 8051-based microcontroller family. This BSP provides a clean abstraction layer with unified APIs across all MS51 device variants and multiple compiler toolchains.

## ✨ Features

- **Layered Architecture** — Clean separation between HAL, Drivers, and Services
- **Multi-Device Support** — Single codebase for all MS51 variants (8K/16K/32K)
- **Multi-Compiler Support** — Works with Keil C51, IAR, and SDCC
- **Rich Driver Set** — GPIO, UART, SPI, I2C, Timer, PWM, ADC, WDT, Flash
- **High-Level Services** — Console, Delay, EEPROM, Power Management, System
- **26 Sample Applications** — Ready-to-run examples for all peripherals
- **Comprehensive Documentation** — API references, guides, and tutorials

## 📁 Project Structure

```
MS51_BSP/
├── hal/                    # Hardware Abstraction Layer
│   ├── include/           # HAL headers (SFR definitions, types, compiler abstraction)
│   └── src/               # HAL implementation
├── drivers/               # Peripheral Drivers
│   ├── include/           # Driver headers (GPIO, UART, SPI, I2C, etc.)
│   └── src/               # Driver implementations
├── services/              # High-Level Services
│   ├── include/           # Service headers (console, delay, EEPROM, etc.)
│   └── src/               # Service implementations
├── bsp/                   # Board Support Package
│   ├── startup/           # Startup code (Keil, IAR, SDCC)
│   ├── linker/            # Linker scripts
│   ├── device/            # Device configurations
│   └── boards/            # Board definitions
├── samples/               # Example applications
└── docs/                  # Documentation
```

## 🎯 Supported Devices

| Family | Flash | RAM | Devices |
|--------|-------|-----|---------|
| 8KB    | 8 KB  | 256 B | MS51DA9AE, MS51BA9AE |
| 16KB   | 16 KB | 1 KB  | MS51FB9AE, MS51XB9AE, MS51XB9BE |
| 32KB   | 32 KB | 1 KB  | MS51FC0AE, MS51XC0BE, MS51EB0AE, MS51EC0AE, MS51TC0AE, MS51PC0AE |

## 🚀 Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/Shoaibashk/MS51_BSP.git
cd MS51_BSP
```

### 2. Select Your Device

In your project, define one of the following macros:

```c
#define MS51_8K     /* For 8KB devices */
#define MS51_16K    /* For 16KB devices */
#define MS51_32K    /* For 32KB devices */
```

### 3. Include the Headers

```c
#include "ms51_hal.h"       /* HAL layer */
#include "ms51_drivers.h"   /* All drivers */
#include "ms51_services.h"  /* All services */
```

### 4. Write Your Application

```c
#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"

int main(void)
{
    gpio_handle_t led;
    gpio_config_t led_cfg = {
        .port = GPIO_PORT_1,
        .pin = 0,
        .mode = GPIO_MODE_PUSH_PULL
    };

    /* Initialize HAL and GPIO */
    HAL_Init();
    DRV_GPIO_Init(&led, &led_cfg);

    /* Blink LED forever */
    while (1) {
        DRV_GPIO_Toggle(&led);
        SVC_Delay_Ms(500);
    }
}
```

## 🔧 Compiler Setup

### Keil C51

1. Add include paths:
   - `hal/include`
   - `drivers/include`
   - `services/include`
   - `bsp/device`
2. Add source files from `hal/src`, `drivers/src`, `services/src`
3. Use startup file from `bsp/startup/keil/`
4. Use linker file from `bsp/linker/keil/`

### IAR Embedded Workbench

1. Add include paths as above
2. Use startup file from `bsp/startup/iar/`
3. Use linker configuration from `bsp/linker/iar/`

### SDCC

1. Add include paths with `-I` flags
2. Use startup file from `bsp/startup/sdcc/`
3. Use linker script from `bsp/linker/sdcc/`

```bash
sdcc -mmcs51 -I hal/include -I drivers/include -I services/include \
     --code-size 0x7600 main.c hal/src/*.c drivers/src/*.c
```

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [Getting Started](docs/guides/getting_started.md) | Setup and first project |
| [Architecture Overview](docs/architecture/overview.md) | System design and layers |
| [HAL API Reference](docs/api/hal_api.md) | HAL function documentation |
| [Driver API Reference](docs/api/driver_api.md) | Driver function documentation |
| [Service API Reference](docs/api/service_api.md) | Service function documentation |
| [Porting Guide](docs/porting_guide.md) | Porting to new devices/compilers |
| [Coding Standards](docs/guides/coding_standards.md) | Code style and conventions |

## 📂 Sample Applications

The `samples/` directory contains ready-to-run examples:

| Category | Samples |
|----------|---------|
| **GPIO** | `blink_led`, `gpio_input_output`, `gpio_pin_interrupt`, `gpio_clock_out` |
| **UART** | `uart_echo`, `uart_interrupt`, `uart_printf` |
| **SPI** | `spi_master`, `spi_flash` |
| **I2C** | `i2c_master`, `i2c_eeprom` |
| **Timer** | `timer_interrupt`, `timer_capture` |
| **PWM** | `pwm_led`, `pwm_independent`, `pwm_complementary` |
| **ADC** | `adc_simple`, `adc_read`, `adc_multi_channel`, `adc_compare` |
| **WDT** | `wdt_reset`, `wdt_interrupt` |
| **Flash** | `flash_dataflash` |
| **Power** | `low_power`, `power_down` |
| **ISP** | `isp_uart` |

See [samples/README.md](samples/README.md) for detailed descriptions.

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────┐
│                   Application                       │
├─────────────────────────────────────────────────────┤
│   Services (svc_console, svc_delay, svc_system)     │
├─────────────────────────────────────────────────────┤
│   Drivers (drv_gpio, drv_uart, drv_spi, drv_i2c)    │
├─────────────────────────────────────────────────────┤
│   HAL (ms51_hal, ms51_sfr, ms51_compiler)           │
├─────────────────────────────────────────────────────┤
│                  Hardware (MS51 MCU)                │
└─────────────────────────────────────────────────────┘
```

## 📄 License

This project is licensed under the Apache License 2.0 - see the [LICENSE.txt](LICENSE.txt) file for details.

## 🤝 Contributing

Contributions are welcome! Please read our coding standards and submit pull requests for any improvements.

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/Shoaibashk/MS51_BSP/issues)
- **Documentation**: See the [docs/](docs/) folder

---

**MS51_BSP** - A clean, modern BSP for Nuvoton MS51 microcontrollers.
