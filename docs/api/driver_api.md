# MS51 Driver API Reference

## Overview

The Driver layer provides peripheral-specific abstractions built on top of the HAL layer. Each driver follows a consistent API pattern with initialization, configuration, and operation functions.

---

## Common Driver Patterns

### Initialization Pattern

```c
/* All drivers follow this pattern */
drv_status_t DRV_XXX_Init(xxx_handle_t *handle, const xxx_config_t *config);
drv_status_t DRV_XXX_DeInit(xxx_handle_t *handle);
```

### Handle Structure

Each driver uses a handle structure to maintain state:

```c
typedef struct {
    uint8_t instance;      /* Peripheral instance number */
    uint8_t initialized;   /* Initialization flag */
    /* Instance-specific fields */
} xxx_handle_t;
```

### Configuration Structure

Driver configurations are passed via const structure pointers:

```c
typedef struct {
    /* Configuration fields */
} xxx_config_t;
```

---

## GPIO Driver

### Header: `drv_gpio.h`

### Types

```c
typedef enum {
    GPIO_PORT_0 = 0,
    GPIO_PORT_1 = 1,
    GPIO_PORT_2 = 2,
    GPIO_PORT_3 = 3
} gpio_port_t;

typedef enum {
    GPIO_MODE_QUASI_BIDIR,    /* Quasi-bidirectional (default) */
    GPIO_MODE_PUSH_PULL,      /* Push-pull output */
    GPIO_MODE_INPUT_ONLY,     /* Input only (high impedance) */
    GPIO_MODE_OPEN_DRAIN      /* Open drain output */
} gpio_mode_t;

typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP   = 1
} gpio_pull_t;

typedef enum {
    GPIO_INT_DISABLE = 0,
    GPIO_INT_FALLING = 1,
    GPIO_INT_RISING  = 2,
    GPIO_INT_BOTH    = 3
} gpio_int_edge_t;

typedef void (*gpio_callback_t)(uint8_t port, uint8_t pin);

typedef struct {
    gpio_port_t port;
    uint8_t pin;
    gpio_mode_t mode;
    gpio_pull_t pull;
    gpio_int_edge_t int_edge;
    gpio_callback_t callback;
} gpio_config_t;

typedef struct {
    gpio_port_t port;
    uint8_t pin;
    uint8_t initialized;
} gpio_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_GPIO_Init(handle, config)` | Initialize GPIO pin |
| `DRV_GPIO_DeInit(handle)` | De-initialize GPIO pin |
| `DRV_GPIO_Write(handle, value)` | Write digital value (0 or 1) |
| `DRV_GPIO_Read(handle)` | Read digital value |
| `DRV_GPIO_Toggle(handle)` | Toggle output state |
| `DRV_GPIO_SetMode(handle, mode)` | Change GPIO mode |
| `DRV_GPIO_SetPull(handle, pull)` | Configure pull-up |
| `DRV_GPIO_EnableInterrupt(handle)` | Enable pin interrupt |
| `DRV_GPIO_DisableInterrupt(handle)` | Disable pin interrupt |

### Example

```c
gpio_handle_t led;
gpio_config_t led_cfg = {
    .port = GPIO_PORT_1,
    .pin = 0,
    .mode = GPIO_MODE_PUSH_PULL,
    .pull = GPIO_PULL_NONE,
    .int_edge = GPIO_INT_DISABLE
};

DRV_GPIO_Init(&led, &led_cfg);
DRV_GPIO_Write(&led, 1);  /* LED on */
DRV_GPIO_Toggle(&led);    /* LED off */
```

---

## UART Driver

### Header: `drv_uart.h`

### Types

