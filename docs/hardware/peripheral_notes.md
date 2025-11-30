# MS51 Peripheral Programming Notes

## GPIO Configuration

### Mode Register Layout

Each GPIO port has two mode registers (PxM1, PxM2):

| P0M1.n | P0M2.n | Mode |
|--------|--------|------|
| 0 | 0 | Quasi-bidirectional (default) |
| 0 | 1 | Push-pull output |
| 1 | 0 | Input only (high-Z) |
| 1 | 1 | Open-drain |

### Pin Configuration Example

```c
/* P1.0 as push-pull output */
P1M1 &= ~0x01;  /* Clear M1.0 */
P1M2 |= 0x01;   /* Set M2.0 */

/* P1.1 as input only */
P1M1 |= 0x02;   /* Set M1.1 */
P1M2 &= ~0x02;  /* Clear M2.1 */
```

### Schmitt Trigger

All inputs have built-in Schmitt trigger for noise immunity.

### Pull-up Resistors

Internal pull-ups are controlled by PxS register:

```c
P1S |= 0x01;   /* Enable pull-up on P1.0 */
P1S &= ~0x01;  /* Disable pull-up on P1.0 */
```

---

## Timer Configuration

### Timer 0/1 Modes

| Mode | Description |
|------|-------------|
| Mode 0 | 13-bit timer (legacy) |
| Mode 1 | 16-bit timer |
| Mode 2 | 8-bit auto-reload |
| Mode 3 | Timer 0: Two 8-bit timers |

### Timer 0 Mode 1 Setup (16-bit)

```c
/* 1ms timer at 24MHz FSYS */
TMOD = 0x01;           /* Timer 0 Mode 1 */
TH0 = (65536-24000) >> 8;
TL0 = (65536-24000) & 0xFF;
TR0 = 1;               /* Start timer */
```

### Timer 2 Auto-reload

```c
/* Timer 2 for 1ms interrupt at 24MHz */
T2MOD = 0x00;          /* Auto-reload mode */
RCMP2H = (65536-24000) >> 8;
RCMP2L = (65536-24000) & 0xFF;
TR2 = 1;               /* Start Timer 2 */
ET2 = 1;               /* Enable interrupt */
```

### Timer Clock Sources

Timer 0/1: FSYS or FSYS/12 (selected by T0M/T1M bits in CKCON)
Timer 2: FSYS or FSYS/16 (selected by T2DIV)
Timer 3: FSYS with 3-bit prescaler

---

## PWM Configuration

### PWM Clock

```c
/* PWM clock = FSYS / (prescaler+1) */
PWMPH = 0x00;          /* Period high byte */
PWMPL = 0xFF;          /* Period low byte (256 counts) */
```

### Independent Mode

```c
/* PWM0 at 50% duty */
PIOCON0 |= 0x01;       /* Enable PWM0 output */
PWM0H = 0x00;          /* Duty high */
PWM0L = 0x80;          /* Duty low (128/256 = 50%) */
PWMCON0 = 0x01;        /* Enable PWM */
```

### Complementary Mode with Dead-time

```c
/* PWM0/PWM1 complementary with dead-time */
PIOCON0 |= 0x03;       /* Enable PWM0, PWM1 */
PWMCON1 |= 0x01;       /* Complementary mode */
BIT_TMP = EA;          /* Save interrupt state */
EA = 0;
TA = 0xAA;
TA = 0x55;
PDTEN = 0x01;          /* Dead-time enable for pair 0 */
PDT01 = 0x10;          /* Dead-time count */
EA = BIT_TMP;
```

### PWM Update (Reload)

```c
/* Load new values atomically */
LOAD = 1;              /* Trigger reload */
while (LOAD);          /* Wait for reload complete */
```

---

## UART Configuration

### UART0 with Timer 1

