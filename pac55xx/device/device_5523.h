
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
 
#ifndef DEVICE_5523_H
#define DEVICE_5523_H

#include "pac5523.h"

#include "pac5xxx.h"
#include "family_55xx.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_tile_driver_manager.h"

#define PWM_TIMER TIMER_A
#define FOC_PWM_Timer TimerA
#define FOC_PWM_Timer_Ptr PAC55XX_TIMERA
#define FOC_PWM_Timer_IRQn TimerA_IRQn
#define FOC_DAC_Timer TimerB
#define ADC_IRQn ADC0_IRQn
#define FOC_ASYM_SUPPLE_Timer_Ptr PAC55XX_TIMERB
#define FOC_ASYM_SUPPLE_Timer_IRQn TimerB_IRQn
#define FOC_ASYM_SUPPLE_Timer TimerB

// QEP timer and port configuration
#define QEP_TIMER_SELECT                TIMER_C
#define QEP_PORT                        QEP_PORT_C

#define PUSER_VALUE_DIFFAMP_GAIN_FINAL PUSER_VALUE_DIFFAMP_GAIN
#define PUSER_VALUE_DIFFAMP_GAIN_FIXED                    FALSE

// Debug DAC Port Supported
#define DEBUG_DAC_SUPPORT_PORT_A_DRL    FALSE

//==========================================
// Motor Driver Control Macro Functions
//==========================================
static inline void motor_psel_pwm(void)
{
    PAC55XX_SCC->PBMUXSEL.w =  0x01110111;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
}

static inline void motor_outen_enable(void)
{
    PAC55XX_GPIOB->OUTMASK.w = 0x00;                    // Mask = 0 enables output
}

static inline void motor_psel_gpio(void)
{
    // Revert PB PWM Driver outputs to GPIOs
    PAC55XX_SCC->PBMUXSEL.w =  0x00000000;              // All PB pins set to GPIO          
}

static inline void motor_out_clear(void)
{
    PAC55XX_GPIOB->OUT.w = 0x00;
}

static inline void motor_psel_pwm_LS(void)
{
    PAC55XX_SCC->PBMUXSEL.w =  0x00000111;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
}

static inline void motor_modesel_gpio(void)
{
    PAC55XX_GPIOB->MODE.w = 0x1515;                     // Set all PB6-PB4, PB2-PB0, IOs to Push Pull Output
}

static inline void enable_bbm()
{
    pac5xxx_tile_register_write(ADDR_ENBBM, 1);
}

static inline void motor_out_LSon(void)
{
    PAC55XX_GPIOB->OUT.w = 0x0007;
}


//=============================================
// Motor Driver Duty Cycle Macro Functions
//=============================================
PAC5XXX_RAMFUNC static inline void m1_u_set_duty(uint16_t duty)             { PAC55XX_TIMERA->CCTR4.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_v_set_duty(uint16_t duty)             { PAC55XX_TIMERA->CCTR5.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_w_set_duty(uint16_t duty)             { PAC55XX_TIMERA->CCTR6.CTR = duty; }

// CAFE specific definition
//=====================================
// Low OC Protection Signal (AIOxx_LPROT)
//=====================================
#define AIO50_LPROT_NONE        0
#define AIO50_LPROT_PR1         BIT_05
#define AIO50_LPROT_PR2         BIT_04
#define AIO50_LPROT_PR1PR2      (BIT_04 | BIT_05)

//=====================================
// High OC Protection Signal (AIOxx_HPROT)
//=====================================
#define AIO50_HPROT_NONE        0
#define AIO50_HPROT_PR1         BIT_07
#define AIO50_HPROT_PR2         BIT_06
#define AIO50_HPROT_PR1PR2      (BIT_06 | BIT_07)

//=====================================
// Driver protection while OC 
//=====================================
#define DRV_HIGHSIDE_PR1        BIT_07
#define DRV_HIGHSIDE_PR2        BIT_06
#define DRV_LOWSIDE_PR1         BIT_05
#define DRV_LOWSIDE_PR2         BIT_04

//=====================================
// Vp voltage selection 
//=====================================
#define VP_VOLTAGE_9V        (0x01 << 6)
#define VP_VOLTAGE_12V       (0x02 << 6)
#define VP_VOLTAGE_15V       (0x03 << 6)
#define VP_CONFIG_BITS_MASK         0x3F

//==========================================================
// Min duty cycle: Version A can be set as 0, otherwise as 1
//==========================================================
#define DUTY_CYCLE_MIN_TICK     1

#endif  //#ifndef DEVICE_5523_H

