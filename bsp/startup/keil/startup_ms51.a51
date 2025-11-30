;------------------------------------------------------------------------------
; MS51 BSP - Keil C51 Startup Code
; 
; This file provides the startup code for MS51 microcontrollers when using
; the Keil C51 compiler.
;
; Copyright (c) 2024-2025 MS51_BSP Contributors
; SPDX-License-Identifier: Apache-2.0
;------------------------------------------------------------------------------

$NOMOD51                    ; Disable predefined 8051 registers

;------------------------------------------------------------------------------
; Include device-specific SFR definitions
;------------------------------------------------------------------------------
; User should include appropriate SFR file based on target device

NAME    STARTUP_MS51

;------------------------------------------------------------------------------
; Stack Configuration
;------------------------------------------------------------------------------
?STACK  SEGMENT   IDATA
        RSEG      ?STACK
        DS        40H           ; 64 bytes of stack space

;------------------------------------------------------------------------------
; External References
;------------------------------------------------------------------------------
        EXTRN CODE (?C_STARTUP)

;------------------------------------------------------------------------------
; Reset Vector
;------------------------------------------------------------------------------
        CSEG AT 0
        LJMP    STARTUP1

;------------------------------------------------------------------------------
; Startup Code
;------------------------------------------------------------------------------
        RSEG    ?C_C51STARTUP
STARTUP1:
        ; Initialize stack pointer
        MOV     SP, #?STACK-1

        ; Clear internal RAM (optional - uncomment if needed)
        ; MOV     R0, #7FH
        ; CLR     A
        ; CLEAR_RAM:
        ;   MOV     @R0, A
        ;   DJNZ    R0, CLEAR_RAM

        ; Jump to C startup code
        LJMP    ?C_STARTUP

;------------------------------------------------------------------------------
        END
