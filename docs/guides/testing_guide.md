# Testing Guide

## 1. Overview

This guide covers testing strategies for embedded MS51 firmware development:

1. **Unit Testing** - Test individual functions in isolation
2. **Integration Testing** - Test module interactions
3. **Hardware-in-the-Loop (HIL)** - Test on real hardware
4. **Continuous Integration** - Automated testing pipeline

## 2. Unit Testing Strategy

### 2.1 Test Framework Setup

For unit testing, we use a host-based approach where code is compiled for the development PC.

```
tests/
├── unit/
│   ├── test_drv_gpio.c
│   ├── test_drv_uart.c
│   ├── test_drv_timer.c
│   └── ...
├── mocks/
│   ├── mock_ms51_hal.h
│   ├── mock_ms51_hal.c
│   └── mock_sfr.h
├── unity/                    # Unity test framework
│   ├── unity.h
│   ├── unity.c
│   └── unity_internals.h
├── CMakeLists.txt
└── run_tests.bat
```

### 2.2 Mock Hardware Layer

```c
/*===========================================================================
 * @file    mock_sfr.h
 * @brief   Mock SFR definitions for unit testing
 *===========================================================================*/
#ifndef MOCK_SFR_H
#define MOCK_SFR_H

#include <stdint.h>

/* Mock SFR as regular variables */
extern uint8_t MOCK_P0;
extern uint8_t MOCK_P1;
extern uint8_t MOCK_P2;
extern uint8_t MOCK_P3;
extern uint8_t MOCK_SCON;
extern uint8_t MOCK_SBUF;
extern uint8_t MOCK_TCON;
extern uint8_t MOCK_TMOD;
extern uint8_t MOCK_TH0;
extern uint8_t MOCK_TL0;
/* ... more mock SFRs ... */

/* Redirect SFR access to mock variables */
#define P0      MOCK_P0
#define P1      MOCK_P1
#define P2      MOCK_P2
#define P3      MOCK_P3
#define SCON    MOCK_SCON
#define SBUF    MOCK_SBUF
#define TCON    MOCK_TCON
#define TMOD    MOCK_TMOD
#define TH0     MOCK_TH0
#define TL0     MOCK_TL0

/* Mock bit manipulation */
#define set_P10     (MOCK_P1 |= 0x01)
#define clr_P10     (MOCK_P1 &= ~0x01)

/* Test helper: Reset all mock SFRs to default */
void Mock_SFR_Reset(void);

#endif /* MOCK_SFR_H */
```

### 2.3 Unit Test Example

