# MS51 MCU Hardware Overview

## Device Family

The MS51 is a family of 8051-based microcontrollers from Nuvoton Technology, optimized for cost-sensitive embedded applications with enhanced peripherals.

---

## Device Variants

### 8KB Flash Devices

| Device | APROM | LDROM | Data Flash | XRAM | Pins | Package |
|--------|-------|-------|------------|------|------|---------|
| MS51DA9AE | 8 KB | 2 KB | 2 KB | 768 B | 16 | TSSOP-16 |
| MS51BA9AE | 8 KB | 2 KB | 2 KB | 768 B | 20 | TSSOP-20 |

### 16KB Flash Devices

| Device | APROM | LDROM | Data Flash | XRAM | Pins | Package |
|--------|-------|-------|------------|------|------|---------|
| MS51FB9AE | 16 KB | 4 KB | 4 KB | 1 KB | 32 | LQFP-32 |
| MS51XB9AE | 16 KB | 4 KB | 4 KB | 1 KB | 20 | TSSOP-20 |
| MS51XB9BE | 16 KB | 4 KB | 4 KB | 1 KB | 16 | TSSOP-16 |

### 32KB Flash Devices

| Device | APROM | LDROM | Data Flash | XRAM | Pins | Package |
|--------|-------|-------|------------|------|------|---------|
| MS51FC0AE | 32 KB | 4 KB | 4 KB | 2 KB | 32 | LQFP-32 |
| MS51XC0BE | 32 KB | 4 KB | 4 KB | 2 KB | 16 | TSSOP-16 |
| MS51EB0AE | 32 KB | 4 KB | 4 KB | 2 KB | 48 | LQFP-48 |
| MS51EC0AE | 32 KB | 4 KB | 4 KB | 2 KB | 64 | LQFP-64 |
| MS51TC0AE | 32 KB | 4 KB | 4 KB | 2 KB | 24 | TSSOP-24 |
| MS51PC0AE | 32 KB | 4 KB | 4 KB | 2 KB | 20 | TSSOP-20 |

---

## Core Features

### CPU
- Enhanced 8051 core, 1T (single clock cycle) instruction execution
- Maximum clock frequency: 24 MHz
- 256 bytes internal SRAM
- Up to 2 KB external SRAM (XRAM)

### Clock System
- **HIRC**: High-speed internal RC oscillator (24/16/16.6 MHz, ±1% accuracy)
- **LIRC**: Low-speed internal RC oscillator (10 kHz, for WKT and WDT)
- **ECLK**: External clock input (up to 24 MHz)
- Programmable clock divider (1:1 to 1:128)
- Clock output capability on dedicated pin

### Memory
- Flash program memory (APROM) with 1000 cycle endurance
- Separate loader memory (LDROM) for ISP bootloader
- Data Flash (EEPROM emulation) with 10,000 cycle endurance
- In-Application Programming (IAP) support
- Security Protection ROM (SPROM) for sensitive code

---

## Peripheral Overview

### GPIO
- Up to 26 I/O pins (device dependent)
- 4 operation modes per pin:
  - Quasi-bidirectional (default)
  - Push-pull output
  - Input only (high impedance)
  - Open-drain
- Optional internal pull-up resistors
- Schmitt trigger input
- Pin interrupt capability on all ports

### Timers
| Timer | Resolution | Features |
|-------|------------|----------|
| Timer 0 | 16-bit | Mode 0/1/2/3, external clock |
| Timer 1 | 16-bit | Mode 0/1/2, UART baud rate |
| Timer 2 | 16-bit | Auto-reload, capture, external clock |
| Timer 3 | 16-bit | Auto-reload, 16-bit resolution |
| WKT | 22-bit | Wake-up timer, LIRC clock |
| WDT | 17-bit | Watchdog, reset/interrupt |

### PWM
- 6 PWM channels (PWM0-5)
- 16-bit resolution
- Independent or complementary mode
- Center-aligned or edge-aligned
- Programmable dead-time (complementary mode)
- Fault brake input
- PWM trigger for ADC

**32KB devices additional features:**
- PWM1, PWM2, PWM3 additional modules
- Enhanced phase control

### UART
| Port | Features |
|------|----------|
| UART0 | Full-duplex, Timer-based baud rate |
| UART1 | Full-duplex, dedicated baud rate generator |

- Up to 115200 baud at 24 MHz
- 8/9-bit data, optional parity
- Multi-processor communication mode
- Wake-up from power-down

### SPI
- Full-duplex master/slave
- 4 clock phase/polarity modes
- MSB or LSB first
- Up to 6 MHz clock (master mode)
- Slave select control

