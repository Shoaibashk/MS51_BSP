# Board Support Package (BSP)

This folder contains board-specific and device-specific support files for the MS51 microcontroller family.

## Structure

```
bsp/
├── startup/           # Startup code for different compilers
│   ├── keil/         # Keil C51 startup files
│   ├── iar/          # IAR startup files
│   └── sdcc/         # SDCC startup files
├── linker/           # Linker scripts
│   ├── keil/         # Keil linker configuration
│   ├── iar/          # IAR linker configuration (.icf)
│   └── sdcc/         # SDCC linker scripts
├── device/           # Device-specific configurations
│   ├── ms51_8k.h     # 8KB device family config
│   ├── ms51_16k.h    # 16KB device family config
│   └── ms51_32k.h    # 32KB device family config
└── boards/           # Board-specific configurations
    └── generic/      # Generic development board
```

## Device Families

| Family | Flash Size | RAM Size | Devices |
|--------|------------|----------|---------|
| 8KB    | 8 KB       | 256 B    | MS51DA9AE, MS51BA9AE |
| 16KB   | 16 KB      | 1 KB     | MS51FB9AE, MS51XB9AE, MS51XB9BE |
| 32KB   | 32 KB      | 1 KB     | MS51FC0AE, MS51XC0BE, MS51EB0AE, MS51EC0AE, MS51TC0AE, MS51PC0AE |

## Usage

1. Select your target device by defining the appropriate macro:
   - `MS51_8K` for 8KB devices
   - `MS51_16K` for 16KB devices  
   - `MS51_32K` for 32KB devices

2. Include the appropriate startup file for your compiler

3. Configure the linker script for your device's memory layout

## Compiler Support

- **Keil C51**: MDK-ARM compatible startup and project files
- **IAR Embedded Workbench**: IAR-specific startup and linker files
- **SDCC**: Open-source Small Device C Compiler support
