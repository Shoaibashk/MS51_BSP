/*===========================================================================
 * @file    ms51_sfr_32k.h
 * @brief   MS51 32KB Variant SFR Definitions
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Special Function Register definitions for MS51 32KB variants:
 *          - MS51FC0AE
 *          - MS51XC0BE
 *          - MS51EB0AE
 *          - MS51EC0AE
 *          - MS51TC0AE
 *          - MS51PC0AE
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_SFR_32K_H
#define MS51_SFR_32K_H

#include "ms51_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Device Information
 *===========================================================================*/
#define MS51_FLASH_SIZE         32768   /* 32KB APROM */
#define MS51_LDROM_SIZE         4096    /* 4KB LDROM */
#define MS51_IRAM_SIZE          256     /* 256 bytes IRAM */
#define MS51_XRAM_SIZE          2048    /* 2KB XRAM */
#define MS51_DATAFLASH_SIZE     4096    /* 4KB Data Flash */
#define MS51_PAGE_SIZE          128     /* Flash page size */

/*===========================================================================
 * SFR Definitions - Compiler Specific
 *===========================================================================*/

#if defined(COMPILER_KEIL)

/*---------------------------------------------------------------------------
 * Keil C51 SFR Definitions - Page 0
 *---------------------------------------------------------------------------*/

/* Port 0 (0x80) */
sfr P0          = 0x80;
sbit P07        = P0^7;
sbit RXD        = P0^7;
sbit P06        = P0^6;
sbit TXD        = P0^6;
sbit P05        = P0^5;
sbit P04        = P0^4;
sbit STADC      = P0^4;
sbit P03        = P0^3;
sbit P02        = P0^2;
sbit RXD_1      = P0^2;
sbit P01        = P0^1;
sbit MISO       = P0^1;
sbit P00        = P0^0;
sbit MOSI       = P0^0;

/* Stack Pointer, Data Pointer (0x81-0x83) */
sfr SP          = 0x81;
sfr DPL         = 0x82;
sfr DPH         = 0x83;

/* RC Oscillator Trim (0x84-0x85) */
sfr RCTRIM0     = 0x84;
sfr RCTRIM1     = 0x85;

/* RWK, PCON (0x86-0x87) */
sfr RWKL        = 0x86;
sfr PCON        = 0x87;

/* Timer Control (0x88-0x8F) */
sfr TCON        = 0x88;
sbit TF1        = TCON^7;
sbit TR1        = TCON^6;
sbit TF0        = TCON^5;
sbit TR0        = TCON^4;
sbit IE1        = TCON^3;
sbit IT1        = TCON^2;
sbit IE0        = TCON^1;
sbit IT0        = TCON^0;

sfr TMOD        = 0x89;
sfr TL0         = 0x8A;
sfr TL1         = 0x8B;
sfr TH0         = 0x8C;
sfr TH1         = 0x8D;
sfr CKCON       = 0x8E;
sfr WKCON       = 0x8F;

/* Port 1 (0x90) */
sfr P1          = 0x90;
sbit P17        = P1^7;
sbit P16        = P1^6;
sbit TXD_1      = P1^6;
sbit P15        = P1^5;
sbit P14        = P1^4;
sbit SDA        = P1^4;
sbit P13        = P1^3;
sbit SCL        = P1^3;
sbit P12        = P1^2;
sbit P11        = P1^1;
sbit P10        = P1^0;

/* System Control (0x91-0x97) */
sfr SFRS        = 0x91;     /* TA Protected */
sfr CAPCON0     = 0x92;
sfr CAPCON1     = 0x93;
sfr CAPCON2     = 0x94;
sfr CKDIV       = 0x95;
sfr CKSWT       = 0x96;     /* TA Protected */
sfr CKEN        = 0x97;     /* TA Protected */

/* UART0 (0x98-0x99) */
sfr SCON        = 0x98;
sbit SM0        = SCON^7;
sbit FE         = SCON^7;
sbit SM1        = SCON^6;
sbit SM2        = SCON^5;
sbit REN        = SCON^4;
sbit TB8        = SCON^3;
sbit RB8        = SCON^2;
sbit TI         = SCON^1;
sbit RI         = SCON^0;

sfr SBUF        = 0x99;
sfr SBUF_1      = 0x9A;
sfr EIE         = 0x9B;
sfr EIE1        = 0x9C;
sfr CHPCON      = 0x9F;     /* TA Protected */

/* Port 2 (0xA0) */
sfr P2          = 0xA0;
sbit P27        = P2^7;
sbit P26        = P2^6;
sbit P25        = P2^5;
sbit P24        = P2^4;
sbit P23        = P2^3;
sbit P22        = P2^2;
sbit P21        = P2^1;
sbit P20        = P2^0;

