/*===========================================================================
 * @file    ms51_sfr_bits.h
 * @brief   MS51 SFR Bit Manipulation Macros
 * @version 2.0.0
 * @date    2024-11-30
 * 
 * @details Provides atomic set/clear macros for all SFR bits.
 *          Includes TA-protected register access sequences.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2024 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_SFR_BITS_H
#define MS51_SFR_BITS_H

#include "ms51_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Global Variables for TA-Protected Access
 *===========================================================================*/
extern BIT BIT_TMP;
extern DATA_MEM uint8_t TA_REG_TMP;
extern DATA_MEM uint8_t BYTE_TMP;
extern DATA_MEM uint8_t SFRS_TMP;

/*===========================================================================
 * PCON Register Bits (0x87)
 *===========================================================================*/
#define set_PCON_SMOD       (PCON |= 0x80)
#define clr_PCON_SMOD       (PCON &= ~0x80)
#define set_PCON_SMOD0      (PCON |= 0x40)
#define clr_PCON_SMOD0      (PCON &= ~0x40)
#define set_PCON_POF        (PCON |= 0x10)
#define clr_PCON_POF        (PCON &= ~0x10)
#define set_PCON_GF1        (PCON |= 0x08)
#define clr_PCON_GF1        (PCON &= ~0x08)
#define set_PCON_GF0        (PCON |= 0x04)
#define clr_PCON_GF0        (PCON &= ~0x04)
#define set_PCON_PD         (PCON |= 0x02)
#define clr_PCON_PD         (PCON &= ~0x02)
#define set_PCON_IDLE       (PCON |= 0x01)
#define clr_PCON_IDLE       (PCON &= ~0x01)

/*===========================================================================
 * TCON Register Bits (0x88) - bit-addressable
 *===========================================================================*/
#define set_TCON_TF1        TF1 = 1
#define clr_TCON_TF1        TF1 = 0
#define set_TCON_TR1        TR1 = 1
#define clr_TCON_TR1        TR1 = 0
#define set_TCON_TF0        TF0 = 1
#define clr_TCON_TF0        TF0 = 0
#define set_TCON_TR0        TR0 = 1
#define clr_TCON_TR0        TR0 = 0
#define set_TCON_IE1        IE1 = 1
#define clr_TCON_IE1        IE1 = 0
#define set_TCON_IT1        IT1 = 1
#define clr_TCON_IT1        IT1 = 0
#define set_TCON_IE0        IE0 = 1
#define clr_TCON_IE0        IE0 = 0
#define set_TCON_IT0        IT0 = 1
#define clr_TCON_IT0        IT0 = 0

/*===========================================================================
 * TMOD Register Bits (0x89)
 *===========================================================================*/
#define set_TMOD_GATE1      (TMOD |= 0x80)
#define clr_TMOD_GATE1      (TMOD &= ~0x80)
#define set_TMOD_CT1        (TMOD |= 0x40)
#define clr_TMOD_CT1        (TMOD &= ~0x40)
#define set_TMOD_M11        (TMOD |= 0x20)
#define clr_TMOD_M11        (TMOD &= ~0x20)
#define set_TMOD_M01        (TMOD |= 0x10)
#define clr_TMOD_M01        (TMOD &= ~0x10)
#define set_TMOD_GATE0      (TMOD |= 0x08)
#define clr_TMOD_GATE0      (TMOD &= ~0x08)
#define set_TMOD_CT0        (TMOD |= 0x04)
#define clr_TMOD_CT0        (TMOD &= ~0x04)
#define set_TMOD_M10        (TMOD |= 0x02)
#define clr_TMOD_M10        (TMOD &= ~0x02)
#define set_TMOD_M00        (TMOD |= 0x01)
#define clr_TMOD_M00        (TMOD &= ~0x01)

/*===========================================================================
 * CKCON Register Bits (0x8E)
 *===========================================================================*/
