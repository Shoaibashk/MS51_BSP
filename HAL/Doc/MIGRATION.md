# Migration Guide: BSP to HAL Library

This guide helps you migrate existing code from the Nuvoton MS51 BSP to the new HAL library.

## Overview

The HAL library provides a more structured and portable API compared to the original BSP. Key differences include:

| Aspect | BSP | HAL |
|--------|-----|-----|
| API Style | Direct register access with macros | Handle-based with init structures |
| Portability | Keil-centric | Keil/SDCC/IAR compatible |
| Documentation | Minimal | Comprehensive |
| Error Handling | None | Status return codes |
| Code Size | Smaller | Slightly larger |

## Migration Steps

### 1. Include Files

**BSP:**
```c
#include "MS51_16K.h"
#include "Common.h"
#include "Delay.h"
#include "GPIO.h"
#include "UART.h"
```

**HAL:**
```c
#include "ms51_hal.h"
```

### 2. GPIO Configuration

**BSP:**
```c
/* Set P1.2 as push-pull output */
P12_PushPull_Mode;

/* Set P0.0 as input */
P00_Input_Mode;

/* Write pin */
P12 = 0;
P12 = 1;

/* Read pin */
if (P06 == 0) { ... }
```

**HAL:**
```c
/* Configure P1.2 as push-pull output */
HAL_GPIO_InitTypeDef gpio_init;
gpio_init.Pin = HAL_GPIO_PIN_2;
gpio_init.Mode = HAL_GPIO_MODE_PUSHPULL;
gpio_init.Schmitt = HAL_GPIO_SCHMITT_DISABLE;
HAL_GPIO_Init(HAL_GPIO_PORT1, &gpio_init);

/* Configure P0.0 as input */
gpio_init.Pin = HAL_GPIO_PIN_0;
gpio_init.Mode = HAL_GPIO_MODE_INPUT;
gpio_init.Schmitt = HAL_GPIO_SCHMITT_ENABLE;
HAL_GPIO_Init(HAL_GPIO_PORT0, &gpio_init);

/* Write pin */
HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, HAL_GPIO_PIN_RESET);
HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, HAL_GPIO_PIN_SET);

/* Read pin */
if (HAL_GPIO_ReadPin(HAL_GPIO_PORT0, HAL_GPIO_PIN_6) == HAL_GPIO_PIN_RESET) { ... }
```

### 3. UART Configuration

**BSP:**
```c
/* Initialize UART0 at 115200 baud */
InitialUART0_Timer1(115200);
TI = 1;

/* Send character */
Send_Data_To_UART0(0x55);

/* Printf */
printf("Hello\n");
```

**HAL:**
```c
/* Initialize UART0 with printf support */
HAL_UART_InitPrintf(24000000UL);

/* Send character */
HAL_UART_HandleTypeDef huart;
huart.Instance = HAL_UART0;
HAL_UART_TransmitByte(&huart, 0x55);

/* Printf (works automatically after HAL_UART_InitPrintf) */
printf("Hello\n");
```

### 4. Timer Configuration

**BSP:**
```c
/* Timer0 delay */
Timer0_Delay1ms(100);
Timer0_Delay100us(500);

/* Timer interrupt */
TIMER0_MODE1_ENABLE;
set_TMOD_T0M1;
TL0 = TIMER_DIV12_VALUE_10ms;
TH0 = TIMER_DIV12_VALUE_10ms>>8;
set_ET0;
set_EA;
TR0 = 1;
```

**HAL:**
```c
/* Timer delay */
HAL_Delay(100);           /* 100ms */
HAL_DelayUs(500);         /* 500us */

/* Timer interrupt */
HAL_Timer01_InitTypeDef timer_init;
timer_init.Mode = HAL_TIMER_MODE1;
timer_init.ClockSource = HAL_TIMER_CLK_FSYS_DIV12;
timer_init.Period = 20000;  /* 10ms at 24MHz/12 */
timer_init.EnableGate = 0;
timer_init.CounterMode = 0;
HAL_Timer0_Init(&timer_init);
HAL_Timer_EnableInterrupt(HAL_TIMER0);
HAL_EnableInterrupts();
HAL_Timer_Start(HAL_TIMER0);
```

### 5. ADC Configuration

**BSP:**
```c
/* Enable ADC */
Enable_ADC_AIN4;
ADCCON1 |= 0x30;  /* Clock div */
ADCCON2 |= 0x0E;  /* Acquisition time */

/* Read ADC */
clr_ADCF;
set_ADCS;
while(!ADCF);
result = (ADCRH << 4) | (ADCRL & 0x0F);
```