/* Auxiliary, BOD, IAP (0xA2-0xAF) */
sfr AUXR1       = 0xA2;
sfr BODCON0     = 0xA3;     /* TA Protected */
sfr IAPTRG      = 0xA4;     /* TA Protected */
sfr IAPUEN      = 0xA5;     /* TA Protected */
sfr IAPAL       = 0xA6;
sfr IAPAH       = 0xA7;

/* Interrupt Enable (0xA8) */
sfr IE          = 0xA8;
sbit EA         = IE^7;
sbit EADC       = IE^6;
sbit EBOD       = IE^5;
sbit ES         = IE^4;
sbit ET1        = IE^3;
sbit EX1        = IE^2;
sbit ET0        = IE^1;
sbit EX0        = IE^0;

sfr SADDR       = 0xA9;
sfr WDCON       = 0xAA;     /* TA Protected */
sfr BODCON1     = 0xAB;     /* TA Protected */
sfr P3M1        = 0xAC;
sfr P3M2        = 0xAD;
sfr IAPFD       = 0xAE;
sfr IAPCN       = 0xAF;

/* Port 3 (0xB0) */
sfr P3          = 0xB0;
sbit P37        = P3^7;
sbit P36        = P3^6;
sbit P35        = P3^5;
sbit P34        = P3^4;
sbit P33        = P3^3;
sbit P32        = P3^2;
sbit P31        = P3^1;
sbit P30        = P3^0;

/* Port Mode Registers (0xB1-0xB7) */
sfr P0M1        = 0xB1;
sfr P0M2        = 0xB2;
sfr P1M1        = 0xB3;
sfr P1M2        = 0xB4;
sfr TOE         = 0xB5;
sfr IPH         = 0xB7;

/* Interrupt Priority (0xB8) */
sfr IP          = 0xB8;
sbit PADC       = IP^6;
sbit PBOD       = IP^5;
sbit PS         = IP^4;
sbit PT1        = IP^3;
sbit PX1        = IP^2;
sbit PT0        = IP^1;
sbit PX0        = IP^0;

sfr SADEN       = 0xB9;
sfr SADEN_1     = 0xBA;
sfr SADDR_1     = 0xBB;
sfr I2DAT       = 0xBC;
sfr I2STAT      = 0xBD;
sfr I2CLK       = 0xBE;
sfr I2TOC       = 0xBF;

/* I2C Control (0xC0) */
sfr I2CON       = 0xC0;
sbit I2CEN      = I2CON^6;
sbit STA        = I2CON^5;
sbit STO        = I2CON^4;
sbit SI         = I2CON^3;
sbit AA         = I2CON^2;
sbit I2CPX      = I2CON^0;

sfr I2ADDR      = 0xC1;
sfr ADCRL       = 0xC2;
sfr ADCRH       = 0xC3;
sfr T3CON       = 0xC4;
sfr RL3         = 0xC5;
sfr RH3         = 0xC6;
sfr TA          = 0xC7;

/* Timer 2 Control (0xC8) */
sfr T2CON       = 0xC8;
sbit TF2        = T2CON^7;
sbit TR2        = T2CON^2;
sbit CM_RL2     = T2CON^0;

sfr T2MOD       = 0xC9;
sfr RCMP2L      = 0xCA;
sfr RCMP2H      = 0xCB;
sfr TL2         = 0xCC;
sfr TH2         = 0xCD;
sfr ADCMPL      = 0xCE;
sfr ADCMPH      = 0xCF;

/* Program Status Word (0xD0) */
sfr PSW         = 0xD0;
sbit CY         = PSW^7;
sbit AC         = PSW^6;
sbit F0         = PSW^5;
sbit RS1        = PSW^4;
sbit RS0        = PSW^3;
sbit OV         = PSW^2;
sbit P          = PSW^0;

/* PWM0 Period and Duty (0xD1-0xD7) */
sfr PWM0PH      = 0xD1;
sfr PWM0C0H     = 0xD2;
sfr PWM0C1H     = 0xD3;
sfr PWM0C2H     = 0xD4;
sfr PWM0C3H     = 0xD5;
sfr PNP         = 0xD6;
sfr PWM0FBD     = 0xD7;

/* PWM0 Control (0xD8) */
sfr PWM0CON0    = 0xD8;
sbit PWMRUN     = PWM0CON0^7;
sbit LOAD       = PWM0CON0^6;
sbit PWMF       = PWM0CON0^5;
sbit CLRPWM     = PWM0CON0^4;

sfr PWM0PL      = 0xD9;
sfr PWM0C0L     = 0xDA;
sfr PWM0C1L     = 0xDB;
sfr PWM0C2L     = 0xDC;
sfr PWM0C3L     = 0xDD;
sfr PIOCON0     = 0xDE;
sfr PWM0CON1    = 0xDF;

