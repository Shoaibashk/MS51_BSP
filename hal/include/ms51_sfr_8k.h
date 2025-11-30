/*===========================================================================
 * @file    ms51_sfr_8k.h
 * @brief   MS51 8KB Variant SFR Definitions
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Special Function Register definitions for MS51 8KB variants:
 *          - MS51DA9AE
 *          - MS51BA9AE
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_SFR_8K_H
#define MS51_SFR_8K_H

#include "ms51_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Device Information
 *===========================================================================*/
#define MS51_FLASH_SIZE         8192    /* 8KB APROM */
#define MS51_LDROM_SIZE         2048    /* 2KB LDROM */
#define MS51_IRAM_SIZE          256     /* 256 bytes IRAM */
#define MS51_XRAM_SIZE          768     /* 768 bytes XRAM */
#define MS51_DATAFLASH_SIZE     2048    /* 2KB Data Flash */
#define MS51_PAGE_SIZE          128     /* Flash page size */

/*===========================================================================
 * SFR Definitions - Compiler Specific
 *===========================================================================*/

#if defined(COMPILER_KEIL)

/*---------------------------------------------------------------------------
 * Keil C51 SFR Definitions
 *---------------------------------------------------------------------------*/

/* Port 0 (0x80) */
sfr P0          = 0x80;
sbit P07        = P0^7;
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
sfr RWK         = 0x86;
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
sbit P15        = P1^5;
sbit P14        = P1^4;
sbit P13        = P1^3;
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
sbit P30        = P3^0;

/* Port Mode Registers (0xB1-0xB7) */
sfr P0M1        = 0xB1;
sfr P0M2        = 0xB2;
sfr P1M1        = 0xB3;
sfr P1M2        = 0xB4;
sfr P2S         = 0xB5;
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

/* PWM Period and Duty (0xD1-0xD7) */
sfr PWMPH       = 0xD1;
sfr PWM0H       = 0xD2;
sfr PWM1H       = 0xD3;
sfr PWM2H       = 0xD4;
sfr PWM3H       = 0xD5;
sfr PNP         = 0xD6;
sfr FBD         = 0xD7;

/* PWM Control (0xD8) */
sfr PWMCON0     = 0xD8;
sbit PWMRUN     = PWMCON0^7;
sbit LOAD       = PWMCON0^6;
sbit PWMF       = PWMCON0^5;
sbit CLRPWM     = PWMCON0^4;

sfr PWMPL       = 0xD9;
sfr PWM0L       = 0xDA;
sfr PWM1L       = 0xDB;
sfr PWM2L       = 0xDC;
sfr PWM3L       = 0xDD;
sfr PIOCON0     = 0xDE;
sfr PWMCON1     = 0xDF;

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
sfr AINDIDS     = 0xF6;
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

/* Dead Time, Power Management (0xF9-0xFF) */
sfr PDTEN       = 0xF9;     /* TA Protected */
sfr PDTCNT      = 0xFA;     /* TA Protected */
sfr PMEN        = 0xFB;
sfr PMD         = 0xFC;
sfr EIP1        = 0xFE;
sfr EIPH1       = 0xFF;

/* Page 1 SFR Aliases */
#define P3S         P3M1    /* Page 1 */
#define P3SR        P3M2    /* Page 1 */
#define P0S         P0M1    /* Page 1 */
#define P0SR        P0M2    /* Page 1 */
#define P1S         P1M1    /* Page 1 */
#define P1SR        P1M2    /* Page 1 */
#define PWM4H       T3CON   /* Page 1 */
#define PWM5H       RL3     /* Page 1 */
#define PIOCON1     RH3     /* Page 1 */
#define PWM4L       TL2     /* Page 1 */
#define PWM5L       TH2     /* Page 1 */
#define PWMINTC     IPH     /* Page 1 */
#define SPCR2       SPCR    /* Page 1 */

#elif defined(COMPILER_IAR)

/*---------------------------------------------------------------------------
 * IAR EW8051 SFR Definitions
 *---------------------------------------------------------------------------*/