```c
/*===========================================================================
 * @file    test_drv_gpio.c
 * @brief   Unit tests for GPIO driver
 *===========================================================================*/
#include "unity.h"
#include "mock_sfr.h"
#include "drv_gpio.h"

void setUp(void)
{
    /* Reset mocks before each test */
    Mock_SFR_Reset();
}

void tearDown(void)
{
    /* Cleanup after each test */
}

/*---------------------------------------------------------------------------
 * Test Cases
 *---------------------------------------------------------------------------*/

void test_GPIO_Init_PushPullMode(void)
{
    /* Arrange */
    gpio_config_t config = {
        .port = GPIO_PORT_1,
        .pin  = GPIO_PIN_2,
        .mode = GPIO_MODE_PUSH_PULL
    };
    
    /* Act */
    drv_status_t result = DRV_GPIO_Init(&config);
    
    /* Assert */
    TEST_ASSERT_EQUAL(DRV_OK, result);
    TEST_ASSERT_BITS_LOW(0x04, MOCK_P1M1);   /* P1M1.2 = 0 */
    TEST_ASSERT_BITS_HIGH(0x04, MOCK_P1M2);  /* P1M2.2 = 1 */
}

void test_GPIO_Init_InputMode(void)
{
    /* Arrange */
    gpio_config_t config = {
        .port = GPIO_PORT_0,
        .pin  = GPIO_PIN_5,
        .mode = GPIO_MODE_INPUT
    };
    
    /* Act */
    drv_status_t result = DRV_GPIO_Init(&config);
    
    /* Assert */
    TEST_ASSERT_EQUAL(DRV_OK, result);
    TEST_ASSERT_BITS_HIGH(0x20, MOCK_P0M1);  /* P0M1.5 = 1 */
    TEST_ASSERT_BITS_LOW(0x20, MOCK_P0M2);   /* P0M2.5 = 0 */
}

void test_GPIO_Init_NullPointer(void)
{
    /* Act */
    drv_status_t result = DRV_GPIO_Init(NULL);
    
    /* Assert */
    TEST_ASSERT_EQUAL(DRV_INVALID_PARAM, result);
}

void test_GPIO_Write_SetHigh(void)
{
    /* Arrange */
    MOCK_P1 = 0x00;
    
    /* Act */
    DRV_GPIO_Write(GPIO_PORT_1, GPIO_PIN_2, 1);
    
    /* Assert */
    TEST_ASSERT_BITS_HIGH(0x04, MOCK_P1);
}

void test_GPIO_Write_SetLow(void)
{
    /* Arrange */
    MOCK_P1 = 0xFF;
    
    /* Act */
    DRV_GPIO_Write(GPIO_PORT_1, GPIO_PIN_2, 0);
    
    /* Assert */
    TEST_ASSERT_BITS_LOW(0x04, MOCK_P1);
}

void test_GPIO_Toggle(void)
{
    /* Arrange */
    MOCK_P1 = 0x00;
    
    /* Act & Assert - First toggle */
    DRV_GPIO_Toggle(GPIO_PORT_1, GPIO_PIN_2);
    TEST_ASSERT_BITS_HIGH(0x04, MOCK_P1);
    
    /* Act & Assert - Second toggle */
    DRV_GPIO_Toggle(GPIO_PORT_1, GPIO_PIN_2);
    TEST_ASSERT_BITS_LOW(0x04, MOCK_P1);
}

void test_GPIO_Read(void)
{
    /* Arrange */
    MOCK_P1 = 0x04;  /* Pin 2 high */
    
    /* Act & Assert */
    TEST_ASSERT_EQUAL(1, DRV_GPIO_Read(GPIO_PORT_1, GPIO_PIN_2));
    
    /* Arrange */
    MOCK_P1 = 0x00;  /* Pin 2 low */
    
    /* Act & Assert */
    TEST_ASSERT_EQUAL(0, DRV_GPIO_Read(GPIO_PORT_1, GPIO_PIN_2));
}

/*---------------------------------------------------------------------------
 * Test Runner
 *---------------------------------------------------------------------------*/
int main(void)
{
    UNITY_BEGIN();
    
    RUN_TEST(test_GPIO_Init_PushPullMode);
    RUN_TEST(test_GPIO_Init_InputMode);
    RUN_TEST(test_GPIO_Init_NullPointer);
    RUN_TEST(test_GPIO_Write_SetHigh);
    RUN_TEST(test_GPIO_Write_SetLow);
    RUN_TEST(test_GPIO_Toggle);
    RUN_TEST(test_GPIO_Read);
    
    return UNITY_END();
}
```

### 2.4 Building and Running Unit Tests

```makefile
# tests/Makefile

CC = gcc
CFLAGS = -Wall -Wextra -I../hal/include -I../drivers/include -Imocks -Iunity

# Test sources
TEST_SRCS = unit/test_drv_gpio.c \
            mocks/mock_sfr.c \
            unity/unity.c

# Driver under test (compiled for host)
DRV_SRCS = ../drivers/src/drv_gpio.c

# Build test executable
test_gpio: $(TEST_SRCS) $(DRV_SRCS)
	$(CC) $(CFLAGS) -DUNIT_TEST -o $@ $^

# Run tests
.PHONY: run
run: test_gpio
	./test_gpio

# Clean
.PHONY: clean
clean:
	rm -f test_gpio *.o
```

## 3. Integration Testing

### 3.1 Integration Test Structure