/* Accumulator (0xE0) */
sfr ACC         = 0xE0;

/* ADC Control (0xE1-0xE7) */
sfr ADCCON1     = 0xE1;
sfr ADCCON2     = 0xE2;
sfr ADCDLY      = 0xE3;
sfr C0L         = 0xE4;
sfr C0H         = 0xE5;
sfr C1L         = 0xE6;
sfr C1H         = 0xE7;

/* ADC Control 0 (0xE8) */
sfr ADCCON0     = 0xE8;
sbit ADCF       = ADCCON0^7;
sbit ADCS       = ADCCON0^6;
sbit ETGSEL1    = ADCCON0^5;
sbit ETGSEL0    = ADCCON0^4;
sbit ADCHS3     = ADCCON0^3;
sbit ADCHS2     = ADCCON0^2;
sbit ADCHS1     = ADCCON0^1;
sbit ADCHS0     = ADCCON0^0;

/* Pin Interrupt (0xE9-0xEF) */
sfr PICON       = 0xE9;
sfr PINEN       = 0xEA;
sfr PIPEN       = 0xEB;
sfr PIF         = 0xEC;
sfr C2L         = 0xED;
sfr C2H         = 0xEE;
sfr EIP         = 0xEF;

/* B Register (0xF0) */
sfr B           = 0xF0;

/* Capture, SPI (0xF1-0xF7) */
sfr CAPCON3     = 0xF1;
sfr CAPCON4     = 0xF2;
sfr SPCR        = 0xF3;
sfr SPSR        = 0xF4;
sfr SPDR        = 0xF5;
sfr AINDIDS0    = 0xF6;
sfr EIPH        = 0xF7;

/* UART1 Control (0xF8) */
sfr SCON_1      = 0xF8;
sbit SM0_1      = SCON_1^7;
sbit FE_1       = SCON_1^7;
sbit SM1_1      = SCON_1^6;
sbit SM2_1      = SCON_1^5;
sbit REN_1      = SCON_1^4;
sbit TB8_1      = SCON_1^3;
sbit RB8_1      = SCON_1^2;
sbit TI_1       = SCON_1^1;
sbit RI_1       = SCON_1^0;

/* PWM0 Dead Time, Power Management (0xF9-0xFF) */
sfr PWM0DTEN    = 0xF9;     /* TA Protected */
sfr PWM0DTCNT   = 0xFA;     /* TA Protected */
sfr PWM0MEN     = 0xFB;
sfr PWM0MD      = 0xFC;
sfr PORDIS      = 0xFD;
sfr EIP1        = 0xFE;
sfr EIPH1       = 0xFF;

/*---------------------------------------------------------------------------
 * Keil C51 SFR Definitions - Page 1
 *---------------------------------------------------------------------------*/
/* Page 1 SFR Aliases - need page switch for access */
#define P3S         P3M1        /* 0xAC Page 1 */
#define P3SR        P3M2        /* 0xAD Page 1 */
#define P0S         P0M1        /* 0xB1 Page 1 */
#define P0SR        P0M2        /* 0xB2 Page 1 */
#define P1S         P1M1        /* 0xB3 Page 1 */
#define P1SR        P1M2        /* 0xB4 Page 1 */
#define PWM0INTC    IPH         /* 0xB7 Page 1 */
#define PWM0C4H     T3CON       /* 0xC4 Page 1 */
#define PWM0C5H     RL3         /* 0xC5 Page 1 */
#define PIOCON1     RH3         /* 0xC6 Page 1 */
#define PWM0C4L     TL2         /* 0xCC Page 1 */
#define PWM0C5L     TH2         /* 0xCD Page 1 */
#define SPCR2       SPCR        /* 0xF3 Page 1 */
#define LVRDIS      EIPH1       /* 0xFF Page 1 */

/*---------------------------------------------------------------------------
 * Keil C51 SFR Definitions - Page 2 (Extended Features)
 *---------------------------------------------------------------------------*/
/* Note: These are accessed at same addresses when SFRS=2 */
/* ADC Bandgap (0x84-0x86) - Page 2 */
#define ADCBAL_ADDR     0x84
#define ADCBAH_ADDR     0x85
#define ADCCON3_ADDR    0x86

/* Port 2 Mode (0x89-0x8F) - Page 2 */
#define P2M1_ADDR       0x89
#define P2M2_ADDR       0x8A
#define P2SR_ADDR       0x8B
#define P2S_P2_ADDR     0x8C
#define ADCSN_ADDR      0x8D
#define ADCCN_ADDR      0x8E
#define ADCSR_ADDR      0x8F

