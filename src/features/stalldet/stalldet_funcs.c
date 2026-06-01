
/**
 * @file stalldet_funcs.c
 * @brief Use for stall detect utility functions
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
#include "stalldet_funcs.h"
#include "stalldet_config.h"
#include "maths.h"
#include "fault.h"

#ifdef ENABLE_STALL_DETECT

void stall_detect_process(Struct_Motor* motor_ptr)
{
    if ((motor_ptr->state.main == STATE_MAIN_RUN) || (motor_ptr->state.main == STATE_MAIN_COASTING))
    {
        motor_ptr->stall.flag_event1 = 0;
        motor_ptr->stall.flag_event2 = 0;
        motor_ptr->stall.flag_event3 = 0;
        motor_ptr->stall.flag_event4 = 0;

        stall_detect_1(&motor_ptr->stall, motor_ptr->app.systick_div_counter, motor_ptr->app.motor_speed_peak_stored,
            motor_ptr->app.descale_voltage_factor, motor_ptr->app.vdrvsq_q14_peak_stored, motor_ptr->foc.iq_q14);

#if defined(ENABLE_STALL_DETECT_NEGSPD)
        if (motor_ptr->mpos.state == STATE_MPOS_ESTIMATED_ANGLE)
        {
            stall_detect_2(&motor_ptr->stall, motor_ptr->mpos.est.est_spd_radps, motor_ptr->app.app_motor_direction);
        }
#endif

        // Stall detection time blanking. First event starts the stall timeout period. Fault is triggered if CNT_LIMIT is reached within the stall timeout period.
        if (motor_ptr->stall.flag_event1 || motor_ptr->stall.flag_event2 || motor_ptr->stall.flag_event3 || motor_ptr->stall.flag_event4)
        {
            if (motor_ptr->stall.ctr_event == 0)
            {
                motor_ptr->stall.flag_process = 1;
                motor_ptr->stall.systick_stamp = motor_ptr->app.systick_div_counter;
            }
            motor_ptr->stall.ctr_event++;

            if (motor_ptr->stall.ctr_event >= STALL_EVENT_CNT_LIMIT)
            {
                set_fault(FAULT_MOTOR_STALLED);
                motor_ptr->stall.ctr_event = 0;
            }
        }

    #ifdef ENABLE_STALL_DETECT_STEADY_STATE
    // In case of PWM Saturation, Stall detection counter is reset to avoid false trigger
        if ((motor_ptr->foc.flag_sat_logicor == 1) || (motor_ptr->app.speed_mode_flag != 1))
        {
            motor_ptr->stall.ctr_event = 0;
        }
    #endif
    
        if (motor_ptr->stall.flag_process)
        {
            if ((motor_ptr->app.systick_div_counter - motor_ptr->stall.systick_stamp) >= motor_ptr->stall.timeout_systicks)
            {
                motor_ptr->stall.ctr_event = 0;
                motor_ptr->stall.ctr_event_1 = motor_ptr->stall.ctr_event_2 = motor_ptr->stall.ctr_event_3 = motor_ptr->stall.ctr_event_4 = 0;
                motor_ptr->stall.flag_process = 0;
            }
        }
    }

    // Check if stall.bemf_constant set to 0, just post warning
    // To do: Temporarily commented out, To be fixed in GUI since two Ke boxes existed
    /*if (motor_ptr->stall.bemf_constant <= 0)
        set_warning(WARNING_KE_VALUE);
    else
        clear_warning(WARNING_KE_VALUE);
    */
}


void stall_detect_calculate_stall_sample_time(Struct_Motor* motor_ptr, int32_t sample_window_freq_radps_q16)
{
    fix16_t temp_var;

    // In systick timer counts, Tdesired = 1/MIN_CL_SPEED(Hz) = 1/50 = 20ms, SystickCounts = Tdesired(20ms) / systick_dt(10ms)
    temp_var = fix16_mul(sample_window_freq_radps_q16, RAD2HZ_FIX16);          // MIN_CL_SPEED(Hz,f16) = MIN_CL_SPEED(rad/s,f16) * RAD2HZ_FIX16(f16)
    temp_var = fix16_div(0x10000, temp_var);                                    // sample_time(sec,f16) = 1(f16) / MIN_CL_SPEED(Hz,f16)
    temp_var = ((fix16_div(temp_var, DT_DIVSYSTICK_Q16) + 0x8000) >> 16);      // systickCounts(f16>>16=q0) = sample_time(sec,f16) / systick_dt(sec,f16)

    if (temp_var <= 0)
    {
        temp_var = 1;
    }

    motor_ptr->stall.sampletime_systicks = temp_var;
}


void stalldet_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->stall.ctr_event_1 = 0;
    motor_ptr->stall.ctr_event_2 = 0;
    motor_ptr->stall.ctr_event_3 = 0;
    motor_ptr->stall.ctr_event_4 = 0;
    motor_ptr->app.motor_speed_peak_stored = 0;
    motor_ptr->app.motor_speed_peak = 0;
    motor_ptr->stall.speed_est = 0;
    motor_ptr->stall.speed_cmd = 0;
    motor_ptr->stall.ctr_event = 0;

#ifdef ENABLE_CBC_CURRENT_LIMIT
    // Reset CBC flag
    motor_ptr->stall.flag_cbc_triggered = 0;
#endif
}


void stalldet_calc_filter_alpha(Struct_Motor* motor_ptr)
{
    motor_ptr->stall.filter_alpha = fix16_div(DT_SYSTICK_Q16, (DT_SYSTICK_Q16 + fix16_div(FIX16_ONE, fix16_mul(FIX16_2PI, STALL_SPEED_FILTER_FC_Q16))));

    // Check if 0 and set warning
    if (motor_ptr->stall.filter_alpha == 0)
    {
        motor_ptr->stall.filter_alpha = 1;
        set_warning(WARNING_STALLFILTGAIN);
    }
}


void stalldet_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Parameter handlers
    switch(param_code)
    {
    case Config_Ke:
        //motor_ptr->stall.bemf_constant = (rx_data >> 10);        // q16
        break;
    case Stall_Detection_TimeOut:
        motor_ptr->stall.timeout_systicks = rx_data;
        break;
    case Motor_BEMF_const:
        motor_ptr->stall.bemf_constant = rx_data;
        break;
    default:
        break;
    }
}


void stalldet_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Calculate detection window
    switch(param_code)
    {
    case open_loop_speed_setpoint:  // JAGTAG: Need to make this case conditional
    case Stall_Detection_TimeOut:
        // Set Sampling window frequency = OL2CL/2
        stall_detect_calculate_stall_sample_time(motor_ptr, motor_ptr->app.speedrefcl_min);
        break;
    default:
        break;
    }

    // Calculate filter gain
    switch(param_code)
    {
    case dt:
        stalldet_calc_filter_alpha(motor_ptr);
        break;
    default:
        break;
    }
}

#endif  //#ifdef ENABLE_STALL_DETECT
