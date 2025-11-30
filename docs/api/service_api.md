# MS51 Service API Reference

## Overview

The Service layer provides high-level abstractions that combine multiple drivers to implement common application patterns. Services are designed for ease of use and portability.

---

## Delay Service

### Header: `svc_delay.h`

Provides precision timing and delay functions using hardware timers.

### Types

```c
typedef struct {
    timer_id_t timer;        /* Timer to use (TIMER_0..TIMER_3) */
    uint8_t use_interrupt;   /* Use interrupt-based tick (1) or polling (0) */
} delay_config_t;
```

### Initialization

```c
drv_status_t SVC_DELAY_Init(const delay_config_t *config);
drv_status_t SVC_DELAY_DeInit(void);
```

**Parameters:**
- `config` - Configuration structure (NULL for defaults: TIMER_0, interrupt mode)

### Blocking Delays

```c
void SVC_DELAY_Ms(uint16_t ms);      /* Millisecond delay */
void SVC_DELAY_Us(uint16_t us);      /* Microsecond delay */
void SVC_DELAY_Sec(uint8_t sec);     /* Second delay */
```

### System Tick

```c
uint32_t SVC_DELAY_GetTicks(void);          /* Get tick count (ms) */
void SVC_DELAY_SetTicks(uint32_t ticks);    /* Set tick count */
uint32_t SVC_DELAY_GetTicksElapsed(uint32_t start);  /* Elapsed since start */
```

### Non-Blocking Delays

```c
typedef uint32_t delay_timer_t;

delay_timer_t SVC_DELAY_StartTimer(uint32_t timeout_ms);
uint8_t SVC_DELAY_IsExpired(delay_timer_t timer);
uint32_t SVC_DELAY_GetRemaining(delay_timer_t timer);
void SVC_DELAY_CancelTimer(delay_timer_t *timer);
```

### Example

```c
/* Initialize with defaults */
SVC_DELAY_Init(NULL);

/* Blocking delay */
SVC_DELAY_Ms(100);

/* Non-blocking timeout */
delay_timer_t timeout = SVC_DELAY_StartTimer(1000);
while (!operation_complete) {
    if (SVC_DELAY_IsExpired(timeout)) {
        /* Timeout occurred */
        break;
    }
}

/* Periodic operation */
uint32_t last_tick = SVC_DELAY_GetTicks();
while (1) {
    if (SVC_DELAY_GetTicksElapsed(last_tick) >= 500) {
        last_tick = SVC_DELAY_GetTicks();
        /* Execute every 500ms */
    }
}
```

---

## Console Service

### Header: `svc_console.h`

Provides formatted output (printf), debug logging, and input handling over UART.

### Types

```c
typedef struct {
    uart_port_t port;         /* UART port (UART_PORT_0 or UART_PORT_1) */
    uint32_t baudrate;        /* Baud rate (default: 115200) */
    uint8_t echo_enable;      /* Echo received characters */
    uint8_t newline_crlf;     /* Use CRLF for newlines */
} console_config_t;
```

### Initialization

```c
drv_status_t SVC_CONSOLE_Init(const console_config_t *config);
drv_status_t SVC_CONSOLE_DeInit(void);
```

### Output Functions

```c
void SVC_CONSOLE_PutChar(char c);
void SVC_CONSOLE_PutString(const char *str);
void SVC_CONSOLE_Printf(const char *fmt, ...);
void SVC_CONSOLE_NewLine(void);
```

**Printf Format Specifiers:**
| Specifier | Description |
|-----------|-------------|
| `%d` | Signed decimal integer |
| `%u` | Unsigned decimal integer |
| `%x` | Lowercase hex |
| `%X` | Uppercase hex |
| `%c` | Character |
| `%s` | String |
| `%%` | Literal percent sign |

### Debug Logging

```c
void SVC_CONSOLE_HexDump(const uint8_t *data, uint16_t len);
void SVC_CONSOLE_HexDumpAddr(uint16_t addr, const uint8_t *data, uint16_t len);

/* Debug macros (conditionally compiled) */
#define DBG_PRINT(fmt, ...)   /* Debug output */
#define DBG_INFO(fmt, ...)    /* Info level */
#define DBG_WARN(fmt, ...)    /* Warning level */
#define DBG_ERROR(fmt, ...)   /* Error level */
#define DBG_ASSERT(cond)      /* Assert condition */
```

