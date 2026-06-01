
/**
 * @file control_1shunt_funcs_asym.h
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

#ifndef CONTROL_1SHUNT_FUNCS_ASYM_H
#define CONTROL_1SHUNT_FUNCS_ASYM_H

#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "control_1shunt_struct.h"
#include "adc_sample.h"
#include "isr_funcs.h"


#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)

/**
 * @brief This function use to program 3 phase duty cycle, increment control counter
 * @param intStruct_Motor *motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void control_1shunt_asym_update_dutycycledn_seqtrigger(Struct_Motor *motor_ptr)
{
    //debugio2_on();
#if defined(FAMILY_52XX)
    // Release TimerB
    PAC5XXX_TIMERB->CTL.CLR = 0;

    // Update CCR latching point to TAPRD-0x1 up
    PAC5XXX_TIMERA->CTL.PRDL = 0;
#else
    // Release TimerB
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 0;

    // Update CCR latching point to TAPRD-0x1 up
    FOC_PWM_Timer_Ptr->CCTL4.CCLATCH = 1;
    FOC_PWM_Timer_Ptr->CCTL5.CCLATCH = 1;
    FOC_PWM_Timer_Ptr->CCTL6.CCLATCH = 1;
#endif

    // Update DN duty cycle from previous control loop run
    m1_u_set_duty(motor_ptr->oneshunt.duty_u_dn_store);
    m1_v_set_duty(motor_ptr->oneshunt.duty_v_dn_store);
    m1_w_set_duty(motor_ptr->oneshunt.duty_w_dn_store);
    
    //debugio2_off();
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use for configure foc control to program 3 phase duty cycle, increment control counter
 * @param intStruct_Motor *motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void control_1shunt_asym_update_dutycycleup_seqtrigger(Struct_Motor *motor_ptr)
{
    //debugio1_on();

#if defined(FAMILY_52XX)
    // Clear and hold TimerB
    PAC5XXX_TIMERB->CTL.CLR = 1;

    // Update CCR latching point to 0x1 down
    PAC5XXX_TIMERA->CTL.PRDL = 1;
#else
    // Clear and hold TimerB
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 1;

    // Update CCR latching point to 0x1 down
    FOC_PWM_Timer_Ptr->CCTL4.CCLATCH = 0;
    FOC_PWM_Timer_Ptr->CCTL5.CCLATCH = 0;
    FOC_PWM_Timer_Ptr->CCTL6.CCLATCH = 0;
#endif

    // Update UP duty cycle from previous control loop run
    __disable_irq();
    m1_u_set_duty(motor_ptr->oneshunt.duty_u_up);
    m1_v_set_duty(motor_ptr->oneshunt.duty_v_up);
    m1_w_set_duty(motor_ptr->oneshunt.duty_w_up);
    #if defined(FAMILY_52XX)
    PAC5XXX_TIMERA->CTR0 = motor_ptr->oneshunt.taccr0_value;
    PAC5XXX_TIMERA->CTR1 = motor_ptr->oneshunt.taccr1_value;
    //pac5xxx_timer_a_ccctr0_value_set(motor_ptr->oneshunt.taccr0_value);
    //pac5xxx_timer_a_ccctr1_value_set(motor_ptr->oneshunt.taccr1_value);
    #else
    FOC_PWM_Timer_Ptr->CCTR0.CTR = motor_ptr->oneshunt.taccr0_value;
    FOC_PWM_Timer_Ptr->CCTR1.CTR = motor_ptr->oneshunt.taccr1_value;
    #endif
    __enable_irq();

    // Store DN duty cycle for following half-period
    motor_ptr->oneshunt.duty_u_dn_store = motor_ptr->oneshunt.duty_u_dn;
    motor_ptr->oneshunt.duty_v_dn_store = motor_ptr->oneshunt.duty_v_dn;
    motor_ptr->oneshunt.duty_w_dn_store = motor_ptr->oneshunt.duty_w_dn;

    //debugio1_off();
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline void config_timer_foc(Struct_Motor *motor_ptr)
{
#if defined(FAMILY_52XX)
    // Configure PWM Timer Base
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);
    pac5xxx_timer_clock_config(FOC_PWM_Timer, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                       // Configure timer clock input for ACLK (50MHz), /1 divider
    pac5xxx_timer_base_config(FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 0);  // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDL = 1;                                                           // 07102015, Latch new TAPRD value when timerA counting down, TACTR value = 0x01
    // JAGTAG: Deadtime config has not been changed to PWM timer reference
    pac5xxx_dtg_config(DTGA0, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA0 for phase U
    pac5xxx_dtg_config(DTGA1, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA1 for phase V
    pac5xxx_dtg_config(DTGA2, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA2 for phase W

    // Configure SUPPLE Timer Base
    pac5xxx_timer_clear(FOC_ASYM_SUPPLE_Timer, 1);
    pac5xxx_timer_clock_config(FOC_ASYM_SUPPLE_Timer, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                       // Configure timer clock input for ACLK (100MHz), /1 divider
    pac5xxx_timer_base_config(FOC_ASYM_SUPPLE_Timer, (motor_ptr->control.motor_period_ticks + (motor_ptr->control.motor_period_ticks >> 1)), 0, TxCTL_MODE_UP, 1);

    // Release Timers
    pac5xxx_timer_clear(FOC_PWM_Timer, 0);
    pac5xxx_timer_clear(FOC_ASYM_SUPPLE_Timer, 0);

#elif defined(FAMILY_55XX)
    // Configure PWM Timer Base
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config((TimerInstance) FOC_PWM_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                           // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config((TimerInstance) FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 0);   // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDLATCH = 0;                                                        // Update duty cycle when TACTR counts from 1 to 0
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL0), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL1), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA1 for phase V
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL2), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS);     // Configure DTGA2 for phase W

    // Configure SUPPLE Timer Base
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config(FOC_ASYM_SUPPLE_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                       // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config(FOC_ASYM_SUPPLE_Timer, (motor_ptr->control.motor_period_ticks + (motor_ptr->control.motor_period_ticks >> 1)), 0, TxCTL_MODE_UP, 1);   // Enable timerB

    // Release Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 0;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 0;

#endif
}


static inline void deconfig_timer_foc(void)
{
#if defined(FAMILY_52XX)
    // Disable Timers
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);
    pac5xxx_timer_clear(FOC_ASYM_SUPPLE_Timer, 1);

#elif defined(FAMILY_55XX)
    // Disable Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.CLR = 1;

#endif
}


static inline void config_int_foc(Struct_Motor *motor_ptr)
{
#if defined(FAMILY_52XX)
    // Configure Interrupts
    pac5xxx_timer_base_int_enable(FOC_PWM_Timer, 1);                                               // Enable TimerA base interrupts
    pac5xxx_timer_base_int_enable(FOC_ASYM_SUPPLE_Timer, 1);
    pac5xxx_adc_int_enable_as0(0);                                                          // Disable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(0);                                                          // Disable ADC interrupts for AS1 complete

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);
    NVIC_EnableIRQ(FOC_ASYM_SUPPLE_Timer_IRQn);
    NVIC_SetPriority(FOC_ASYM_SUPPLE_Timer_IRQn,1);

#elif defined(FAMILY_55XX)
    // Configure Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 1;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.BASEIE = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->INT.BASEIF = 1;

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);
    NVIC_EnableIRQ(FOC_ASYM_SUPPLE_Timer_IRQn);
    NVIC_SetPriority(FOC_ASYM_SUPPLE_Timer_IRQn,1);

#endif
}


static inline void deconfig_int_foc(void)
{
#if defined(FAMILY_52XX)
    // Disable Interrupts
    pac5xxx_adc_int_enable_as0(0);                                                          // Disable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(0);                                                          // Disable ADC interrupts for AS1 complete
    pac5xxx_timer_base_int_enable(FOC_PWM_Timer, 0);
    pac5xxx_timer_base_int_enable(FOC_ASYM_SUPPLE_Timer, 0);

#elif defined(FAMILY_55XX)
    // Disable Interrupts
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 0;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;
    FOC_ASYM_SUPPLE_Timer_Ptr->CTL.BASEIE = 0;
    FOC_ASYM_SUPPLE_Timer_Ptr->INT.BASEIF = 1;
#endif
}


static inline void config_int_ctrlctr(void)
{
#if defined(FAMILY_52XX)
    pac5xxx_timer_base_int_enable(FOC_PWM_Timer, 1);                                               // Enable TimerA base interrupts
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#elif defined(FAMILY_55XX)
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 1;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#endif
}


static inline void deconfig_int_ctrlctr(void)
{
#if defined(FAMILY_52XX)
    pac5xxx_timer_base_int_enable(FOC_PWM_Timer, 0);

#elif defined(FAMILY_55XX)
    FOC_PWM_Timer_Ptr->CTL.BASEIE = 0;
    FOC_PWM_Timer_Ptr->INT.BASEIF = 1;

#endif
}

#endif  //#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
#endif  //#ifndef CONTROL_1SHUNT_FUNCS_ASYM_H