#define set_CKCON_T1M       (CKCON |= 0x10)
#define clr_CKCON_T1M       (CKCON &= ~0x10)
#define set_CKCON_T0M       (CKCON |= 0x08)
#define clr_CKCON_T0M       (CKCON &= ~0x08)
#define set_CKCON_PWMCKS    (CKCON |= 0x04)
#define clr_CKCON_PWMCKS    (CKCON &= ~0x04)

/*===========================================================================
 * SFRS Register Bits (0x91) - TA Protected
 *===========================================================================*/
#define set_SFRS_SFRPAGE    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; SFRS |= 0x01; EA = BIT_TMP; } while(0)
#define clr_SFRS_SFRPAGE    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; SFRS &= ~0x01; EA = BIT_TMP; } while(0)

/*===========================================================================
 * SCON Register Bits (0x98) - bit-addressable
 *===========================================================================*/
#define set_SCON_SM0        SM0 = 1
#define clr_SCON_SM0        SM0 = 0
#define set_SCON_SM1        SM1 = 1
#define clr_SCON_SM1        SM1 = 0
#define set_SCON_SM2        SM2 = 1
#define clr_SCON_SM2        SM2 = 0
#define set_SCON_REN        REN = 1
#define clr_SCON_REN        REN = 0
#define set_SCON_TB8        TB8 = 1
#define clr_SCON_TB8        TB8 = 0
#define set_SCON_RB8        RB8 = 1
#define clr_SCON_RB8        RB8 = 0
#define set_SCON_TI         TI = 1
#define clr_SCON_TI         TI = 0
#define set_SCON_RI         RI = 1
#define clr_SCON_RI         RI = 0

/*===========================================================================
 * CHPCON Register Bits (0x9F) - TA Protected
 *===========================================================================*/
#define set_CHPCON_SWRST    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; CHPCON |= 0x80; EA = BIT_TMP; } while(0)
#define set_CHPCON_BS       do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; CHPCON |= 0x02; EA = BIT_TMP; } while(0)
#define clr_CHPCON_BS       do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; CHPCON &= ~0x02; EA = BIT_TMP; } while(0)
#define set_CHPCON_IAPEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; CHPCON |= 0x01; EA = BIT_TMP; } while(0)
#define clr_CHPCON_IAPEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; CHPCON &= ~0x01; EA = BIT_TMP; } while(0)

/*===========================================================================
 * BODCON0 Register Bits (0xA3) - TA Protected
 *===========================================================================*/
#define set_BODCON0_BODEN   do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; BODCON0 |= 0x80; EA = BIT_TMP; } while(0)
#define clr_BODCON0_BODEN   do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; BODCON0 &= ~0x80; EA = BIT_TMP; } while(0)
#define set_BODCON0_BODRST  do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; BODCON0 |= 0x04; EA = BIT_TMP; } while(0)
#define clr_BODCON0_BODRST  do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; BODCON0 &= ~0x04; EA = BIT_TMP; } while(0)
#define set_BODCON0_BOF     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; BODCON0 |= 0x08; EA = BIT_TMP; } while(0)
#define clr_BODCON0_BOF     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; BODCON0 &= ~0x08; EA = BIT_TMP; } while(0)

/*===========================================================================
 * IAPUEN Register Bits (0xA5) - TA Protected
 *===========================================================================*/
#define set_IAPUEN_APUEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN |= 0x01; EA = BIT_TMP; } while(0)
#define clr_IAPUEN_APUEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN &= ~0x01; EA = BIT_TMP; } while(0)
#define set_IAPUEN_LDUEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN |= 0x02; EA = BIT_TMP; } while(0)
#define clr_IAPUEN_LDUEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN &= ~0x02; EA = BIT_TMP; } while(0)
#define set_IAPUEN_CFUEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN |= 0x04; EA = BIT_TMP; } while(0)
#define clr_IAPUEN_CFUEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN &= ~0x04; EA = BIT_TMP; } while(0)
#define set_IAPUEN_SPMEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN |= 0x08; EA = BIT_TMP; } while(0)
#define clr_IAPUEN_SPMEN    do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; IAPUEN &= ~0x08; EA = BIT_TMP; } while(0)

