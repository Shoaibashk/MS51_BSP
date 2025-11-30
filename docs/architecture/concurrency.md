# Concurrency Model & Interrupt Handling

## 1. Overview

The MS51 BSP uses a **bare-metal, interrupt-driven** concurrency model. Understanding proper interrupt handling is critical for reliable firmware operation.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        MAIN LOOP (Background)                           │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │  while(1) {                                                     │   │
│  │      // Process events                                          │   │
│  │      // State machine updates                                   │   │
│  │      // Sleep when idle                                         │   │
│  │  }                                                              │   │
│  └───────────────────────────────▲─────────────────────────────────┘   │
│                                  │ Return from ISR                      │
├──────────────────────────────────┼──────────────────────────────────────┤
│  INTERRUPT SERVICE ROUTINES      │ (Foreground)                         │
│  ┌───────────┐ ┌───────────┐ ┌───┴───────┐ ┌───────────┐              │
│  │  Timer0   │ │   UART0   │ │    ADC    │ │    I2C    │  ...        │
│  │   ISR     │ │    ISR    │ │    ISR    │ │    ISR    │              │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘              │
│        ▲             ▲             ▲             ▲                     │
├────────┼─────────────┼─────────────┼─────────────┼─────────────────────┤
│        │ HARDWARE INTERRUPTS       │             │                      │
│  ┌─────┴─────────────┴─────────────┴─────────────┴───────────────────┐ │
│  │                    HARDWARE PERIPHERALS                            │ │
│  └────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────┘
```

## 2. Interrupt System Architecture

### 2.1 MS51 Interrupt Priority Levels

The MS51 has a 4-level priority scheme:

| Priority | Level | Description |
|----------|-------|-------------|
| 0 | Lowest | Can be preempted by all higher priorities |
| 1 | Low | Can be preempted by levels 2 and 3 |
| 2 | High | Can be preempted by level 3 only |
| 3 | Highest | Cannot be preempted |

### 2.2 Interrupt Vector Table

| Vector | Address | Source | Default Priority |
|--------|---------|--------|------------------|
| 0 | 0x0003 | External INT0 | 0 |
| 1 | 0x000B | Timer 0 | 0 |
| 2 | 0x0013 | External INT1 | 0 |
| 3 | 0x001B | Timer 1 | 0 |
| 4 | 0x0023 | UART0 | 0 |
| 5 | 0x002B | Timer 2 | 0 |
| 6 | 0x0033 | I2C | 0 |
| 7 | 0x003B | Pin Interrupt | 0 |
| 8 | 0x0043 | BOD | 0 |
| 9 | 0x004B | SPI | 0 |
| 10 | 0x0053 | WDT | 0 |
| 11 | 0x005B | ADC | 0 |
| 12 | 0x0063 | Capture | 0 |
| 13 | 0x006B | PWM | 0 |
| 15 | 0x007B | UART1 | 0 |
| 16 | 0x0083 | Timer 3 | 0 |
| 17 | 0x008B | WKT | 0 |

## 3. ISR Implementation Patterns

### 3.1 Standard ISR Template

```c
/*===========================================================================
 * ISR Template with SFR Page Handling
 * 
 * CRITICAL: MS51 has 2 SFR pages. ISRs must preserve and restore the page.
 *===========================================================================*/

#if defined(__C51__)
void Timer0_ISR(void) interrupt 1
#elif defined(__ICC8051__)
#pragma vector = 0x0B
__interrupt void Timer0_ISR(void)
#elif defined(__SDCC__)
void Timer0_ISR(void) __interrupt(1)
#endif
{
    /* 1. Save SFR page state (CRITICAL!) */
    uint8_t sfrs_save = SFRS;
    SFRS = 0;  /* Switch to page 0 for consistent register access */
    
    /* 2. Clear interrupt flag FIRST to allow re-triggering */
    clr_TCON_TF0;
    
    /* 3. Reload timer if needed (auto-reload mode) */
    TH0 = timer0_reload_h;
    TL0 = timer0_reload_l;
    
    /* 4. Minimal ISR work - set flag, update counter */
    timer0_tick_count++;
    timer0_flag = 1;
    
    /* 5. Invoke callback if registered */
    if (timer0_callback != NULL) {
        timer0_callback(TIMER_EVT_OVERFLOW, NULL);
    }
    
    /* 6. Restore SFR page (CRITICAL!) */
    if (sfrs_save) {
        ENABLE_SFR_PAGE1;
    }
}
```

### 3.2 ISR with Ring Buffer (UART Example)

```c
/* Shared ring buffer structure */
typedef struct {
    __xdata uint8_t buffer[64];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;
} ring_buffer_t;

