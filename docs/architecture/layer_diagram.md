# Layer Diagram & Module Responsibilities

## 1. Detailed Layer Diagram

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                              APPLICATION LAYER                                 ║
║  ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐                  ║
║  │   main.c   │ │ app_task.c │ │ app_fsm.c  │ │ app_cfg.h  │                  ║
║  └─────┬──────┘ └─────┬──────┘ └─────┬──────┘ └────────────┘                  ║
╠════════╪══════════════╪══════════════╪════════════════════════════════════════╣
║        │              │              │                                         ║
║        ▼              ▼              ▼           SERVICE LAYER                 ║
║  ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐   ║
║  │ svc_delay  │ │svc_console │ │ svc_eeprom │ │ svc_power  │ │ svc_system │   ║
║  │            │ │            │ │            │ │            │ │            │   ║
║  │ Timer_Delay│ │ printf     │ │ EE_Read    │ │ PowerDown  │ │ SoftReset  │   ║
║  │ Timer_Tick │ │ scanf      │ │ EE_Write   │ │ Idle       │ │ WDT_Kick   │   ║
║  └─────┬──────┘ └─────┬──────┘ └─────┬──────┘ └─────┬──────┘ └─────┬──────┘   ║
╠════════╪══════════════╪══════════════╪══════════════╪══════════════╪══════════╣
║        │              │              │              │              │           ║
║        ▼              ▼              ▼              ▼              ▼           ║
║                              DRIVER LAYER                                      ║
║  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐      ║
║  │drv_gpio │ │drv_uart │ │ drv_spi │ │ drv_i2c │ │drv_timer│ │ drv_pwm │      ║
║  ├─────────┤ ├─────────┤ ├─────────┤ ├─────────┤ ├─────────┤ ├─────────┤      ║
║  │GPIO_Init│ │UART_Init│ │SPI_Init │ │I2C_Init │ │TMR_Init │ │PWM_Init │      ║
║  │GPIO_Set │ │UART_Send│ │SPI_Xfer │ │I2C_Xfer │ │TMR_Start│ │PWM_Start│      ║
║  │GPIO_Get │ │UART_Recv│ │SPI_Poll │ │I2C_Poll │ │TMR_Stop │ │PWM_SetDC│      ║
║  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘      ║
║       │          │          │          │          │          │               ║
║  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐                  ║
║  │ drv_adc │ │ drv_wdt │ │ drv_wkt │ │ drv_bod │ │ drv_iap │                  ║
║  ├─────────┤ ├─────────┤ ├─────────┤ ├─────────┤ ├─────────┤                  ║
║  │ADC_Init │ │WDT_Init │ │WKT_Init │ │BOD_Init │ │IAP_Read │                  ║
║  │ADC_Read │ │WDT_Kick │ │WKT_Start│ │BOD_Cfg  │ │IAP_Write│                  ║
║  │ADC_Trig │ │WDT_Stop │ │WKT_Stop │ │BOD_ISR  │ │IAP_Erase│                  ║
║  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘                  ║
╠═══════╪══════════╪══════════╪══════════╪══════════╪═══════════════════════════╣
║       │          │          │          │          │                            ║
║       ▼          ▼          ▼          ▼          ▼            HAL LAYER       ║
║  ┌──────────────────────────────────────────────────────────────────────────┐ ║
║  │                           ms51_hal.h                                     │ ║
║  ├──────────────────────────────────────────────────────────────────────────┤ ║
║  │  ms51_sfr.h    │ ms51_sfr_bits.h │ ms51_compiler.h │ ms51_types.h        │ ║
║  ├────────────────┴─────────────────┴─────────────────┴─────────────────────┤ ║
║  │  • SFR Addresses        • set_XXX / clr_XXX      • __data, __code       │ ║
║  │  • Bit Definitions      • TA-Protected Access    • interrupt syntax     │ ║
║  │  • Register Maps        • Atomic Operations      • Inline assembly      │ ║
║  └──────────────────────────────────────────────────────────────────────────┘ ║
╠═════════════════════════════════════════════════════════════════════════════════╣
║                              BSP LAYER                                          ║
║  ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐                   ║
║  │  startup   │ │   linker   │ │   boards   │ │   clock    │                   ║
║  ├────────────┤ ├────────────┤ ├────────────┤ ├────────────┤                   ║
║  │ Reset_Hndl │ │ CODE sect  │ │ Board pins │ │ HIRC cfg   │                   ║
║  │ Stack init │ │ DATA sect  │ │ LED/Button │ │ LIRC cfg   │                   ║
║  │ RAM init   │ │ XDATA sect │ │ UART pins  │ │ ECLK cfg   │                   ║
║  └────────────┘ └────────────┘ └────────────┘ └────────────┘                   ║
╠═════════════════════════════════════════════════════════════════════════════════╣
║                              HARDWARE                                           ║
║  ┌──────────────────────────────────────────────────────────────────────────┐  ║
║  │         MS51 MCU (8051 Core @ 24MHz, Flash, SRAM, Peripherals)           │  ║
║  └──────────────────────────────────────────────────────────────────────────┘  ║
╚═════════════════════════════════════════════════════════════════════════════════╝
```

## 2. Module Interface Contracts

### 2.1 HAL Layer Interface

```c
/*==========================================================================
 * HAL Layer provides:
 *   - SFR register access (read/write)
 *   - Bit manipulation (atomic set/clear)
 *   - TA-protected register access
 *   - Compiler abstraction macros
 *==========================================================================*/

