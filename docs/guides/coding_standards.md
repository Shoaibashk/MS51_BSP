# Embedded C Coding Standards

## 1. General Principles

1. **Readability** over cleverness
2. **Consistency** throughout the codebase
3. **Safety** in all constructs
4. **Portability** across compilers
5. **Testability** by design

## 2. File Organization

### 2.1 Header File Template

```c
/*===========================================================================
 * @file    drv_uart.h
 * @brief   UART Driver Interface
 * @version 1.0.0
 * @date    2024-01-01
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Your Company
 *===========================================================================*/

#ifndef DRV_UART_H
#define DRV_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "ms51_hal.h"
#include "drv_common.h"

/*---------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------*/
#define UART_PORT_0         0
#define UART_PORT_1         1

/*---------------------------------------------------------------------------
 * Type Definitions
 *---------------------------------------------------------------------------*/
typedef struct {
    uint32_t baudrate;
    uint8_t  data_bits;
    uint8_t  parity;
    uint8_t  stop_bits;
} uart_config_t;

typedef struct {
    uint8_t port;
    /* ... internal state ... */
} uart_handle_t;

/*---------------------------------------------------------------------------
 * Function Prototypes
 *---------------------------------------------------------------------------*/
drv_status_t DRV_UART_Init(uart_handle_t *handle, uint8_t port, 
                           const uart_config_t *config);
drv_status_t DRV_UART_DeInit(uart_handle_t *handle);
drv_status_t DRV_UART_Write(uart_handle_t *handle, const uint8_t *data, 
                            uint16_t len);
drv_status_t DRV_UART_Read(uart_handle_t *handle, uint8_t *data, 
                           uint16_t len, uint16_t *actual);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */
```

### 2.2 Source File Template

```c
/*===========================================================================
 * @file    drv_uart.c
 * @brief   UART Driver Implementation
 * @version 1.0.0
 * @date    2024-01-01
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Your Company
 *===========================================================================*/

/*---------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "drv_uart.h"

/*---------------------------------------------------------------------------
 * Private Macros
 *---------------------------------------------------------------------------*/
#define UART_TX_TIMEOUT_MS  100

/*---------------------------------------------------------------------------
 * Private Types
 *---------------------------------------------------------------------------*/
typedef struct {
    uint8_t state;
    /* ... */
} uart_internal_t;

/*---------------------------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------------------------*/
static uart_internal_t uart_state[2];

/*---------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
static void uart_set_baudrate(uint8_t port, uint32_t baudrate);

/*---------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * @brief  Initialize UART peripheral
 * @param  handle  Pointer to UART handle
 * @param  port    UART port (UART_PORT_0 or UART_PORT_1)
 * @param  config  Pointer to configuration structure
 * @return DRV_OK on success, error code otherwise
 */
drv_status_t DRV_UART_Init(uart_handle_t *handle, uint8_t port,
                           const uart_config_t *config)
{
    /* Parameter validation */
    if (handle == NULL || config == NULL) {
        return DRV_INVALID_PARAM;
    }
    if (port > UART_PORT_1) {
        return DRV_INVALID_PARAM;
    }
    
    /* Implementation... */
    
    return DRV_OK;
}

/*---------------------------------------------------------------------------
 * Private Functions
 *---------------------------------------------------------------------------*/

static void uart_set_baudrate(uint8_t port, uint32_t baudrate)
{
    /* Implementation... */
}
```

## 3. Naming Conventions

### 3.1 General Rules

| Element | Convention | Example |
|---------|------------|---------|
| Files | lowercase_with_underscores | `drv_uart.c` |
| Headers | lowercase_with_underscores | `drv_uart.h` |
| Functions | PascalCase with prefix | `DRV_UART_Init` |
| Local functions | snake_case with static | `static void set_baud()` |
| Variables | snake_case | `uint8_t rx_count` |
| Constants | UPPER_SNAKE_CASE | `#define MAX_BUFFER 64` |
| Types | snake_case with _t suffix | `uart_config_t` |
| Enums | UPPER_SNAKE_CASE | `UART_PARITY_NONE` |

### 3.2 Prefix Conventions

| Prefix | Layer | Example |
|--------|-------|---------|
| `HAL_` | HAL layer | `HAL_SFR_READ()` |
| `DRV_` | Driver layer | `DRV_GPIO_Init()` |
| `SVC_` | Service layer | `SVC_Delay_Ms()` |
| `APP_` | Application | `APP_Task_Run()` |
| `ISR_` | Interrupt handlers | `ISR_Timer0_Handler()` |

### 3.3 Variable Naming