/* Pull-up Control (0x92-0x97) - Page 2 */
#define P0UP_ADDR       0x92
#define P1UP_ADDR       0x93
#define P2UP_ADDR       0x94
#define P3UP_ADDR       0x95
#define RWKH_ADDR       0x97

/* Additional ADC and Pull-down (0x99-0x9D) - Page 2 */
#define AINDIDS1_ADDR   0x99
#define P0DW_ADDR       0x9A
#define P1DW_ADDR       0x9B
#define P2DW_ADDR       0x9C
#define P3DW_ADDR       0x9D

/* AUXR2-AUXR8 (0xA1-0xA7) - Page 2 */
#define AUXR2_ADDR      0xA1
#define AUXR3_ADDR      0xA2
#define AUXR4_ADDR      0xA3
#define AUXR5_ADDR      0xA4
#define AUXR6_ADDR      0xA5
#define AUXR7_ADDR      0xA6
#define AUXR8_ADDR      0xA7

/* PWM1 Registers (0xA9-0xB7) - Page 2 */
#define PWM1PH_ADDR     0xA9
#define PWM1C0H_ADDR    0xAA
#define PWM1C1H_ADDR    0xAB
#define PWM1MD_ADDR     0xAC
#define PWM1MEN_ADDR    0xAD
#define PWM1PL_ADDR     0xB1
#define PWM1C0L_ADDR    0xB2
#define PWM1C1L_ADDR    0xB3
#define PWM1CON0_ADDR   0xB4
#define PWM1CON1_ADDR   0xB5
#define PWM1INTC_ADDR   0xB6
#define PIOCON2_ADDR    0xB7

/* PWM2 Registers (0xB9-0xC6) - Page 2 */
#define PWM2PH_ADDR     0xB9
#define PWM2C0H_ADDR    0xBA
#define PWM2C1H_ADDR    0xBB
#define PWM2MD_ADDR     0xBC
#define PWM2MEN_ADDR    0xBD
#define PWM2PL_ADDR     0xC1
#define PWM2C0L_ADDR    0xC2
#define PWM2C1L_ADDR    0xC3
#define PWM2CON0_ADDR   0xC4
#define PWM2CON1_ADDR   0xC5
#define PWM2INTC_ADDR   0xC6

/* PWM3 Registers (0xC9-0xD7) - Page 2 */
#define PWM3PH_ADDR     0xC9
#define PWM3C0H_ADDR    0xCA
#define PWM3C1H_ADDR    0xCB
#define PWM3MD_ADDR     0xCC
#define PWM3MEN_ADDR    0xCD
#define EIP2_ADDR       0xCE
#define EIPH2_ADDR      0xCF
#define PWM3PL_ADDR     0xD1
#define PWM3C0L_ADDR    0xD2
#define PWM3C1L_ADDR    0xD3
#define PWM3CON0_ADDR   0xD4
#define PWM3CON1_ADDR   0xD5
#define PWM3INTC_ADDR   0xD6
#define XTLCON_ADDR     0xD7

/* Smart Card Registers (0xD9-0xEF) - Page 2 */
#define SC0DR_ADDR      0xD9
#define SC0EGT_ADDR     0xDA
#define SC0ETURD0_ADDR  0xDB
#define SC0ETURD1_ADDR  0xDC
#define SC0IE_ADDR      0xDD
#define SC0IS_ADDR      0xDE
#define SC0TSR_ADDR     0xDF
#define SC1DR_ADDR      0xE1
#define SC1EGT_ADDR     0xE2
#define SC1ETURD0_ADDR  0xE3
#define SC1ETURD1_ADDR  0xE4
#define SC1IE_ADDR      0xE5
#define SC1IS_ADDR      0xE6
#define SC1TSR_ADDR     0xE7
#define SC2DR_ADDR      0xE9
#define SC2EGT_ADDR     0xEA
#define SC2ETURD0_ADDR  0xEB
#define SC2ETURD1_ADDR  0xEC
#define SC2IE_ADDR      0xED
#define SC2IS_ADDR      0xEE
#define SC2TSR_ADDR     0xEF

/* SC Control and PIPS (0xF1-0xFF) - Page 2 */
#define SC0CR0_ADDR     0xF1
#define SC0CR1_ADDR     0xF2
#define SC1CR0_ADDR     0xF3
#define SC1CR1_ADDR     0xF4
#define SC2CR0_ADDR     0xF5
#define SC2CR1_ADDR     0xF6
#define PIPS7_ADDR      0xF7
#define PIPS0_ADDR      0xF9
#define PIPS1_ADDR      0xFA
#define PIPS2_ADDR      0xFB
#define PIPS3_ADDR      0xFC
#define PIPS4_ADDR      0xFD
#define PIPS5_ADDR      0xFE
#define PIPS6_ADDR      0xFF

