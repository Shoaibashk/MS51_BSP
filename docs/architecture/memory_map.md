# Memory Map & Linker Configuration

## 1. MS51 Memory Architecture

### 1.1 Memory Overview

The MS51 series uses the classic 8051 memory architecture with enhancements:

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                           CODE MEMORY (PROGRAM)                              │
├──────────────────────────────────────────────────────────────────────────────┤
│  APROM (Application ROM)     │ Main application code                        │
│  LDROM (Loader ROM)          │ Bootloader/ISP code (optional)               │
│  SPROM (Secure ROM)          │ Secure/protected code                        │
│  CONFIG (Configuration)      │ Device configuration bytes                   │
└──────────────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────────────┐
│                           DATA MEMORY                                        │
├──────────────────────────────────────────────────────────────────────────────┤
│  IRAM (Internal RAM)         │ 256 bytes, fast access                        │
│  XRAM (External RAM)         │ 768-2048 bytes, slower access                 │
│  SFR (Special Function Regs) │ Peripheral control registers                  │
└──────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Device Variant Memory Sizes

| Device | APROM | LDROM | SPROM | IRAM | XRAM | Data Flash |
|--------|-------|-------|-------|------|------|------------|
| MS51xA9AE (8K) | 8KB | 2KB | 256B | 256B | 768B | 4KB |
| MS51xB9xE (16K) | 16KB | 4KB | 256B | 256B | 1KB | 4KB |
| MS51xC0xE (16K) | 16KB | 4KB | 256B | 256B | 1KB | 4KB |
| MS51xC0AE (32K) | 32KB | 4KB | 256B | 256B | 2KB | 4KB |

## 2. Detailed Memory Maps

### 2.1 Code Memory Map (16KB Variant)

```
        APROM                    LDROM                   CONFIG
┌────────────────┐        ┌────────────────┐        ┌────────────────┐
│ 0x3FFF         │        │ 0x3FFF         │        │ 0x30004        │
│                │        │                │        │   CONFIG4      │
│   Application  │        │   Bootloader   │        ├────────────────┤
│      Code      │        │     Code       │        │ 0x30003        │
│                │        │                │        │   CONFIG3      │
│   (16384 bytes)│        │   (4096 bytes) │        ├────────────────┤
│                │        │                │        │ 0x30002        │
│                │        │                │        │   CONFIG2      │
│                │        │                │        ├────────────────┤
│                │        │                │        │ 0x30001        │
│                │        │                │        │   CONFIG1      │
│ Reset Vector   │        │                │        ├────────────────┤
│ 0x0000         │        │ 0x0000         │        │ 0x30000        │
└────────────────┘        └────────────────┘        │   CONFIG0      │
                                                    └────────────────┘
```

### 2.2 APROM Layout (Typical Application)

```
0x3FFF ┌────────────────────────────────────────┐
       │                                        │
       │           Data Flash Area              │
       │         (EEPROM Emulation)             │ 
       │            4KB (128 pages)             │
       │                                        │
0x3000 ├────────────────────────────────────────┤
       │                                        │
       │                                        │
       │           Application Code             │
       │                                        │
       │                                        │
0x0100 ├────────────────────────────────────────┤
       │        Interrupt Vector Table          │
       │                                        │
0x0003 ├────────────────────────────────────────┤
       │          Reset Vector (LJMP)           │
0x0000 └────────────────────────────────────────┘
```

### 2.3 Internal RAM (IRAM) Map

```
0xFF ┌────────────────────────────────────────┐
     │                                        │
     │              STACK                     │  ▲
     │           (grows down)                 │  │
     │                                        │  │
     ├────────────────────────────────────────┤ ◄── SP (Stack Pointer)
     │                                        │
     │       Indirect Addressable Only        │
     │          (IDATA: 0x80-0xFF)            │
     │                                        │
0x80 ├────────────────────────────────────────┤
     │                                        │
     │    Direct & Indirect Addressable       │
     │          (DATA: 0x00-0x7F)             │
     │                                        │
     │  0x00-0x07: Register Bank 0 (R0-R7)    │
     │  0x08-0x0F: Register Bank 1            │
     │  0x10-0x17: Register Bank 2            │
     │  0x18-0x1F: Register Bank 3            │
     │  0x20-0x2F: Bit-addressable area       │
     │  0x30-0x7F: General purpose            │
     │                                        │
0x00 └────────────────────────────────────────┘
```