```c
typedef enum {
    UART_PORT_0 = 0,
    UART_PORT_1 = 1
} uart_port_t;

typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD  = 1,
    UART_PARITY_EVEN = 2
} uart_parity_t;

typedef enum {
    UART_STOPBITS_1 = 0,
    UART_STOPBITS_2 = 1
} uart_stopbits_t;

typedef void (*uart_callback_t)(uint8_t data);

typedef struct {
    uart_port_t port;
    uint32_t baudrate;
    uart_parity_t parity;
    uart_stopbits_t stopbits;
    uint8_t tx_int_enable;
    uint8_t rx_int_enable;
    uart_callback_t rx_callback;
    uart_callback_t tx_callback;
} uart_config_t;

typedef struct {
    uart_port_t port;
    uint8_t initialized;
    uart_callback_t rx_callback;
    uart_callback_t tx_callback;
} uart_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_UART_Init(handle, config)` | Initialize UART |
| `DRV_UART_DeInit(handle)` | De-initialize UART |
| `DRV_UART_SendByte(handle, data)` | Send single byte (blocking) |
| `DRV_UART_ReceiveByte(handle, data, timeout)` | Receive byte with timeout |
| `DRV_UART_SendBuffer(handle, buf, len)` | Send buffer |
| `DRV_UART_ReceiveBuffer(handle, buf, len, timeout)` | Receive buffer |
| `DRV_UART_SendString(handle, str)` | Send null-terminated string |
| `DRV_UART_IsRxReady(handle)` | Check if RX data available |
| `DRV_UART_IsTxReady(handle)` | Check if TX ready |

### Example

```c
uart_handle_t uart0;
uart_config_t uart_cfg = {
    .port = UART_PORT_0,
    .baudrate = 115200,
    .parity = UART_PARITY_NONE,
    .stopbits = UART_STOPBITS_1,
    .rx_int_enable = 0,
    .tx_int_enable = 0
};

DRV_UART_Init(&uart0, &uart_cfg);
DRV_UART_SendString(&uart0, "Hello, MS51!\r\n");
```

---

## Timer Driver

### Header: `drv_timer.h`

### Types

```c
typedef enum {
    TIMER_0 = 0,
    TIMER_1 = 1,
    TIMER_2 = 2,
    TIMER_3 = 3
} timer_id_t;

typedef enum {
    TIMER_MODE_0 = 0,   /* 13-bit timer */
    TIMER_MODE_1 = 1,   /* 16-bit timer */
    TIMER_MODE_2 = 2,   /* 8-bit auto-reload */
    TIMER_MODE_3 = 3    /* Timer 0: two 8-bit timers */
} timer_mode_t;

typedef enum {
    TIMER_CLK_FSYS      = 0,
    TIMER_CLK_FSYS_12   = 1,
    TIMER_CLK_EXT       = 2
} timer_clk_src_t;

typedef void (*timer_callback_t)(timer_id_t id);

typedef struct {
    timer_id_t id;
    timer_mode_t mode;
    timer_clk_src_t clock_source;
    uint16_t period_us;
    uint8_t auto_reload;
    uint8_t int_enable;
    timer_callback_t callback;
} timer_config_t;

typedef struct {
    timer_id_t id;
    uint8_t initialized;
    timer_callback_t callback;
} timer_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_TIMER_Init(handle, config)` | Initialize timer |
| `DRV_TIMER_DeInit(handle)` | De-initialize timer |
| `DRV_TIMER_Start(handle)` | Start timer |
| `DRV_TIMER_Stop(handle)` | Stop timer |
| `DRV_TIMER_SetPeriod(handle, period_us)` | Set timer period |
| `DRV_TIMER_GetCounter(handle)` | Get current counter value |
| `DRV_TIMER_SetCallback(handle, callback)` | Set interrupt callback |

### Example

```c
timer_handle_t timer0;
timer_config_t tim_cfg = {
    .id = TIMER_0,
    .mode = TIMER_MODE_1,
    .clock_source = TIMER_CLK_FSYS_12,
    .period_us = 1000,
    .auto_reload = 1,
    .int_enable = 1,
    .callback = timer0_isr
};

DRV_TIMER_Init(&timer0, &tim_cfg);
DRV_TIMER_Start(&timer0);
```

---

## ADC Driver

### Header: `drv_adc.h`

### Types