/* Legacy aliases for backward compatibility */
#define PWMPH       PWM0PH
#define PWM0H       PWM0C0H
#define PWM1H       PWM0C1H
#define PWM2H       PWM0C2H
#define PWM3H       PWM0C3H
#define FBD         PWM0FBD
#define PWMCON0     PWM0CON0
#define PWMPL       PWM0PL
#define PWM0L       PWM0C0L
#define PWM1L       PWM0C1L
#define PWM2L       PWM0C2L
#define PWM3L       PWM0C3L
#define PWMCON1     PWM0CON1
#define PDTEN       PWM0DTEN
#define PDTCNT      PWM0DTCNT
#define PMEN        PWM0MEN
#define PMD         PWM0MD
#define AINDIDS     AINDIDS0
#define RWK         RWKL

#elif defined(COMPILER_IAR)

/*---------------------------------------------------------------------------
 * IAR EW8051 SFR Definitions - Page 0
 *---------------------------------------------------------------------------*/
__sfr __no_init volatile unsigned char P0     @ 0x80;
__sfr __no_init volatile unsigned char SP     @ 0x81;
__sfr __no_init volatile unsigned char DPL    @ 0x82;
__sfr __no_init volatile unsigned char DPH    @ 0x83;
__sfr __no_init volatile unsigned char RCTRIM0 @ 0x84;
__sfr __no_init volatile unsigned char RCTRIM1 @ 0x85;
__sfr __no_init volatile unsigned char RWKL   @ 0x86;
__sfr __no_init volatile unsigned char PCON   @ 0x87;

__sfr __no_init volatile unsigned char TCON   @ 0x88;
__sfr __no_init volatile unsigned char TMOD   @ 0x89;
__sfr __no_init volatile unsigned char TL0    @ 0x8A;
__sfr __no_init volatile unsigned char TL1    @ 0x8B;
__sfr __no_init volatile unsigned char TH0    @ 0x8C;
__sfr __no_init volatile unsigned char TH1    @ 0x8D;
__sfr __no_init volatile unsigned char CKCON  @ 0x8E;
__sfr __no_init volatile unsigned char WKCON  @ 0x8F;

__sfr __no_init volatile unsigned char P1     @ 0x90;
__sfr __no_init volatile unsigned char SFRS   @ 0x91;
__sfr __no_init volatile unsigned char CAPCON0 @ 0x92;
__sfr __no_init volatile unsigned char CAPCON1 @ 0x93;
__sfr __no_init volatile unsigned char CAPCON2 @ 0x94;
__sfr __no_init volatile unsigned char CKDIV  @ 0x95;
__sfr __no_init volatile unsigned char CKSWT  @ 0x96;
__sfr __no_init volatile unsigned char CKEN   @ 0x97;

__sfr __no_init volatile unsigned char SCON   @ 0x98;
__sfr __no_init volatile unsigned char SBUF   @ 0x99;
__sfr __no_init volatile unsigned char SBUF_1 @ 0x9A;
__sfr __no_init volatile unsigned char EIE    @ 0x9B;
__sfr __no_init volatile unsigned char EIE1   @ 0x9C;
__sfr __no_init volatile unsigned char CHPCON @ 0x9F;

__sfr __no_init volatile unsigned char P2     @ 0xA0;
__sfr __no_init volatile unsigned char AUXR1  @ 0xA2;
__sfr __no_init volatile unsigned char BODCON0 @ 0xA3;
__sfr __no_init volatile unsigned char IAPTRG @ 0xA4;
__sfr __no_init volatile unsigned char IAPUEN @ 0xA5;
__sfr __no_init volatile unsigned char IAPAL  @ 0xA6;
__sfr __no_init volatile unsigned char IAPAH  @ 0xA7;

__sfr __no_init volatile unsigned char IE     @ 0xA8;
__sfr __no_init volatile unsigned char SADDR  @ 0xA9;
__sfr __no_init volatile unsigned char WDCON  @ 0xAA;
__sfr __no_init volatile unsigned char BODCON1 @ 0xAB;
__sfr __no_init volatile unsigned char P3M1   @ 0xAC;
__sfr __no_init volatile unsigned char P3M2   @ 0xAD;
__sfr __no_init volatile unsigned char IAPFD  @ 0xAE;
__sfr __no_init volatile unsigned char IAPCN  @ 0xAF;

__sfr __no_init volatile unsigned char P3     @ 0xB0;
__sfr __no_init volatile unsigned char P0M1   @ 0xB1;
__sfr __no_init volatile unsigned char P0M2   @ 0xB2;
__sfr __no_init volatile unsigned char P1M1   @ 0xB3;
__sfr __no_init volatile unsigned char P1M2   @ 0xB4;
__sfr __no_init volatile unsigned char TOE    @ 0xB5;
__sfr __no_init volatile unsigned char IPH    @ 0xB7;

