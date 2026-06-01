;/*****************************************************************************
; * @file:    startup_pac5xxx_iar.s
; * @purpose: CMSIS Cortex-M0 Core Device Startup File for the 
; *           PAC5XXX Device
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2015-2019, Qorvo, Inc.
; *
; * THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES, 
; * AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S 
; * APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.; 
; * (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT 
; * QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
; * DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
; * ONLY BY CERTAIN AUTHORIZED PERSONS. 
; *
; *****************************************************************************/


;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start

        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        DATA
__iar_init$$done:               ; This vector table is not needed until after copy initialization is done
__vector_table
        DCD     sfe(CSTACK)                 ; Top of Stack
        DCD     Reset_Handler               ; Reset Handler
        DCD     NMI_Handler                 ; NMI Handler
        DCD     HardFault_Handler           ; Hard Fault Handler
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
__vector_table_0x1c
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     SVC_Handler                 ; SVCall Handler
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     PendSV_Handler              ; PendSV Handler
        DCD     SysTick_Handler             ; SysTick Handler
        DCD     MemCtl_IRQHandler           ; 0: Memory Controller Handler
        DCD     SysClk_IRQHandler           ; 1: System and Clock Controller Handler
        DCD     Wdt_IRQHandler              ; 2: Watchdog Timer Handler
        DCD     GpioA_IRQHandler            ; 3: GPIO A Handler
        DCD     GpioB_IRQHandler            ; 4: GPIO B Handler
        DCD     GpioC_IRQHandler            ; 5: GPIO C Handler
        DCD     GpioD_IRQHandler            ; 6: GPIO D Handler
        DCD     GpioE_IRQHandler            ; 7: GPIO E Handler
        DCD     TimerA_IRQHandler           ; 8: Timer A Handler (Base, CC 0-5)
        DCD     TimerAExt_IRQHandler        ; 9: Timer A Ext Handler (CC 6, 7)
        DCD     TimerB_IRQHandler           ; 10: Timer B Handler (Base + CC)
        DCD     TimerC_IRQHandler           ; 11: Timer C Handler (Base + CC)
        DCD     TimerD_IRQHandler           ; 12: Timer D Handler (Base + CC)
        DCD     ADC_IRQHandler              ; 13: ADC Handler
        DCD     SocBridge_IRQHandler        ; 14: SocBridge Handler
        DCD     SPIB_IRQHandler             ; 15: SPI-B Handler
        DCD     UART_IRQHandler             ; 16: UART Handler
        DCD     I2C_IRQHandler              ; 17: I2C Handler
        DCD     EMUX_IRQHandler             ; 18: EMUX Handler
        DCD     RTC_IRQHandler              ; 19: RTC Handler

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        SECTION .text:CODE:REORDER:NOROOT(1)

        PUBWEAK NMI_Handler
        PUBWEAK HardFault_Handler
        PUBWEAK SVC_Handler
        PUBWEAK PendSV_Handler
        PUBWEAK SysTick_Handler
        PUBWEAK MemCtl_IRQHandler
        PUBWEAK SysClk_IRQHandler
        PUBWEAK Wdt_IRQHandler
        PUBWEAK GpioA_IRQHandler
        PUBWEAK GpioB_IRQHandler
        PUBWEAK GpioC_IRQHandler
        PUBWEAK GpioD_IRQHandler
        PUBWEAK GpioE_IRQHandler
        PUBWEAK TimerA_IRQHandler
        PUBWEAK TimerAExt_IRQHandler
        PUBWEAK TimerB_IRQHandler
        PUBWEAK TimerC_IRQHandler
        PUBWEAK TimerD_IRQHandler
        PUBWEAK ADC_IRQHandler
        PUBWEAK SocBridge_IRQHandler
        PUBWEAK SPIB_IRQHandler
        PUBWEAK UART_IRQHandler
        PUBWEAK I2C_IRQHandler
        PUBWEAK EMUX_IRQHandler
        PUBWEAK RTC_IRQHandler
        PUBWEAK Default_Handler

NMI_Handler:
HardFault_Handler:
MemManage_Handler:
BusFault_Handler:
UsageFault_Handler:
SVC_Handler:
DebugMon_Handler:
PendSV_Handler:
SysTick_Handler:
MemCtl_IRQHandler:
SysClk_IRQHandler:
Wdt_IRQHandler:
GpioA_IRQHandler:
GpioB_IRQHandler:
GpioC_IRQHandler:
GpioD_IRQHandler:
GpioE_IRQHandler:
TimerA_IRQHandler:
TimerAExt_IRQHandler:
TimerB_IRQHandler:
TimerC_IRQHandler:
TimerD_IRQHandler:
ADC_IRQHandler:
SocBridge_IRQHandler:
SPIB_IRQHandler:
UART_IRQHandler:
I2C_IRQHandler:
EMUX_IRQHandler:
RTC_IRQHandler:
Default_Handler:
        BL Default_Handler
        

        PUBWEAK Reset_Handler        
        EXTERN  pac_recovery
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler:
        ; Call pac_recovery function to allow recovery of mis-configured device  
        LDR R0, =pac_recovery
        BLX  R0

        LDR R0, =__iar_program_start
        BX  R0
        NOP

;=============================================================================
; @brief  This function implements a delay in assembly to provide a consistent
;         delay independent of compiler optimizations.
;
; @param  counts - number of loop iterations
; @retval None
;=============================================================================        
;void pac_delay_asm(uint32_t counts)
        PUBLIC pac_delay_asm
pac_delay_asm:
        push {r1}           ; Save r1 for use in this function
        movs r1, #1         ; r1=1 for decrement value
delay_loop1:
        subs  r0,r0,r1      ; Decrement r0=counts by 1
        bne  delay_loop1    ; Keep looping until counts == 0
        pop  {r1}           ; Restore r1
        bx   lr             ; Return to calling function
        

        END