```c
typedef enum {
    ADC_CH_0  = 0,
    ADC_CH_1  = 1,
    ADC_CH_2  = 2,
    ADC_CH_3  = 3,
    ADC_CH_4  = 4,
    ADC_CH_5  = 5,
    ADC_CH_6  = 6,
    ADC_CH_7  = 7,
    ADC_CH_BANDGAP = 8,
    ADC_CH_VDD_DIV4 = 15
} adc_channel_t;

typedef enum {
    ADC_TRIG_SOFTWARE = 0,
    ADC_TRIG_PWM      = 1,
    ADC_TRIG_GPIO     = 2
} adc_trigger_t;

typedef enum {
    ADC_ALIGN_RIGHT = 0,
    ADC_ALIGN_LEFT  = 1
} adc_align_t;

typedef void (*adc_callback_t)(uint16_t result);

typedef struct {
    adc_channel_t channel;
    adc_trigger_t trigger;
    adc_align_t alignment;
    uint8_t int_enable;
    adc_callback_t callback;
} adc_config_t;

typedef struct {
    uint8_t initialized;
    adc_channel_t channel;
    adc_callback_t callback;
} adc_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_ADC_Init(handle, config)` | Initialize ADC |
| `DRV_ADC_DeInit(handle)` | De-initialize ADC |
| `DRV_ADC_StartConversion(handle)` | Start single conversion |
| `DRV_ADC_GetResult(handle)` | Get conversion result (12-bit) |
| `DRV_ADC_ReadBlocking(handle, channel)` | Blocking read of channel |
| `DRV_ADC_SetChannel(handle, channel)` | Change input channel |
| `DRV_ADC_EnableCompare(handle, hi, lo)` | Enable compare interrupt |
| `DRV_ADC_GetVDD(handle)` | Calculate VDD from bandgap |

### Example

```c
adc_handle_t adc;
adc_config_t adc_cfg = {
    .channel = ADC_CH_0,
    .trigger = ADC_TRIG_SOFTWARE,
    .alignment = ADC_ALIGN_RIGHT,
    .int_enable = 0
};

DRV_ADC_Init(&adc, &adc_cfg);
uint16_t value = DRV_ADC_ReadBlocking(&adc, ADC_CH_0);
```

---

## PWM Driver

### Header: `drv_pwm.h`

### Types

```c
typedef enum {
    PWM_CH_0 = 0,
    PWM_CH_1 = 1,
    PWM_CH_2 = 2,
    PWM_CH_3 = 3,
    PWM_CH_4 = 4,
    PWM_CH_5 = 5
} pwm_channel_t;

typedef enum {
    PWM_CLK_FSYS    = 0,
    PWM_CLK_TIMER1  = 1
} pwm_clk_src_t;

typedef enum {
    PWM_MODE_INDEPENDENT   = 0,
    PWM_MODE_COMPLEMENTARY = 1,
    PWM_MODE_SYNC          = 2
} pwm_mode_t;

typedef enum {
    PWM_ALIGN_EDGE   = 0,
    PWM_ALIGN_CENTER = 1
} pwm_align_t;

typedef struct {
    pwm_channel_t channel;
    pwm_clk_src_t clock_source;
    pwm_mode_t mode;
    pwm_align_t alignment;
    uint8_t prescaler;
    uint16_t period;
    uint16_t duty;
    uint8_t invert;
    uint8_t dead_time;
} pwm_config_t;

typedef struct {
    pwm_channel_t channel;
    uint8_t initialized;
} pwm_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_PWM_Init(handle, config)` | Initialize PWM channel |
| `DRV_PWM_DeInit(handle)` | De-initialize PWM |
| `DRV_PWM_Start(handle)` | Start PWM output |
| `DRV_PWM_Stop(handle)` | Stop PWM output |
| `DRV_PWM_SetDuty(handle, duty)` | Set duty cycle value |
| `DRV_PWM_SetDutyPercent(handle, percent)` | Set duty as 0-100% |
| `DRV_PWM_SetPeriod(handle, period)` | Set PWM period |
| `DRV_PWM_SetDeadTime(handle, time)` | Set dead time |

### Example

```c
pwm_handle_t pwm0;
pwm_config_t pwm_cfg = {
    .channel = PWM_CH_0,
    .clock_source = PWM_CLK_FSYS,
    .mode = PWM_MODE_INDEPENDENT,
    .alignment = PWM_ALIGN_EDGE,
    .prescaler = 0,
    .period = 1000,
    .duty = 500,    /* 50% */
    .invert = 0
};

DRV_PWM_Init(&pwm0, &pwm_cfg);
DRV_PWM_Start(&pwm0);
DRV_PWM_SetDutyPercent(&pwm0, 75);  /* Change to 75% */
```

