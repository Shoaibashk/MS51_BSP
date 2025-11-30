# MS51 BSP Porting Guide

## Overview

This guide explains how to port the MS51 BSP to different:
- Device variants (8K, 16K, 32K)
- Compilers (Keil, IAR, SDCC)
- Custom boards and applications

---

## Project Structure

```
MS51_BSP/
├── hal/                    # Hardware Abstraction Layer
│   ├── include/           # HAL headers
│   │   ├── ms51_hal.h     # Main HAL header
│   │   ├── ms51_compiler.h
│   │   ├── ms51_types.h
│   │   ├── ms51_sfr_bits.h
│   │   ├── ms51_sfr_8k.h
│   │   ├── ms51_sfr_16k.h
│   │   └── ms51_sfr_32k.h
│   └── src/               # HAL source
├── drivers/               # Peripheral Drivers
│   ├── include/
│   └── src/
├── services/              # High-level Services
│   ├── include/
│   └── src/
├── bsp/                   # Board Support Package
│   ├── startup/           # Startup code (Keil, IAR, SDCC)
│   ├── linker/            # Linker scripts
│   ├── device/            # Device configurations
│   └── boards/            # Board definitions
├── samples/               # Sample Applications
└── docs/                  # Documentation
```

---

## Device Selection

### Step 1: Choose Device Configuration

Include the appropriate device configuration from `bsp/device/`:

| Device Series | Config File | Flash | RAM |
|---------------|-------------|-------|-----|
| MS51DA9AE, MS51BA9AE | `ms51_8k.h` | 8 KB | 256 B |
| MS51FB9AE, MS51XB9AE, MS51XB9BE | `ms51_16k.h` | 16 KB | 1 KB |
| MS51FC0AE, MS51XC0BE, MS51EB0AE, etc. | `ms51_32k.h` | 32 KB | 1 KB |

### Step 2: Define Device Macro

In your project settings or a configuration header:

```c
/* For 8KB devices */
#define MS51_DEVICE_8K

/* For 16KB devices */
#define MS51_DEVICE_16K

/* For 32KB devices */
#define MS51_DEVICE_32K
```

### Step 3: Configure Memory Addresses

```c
/* Data Flash base address varies by device */
#if defined(MS51_DEVICE_8K)
    #define DATAFLASH_BASE  0x3000
    #define DATAFLASH_SIZE  2048
#elif defined(MS51_DEVICE_16K)
    #define DATAFLASH_BASE  0x3800
    #define DATAFLASH_SIZE  4096
#elif defined(MS51_DEVICE_32K)
    #define DATAFLASH_BASE  0x7000
    #define DATAFLASH_SIZE  4096
#endif
```

---

## Compiler Configuration

### Keil C51

**Project Settings:**
1. Target → Device: Select MS51 variant
2. Target → Memory Model: Small
3. Target → Code Rom Size: Large
4. C51 → Include Paths: Add HAL, drivers, services include folders

**Startup File:**
Use `STARTUP.A51` from Keil or customize for your needs.

**Compiler Defines:**
```
MS51_DEVICE_16K
```

### IAR Embedded Workbench for 8051

**Project Settings:**
1. General Options → Target → Device: Select MS51
2. General Options → Target → Code Model: Large
3. C/C++ Compiler → Preprocessor → Additional include directories

**Compiler Defines:**
```
MS51_DEVICE_16K
```

**Linker Configuration:**
Use default linker file or customize memory regions.

### SDCC

**Makefile Example:**
```makefile
CC = sdcc
CFLAGS = -mmcs51 --model-small
CFLAGS += -I./hal/include -I./drivers/include -I./services/include
CFLAGS += -DMS51_DEVICE_16K

# Memory settings for MS51FB9AE
LDFLAGS = --code-size 16384 --xram-size 1024

SRCS = main.c \
       drivers/src/drv_gpio.c \
       drivers/src/drv_uart.c \
       services/src/svc_delay.c

all: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o firmware.ihx $^
```

---

## Creating a New Project

### Step 1: Create Project Directory

```
my_project/
├── src/
│   └── main.c
├── inc/
│   └── config.h
└── Makefile (or .uvprojx for Keil)
```

### Step 2: Include BSP Files

Add include paths:
- `MS51_BSP/hal/include`
- `MS51_BSP/drivers/include`
- `MS51_BSP/services/include`

Add source files:
- Required HAL files (usually just headers)
- Driver source files for peripherals you use
- Service source files as needed

### Step 3: Create Configuration Header

```c
/* config.h */
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Device selection */
#define MS51_DEVICE_16K

/* System clock */
#define SYSTEM_CLOCK    24000000UL

/* Enable/disable features */
#define USE_CONSOLE     1
#define USE_EEPROM      0

/* Pin assignments */
#define LED_PORT        GPIO_PORT_1
#define LED_PIN         0

#endif
```

### Step 4: Write Main Application

```c
/* main.c */
#include "config.h"
#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"
#include "svc_system.h"

#if USE_CONSOLE
#include "svc_console.h"
#endif

int main(void)
{
    /* Initialize system */
    system_config_t sys_cfg = {
        .clock_source = CLK_SRC_HIRC_24MHZ,
        .clock_divider = 0,
        .wdt_enable = 0
    };
    SVC_SYSTEM_Init(&sys_cfg);

    /* Initialize delay */
    SVC_DELAY_Init(NULL);

#if USE_CONSOLE
    /* Initialize console */
    console_config_t con_cfg = {
        .port = UART_PORT_0,
        .baudrate = 115200
    };
    SVC_CONSOLE_Init(&con_cfg);
    SVC_CONSOLE_Printf("System started\r\n");
#endif

    /* Application code... */
    while (1) {
        /* Main loop */
    }
}
```

---