__xdata ring_buffer_t uart0_rx_buf;
volatile __bit uart0_rx_ready;

#if defined(__C51__)
void UART0_ISR(void) interrupt 4
#elif defined(__ICC8051__)
#pragma vector = 0x23
__interrupt void UART0_ISR(void)
#elif defined(__SDCC__)
void UART0_ISR(void) __interrupt(4)
#endif
{
    uint8_t sfrs_save = SFRS;
    SFRS = 0;
    
    /* Handle RX interrupt */
    if (RI) {
        clr_SCON_RI;  /* Clear flag immediately */
        
        /* Store data in ring buffer if not full */
        if (uart0_rx_buf.count < sizeof(uart0_rx_buf.buffer)) {
            uart0_rx_buf.buffer[uart0_rx_buf.head] = SBUF;
            uart0_rx_buf.head = (uart0_rx_buf.head + 1) % sizeof(uart0_rx_buf.buffer);
            uart0_rx_buf.count++;
            uart0_rx_ready = 1;
        }
        /* else: buffer overflow, data lost */
    }
    
    /* Handle TX interrupt */
    if (TI) {
        clr_SCON_TI;  /* Clear flag */
        /* TX complete handling done in driver */
    }
    
    if (sfrs_save) {
        ENABLE_SFR_PAGE1;
    }
}
```

### 3.3 ISR with Callback

```c
/* Callback function pointer */
typedef void (*isr_callback_t)(uint8_t event, void *param);

__data isr_callback_t adc_callback = NULL;
volatile __bit adc_complete_flag;
volatile uint16_t adc_result;

#if defined(__C51__)
void ADC_ISR(void) interrupt 11
#elif defined(__ICC8051__)
#pragma vector = 0x5B
__interrupt void ADC_ISR(void)
#elif defined(__SDCC__)
void ADC_ISR(void) __interrupt(11)
#endif
{
    uint8_t sfrs_save = SFRS;
    SFRS = 0;
    
    clr_ADCCON0_ADCF;  /* Clear ADC flag */
    
    /* Read result (12-bit, left-aligned) */
    adc_result = ((uint16_t)ADCRH << 4) | (ADCRL >> 4);
    adc_complete_flag = 1;
    
    /* Invoke callback if registered */
    if (adc_callback != NULL) {
        adc_callback(ADC_EVT_COMPLETE, (void *)&adc_result);
    }
    
    if (sfrs_save) {
        ENABLE_SFR_PAGE1;
    }
}

/* Driver API to register callback */
void DRV_ADC_RegisterCallback(isr_callback_t callback) {
    HAL_ENTER_CRITICAL();
    adc_callback = callback;
    HAL_EXIT_CRITICAL();
}
```

## 4. Critical Sections

### 4.1 Critical Section Macros

```c
/*===========================================================================
 * Critical Section Implementation
 * 
 * Used to protect shared data between main loop and ISRs.
 * Disables global interrupts during the critical section.
 *===========================================================================*/

/* Method 1: Save and restore EA */
#define HAL_ENTER_CRITICAL()    { uint8_t _ea_save = EA; EA = 0;
#define HAL_EXIT_CRITICAL()     EA = _ea_save; }

/* Method 2: Nested-safe version with counter */
extern __data uint8_t critical_nesting;

#define HAL_ENTER_CRITICAL_NESTED() do { \
    EA = 0; \
    critical_nesting++; \
} while(0)

#define HAL_EXIT_CRITICAL_NESTED() do { \
    if (--critical_nesting == 0) { \
        EA = 1; \
    } \
} while(0)