__sfr __no_init volatile unsigned char P0     @ 0x80;
__sfr __no_init volatile unsigned char SP     @ 0x81;
__sfr __no_init volatile unsigned char DPL    @ 0x82;
__sfr __no_init volatile unsigned char DPH    @ 0x83;
__sfr __no_init volatile unsigned char RCTRIM0 @ 0x84;
__sfr __no_init volatile unsigned char RCTRIM1 @ 0x85;
__sfr __no_init volatile unsigned char RWK    @ 0x86;
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
__sfr __no_init volatile unsigned char P2S    @ 0xB5;
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
__sfr __no_init volatile unsigned char PWMPH  @ 0xD1;
__sfr __no_init volatile unsigned char PWM0H  @ 0xD2;
__sfr __no_init volatile unsigned char PWM1H  @ 0xD3;
__sfr __no_init volatile unsigned char PWM2H  @ 0xD4;
__sfr __no_init volatile unsigned char PWM3H  @ 0xD5;
__sfr __no_init volatile unsigned char PNP    @ 0xD6;
__sfr __no_init volatile unsigned char FBD    @ 0xD7;

__sfr __no_init volatile unsigned char PWMCON0 @ 0xD8;
__sfr __no_init volatile unsigned char PWMPL  @ 0xD9;
__sfr __no_init volatile unsigned char PWM0L  @ 0xDA;
__sfr __no_init volatile unsigned char PWM1L  @ 0xDB;
__sfr __no_init volatile unsigned char PWM2L  @ 0xDC;
__sfr __no_init volatile unsigned char PWM3L  @ 0xDD;
__sfr __no_init volatile unsigned char PIOCON0 @ 0xDE;
__sfr __no_init volatile unsigned char PWMCON1 @ 0xDF;

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
__sfr __no_init volatile unsigned char AINDIDS @ 0xF6;
__sfr __no_init volatile unsigned char EIPH   @ 0xF7;

__sfr __no_init volatile unsigned char SCON_1 @ 0xF8;
__sfr __no_init volatile unsigned char PDTEN  @ 0xF9;
__sfr __no_init volatile unsigned char PDTCNT @ 0xFA;
__sfr __no_init volatile unsigned char PMEN   @ 0xFB;
__sfr __no_init volatile unsigned char PMD    @ 0xFC;
__sfr __no_init volatile unsigned char EIP1   @ 0xFE;
__sfr __no_init volatile unsigned char EIPH1  @ 0xFF;

#elif defined(COMPILER_SDCC)

/*---------------------------------------------------------------------------
 * SDCC SFR Definitions
 *---------------------------------------------------------------------------*/
__sfr __at(0x80) P0;
__sfr __at(0x81) SP;
__sfr __at(0x82) DPL;
__sfr __at(0x83) DPH;
__sfr __at(0x84) RCTRIM0;
__sfr __at(0x85) RCTRIM1;
__sfr __at(0x86) RWK;
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
__sfr __at(0xB5) P2S;
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
__sfr __at(0xD1) PWMPH;
__sfr __at(0xD2) PWM0H;
__sfr __at(0xD3) PWM1H;
__sfr __at(0xD4) PWM2H;
__sfr __at(0xD5) PWM3H;
__sfr __at(0xD6) PNP;
__sfr __at(0xD7) FBD;

__sfr __at(0xD8) PWMCON0;
__sfr __at(0xD9) PWMPL;
__sfr __at(0xDA) PWM0L;
__sfr __at(0xDB) PWM1L;
__sfr __at(0xDC) PWM2L;
__sfr __at(0xDD) PWM3L;
__sfr __at(0xDE) PIOCON0;
__sfr __at(0xDF) PWMCON1;

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
__sfr __at(0xF6) AINDIDS;
__sfr __at(0xF7) EIPH;

__sfr __at(0xF8) SCON_1;
__sfr __at(0xF9) PDTEN;
__sfr __at(0xFA) PDTCNT;
__sfr __at(0xFB) PMEN;
__sfr __at(0xFC) PMD;
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

#else
    /* Mock SFRs for unit testing - defined in mock files */
#endif

#ifdef __cplusplus
}
#endif

#endif /* MS51_SFR_8K_H */