/*===========================================================================
 * IE Register Bits (0xA8) - bit-addressable
 *===========================================================================*/
#define set_IE_EA           EA = 1
#define clr_IE_EA           EA = 0
#define set_IE_EADC         EADC = 1
#define clr_IE_EADC         EADC = 0
#define set_IE_EBOD         EBOD = 1
#define clr_IE_EBOD         EBOD = 0
#define set_IE_ES           ES = 1
#define clr_IE_ES           ES = 0
#define set_IE_ET1          ET1 = 1
#define clr_IE_ET1          ET1 = 0
#define set_IE_EX1          EX1 = 1
#define clr_IE_EX1          EX1 = 0
#define set_IE_ET0          ET0 = 1
#define clr_IE_ET0          ET0 = 0
#define set_IE_EX0          EX0 = 1
#define clr_IE_EX0          EX0 = 0

/*===========================================================================
 * WDCON Register Bits (0xAA) - TA Protected
 *===========================================================================*/
#define set_WDCON_WDTEN     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON |= 0x80; EA = BIT_TMP; } while(0)
#define clr_WDCON_WDTEN     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON &= ~0x80; EA = BIT_TMP; } while(0)
#define set_WDCON_WDCLR     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON |= 0x40; EA = BIT_TMP; } while(0)
#define set_WDCON_WDTF      do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON |= 0x20; EA = BIT_TMP; } while(0)
#define clr_WDCON_WDTF      do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON &= ~0x20; EA = BIT_TMP; } while(0)
#define set_WDCON_WIDPD     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON |= 0x10; EA = BIT_TMP; } while(0)
#define clr_WDCON_WIDPD     do { BIT_TMP = EA; EA = 0; TA = 0xAA; TA = 0x55; WDCON &= ~0x10; EA = BIT_TMP; } while(0)

/*===========================================================================
 * EIE Register Bits (0x9B)
 *===========================================================================*/
#define set_EIE_ET2         (EIE |= 0x80)
#define clr_EIE_ET2         (EIE &= ~0x80)
#define set_EIE_ESPI        (EIE |= 0x40)
#define clr_EIE_ESPI        (EIE &= ~0x40)
#define set_EIE_EFB         (EIE |= 0x20)
#define clr_EIE_EFB         (EIE &= ~0x20)
#define set_EIE_EWDT        (EIE |= 0x10)
#define clr_EIE_EWDT        (EIE &= ~0x10)
#define set_EIE_EPWM        (EIE |= 0x08)
#define clr_EIE_EPWM        (EIE &= ~0x08)
#define set_EIE_ECAP        (EIE |= 0x04)
#define clr_EIE_ECAP        (EIE &= ~0x04)
#define set_EIE_EPI         (EIE |= 0x02)
#define clr_EIE_EPI         (EIE &= ~0x02)
#define set_EIE_EI2C        (EIE |= 0x01)
#define clr_EIE_EI2C        (EIE &= ~0x01)

/*===========================================================================
 * EIE1 Register Bits (0x9C)
 *===========================================================================*/
#define set_EIE1_EWKT       (EIE1 |= 0x04)
#define clr_EIE1_EWKT       (EIE1 &= ~0x04)
#define set_EIE1_ET3        (EIE1 |= 0x02)
#define clr_EIE1_ET3        (EIE1 &= ~0x02)
#define set_EIE1_ES1        (EIE1 |= 0x01)
#define clr_EIE1_ES1        (EIE1 &= ~0x01)

/*===========================================================================
 * I2CON Register Bits (0xC0) - bit-addressable
 *===========================================================================*/