```c
/* 115200 baud at 24MHz */
SCON = 0x52;           /* Mode 1, REN, TI */
TMOD = 0x20;           /* Timer 1 Mode 2 */
TH1 = 256 - (1000000UL / 115200);  /* ~243 for 115200 */
CKCON |= 0x10;         /* Timer 1 FSYS */
TR1 = 1;               /* Start Timer 1 */
```

### UART1 with Internal Baud Rate

```c
/* UART1 at 115200 baud */
SCON_1 = 0x52;         /* Mode 1, REN, TI */
T3CON = 0x08;          /* BRCK = 1 (UART1 uses Timer 3) */
RH3 = (256 - (24000000UL/16/115200)) >> 8;
RL3 = (256 - (24000000UL/16/115200)) & 0xFF;
TR3 = 1;               /* Start Timer 3 */
```

### Transmit/Receive

```c
/* Send byte */
SBUF = data;
while (!TI);           /* Wait for transmit complete */
TI = 0;                /* Clear flag */

/* Receive byte */
while (!RI);           /* Wait for receive */
data = SBUF;
RI = 0;                /* Clear flag */
```

---

## ADC Configuration

### Basic Setup

```c
/* Enable ADC, channel 0 */
ADCCON1 = 0x01;        /* ADC on */
ADCCON0 = 0x00;        /* Channel 0, software trigger */
```

### Start Conversion

```c
/* Single conversion */
ADCS = 1;              /* Start conversion */
while (ADCF == 0);     /* Wait for complete */
result = ((uint16_t)ADCRH << 4) | (ADCRL >> 4);  /* 12-bit result */
ADCF = 0;              /* Clear flag */
```

### Bandgap Reference

```c
/* Read bandgap for VDD calculation */
ADCCON0 = 0x08;        /* Select bandgap channel */
ADCS = 1;
while (ADCF == 0);
uint16_t bandgap = ((uint16_t)ADCRH << 4) | (ADCRL >> 4);
/* VDD = (4096 * 1.22V) / bandgap_reading */
uint16_t vdd_mv = (uint32_t)4096 * 1220 / bandgap;
```

---

## SPI Configuration

### Master Mode

```c
/* SPI Master, Mode 0, FSYS/4 clock */
SPSR = 0x00;           /* Clear status */
SPCR = 0x50;           /* Master, CPOL=0, CPHA=0, enable */
SPCR2 = 0x01;          /* Clock divider = FSYS/4 */
```

### Data Transfer

```c
/* Full-duplex transfer */
SPDR = tx_data;        /* Start transfer */
while (!(SPSR & 0x80)); /* Wait for SPIF */
rx_data = SPDR;        /* Read received data */
```

---

## I²C Configuration

### Master Mode

```c
/* I2C Master, 100kHz */
I2CLK = 24000000UL / 4 / 100000 - 1;  /* Clock divider */
I2CON = 0x44;          /* I2CEN, AA */
```

### Master Write

```c
/* Start condition */
STA = 1;
SI = 0;
while (!SI);           /* Wait for START sent */

/* Send address + W */
I2DAT = (slave_addr << 1) | 0;
SI = 0;
while (!SI);           /* Wait for ACK */

/* Send data */
I2DAT = data;
SI = 0;
while (!SI);           /* Wait for ACK */

/* Stop condition */
STO = 1;
SI = 0;
```

---

## Flash/IAP Programming

### Enable IAP

```c
/* Enable IAP access to Data Flash */
TA = 0xAA;
TA = 0x55;
CHPCON |= 0x01;        /* IAPEN = 1 */
TA = 0xAA;
TA = 0x55;
IAPUEN |= 0x01;        /* APUEN = 1 (enable APROM update) */
```

### Read Flash

```c
/* Read byte from Data Flash */
IAPAL = addr & 0xFF;
IAPAH = addr >> 8;
IAPCN = 0x00;          /* Read command */
TA = 0xAA;
TA = 0x55;
IAPTRG |= 0x01;        /* Trigger */
while (IAPTRG & 0x01); /* Wait complete */
data = IAPFD;
```

### Write Flash

