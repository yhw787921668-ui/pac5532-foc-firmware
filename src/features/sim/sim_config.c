
/**
 * @file sim_config.c
 * @brief Use for SIM configuration
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

#include "isr_config.h"
#include "config_features.h"
#include "motor_struct.h"
#include "sim_config.h"
#include "utils.h"
#include "control_funcs.h"
#include "control_loop.h"
#include "mpos_config.h"


#if defined(ENABLE_SIM)

/**
 * @brief This function use for SIM sequencer and Interrupt initialization
 */
void sim_config(Struct_Motor* motor_ptr)
{
    // Config pointers
    sim_fptr_config(motor_ptr);

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &empty_function;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &hall_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
    motor_ptr->mpos.state = STATE_MPOS_HALL_ANGLE_INTERPOLATE;
#elif (MPOS_SELECT == MPOS_LBG1_START_HALL)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &hall_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
    motor_ptr->mpos.state = STATE_MPOS_HALL_ANGLE_INTERPOLATE;
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &qep_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
    motor_ptr->mpos.state = STATE_MPOS_QEP;
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &qep_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
    motor_ptr->mpos.state = STATE_MPOS_QEP;
#endif


    // Config sequencer
    config_seq_sim();

    // Configure timers and interrupts
    config_timer_foc(motor_ptr);
    config_ctrlctr(motor_ptr);
    config_int_sim();

    motor_ptr->state.precontrol = STATE_PRECONTROL_SIM;

    // Configure control divider
    config_control_dt(motor_ptr, motor_ptr->sim.dt_control_q26, motor_ptr->sim.dt_control_1div_q6, SIM_CONTROL_DIVIDER);
}


void sim_deconfig(Struct_Motor* motor_ptr, int32_t flag_pwm_timer_deconfig)
{
    motor_ptr->state.precontrol = STATE_PRECONTROL_IDLE;

    // De-Configure timers and interrupts
    if (flag_pwm_timer_deconfig)
    {
        deconfig_timer_foc();
        deconfig_ctrlctr(motor_ptr);
    }
    deconfig_int_sim();

    // De-configure sequencer triggers
    deconfig_seq();

    // De-Configure pointers
    sim_fptr_deconfig(motor_ptr, flag_pwm_timer_deconfig);
}


void sim_fptr_config(Struct_Motor* motor_ptr)
{
#ifdef ENABLE_INT_ADC
    motor_ptr->fptr.fptr_int_ADC                =    (void (*)(void *)) &control_loop;
#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    motor_ptr->fptr.fptr_int_clr                    =    (void (*) (void)) &clear_adc_int_flag; 
    motor_ptr->fptr.fptr_int_check                  =    (int32_t (*) (void)) &read_adc_int_flag;
    motor_ptr->fptr.fptr_int_clr_0                  =    (void (*) (void)) &clear_adc_int_flag;
#endif
#endif
}


void sim_fptr_deconfig(Struct_Motor* motor_ptr, int32_t flag_pwm_timer_deconfig)
{
#ifdef ENABLE_INT_ADC
    motor_ptr->fptr.fptr_int_ADC                =    (void (*)(void *)) &empty_function;
#endif

#ifdef ENABLE_DEBUG
#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    motor_ptr->fptr.fptr_int_clr                    =    (void (*) (void)) &empty_function; 
    motor_ptr->fptr.fptr_int_check                  =    (int32_t (*) (void)) &empty_function;
    motor_ptr->fptr.fptr_int_clr_0                  =    (void (*) (void)) &empty_function;
#endif
#endif
}


void config_int_sim(void)
{
#if defined(FAMILY_52XX)
    // Configure Interrupts
    pac5xxx_adc_int_enable_as0(1);                                                          // Enable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(0);                                                          // Disable ADC interrupts for AS1 complete

    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn,1);

#elif defined(FAMILY_55XX)
    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn,1);

#endif
}


void deconfig_int_sim(void)
{
#if defined(FAMILY_52XX)
    // De-configure NVIC
    NVIC_DisableIRQ(ADC_IRQn);

#elif defined(FAMILY_55XX)
    // De-configure NVIC
    NVIC_DisableIRQ(ADC_IRQn);

#endif  //defined(FAMILY_XXXX)
}

#endif //#if defined(ENABLE_SIM)