#define set_I2CON_I2CEN     I2CEN = 1
#define clr_I2CON_I2CEN     I2CEN = 0
#define set_I2CON_STA       STA = 1
#define clr_I2CON_STA       STA = 0
#define set_I2CON_STO       STO = 1
#define clr_I2CON_STO       STO = 0
#define set_I2CON_SI        SI = 1
#define clr_I2CON_SI        SI = 0
#define set_I2CON_AA        AA = 1
#define clr_I2CON_AA        AA = 0

/*===========================================================================
 * T3CON Register Bits (0xC4)
 *===========================================================================*/
#define set_T3CON_TR3       (T3CON |= 0x04)
#define clr_T3CON_TR3       (T3CON &= ~0x04)
#define set_T3CON_TF3       (T3CON |= 0x10)
#define clr_T3CON_TF3       (T3CON &= ~0x10)
#define set_T3CON_BRCK      (T3CON |= 0x20)
#define clr_T3CON_BRCK      (T3CON &= ~0x20)

/*===========================================================================
 * T2CON Register Bits (0xC8) - bit-addressable
 *===========================================================================*/
#define set_T2CON_TF2       TF2 = 1
#define clr_T2CON_TF2       TF2 = 0
#define set_T2CON_TR2       TR2 = 1
#define clr_T2CON_TR2       TR2 = 0
#define set_T2CON_CM_RL2    CM_RL2 = 1
#define clr_T2CON_CM_RL2    CM_RL2 = 0

/*===========================================================================
 * ADCCON0 Register Bits (0xE8) - bit-addressable
 *===========================================================================*/
#define set_ADCCON0_ADCF    ADCF = 1
#define clr_ADCCON0_ADCF    ADCF = 0
#define set_ADCCON0_ADCS    ADCS = 1
#define clr_ADCCON0_ADCS    ADCS = 0

/*===========================================================================
 * ADCCON1 Register Bits (0xE1)
 *===========================================================================*/
#define set_ADCCON1_ADCEN   (ADCCON1 |= 0x01)
#define clr_ADCCON1_ADCEN   (ADCCON1 &= ~0x01)

/*===========================================================================
 * PWMCON0 Register Bits (0xD8) - bit-addressable
 *===========================================================================*/
#define set_PWMCON0_PWMRUN  PWMRUN = 1
#define clr_PWMCON0_PWMRUN  PWMRUN = 0
#define set_PWMCON0_LOAD    LOAD = 1
#define clr_PWMCON0_LOAD    LOAD = 0
#define set_PWMCON0_PWMF    PWMF = 1
#define clr_PWMCON0_PWMF    PWMF = 0
#define set_PWMCON0_CLRPWM  CLRPWM = 1
#define clr_PWMCON0_CLRPWM  CLRPWM = 0

/*===========================================================================
 * SCON_1 Register Bits (0xF8) - bit-addressable
 *===========================================================================*/
#define set_SCON_1_SM0_1    SM0_1 = 1
#define clr_SCON_1_SM0_1    SM0_1 = 0
#define set_SCON_1_SM1_1    SM1_1 = 1
#define clr_SCON_1_SM1_1    SM1_1 = 0
#define set_SCON_1_SM2_1    SM2_1 = 1
#define clr_SCON_1_SM2_1    SM2_1 = 0
#define set_SCON_1_REN_1    REN_1 = 1
#define clr_SCON_1_REN_1    REN_1 = 0
#define set_SCON_1_TI_1     TI_1 = 1
#define clr_SCON_1_TI_1     TI_1 = 0
#define set_SCON_1_RI_1     RI_1 = 1
#define clr_SCON_1_RI_1     RI_1 = 0

/*===========================================================================
 * GPIO Mode Macros
 * P[n]M1.x  P[n]M2.x  Mode
 *    0         0      Quasi-bidirectional (default)
 *    0         1      Push-pull output
 *    1         0      Input only (high-impedance)
 *    1         1      Open-drain
 *===========================================================================*/

