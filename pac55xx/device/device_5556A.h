
//=============================================================================
// Copyright (C) 2017 - 2023, Qorvo, Inc.
//
// THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
// AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
// APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
// (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
// QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
// DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
// ONLY BY CERTAIN AUTHORIZED PERSONS.
//
//=============================================================================
 
#ifndef DEVICE_5556A_H
#define DEVICE_5556A_H

#include "pac5556.h"

#include "pac5xxx.h"
#include "family_55xx.h"
#include "pac5xxx_driver_tile.h"

#define PWM_TIMER TIMER_B
#define FOC_PWM_Timer TimerB
#define FOC_PWM_Timer_Ptr PAC55XX_TIMERB
#define FOC_PWM_Timer_IRQn TimerB_IRQn
#define FOC_DAC_Timer TimerC
#define ADC_IRQn ADC0_IRQn
#define FOC_ASYM_SUPPLE_Timer_Ptr PAC55XX_TIMERA
#define FOC_ASYM_SUPPLE_Timer_IRQn TimerA_IRQn
#define FOC_ASYM_SUPPLE_Timer TimerA

// QEP timer and port configuration
#define QEP_TIMER_SELECT                TIMER_D
#define QEP_PORT                        QEP_PORT_D

#define PUSER_VALUE_DIFFAMP_GAIN_FINAL PUSER_VALUE_DIFFAMP_GAIN
#define PUSER_VALUE_DIFFAMP_GAIN_FIXED                    FALSE

// Debug DAC Port Supported
#define DEBUG_DAC_SUPPORT_PORT_A_DRL    FALSE

//==========================================
// Motor Driver Control Macro Functions
//==========================================
static inline void motor_psel_pwm(void)
{
    PAC55XX_SCC->PBMUXSEL.P0 = 2;              // PB0=TBPWM0, DRL0
    PAC55XX_SCC->PBMUXSEL.P1 = 2;              // PB1=TAPWM1, DRL1
    PAC55XX_SCC->PBMUXSEL.P2 = 2;              // PB2=TAPWM2, DRL2
    
    PAC55XX_SCC->PCMUXSEL.P4 = 1;              // PC4 = TBPWM4, DXH0
    PAC55XX_SCC->PCMUXSEL.P5 = 1;              // PC5 = TBPWM5, DXH1
    PAC55XX_SCC->PCMUXSEL.P6 = 1;              // PC6 = TBPWM6, DHX2
}

static inline void motor_outen_enable(void)
{
    PAC55XX_GPIOB->OUTMASK.P0 = 0;             // Mask = 0 enables output
    PAC55XX_GPIOB->OUTMASK.P1 = 0;             // Mask = 0 enables output
    PAC55XX_GPIOB->OUTMASK.P2 = 0;             // Mask = 0 enables output

    PAC55XX_GPIOC->OUTMASK.P4 = 0;             // Mask = 0 enables output
    PAC55XX_GPIOC->OUTMASK.P5 = 0;             // Mask = 0 enables output
    PAC55XX_GPIOC->OUTMASK.P6 = 0;             // Mask = 0 enables output
}

static inline void motor_psel_gpio(void)
{
    // Revert PB PWM Driver outputs to GPIOs
    // PB0, PB1, PB2  pins set to GPIO
    PAC55XX_SCC->PBMUXSEL.P0 = 0;              // PB0 as an output, DRL0
    PAC55XX_SCC->PBMUXSEL.P1 = 0;              // PB1 as an output, DRL1
    PAC55XX_SCC->PBMUXSEL.P2 = 0;              // PB2 as an output, DRL2

    // Revert PC PWM Driver outputs to GPIOs
    // PC4, PC5, PC6  pins set to GPIO
    PAC55XX_SCC->PCMUXSEL.P4 = 0;              // PC4 as an output, DXH0
    PAC55XX_SCC->PCMUXSEL.P5 = 0;              // PC5 as an output, DXH1
    PAC55XX_SCC->PCMUXSEL.P6 = 0;              // PC6 as an output, DXH2
}