### I²C
- Master and slave modes
- Standard mode (100 kHz)
- Fast mode (400 kHz)
- 7-bit addressing
- Timeout detection
- Wake-up from power-down

### ADC
- 12-bit SAR ADC
- Up to 8 external channels
- Internal bandgap reference (1.22V)
- VDD/4 measurement channel
- Hardware compare function
- Multiple trigger sources:
  - Software trigger
  - PWM trigger
  - GPIO trigger

### Comparator (32KB devices)
- 2 comparators with programmable hysteresis
- External and internal reference inputs
- Digital filter
- Wake-up capability

---

## Power Management

### Operating Modes

| Mode | CPU | Peripherals | Oscillators | Wake-up Time |
|------|-----|-------------|-------------|--------------|
| Normal | Run | Active | All | - |
| Idle | Stop | Active | All | Immediate |
| Power-Down | Stop | Stop | LIRC only | ~6 μs |

### Power-Down Wake-up Sources
- INT0/INT1 external interrupts
- GPIO pin interrupt
- Wake-up Timer (WKT)
- I²C slave address match
- UART RX activity
- Brown-out detector

### Brown-out Detector (BOD)
- Programmable threshold: 2.0V, 2.4V, 3.7V, 4.4V
- Reset or interrupt on low voltage
- Filter to reduce noise sensitivity

---

## Security Features

### Code Protection
- CONFIG lock bits for read protection
- Security lock prevents code readout
- SPROM for secure code storage

### Device ID
- 12-byte Unique Device ID (UID)
- 8-byte Unique Customer ID (UCID)
- Device ID for identification

---

## Memory Map

### Code Memory

```
0x0000 +----------------+
       |     APROM      |
       |  (8/16/32 KB)  |
0x3000 +----------------+ (8KB device)
0x3800 +----------------+ (16KB device)
0x7000 +----------------+ (32KB device)
       |   Data Flash   |
       |   (2/4/4 KB)   |
       +----------------+
       
LDROM: Separate address space (2KB or 4KB)
```

### SFR Memory (0x80-0xFF)

```
0x80 +----------------+
     |  Standard 8051 |
     |     SFRs       |
0xA0 +----------------+
     |  Extended SFRs |
     |  (Peripherals) |
0xFF +----------------+
```

### SFR Paging (32KB devices)

Page 0: Standard SFRs
Page 1: Extended SFRs (PWM, ADC extended)
Page 2: Advanced SFRs (PWM1-3, SmartCard, PIPS)

---

## Electrical Characteristics

### Operating Conditions
| Parameter | Min | Typ | Max | Unit |
|-----------|-----|-----|-----|------|
| VDD (3.3V range) | 2.4 | 3.3 | 5.5 | V |
| VDD (5V range) | 4.5 | 5.0 | 5.5 | V |
| Temperature | -40 | 25 | 85 | °C |

### Current Consumption (VDD = 3.3V, 24 MHz)
| Mode | Typical | Unit |
|------|---------|------|
| Normal (all peripherals) | 4.5 | mA |
| Idle | 1.8 | mA |
| Power-Down | 1.0 | μA |

### I/O Characteristics
| Parameter | Value | Unit |
|-----------|-------|------|
| Output drive (push-pull) | 20 | mA |
| Input leakage | ±1 | μA |
| Pull-up resistance | 30-70 | kΩ |

---

## Package Pinouts

### Common Pin Functions

| Pin | Function | Alternate Functions |
|-----|----------|---------------------|
| VDD | Power supply | - |
| VSS | Ground | - |
| RST | Reset input | - |
| P0.0-P0.7 | GPIO Port 0 | Various peripherals |
| P1.0-P1.7 | GPIO Port 1 | Various peripherals |
| P2.0-P2.7 | GPIO Port 2 | Various peripherals |
| P3.0-P3.7 | GPIO Port 3 | Various peripherals |

---

## Development Tools

### Supported Compilers
- Keil C51 9.60 or later
- IAR Embedded Workbench for 8051 10.30 or later
- SDCC 4.0 or later (open source)

### Programming/Debug
- Nu-Link programmer/debugger
- ICP (In-Circuit Programming) via ISP pins
- ISP (In-System Programming) via UART/I²C

### Evaluation Boards
- NuTiny-SDK-MS51 series

---

## See Also

- [Peripheral Notes](peripheral_notes.md)
- [HAL API Reference](../api/hal_api.md)
- [Porting Guide](../porting_guide.md)