---

## SPI Driver

### Header: `drv_spi.h`

### Types

```c
typedef enum {
    SPI_MODE_MASTER = 0,
    SPI_MODE_SLAVE  = 1
} spi_mode_t;

typedef enum {
    SPI_CPOL_0 = 0,  /* Clock idle low */
    SPI_CPOL_1 = 1   /* Clock idle high */
} spi_cpol_t;

typedef enum {
    SPI_CPHA_0 = 0,  /* Sample on leading edge */
    SPI_CPHA_1 = 1   /* Sample on trailing edge */
} spi_cpha_t;

typedef enum {
    SPI_CLK_DIV_2   = 0,
    SPI_CLK_DIV_4   = 1,
    SPI_CLK_DIV_8   = 2,
    SPI_CLK_DIV_16  = 3,
    SPI_CLK_DIV_32  = 4,
    SPI_CLK_DIV_64  = 5,
    SPI_CLK_DIV_128 = 6
} spi_clk_div_t;

typedef void (*spi_callback_t)(uint8_t data);

typedef struct {
    spi_mode_t mode;
    spi_cpol_t cpol;
    spi_cpha_t cpha;
    spi_clk_div_t clock_div;
    uint8_t lsb_first;
    uint8_t int_enable;
    spi_callback_t callback;
} spi_config_t;

typedef struct {
    uint8_t initialized;
    spi_callback_t callback;
} spi_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_SPI_Init(handle, config)` | Initialize SPI |
| `DRV_SPI_DeInit(handle)` | De-initialize SPI |
| `DRV_SPI_TransferByte(handle, tx)` | Send/receive single byte |
| `DRV_SPI_TransferBuffer(handle, tx, rx, len)` | Transfer buffer |
| `DRV_SPI_SendByte(handle, data)` | Send byte (ignore RX) |
| `DRV_SPI_ReceiveByte(handle)` | Receive byte (send 0xFF) |
| `DRV_SPI_SetClockDiv(handle, div)` | Change clock divider |

### Example

```c
spi_handle_t spi;
spi_config_t spi_cfg = {
    .mode = SPI_MODE_MASTER,
    .cpol = SPI_CPOL_0,
    .cpha = SPI_CPHA_0,
    .clock_div = SPI_CLK_DIV_4,
    .lsb_first = 0
};

DRV_SPI_Init(&spi, &spi_cfg);
uint8_t rx = DRV_SPI_TransferByte(&spi, 0x9F);  /* Read JEDEC ID */
```

---

## I2C Driver

### Header: `drv_i2c.h`

### Types

```c
typedef enum {
    I2C_MODE_MASTER = 0,
    I2C_MODE_SLAVE  = 1
} i2c_mode_t;

typedef enum {
    I2C_SPEED_100K = 0,   /* Standard mode */
    I2C_SPEED_400K = 1    /* Fast mode */
} i2c_speed_t;

typedef void (*i2c_callback_t)(uint8_t status);

typedef struct {
    i2c_mode_t mode;
    i2c_speed_t speed;
    uint8_t slave_addr;       /* For slave mode */
    uint8_t int_enable;
    i2c_callback_t callback;
} i2c_config_t;

typedef struct {
    uint8_t initialized;
    i2c_mode_t mode;
    i2c_callback_t callback;
} i2c_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_I2C_Init(handle, config)` | Initialize I2C |
| `DRV_I2C_DeInit(handle)` | De-initialize I2C |
| `DRV_I2C_MasterWrite(handle, addr, data, len)` | Master transmit |
| `DRV_I2C_MasterRead(handle, addr, data, len)` | Master receive |
| `DRV_I2C_MasterWriteRead(handle, addr, tx, txlen, rx, rxlen)` | Write then read |
| `DRV_I2C_SlaveEnable(handle)` | Enable slave mode |
| `DRV_I2C_SlaveDisable(handle)` | Disable slave mode |

### Example

```c
i2c_handle_t i2c;
i2c_config_t i2c_cfg = {
    .mode = I2C_MODE_MASTER,
    .speed = I2C_SPEED_100K
};

DRV_I2C_Init(&i2c, &i2c_cfg);

uint8_t tx[] = {0x00};  /* Register address */
uint8_t rx[2];
DRV_I2C_MasterWriteRead(&i2c, 0x50, tx, 1, rx, 2);  /* Read 2 bytes from EEPROM */
```

