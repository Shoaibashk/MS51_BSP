# MS51 BSP Architecture Overview

## 1. High-Level Architecture Summary

The MS51 BSP implements a **layered architecture** that separates hardware-specific code from application logic. This design enables:
- **Portability** across MS51 device variants
- **Testability** through hardware abstraction
- **Maintainability** with clear module boundaries
- **Reusability** of driver and service code

```
┌─────────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                            │
│  (User application code, business logic, state machines)            │
├─────────────────────────────────────────────────────────────────────┤
│                         SERVICE LAYER                               │
│  (Printf, Delay, EEPROM Emulation, Power Mgmt, Debug Console)       │
├─────────────────────────────────────────────────────────────────────┤
│                         DRIVER LAYER                                │
│  (GPIO, UART, SPI, I2C, Timer, PWM, ADC, WDT, WKT, BOD, IAP)        │
├─────────────────────────────────────────────────────────────────────┤
│                           HAL LAYER                                 │
│  (Register Access, Compiler Abstraction, Interrupt Vectors)         │
├─────────────────────────────────────────────────────────────────────┤
│                           BSP LAYER                                 │
│  (Startup, Linker Config, Board Pinout, Clock Config)               │
├─────────────────────────────────────────────────────────────────────┤
│                          HARDWARE                                   │
│  (MS51 MCU: CPU, Flash, SRAM, Peripherals)                          │
└─────────────────────────────────────────────────────────────────────┘
```

## 2. Layer Responsibilities

### 2.1 Hardware Layer
**Responsibility:** Physical MS51 microcontroller hardware.
- 8051-compatible CPU core (1T @ 24MHz max)
- Flash memory (APROM, LDROM, SPROM, CONFIG)
- IRAM (256 bytes), XRAM (up to 2KB)
- On-chip peripherals

### 2.2 BSP Layer
**Responsibility:** Board-specific and device-specific initialization.
- **Startup code:** Reset vector, stack initialization, memory init
- **Linker configuration:** Memory placement, section definitions
- **Board pinout:** Device variant pin assignments
- **Clock configuration:** HIRC/LIRC/external clock setup
- **Power-on defaults:** Safe GPIO states, watchdog config

**Files:**
```
bsp/
├── startup/           # Compiler-specific startup files
├── linker/            # Linker scripts (Keil, IAR, SDCC)
├── boards/            # Board-specific configurations
└── clock/             # Clock source configurations
```

### 2.3 HAL Layer
**Responsibility:** Hardware abstraction and compiler portability.
- **SFR definitions:** Register addresses for each device variant
- **Bit manipulation macros:** Atomic set/clear operations
- **TA-protected register access:** Timed-access unlock sequences
- **Compiler abstraction:** `__data`, `__code`, interrupt syntax
- **Interrupt vector definitions:** Standard vector numbers

**Files:**
```
hal/
├── include/
│   ├── ms51_hal.h            # Main HAL header
│   ├── ms51_sfr.h            # SFR definitions (per variant)
│   ├── ms51_sfr_bits.h       # Bit manipulation macros
│   ├── ms51_compiler.h       # Compiler abstraction
│   └── ms51_types.h          # Type definitions
└── src/
    └── ms51_hal.c            # HAL implementation
```

### 2.4 Driver Layer
**Responsibility:** Peripheral driver implementations.
- Hardware initialization for each peripheral
- Polled and interrupt-driven operation modes
- Error handling and timeout support
- Clean API with consistent patterns

**Peripheral Drivers:**
| Driver | Description |
|--------|-------------|
| `gpio` | Port configuration, input/output, interrupts |
| `uart` | UART0/UART1 with baud rate generation |
| `spi` | SPI master/slave modes |
| `i2c` | I2C master/slave with timeout |
| `timer` | Timer0/1/2/3 modes (delay, PWM, capture) |
| `pwm` | 6-channel PWM with complementary/dead-time |
| `adc` | 12-bit ADC with multiple trigger sources |
| `wdt` | Watchdog timer |
| `wkt` | Wake-up timer |
| `bod` | Brown-out detector |
| `iap` | In-application programming |