/* SFR Access */
#define HAL_SFR_READ(reg)           (reg)
#define HAL_SFR_WRITE(reg, val)     ((reg) = (val))

/* Bit Manipulation */
#define HAL_BIT_SET(reg, bit)       ((reg) |= (1U << (bit)))
#define HAL_BIT_CLR(reg, bit)       ((reg) &= ~(1U << (bit)))
#define HAL_BIT_GET(reg, bit)       (((reg) >> (bit)) & 1U)
#define HAL_BIT_TOGGLE(reg, bit)    ((reg) ^= (1U << (bit)))

/* Critical Section */
#define HAL_ENTER_CRITICAL()        { uint8_t _ea = EA; EA = 0;
#define HAL_EXIT_CRITICAL()         EA = _ea; }

/* TA-Protected Access */
#define HAL_TA_UNLOCK()             { TA = 0xAA; TA = 0x55; }
```

### 2.2 Driver Layer Interface

```c
/*==========================================================================
 * Driver Layer provides:
 *   - Peripheral initialization
 *   - Data transfer (polled/interrupt)
 *   - Status/error reporting
 *   - Callback registration
 *==========================================================================*/

/* Standard return codes */
typedef enum {
    DRV_OK            = 0,   /* Success */
    DRV_ERROR         = -1,  /* General error */
    DRV_BUSY          = -2,  /* Peripheral busy */
    DRV_TIMEOUT       = -3,  /* Operation timeout */
    DRV_INVALID_PARAM = -4,  /* Invalid parameter */
    DRV_NOT_INIT      = -5   /* Not initialized */
} drv_status_t;

/* Standard callback prototype */
typedef void (*drv_callback_t)(uint8_t event, void *param);

/* Standard API pattern */
drv_status_t DRV_XXX_Init(xxx_handle_t *handle, const xxx_config_t *config);
drv_status_t DRV_XXX_DeInit(xxx_handle_t *handle);
drv_status_t DRV_XXX_Start(xxx_handle_t *handle);
drv_status_t DRV_XXX_Stop(xxx_handle_t *handle);
drv_status_t DRV_XXX_RegisterCallback(xxx_handle_t *handle, drv_callback_t cb);
```

### 2.3 Service Layer Interface

```c
/*==========================================================================
 * Service Layer provides:
 *   - High-level abstractions
 *   - Composite operations
 *   - Application-friendly APIs
 *==========================================================================*/

/* Example: Delay Service */
void SVC_Delay_Init(void);
void SVC_Delay_Ms(uint16_t ms);
void SVC_Delay_Us(uint16_t us);
uint32_t SVC_Delay_GetTick(void);

