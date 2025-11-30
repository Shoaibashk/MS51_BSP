/*===========================================================================
 * @file    ms51_compiler.h
 * @brief   MS51 Compiler Abstraction Layer
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details Provides compiler-independent macros for memory qualifiers,
 *          interrupt declarations, inline assembly, and intrinsics.
 * 
 * Supported compilers:
 *   - Keil C51 (v9.60+)
 *   - IAR Embedded Workbench for 8051 (v10.30+)
 *   - SDCC (v4.0+)
 *   - GCC (for unit testing on host)
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_COMPILER_H
#define MS51_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Compiler Detection
 *===========================================================================*/
#if defined(__C51__)
    #define COMPILER_KEIL       1
    #define COMPILER_NAME       "Keil C51"
    
#elif defined(__ICC8051__)
    #define COMPILER_IAR        1
    #define COMPILER_NAME       "IAR EW8051"
    
#elif defined(__SDCC__)
    #define COMPILER_SDCC       1
    #define COMPILER_NAME       "SDCC"
    
#elif defined(__GNUC__)
    #define COMPILER_GCC        1
    #define COMPILER_NAME       "GCC (Host)"
    
#else
    #error "Unsupported compiler. Please use Keil C51, IAR EW8051, or SDCC."
#endif

/*===========================================================================
 * Memory Space Qualifiers
 * 
 * DATA_MEM   - Internal directly addressable RAM (0x00-0x7F)
 * IDATA_MEM  - Internal indirectly addressable RAM (0x00-0xFF)
 * XDATA_MEM  - External RAM (movx instructions)
 * CODE_MEM   - Program memory (movc instructions)
 * PDATA_MEM  - Paged external RAM (movx @Ri)
 *===========================================================================*/

#if defined(COMPILER_KEIL)
    #define DATA_MEM        data
    #define IDATA_MEM       idata
    #define XDATA_MEM       xdata
    #define CODE_MEM        code
    #define PDATA_MEM       pdata
    #define NEAR_MEM        data
    #define FAR_MEM         xdata
    
#elif defined(COMPILER_IAR)
    #define DATA_MEM        __data
    #define IDATA_MEM       __idata
    #define XDATA_MEM       __xdata
    #define CODE_MEM        __code
    #define PDATA_MEM       __pdata
    #define NEAR_MEM        __data
    #define FAR_MEM         __xdata
    
#elif defined(COMPILER_SDCC)
    #define DATA_MEM        __data
    #define IDATA_MEM       __idata
    #define XDATA_MEM       __xdata
    #define CODE_MEM        __code
    #define PDATA_MEM       __pdata
    #define NEAR_MEM        __near
    #define FAR_MEM         __far
    
#else  /* Host compiler for unit testing */
    #define DATA_MEM
    #define IDATA_MEM
    #define XDATA_MEM
    #define CODE_MEM        const
    #define PDATA_MEM
    #define NEAR_MEM
    #define FAR_MEM
#endif

/*===========================================================================
 * Bit-Addressable Memory
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #define BIT_MEM         bit
    #define BDATA_MEM       bdata
    #define SBIT            sbit
    
#elif defined(COMPILER_IAR)
    #define BIT_MEM         __bit
    #define BDATA_MEM       __bdata
    #define SBIT            __bit
    
#elif defined(COMPILER_SDCC)
    #define BIT_MEM         __bit
    #define BDATA_MEM       __bdata
    #define SBIT            __sbit
    
#else
    #define BIT_MEM         unsigned char
    #define BDATA_MEM
    #define SBIT            unsigned char
#endif

/*===========================================================================
 * Special Function Register (SFR) Declarations
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #define SFR(name, addr)         sfr name = addr
    #define SFR16(name, addr)       sfr16 name = addr
    #define SBIT_DEF(name, addr, bit) sbit name = addr^bit
    
#elif defined(COMPILER_IAR)
    #define SFR(name, addr)         __sfr __no_init volatile unsigned char name @ addr
    #define SFR16(name, addr)       __sfr __no_init volatile unsigned int name @ addr
    #define SBIT_DEF(name, addr, bit) /* IAR uses different syntax */
    
#elif defined(COMPILER_SDCC)
    #define SFR(name, addr)         __sfr __at(addr) name
    #define SFR16(name, addr)       __sfr16 __at(addr) name
    #define SBIT_DEF(name, addr, bit) __sbit __at(addr+bit) name
    
#else
    /* Mock SFRs for unit testing */
    #define SFR(name, addr)         extern volatile unsigned char name
    #define SFR16(name, addr)       extern volatile unsigned int name
    #define SBIT_DEF(name, addr, bit) extern volatile unsigned char name
#endif

/*===========================================================================
 * Interrupt Declarations
 * 
 * Usage:
 *   ISR_HANDLER(Timer0_ISR, TIMER0_VECTOR)
 *   {
 *       // ISR code
 *   }
 *===========================================================================*/

