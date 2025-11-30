;/*---------------------------------------------------------------------------
; MS51 BSP - SDCC Startup Code
; 
; This file provides the startup code for MS51 microcontrollers when using
; the Small Device C Compiler (SDCC).
;
; Copyright (c) 2024-2025 MS51_BSP Contributors
; SPDX-License-Identifier: Apache-2.0
;---------------------------------------------------------------------------*/

        .module startup_ms51
        .globl  _main
        .globl  __sdcc_gsinit_startup

;------------------------------------------------------------------------------
; Stack Configuration
;------------------------------------------------------------------------------
        .area   SSEG    (DATA)
__start__stack:
        .ds     0x40            ; 64 bytes stack space

;------------------------------------------------------------------------------
; Reset Vector
;------------------------------------------------------------------------------
        .area   CSEG    (CODE)
        .org    0x0000
        ljmp    __sdcc_gsinit_startup

;------------------------------------------------------------------------------
; Startup Code
;------------------------------------------------------------------------------
        .area   GSINIT  (CODE)
__sdcc_gsinit_startup:
        ; Initialize stack pointer
        mov     sp, #__start__stack - 1

        ; Clear BSS section
        ; (SDCC runtime handles this)

        ; Initialize DATA section
        ; (SDCC runtime handles this)

;------------------------------------------------------------------------------
; Jump to main
;------------------------------------------------------------------------------
        .area   GSFINAL (CODE)
        lcall   _main

        ; Halt on return
__loop_forever:
        sjmp    __loop_forever

        .end