/* Port 0 Pin Modes */
#define P00_QUASI_MODE      do { P0M1 &= ~0x01; P0M2 &= ~0x01; } while(0)
#define P00_PUSHPULL_MODE   do { P0M1 &= ~0x01; P0M2 |= 0x01; } while(0)
#define P00_INPUT_MODE      do { P0M1 |= 0x01; P0M2 &= ~0x01; } while(0)
#define P00_OPENDRAIN_MODE  do { P0M1 |= 0x01; P0M2 |= 0x01; } while(0)

#define P01_QUASI_MODE      do { P0M1 &= ~0x02; P0M2 &= ~0x02; } while(0)
#define P01_PUSHPULL_MODE   do { P0M1 &= ~0x02; P0M2 |= 0x02; } while(0)
#define P01_INPUT_MODE      do { P0M1 |= 0x02; P0M2 &= ~0x02; } while(0)
#define P01_OPENDRAIN_MODE  do { P0M1 |= 0x02; P0M2 |= 0x02; } while(0)

#define P02_QUASI_MODE      do { P0M1 &= ~0x04; P0M2 &= ~0x04; } while(0)
#define P02_PUSHPULL_MODE   do { P0M1 &= ~0x04; P0M2 |= 0x04; } while(0)
#define P02_INPUT_MODE      do { P0M1 |= 0x04; P0M2 &= ~0x04; } while(0)
#define P02_OPENDRAIN_MODE  do { P0M1 |= 0x04; P0M2 |= 0x04; } while(0)

#define P03_QUASI_MODE      do { P0M1 &= ~0x08; P0M2 &= ~0x08; } while(0)
#define P03_PUSHPULL_MODE   do { P0M1 &= ~0x08; P0M2 |= 0x08; } while(0)
#define P03_INPUT_MODE      do { P0M1 |= 0x08; P0M2 &= ~0x08; } while(0)
#define P03_OPENDRAIN_MODE  do { P0M1 |= 0x08; P0M2 |= 0x08; } while(0)

#define P04_QUASI_MODE      do { P0M1 &= ~0x10; P0M2 &= ~0x10; } while(0)
#define P04_PUSHPULL_MODE   do { P0M1 &= ~0x10; P0M2 |= 0x10; } while(0)
#define P04_INPUT_MODE      do { P0M1 |= 0x10; P0M2 &= ~0x10; } while(0)
#define P04_OPENDRAIN_MODE  do { P0M1 |= 0x10; P0M2 |= 0x10; } while(0)

#define P05_QUASI_MODE      do { P0M1 &= ~0x20; P0M2 &= ~0x20; } while(0)
#define P05_PUSHPULL_MODE   do { P0M1 &= ~0x20; P0M2 |= 0x20; } while(0)
#define P05_INPUT_MODE      do { P0M1 |= 0x20; P0M2 &= ~0x20; } while(0)
#define P05_OPENDRAIN_MODE  do { P0M1 |= 0x20; P0M2 |= 0x20; } while(0)

#define P06_QUASI_MODE      do { P0M1 &= ~0x40; P0M2 &= ~0x40; } while(0)
#define P06_PUSHPULL_MODE   do { P0M1 &= ~0x40; P0M2 |= 0x40; } while(0)
#define P06_INPUT_MODE      do { P0M1 |= 0x40; P0M2 &= ~0x40; } while(0)
#define P06_OPENDRAIN_MODE  do { P0M1 |= 0x40; P0M2 |= 0x40; } while(0)

#define P07_QUASI_MODE      do { P0M1 &= ~0x80; P0M2 &= ~0x80; } while(0)
#define P07_PUSHPULL_MODE   do { P0M1 &= ~0x80; P0M2 |= 0x80; } while(0)
#define P07_INPUT_MODE      do { P0M1 |= 0x80; P0M2 &= ~0x80; } while(0)
#define P07_OPENDRAIN_MODE  do { P0M1 |= 0x80; P0M2 |= 0x80; } while(0)