**HAL:**
```c
/* Initialize ADC */
HAL_ADC_SimpleInit(HAL_ADC_DIV_2, HAL_ADC_AQT_5);
HAL_ADC_ConfigChannel(HAL_ADC_CHANNEL_4);
HAL_ADC_Enable();

/* Read ADC */
uint16_t result = HAL_ADC_Read(HAL_ADC_CHANNEL_4);
```

### 6. I2C Configuration

**BSP:**
```c
/* Initialize I2C */
I2C_Master_Open(24000000, 100000);

/* Write to slave */
I2C_Write_Byte(slaveAddr, data);

/* Read from slave */
data = I2C_Read_Byte(slaveAddr);
```

**HAL:**
```c
/* Initialize I2C */
HAL_I2C_HandleTypeDef hi2c;
hi2c.Init.Mode = HAL_I2C_MODE_MASTER;
hi2c.Init.ClockSpeed = HAL_I2C_SPEED_STANDARD;
HAL_I2C_Init(&hi2c);

/* Write to slave */
HAL_I2C_MasterTransmit(&hi2c, slaveAddr, &data, 1, 1000);

/* Read from slave */
HAL_I2C_MasterReceive(&hi2c, slaveAddr, &data, 1, 1000);
```

### 7. PWM Configuration

**BSP:**
```c
/* Configure PWM */
PWM0_P12_OUTPUT_ENABLE;
PWM_IMDEPENDENT_MODE;
PWM_CLOCK_DIV_8;
PWMPH = 0x07;
PWMPL = 0xCF;
PWM0H = 0x03;
PWM0L = 0xE7;
set_PWMRUN;
```

**HAL:**
```c
/* Configure PWM */
HAL_PWM_SimpleInit(HAL_PWM_DIV_8, 1999);  /* Period = 1999 */
HAL_PWM_SetDuty(HAL_PWM_CHANNEL_0, 999);   /* 50% duty */
HAL_PWM_EnableOutput(HAL_PWM_CHANNEL_0);
HAL_PWM_Start();
```

## Common Macro Translations

| BSP Macro | HAL Equivalent |
|-----------|----------------|
| `set_EA` | `HAL_EnableInterrupts()` |
| `clr_EA` | `HAL_DisableInterrupts()` |
| `P12 = 0` | `HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, HAL_GPIO_PIN_RESET)` |
| `P12 = 1` | `HAL_GPIO_WritePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2, HAL_GPIO_PIN_SET)` |
| `P12 ^= 1` | `HAL_GPIO_TogglePin(HAL_GPIO_PORT1, HAL_GPIO_PIN_2)` |
| `Timer0_Delay1ms(n)` | `HAL_Delay(n)` |
| `set_ADCS` | `HAL_ADC_StartConversion()` |
| `while(!ADCF)` | `while(!HAL_ADC_IsConversionComplete())` |

## TA-Protected Register Access

**BSP:**
```c
TA = 0xAA;
TA = 0x55;
SFRS = 0x01;
```

**HAL:**
```c
HAL_TA_WRITE(SFRS, 0x01);
/* or */
HAL_SFR_PAGE1();
```

## Interrupt Handlers

The HAL provides optional interrupt handlers that you can hook into:

**BSP:**
```c
void Timer0_ISR(void) interrupt 1
{
    TF0 = 0;
    // Your code
}
```

**HAL:**
```c
void Timer0_ISR(void) interrupt 1
{
    HAL_Timer_ClearFlag(HAL_TIMER0);
    // Reload if needed
    TH0 = g_timer0_th;
    TL0 = g_timer0_tl;
    
    // Your code
    HAL_SysTickCallback();  // Optional: call HAL callback
}
```

## Tips for Migration

1. **Start with a simple test**: Migrate a blink example first to verify the setup.

2. **Keep BSP as reference**: Don't delete the BSP code immediately; use it as reference.

3. **Enable modules selectively**: Only enable HAL modules you need in `ms51_hal_conf.h`.

4. **Use HAL delays carefully**: HAL delay functions may use Timer0. Adjust if you need Timer0 for other purposes.

5. **Check pin assignments**: Verify that HAL pin configurations match your hardware.

6. **Review clock settings**: Ensure `HAL_SYSCLK_FREQ` in `ms51_hal_conf.h` matches your actual clock.

## Code Size Considerations

The HAL library adds some overhead compared to direct register access. To minimize code size:

1. Disable unused modules in `ms51_hal_conf.h`
2. Use simple init functions instead of full handle-based initialization
3. For very tight code constraints, consider using HAL for development and optimizing critical sections later

## Questions?

If you encounter issues during migration, please check the examples or open an issue on GitHub.
