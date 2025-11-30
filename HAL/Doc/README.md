# MS51 HAL Library Documentation

## Overview

The MS51 HAL (Hardware Abstraction Layer) Library provides a unified, high-level API for programming Nuvoton MS51 series microcontrollers. This library is designed to be:

- **Portable**: Works with Keil C51, SDCC, and IAR compilers
- **Consistent**: Follows STM32 HAL-style API conventions
- **Well-documented**: Comprehensive documentation and examples
- **Efficient**: Optimized for 8051 architecture

## Supported Devices

| Device | Flash | SRAM | GPIO | Status |
|--------|-------|------|------|--------|
| MS51FB9AE | 16K | 1K | 18 | ✅ Supported |
| MS51XB9AE | 16K | 1K | 18 | ✅ Supported |
| MS51XB9BE | 16K | 1K | 18 | ✅ Supported |
| MS51DA9AE | 8K | 1K | 18 | ✅ Supported |
| MS51BA9AE | 8K | 1K | 18 | ✅ Supported |
| MS51FC0AE | 4K | 256 | 8 | ✅ Supported |

## Compiler Support

| Compiler | Version | Status |
|----------|---------|--------|
| Keil C51 | 9.x+ | ✅ Supported |
| SDCC | 4.x+ | ✅ Supported |
| IAR 8051 | 10.x+ | ✅ Supported |

## Features

### HAL Modules

| Module | Description | Header File |
|--------|-------------|-------------|
| GPIO | General Purpose I/O | `ms51_hal_gpio.h` |
| UART | Universal Async Receiver/Transmitter | `ms51_hal_uart.h` |
| Timer | Timer/Counter 0/1/2/3 | `ms51_hal_timer.h` |
| ADC | 12-bit Analog-to-Digital Converter | `ms51_hal_adc.h` |
| I2C | I2C Master/Slave Communication | `ms51_hal_i2c.h` |
| SPI | SPI Master/Slave Communication | `ms51_hal_spi.h` |
| PWM | 6-channel PWM Generator | `ms51_hal_pwm.h` |
| WDT | Watchdog Timer | `ms51_hal_wdt.h` |
| Flash | Flash/IAP Programming | `ms51_hal_flash.h` |
| System | Clock, Power, Reset | `ms51_hal_system.h` |
| BOD | Brown-out Detector | `ms51_hal_bod.h` |
| WKT | Wake-up Timer | `ms51_hal_wkt.h` |
| SPROM | Security Protected ROM | `ms51_hal_sprom.h` |
| EEPROM | EEPROM Emulation (Data Flash) | `ms51_hal_eeprom.h` |

## Quick Start

### 1. Include the HAL header

```c
#include "ms51_hal.h"
```

### 2. Copy and modify the configuration file

Copy `ms51_hal_conf.h` to your project and enable/disable modules as needed:

```c
#define HAL_GPIO_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
// #define HAL_I2C_MODULE_ENABLED  // Disable if not used
```

### 3. Initialize HAL and peripherals

```c
void main(void)
{
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize peripherals */
    HAL_UART_InitPrintf(24000000UL);
    
    /* Your application code */
    printf("Hello, MS51!\n\r");
    
    while (1) {
        /* Main loop */
    }
}
```

## API Reference

### HAL Status Codes

All HAL functions return `HAL_StatusTypeDef`:

| Status | Value | Description |
|--------|-------|-------------|
| `HAL_OK` | 0x00 | Operation successful |
| `HAL_ERROR` | 0x01 | General error |
| `HAL_BUSY` | 0x02 | Resource busy |
| `HAL_TIMEOUT` | 0x03 | Operation timeout |
| `HAL_INVALID` | 0x04 | Invalid parameter |

### GPIO API

```c
/* Initialize GPIO pin */
HAL_StatusTypeDef HAL_GPIO_Init(HAL_GPIO_PortTypeDef port, HAL_GPIO_InitTypeDef *init);

/* Write pin state */
void HAL_GPIO_WritePin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin, 
                       HAL_GPIO_PinStateTypeDef state);

/* Read pin state */
HAL_GPIO_PinStateTypeDef HAL_GPIO_ReadPin(HAL_GPIO_PortTypeDef port, 
                                           HAL_GPIO_PinTypeDef pin);

/* Toggle pin state */
void HAL_GPIO_TogglePin(HAL_GPIO_PortTypeDef port, HAL_GPIO_PinTypeDef pin);
```

### UART API

```c
/* Initialize UART with printf support */
HAL_StatusTypeDef HAL_UART_InitPrintf(uint32_t sysclk);

/* Transmit data (blocking) */
HAL_StatusTypeDef HAL_UART_Transmit(HAL_UART_HandleTypeDef *huart, 
                                     uint8_t *pData, uint16_t size, uint32_t timeout);

/* Receive data (blocking) */
HAL_StatusTypeDef HAL_UART_Receive(HAL_UART_HandleTypeDef *huart, 
                                    uint8_t *pData, uint16_t size, uint32_t timeout);
```

