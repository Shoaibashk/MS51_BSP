/*===========================================================================
 * @file    ms51_types.h
 * @brief   MS51 Standard Type Definitions
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details Provides fixed-width integer types and common type definitions
 *          compatible with all supported compilers.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_TYPES_H
#define MS51_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 * Fixed-Width Integer Types
 * These match C99 stdint.h for portability
 *---------------------------------------------------------------------------*/

/* Unsigned integers */
typedef unsigned char       uint8_t;
typedef unsigned int        uint16_t;
typedef unsigned long       uint32_t;

/* Signed integers */
typedef signed char         int8_t;
typedef signed int          int16_t;
typedef signed long         int32_t;

/* Legacy uppercase types (for compatibility) */
typedef unsigned char       UINT8;
typedef unsigned int        UINT16;
typedef unsigned long       UINT32;
typedef signed char         INT8;
typedef signed int          INT16;
typedef signed long         INT32;

/*---------------------------------------------------------------------------
 * Boolean Type
 *---------------------------------------------------------------------------*/
#if defined(__C51__)
    typedef bit             BIT;
    typedef bit             bool_t;
    #define TRUE            1
    #define FALSE           0
    
#elif defined(__ICC8051__)
    #define BIT             __no_init bool __bit
    typedef unsigned char   bool_t;
    #define TRUE            1
    #define FALSE           0
    
#elif defined(__SDCC__)
    typedef __bit           BIT;
    typedef __bit           bool_t;
    #define TRUE            1
    #define FALSE           0
    
#else
    /* For unit testing on host */
    typedef unsigned char   BIT;
    typedef unsigned char   bool_t;
    #define TRUE            1
    #define FALSE           0
#endif

/*---------------------------------------------------------------------------
 * Common Type Definitions
 *---------------------------------------------------------------------------*/

/* NULL pointer */
#ifndef NULL
    #define NULL            ((void *)0)
#endif

/* Boolean values */
#ifndef true
    #define true            1
#endif
#ifndef false
    #define false           0
#endif

/* Enable/Disable */
#define DISABLE             0
#define ENABLE              1

/* Success/Fail */
#define PASS                0
#define FAIL                1

/* Status codes (used throughout the BSP) */
typedef enum {
    HAL_OK              =  0,       /**< Success */
    HAL_ERROR           = -1,       /**< General error */
    HAL_BUSY            = -2,       /**< Resource busy */
    HAL_TIMEOUT         = -3,       /**< Operation timeout */
    HAL_INVALID_PARAM   = -4,       /**< Invalid parameter */
    HAL_NOT_SUPPORTED   = -5,       /**< Feature not supported */
    HAL_NOT_INIT        = -6        /**< Not initialized */
} hal_status_t;

/*---------------------------------------------------------------------------
 * Byte Manipulation Macros
 *---------------------------------------------------------------------------*/

/**
 * @brief Extract high byte from 16-bit value
 */
#define HIBYTE(val16)       ((uint8_t)(((uint16_t)(val16)) >> 8))

/**
 * @brief Extract low byte from 16-bit value
 */
#define LOBYTE(val16)       ((uint8_t)(((uint16_t)(val16)) & 0xFF))

/**
 * @brief Combine two bytes into 16-bit value
 */
#define MAKEWORD(hi, lo)    ((uint16_t)(((uint16_t)(hi) << 8) | (uint8_t)(lo)))

/**
 * @brief Combine four bytes into 32-bit value
 */
#define MAKEDWORD(b3, b2, b1, b0)   \
    ((uint32_t)(((uint32_t)(b3) << 24) | ((uint32_t)(b2) << 16) | \
                ((uint32_t)(b1) << 8) | (uint32_t)(b0)))

/**
 * @brief Extract bytes from 32-bit value
 */
#define BYTE0(val32)        ((uint8_t)(((uint32_t)(val32)) & 0xFF))
#define BYTE1(val32)        ((uint8_t)((((uint32_t)(val32)) >> 8) & 0xFF))
#define BYTE2(val32)        ((uint8_t)((((uint32_t)(val32)) >> 16) & 0xFF))
#define BYTE3(val32)        ((uint8_t)((((uint32_t)(val32)) >> 24) & 0xFF))

/*---------------------------------------------------------------------------
 * Bit Manipulation Macros
 *---------------------------------------------------------------------------*/

/**
 * @brief Set bit position
 */
#define SET_BIT(val, bit)   ((val) |= (1U << (bit)))

/**
 * @brief Clear bit position
 */
#define CLR_BIT(val, bit)   ((val) &= ~(1U << (bit)))

/**
 * @brief Toggle bit position
 */
#define TGL_BIT(val, bit)   ((val) ^= (1U << (bit)))

/**
 * @brief Get bit value
 */
#define GET_BIT(val, bit)   (((val) >> (bit)) & 1U)

/**
 * @brief Create bit mask
 */
#define BIT_MASK(bit)       (1U << (bit))

/*---------------------------------------------------------------------------
 * Alignment and Packing
 *---------------------------------------------------------------------------*/
#if defined(__C51__)
    #define PACKED          /* Keil C51 default packing */
    #define ALIGNED(n)      /* No alignment control */
    
#elif defined(__ICC8051__)
    #define PACKED          __packed
    #define ALIGNED(n)      /* No alignment control */
    
#elif defined(__SDCC__)
    #define PACKED          /* SDCC default packing */
    #define ALIGNED(n)      /* No alignment control */
    
#else
    #define PACKED
    #define ALIGNED(n)
#endif

/*---------------------------------------------------------------------------
 * Static Assertion
 *---------------------------------------------------------------------------*/
#if defined(__C51__) && (__C51__ >= 960)
    #define STATIC_ASSERT(expr, msg)    _Static_assert(expr, msg)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define STATIC_ASSERT(expr, msg)    _Static_assert(expr, msg)
#else
    /* Fallback for older compilers */
    #define STATIC_ASSERT(expr, msg)    typedef char static_assertion_##__LINE__[(expr) ? 1 : -1]
#endif

/*---------------------------------------------------------------------------
 * Unused Parameter
 *---------------------------------------------------------------------------*/
#define UNUSED(x)           ((void)(x))

#ifdef __cplusplus
}
#endif

#endif /* MS51_TYPES_H */
