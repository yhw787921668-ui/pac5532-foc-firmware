
/**
 * @file control_1shunt_funcs_asym2.h
 * @brief Use for single shunt control functions
 */

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

#ifndef CONTROL_1SHUNT_FUNCS_ASYM2_H
#define CONTROL_1SHUNT_FUNCS_ASYM2_H

#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "control_1shunt_struct.h"
#include "adc_sample.h"
#include "isr_funcs.h"


#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)

PAC5XXX_RAMFUNC static inline void m1_u_set_duty_up(uint16_t duty)             { FOC_PWM_Timer_Ptr->CCTR0.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_v_set_duty_up(uint16_t duty)             { FOC_PWM_Timer_Ptr->CCTR1.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_w_set_duty_up(uint16_t duty)             { FOC_PWM_Timer_Ptr->CCTR2.CTR = duty; }

PAC5XXX_RAMFUNC static inline void m1_u_set_duty_dn(uint16_t duty)             { FOC_PWM_Timer_Ptr->CCTR4.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_v_set_duty_dn(uint16_t duty)             { FOC_PWM_Timer_Ptr->CCTR5.CTR = duty; }
PAC5XXX_RAMFUNC static inline void m1_w_set_duty_dn(uint16_t duty)             { FOC_PWM_Timer_Ptr->CCTR6.CTR = duty; }


PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void control_1shunt_newasym_update_dutycycle_seqtrigger(Struct_Motor *motor_ptr)
{
//    debugio1_toggle();

    FOC_PWM_Timer_Ptr->CTL.LATCH = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.LATCH = 1;
    // This may not be required if the default is zero already
    /*PAC55XX_TIMERA->CCTL0.CCLATCH = 0;
    PAC55XX_TIMERA->CCTL1.CCLATCH = 0;
    PAC55XX_TIMERA->CCTL2.CCLATCH = 0;
    PAC55XX_TIMERA->CCTL4.CCLATCH = 0;
    PAC55XX_TIMERA->CCTL5.CCLATCH = 0;
    PAC55XX_TIMERA->CCTL6.CCLATCH = 0;
    PAC55XX_TIMERB->CCTL0.CCLATCH = 0;
    PAC55XX_TIMERB->CCTL1.CCLATCH = 0;*/
    
    // Update UP duty cycle from previous control loop run
    __disable_irq();
    m1_u_set_duty_up(motor_ptr->oneshunt.duty_u_up);
    m1_v_set_duty_up(motor_ptr->oneshunt.duty_v_up);
    m1_w_set_duty_up(motor_ptr->oneshunt.duty_w_up);
    m1_u_set_duty_dn(motor_ptr->oneshunt.duty_u_dn);
    m1_v_set_duty_dn(motor_ptr->oneshunt.duty_v_dn);
    m1_w_set_duty_dn(motor_ptr->oneshunt.duty_w_dn);

    FOC_ASYM_SUPPLE_Timer_Ptr->CCTR0.CTR = motor_ptr->oneshunt.taccr0_value;
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTR1.CTR = motor_ptr->oneshunt.taccr1_value;
    __enable_irq();
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline void config_timer_foc(Struct_Motor *motor_ptr)
{
    // Configure PWM Timer Base
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config((TimerInstance) FOC_PWM_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                           // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config((TimerInstance) FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_ASYMMETRIC, 0);   // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDLATCH = 0;                                                        // Update duty cycle when TACTR counts from 1 to 0
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL0), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL1), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA1 for phase V
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL2), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA2 for phase W

    // Configure SUPPLE Timer Base
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config(FOC_ASYM_SUPPLE_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                       // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config(FOC_ASYM_SUPPLE_Timer, motor.control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 1);   // Enable timerB

    // Release Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 0;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 0;
}


static inline void deconfig_timer_foc(void)
{
    // Disable Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 1;
}


static inline void config_int_foc(Struct_Motor *motor_ptr)
{
    // Configure Interrupts for FOC
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTR2.CTR = (motor_ptr->control.motor_period_ticks >> 1);
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTL2.CCINTEDGE = 0;
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTL2.CCINTEN = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTL2.CCMODE = 0;
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTL2.CCLATCH = 0;

    // Configure Interrupts for FOC control counter
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 1;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

    // Configure NVIC
    NVIC_EnableIRQ(FOC_ASYM_SUPPLE_Timer_IRQn);
    NVIC_SetPriority(FOC_ASYM_SUPPLE_Timer_IRQn, 1);
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn, 0);
}


static inline void deconfig_int_foc(void)
{
    // Disable Interrupts
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTL2.CCINTEN = 0;
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 0;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;
}


static inline void config_int_ctrlctr(void)
{
    // Configure Interrupts for FOC control counter
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 1;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn, 0);
}


static inline void deconfig_int_ctrlctr(void)
{
    // Disable Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 0;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;
}

#endif  //#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
#endif  //#ifndef CONTROL_1SHUNT_FUNCS_ASYM2_H