/* Example: Console Service */
void SVC_Console_Init(uint32_t baudrate);
void SVC_Console_PutChar(char c);
char SVC_Console_GetChar(void);
int SVC_Console_Printf(const char *fmt, ...);
```

## 3. Data Flow Examples

### 3.1 UART Transmit Flow

```
Application                Service                  Driver                   HAL
    │                         │                       │                       │
    │ printf("Hello")         │                       │                       │
    │────────────────────────►│                       │                       │
    │                         │ SVC_Console_PutChar() │                       │
    │                         │──────────────────────►│                       │
    │                         │                       │ DRV_UART_Write()      │
    │                         │                       │──────────────────────►│
    │                         │                       │                       │ SBUF = data
    │                         │                       │                       │ Wait TI
    │                         │                       │◄──────────────────────│
    │                         │◄──────────────────────│ DRV_OK                │
    │◄────────────────────────│                       │                       │
    │                         │                       │                       │
```

### 3.2 Timer Interrupt Flow

```
Hardware                     HAL                    Driver                 Application
    │                         │                       │                       │
    │ Timer Overflow          │                       │                       │
    │────────────────────────►│                       │                       │
    │                         │ ISR Vector Jump       │                       │
    │                         │──────────────────────►│                       │
    │                         │                       │ Clear TF flag         │
    │                         │                       │ Reload timer          │
    │                         │                       │                       │
    │                         │                       │ Invoke callback       │
    │                         │                       │──────────────────────►│
    │                         │                       │                       │ User handler
    │                         │                       │◄──────────────────────│
    │                         │◄──────────────────────│ Return from ISR       │
    │◄────────────────────────│                       │                       │
    │                         │                       │                       │
```

## 4. File Organization

```
MS51_BSP/
├── docs/                          # Documentation (this folder)
│   ├── architecture/
│   ├── api/
│   ├── guides/
│   └── samples/
│
├── bsp/                           # Board Support Package
│   ├── startup/
│   │   ├── keil/
│   │   ├── iar/
│   │   └── sdcc/
│   ├── linker/
│   │   ├── keil/
│   │   ├── iar/
│   │   └── sdcc/
│   └── boards/
│       ├── nutiny_ms51fb9ae/
│       └── custom_board/
│
├── hal/                           # Hardware Abstraction Layer
│   ├── include/
│   │   ├── ms51_hal.h
│   │   ├── ms51_sfr_8k.h
│   │   ├── ms51_sfr_16k.h
│   │   ├── ms51_sfr_32k.h
│   │   ├── ms51_sfr_bits.h
│   │   ├── ms51_compiler.h
│   │   └── ms51_types.h
│   └── src/
│       └── ms51_hal.c
│
├── drivers/                       # Peripheral Drivers
│   ├── include/
│   │   ├── drv_common.h
│   │   ├── drv_gpio.h
│   │   ├── drv_uart.h
│   │   ├── drv_spi.h
│   │   ├── drv_i2c.h
│   │   ├── drv_timer.h
│   │   ├── drv_pwm.h
│   │   ├── drv_adc.h
│   │   ├── drv_wdt.h
│   │   ├── drv_wkt.h
│   │   ├── drv_bod.h
│   │   └── drv_iap.h
│   └── src/
│       ├── drv_gpio.c
│       ├── drv_uart.c
│       └── ...
│
├── services/                      # Service Layer
│   ├── include/
│   │   ├── svc_delay.h
│   │   ├── svc_console.h
│   │   ├── svc_eeprom.h
│   │   ├── svc_power.h
│   │   └── svc_system.h
│   └── src/
│       ├── svc_delay.c
│       ├── svc_console.c
│       └── ...
│
├── samples/                       # Sample Applications
│   ├── blink_led/
│   ├── uart_echo/
│   ├── adc_read/
│   └── ...
│
├── tests/                         # Unit Tests
│   ├── unit/
│   ├── integration/
│   └── mocks/
│
├── tools/                         # Build Tools & Scripts
│   ├── cmake/
│   └── scripts/
│
├── CHANGELOG.md
├── LICENSE.txt
└── README.md
```