/* Usage example */
void safe_increment(void) {
    HAL_ENTER_CRITICAL();
    shared_counter++;  /* Atomic with respect to ISRs */
    HAL_EXIT_CRITICAL();
}
```

### 4.2 When to Use Critical Sections

| Scenario | Need Critical Section? | Notes |
|----------|----------------------|-------|
| Reading single byte | No | 8051 reads are atomic |
| Writing single byte | No | 8051 writes are atomic |
| Reading 16-bit value | **YES** | May be interrupted mid-read |
| Writing 16-bit value | **YES** | May be interrupted mid-write |
| Read-modify-write | **YES** | Can cause race condition |
| Updating ring buffer pointers | **YES** | Multiple memory accesses |
| Setting/clearing flags | Depends | Single `__bit` is atomic |

### 4.3 Critical Section Examples

```c
/* Example 1: Safe 16-bit read */
uint16_t get_timer_count(void) {
    uint16_t count;
    HAL_ENTER_CRITICAL();
    count = timer_count_16bit;
    HAL_EXIT_CRITICAL();
    return count;
}

/* Example 2: Safe read-modify-write */
void set_flag_bits(uint8_t mask) {
    HAL_ENTER_CRITICAL();
    status_flags |= mask;  /* Read-modify-write */
    HAL_EXIT_CRITICAL();
}

/* Example 3: Ring buffer dequeue (main context) */
uint8_t uart_getchar(void) {
    uint8_t data;
    
    /* Wait for data */
    while (uart_rx_buf.count == 0);
    
    HAL_ENTER_CRITICAL();
    data = uart_rx_buf.buffer[uart_rx_buf.tail];
    uart_rx_buf.tail = (uart_rx_buf.tail + 1) % sizeof(uart_rx_buf.buffer);
    uart_rx_buf.count--;
    HAL_EXIT_CRITICAL();
    
    return data;
}
```

## 5. Volatile Keyword Usage

### 5.1 When to Use `volatile`

```c
/* Rule: Use volatile for ANY variable accessed by both ISR and main code */

/* Correct: ISR modifies, main reads */
volatile __bit uart_rx_ready;
volatile uint8_t timer_tick;
volatile uint16_t adc_result;

/* Correct: Hardware registers are always volatile */
sfr volatile P0 = 0x80;

/* Incorrect: Missing volatile - compiler may optimize incorrectly */
uint8_t timer_tick;  /* BUG: May be cached in register */

void main(void) {
    while (timer_tick == 0);  /* May infinite loop due to caching! */
}
```

### 5.2 Volatile with Pointers

```c
/* Pointer to volatile data */
volatile uint8_t *ptr_to_volatile;

/* Volatile pointer to data (rare) */
uint8_t * volatile volatile_ptr;

/* Volatile pointer to volatile data */
volatile uint8_t * volatile both_volatile;
```

## 6. SFR Page Handling

### 6.1 MS51 SFR Pages

The MS51 has two SFR pages (Page 0 and Page 1). Some registers only exist on Page 1.

```c
/* SFR Page selection register */
sfr SFRS = 0x91;  /* Bit 0: SFRPAGE */

/* Page 0 access (default) */
#define ENABLE_SFR_PAGE0    clr_SFRS_SFRPAGE

/* Page 1 access (TA-protected) */
#define ENABLE_SFR_PAGE1    set_SFRS_SFRPAGE
```

### 6.2 ISR SFR Page Rules

```
┌──────────────────────────────────────────────────────────────────────┐
│  ISR SFR PAGE HANDLING RULES                                         │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  1. ALWAYS save SFRS at ISR entry                                    │
│  2. Switch to known page (usually Page 0) for ISR code               │
│  3. ALWAYS restore SFRS at ISR exit                                  │
│                                                                      │
│  ISR Entry:                                                          │
│    sfrs_save = SFRS;    // Save current page                         │
│    SFRS = 0;            // Switch to page 0                          │
│                                                                      │
│  ISR Exit:                                                           │
│    if (sfrs_save) {                                                  │
│        ENABLE_SFR_PAGE1; // Restore page 1 if needed                 │
│    }                                                                 │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

## 7. Timing and Latency

