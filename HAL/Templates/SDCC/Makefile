# MS51 HAL Project - SDCC Makefile
# Template for building with SDCC compiler

# Compiler and tools
CC = sdcc
OBJCOPY = sdobjcopy
PACKIHX = packihx

# Target name
TARGET = ms51_hal_project

# Device settings
MCU = mcs51
MODEL = --model-small
CODE_SIZE = --code-size 16384
XRAM_SIZE = --xram-size 1024

# Directories
HAL_ROOT = ../..
HAL_CORE = $(HAL_ROOT)/Core
HAL_DEVICE = $(HAL_ROOT)/Device/MS51_16K
HAL_DRIVERS = $(HAL_ROOT)/Drivers

# Include paths
INCLUDES = -I$(HAL_CORE)/Inc \
           -I$(HAL_DEVICE) \
           -I$(HAL_DRIVERS)/GPIO \
           -I$(HAL_DRIVERS)/UART \
           -I$(HAL_DRIVERS)/Timer \
           -I$(HAL_DRIVERS)/ADC \
           -I$(HAL_DRIVERS)/I2C \
           -I$(HAL_DRIVERS)/SPI \
           -I$(HAL_DRIVERS)/PWM \
           -I$(HAL_DRIVERS)/WDT \
           -I$(HAL_DRIVERS)/Flash \
           -I$(HAL_DRIVERS)/System

# Compiler flags
CFLAGS = -m$(MCU) $(MODEL) $(CODE_SIZE) $(XRAM_SIZE) $(INCLUDES)
CFLAGS += --opt-code-size
CFLAGS += -DMS51_16K
CFLAGS += -DHAL_COMPILER_SDCC

# Source files
SOURCES = main.c \
          $(HAL_CORE)/Src/ms51_hal.c \
          $(HAL_DRIVERS)/GPIO/ms51_hal_gpio.c \
          $(HAL_DRIVERS)/UART/ms51_hal_uart.c \
          $(HAL_DRIVERS)/Timer/ms51_hal_timer.c \
          $(HAL_DRIVERS)/ADC/ms51_hal_adc.c \
          $(HAL_DRIVERS)/I2C/ms51_hal_i2c.c \
          $(HAL_DRIVERS)/SPI/ms51_hal_spi.c \
          $(HAL_DRIVERS)/PWM/ms51_hal_pwm.c \
          $(HAL_DRIVERS)/WDT/ms51_hal_wdt.c \
          $(HAL_DRIVERS)/Flash/ms51_hal_flash.c \
          $(HAL_DRIVERS)/System/ms51_hal_system.c

# Object files
OBJECTS = $(SOURCES:.c=.rel)

# Default target
all: $(TARGET).ihx

# Compile source files
%.rel: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET).ihx: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Clean
clean:
	rm -f *.rel *.rst *.lst *.map *.mem *.ihx *.hex *.bin *.asm *.sym *.lnk
	rm -f $(HAL_CORE)/Src/*.rel $(HAL_CORE)/Src/*.rst $(HAL_CORE)/Src/*.lst
	rm -f $(HAL_DRIVERS)/*/*.rel $(HAL_DRIVERS)/*/*.rst $(HAL_DRIVERS)/*/*.lst

# Convert to binary
$(TARGET).bin: $(TARGET).ihx
	$(OBJCOPY) -I ihex -O binary $< $@

# Flash programming (example using nuvoicp or similar)
flash: $(TARGET).ihx
	@echo "Use your preferred programmer to flash $(TARGET).ihx"
	# nuvoicp -w $(TARGET).ihx

.PHONY: all clean flash