### 2.4 External RAM (XRAM) Map

```
0x03FF ┌────────────────────────────────────────┐ (1KB variant)
       │                                        │
       │            Application XRAM            │
       │           (Buffers, Arrays)            │
       │                                        │
0x0000 └────────────────────────────────────────┘
```

### 2.5 SFR Map

```
0xFF ┌────────────────────────────────────────┐
     │                                        │
     │    Special Function Registers          │
     │    (Direct access only: 0x80-0xFF)     │
     │                                        │
     │  0x80: P0     0x90: P1     0xA0: P2    │
     │  0x98: SCON   0xC0: I2CON  0xC8: T2CON │
     │  0xD0: PSW    0xE0: ACC    0xF0: B     │
     │                                        │
     │  Page 0 / Page 1 (via SFRS register)   │
     │                                        │
0x80 └────────────────────────────────────────┘
```

## 3. Linker Configuration

### 3.1 Keil C51 Linker Settings

```
; Keil LX51 Linker Control File for MS51FB9AE (16KB)

; Code memory segments
CODE    (0x0000-0x3FFF)     ; APROM: 16KB
CONST   (0x0000-0x3FFF)     ; Constants in code space

; Internal RAM segments  
DATA    (0x0020-0x007F)     ; Direct addressable (skip register banks & bit area)
IDATA   (0x0080-0x00FF)     ; Indirect addressable

; Stack configuration
STACK   SIZE(64)            ; 64 bytes stack
STACKPTR(0x00FF)            ; Stack starts at top of IRAM

; External RAM
XDATA   (0x0000-0x03FF)     ; XRAM: 1KB
```

### 3.2 IAR EW8051 Linker Configuration

```
//------------------------------------------------------
// IAR Linker Configuration for MS51FB9AE (16KB)
//------------------------------------------------------

define memory MEM with size = 64K;

// Code regions
define region CODE_region = MEM:[from 0x0000 to 0x3FFF];

// Data regions
define region DATA_region = MEM:[from 0x0020 to 0x007F];
define region IDATA_region = MEM:[from 0x0080 to 0x00FF];
define region XDATA_region = MEM:[from 0x0000 to 0x03FF];
define region BIT_region = MEM:[from 0x0020 to 0x002F];

// Stack
define block CSTACK with size = 64, alignment = 1 { };

// Placements
place in CODE_region { readonly };
place in DATA_region { readwrite };
place at end of IDATA_region { block CSTACK };
place in XDATA_region { section XDATA };
```

### 3.3 SDCC Linker Configuration

```makefile
# SDCC Memory Configuration for MS51FB9AE (16KB)

# Code space
CODE_SIZE = 16384    # 16KB APROM

# Internal RAM
IRAM_SIZE = 256      # 256 bytes
DATA_START = 0x20    # After register banks
DATA_SIZE = 96       # 0x20 to 0x7F

# Stack
STACK_SIZE = 64
STACK_LOC = 0xFF     # Top of IRAM, grows down

# External RAM
XRAM_SIZE = 1024     # 1KB
XRAM_START = 0x0000

# SDCC command line options
LDFLAGS = --code-size $(CODE_SIZE) \
          --iram-size $(IRAM_SIZE) \
          --xram-size $(XRAM_SIZE) \
          --xram-loc $(XRAM_START) \
          --stack-loc $(STACK_LOC) \
          --stack-size $(STACK_SIZE)
```

## 4. Memory Placement Guidelines

### 4.1 Variable Placement Strategy

| Variable Type | Memory Region | Keyword | Use Case |
|---------------|---------------|---------|----------|
| Fast access | DATA (0x00-0x7F) | `__data` | Loop counters, flags |
| General purpose | IDATA (0x80-0xFF) | `__idata` | Local variables |
| Large buffers | XDATA | `__xdata` | Rx/Tx buffers, arrays |
| Constants | CODE | `__code` | Lookup tables, strings |
| Bit variables | BIT area | `__bit` | Boolean flags |