### Input Functions

```c
int SVC_CONSOLE_GetChar(uint16_t timeout_ms);
uint8_t SVC_CONSOLE_GetLine(char *buffer, uint8_t max_len, uint16_t timeout_ms);
uint8_t SVC_CONSOLE_IsRxReady(void);
void SVC_CONSOLE_Flush(void);
```

### Example

```c
console_config_t cfg = {
    .port = UART_PORT_0,
    .baudrate = 115200,
    .echo_enable = 1,
    .newline_crlf = 1
};
SVC_CONSOLE_Init(&cfg);

/* Formatted output */
SVC_CONSOLE_Printf("System started at %u ms\r\n", SVC_DELAY_GetTicks());
SVC_CONSOLE_Printf("Value: 0x%04X (%d)\r\n", value, value);

/* Debug output */
DBG_INFO("Initialization complete");
DBG_WARN("Low battery: %d%%", battery_level);

/* Hex dump */
uint8_t buffer[32];
SVC_CONSOLE_HexDump(buffer, sizeof(buffer));
/* Output:
 * 00 01 02 03 04 05 06 07-08 09 0A 0B 0C 0D 0E 0F  ................
 * 10 11 12 13 14 15 16 17-18 19 1A 1B 1C 1D 1E 1F  ................
 */

/* Command input */
char cmd[32];
SVC_CONSOLE_Printf("Enter command: ");
if (SVC_CONSOLE_GetLine(cmd, sizeof(cmd), 5000)) {
    /* Process command */
}
```

---

## EEPROM Service

### Header: `svc_eeprom.h`

Provides EEPROM-like persistent storage using Data Flash with wear leveling and CRC protection.

### Types

```c
typedef struct {
    uint16_t base_address;    /* Data Flash base address */
    uint16_t size;            /* Usable size in bytes */
    uint8_t page_buffer[128]; /* Internal page buffer */
} eeprom_config_t;
```

### Initialization

```c
drv_status_t SVC_EEPROM_Init(const eeprom_config_t *config);
drv_status_t SVC_EEPROM_DeInit(void);
```

### Byte Operations

```c
drv_status_t SVC_EEPROM_ReadByte(uint16_t offset, uint8_t *data);
drv_status_t SVC_EEPROM_WriteByte(uint16_t offset, uint8_t data);
```

### Block Operations

```c
drv_status_t SVC_EEPROM_ReadBlock(uint16_t offset, uint8_t *data, uint16_t len);
drv_status_t SVC_EEPROM_WriteBlock(uint16_t offset, const uint8_t *data, uint16_t len);
```

### Structured Data

```c
drv_status_t SVC_EEPROM_ReadStruct(uint16_t offset, void *data, uint16_t size);
drv_status_t SVC_EEPROM_WriteStruct(uint16_t offset, const void *data, uint16_t size);
```

**Note:** Struct operations automatically append CRC-8 for data integrity verification.

### Utility Functions

```c
drv_status_t SVC_EEPROM_Erase(void);              /* Erase all EEPROM */
drv_status_t SVC_EEPROM_Format(void);             /* Format for fresh start */
uint8_t SVC_EEPROM_Verify(uint16_t offset, uint16_t len);  /* Verify data integrity */
uint32_t SVC_EEPROM_GetWriteCount(void);          /* Get total write cycles */
```

### Example

```c
/* Initialize EEPROM service */
eeprom_config_t cfg = {
    .base_address = 0x3800,  /* 16K device */
    .size = 512
};
SVC_EEPROM_Init(&cfg);

/* Store configuration structure */
typedef struct {
    uint16_t magic;
    uint8_t brightness;
    uint8_t volume;
    uint32_t serial_number;
} app_config_t;

app_config_t config = {
    .magic = 0xA55A,
    .brightness = 80,
    .volume = 50,
    .serial_number = 12345678
};

SVC_EEPROM_WriteStruct(0, &config, sizeof(config));

/* Load configuration */
app_config_t loaded;
if (SVC_EEPROM_ReadStruct(0, &loaded, sizeof(loaded)) == DRV_OK) {
    if (loaded.magic == 0xA55A) {
        /* Valid configuration loaded */
    }
}
```