__sfr __no_init volatile unsigned char IP     @ 0xB8;
__sfr __no_init volatile unsigned char SADEN  @ 0xB9;
__sfr __no_init volatile unsigned char SADEN_1 @ 0xBA;
__sfr __no_init volatile unsigned char SADDR_1 @ 0xBB;
__sfr __no_init volatile unsigned char I2DAT  @ 0xBC;
__sfr __no_init volatile unsigned char I2STAT @ 0xBD;
__sfr __no_init volatile unsigned char I2CLK  @ 0xBE;
__sfr __no_init volatile unsigned char I2TOC  @ 0xBF;

__sfr __no_init volatile unsigned char I2CON  @ 0xC0;
__sfr __no_init volatile unsigned char I2ADDR @ 0xC1;
__sfr __no_init volatile unsigned char ADCRL  @ 0xC2;
__sfr __no_init volatile unsigned char ADCRH  @ 0xC3;
__sfr __no_init volatile unsigned char T3CON  @ 0xC4;
__sfr __no_init volatile unsigned char RL3    @ 0xC5;
__sfr __no_init volatile unsigned char RH3    @ 0xC6;
__sfr __no_init volatile unsigned char TA     @ 0xC7;

__sfr __no_init volatile unsigned char T2CON  @ 0xC8;
__sfr __no_init volatile unsigned char T2MOD  @ 0xC9;
__sfr __no_init volatile unsigned char RCMP2L @ 0xCA;
__sfr __no_init volatile unsigned char RCMP2H @ 0xCB;
__sfr __no_init volatile unsigned char TL2    @ 0xCC;
__sfr __no_init volatile unsigned char TH2    @ 0xCD;
__sfr __no_init volatile unsigned char ADCMPL @ 0xCE;
__sfr __no_init volatile unsigned char ADCMPH @ 0xCF;

__sfr __no_init volatile unsigned char PSW    @ 0xD0;
__sfr __no_init volatile unsigned char PWM0PH @ 0xD1;
__sfr __no_init volatile unsigned char PWM0C0H @ 0xD2;
__sfr __no_init volatile unsigned char PWM0C1H @ 0xD3;
__sfr __no_init volatile unsigned char PWM0C2H @ 0xD4;
__sfr __no_init volatile unsigned char PWM0C3H @ 0xD5;
__sfr __no_init volatile unsigned char PNP    @ 0xD6;
__sfr __no_init volatile unsigned char PWM0FBD @ 0xD7;

__sfr __no_init volatile unsigned char PWM0CON0 @ 0xD8;
__sfr __no_init volatile unsigned char PWM0PL @ 0xD9;
__sfr __no_init volatile unsigned char PWM0C0L @ 0xDA;
__sfr __no_init volatile unsigned char PWM0C1L @ 0xDB;
__sfr __no_init volatile unsigned char PWM0C2L @ 0xDC;
__sfr __no_init volatile unsigned char PWM0C3L @ 0xDD;
__sfr __no_init volatile unsigned char PIOCON0 @ 0xDE;
__sfr __no_init volatile unsigned char PWM0CON1 @ 0xDF;

__sfr __no_init volatile unsigned char ACC    @ 0xE0;
__sfr __no_init volatile unsigned char ADCCON1 @ 0xE1;
__sfr __no_init volatile unsigned char ADCCON2 @ 0xE2;
__sfr __no_init volatile unsigned char ADCDLY @ 0xE3;
__sfr __no_init volatile unsigned char C0L    @ 0xE4;
__sfr __no_init volatile unsigned char C0H    @ 0xE5;
__sfr __no_init volatile unsigned char C1L    @ 0xE6;
__sfr __no_init volatile unsigned char C1H    @ 0xE7;

__sfr __no_init volatile unsigned char ADCCON0 @ 0xE8;
__sfr __no_init volatile unsigned char PICON  @ 0xE9;
__sfr __no_init volatile unsigned char PINEN  @ 0xEA;
__sfr __no_init volatile unsigned char PIPEN  @ 0xEB;
__sfr __no_init volatile unsigned char PIF    @ 0xEC;
__sfr __no_init volatile unsigned char C2L    @ 0xED;
__sfr __no_init volatile unsigned char C2H    @ 0xEE;
__sfr __no_init volatile unsigned char EIP    @ 0xEF;

__sfr __no_init volatile unsigned char B      @ 0xF0;
__sfr __no_init volatile unsigned char CAPCON3 @ 0xF1;
__sfr __no_init volatile unsigned char CAPCON4 @ 0xF2;
__sfr __no_init volatile unsigned char SPCR   @ 0xF3;
__sfr __no_init volatile unsigned char SPSR   @ 0xF4;
__sfr __no_init volatile unsigned char SPDR   @ 0xF5;
__sfr __no_init volatile unsigned char AINDIDS0 @ 0xF6;
__sfr __no_init volatile unsigned char EIPH   @ 0xF7;