static inline void motor_out_clear(void)
{
    PAC55XX_GPIOB->OUT.P0 = 0;                 // PB0 as an output, DRL0
    PAC55XX_GPIOB->OUT.P1 = 0;                 // PB1 as an output, DRL1
    PAC55XX_GPIOB->OUT.P2 = 0;                 // PB2 as an output, DRL2

    PAC55XX_GPIOC->OUT.P4 = 0;                 // PC4 as an output, DXH0
    PAC55XX_GPIOC->OUT.P5 = 0;                 // PC5 as an output, DXH1
    PAC55XX_GPIOC->OUT.P6 = 0;                 // PC6 as an output, DXH2
}

static inline void motor_psel_pwm_LS(void)
{
    PAC55XX_SCC->PBMUXSEL.P0 = 2;              // PB0=TBPWM0
    PAC55XX_SCC->PBMUXSEL.P1 = 2;              // PB0=TBPWM1
    PAC55XX_SCC->PBMUXSEL.P2 = 2;              // PB2=TBPWM2

    // Revert PC PWM Driver outputs to GPIOs
    // PC4, PC5, PC6  pins set to GPIO
    PAC55XX_SCC->PCMUXSEL.P4 = 0;              // PC4 as an output, DXH0
    PAC55XX_SCC->PCMUXSEL.P5 = 0;              // PC5 as an output, DXH1
    PAC55XX_SCC->PCMUXSEL.P6 = 0;              // PC6 as an output, DXH2

    // PC4, PC5, PC6  pins outputs set to 0
    PAC55XX_GPIOC->OUT.P4 = 0;                 // PC4 as an output, DXH0
    PAC55XX_GPIOC->OUT.P5 = 0;                 // PC5 as an output, DXH1
    PAC55XX_GPIOC->OUT.P6 = 0;                 // PC6 as an output, DXH2
}

static inline void motor_modesel_gpio(void)
{
    PAC55XX_GPIOB->MODE.P0 = 1;                     // Set PB0, IOs to Push Pull Output
    PAC55XX_GPIOB->MODE.P1 = 1;                     // Set PB1, IOs to Push Pull Output
    PAC55XX_GPIOB->MODE.P2 = 1;                     // Set PB2, IOs to Push Pull Output

    PAC55XX_GPIOC->MODE.P4 = 1;                     // Set PC4, IOs to Push Pull Output
    PAC55XX_GPIOC->MODE.P5 = 1;                     // Set PC5, IOs to Push Pull Output
    PAC55XX_GPIOC->MODE.P6 = 1;                     // Set PC6, IOs to Push Pull Output
}

static inline void enable_bbm()
{
}

static inline void motor_out_LSon(void)
{
    PAC55XX_GPIOB->OUT.w = 0x0007;
}


//=============================================
// Motor Driver Duty Cycle Macro Functions
//=============================================
PAC5XXX_RAMFUNC static inline void m1_u_set_duty(uint16_t duty)             { PAC55XX_TIMERB->CCTR4.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_v_set_duty(uint16_t duty)             { PAC55XX_TIMERB->CCTR5.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_w_set_duty(uint16_t duty)             { PAC55XX_TIMERB->CCTR6.CTR = duty; }

// CAFE specific definition
//=====================================
// Low OC Protection Signal (AIOxx_LPROT)
//=====================================
#define AIO50_LPROT_NONE        0
#define AIO50_LPROT_PR          BIT_04

//=====================================
// High OC Protection Signal (AIOxx_HPROT)
//=====================================
#define AIO50_HPROT_NONE        0
#define AIO50_HPROT_PR          BIT_05

//=====================================
// Driver protection while OC 
//=====================================
#define DRV_HIGHSIDE_PR         BIT_03
#define DRV_LOWSIDE_PR          BIT_02

//=====================================
// Vp voltage selection 
//=====================================
#define VP_VOLTAGE_12V       (0x00 << 3)
#define VP_VOLTAGE_15V       (0x01 << 3)
#define VP_CONFIG_BITS_MASK         0xF7

//==========================================================
// Min duty cycle: Version A can be set as 0, otherwise as 1
//==========================================================
#define DUTY_CYCLE_MIN_TICK     1

//==================================================================
// For internal Vms, duty cycle exclusion is true for 5556 and 5256
//==================================================================
#define VMS_DUTY_CYCLE_EXCLUSION           TRUE

//=====================================
// Internal VMS measurement method
//=====================================
#define VMS_FACTORY_CAL          TRUE

#endif  //#ifndef DEVICE_5556A_H