## Porting to Custom Hardware

### GPIO Mapping

Create a board-specific header:

```c
/* board.h */
#ifndef _BOARD_H_
#define _BOARD_H_

/* LED definitions */
#define LED1_PORT   GPIO_PORT_1
#define LED1_PIN    0
#define LED2_PORT   GPIO_PORT_1
#define LED2_PIN    1

/* Button definitions */
#define BTN1_PORT   GPIO_PORT_3
#define BTN1_PIN    0

/* UART pin remapping (if supported) */
#define UART_RX_PORT    GPIO_PORT_0
#define UART_RX_PIN     6
#define UART_TX_PORT    GPIO_PORT_0
#define UART_TX_PIN     7

/* SPI pin mapping */
#define SPI_SCK_PORT    GPIO_PORT_0
#define SPI_SCK_PIN     0
#define SPI_MISO_PORT   GPIO_PORT_0
#define SPI_MISO_PIN    1
#define SPI_MOSI_PORT   GPIO_PORT_0
#define SPI_MOSI_PIN    2
#define SPI_SS_PORT     GPIO_PORT_0
#define SPI_SS_PIN      3

#endif
```

### Peripheral Configuration

Create application-specific initialization:

```c
/* app_init.c */
#include "board.h"
#include "drv_gpio.h"

static gpio_handle_t g_led1, g_led2, g_btn1;

void APP_InitGPIO(void)
{
    gpio_config_t cfg;

    /* Initialize LED1 */
    cfg.port = LED1_PORT;
    cfg.pin = LED1_PIN;
    cfg.mode = GPIO_MODE_PUSH_PULL;
    DRV_GPIO_Init(&g_led1, &cfg);

    /* Initialize LED2 */
    cfg.port = LED2_PORT;
    cfg.pin = LED2_PIN;
    DRV_GPIO_Init(&g_led2, &cfg);

    /* Initialize button with pull-up and interrupt */
    cfg.port = BTN1_PORT;
    cfg.pin = BTN1_PIN;
    cfg.mode = GPIO_MODE_INPUT_ONLY;
    cfg.pull = GPIO_PULL_UP;
    cfg.int_edge = GPIO_INT_FALLING;
    cfg.callback = button_handler;
    DRV_GPIO_Init(&g_btn1, &cfg);
    DRV_GPIO_EnableInterrupt(&g_btn1);
}
```

---

## Memory Optimization

### Code Size Reduction

1. **Include only needed drivers:**
   ```c
   /* Only include what you use */
   #include "drv_gpio.h"
   #include "drv_uart.h"
   /* Don't include drv_spi.h if not using SPI */
   ```

2. **Disable unused services:**
   ```c
   #define CONSOLE_PRINTF_ENABLE   0  /* Disable printf */
   #define CONSOLE_HEXDUMP_ENABLE  0  /* Disable hex dump */
   ```

3. **Use smaller data types:**
   ```c
   uint8_t counter;   /* Use instead of int when possible */
   ```

### RAM Optimization

1. **Reduce buffer sizes:**
   ```c
   #define CONSOLE_TX_BUFFER_SIZE  32  /* Reduce from default 64 */
   #define EEPROM_PAGE_BUFFER_SIZE 64  /* Reduce from 128 */
   ```

2. **Use code memory for constants:**
   ```c
   const MS51_CODE char message[] = "Hello";
   ```

---

## Interrupt Handling

### Custom Interrupt Handlers

```c
/* Using HAL interrupt macro */
MS51_INTERRUPT(Timer0_ISR, 1)
{
    /* Timer 0 interrupt handler */
    TF0 = 0;  /* Clear flag */
    /* Your code here */
}

/* Alternative: Direct ISR (Keil) */
void Timer0_ISR(void) interrupt 1
{
    TF0 = 0;
    /* Your code here */
}
```

### Interrupt Priority Configuration

```c
void APP_ConfigInterrupts(void)
{
    /* Set Timer 0 to high priority */
    IP |= 0x02;
    IPH |= 0x02;

    /* Enable interrupts */
    ET0 = 1;  /* Timer 0 */
    EA = 1;   /* Global */
}
```

---

## Debugging Tips

### UART Debug Output

```c
/* Add debug output */
#ifdef DEBUG
    SVC_CONSOLE_Printf("Value: 0x%04X\r\n", value);
#endif
```

### Conditional Compilation

```c
/* Debug build */
#ifdef DEBUG
    #define DBG_LOG(msg)  SVC_CONSOLE_Printf(msg)
#else
    #define DBG_LOG(msg)  /* Nothing */
#endif
```

### LED Indicators

```c
/* Quick status indication */
void status_blink(uint8_t count)
{
    while (count--) {
        DRV_GPIO_Toggle(&g_led);
        SVC_DELAY_Ms(100);
    }
}
```

---

## Common Issues

### Issue: Compilation Errors with SFR Access

**Cause:** Wrong SFR header included for device.
**Solution:** Verify device selection and include correct header.

### Issue: Watchdog Reset

**Cause:** WDT enabled in CONFIG but not being kicked.
**Solution:** Either disable WDT in CONFIG or add regular WDT_Kick() calls.

### Issue: Clock Not Accurate

**Cause:** HIRC trim value not correct.
**Solution:** Use CONFIG to set proper HIRC frequency or calibrate at runtime.

### Issue: Flash Write Failure

**Cause:** TA protection sequence not correctly implemented.
**Solution:** Use HAL_TA_WRITE() macro and ensure no interrupts during sequence.

---

## See Also

- [HAL API Reference](api/hal_api.md)
- [Driver API Reference](api/driver_api.md)
- [Service API Reference](api/service_api.md)
- [Hardware Overview](hardware/ms51_overview.md)
- [CHANGELOG](../CHANGELOG.md)