---

## Watchdog Driver

### Header: `drv_wdt.h`

### Types

```c
typedef enum {
    WDT_PRESCALE_DIV_1   = 0,
    WDT_PRESCALE_DIV_2   = 1,
    WDT_PRESCALE_DIV_4   = 2,
    WDT_PRESCALE_DIV_8   = 3,
    WDT_PRESCALE_DIV_16  = 4,
    WDT_PRESCALE_DIV_32  = 5,
    WDT_PRESCALE_DIV_64  = 6,
    WDT_PRESCALE_DIV_128 = 7
} wdt_prescale_t;

typedef void (*wdt_callback_t)(void);

typedef struct {
    wdt_prescale_t prescaler;
    uint8_t reset_enable;
    uint8_t int_enable;
    wdt_callback_t callback;
} wdt_config_t;

typedef struct {
    uint8_t initialized;
    wdt_callback_t callback;
} wdt_handle_t;
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_WDT_Init(handle, config)` | Initialize watchdog |
| `DRV_WDT_DeInit(handle)` | De-initialize watchdog |
| `DRV_WDT_Start(handle)` | Start watchdog |
| `DRV_WDT_Stop(handle)` | Stop watchdog |
| `DRV_WDT_Kick(handle)` | Reset/feed watchdog |
| `DRV_WDT_GetTimeoutMs(handle)` | Get timeout in ms |

### Example

```c
wdt_handle_t wdt;
wdt_config_t wdt_cfg = {
    .prescaler = WDT_PRESCALE_DIV_64,
    .reset_enable = 1,
    .int_enable = 0
};

DRV_WDT_Init(&wdt, &wdt_cfg);
DRV_WDT_Start(&wdt);

while (1) {
    /* Main loop */
    DRV_WDT_Kick(&wdt);  /* Must kick regularly */
}
```

---

## Flash Driver

### Header: `drv_flash.h`

### Types

```c
typedef enum {
    FLASH_AREA_APROM = 0,
    FLASH_AREA_LDROM = 1,
    FLASH_AREA_CONFIG = 2,
    FLASH_AREA_DATAFLASH = 3
} flash_area_t;

typedef struct {
    flash_area_t area;
    uint8_t iap_enable;
} flash_config_t;

typedef struct {
    uint8_t initialized;
    flash_area_t area;
} flash_handle_t;
```

### Constants

```c
#define FLASH_PAGE_SIZE     128
#define DATAFLASH_BASE_8K   0x3000   /* 8K devices */
#define DATAFLASH_BASE_16K  0x3800   /* 16K devices */
#define DATAFLASH_BASE_32K  0x7000   /* 32K devices */
```

### Functions

| Function | Description |
|----------|-------------|
| `DRV_FLASH_Init(handle, config)` | Initialize flash driver |
| `DRV_FLASH_DeInit(handle)` | De-initialize flash |
| `DRV_FLASH_Read(handle, addr, data, len)` | Read from flash |
| `DRV_FLASH_WriteByte(handle, addr, data)` | Write single byte |
| `DRV_FLASH_WriteBuffer(handle, addr, data, len)` | Write buffer |
| `DRV_FLASH_ErasePage(handle, addr)` | Erase 128-byte page |
| `DRV_FLASH_EraseAll(handle)` | Erase entire area |

### Example

```c
flash_handle_t flash;
flash_config_t flash_cfg = {
    .area = FLASH_AREA_DATAFLASH,
    .iap_enable = 1
};

DRV_FLASH_Init(&flash, &flash_cfg);

/* Read configuration */
uint8_t config[16];
DRV_FLASH_Read(&flash, DATAFLASH_BASE_16K, config, 16);

/* Write configuration */
DRV_FLASH_ErasePage(&flash, DATAFLASH_BASE_16K);
DRV_FLASH_WriteBuffer(&flash, DATAFLASH_BASE_16K, config, 16);
```

---

## See Also

- [HAL API Reference](hal_api.md)
- [Service API Reference](service_api.md)
- [MS51 Hardware Overview](../hardware/ms51_overview.md)
