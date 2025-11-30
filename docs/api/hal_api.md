# MS51 HAL API Reference

## Overview

The Hardware Abstraction Layer (HAL) provides low-level access to MS51 hardware resources with compiler-independent abstractions.

## Version Information

```c
#define MS51_HAL_VERSION_MAJOR  2
#define MS51_HAL_VERSION_MINOR  0
#define MS51_HAL_VERSION_PATCH  0
#define MS51_HAL_VERSION        0x02000000UL
```

---

## Compiler Abstraction

### Header: `ms51_compiler.h`

#### Compiler Detection Macros

| Macro | Description |
|-------|-------------|
| `MS51_COMPILER_KEIL` | Defined when using Keil C51 |
| `MS51_COMPILER_IAR` | Defined when using IAR EW8051 |
| `MS51_COMPILER_SDCC` | Defined when using SDCC |

#### Memory Qualifiers

| Macro | Description |
|-------|-------------|
| `MS51_CODE` | Code memory (ROM) |
| `MS51_DATA` | Internal data memory (directly addressable) |
| `MS51_IDATA` | Internal data memory (indirectly addressable) |
| `MS51_XDATA` | External data memory |
| `MS51_BIT` | Bit-addressable memory |
| `MS51_SFR` | Special Function Register |
| `MS51_SBIT` | SFR bit |

#### Function Qualifiers

| Macro | Usage |
|-------|-------|
| `MS51_INTERRUPT(name, vector)` | Interrupt handler declaration |
| `MS51_USING(bank)` | Register bank specification |
| `MS51_REENTRANT` | Reentrant function modifier |
| `MS51_CRITICAL` | Critical section function modifier |

#### Utility Macros

| Macro | Description |
|-------|-------------|
| `MS51_NOP()` | Single-cycle NOP instruction |
| `MS51_ENABLE_INTERRUPTS()` | Enable global interrupts (EA=1) |
| `MS51_DISABLE_INTERRUPTS()` | Disable global interrupts (EA=0) |

---

## Type Definitions

### Header: `ms51_types.h`

#### Fixed-Width Integer Types

| Type | Size | Range |
|------|------|-------|
| `uint8_t` | 8 bits | 0 to 255 |
| `int8_t` | 8 bits | -128 to 127 |
| `uint16_t` | 16 bits | 0 to 65535 |
| `int16_t` | 16 bits | -32768 to 32767 |
| `uint32_t` | 32 bits | 0 to 4294967295 |
| `int32_t` | 32 bits | -2147483648 to 2147483647 |
| `bool_t` | 8 bits | 0 or 1 |

#### Boolean Constants

```c
#define TRUE   1
#define FALSE  0
#define NULL   ((void *)0)
```

#### Bit Manipulation Macros

```c
BIT_SET(reg, bit)      // Set bit in register
BIT_CLR(reg, bit)      // Clear bit in register
BIT_TGL(reg, bit)      // Toggle bit in register
BIT_CHK(reg, bit)      // Check if bit is set
BIT_VAL(bit)           // Convert bit position to value (1 << bit)
BIT_MASK(bits)         // Create mask of 'bits' consecutive bits
```

#### Byte Manipulation

```c
LOBYTE(word)           // Extract low byte from 16-bit value
HIBYTE(word)           // Extract high byte from 16-bit value
MAKEWORD(hi, lo)       // Combine two bytes into 16-bit value
MIN(a, b)              // Return minimum of two values
MAX(a, b)              // Return maximum of two values
CLAMP(x, min, max)     // Clamp value to range
```

---

## Driver Status Codes

### Enumeration: `drv_status_t`

| Status | Value | Description |
|--------|-------|-------------|
| `DRV_OK` | 0 | Operation completed successfully |
| `DRV_ERROR` | -1 | General error occurred |
| `DRV_BUSY` | -2 | Resource is busy |
| `DRV_TIMEOUT` | -3 | Operation timed out |
| `DRV_INVALID_PARAM` | -4 | Invalid parameter provided |
| `DRV_NOT_INIT` | -5 | Driver not initialized |
| `DRV_ALREADY_INIT` | -6 | Driver already initialized |
| `DRV_NOT_SUPPORTED` | -7 | Feature not supported |

### Error Checking Macros

```c
DRV_CHECK_PTR(ptr)     // Return DRV_INVALID_PARAM if ptr is NULL
DRV_CHECK_INIT(flag)   // Return DRV_NOT_INIT if flag is 0
DRV_CHECK_STATUS(s)    // Return status if not DRV_OK
```

---

## SFR Bit Definitions

### Header: `ms51_sfr_bits.h`