### Timer API

```c
/* Initialize timer */
HAL_StatusTypeDef HAL_Timer0_Init(HAL_Timer01_InitTypeDef *init);

/* Start/Stop timer */
HAL_StatusTypeDef HAL_Timer_Start(HAL_Timer_InstanceTypeDef timer);
HAL_StatusTypeDef HAL_Timer_Stop(HAL_Timer_InstanceTypeDef timer);

/* Delay functions */
void HAL_Timer_DelayMs(uint32_t sysclk, uint16_t ms);
void HAL_Timer_DelayUs(uint32_t sysclk, uint16_t us);
```

### ADC API

```c
/* Initialize ADC */
HAL_StatusTypeDef HAL_ADC_SimpleInit(HAL_ADC_ClkDivTypeDef clockDiv, 
                                      HAL_ADC_AcqTimeTypeDef acqTime);

/* Read ADC value */
uint16_t HAL_ADC_Read(HAL_ADC_ChannelTypeDef channel);

/* Read VDD voltage */
uint16_t HAL_ADC_CalculateVDD(uint16_t bandgapADC);
```

### I2C API

```c
/* Initialize I2C */
HAL_StatusTypeDef HAL_I2C_Init(HAL_I2C_HandleTypeDef *hi2c);

/* Master transmit/receive */
HAL_StatusTypeDef HAL_I2C_MasterTransmit(HAL_I2C_HandleTypeDef *hi2c, 
                                          uint8_t slaveAddr, uint8_t *pData, 
                                          uint16_t size, uint32_t timeout);

/* Memory read/write */
HAL_StatusTypeDef HAL_I2C_Mem_Write(HAL_I2C_HandleTypeDef *hi2c, uint8_t slaveAddr,
                                     uint16_t memAddr, uint8_t memAddrSize,
                                     uint8_t *pData, uint16_t size, uint32_t timeout);
```

### PWM API

```c
/* Initialize PWM */
HAL_StatusTypeDef HAL_PWM_SimpleInit(HAL_PWM_DivTypeDef clockDiv, uint16_t period);

/* Start/Stop PWM */
void HAL_PWM_Start(void);
void HAL_PWM_Stop(void);

/* Set duty cycle */
HAL_StatusTypeDef HAL_PWM_SetDuty(HAL_PWM_ChannelTypeDef channel, uint16_t duty);
HAL_StatusTypeDef HAL_PWM_SetDutyPercent(HAL_PWM_ChannelTypeDef channel, uint8_t percent);
```

### EEPROM API

```c
/* Initialize EEPROM emulation */
HAL_StatusTypeDef HAL_EEPROM_Init(void);

/* Read/Write single bytes */
uint8_t HAL_EEPROM_ReadByte(uint16_t address);
HAL_StatusTypeDef HAL_EEPROM_WriteByte(uint16_t address, uint8_t data);

/* Read/Write arrays */
HAL_StatusTypeDef HAL_EEPROM_Read(uint16_t address, uint8_t *pData, uint16_t size);
HAL_StatusTypeDef HAL_EEPROM_Write(uint16_t address, uint8_t *pData, uint16_t size);

/* Read/Write 16-bit and 32-bit values */
uint16_t HAL_EEPROM_ReadU16(uint16_t address);
HAL_StatusTypeDef HAL_EEPROM_WriteU16(uint16_t address, uint16_t data);
uint32_t HAL_EEPROM_ReadU32(uint16_t address);
HAL_StatusTypeDef HAL_EEPROM_WriteU32(uint16_t address, uint32_t data);

/* Update with wear leveling optimization */
HAL_StatusTypeDef HAL_EEPROM_Update(uint16_t address, uint8_t data);
```

### SPROM API

```c
/* Read SPROM data */
uint8_t HAL_SPROM_ReadByte(uint8_t address);
HAL_StatusTypeDef HAL_SPROM_Read(uint8_t address, uint8_t *pData, uint8_t size);

/* Write SPROM data */
HAL_StatusTypeDef HAL_SPROM_WriteByte(uint8_t address, uint8_t data);
HAL_StatusTypeDef HAL_SPROM_Write(uint8_t address, uint8_t *pData, uint8_t size);

/* Verify and lock */
HAL_StatusTypeDef HAL_SPROM_Verify(uint8_t address, uint8_t *pData, uint8_t size);
uint8_t HAL_SPROM_IsLocked(void);
HAL_StatusTypeDef HAL_SPROM_Lock(void);  /* WARNING: Permanent! */
```

## Polling vs Interrupt-Driven Modes

All major communication peripherals support both polling and interrupt-driven operation:

### Available Modes

| Peripheral | Polling | Interrupt | Notes |
|------------|---------|-----------|-------|
| UART | ✅ `HAL_UART_Transmit()` | ✅ Callbacks | TX/RX with callbacks |
| I2C | ✅ `HAL_I2C_MasterTransmit()` | ✅ `HAL_I2C_MasterTransmit_IT()` | Master/Slave modes |
| SPI | ✅ `HAL_SPI_Transmit()` | ✅ `HAL_SPI_Transmit_IT()` | Full duplex support |
| ADC | ✅ `HAL_ADC_Read()` | ✅ `HAL_ADC_Start_IT()` | Single/Continuous |
| Timer | ✅ `HAL_Timer_DelayMs()` | ✅ `HAL_Timer_EnableInterrupt()` | All timers |
| GPIO | ✅ `HAL_GPIO_ReadPin()` | ✅ `HAL_GPIO_ConfigInterrupt()` | Pin interrupt |
| PWM | ✅ `HAL_PWM_SetDuty()` | ✅ `HAL_PWM_EnableInterrupt()` | Period/Duty interrupts |
| WDT | ✅ `HAL_WDT_Feed()` | ✅ Interrupt mode | Early warning |
| BOD | ✅ `HAL_BOD_GetFlag()` | ✅ `HAL_BOD_EnableInterrupt()` | Brown-out detect |
| WKT | ✅ Polling | ✅ Auto-wakeup | Power-down wake |

### Interrupt Mode Example

```c
/* I2C interrupt mode example */
void main(void)
{
    HAL_I2C_HandleTypeDef hi2c;
    uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
    
    HAL_Init();
    HAL_I2C_Init(&hi2c);
    
    /* Non-blocking transmit with callback */
    HAL_I2C_MasterTransmit_IT(&hi2c, 0x50, data, 4);
    
    while (1) {
        /* Do other work while I2C transfer in progress */
    }
}

/* Callback when transfer completes */
void HAL_I2C_MasterTxCpltCallback(HAL_I2C_HandleTypeDef *hi2c)
{
    /* Transfer complete - handle here */
}
```

## Examples

See the `Examples/` directory for complete example projects:

- `GPIO/gpio_blink` - LED blink example
- `GPIO/gpio_input` - Button input example
- `UART/uart_printf` - Printf debug output
- `UART/uart_echo` - Serial echo example
- `Timer/timer_interrupt` - Timer interrupt example
- `Timer/timer_delay` - Timer-based delays
- `ADC/adc_single` - Single ADC conversion
- `ADC/adc_vdd` - VDD voltage measurement
- `I2C/i2c_eeprom` - External EEPROM read/write
- `SPI/spi_master` - SPI master mode
- `SPI/spi_slave` - SPI slave with interrupt
- `PWM/pwm_led` - LED dimmer with breathing effect
- `WDT/wdt_reset` - Watchdog reset example
- `BOD/bod_detect` - Brown-out detection
- `PowerManagement/powerdown_wakeup` - Power-down with wake-up
- `EEPROM/eeprom_basic` - Internal data flash as EEPROM
- `SPROM/sprom_basic` - Security protected ROM operations

## Migration from BSP

If you're migrating from the original Nuvoton BSP, see [MIGRATION.md](MIGRATION.md) for a detailed guide.

## Directory Structure

```
HAL/
├── Core/
│   ├── Inc/
│   │   ├── ms51_hal.h         # Main HAL header
│   │   └── ms51_hal_conf.h    # Configuration template
│   └── Src/
│       └── ms51_hal.c         # Core HAL implementation
├── Device/
│   └── MS51_16K/
│       └── ms51_16k_hal.h     # Device-specific definitions
├── Drivers/
│   ├── GPIO/                  # GPIO driver
│   ├── UART/                  # UART driver
│   ├── Timer/                 # Timer driver
│   ├── ADC/                   # ADC driver
│   ├── I2C/                   # I2C driver
│   ├── SPI/                   # SPI driver
│   ├── PWM/                   # PWM driver
│   ├── WDT/                   # Watchdog driver
│   ├── Flash/                 # Flash/IAP driver
│   ├── System/                # System driver
│   ├── BOD/                   # Brown-out Detector driver
│   ├── WKT/                   # Wake-up Timer driver
│   ├── SPROM/                 # Security Protected ROM driver
│   └── EEPROM/                # EEPROM Emulation driver
├── Examples/                  # Example projects
├── Templates/                 # Project templates
└── Doc/                       # Documentation
```

## License

This library is licensed under the Apache-2.0 License. See LICENSE for details.

## Contributing

Contributions are welcome! Please submit pull requests with:

1. Clear description of changes
2. Updated documentation if needed
3. Test results on supported hardware

## Support

For issues and feature requests, please use the GitHub issue tracker.
