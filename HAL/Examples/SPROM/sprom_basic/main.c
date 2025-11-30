/**
 *******************************************************************************
 * @file    main.c
 * @brief   SPROM HAL Example - Security Protected ROM Operations
 * @version 1.0.0
 * @date    2024
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.
 *
 * @details This example demonstrates SPROM operations:
 *          - Read SPROM data
 *          - Write data to SPROM
 *          - Verify SPROM content
 *          - Check lock status
 *
 * @warning Do NOT lock SPROM unless you understand the implications!
 *          Once locked, SPROM cannot be unlocked except by full chip erase.
 *
 *******************************************************************************
 */

#include "ms51_hal.h"

/* Test data for SPROM */
uint8_t sprom_test_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t sprom_read_buffer[16];

/**
 * @brief  Main program entry
 */
void main(void)
{
    HAL_StatusTypeDef status;
    uint8_t i;
    uint8_t isLocked;
    
    /* Initialize HAL */
    HAL_Init();
    
    /* Initialize UART for debug output */
    HAL_UART_InitPrintf(HAL_SYSCLK_FREQ);
    
    printf("MS51 HAL SPROM Example\r\n");
    printf("======================\r\n\r\n");
    
    printf("SPROM Size: %d bytes\r\n", HAL_SPROM_SIZE);
    printf("SPROM Base Address: 0x%04X\r\n\r\n", HAL_SPROM_BASE);
    
    /* Check if SPROM is locked */
    isLocked = HAL_SPROM_IsLocked();
    printf("SPROM Lock Status: %s\r\n\r\n", isLocked ? "LOCKED" : "UNLOCKED");
    
    if (isLocked) {
        printf("WARNING: SPROM is locked. Cannot modify data.\r\n");
        printf("Only read operations will work.\r\n\r\n");
    }
    
    /* Read current SPROM content */
    printf("1. Reading first 16 bytes from SPROM...\r\n");
    status = HAL_SPROM_Read(0x00, sprom_read_buffer, 16);
    if (status == HAL_OK) {
        printf("   Address 0x00-0x0F: ");
        for (i = 0; i < 16; i++) {
            printf("%02X ", sprom_read_buffer[i]);
        }
        printf("\r\n");
    } else {
        printf("   Read failed!\r\n");
    }
    
    /* Read using single byte function */
    printf("\n2. Reading single bytes from SPROM...\r\n");
    for (i = 0; i < 8; i++) {
        uint8_t data = HAL_SPROM_ReadByte(i);
        printf("   SPROM[0x%02X] = 0x%02X\r\n", i, data);
    }
    
    /* Write test (only if not locked) */
    if (!isLocked) {
        printf("\n3. Writing test data to SPROM address 0x00...\r\n");
        printf("   Data to write: ");
        for (i = 0; i < sizeof(sprom_test_data); i++) {
            printf("%02X ", sprom_test_data[i]);
        }
        printf("\r\n");
        
        status = HAL_SPROM_Write(0x00, sprom_test_data, sizeof(sprom_test_data));
        if (status == HAL_OK) {
            printf("   Write successful!\r\n");
            
            /* Read back and verify */
            printf("\n4. Verifying written data...\r\n");
            status = HAL_SPROM_Verify(0x00, sprom_test_data, sizeof(sprom_test_data));
            if (status == HAL_OK) {
                printf("   Verification: PASSED\r\n");
            } else {
                printf("   Verification: FAILED\r\n");
            }
            
            /* Read back to show */
            printf("\n5. Reading back written data...\r\n");
            status = HAL_SPROM_Read(0x00, sprom_read_buffer, sizeof(sprom_test_data));
            if (status == HAL_OK) {
                printf("   Data read: ");
                for (i = 0; i < sizeof(sprom_test_data); i++) {
                    printf("%02X ", sprom_read_buffer[i]);
                }
                printf("\r\n");
            }
        } else {
            printf("   Write failed!\r\n");
        }
        
        /* Demonstrate WriteVerify function */
        printf("\n6. Using WriteVerify function...\r\n");
        sprom_test_data[0] = 0xAA;
        sprom_test_data[1] = 0xBB;
        status = HAL_SPROM_WriteVerify(0x00, sprom_test_data, 2);
        printf("   WriteVerify result: %s\r\n", 
               (status == HAL_OK) ? "SUCCESS" : "FAILED");
               
        /*
         * WARNING: DO NOT UNCOMMENT THIS UNLESS YOU WANT TO LOCK THE SPROM!
         * Once locked, the SPROM cannot be modified or read from outside.
         * Only a full chip erase will unlock it.
         *
         * printf("\n7. Locking SPROM (THIS IS PERMANENT!)...\r\n");
         * status = HAL_SPROM_Lock();
         * if (status == HAL_OK) {
         *     printf("   SPROM is now LOCKED!\r\n");
         * }
         */
    } else {
        printf("\nSkipping write tests - SPROM is locked.\r\n");
    }
    
    printf("\r\n=== SPROM Example Complete ===\r\n");
    
    while (1) {
        HAL_Delay(1000);
    }
}