### 4.2 Code Examples

```c
/* Fast access variables (DATA region) */
__data uint8_t tick_counter;
__data uint8_t state_flags;

/* General variables (IDATA region) */
__idata uint8_t temp_buffer[16];

/* Large buffers (XDATA region) */
__xdata uint8_t uart_rx_buffer[128];
__xdata uint8_t uart_tx_buffer[128];

/* Constants (CODE region) */
__code const uint8_t sin_table[256] = { /* ... */ };
__code const char version_string[] = "v1.0.0";

/* Bit variables (BIT region) */
__bit flag_uart_rx_ready;
__bit flag_timer_expired;
```

### 4.3 Interrupt Vector Table

```c
/* Interrupt Vector Addresses */
#define INT_VECTOR_RESET        0x0000  /* Reset */
#define INT_VECTOR_EXT0         0x0003  /* External Interrupt 0 */
#define INT_VECTOR_TIMER0       0x000B  /* Timer 0 Overflow */
#define INT_VECTOR_EXT1         0x0013  /* External Interrupt 1 */
#define INT_VECTOR_TIMER1       0x001B  /* Timer 1 Overflow */
#define INT_VECTOR_UART0        0x0023  /* UART0 */
#define INT_VECTOR_TIMER2       0x002B  /* Timer 2 */
#define INT_VECTOR_I2C          0x0033  /* I2C */
#define INT_VECTOR_PIN_INT      0x003B  /* Pin Interrupt */
#define INT_VECTOR_BOD          0x0043  /* Brown-out Detect */
#define INT_VECTOR_SPI          0x004B  /* SPI */
#define INT_VECTOR_WDT          0x0053  /* Watchdog Timer */
#define INT_VECTOR_ADC          0x005B  /* ADC */
#define INT_VECTOR_CAPTURE      0x0063  /* Input Capture */
#define INT_VECTOR_PWM          0x006B  /* PWM */
#define INT_VECTOR_UART1        0x007B  /* UART1 */
#define INT_VECTOR_TIMER3       0x0083  /* Timer 3 */
#define INT_VECTOR_WKT          0x008B  /* Wake-up Timer */
```

## 5. Stack Considerations

### 5.1 Stack Usage Guidelines

- **Minimum stack:** 32 bytes (simple applications)
- **Typical stack:** 64 bytes (moderate nesting)
- **Maximum recommended:** 128 bytes (deep call trees)

### 5.2 Stack Overflow Detection

```c
/* Stack canary for overflow detection */
#define STACK_CANARY_VALUE  0xDEAD
#define STACK_CANARY_ADDR   0x80    /* Bottom of usable IDATA */

void Stack_Init(void) {
    *((uint16_t __idata *)STACK_CANARY_ADDR) = STACK_CANARY_VALUE;
}

bool Stack_CheckOverflow(void) {
    return (*((uint16_t __idata *)STACK_CANARY_ADDR) != STACK_CANARY_VALUE);
}
```

## 6. Data Flash (EEPROM) Area

### 6.1 Data Flash Organization

The top 4KB of APROM can be used as data flash for EEPROM emulation:

```
0x3FFF ┌────────────────────────────────┐
       │   Page 31 (128 bytes)          │
       ├────────────────────────────────┤
       │   Page 30                      │
       ├────────────────────────────────┤
       │          ...                   │
       ├────────────────────────────────┤
       │   Page 1                       │
       ├────────────────────────────────┤
       │   Page 0 (128 bytes)           │
0x3000 └────────────────────────────────┘

Total: 32 pages × 128 bytes = 4096 bytes
```

### 6.2 Data Flash Access

```c
/* Data flash base address (16KB variant) */
#define DATAFLASH_BASE      0x3000
#define DATAFLASH_SIZE      0x1000  /* 4KB */
#define DATAFLASH_PAGE_SIZE 128

/* Calculate page address */
#define DATAFLASH_PAGE_ADDR(page) (DATAFLASH_BASE + ((page) * DATAFLASH_PAGE_SIZE))
```
