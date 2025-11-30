# Safety & Non-Functional Checklist

## 1. Code Safety Checklist

### 1.1 Memory Safety

| Check | Status | Notes |
|-------|--------|-------|
| All pointers validated before use | ☐ | Check for NULL |
| Array bounds checked | ☐ | Use sizeof(), define bounds |
| Stack overflow protection | ☐ | Canary value at stack bottom |
| No dynamic memory (malloc/free) | ☐ | Static allocation only |
| Buffer sizes defined as constants | ☐ | #define BUFFER_SIZE |
| Ring buffer wrap-around correct | ☐ | Modulo operation |

### 1.2 Integer Safety

| Check | Status | Notes |
|-------|--------|-------|
| Fixed-width types used (uint8_t) | ☐ | No plain int/char |
| Overflow checked for arithmetic | ☐ | Especially counters |
| Signed/unsigned mixing avoided | ☐ | Explicit casts if needed |
| Division by zero prevented | ☐ | Check divisor |
| Shift amount within range | ☐ | 0 to (width-1) |

### 1.3 Concurrency Safety

| Check | Status | Notes |
|-------|--------|-------|
| Shared variables marked volatile | ☐ | ISR/main communication |
| Critical sections for multi-byte | ☐ | 16/32-bit variables |
| SFR page saved/restored in ISR | ☐ | All ISRs |
| ISR flags cleared promptly | ☐ | First action in ISR |
| No blocking in ISR | ☐ | Set flag, exit quickly |
| Reentrant functions identified | ☐ | Use local variables |

### 1.4 Error Handling

| Check | Status | Notes |
|-------|--------|-------|
| All return values checked | ☐ | Don't ignore errors |
| Error codes defined | ☐ | Enum with meanings |
| Graceful degradation on error | ☐ | Don't just crash |
| Debug output for errors | ☐ | #ifdef DEBUG |
| Assertions in debug builds | ☐ | ASSERT() macro |

## 2. Watchdog Checklist

### 2.1 WDT Configuration

```c
/*---------------------------------------------------------------------------
 * Watchdog Timer Guidelines
 *---------------------------------------------------------------------------*/

/* Recommended WDT timeout selection */
#define WDT_TIMEOUT_16MS    0x00    /* Very short - only for tight loops */
#define WDT_TIMEOUT_64MS    0x01    /* Short */
#define WDT_TIMEOUT_256MS   0x02    /* Medium */
#define WDT_TIMEOUT_1024MS  0x03    /* Standard (recommended) */
#define WDT_TIMEOUT_4096MS  0x04    /* Long */
#define WDT_TIMEOUT_16384MS 0x05    /* Very long */

/* WDT refresh pattern */
void main(void)
{
    WDT_Init(WDT_TIMEOUT_1024MS);
    
    while (1) {
        /* Refresh at start of main loop */
        WDT_Refresh();
        
        /* Application tasks - must complete within timeout */
        Task1_Run();
        Task2_Run();
        Task3_Run();
        
        /* If any task hangs, WDT will reset */
    }
}
```

### 2.2 WDT Checklist

| Check | Status | Notes |
|-------|--------|-------|
| WDT enabled in production | ☐ | Safety net for lockups |
| Timeout appropriate for loop time | ☐ | 2x max loop time |
| WDT refresh in main loop only | ☐ | Never in ISR |
| WDT disabled during flash write | ☐ | IAP operations |
| WDT reset reason logged | ☐ | For debugging |
| WDT tested by intentional lockup | ☐ | Verify recovery |

## 3. Brown-Out Detector (BOD) Checklist

### 3.1 BOD Configuration

```c
/*---------------------------------------------------------------------------
 * BOD Threshold Selection
 *---------------------------------------------------------------------------*/

/* Choose based on minimum operating voltage */
#define BOD_THRESHOLD_1V8   0x00    /* 1.8V - very low power */
#define BOD_THRESHOLD_2V0   0x01    /* 2.0V */
#define BOD_THRESHOLD_2V4   0x02    /* 2.4V */
#define BOD_THRESHOLD_2V7   0x03    /* 2.7V - standard 3.3V systems */
#define BOD_THRESHOLD_3V0   0x04    /* 3.0V */
#define BOD_THRESHOLD_3V7   0x05    /* 3.7V - 5V systems */
#define BOD_THRESHOLD_4V4   0x06    /* 4.4V */

/* Recommended: Set BOD 0.3-0.5V below nominal VDD */
/* For 3.3V system: Use 2.7V threshold */
/* For 5.0V system: Use 4.4V threshold */
```

### 3.2 BOD Checklist

| Check | Status | Notes |
|-------|--------|-------|
| BOD enabled | ☐ | Prevent undervoltage operation |
| Threshold appropriate for VDD | ☐ | ~0.4V below nominal |
| BOD reset vs interrupt decided | ☐ | Reset recommended |
| Power supply stability verified | ☐ | Check for droops |
| BOD in power-down mode | ☐ | Trade-off: power vs safety |

## 4. Power Management Checklist

### 4.1 Power Modes

| Mode | Current | Wake Sources | Use Case |
|------|---------|--------------|----------|
| Normal | ~5mA | N/A | Active processing |
| Idle | ~2mA | Any interrupt | Wait for event |
| Power-Down | ~1µA | WKT, Pin INT, I2C | Long sleep |

