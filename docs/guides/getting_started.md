# Getting Started Guide

## 1. Prerequisites

### 1.1 Required Tools

| Tool | Version | Download |
|------|---------|----------|
| Keil C51 | 9.60+ | [Keil Website](https://www.keil.com/c51/) |
| IAR EW8051 | 10.30+ | [IAR Website](https://www.iar.com/products/architectures/iar-embedded-workbench-for-8051/) |
| SDCC | 4.0+ | [SDCC Website](http://sdcc.sourceforge.net/) |
| Nu-Link Driver | Latest | [Nuvoton Website](https://www.nuvoton.com/tool-and-software/ide-and-compiler/) |

### 1.2 Hardware Requirements

- MS51 development board (NuTiny-MS51FB9AE recommended)
- Nu-Link programmer/debugger
- USB cable
- Optional: Logic analyzer, oscilloscope

## 2. Project Structure

```
MS51_BSP/
├── hal/                    # Hardware Abstraction Layer
│   ├── include/
│   │   ├── ms51_hal.h      # Main HAL header
│   │   ├── ms51_sfr.h      # SFR definitions
│   │   └── ...
│   └── src/
├── drivers/                # Peripheral Drivers
│   ├── include/
│   └── src/
├── services/               # High-level Services
│   ├── include/
│   └── src/
├── bsp/                    # Board Support Package
│   ├── startup/
│   ├── linker/
│   └── boards/
├── samples/                # Example Applications
└── docs/                   # Documentation
```

## 3. Quick Start

### 3.1 Create a New Project (Keil)

1. **Open Keil µVision**
2. **Create New Project**
   - Project → New µVision Project
   - Select folder and name your project
   - Select Device: `Nuvoton MS51FB9AE`

3. **Add BSP Files**
   ```
   Source Group 1/
   ├── main.c                      (your application)
   ├── hal/src/ms51_hal.c
   ├── drivers/src/drv_gpio.c
   ├── drivers/src/drv_uart.c
   └── services/src/svc_console.c
   ```

4. **Configure Include Paths**
   - Options for Target → C51 → Include Paths:
   ```
   .\hal\include
   .\drivers\include
   .\services\include
   ```

5. **Configure Memory Model**
   - Options for Target → Target:
     - Memory Model: Large
     - Code ROM Size: Large
   - Options for Target → C51:
     - Use extended linker
     - Run User Programs After Build (for hex2bin if needed)

### 3.2 Minimal "Hello World"

```c
/*---------------------------------------------------------------------------
 * File: main.c
 * Description: Minimal LED blink example
 *---------------------------------------------------------------------------*/
#include "ms51_hal.h"
#include "drv_gpio.h"
#include "svc_delay.h"

/* LED connected to P1.2 on NuTiny board */
#define LED_PORT    GPIO_PORT_1
#define LED_PIN     GPIO_PIN_2

int main(void)
{
    /* 1. Initialize system clock (24MHz HIRC) */
    HAL_SYS_SetHIRC(HIRC_24MHZ);
    
    /* 2. Configure LED pin as push-pull output */
    gpio_config_t led_cfg = {
        .mode = GPIO_MODE_PUSH_PULL,
        .port = LED_PORT,
        .pin  = LED_PIN
    };
    DRV_GPIO_Init(&led_cfg);
    
    /* 3. Initialize delay service */
    SVC_Delay_Init();
    
    /* 4. Main loop - blink LED */
    while (1) {
        DRV_GPIO_Toggle(LED_PORT, LED_PIN);
        SVC_Delay_Ms(500);
    }
    
    return 0;  /* Never reached */
}
```

### 3.3 UART Printf Example

```c
/*---------------------------------------------------------------------------
 * File: main.c
 * Description: UART printf example
 *---------------------------------------------------------------------------*/
#include "ms51_hal.h"
#include "drv_uart.h"
#include "svc_console.h"

int main(void)
{
    /* 1. Initialize system clock */
    HAL_SYS_SetHIRC(HIRC_24MHZ);
    
    /* 2. Initialize console (UART0 @ 115200 baud) */
    SVC_Console_Init(115200);
    
    /* 3. Print welcome message */
    printf("\n\r=================================\n\r");
    printf("MS51 BSP - UART Printf Example\n\r");
    printf("System Clock: 24 MHz\n\r");
    printf("=================================\n\r");
    
    /* 4. Main loop */
    uint32_t counter = 0;
    while (1) {
        printf("Counter: %lu\n\r", counter++);
        SVC_Delay_Ms(1000);
    }
    
    return 0;
}
```

## 4. Build and Flash

### 4.1 Building the Project

**Keil:**
1. Press F7 or Build → Build Target
2. Check Build Output window for errors
3. Successful build creates `.hex` file in `Objects/` folder

**SDCC:**
```bash
cd samples/blink_led
make all
```

**IAR:**
1. Project → Make (F7)
2. Check build log for errors

### 4.2 Flashing the Target

**Using Nu-Link (Keil):**
1. Connect Nu-Link to target board
2. Options for Target → Debug → Use: Nu-Link Debugger
3. Flash → Download (F8)

**Using NuMicro ISP:**
1. Open NuMicro ISP Programming Tool
2. Select chip: MS51FB9AE
3. Load `.hex` file
4. Click "Program"

## 5. Debugging

### 5.1 Debug Session (Keil)

1. Connect Nu-Link programmer
2. Press Ctrl+F5 or Debug → Start/Stop Debug Session
3. Set breakpoints by clicking in the margin
4. Press F5 to run, F10 to step over, F11 to step into

### 5.2 Debug via UART

```c
/* Add debug prints for tracing */
#include "svc_console.h"

void some_function(void) {
    printf("[DEBUG] Entering some_function\n\r");
    
    /* ... your code ... */
    
    printf("[DEBUG] Value = 0x%02X\n\r", value);
    printf("[DEBUG] Exiting some_function\n\r");
}
```

### 5.3 Debug Macros

```c
/* Enable/disable debug output */
#define DEBUG_ENABLE    1

#if DEBUG_ENABLE
    #define DBG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DBG_PRINT(fmt, ...)  /* Empty */
#endif

/* Usage */
DBG_PRINT("[INFO] Timer started\n\r");
DBG_PRINT("[ERROR] Invalid parameter: %d\n\r", param);
```

## 6. Common Issues

### 6.1 Build Errors

| Error | Cause | Solution |
|-------|-------|----------|
| "Undefined symbol" | Missing source file | Add file to project |
| "Cannot open include file" | Wrong include path | Check include paths |
| "Segment too large" | Code/data overflow | Check memory model |
| "Multiple definition" | Header not guarded | Add include guards |

### 6.2 Runtime Issues

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| No LED blink | Wrong pin config | Check GPIO mode |
| UART garbled | Wrong baud rate | Verify clock/baud settings |
| Random resets | Watchdog active | Disable or feed WDT |
| Crashes in ISR | SFR page issue | Save/restore SFRS |

### 6.3 Programming Issues

| Error | Cause | Solution |
|-------|-------|----------|
| "No device found" | Connection issue | Check USB/wiring |
| "Device locked" | Security bits set | Use mass erase |
| "Verify failed" | Config mismatch | Check CONFIG bytes |

## 7. Next Steps

1. **Explore Samples**: Check `samples/` folder for peripheral examples
2. **Read Architecture**: See [Architecture Overview](../architecture/overview.md)
3. **API Reference**: See [HAL API](../api/hal_api.md) and [Driver API](../api/driver_api.md)
4. **Coding Standards**: See [Coding Standards](coding_standards.md)

## 8. Getting Help

- **Documentation**: Check `docs/` folder
- **Sample Code**: Check `samples/` folder
- **Nuvoton Forum**: [forum.nuvoton.com](https://forum.nuvoton.com)
- **Datasheet**: MS51 Series Datasheet and Reference Manual