/* Interrupt vector numbers */
#define INT0_VECTOR         0       /* External Interrupt 0 */
#define TIMER0_VECTOR       1       /* Timer 0 */
#define INT1_VECTOR         2       /* External Interrupt 1 */
#define TIMER1_VECTOR       3       /* Timer 1 */
#define UART0_VECTOR        4       /* UART 0 */
#define TIMER2_VECTOR       5       /* Timer 2 */
#define I2C_VECTOR          6       /* I2C */
#define PIN_INT_VECTOR      7       /* Pin Interrupt */
#define BOD_VECTOR          8       /* Brown-out Detect */
#define SPI_VECTOR          9       /* SPI */
#define WDT_VECTOR          10      /* Watchdog Timer */
#define ADC_VECTOR          11      /* ADC */
#define CAPTURE_VECTOR      12      /* Input Capture */
#define PWM_VECTOR          13      /* PWM */
#define UART1_VECTOR        15      /* UART 1 */
#define TIMER3_VECTOR       16      /* Timer 3 */
#define WKT_VECTOR          17      /* Wake-up Timer */

#if defined(COMPILER_KEIL)
    #define ISR_HANDLER(name, vector)   void name(void) interrupt vector
    #define ISR_USING(bank)             using bank
    
#elif defined(COMPILER_IAR)
    /* IAR requires #pragma, cannot be in macro cleanly */
    #define ISR_HANDLER(name, vector)   __interrupt void name(void)
    #define ISR_USING(bank)             /* Not applicable */
    
    /* Vector addresses for IAR */
    #define ISR_VECTOR_ADDR(vector)     (0x03 + ((vector) * 8))
    
#elif defined(COMPILER_SDCC)
    #define ISR_HANDLER(name, vector)   void name(void) __interrupt(vector)
    #define ISR_USING(bank)             __using(bank)
    
#else
    #define ISR_HANDLER(name, vector)   void name(void)
    #define ISR_USING(bank)
#endif

/*===========================================================================
 * Inline Assembly
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #define ASM(x)          _asm x _endasm
    #define ASM_NOP()       _nop_()
    #define PUSH_SFR(sfr)   _push_(sfr)
    #define POP_SFR(sfr)    _pop_(sfr)
    
#elif defined(COMPILER_IAR)
    #define ASM(x)          asm(x)
    #define ASM_NOP()       asm("nop")
    #define PUSH_SFR(sfr)   asm("PUSH " #sfr)
    #define POP_SFR(sfr)    asm("POP " #sfr)
    
#elif defined(COMPILER_SDCC)
    #define ASM(x)          __asm__(x)
    #define ASM_NOP()       __asm__("nop")
    #define PUSH_SFR(sfr)   __asm__("PUSH " #sfr)
    #define POP_SFR(sfr)    __asm__("POP " #sfr)
    
#else
    #define ASM(x)          /* No-op for host */
    #define ASM_NOP()       ((void)0)
    #define PUSH_SFR(sfr)   ((void)0)
    #define POP_SFR(sfr)    ((void)0)
#endif

/*===========================================================================
 * NOP Instruction
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #include <intrins.h>
    #define CALL_NOP        _nop_()
    
#elif defined(COMPILER_IAR)
    #include <intrinsics.h>
    #define CALL_NOP        __no_operation()
    
#elif defined(COMPILER_SDCC)
    #define CALL_NOP        __asm__("nop")
    
#else
    #define CALL_NOP        ((void)0)
#endif

/*===========================================================================
 * Inline Function Hint
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #define INLINE          /* Keil inlines small functions automatically */
    
#elif defined(COMPILER_IAR)
    #define INLINE          inline
    
#elif defined(COMPILER_SDCC)
    #define INLINE          inline
    
#else
    #define INLINE          static inline
#endif

/*===========================================================================
 * Weak Linking
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #define WEAK            /* Use linker overlay for weak symbols */
    
#elif defined(COMPILER_IAR)
    #define WEAK            __weak
    
#elif defined(COMPILER_SDCC)
    #define WEAK            /* SDCC 4.0+ may support __attribute__((weak)) */
    
#else
    #define WEAK            __attribute__((weak))
#endif

/*===========================================================================
 * Section Placement
 *===========================================================================*/
#if defined(COMPILER_KEIL)
    #define SECTION(name)   /* Use pragma in Keil */
    
#elif defined(COMPILER_IAR)
    #define SECTION(name)   @ name
    
#elif defined(COMPILER_SDCC)
    #define SECTION(name)   __at(name)
    
#else
    #define SECTION(name)   __attribute__((section(name)))
#endif

/*===========================================================================
 * Volatile and Const
 *===========================================================================*/
/* Ensure proper volatile for hardware registers */
#ifndef __IO
    #define __IO            volatile
#endif

#ifndef __I
    #define __I             volatile const
#endif

#ifndef __O
    #define __O             volatile
#endif

#ifdef __cplusplus
}
#endif

#endif /* MS51_COMPILER_H */