```c
/* Good: Descriptive names */
uint8_t  rx_buffer_count;
uint16_t adc_sample_value;
uint32_t system_tick_count;

/* Bad: Unclear abbreviations */
uint8_t  rbc;
uint16_t asv;
uint32_t stc;

/* Prefix for scope */
static uint8_t s_module_state;      /* static/module-level */
       uint8_t g_global_counter;    /* global (use sparingly!) */

/* Pointer naming */
uint8_t *ptr_buffer;                /* Pointer to buffer */
const uint8_t *ptr_const_data;      /* Pointer to constant */
```

## 4. Code Style

### 4.1 Indentation and Braces

```c
/* Use 4 spaces for indentation (no tabs) */
/* Braces on same line for control structures */
/* Braces on new line for functions */

void Example_Function(uint8_t param)
{
    if (param > 0) {
        /* Do something */
        for (uint8_t i = 0; i < param; i++) {
            process_item(i);
        }
    } else {
        /* Handle zero case */
        handle_zero();
    }
    
    switch (param) {
        case 0:
            action_zero();
            break;
        case 1:
            action_one();
            break;
        default:
            action_default();
            break;
    }
}
```

### 4.2 Line Length and Wrapping

```c
/* Maximum line length: 80-100 characters */
/* Wrap long function calls */

drv_status_t result = DRV_UART_Init(
    &uart_handle,
    UART_PORT_0,
    &uart_config
);

/* Wrap long conditions */
if ((condition_one == TRUE) &&
    (condition_two == TRUE) &&
    (condition_three == TRUE)) {
    /* ... */
}

/* Wrap long macro definitions */
#define COMPLEX_MACRO(a, b, c) \
    do { \
        operation_one(a); \
        operation_two(b); \
        operation_three(c); \
    } while (0)
```

### 4.3 Spacing

```c
/* Space after keywords */
if (condition)
while (running)
for (i = 0; i < 10; i++)
switch (value)

/* No space after function name */
function_call(arg1, arg2);

/* Space around binary operators */
result = a + b;
if (x == 0 && y > 10)

/* No space for unary operators */
value++;
!flag
~mask

/* Space after comma */
function(arg1, arg2, arg3);
```

## 5. Safety Practices

### 5.1 Integer Types

```c
/* Always use fixed-width types */
#include "ms51_types.h"

uint8_t   byte_value;      /* 0 to 255 */
int8_t    signed_byte;     /* -128 to 127 */
uint16_t  word_value;      /* 0 to 65535 */
int16_t   signed_word;     /* -32768 to 32767 */
uint32_t  dword_value;     /* 0 to 4294967295 */
int32_t   signed_dword;    /* -2147483648 to 2147483647 */

/* Avoid plain int, char, long - size varies by compiler */
```

### 5.2 Pointer Safety

```c
/* Always initialize pointers */
uint8_t *ptr = NULL;

/* Check pointers before use */
if (ptr != NULL) {
    *ptr = value;
}

/* Use const for read-only data */
void process_data(const uint8_t *data, uint16_t len);

/* Use volatile for hardware registers and shared variables */
volatile uint8_t *hw_register = (volatile uint8_t *)0xE0;
```

### 5.3 Array Bounds

```c
/* Always check array indices */
#define BUFFER_SIZE 64

uint8_t buffer[BUFFER_SIZE];
uint8_t index = 0;

/* Safe array access */
if (index < BUFFER_SIZE) {
    buffer[index] = value;
    index++;
}

/* Use sizeof for compile-time size */
for (uint8_t i = 0; i < sizeof(buffer); i++) {
    buffer[i] = 0;
}
```

### 5.4 Assertions

```c
/* Use assertions for development-time checks */
#include "assert.h"

#define ASSERT(expr)  do { if (!(expr)) { ASSERT_Handler(__FILE__, __LINE__); } } while(0)

void ASSERT_Handler(const char *file, int line)
{
    /* Log error and halt */
    printf("ASSERT FAILED: %s:%d\n", file, line);
    while (1);  /* Halt for debugging */
}

/* Usage */
void process_buffer(uint8_t *buf, uint16_t len)
{
    ASSERT(buf != NULL);
    ASSERT(len > 0 && len <= MAX_BUFFER_SIZE);
    /* ... */
}
```

### 5.5 Magic Numbers

```c
/* Bad: Magic numbers */
if (status == 0x03) {
    delay(1000);
}

/* Good: Named constants */
#define STATUS_READY        0x03
#define DELAY_1_SECOND_MS   1000

if (status == STATUS_READY) {
    delay(DELAY_1_SECOND_MS);
}
```

## 6. Compiler Portability

### 6.1 Compiler Abstraction