```c
/* Write byte to Data Flash */
IAPAL = addr & 0xFF;
IAPAH = addr >> 8;
IAPFD = data;
IAPCN = 0x21;          /* Program command */
TA = 0xAA;
TA = 0x55;
IAPTRG |= 0x01;        /* Trigger */
while (IAPTRG & 0x01); /* Wait complete */
```

### Erase Page

```c
/* Erase 128-byte page */
IAPAL = page_addr & 0xFF;
IAPAH = page_addr >> 8;
IAPCN = 0x22;          /* Page erase command */
TA = 0xAA;
TA = 0x55;
IAPTRG |= 0x01;        /* Trigger */
while (IAPTRG & 0x01); /* Wait complete */
```

---

## Power Management

### Enter Idle Mode

```c
/* CPU halts, peripherals continue */
PCON |= 0x01;          /* IDL = 1 */
/* Wakes on any interrupt */
```

### Enter Power-Down Mode

```c
/* Minimal power consumption */
/* First configure wake-up sources */
WKCON |= 0x01;         /* Enable WKT wake-up */
PCON |= 0x02;          /* PD = 1 */
_nop_();               /* Required for reliable entry */
/* Wakes on WKT, INT0/1, GPIO, I2C, UART */
```

### Wake-up Timer

```c
/* WKT with LIRC (10kHz) */
WKCON = 0x03;          /* Enable WKT, LIRC source */
RWK = 0x00;            /* Reload value (MSB) */
RWKH = 0x10;           /* Full value = period */
WKTF = 0;              /* Clear flag */
/* WKT period = (RWK * 256 + RWKH) * 100us */
```

---

## Brown-out Detector

### Configuration

```c
/* BOD at 2.4V threshold, reset enabled */
TA = 0xAA;
TA = 0x55;
BODCON0 = 0x84;        /* BOD enabled, 2.4V, reset mode */

/* Check BOD status */
if (BODCON0 & 0x08) {  /* BORF flag */
    /* Brown-out occurred */
    BODCON0 &= ~0x08;  /* Clear flag */
}
```

### BOD Thresholds

| BODCON0[6:5] | Threshold |
|--------------|-----------|
| 00 | 4.4V |
| 01 | 3.7V |
| 10 | 2.4V |
| 11 | 2.0V |

---

## Watchdog Timer

### Configuration

```c
/* WDT with 1.4s timeout, reset enabled */
TA = 0xAA;
TA = 0x55;
WDCON = 0x07;          /* Enable WDT, prescaler = 7 */
```

### Feed Watchdog

```c
/* Clear WDT counter */
TA = 0xAA;
TA = 0x55;
WDCON |= 0x40;         /* WDCLR = 1 */
```

### WDT Timeout Calculation

Timeout = 2^(17 + prescaler) / FSYS

| Prescaler | Timeout @ 24MHz |
|-----------|-----------------|
| 0 | 5.5 ms |
| 1 | 11 ms |
| 2 | 22 ms |
| 3 | 44 ms |
| 4 | 87 ms |
| 5 | 175 ms |
| 6 | 350 ms |
| 7 | 700 ms |

---

## Interrupt Priorities

### Priority Levels

MS51 supports 4 interrupt priority levels (0-3, where 3 is highest).

### Priority Registers

| Register | Bits | Interrupts |
|----------|------|------------|
| IP | IPL | INT0, Timer0, INT1, Timer1, UART0, Timer2 |
| IPH | IPH | High bits for above |
| EIP | EIPL | ADC, BOD, SPI, PWM, Capture, I2C |
| EIPH | EIPH | High bits for above |

### Setting Priority

```c
/* Set Timer 0 to highest priority (3) */
IP |= 0x02;            /* IPL.1 = 1 */
IPH |= 0x02;           /* IPH.1 = 1 */
```

---

## See Also

- [MS51 Hardware Overview](ms51_overview.md)
- [HAL API Reference](../api/hal_api.md)
- [Driver API Reference](../api/driver_api.md)
