/**
 *******************************************************************************
 * @file    main.c
 * @brief   I2C EEPROM Example - MS51 HAL Library
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @details I2C communication example with external EEPROM.
 *          Writes and reads data from 24C02 EEPROM.
 *
 * Hardware:
 *   - MS51FB9AE development board
 *   - 24C02 EEPROM connected to I2C bus
 *   - SCL on P1.3, SDA on P1.4
 *   - UART0: TXD on P0.6, RXD on P0.7
 *
 *******************************************************************************
 */

#include "ms51_hal.h"
#include <stdio.h>

/* System clock frequency */
#define SYSCLK_FREQ     24000000UL

/* EEPROM I2C address (7-bit) */
#define EEPROM_ADDR     0x50

/* Test data */
#define TEST_ADDR       0x0010
#define TEST_DATA_SIZE  8

/* I2C handle */
HAL_I2C_HandleTypeDef hi2c;

/**
 * @brief  Main program entry
 */
void main(void)
{
    uint8_t writeData[TEST_DATA_SIZE] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t readData[TEST_DATA_SIZE];
    uint8_t i;
    HAL_StatusTypeDef status;
    
    /* Initialize HAL library */
    HAL_Init();
    
    /* Initialize UART for printf */
    HAL_UART_InitPrintf(SYSCLK_FREQ);
    
    /* Initialize I2C as Master at 100kHz */
    hi2c.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c.Init.ClockSpeed = HAL_I2C_SPEED_STANDARD;
    HAL_I2C_Init(&hi2c);
    
    /* Print welcome message */
    printf("\n\r");
    printf("MS51 HAL I2C EEPROM Example\n\r");
    printf("EEPROM Address: 0x%02X\n\r", EEPROM_ADDR);
    printf("\n\r");
    
    /* Check if EEPROM is present */
    printf("Checking EEPROM... ");
    status = HAL_I2C_IsDeviceReady(&hi2c, EEPROM_ADDR, 3, 100);
    if (status != HAL_OK) {
        printf("FAILED! EEPROM not found.\n\r");
        while (1);
    }
    printf("OK\n\r");
    
    /* Write test data to EEPROM */
    printf("Writing data to address 0x%04X...\n\r", TEST_ADDR);
    for (i = 0; i < TEST_DATA_SIZE; i++) {
        printf("  [%d] = 0x%02X\n\r", i, writeData[i]);
    }
    
    status = HAL_I2C_Mem_Write(&hi2c, EEPROM_ADDR, TEST_ADDR, 2, 
                                writeData, TEST_DATA_SIZE, 1000);
    if (status != HAL_OK) {
        printf("Write FAILED!\n\r");
        while (1);
    }
    printf("Write complete.\n\r");
    
    /* Wait for EEPROM write cycle (max 5ms) */
    HAL_Delay(10);
    
    /* Read data back from EEPROM */
    printf("\n\rReading data from address 0x%04X...\n\r", TEST_ADDR);
    status = HAL_I2C_Mem_Read(&hi2c, EEPROM_ADDR, TEST_ADDR, 2, 
                               readData, TEST_DATA_SIZE, 1000);
    if (status != HAL_OK) {
        printf("Read FAILED!\n\r");
        while (1);
    }
    
    for (i = 0; i < TEST_DATA_SIZE; i++) {
        printf("  [%d] = 0x%02X", i, readData[i]);
        if (readData[i] == writeData[i]) {
            printf(" OK\n\r");
        } else {
            printf(" MISMATCH!\n\r");
        }
    }
    
    printf("\n\rTest complete.\n\r");
    
    /* Loop forever */
    while (1) {
        HAL_Delay(1000);
    }
}