### 7.1 Interrupt Latency

| Factor | Cycles | Notes |
|--------|--------|-------|
| Minimum latency | 7 | Push PC, jump to vector |
| Context save | 4-8 | Push registers (if using) |
| SFR page save | 2 | Read SFRS |
| Total minimum | ~15 | Before first ISR instruction |

### 7.2 ISR Execution Time Guidelines

| Guideline | Limit | Reason |
|-----------|-------|--------|
| Fast ISR | < 50 cycles | Minimal jitter, responsive system |
| Normal ISR | < 200 cycles | Acceptable for most applications |
| Long ISR | > 200 cycles | May cause missed interrupts |
| Very Long | > 1000 cycles | **AVOID** - use deferred processing |

### 7.3 Deferred Processing Pattern

```c
/* ISR sets flag, main loop processes */
volatile __bit process_adc_flag;
volatile uint16_t adc_raw_value;

/* Fast ISR - just capture data and set flag */
void ADC_ISR(void) interrupt 11 {
    uint8_t sfrs_save = SFRS;
    SFRS = 0;
    
    clr_ADCCON0_ADCF;
    adc_raw_value = (ADCRH << 4) | (ADCRL >> 4);
    process_adc_flag = 1;  /* Signal main loop */
    
    if (sfrs_save) ENABLE_SFR_PAGE1;
}

/* Main loop - heavy processing done here */
void main(void) {
    while (1) {
        if (process_adc_flag) {
            process_adc_flag = 0;
            
            /* Heavy processing in main context */
            filtered_value = apply_filter(adc_raw_value);
            scaled_value = apply_scaling(filtered_value);
            check_limits(scaled_value);
        }
        
        /* Other tasks... */
    }
}
```

## 8. Priority Configuration

### 8.1 Setting Interrupt Priorities

```c
/* Priority registers: IP, IPH */
/* Each interrupt has 2 bits: IPH.x and IP.x */
/* Priority = (IPH.x << 1) | IP.x = 0, 1, 2, or 3 */

/* Example: Set Timer0 to highest priority (3) */
void set_timer0_high_priority(void) {
    set_IPH_PT0H;   /* IPH.1 = 1 */
    set_IP_PT0;     /* IP.1 = 1 */
    /* Priority = 3 (highest) */
}

/* Example: Set UART0 to low priority (1) */
void set_uart0_low_priority(void) {
    clr_IPH_PSH;    /* IPH.4 = 0 */
    set_IP_PS;      /* IP.4 = 1 */
    /* Priority = 1 (low) */
}
```

### 8.2 Priority Assignment Guidelines

| Peripheral | Suggested Priority | Rationale |
|------------|-------------------|-----------|
| WDT | 3 (Highest) | System safety |
| Timer (system tick) | 2-3 | Timing critical |
| Communication (UART/SPI/I2C) | 1-2 | Data loss prevention |
| ADC | 1 | Sample timing |
| GPIO/Pin interrupts | 0-1 | User input |
| PWM | 0-1 | Usually auto-reload |

## 9. Debugging ISR Issues

### 9.1 Common ISR Bugs

| Bug | Symptom | Fix |
|-----|---------|-----|
| Missing SFR page restore | Random crashes | Always save/restore SFRS |
| Missing volatile | Stuck in loop | Add volatile to shared vars |
| Missing critical section | Corrupted data | Protect multi-byte access |
| Clearing flag too late | Double interrupt | Clear flag first in ISR |
| ISR too long | Missed interrupts | Use deferred processing |
| Recursive interrupt | Stack overflow | Disable INT in ISR if needed |

### 9.2 Debug Helpers

```c
/* ISR execution counter for debugging */
volatile uint32_t isr_exec_count[18];  /* One per interrupt source */

/* ISR timing measurement */
volatile uint16_t isr_max_cycles;

void measure_isr_time(void) {
    uint16_t start = TL0 | (TH0 << 8);
    /* ... ISR code ... */
    uint16_t elapsed = (TL0 | (TH0 << 8)) - start;
    if (elapsed > isr_max_cycles) {
        isr_max_cycles = elapsed;
    }
}
```