**Files:**
```
drivers/
├── include/
│   ├── drv_gpio.h
│   ├── drv_uart.h
│   ├── drv_spi.h
│   ├── drv_i2c.h
│   ├── drv_timer.h
│   ├── drv_pwm.h
│   ├── drv_adc.h
│   ├── drv_wdt.h
│   ├── drv_wkt.h
│   ├── drv_bod.h
│   └── drv_iap.h
└── src/
    ├── drv_gpio.c
    ├── drv_uart.c
    └── ...
```

### 2.5 Service Layer
**Responsibility:** Higher-level services built on drivers.
- **Debug console:** Printf/scanf over UART
- **Delay functions:** Blocking and non-blocking delays
- **EEPROM emulation:** Data flash read/write abstraction
- **Power management:** Idle/power-down mode helpers
- **System utilities:** Software reset, watchdog refresh

**Files:**
```
services/
├── include/
│   ├── svc_delay.h
│   ├── svc_console.h
│   ├── svc_eeprom.h
│   ├── svc_power.h
│   └── svc_system.h
└── src/
    ├── svc_delay.c
    ├── svc_console.c
    └── ...
```

### 2.6 Application Layer
**Responsibility:** User application code.
- Business logic and state machines
- Application-specific initialization
- Main loop and event handling

**Files:**
```
app/
├── main.c
├── app_config.h
└── ...
```

## 3. Dependency Rules

```
     APPLICATION
          │
          ▼
       SERVICE ────────┐
          │            │
          ▼            ▼
       DRIVER ←──── (direct HAL access allowed)
          │
          ▼
         HAL
          │
          ▼
         BSP
```

**Rules:**
1. Upper layers depend only on layers directly below
2. Drivers may access HAL directly (for efficiency)
3. Services may access HAL for performance-critical code
4. Application should primarily use Service/Driver APIs
5. No circular dependencies between layers

## 4. Key Design Patterns

### 4.1 Handle-Based API
Each driver uses opaque handles for multi-instance support:
```c
uart_handle_t uart0;
DRV_UART_Init(&uart0, UART_PORT_0, &config);
DRV_UART_Write(&uart0, data, len);
```

### 4.2 Configuration Structures
All initialization uses typed config structures:
```c
typedef struct {
    uint32_t baudrate;
    uint8_t  data_bits;
    uint8_t  parity;
    uint8_t  stop_bits;
} uart_config_t;
```

### 4.3 Callback Registration
Interrupt handlers use callbacks for flexibility:
```c
DRV_UART_RegisterCallback(&uart0, UART_EVT_RX, my_rx_handler);
```

### 4.4 Error Return Codes
All functions return status codes:
```c
typedef enum {
    DRV_OK = 0,
    DRV_ERROR,
    DRV_BUSY,
    DRV_TIMEOUT,
    DRV_INVALID_PARAM
} drv_status_t;
```

## 5. Memory Considerations

### 5.1 Memory Map (MS51 16KB variant)
```
┌───────────────────┐ 0xFFFF
│     Reserved      │
├───────────────────┤ 0x4000
│      APROM        │ (16KB application)
├───────────────────┤ 0x0000

┌───────────────────┐ 0x03FF
│      XRAM         │ (1KB external data)
├───────────────────┤ 0x0000

┌───────────────────┐ 0x00FF
│      IRAM         │ (256B internal data)
│   (Stack grows ↓) │
├───────────────────┤ 0x0080
│   Direct/Indirect │ (128B)
├───────────────────┤ 0x0000
```

### 5.2 Code Placement Guidelines
| Memory Type | Usage |
|-------------|-------|
| `__code` | Constant data, lookup tables |
| `__data` | Frequently accessed variables (<128B) |
| `__idata` | Stack, general variables (128-255) |
| `__xdata` | Large buffers, less frequent access |

## 6. Next Steps

- See [Concurrency Model](concurrency.md) for ISR guidelines
- See [Memory Map](memory_map.md) for detailed memory layout
- See [Coding Standards](../guides/coding_standards.md) for style guide