### 4.2 Power-Down Entry Checklist

```c
/*---------------------------------------------------------------------------
 * Safe Power-Down Entry Sequence
 *---------------------------------------------------------------------------*/
void enter_power_down(void)
{
    /* 1. Disable non-essential peripherals */
    DRV_ADC_DeInit();
    DRV_PWM_Stop();
    
    /* 2. Configure wake-up source */
    WKT_Start(WAKEUP_INTERVAL_MS);
    /* or: Configure pin interrupt for button wake */
    
    /* 3. Set GPIO to low-power state */
    configure_gpio_for_sleep();
    
    /* 4. Disable BOD if ultra-low power needed (CAUTION!) */
    #if DISABLE_BOD_IN_SLEEP
    BOD_Disable();
    #endif
    
    /* 5. Enter power-down */
    set_PCON_PD;
    
    /* --- Device is now sleeping --- */
    
    /* 6. Execution resumes here after wake-up */
    
    /* 7. Re-enable BOD immediately */
    #if DISABLE_BOD_IN_SLEEP
    BOD_Enable();
    #endif
    
    /* 8. Re-initialize peripherals */
    reinit_peripherals();
}
```

### 4.3 Power Checklist

| Check | Status | Notes |
|-------|--------|-------|
| Unused pins set to known state | ☐ | Output low or input w/pullup |
| Peripherals disabled when not used | ☐ | Save power |
| Power-down used when idle | ☐ | Battery applications |
| Wake-up sources configured | ☐ | WKT, pin, I2C address match |
| Current measured in each mode | ☐ | Verify against datasheet |
| No floating inputs | ☐ | Can cause oscillation |

## 5. Security Considerations

### 5.1 Flash Protection

| Check | Status | Notes |
|-------|--------|-------|
| APROM lock bit set in production | ☐ | Prevent read-back |
| LDROM protected if used | ☐ | Bootloader security |
| SPROM used for sensitive code | ☐ | Highest protection |
| CONFIG locked in production | ☐ | Prevent modification |

### 5.2 Secure Coding

| Check | Status | Notes |
|-------|--------|-------|
| No secrets in APROM (use SPROM) | ☐ | Keys, passwords |
| Input validation on all interfaces | ☐ | UART, I2C commands |
| Buffer overflow prevention | ☐ | Bounds checking |
| No debug interface in production | ☐ | Remove UART debug |

## 6. OTA/Bootloader Considerations

### 6.1 Bootloader Architecture

```
┌─────────────────────────────────────────────────────┐
│                    FLASH LAYOUT                      │
├─────────────────────────────────────────────────────┤
│  0x0000 - 0x2FFF: APROM (Application)               │
│  0x3000 - 0x3FFF: Data Flash (Config/EEPROM)        │
├─────────────────────────────────────────────────────┤
│  LDROM (Bootloader) - Separate region               │
│  - ISP code                                         │
│  - Update validation                                │
│  - Jump to APROM                                    │
└─────────────────────────────────────────────────────┘
```

### 6.2 OTA Update Checklist

| Check | Status | Notes |
|-------|--------|-------|
| Bootloader in LDROM | ☐ | Protected from app bugs |
| Firmware version stored | ☐ | Check before update |
| CRC/checksum validation | ☐ | Verify image integrity |
| Rollback capability | ☐ | If update fails |
| Communication security | ☐ | Encryption if sensitive |
| Power loss tolerance | ☐ | Dual-bank or recovery |

## 7. Non-Functional Requirements Checklist

### 7.1 Performance

| Requirement | Target | Actual | Status |
|-------------|--------|--------|--------|
| Main loop time | < 10ms | | ☐ |
| ISR latency | < 50 cycles | | ☐ |
| UART throughput | 115200 baud | | ☐ |
| ADC sample rate | 100 ksps | | ☐ |
| PWM frequency | 20 kHz | | ☐ |

### 7.2 Resource Usage

| Resource | Limit | Actual | Status |
|----------|-------|--------|--------|
| APROM (code) | 16KB | | ☐ |
| IRAM (data) | 256B | | ☐ |
| XRAM (xdata) | 1KB | | ☐ |
| Stack usage | 64B | | ☐ |

### 7.3 Reliability

| Requirement | Target | Test Method | Status |
|-------------|--------|-------------|--------|
| MTBF | > 10,000 hrs | Calculation | ☐ |
| Operating temp | -40 to +85°C | Chamber test | ☐ |
| ESD immunity | ±4kV contact | ESD gun test | ☐ |
| Power cycle | > 100,000 | Automated test | ☐ |
| Flash endurance | > 100,000 writes | Stress test | ☐ |

## 8. Code Review Checklist

### 8.1 Pre-Commit Checks

- [ ] Code compiles without warnings (all compilers)
- [ ] Unit tests pass
- [ ] Static analysis clean (cppcheck, lint)
- [ ] Coding standards followed
- [ ] Comments updated
- [ ] Version number incremented
- [ ] CHANGELOG updated

### 8.2 Review Focus Areas

- [ ] Error handling complete
- [ ] Boundary conditions handled
- [ ] Thread safety (ISR/main)
- [ ] Resource cleanup on error
- [ ] No magic numbers
- [ ] Functions have single responsibility
- [ ] Complexity reasonable (cyclomatic < 10)