```c
/*===========================================================================
 * @file    test_uart_loopback.c
 * @brief   Integration test - UART TX/RX loopback
 *===========================================================================*/
#include "ms51_hal.h"
#include "drv_uart.h"
#include "test_framework.h"

/* Test: UART loopback (requires TX-RX physical connection) */
TEST_CASE(uart_loopback_test)
{
    uint8_t tx_data[] = {0x55, 0xAA, 0x12, 0x34};
    uint8_t rx_data[sizeof(tx_data)];
    uint16_t actual;
    
    /* Initialize UART */
    uart_config_t config = {
        .baudrate = 115200,
        .data_bits = 8,
        .parity = UART_PARITY_NONE,
        .stop_bits = 1
    };
    
    uart_handle_t uart;
    TEST_ASSERT_OK(DRV_UART_Init(&uart, UART_PORT_0, &config));
    
    /* Transmit data */
    TEST_ASSERT_OK(DRV_UART_Write(&uart, tx_data, sizeof(tx_data)));
    
    /* Receive data (with timeout) */
    TEST_ASSERT_OK(DRV_UART_Read(&uart, rx_data, sizeof(rx_data), &actual));
    
    /* Verify */
    TEST_ASSERT_EQUAL(sizeof(tx_data), actual);
    TEST_ASSERT_ARRAY_EQUAL(tx_data, rx_data, sizeof(tx_data));
    
    DRV_UART_DeInit(&uart);
}
```

### 3.2 Integration Test Harness

```c
/*===========================================================================
 * @file    integration_main.c
 * @brief   Integration test harness for MS51
 *===========================================================================*/
#include "ms51_hal.h"
#include "svc_console.h"
#include "test_framework.h"

/* External test declarations */
extern void test_gpio_basic(void);
extern void test_uart_loopback(void);
extern void test_timer_accuracy(void);
extern void test_adc_calibration(void);

void main(void)
{
    /* System initialization */
    HAL_SYS_SetHIRC(HIRC_24MHZ);
    SVC_Console_Init(115200);
    
    printf("\n\r========================================\n\r");
    printf("MS51 BSP Integration Tests\n\r");
    printf("========================================\n\r");
    
    /* Run tests */
    TEST_RUN(test_gpio_basic);
    TEST_RUN(test_uart_loopback);
    TEST_RUN(test_timer_accuracy);
    TEST_RUN(test_adc_calibration);
    
    /* Print summary */
    TEST_PRINT_SUMMARY();
    
    while (1);
}
```

## 4. Hardware-in-the-Loop (HIL) Testing

### 4.1 HIL Test Setup

```
┌─────────────────────────────────────────────────────────────────┐
│                    Host PC (Test Controller)                     │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │              HIL Test Script (Python)                    │    │
│  │  - Load firmware                                         │    │
│  │  - Send test commands via UART                           │    │
│  │  - Verify responses                                      │    │
│  │  - Measure timing with scope (optional)                  │    │
│  └─────────────────────────────────────────────────────────┘    │
│                           │                                      │
│                    USB-Serial                                    │
└───────────────────────────┼─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                    MS51 Target Board                             │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │              HIL Test Firmware                           │    │
│  │  - Command interpreter                                   │    │
│  │  - Test execution                                        │    │
│  │  - Result reporting                                      │    │
│  └─────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 HIL Test Firmware

```c
/*===========================================================================
 * @file    hil_test_fw.c
 * @brief   Hardware-in-the-Loop test firmware
 *===========================================================================*/
#include "ms51_hal.h"
#include "svc_console.h"

/* Command definitions */
#define CMD_TEST_GPIO   'G'
#define CMD_TEST_ADC    'A'
#define CMD_TEST_TIMER  'T'
#define CMD_TEST_PWM    'P'
#define CMD_STATUS      'S'