/* Port 1 Pin Modes */
#define P10_QUASI_MODE      do { P1M1 &= ~0x01; P1M2 &= ~0x01; } while(0)
#define P10_PUSHPULL_MODE   do { P1M1 &= ~0x01; P1M2 |= 0x01; } while(0)
#define P10_INPUT_MODE      do { P1M1 |= 0x01; P1M2 &= ~0x01; } while(0)
#define P10_OPENDRAIN_MODE  do { P1M1 |= 0x01; P1M2 |= 0x01; } while(0)

#define P11_QUASI_MODE      do { P1M1 &= ~0x02; P1M2 &= ~0x02; } while(0)
#define P11_PUSHPULL_MODE   do { P1M1 &= ~0x02; P1M2 |= 0x02; } while(0)
#define P11_INPUT_MODE      do { P1M1 |= 0x02; P1M2 &= ~0x02; } while(0)
#define P11_OPENDRAIN_MODE  do { P1M1 |= 0x02; P1M2 |= 0x02; } while(0)

#define P12_QUASI_MODE      do { P1M1 &= ~0x04; P1M2 &= ~0x04; } while(0)
#define P12_PUSHPULL_MODE   do { P1M1 &= ~0x04; P1M2 |= 0x04; } while(0)
#define P12_INPUT_MODE      do { P1M1 |= 0x04; P1M2 &= ~0x04; } while(0)
#define P12_OPENDRAIN_MODE  do { P1M1 |= 0x04; P1M2 |= 0x04; } while(0)

#define P13_QUASI_MODE      do { P1M1 &= ~0x08; P1M2 &= ~0x08; } while(0)
#define P13_PUSHPULL_MODE   do { P1M1 &= ~0x08; P1M2 |= 0x08; } while(0)
#define P13_INPUT_MODE      do { P1M1 |= 0x08; P1M2 &= ~0x08; } while(0)
#define P13_OPENDRAIN_MODE  do { P1M1 |= 0x08; P1M2 |= 0x08; } while(0)

#define P14_QUASI_MODE      do { P1M1 &= ~0x10; P1M2 &= ~0x10; } while(0)
#define P14_PUSHPULL_MODE   do { P1M1 &= ~0x10; P1M2 |= 0x10; } while(0)
#define P14_INPUT_MODE      do { P1M1 |= 0x10; P1M2 &= ~0x10; } while(0)
#define P14_OPENDRAIN_MODE  do { P1M1 |= 0x10; P1M2 |= 0x10; } while(0)

#define P15_QUASI_MODE      do { P1M1 &= ~0x20; P1M2 &= ~0x20; } while(0)
#define P15_PUSHPULL_MODE   do { P1M1 &= ~0x20; P1M2 |= 0x20; } while(0)
#define P15_INPUT_MODE      do { P1M1 |= 0x20; P1M2 &= ~0x20; } while(0)
#define P15_OPENDRAIN_MODE  do { P1M1 |= 0x20; P1M2 |= 0x20; } while(0)

#define P16_QUASI_MODE      do { P1M1 &= ~0x40; P1M2 &= ~0x40; } while(0)
#define P16_PUSHPULL_MODE   do { P1M1 &= ~0x40; P1M2 |= 0x40; } while(0)
#define P16_INPUT_MODE      do { P1M1 |= 0x40; P1M2 &= ~0x40; } while(0)
#define P16_OPENDRAIN_MODE  do { P1M1 |= 0x40; P1M2 |= 0x40; } while(0)

#define P17_QUASI_MODE      do { P1M1 &= ~0x80; P1M2 &= ~0x80; } while(0)
#define P17_PUSHPULL_MODE   do { P1M1 &= ~0x80; P1M2 |= 0x80; } while(0)
#define P17_INPUT_MODE      do { P1M1 |= 0x80; P1M2 &= ~0x80; } while(0)
#define P17_OPENDRAIN_MODE  do { P1M1 |= 0x80; P1M2 |= 0x80; } while(0)