__sfr __no_init volatile unsigned char SCON_1 @ 0xF8;
__sfr __no_init volatile unsigned char PWM0DTEN @ 0xF9;
__sfr __no_init volatile unsigned char PWM0DTCNT @ 0xFA;
__sfr __no_init volatile unsigned char PWM0MEN @ 0xFB;
__sfr __no_init volatile unsigned char PWM0MD @ 0xFC;
__sfr __no_init volatile unsigned char PORDIS @ 0xFD;
__sfr __no_init volatile unsigned char EIP1   @ 0xFE;
__sfr __no_init volatile unsigned char EIPH1  @ 0xFF;

#elif defined(COMPILER_SDCC)

/*---------------------------------------------------------------------------
 * SDCC SFR Definitions - Page 0
 *---------------------------------------------------------------------------*/
__sfr __at(0x80) P0;
__sfr __at(0x81) SP;
__sfr __at(0x82) DPL;
__sfr __at(0x83) DPH;
__sfr __at(0x84) RCTRIM0;
__sfr __at(0x85) RCTRIM1;
__sfr __at(0x86) RWKL;
__sfr __at(0x87) PCON;

__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8A) TL0;
__sfr __at(0x8B) TL1;
__sfr __at(0x8C) TH0;
__sfr __at(0x8D) TH1;
__sfr __at(0x8E) CKCON;
__sfr __at(0x8F) WKCON;

__sfr __at(0x90) P1;
__sfr __at(0x91) SFRS;
__sfr __at(0x92) CAPCON0;
__sfr __at(0x93) CAPCON1;
__sfr __at(0x94) CAPCON2;
__sfr __at(0x95) CKDIV;
__sfr __at(0x96) CKSWT;
__sfr __at(0x97) CKEN;

__sfr __at(0x98) SCON;
__sfr __at(0x99) SBUF;
__sfr __at(0x9A) SBUF_1;
__sfr __at(0x9B) EIE;
__sfr __at(0x9C) EIE1;
__sfr __at(0x9F) CHPCON;

__sfr __at(0xA0) P2;
__sfr __at(0xA2) AUXR1;
__sfr __at(0xA3) BODCON0;
__sfr __at(0xA4) IAPTRG;
__sfr __at(0xA5) IAPUEN;
__sfr __at(0xA6) IAPAL;
__sfr __at(0xA7) IAPAH;

__sfr __at(0xA8) IE;
__sfr __at(0xA9) SADDR;
__sfr __at(0xAA) WDCON;
__sfr __at(0xAB) BODCON1;
__sfr __at(0xAC) P3M1;
__sfr __at(0xAD) P3M2;
__sfr __at(0xAE) IAPFD;
__sfr __at(0xAF) IAPCN;

__sfr __at(0xB0) P3;
__sfr __at(0xB1) P0M1;
__sfr __at(0xB2) P0M2;
__sfr __at(0xB3) P1M1;
__sfr __at(0xB4) P1M2;
__sfr __at(0xB5) TOE;
__sfr __at(0xB7) IPH;

__sfr __at(0xB8) IP;
__sfr __at(0xB9) SADEN;
__sfr __at(0xBA) SADEN_1;
__sfr __at(0xBB) SADDR_1;
__sfr __at(0xBC) I2DAT;
__sfr __at(0xBD) I2STAT;
__sfr __at(0xBE) I2CLK;
__sfr __at(0xBF) I2TOC;

__sfr __at(0xC0) I2CON;
__sfr __at(0xC1) I2ADDR;
__sfr __at(0xC2) ADCRL;
__sfr __at(0xC3) ADCRH;
__sfr __at(0xC4) T3CON;
__sfr __at(0xC5) RL3;
__sfr __at(0xC6) RH3;
__sfr __at(0xC7) TA;

__sfr __at(0xC8) T2CON;
__sfr __at(0xC9) T2MOD;
__sfr __at(0xCA) RCMP2L;
__sfr __at(0xCB) RCMP2H;
__sfr __at(0xCC) TL2;
__sfr __at(0xCD) TH2;
__sfr __at(0xCE) ADCMPL;
__sfr __at(0xCF) ADCMPH;

__sfr __at(0xD0) PSW;
__sfr __at(0xD1) PWM0PH;
__sfr __at(0xD2) PWM0C0H;
__sfr __at(0xD3) PWM0C1H;
__sfr __at(0xD4) PWM0C2H;
__sfr __at(0xD5) PWM0C3H;
__sfr __at(0xD6) PNP;
__sfr __at(0xD7) PWM0FBD;