void process_command(char cmd)
{
    switch (cmd) {
        case CMD_TEST_GPIO:
            run_gpio_test();
            break;
        case CMD_TEST_ADC:
            run_adc_test();
            break;
        case CMD_TEST_TIMER:
            run_timer_test();
            break;
        case CMD_TEST_PWM:
            run_pwm_test();
            break;
        case CMD_STATUS:
            printf("READY\n\r");
            break;
        default:
            printf("ERR:UNKNOWN_CMD\n\r");
            break;
    }
}

void main(void)
{
    HAL_SYS_SetHIRC(HIRC_24MHZ);
    SVC_Console_Init(115200);
    
    printf("HIL_TEST_FW_READY\n\r");
    
    while (1) {
        if (uart_data_available()) {
            char cmd = uart_getchar();
            process_command(cmd);
        }
    }
}
```

### 4.3 HIL Test Script (Python)

```python
#!/usr/bin/env python3
"""
MS51 HIL Test Script
"""
import serial
import time
import sys

class MS51Tester:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(port, baudrate, timeout=2)
        time.sleep(0.5)  # Wait for reset
        
    def send_command(self, cmd):
        self.ser.write(cmd.encode())
        response = self.ser.readline().decode().strip()
        return response
        
    def wait_ready(self):
        response = self.send_command('S')
        return response == "READY"
        
    def test_gpio(self):
        print("Testing GPIO...", end=" ")
        response = self.send_command('G')
        if response == "GPIO:PASS":
            print("PASS")
            return True
        else:
            print(f"FAIL ({response})")
            return False
            
    def test_adc(self):
        print("Testing ADC...", end=" ")
        response = self.send_command('A')
        if response.startswith("ADC:PASS"):
            print("PASS")
            return True
        else:
            print(f"FAIL ({response})")
            return False
            
    def run_all_tests(self):
        results = []
        
        if not self.wait_ready():
            print("ERROR: Device not ready")
            return False
            
        results.append(("GPIO", self.test_gpio()))
        results.append(("ADC", self.test_adc()))
        # Add more tests...
        
        print("\n--- Test Summary ---")
        passed = sum(1 for _, r in results if r)
        total = len(results)
        print(f"Passed: {passed}/{total}")
        
        return passed == total

if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else "COM3"
    tester = MS51Tester(port)
    success = tester.run_all_tests()
    sys.exit(0 if success else 1)
```

## 5. Continuous Integration

### 5.1 CI Pipeline Configuration

```yaml
# .github/workflows/ci.yml

name: MS51 BSP CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc make
          
      - name: Build unit tests
        run: |
          cd tests
          make all
          
      - name: Run unit tests
        run: |
          cd tests
          make run

  static-analysis:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Install cppcheck
        run: sudo apt-get install -y cppcheck
        
      - name: Run static analysis
        run: |
          cppcheck --enable=all --error-exitcode=1 \
            --suppress=missingIncludeSystem \
            hal/ drivers/ services/

  build-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Install SDCC
        run: sudo apt-get install -y sdcc
        
      - name: Build with SDCC
        run: |
          cd samples/blink_led
          make COMPILER=sdcc
```

### 5.2 Test Coverage

```makefile
# Generate coverage report with gcov

CFLAGS += --coverage

test_with_coverage: test_gpio
	./test_gpio
	gcov drv_gpio.c
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report
```

## 6. Test Checklist

### 6.1 Unit Test Checklist

- [ ] All public API functions have unit tests
- [ ] Boundary conditions tested (min, max, zero)
- [ ] Error paths tested (null pointers, invalid params)
- [ ] Return values verified
- [ ] Side effects on mock SFRs verified
- [ ] Test coverage > 80%

### 6.2 Integration Test Checklist

- [ ] Module initialization sequence tested
- [ ] Inter-module communication tested
- [ ] Resource sharing tested (shared timers, etc.)
- [ ] Error propagation tested
- [ ] Power mode transitions tested

### 6.3 HIL Test Checklist

- [ ] All peripherals functional on target
- [ ] Timing requirements met
- [ ] Edge cases tested on real hardware
- [ ] Temperature range tested (if applicable)
- [ ] Long-duration stability tested