/* Port 3 Pin Modes */
#define P30_QUASI_MODE      do { P3M1 &= ~0x01; P3M2 &= ~0x01; } while(0)
#define P30_PUSHPULL_MODE   do { P3M1 &= ~0x01; P3M2 |= 0x01; } while(0)
#define P30_INPUT_MODE      do { P3M1 |= 0x01; P3M2 &= ~0x01; } while(0)
#define P30_OPENDRAIN_MODE  do { P3M1 |= 0x01; P3M2 |= 0x01; } while(0)

/* All GPIO to Quasi Mode */
#define ALL_GPIO_QUASI_MODE do { P0M1 = 0; P0M2 = 0; P1M1 = 0; P1M2 = 0; P3M1 = 0; P3M2 = 0; } while(0)

/*===========================================================================
 * Interrupt Enable/Disable Convenience Macros
 *===========================================================================*/
#define ENABLE_GLOBAL_INTERRUPT     set_IE_EA
#define DISABLE_GLOBAL_INTERRUPT    clr_IE_EA

#define ENABLE_ADC_INTERRUPT        set_IE_EADC
#define DISABLE_ADC_INTERRUPT       clr_IE_EADC
#define ENABLE_BOD_INTERRUPT        set_IE_EBOD
#define DISABLE_BOD_INTERRUPT       clr_IE_EBOD
#define ENABLE_UART0_INTERRUPT      set_IE_ES
#define DISABLE_UART0_INTERRUPT     clr_IE_ES
#define ENABLE_TIMER1_INTERRUPT     set_IE_ET1
#define DISABLE_TIMER1_INTERRUPT    clr_IE_ET1
#define ENABLE_INT1_INTERRUPT       set_IE_EX1
#define DISABLE_INT1_INTERRUPT      clr_IE_EX1
#define ENABLE_TIMER0_INTERRUPT     set_IE_ET0
#define DISABLE_TIMER0_INTERRUPT    clr_IE_ET0
#define ENABLE_INT0_INTERRUPT       set_IE_EX0
#define DISABLE_INT0_INTERRUPT      clr_IE_EX0

#define ENABLE_TIMER2_INTERRUPT     set_EIE_ET2
#define DISABLE_TIMER2_INTERRUPT    clr_EIE_ET2
#define ENABLE_SPI_INTERRUPT        set_EIE_ESPI
#define DISABLE_SPI_INTERRUPT       clr_EIE_ESPI
#define ENABLE_WDT_INTERRUPT        set_EIE_EWDT
#define DISABLE_WDT_INTERRUPT       clr_EIE_EWDT
#define ENABLE_PWM_INTERRUPT        set_EIE_EPWM
#define DISABLE_PWM_INTERRUPT       clr_EIE_EPWM
#define ENABLE_CAPTURE_INTERRUPT    set_EIE_ECAP
#define DISABLE_CAPTURE_INTERRUPT   clr_EIE_ECAP
#define ENABLE_PININT_INTERRUPT     set_EIE_EPI
#define DISABLE_PININT_INTERRUPT    clr_EIE_EPI
#define ENABLE_I2C_INTERRUPT        set_EIE_EI2C
#define DISABLE_I2C_INTERRUPT       clr_EIE_EI2C

#define ENABLE_WKT_INTERRUPT        set_EIE1_EWKT
#define DISABLE_WKT_INTERRUPT       clr_EIE1_EWKT
#define ENABLE_TIMER3_INTERRUPT     set_EIE1_ET3
#define DISABLE_TIMER3_INTERRUPT    clr_EIE1_ET3
#define ENABLE_UART1_INTERRUPT      set_EIE1_ES1
#define DISABLE_UART1_INTERRUPT     clr_EIE1_ES1

#ifdef __cplusplus
}
#endif

#endif /* MS51_SFR_BITS_H */
