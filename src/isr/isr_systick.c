
/**
 * @file isr_systicks.c
 * @brief Use for systick timer interrupt handler to run non critical FOC functions
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
 
#include "motor_struct.h"
#include "app_funcs.h"
#include "dcbus_check.h"
#include "speed_check.h"
#include "param_funcs.h"
#include "control_funcs.h"
#include "mpos_funcs.h"
#include "vbus_funcs.h"
#include "satmon.h"
#include "VSM_control.h"  //add  by:owz
#include "Driver_Strategy.h"      //add  by:owz

#if (COMMSEL_PPM == TRUE)
#include "ppm.h"
#endif

#ifdef ENABLE_TM
#include "tm.h"
#endif

#if (ENABLE_STACK_CHECK == TRUE)
#include "stack_check.h"
#endif

#ifdef ENABLE_STALL_DETECT
#include "stalldet_funcs.h"
#endif

#ifdef ENABLE_OPD_CL
#include "opd_cl_funcs.h"
#endif

#if defined(ENABLE_FIELD_WEAKENING)
#include "fw_funcs.h"
#endif

#if (ENABLE_WATCHDOG == TRUE)
#include "pac5xxx_driver_watchdog.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
#include "mod_ovm_funcs.h"
#endif

#ifdef ENABLE_AUTOTUNE_PI
#include "atune_funcs.h"
#endif

#if (COMMSEL_PUSH_BUTTON == TRUE)
#include "push_button.h"
#endif

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && defined(ENABLE_LCORR_VBUS)
#include "control_power2_funcs.h"
#endif

#ifdef ENABLE_QEP_LINEARIZATION_TUNING
#include "qep_linearization_sm.h"
#endif

/**
 * @brief Process interrupt for SysTick_Handler to run non critical FOC functions
 */
void SysTick_Handler(void)
{
    static Struct_Motor* motor_ptr = &motor;

    // Increment systick counter
    motor_ptr->app.systick_counter++;
	
	  // VCU控制策略调用
	  system_running_state();

#if (MODULE_MPOS_LBG1 == ENABLED)
    lbg1_statemon(motor_ptr);
#endif

#if (COMMSEL_PPM == TRUE)
    ppm_convert_pulse_width_into_speed(&motor_ptr->app.motor_enable_cmd, &motor_ptr->control.outerloop_target, motor_ptr->state.main, motor_ptr);
#endif

#ifdef ENABLE_STALL_DETECT
    // Filter speed and speec command for stall detection
//    stall_detect_filter(&motor_ptr->stall, motor_ptr->control.outerloop_ramp_scaled_q14, motor_ptr->control.outerloop_fdbk_scaled_q14);
    stall_detect_filter(&motor_ptr->stall, motor_ptr->foc.iqref_ramp_q14, motor_ptr->foc.iq_q14);
#endif

#ifndef DISABLE_AUTOVDC
    // Sample bus voltage
    sample_dcbus(motor_ptr);
#if (MODULE_MPOS_LBG1 == ENABLED)
    lbg1_process_voltage_scale(motor_ptr);
#endif
#endif

#if !defined(DISABLE_AUTOVDC) && defined(ENABLE_DCBUS_MIN_MAX_CHECK)
    // Check Min/Max Bus Voltage
    check_dcbus_fault(motor_ptr);
#endif
    
    #ifdef ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS
    // Run Main State Machine at 2ms
    statemachine_appmain();
    #endif
    
    // Operations done at the divided systick frequency
    if (++motor_ptr->app.systick_divider_cnt >= SYSTICK_HANDLER_DIVIDER)
    {
        motor_ptr->app.systick_divider_cnt = 0;
        ++motor_ptr->app.systick_div_counter;

        // Check for voltage vector saturation
        check_pwm_sat(motor_ptr);

        #if (ENABLE_WATCHDOG == TRUE)
        // WDT Refresh
        pac5xxx_watchdog_reset();
        #endif

        #if (ENABLE_STACK_CHECK == TRUE)
        // Test stack over and under flow
        stack_test();
        #endif

        #if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
        check_overmod_status(motor_ptr);
        #endif

        #ifdef ENABLE_CLSPEED_MIN_MAX_CHECK
        // Check Min/Max Closed-Loop Speed
        check_speedlimitcl_fault(motor_ptr);
        #endif

        #ifdef ENABLE_FIELD_WEAKENING
        // Field Weakening
        fw_controller(motor_ptr);
        #endif

        #if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && defined(ENABLE_LCORR_VBUS)
        // Power measurement compensation coefficient calculation
        power_compensation_coefficient_calc(motor_ptr);
        #endif

        #if defined(ENABLE_STALL_DETECT)
        // Stall Detection
        store_peak_speed_vdrive_value(motor_ptr);
        stall_detect_process(motor_ptr);
        #endif

        #ifdef ENABLE_OPD_CL
        // Closed-Loop Open Phase Detection
        check_opdcl_fault(motor_ptr);
        #endif

        #ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
        check_speedlimitcl_need_to_change(motor_ptr);
        #endif

        // Periodic PI gain converter
        picon_convert_gain(&motor_ptr->foc.Id_pid, motor_ptr->app.descale_current_factor, motor_ptr->app.v_scale_q16);
        picon_convert_gain(&motor_ptr->foc.Iq_pid, motor_ptr->app.descale_current_factor, motor_ptr->app.v_scale_q16);

        #if defined(ENABLE_AUTOTUNE_MID)
        // Process AT systick ISR operations
        mid2_process_systick(motor_ptr);
        #endif
        
        #if defined(ENABLE_AUTOTUNE_PI)
        // Process AT systick ISR operations
        atpi_process_systick(motor_ptr);
        #endif

        #if (COMMSEL_PUSH_BUTTON == TRUE)
        statemachine_buttoncontrol();
        #endif

        #if defined(ENABLE_TEMPREAD)
        temp_read();
        #endif
        
        #ifndef ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS
        // Main State Machine
        statemachine_appmain();
        #endif
        
        #ifdef ENABLE_QEP_LINEARIZATION_TUNING
        qep_linearization_sm(motor_ptr);
        #endif
    }
}


