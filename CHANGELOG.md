# Changelog

All notable changes to the MS51 BSP will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.0] - 2025-11-30

### Added

#### Board Support Package (BSP)
- `bsp/` folder with complete board support infrastructure
- `bsp/startup/` - Startup code for Keil, IAR, and SDCC compilers
- `bsp/linker/` - Linker scripts for all device variants (8K, 16K, 32K)
- `bsp/device/` - Device configuration headers with memory maps and peripheral availability
- `bsp/boards/generic/` - Generic board template for custom hardware

### Changed

- Simplified project structure by consolidating device support
- Updated documentation to reflect new BSP architecture
- Root `README.md` now serves as project entry point

### Removed

- **Legacy vendor folders** - Removed `MS51DA9AE_MS51BA9AE/`, `MS51FB9AE_MS51XB9AE_MS51XB9BE/`, and `MS51FC0AE_MS51XC0BE_MS51EB0AE_MS51EC0AE_MS51TC0AE_MS51PC0AE/` folders
- **Document folder** - Removed `Document/` (historical git revision info archived in CHANGELOG)
- Duplicate sample code (144+ legacy samples replaced by 26 unified examples)

### Notes

This release completes the migration from the original vendor BSP structure to a clean, modern layered architecture. All device-specific code is now unified using compile-time device selection macros (`MS51_8K`, `MS51_16K`, `MS51_32K`).

---

## [2.0.0] - 2025-11-30

### Added

#### HAL Layer
- `ms51_hal.h` - Unified HAL header with version information
- `ms51_compiler.h` - Multi-compiler abstraction (Keil, IAR, SDCC)
- `ms51_types.h` - Standard type definitions and utility macros
- `ms51_sfr_bits.h` - Symbolic bit position definitions for all SFRs
- `ms51_sfr_8k.h` - SFR definitions for 8KB devices (MS51DA9AE, MS51BA9AE)
- `ms51_sfr_16k.h` - SFR definitions for 16KB devices (MS51FB9AE, MS51XB9AE, MS51XB9BE)
- `ms51_sfr_32k.h` - SFR definitions for 32KB devices with Page 2 extended registers

#### Driver Layer
- `drv_gpio.h/c` - GPIO driver with mode control, interrupt support
- `drv_uart.h/c` - UART0/UART1 driver with interrupt and polling modes
- `drv_timer.h/c` - Timer0-3 driver with multiple modes
- `drv_adc.h/c` - 12-bit ADC driver with multiple trigger sources
- `drv_pwm.h/c` - 6-channel PWM driver with complementary mode support
- `drv_spi.h/c` - SPI master/slave driver
- `drv_i2c.h/c` - I2C master/slave driver
- `drv_wdt.h/c` - Watchdog timer driver
- `drv_flash.h/c` - Flash/IAP programming driver

#### Service Layer
- `svc_delay.h/c` - Timer-based delay and tick services
- `svc_console.h/c` - Printf, debug logging, and console input
- `svc_eeprom.h/c` - EEPROM emulation with wear leveling
- `svc_power.h/c` - Power management and wake-up control
- `svc_system.h/c` - System control, clock, WDT, device info

#### Documentation
- `docs/api/hal_api.md` - HAL layer API reference
- `docs/api/driver_api.md` - Driver layer API reference
- `docs/api/service_api.md` - Service layer API reference
- `docs/hardware/ms51_overview.md` - Device family overview
- `docs/hardware/peripheral_notes.md` - Peripheral programming notes
- `docs/porting_guide.md` - Porting guide for new projects

#### Samples
- `samples/blink_led/` - GPIO driver demonstration
- `samples/uart_echo/` - UART and console service demonstration
- `samples/adc_read/` - ADC driver with VDD measurement
- `samples/pwm_led/` - PWM breathing LED effect
- `samples/low_power/` - Power-down with WKT wake-up

### Changed

- Complete architecture redesign with layered approach (HAL → Driver → Service)
- Unified API patterns across all drivers
- Consistent error handling with `drv_status_t` return codes
- Handle-based driver instances for better state management

### Deprecated

- Original register-based sample code (retained in legacy folder for reference)

### Removed

- Direct SFR manipulation in application code (now abstracted through HAL)

### Security

- TA-protected register access properly abstracted in HAL
- SPROM support for secure code storage

---

## [1.0.0] - Previous Release

### Notes

Original Nuvoton MS51 BSP release with register-based sample code.
This version is retained in the `MS51*/SampleCode/` directories for reference.

---

## Migration Guide

### Migrating from 1.x to 2.0

1. **Include Headers**
   ```c
   /* Old */
   #include "MS51_16K.h"
   
   /* New */
   #include "ms51_hal.h"
   #include "drv_gpio.h"  /* Add drivers as needed */
   ```

2. **GPIO Configuration**
   ```c
   /* Old */
   P1M1 &= ~0x01;
   P1M2 |= 0x01;
   P10 = 0;
   
   /* New */
   gpio_handle_t led;
   gpio_config_t cfg = {
       .port = GPIO_PORT_1,
       .pin = 0,
       .mode = GPIO_MODE_PUSH_PULL
   };
   DRV_GPIO_Init(&led, &cfg);
   DRV_GPIO_Write(&led, 0);
   ```

3. **UART Configuration**
   ```c
   /* Old */
   SCON = 0x52;
   TMOD = 0x20;
   TH1 = 0xFA;
   TR1 = 1;
   
   /* New */
   uart_handle_t uart;
   uart_config_t cfg = {
       .port = UART_PORT_0,
       .baudrate = 115200
   };
   DRV_UART_Init(&uart, &cfg);
   ```

4. **Error Handling**
   ```c
   /* Old - no error checking */
   InitUART0();
   
   /* New - proper error handling */
   drv_status_t status = DRV_UART_Init(&uart, &cfg);
   if (status != DRV_OK) {
       /* Handle error */
   }
   ```

See [Porting Guide](docs/porting_guide.md) for complete migration instructions.
