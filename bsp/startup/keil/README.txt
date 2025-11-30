; Keil C51 Startup Configuration
;
; This file provides startup configuration options for the MS51 BSP.
; Include this in your Keil project settings.
;
; Memory Model: Small (default) or Large
; Stack Size: Configurable via ?STACK segment

; Compiler Optimization Recommendations:
; - Level 8 or higher for production code
; - Favor speed for timing-critical applications
; - Favor size for memory-constrained applications

; Interrupt Configuration:
; - Interrupt vectors are defined in the HAL layer
; - User interrupts should use the ms51_hal.h definitions

; Notes:
; - The startup code initializes the stack pointer
; - Global variables are initialized by the C runtime
; - The main() function is called after initialization