---

## Power Service

### Header: `svc_power.h`

Provides power management, low-power modes, and wake-up control.

### Types

```c
typedef enum {
    POWER_MODE_RUN      = 0,  /* Normal operation */
    POWER_MODE_IDLE     = 1,  /* CPU halted, peripherals active */
    POWER_MODE_POWERDOWN = 2  /* Deep sleep, minimal power */
} power_mode_t;

typedef enum {
    WAKEUP_SRC_NONE    = 0x00,
    WAKEUP_SRC_INT0    = 0x01,
    WAKEUP_SRC_INT1    = 0x02,
    WAKEUP_SRC_WKT     = 0x04,
    WAKEUP_SRC_GPIO    = 0x08,
    WAKEUP_SRC_I2C     = 0x10,
    WAKEUP_SRC_UART    = 0x20,
    WAKEUP_SRC_BOD     = 0x40
} wakeup_source_t;

typedef struct {
    power_mode_t default_mode;
    wakeup_source_t wakeup_mask;
    uint8_t bod_enable;
    uint8_t wkt_enable;
    uint32_t wkt_period_ms;
} power_config_t;
```

### Initialization

```c
drv_status_t SVC_POWER_Init(const power_config_t *config);
drv_status_t SVC_POWER_DeInit(void);
```

### Power Mode Control

```c
drv_status_t SVC_POWER_EnterIdle(void);
drv_status_t SVC_POWER_EnterPowerDown(void);
power_mode_t SVC_POWER_GetMode(void);
wakeup_source_t SVC_POWER_GetWakeupSource(void);
```

### Wake-up Timer (WKT)

```c
drv_status_t SVC_POWER_WKT_Configure(uint32_t period_ms);
drv_status_t SVC_POWER_WKT_Start(void);
drv_status_t SVC_POWER_WKT_Stop(void);
```

### Brown-out Detector (BOD)

```c
drv_status_t SVC_POWER_BOD_Enable(uint8_t threshold);
drv_status_t SVC_POWER_BOD_Disable(void);
uint8_t SVC_POWER_BOD_GetStatus(void);
```

### Power-on Detection

```c
uint8_t SVC_POWER_IsPOR(void);           /* Power-on reset occurred */
uint8_t SVC_POWER_IsBOR(void);           /* Brown-out reset occurred */
void SVC_POWER_ClearPORFlag(void);
```

### Example

```c
/* Configure power management */
power_config_t cfg = {
    .default_mode = POWER_MODE_RUN,
    .wakeup_mask = WAKEUP_SRC_WKT | WAKEUP_SRC_GPIO,
    .bod_enable = 1,
    .wkt_enable = 1,
    .wkt_period_ms = 1000
};
SVC_POWER_Init(&cfg);

/* Configure WKT for periodic wake-up */
SVC_POWER_WKT_Configure(5000);  /* Wake every 5 seconds */
SVC_POWER_WKT_Start();

/* Enter low-power mode */
while (1) {
    /* Do work... */
    
    /* Enter power-down until WKT or GPIO wakes us */
    SVC_POWER_EnterPowerDown();
    
    /* Check what woke us up */
    wakeup_source_t src = SVC_POWER_GetWakeupSource();
    if (src & WAKEUP_SRC_WKT) {
        /* WKT timeout - do periodic task */
    }
    if (src & WAKEUP_SRC_GPIO) {
        /* GPIO interrupt - handle button */
    }
}
```

---

## System Service

### Header: `svc_system.h`

Provides system-level functions: reset control, clock management, watchdog, and device information.

### Types

```c
typedef enum {
    CLK_SRC_HIRC_24MHZ = 0,
    CLK_SRC_HIRC_16MHZ = 1,
    CLK_SRC_HIRC_166MHZ = 2,
    CLK_SRC_LIRC       = 3,
    CLK_SRC_ECLK       = 4
} clock_source_t;

typedef enum {
    RESET_SRC_UNKNOWN  = 0,
    RESET_SRC_POR      = 1,   /* Power-on reset */
    RESET_SRC_EXTERNAL = 2,   /* External reset pin */
    RESET_SRC_WDT      = 3,   /* Watchdog reset */
    RESET_SRC_BOD      = 4,   /* Brown-out reset */
    RESET_SRC_SOFTWARE = 5    /* Software reset */
} reset_source_t;

typedef struct {
    clock_source_t clock_source;
    uint8_t clock_divider;
    uint8_t wdt_enable;
    uint16_t wdt_timeout_ms;
} system_config_t;
```