__sfr __at(0xD8) PWM0CON0;
__sfr __at(0xD9) PWM0PL;
__sfr __at(0xDA) PWM0C0L;
__sfr __at(0xDB) PWM0C1L;
__sfr __at(0xDC) PWM0C2L;
__sfr __at(0xDD) PWM0C3L;
__sfr __at(0xDE) PIOCON0;
__sfr __at(0xDF) PWM0CON1;

__sfr __at(0xE0) ACC;
__sfr __at(0xE1) ADCCON1;
__sfr __at(0xE2) ADCCON2;
__sfr __at(0xE3) ADCDLY;
__sfr __at(0xE4) C0L;
__sfr __at(0xE5) C0H;
__sfr __at(0xE6) C1L;
__sfr __at(0xE7) C1H;

__sfr __at(0xE8) ADCCON0;
__sfr __at(0xE9) PICON;
__sfr __at(0xEA) PINEN;
__sfr __at(0xEB) PIPEN;
__sfr __at(0xEC) PIF;
__sfr __at(0xED) C2L;
__sfr __at(0xEE) C2H;
__sfr __at(0xEF) EIP;

__sfr __at(0xF0) B;
__sfr __at(0xF1) CAPCON3;
__sfr __at(0xF2) CAPCON4;
__sfr __at(0xF3) SPCR;
__sfr __at(0xF4) SPSR;
__sfr __at(0xF5) SPDR;
__sfr __at(0xF6) AINDIDS0;
__sfr __at(0xF7) EIPH;

__sfr __at(0xF8) SCON_1;
__sfr __at(0xF9) PWM0DTEN;
__sfr __at(0xFA) PWM0DTCNT;
__sfr __at(0xFB) PWM0MEN;
__sfr __at(0xFC) PWM0MD;
__sfr __at(0xFD) PORDIS;
__sfr __at(0xFE) EIP1;
__sfr __at(0xFF) EIPH1;

/* SDCC sbit definitions */
__sbit __at(0x88+7) TF1;
__sbit __at(0x88+6) TR1;
__sbit __at(0x88+5) TF0;
__sbit __at(0x88+4) TR0;
__sbit __at(0x88+3) IE1;
__sbit __at(0x88+2) IT1;
__sbit __at(0x88+1) IE0;
__sbit __at(0x88+0) IT0;

__sbit __at(0xA8+7) EA;
__sbit __at(0xA8+6) EADC;
__sbit __at(0xA8+5) EBOD;
__sbit __at(0xA8+4) ES;
__sbit __at(0xA8+3) ET1;
__sbit __at(0xA8+2) EX1;
__sbit __at(0xA8+1) ET0;
__sbit __at(0xA8+0) EX0;

__sbit __at(0x98+7) SM0;
__sbit __at(0x98+6) SM1;
__sbit __at(0x98+5) SM2;
__sbit __at(0x98+4) REN;
__sbit __at(0x98+3) TB8;
__sbit __at(0x98+2) RB8;
__sbit __at(0x98+1) TI;
__sbit __at(0x98+0) RI;

__sbit __at(0xE8+7) ADCF;
__sbit __at(0xE8+6) ADCS;

__sbit __at(0xD8+7) PWMRUN;
__sbit __at(0xD8+6) LOAD;
__sbit __at(0xD8+5) PWMF;
__sbit __at(0xD8+4) CLRPWM;

__sbit __at(0xC0+6) I2CEN;
__sbit __at(0xC0+5) STA;
__sbit __at(0xC0+4) STO;
__sbit __at(0xC0+3) SI;
__sbit __at(0xC0+2) AA;

__sbit __at(0xC8+7) TF2;
__sbit __at(0xC8+2) TR2;

__sbit __at(0xF8+1) TI_1;
__sbit __at(0xF8+0) RI_1;

/* Legacy aliases */
#define PWMPH       PWM0PH
#define PWM0H       PWM0C0H
#define PWM1H       PWM0C1H
#define PWM2H       PWM0C2H
#define PWM3H       PWM0C3H
#define FBD         PWM0FBD
#define PWMCON0     PWM0CON0
#define PWMPL       PWM0PL
#define PWM0L       PWM0C0L
#define PWM1L       PWM0C1L
#define PWM2L       PWM0C2L
#define PWM3L       PWM0C3L
#define PWMCON1     PWM0CON1
#define PDTEN       PWM0DTEN
#define PDTCNT      PWM0DTCNT
#define PMEN        PWM0MEN
#define PMD         PWM0MD
#define AINDIDS     AINDIDS0
#define RWK         RWKL

#else
    /* Mock SFRs for unit testing - defined in mock files */
#endif

#ifdef __cplusplus
}
#endif

#endif /* MS51_SFR_32K_H */
