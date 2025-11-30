# MS51 BSP Documentation

Welcome to the MS51 Board Support Package documentation. This guide provides comprehensive information for developing applications with the MS51 microcontroller family.

## 📚 Documentation Index

### Getting Started

- [Getting Started Guide](guides/getting_started.md) — First project setup
- [Architecture Overview](architecture/overview.md) — Understanding the BSP layers

### Architecture

| Document | Description |
|----------|-------------|
| [Overview](architecture/overview.md) | High-level system architecture |
| [Layer Diagram](architecture/layer_diagram.md) | BSP layer responsibilities |
| [Memory Map](architecture/memory_map.md) | Memory layout and sections |
| [Concurrency](architecture/concurrency.md) | ISR handling and critical sections |

### API Reference

| Document | Description |
|----------|-------------|
| [HAL API](api/hal_api.md) | Hardware Abstraction Layer functions |
| [Driver API](api/driver_api.md) | Peripheral driver functions |
| [Service API](api/service_api.md) | High-level service functions |

### Developer Guides

| Document | Description |
|----------|-------------|
| [Coding Standards](guides/coding_standards.md) | Code style and conventions |
| [Porting Guide](porting_guide.md) | Porting to new devices/compilers |
| [Testing Guide](guides/testing_guide.md) | Unit and integration testing |
| [Safety Checklist](guides/safety_checklist.md) | MISRA and safety guidelines |

### Hardware Reference

| Document | Description |
|----------|-------------|
| [MS51 Overview](hardware/ms51_overview.md) | MS51 family overview |
| [Peripheral Notes](hardware/peripheral_notes.md) | Peripheral-specific information |

## 🎯 Supported Devices

| Family | Flash | RAM | Devices |
|--------|-------|-----|---------|
| 8KB    | 8 KB  | 256 B | MS51DA9AE, MS51BA9AE |
| 16KB   | 16 KB | 1 KB  | MS51FB9AE, MS51XB9AE, MS51XB9BE |
| 32KB   | 32 KB | 1 KB  | MS51FC0AE, MS51XC0BE, MS51EB0AE, MS51EC0AE, MS51TC0AE, MS51PC0AE |

## 🔧 Supported Toolchains

| Toolchain | Version | Status |
|-----------|---------|--------|
| Keil C51 | 9.60+ | ✅ Fully Supported |
| IAR EW8051 | 10.30+ | ✅ Fully Supported |
| SDCC | 4.0+ | ✅ Fully Supported |

## 📁 Project Structure

```
MS51_BSP/
├── hal/                    # Hardware Abstraction Layer
│   ├── include/           # HAL headers
│   └── src/               # HAL implementation
├── drivers/               # Peripheral Drivers
│   ├── include/           # Driver headers
│   └── src/               # Driver implementations
├── services/              # High-Level Services
│   ├── include/           # Service headers
│   └── src/               # Service implementations
├── bsp/                   # Board Support Package
│   ├── startup/           # Startup code (Keil, IAR, SDCC)
│   ├── linker/            # Linker scripts
│   ├── device/            # Device configurations
│   └── boards/            # Board definitions
├── samples/               # Example applications
└── docs/                  # This documentation
```

## 📄 License

Apache License 2.0 - See [LICENSE.txt](../LICENSE.txt)

## 📝 Version History

See [CHANGELOG.md](../CHANGELOG.md) for detailed version history.
