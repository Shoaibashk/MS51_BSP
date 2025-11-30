/**
 *******************************************************************************
 * @file    main.c
 * @brief   EEPROM HAL Example - Basic Read/Write
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates basic EEPROM emulation operations:
 *          - Initialize EEPROM area
 *          - Write single bytes
 *          - Write arrays of data
 *          - Read data back
 *          - Write/read 16-bit and 32-bit values
 *          - Update with wear leveling optimization
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Test data */
uint8_t test_data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint8_t read_buffer[8];

/**
 * @brief  Main program entry
 */
void main(void)
{
    HAL_StatusTypeDef status;
    uint16_t val16;
    uint32_t val32;
    uint8_t i;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(HAL_SYSCLK_FREQ);
    
    printf("MS51 HAL EEPROM Example\r\n");
    printf("=======================\r\n\r\n");
    
    /* Initialize EEPROM with default settings */
    status = HAL_EEPROM_Init();
    if (status != HAL_OK) {
        printf("EEPROM Init failed!\r\n");
        while (1);
    }
    printf("EEPROM initialized at address 0x%04X\r\n", HAL_EEPROM_GetBaseAddress());
    
    /* Write a single byte */
    printf("\n1. Writing single byte 0xAB to address 0x0000...\r\n");
    status = HAL_EEPROM_WriteByte(0x0000, 0xAB);
    if (status == HAL_OK) {
        uint8_t readByte = HAL_EEPROM_ReadByte(0x0000);
        printf("   Read back: 0x%02X %s\r\n", readByte, 
               (readByte == 0xAB) ? "(OK)" : "(FAIL)");
    } else {
        printf("   Write failed!\r\n");
    }
    
    /* Write array of data */
    printf("\n2. Writing array of 8 bytes to address 0x0010...\r\n");
    status = HAL_EEPROM_Write(0x0010, test_data, sizeof(test_data));
    if (status == HAL_OK) {
        printf("   Data written: ");
        for (i = 0; i < sizeof(test_data); i++) {
            printf("%02X ", test_data[i]);
        }
        printf("\r\n");
        
        /* Read back */
        status = HAL_EEPROM_Read(0x0010, read_buffer, sizeof(read_buffer));
        if (status == HAL_OK) {
            printf("   Data read:    ");
            for (i = 0; i < sizeof(read_buffer); i++) {
                printf("%02X ", read_buffer[i]);
            }
            printf("\r\n");
        }
    }
    
    /* Write 16-bit value */
    printf("\n3. Writing 16-bit value 0x1234 to address 0x0020...\r\n");
    status = HAL_EEPROM_WriteU16(0x0020, 0x1234);
    if (status == HAL_OK) {
        val16 = HAL_EEPROM_ReadU16(0x0020);
        printf("   Read back: 0x%04X %s\r\n", val16, 
               (val16 == 0x1234) ? "(OK)" : "(FAIL)");
    }
    
    /* Write 32-bit value */
    printf("\n4. Writing 32-bit value 0xDEADBEEF to address 0x0030...\r\n");
    status = HAL_EEPROM_WriteU32(0x0030, 0xDEADBEEFUL);
    if (status == HAL_OK) {
        val32 = HAL_EEPROM_ReadU32(0x0030);
        printf("   Read back: 0x%08lX %s\r\n", val32, 
               (val32 == 0xDEADBEEFUL) ? "(OK)" : "(FAIL)");
    }
    
    /* Demonstrate Update function (wear leveling) */
    printf("\n5. Using Update function (wear-optimized write)...\r\n");
    
    /* First update - will write */
    printf("   Update address 0x0040 with 0x55...\r\n");
    status = HAL_EEPROM_Update(0x0040, 0x55);
    printf("   Result: %s\r\n", (status == HAL_OK) ? "Written" : "Failed");
    
    /* Second update with same value - should skip write */
    printf("   Update address 0x0040 with 0x55 again (should skip)...\r\n");
    status = HAL_EEPROM_Update(0x0040, 0x55);
    printf("   Result: %s (skipped if data unchanged)\r\n", 
           (status == HAL_OK) ? "OK" : "Failed");
    
    /* Third update with different value - will write */
    printf("   Update address 0x0040 with 0xAA...\r\n");
    status = HAL_EEPROM_Update(0x0040, 0xAA);
    printf("   Result: %s\r\n", (status == HAL_OK) ? "Written" : "Failed");
    
    /* Verify and report */
    printf("\n6. Verification with HAL_EEPROM_Verify()...\r\n");
    status = HAL_EEPROM_Verify(0x0010, test_data, sizeof(test_data));
    printf("   Data at 0x0010: %s\r\n", 
           (status == HAL_OK) ? "VERIFIED OK" : "VERIFICATION FAILED");
    
    printf("\r\n=== EEPROM Example Complete ===\r\n");
    
    while (1) {
        HAL_Delay(1000);
    }
}
