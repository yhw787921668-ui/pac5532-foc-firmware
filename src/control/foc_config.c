
/**
 * @file foc_config.c
 * @brief Use for FOC configuration
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#include "motor_struct.h"
#include "foc.h"
#include "param_calc.h"
#include "control_funcs.h"
#include "control_loop.h"
#include "foc_config.h"
#include "utils.h"
#include "control_check.h"


void foc_config(Struct_Motor* motor_ptr, int32_t flag_pwm_timer_config)
{
    // Set ISR pointers to FOC
    foc_fptr_config(motor_ptr);

    // Set PWM period to FOC
    motor_ptr->control.motor_period_ticks = motor_ptr->foc.motor_period_ticks;
    motor_ptr->control.motor_period_ticks_1div = fix16_div(FIX16_1P0, (motor_ptr->control.motor_period_ticks << 16));

    // Set PWM duty cycles to 50%
    m1_u_set_duty(motor_ptr->control.motor_period_ticks >> 1);                                          // Set initial duty cycle to 50% (off)
    m1_v_set_duty(motor_ptr->control.motor_period_ticks >> 1);                                          // Set initial duty cycle to 50% (off)
    m1_w_set_duty(motor_ptr->control.motor_period_ticks >> 1);                                          // Set initial duty cycle to 50% (off)

    // Configure sequencers for FOC
    config_seq_foc();

    // Configure timers and interrupts for FOC
    if (flag_pwm_timer_config)
    {
        config_timer_foc(motor_ptr);
        config_ctrlctr(motor_ptr);
    }
    config_int_foc(motor_ptr);

    motor_ptr->state.precontrol = STATE_PRECONTROL_FOC;

    // Configure control divider for FOC
    config_control_dt(motor_ptr, motor_ptr->foc.dt_control_q26, motor_ptr->foc.dt_control_1div_q6, motor_ptr->foc.ctrl_divider_foc);

    // Configure min/max duty
    foc_config_duty(motor_ptr);

#ifdef CONTROL_VDVQ_VECTOR_LIMIT
    config_vdvq_vector_limit(motor_ptr);
#endif

#ifdef ENABLE_BOOTPROT
    fix16_t temp3;
    temp3 = BOOTPROT_TIME_LIMIT_SEC_x1024; //- motor_ptr->foc_dt;
    if (temp3 < 0)
        temp3 = 0;
    motor_ptr->bootprot.bootprot_max_counts = (fix16_mul(temp3, motor_ptr->control.dt_control_1div_q6) >> 16);    // q0 = q16>>16

    motor_ptr->bootprot.bootprot_pwm_max_ticks = (fix16_mul((fix16_t)(motor_ptr->control.motor_period_ticks << 16), BOOTPROT_DUTY_MAX_PCT_Q16) >> 16);
#endif

    motor_ptr->foc.state_foc = STATE_FOC_FULLFLOW;
}


void foc_deconfig(Struct_Motor* motor_ptr)
{
    motor_ptr->state.precontrol = STATE_PRECONTROL_IDLE;

    // De-Configure timers and interrupts
    deconfig_timer_foc();
    deconfig_ctrlctr(motor_ptr);
    deconfig_int_foc();

     // De-Configure sequencer triggers
    deconfig_seq();

    // De-Configure pointers
    foc_fptr_deconfig(motor_ptr);
}


void foc_fptr_config(Struct_Motor* motor_ptr)
{
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    //////////////////////////////////////////////
    // CONFIG_SHUNT_3SHUNT
    //////////////////////////////////////////////
    motor_ptr->fptr.fptr_int_ADC                    =    (void (*)(void *)) &control_loop;
#ifdef ENABLE_INT_ADC
#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    motor_ptr->fptr.fptr_int_clr                    =    (void (*) (void)) &clear_adc_int_flag; 
    motor_ptr->fptr.fptr_int_check                  =    (int32_t(*) (void)) &read_adc_int_flag;
    motor_ptr->fptr.fptr_int_clr_0                  =    (void (*) (void)) &clear_adc_int_flag; 
#endif
#endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    //////////////////////////////////////////////
    // MODE_1SHUNT_EXTENSION
    //////////////////////////////////////////////
    motor_ptr->fptr.fptr_int_ADC                    =    (void (*)(void *)) &control_loop;
#ifdef ENABLE_INT_ADC
#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    motor_ptr->fptr.fptr_int_clr                    =    (void (*) (void)) &clear_adc_int_flag; 
    motor_ptr->fptr.fptr_int_check                  =    (int32_t(*) (void)) &read_adc_int_flag;
    motor_ptr->fptr.fptr_int_clr_0                  =    (void (*) (void)) &clear_adc_int_flag;    
#endif
#endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TB_base            =    (void (*)(void *)) &control_loop;
        motor_ptr->fptr.fptr_int_TA_base            =    (void (*)(void *)) &control_1shunt_asym_update_dutycycledn_seqtrigger;
         #ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
             motor_ptr->fptr.fptr_int_clr           =    (void (*) (void)) &clear_timer_b_interrupt_flags; 
             motor_ptr->fptr.fptr_int_check         =    (int32_t(*) (void)) &read_timer_b_base_interrupt_flags;
             motor_ptr->fptr.fptr_int_clr_0         =    (void (*) (void)) &clear_timer_b_interrupt_flags;    
         #endif

    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TA_base            =    (void (*)(void *)) &control_loop;
        motor_ptr->fptr.fptr_int_TB_base            =    (void (*)(void *)) &control_1shunt_asym_update_dutycycledn_seqtrigger;
         #ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
             motor_ptr->fptr.fptr_int_clr           =    (void (*) (void)) &clear_timer_a_interrupt_flags; 
             motor_ptr->fptr.fptr_int_check         =    (int32_t(*) (void)) &read_timer_a_base_interrupt_flags;
             motor_ptr->fptr.fptr_int_clr_0         =    (void (*) (void)) &clear_timer_a_interrupt_flags;    
         #endif
#endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC_TIMER_55XX
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TB_ccr2            =    (void (*)(void *)) &control_loop;
        #ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
             motor_ptr->fptr.fptr_int_clr           =    (void (*) void)) &clear_timer_b_interrupt_flags; 
             motor_ptr->fptr.fptr_int_check         =    (int32_t(*) (void)) &read_timer_b_ccr2_interrupt_flag;
             motor_ptr->fptr.fptr_int_clr_0         =    (void (*) (void)) &clear_timer_b_interrupt_flags;    
        #endif
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TA_ccr2            =    (void (*)(void *)) &control_loop;
        #ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
             motor_ptr->fptr.fptr_int_clr           =    (void (*)) &clear_timer_a_interrupt_flags; 
             motor_ptr->fptr.fptr_int_check         =    (int32_t(*) (void)) &read_timer_a_ccr2_interrupt_flag;
             motor_ptr->fptr.fptr_int_clr_0         =    (void (*) (void)) &clear_timer_a_interrupt_flags;    
        #endif
    #endif

#endif
}


void foc_fptr_deconfig(Struct_Motor* motor_ptr)
{
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    //////////////////////////////////////////////
    // CONFIG_SHUNT_3SHUNT
    //////////////////////////////////////////////
    motor_ptr->fptr.fptr_int_ADC                    =    (void (*)(void *)) &empty_function;

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    //////////////////////////////////////////////
    // MODE_1SHUNT_EXTENSION
    //////////////////////////////////////////////
    motor_ptr->fptr.fptr_int_ADC                    =    (void (*)(void *)) &empty_function;

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TB_base            =    (void (*)(void *)) &empty_function;
        motor_ptr->fptr.fptr_int_TA_base            =    (void (*)(void *)) &empty_function;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TA_base            =    (void (*)(void *)) &empty_function;
        motor_ptr->fptr.fptr_int_TB_base            =    (void (*)(void *)) &empty_function;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC_TIMER_55XX
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TB_ccr2            =    (void (*)(void *)) &empty_function;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TA_ccr2            =    (void (*)(void *)) &empty_function;
    #endif

#endif

#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    motor_ptr->fptr.fptr_int_clr                    =    (void (*) (void)) &empty_function; 
    motor_ptr->fptr.fptr_int_check                  =    (int32_t (*) (void)) &empty_function;
    motor_ptr->fptr.fptr_int_clr_0                  =    (void (*) (void)) &empty_function;
#endif
}