Provides symbolic names for all SFR bit positions:

```c
/* Example: P0 bits */
#define P00   0
#define P01   1
#define P02   2
/* ... */

/* Example: TCON bits */
#define IT0   0    // INT0 trigger type
#define IE0   1    // INT0 flag
#define IT1   2    // INT1 trigger type
#define IE1   3    // INT1 flag
#define TR0   4    // Timer 0 run
#define TF0   5    // Timer 0 overflow flag
#define TR1   6    // Timer 1 run
#define TF1   7    // Timer 1 overflow flag
```

---

## SFR Access

### Time Access (TA) Protection

Some sensitive SFRs require a Time Access unlock sequence:

```c
HAL_TA_WRITE(sfr, value)  // Write to TA-protected register
```

**Usage:**
```c
/* Modify CKSWT requires TA unlock */
HAL_TA_WRITE(CKSWT, 0x00);  // Switch to HIRC

/* Trigger software reset */
HAL_TA_WRITE(CHPCON, CHPCON | 0x80);
```

### Protected Registers

| Register | Function |
|----------|----------|
| `CHPCON` | Chip control (software reset, boot area) |
| `CKSWT` | Clock switch control |
| `CKEN` | Clock enable control |
| `CKDIV` | Clock divider |
| `WDCON` | Watchdog control |
| `BODCON0/1` | Brown-out detector control |

---

## Clock Configuration

### System Clock Constants

```c
#define HIRC_24MHZ    24000000UL
#define HIRC_16MHZ    16000000UL
#define HIRC_166MHZ   16600000UL
#define LIRC_FREQ     10000UL
```

### HAL Clock Functions

```c
void HAL_SYS_SetHIRC(uint32_t freq);
void HAL_SYS_EnableLIRC(void);
void HAL_SYS_DisableLIRC(void);
void HAL_SYS_SetClockDivider(uint8_t div);
```

**Example:**
```c
/* Set HIRC to 16 MHz */
HAL_SYS_SetHIRC(HIRC_16MHZ);

/* Enable LIRC for WKT */
HAL_SYS_EnableLIRC();

/* Divide clock by 2 */
HAL_SYS_SetClockDivider(1);  /* FSYS = FOSC / 2 */
```

---

## Reset Control

```c
void HAL_SYS_Reset(void);           // Software reset
void HAL_SYS_ResetToAPROM(void);    // Reset and boot from APROM
void HAL_SYS_ResetToLDROM(void);    // Reset and boot from LDROM
```

---

## Critical Section

```c
HAL_ENTER_CRITICAL()   // Disable interrupts, save state
HAL_EXIT_CRITICAL()    // Restore interrupt state
```

**Usage:**
```c
uint8_t saved_ea;
HAL_ENTER_CRITICAL();
/* Critical section code - interrupts disabled */
HAL_EXIT_CRITICAL();
```

---

## Delay Functions

```c
void HAL_DELAY_US(uint16_t us);    // Microsecond delay (software)
void HAL_DELAY_MS(uint16_t ms);    // Millisecond delay (software)
```

**Note:** These are software delay loops. For accurate timing, use the Timer driver or Delay service.

---

## Device Variants

### 8K Devices (ms51_sfr_8k.h)
- MS51DA9AE, MS51BA9AE
- 8 KB APROM, 768 bytes XRAM
- 2 KB LDROM, 2 KB Data Flash

### 16K Devices (ms51_sfr_16k.h)
- MS51FB9AE, MS51XB9AE, MS51XB9BE
- 16 KB APROM, 1 KB XRAM
- 4 KB LDROM, 4 KB Data Flash

### 32K Devices (ms51_sfr_32k.h)
- MS51FC0AE, MS51XC0BE, MS51EB0AE, MS51EC0AE, MS51TC0AE, MS51PC0AE
- 32 KB APROM, 2 KB XRAM
- 4 KB LDROM, 4 KB Data Flash
- Extended features: PWM1-3, Smart Card, PIPS

---

## Include Order

```c
/* HAL Layer Includes */
#include "ms51_hal.h"        /* Main HAL header - includes all below */

/* Or individual includes */
#include "ms51_compiler.h"   /* Compiler abstraction */
#include "ms51_types.h"      /* Type definitions */
#include "ms51_sfr_16k.h"    /* SFR definitions for target device */
#include "ms51_sfr_bits.h"   /* Bit position definitions */
```

---

## See Also

- [Driver API Reference](driver_api.md)
- [Service API Reference](service_api.md)
- [MS51 Hardware Overview](../hardware/ms51_overview.md)
- [Porting Guide](../porting_guide.md)
