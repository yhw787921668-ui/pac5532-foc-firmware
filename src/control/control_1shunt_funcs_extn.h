
/**
 * @file control_1shunt_funcs_extn.h
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

#ifndef CONTROL_1SHUNT_FUNCS_EXTN_H
#define CONTROL_1SHUNT_FUNCS_EXTN_H

#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "control_1shunt_struct.h"
#include "adc_sample.h"
#include "isr_funcs.h"

#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)

/**
 * @brief This function use for configure foc control to program 3 phase duty cycle
 * @param intStruct_Motor *motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void control_1shunt_extn_update_dutycycle_seqtrigger(Struct_Motor *motor_ptr)
{
    // Update duty cycle from previous control loop run
    __disable_irq();
    m1_u_set_duty(motor_ptr->control.u_duty_value);
    m1_v_set_duty(motor_ptr->control.v_duty_value);
    m1_w_set_duty(motor_ptr->control.w_duty_value);
    
#if defined(FAMILY_52XX)
    FOC_PWM_Timer_Ptr->CTR0 = motor_ptr->oneshunt.taccr0_value;
    FOC_PWM_Timer_Ptr->CTR1 = motor_ptr->oneshunt.taccr1_value;
#endif

#if defined(FAMILY_55XX)
    FOC_PWM_Timer_Ptr->CCTR0.CTR = motor_ptr->oneshunt.taccr0_value;
    FOC_PWM_Timer_Ptr->CCTR1.CTR = motor_ptr->oneshunt.taccr1_value;
#endif
    __enable_irq();
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline void config_timer_foc(Struct_Motor *motor_ptr)
{
#if defined(FAMILY_52XX)
    // Configure PWM Timer Base
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);
    pac5xxx_timer_clock_config(FOC_PWM_Timer, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                                // Configure timer clock input for ACLK (50MHz), /1 divider
    pac5xxx_timer_base_config(FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 0);    // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDL = 1;                                                        // Latch new TAPRD value when timerA counting down, TACTR value = 0x01
    // JAGTAG: Deadtime config has not been changed to PWM timer reference
    pac5xxx_dtg_config(DTGA0, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA0 for phase U
    pac5xxx_dtg_config(DTGA1, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA1 for phase V
    pac5xxx_dtg_config(DTGA2, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA2 for phase W

    // Release Timers
    pac5xxx_timer_clear(FOC_PWM_Timer, 0);

#elif defined(FAMILY_55XX)
    // Configure PWM Timer Base
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config((TimerInstance) FOC_PWM_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                           // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config((TimerInstance) FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 0);   // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDLATCH = 0;                                                        // Update duty cycle when TACTR counts from 1 to 0
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL0), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL1), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA1 for phase V
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL2), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA2 for phase W

    // Release Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 0;

#endif
}


static inline void deconfig_timer_foc(void)
{
#if defined(FAMILY_52XX)
    // Disable PWM Timer
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);

#elif defined(FAMILY_55XX)
    // Disable PWM Timer
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;

#endif
}


static inline void config_int_foc(Struct_Motor *motor_ptr)
{
#if defined(FAMILY_52XX)
    // Configure Interrupts
    pac5xxx_adc_int_enable_as0(0);                                                          // Disable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(1);                                                          // Enable ADC interrupts for AS1 complete

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn,1);

#elif defined(FAMILY_55XX)
    // Configure Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 1;          // Enable base interrupts for control counter
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn,1);
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#endif
}


static inline void deconfig_int_foc(void)
{
#if defined(FAMILY_52XX)
    // Disable Interrupts
    pac5xxx_adc_int_enable_as0(0);                                                          // Disable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(0);                                                          // Disable ADC interrupts for AS1 complete

    // Disable NVIC
    NVIC_DisableIRQ(ADC_IRQn);

#elif defined(FAMILY_55XX)
    // Disable Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 0;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

    // Disable NVIC
    NVIC_DisableIRQ(ADC_IRQn);

#endif
}


static inline void config_int_ctrlctr(void)
{
#if defined(FAMILY_52XX)
    // Configure Interrupts
    pac5xxx_timer_base_int_enable(FOC_PWM_Timer, 1);                                        // Enable base interrupts for control counter

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#elif defined(FAMILY_55XX)
    // Configure Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 1;          // Enable base interrupts for control counter
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#endif
}


static inline void deconfig_int_ctrlctr(void)
{
#if defined(FAMILY_52XX)
    // Disable Interrupts
    pac5xxx_timer_base_int_enable(FOC_PWM_Timer, 0);

#elif defined(FAMILY_55XX)
    // Disable Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 0;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

#endif
}

#endif  //#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
#endif  //#ifndef CONTROL_1SHUNT_FUNCS_EXTN_H