```c
/*---------------------------------------------------------------------------
 * Compiler-specific keywords abstraction
 *---------------------------------------------------------------------------*/
#if defined(__C51__)          /* Keil C51 */
    #define DATA_ATTR   data
    #define IDATA_ATTR  idata
    #define XDATA_ATTR  xdata
    #define CODE_ATTR   code
    #define BIT_ATTR    bit
    #define INTERRUPT(n) interrupt n
    
#elif defined(__ICC8051__)    /* IAR EW8051 */
    #define DATA_ATTR   __data
    #define IDATA_ATTR  __idata
    #define XDATA_ATTR  __xdata
    #define CODE_ATTR   __code
    #define BIT_ATTR    __bit
    #define INTERRUPT(n) /* Use #pragma vector */
    
#elif defined(__SDCC__)       /* SDCC */
    #define DATA_ATTR   __data
    #define IDATA_ATTR  __idata
    #define XDATA_ATTR  __xdata
    #define CODE_ATTR   __code
    #define BIT_ATTR    __bit
    #define INTERRUPT(n) __interrupt(n)
    
#else
    #error "Unsupported compiler"
#endif
```

### 6.2 ISR Declaration

```c
/*---------------------------------------------------------------------------
 * Portable ISR declaration macros
 *---------------------------------------------------------------------------*/
#if defined(__C51__)
    #define ISR_HANDLER(name, vector)  void name(void) interrupt vector
    
#elif defined(__ICC8051__)
    #define ISR_HANDLER(name, vector)  \
        _Pragma("vector=" #vector)     \
        __interrupt void name(void)
        
#elif defined(__SDCC__)
    #define ISR_HANDLER(name, vector)  void name(void) __interrupt(vector)
#endif

/* Usage */
ISR_HANDLER(Timer0_ISR, 1)
{
    /* ISR code */
}
```

## 7. Documentation

### 7.1 Function Documentation

```c
/**
 * @brief   Transmit data over UART
 * 
 * @details This function transmits the specified data buffer over the
 *          configured UART port. The function blocks until all data is
 *          transmitted or a timeout occurs.
 * 
 * @param[in]  handle   Pointer to initialized UART handle
 * @param[in]  data     Pointer to data buffer to transmit
 * @param[in]  len      Number of bytes to transmit
 * 
 * @return  DRV_OK       - Success
 * @return  DRV_TIMEOUT  - Transmission timeout
 * @return  DRV_ERROR    - Hardware error
 * 
 * @note    This function disables interrupts during transmission.
 * @warning Buffer must remain valid until function returns.
 * 
 * @code
 *   uint8_t data[] = {0x01, 0x02, 0x03};
 *   status = DRV_UART_Write(&uart0, data, sizeof(data));
 * @endcode
 */
drv_status_t DRV_UART_Write(uart_handle_t *handle, const uint8_t *data,
                            uint16_t len);
```

### 7.2 In-Line Comments

```c
/* Explain WHY, not WHAT */

/* Bad: States the obvious */
counter++;  /* Increment counter */

/* Good: Explains intent */
counter++;  /* Advance to next sample in rolling average */

/* Good: Documents hardware quirk */
CALL_NOP;   /* Required delay: SPI needs 2 cycles after CS assert */
```

## 8. Error Handling

### 8.1 Return Codes

```c
/* Standard return code enum */
typedef enum {
    DRV_OK            =  0,   /* Success */
    DRV_ERROR         = -1,   /* General error */
    DRV_BUSY          = -2,   /* Resource busy */
    DRV_TIMEOUT       = -3,   /* Operation timed out */
    DRV_INVALID_PARAM = -4,   /* Invalid parameter */
    DRV_NOT_INIT      = -5,   /* Not initialized */
    DRV_NOT_SUPPORTED = -6,   /* Feature not supported */
    DRV_NO_MEMORY     = -7,   /* Out of memory */
    DRV_HW_ERROR      = -8    /* Hardware error */
} drv_status_t;
```

### 8.2 Error Checking Pattern

```c
drv_status_t init_system(void)
{
    drv_status_t status;
    
    /* Initialize GPIO - check result */
    status = DRV_GPIO_Init(&gpio_config);
    if (status != DRV_OK) {
        return status;  /* Propagate error */
    }
    
    /* Initialize UART - check result */
    status = DRV_UART_Init(&uart_handle, UART0, &uart_config);
    if (status != DRV_OK) {
        DRV_GPIO_DeInit();  /* Cleanup on error */
        return status;
    }
    
    return DRV_OK;
}
```

## 9. MISRA-C Guidelines

This codebase follows key MISRA-C:2012 guidelines:

| Rule | Description | Enforcement |
|------|-------------|-------------|
| 10.1 | No implicit type conversions | Use explicit casts |
| 11.3 | No casts between pointer and integer | Use `uintptr_t` |
| 12.1 | Explicit operator precedence | Use parentheses |
| 14.4 | Loop counters unsigned | Use `uint8_t` etc |
| 15.5 | Single exit point encouraged | Minimize early returns |
| 17.7 | Check return values | Always handle errors |
| 21.3 | No dynamic memory | Use static allocation |