### Initialization

```c
drv_status_t SVC_SYSTEM_Init(const system_config_t *config);
drv_status_t SVC_SYSTEM_DeInit(void);
```

### Reset Control

```c
void SVC_SYSTEM_Reset(void);           /* Software reset */
void SVC_SYSTEM_ResetToAPROM(void);    /* Reset to application area */
void SVC_SYSTEM_ResetToLDROM(void);    /* Reset to bootloader */
reset_source_t SVC_SYSTEM_GetResetSource(void);
void SVC_SYSTEM_ClearResetSource(void);
```

### Clock Management

```c
drv_status_t SVC_SYSTEM_SetClockSource(clock_source_t source);
uint32_t SVC_SYSTEM_GetClockHz(void);
drv_status_t SVC_SYSTEM_SetClockDivider(uint8_t divider);
drv_status_t SVC_SYSTEM_EnableClockOutput(uint8_t divider);
drv_status_t SVC_SYSTEM_DisableClockOutput(void);
```

### Watchdog Management

```c
drv_status_t SVC_SYSTEM_WDT_Start(uint16_t timeout_ms);
drv_status_t SVC_SYSTEM_WDT_Stop(void);
void SVC_SYSTEM_WDT_Kick(void);
uint8_t SVC_SYSTEM_WDT_IsReset(void);
```

### Device Information

```c
drv_status_t SVC_SYSTEM_GetUID(uint8_t *uid);    /* 12-byte Unique ID */
drv_status_t SVC_SYSTEM_GetUCID(uint8_t *ucid);  /* 8-byte Unique Customer ID */
uint32_t SVC_SYSTEM_GetDeviceID(void);
uint32_t SVC_SYSTEM_GetBSPVersion(void);
```

### Interrupt Management

```c
void SVC_SYSTEM_EnableInterrupts(void);
void SVC_SYSTEM_DisableInterrupts(void);
uint8_t SVC_SYSTEM_EnterCritical(void);
void SVC_SYSTEM_ExitCritical(uint8_t state);
```

### Example

```c
/* Initialize system */
system_config_t cfg = {
    .clock_source = CLK_SRC_HIRC_24MHZ,
    .clock_divider = 0,
    .wdt_enable = 1,
    .wdt_timeout_ms = 1000
};
SVC_SYSTEM_Init(&cfg);

/* Check reset source */
reset_source_t rst = SVC_SYSTEM_GetResetSource();
if (rst == RESET_SRC_WDT) {
    DBG_ERROR("Watchdog reset occurred!");
}

/* Get device info */
uint8_t uid[12];
SVC_SYSTEM_GetUID(uid);
SVC_CONSOLE_Printf("Device UID: ");
SVC_CONSOLE_HexDump(uid, 12);

SVC_CONSOLE_Printf("BSP Version: 0x%08lX\r\n", SVC_SYSTEM_GetBSPVersion());
SVC_CONSOLE_Printf("Clock: %lu Hz\r\n", SVC_SYSTEM_GetClockHz());

/* Main loop with watchdog */
while (1) {
    /* Application code... */
    SVC_SYSTEM_WDT_Kick();
}
```

---

## Service Dependencies

| Service | Required Drivers | Optional Drivers |
|---------|-----------------|------------------|
| Delay | Timer | - |
| Console | UART | - |
| EEPROM | Flash | - |
| Power | - | WDT |
| System | WDT | Flash (for UID/UCID) |

---

## Initialization Order

For proper operation, initialize services in this order:

1. `SVC_SYSTEM_Init()` - Clock and basic system setup
2. `SVC_DELAY_Init()` - Timing infrastructure
3. `SVC_CONSOLE_Init()` - Debug output
4. `SVC_EEPROM_Init()` - Persistent storage
5. `SVC_POWER_Init()` - Power management

---

## See Also

- [HAL API Reference](hal_api.md)
- [Driver API Reference](driver_api.md)
- [MS51 Hardware Overview](../hardware/ms51_overview.md)
