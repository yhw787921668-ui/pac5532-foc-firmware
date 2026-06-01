/**
 * @file opd_cl_funcs.c
 * @brief Use for control functions in OPD
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

#include "opd_cl_funcs.h"
#include "opd_cl_config.h"
#include "fault.h"
//#include "debug.h"
#include "utils.h"

#ifdef ENABLE_OPD_CL

///
/// @brief  Calculate motor open winding by comparing all three phase current
///
void check_opdcl_fault(Struct_Motor* motor_ptr)
{
    uint32_t flag_opdcl_fault = 0;
    Struct_OPDCL* opdcl_ptr = &motor_ptr->opdcl;

    // Check is any phase peak current fall below current threshold set error flag
#if !defined(ENABLE_OPD_CL_FAST)
    if ((motor_ptr->app.systick_div_counter - opdcl_ptr->phase_open_fault_last_sample_time_value) >= opdcl_ptr->opdcl_sampletime_systicks)
    {
        opdcl_ptr->phase_open_fault_last_sample_time_value = motor_ptr->app.systick_div_counter;
        if ((motor_ptr->state.main == STATE_MAIN_RUN) && (absval(motor_ptr->mpos.mpos_spd_radps) > motor_ptr->opdcl.opdcl_minspeed_radps_q16) && motor_ptr->opdcl.flag_prev_spd_above_min)
#else
        static int32_t speed_filtered, torque_feedback;
        speed_filtered += (speed_filter_cofficient * ((absval(motor_ptr->mpos.mpos_spd_radps) - speed_filtered) >> 10));//q16
        torque_feedback += (torque_filter_cofficient * (absval(motor_ptr->foc.iq_q14) - torque_feedback)) >> 10;//q14
        // Test only
    #if 0
//        debug_var_1 = motor_ptr->opdcl.Iu_sq_q24 >> 8;
//        debug_var_2 = motor_ptr->opdcl.Iu_sq_f_q24 >> 8;
//        debug_var_3 = motor_ptr->foc.iqref_ramp_q14 << 2;
//        debug_var_4 = torque_feedback << 2;
//        debug_var_5 = motor_ptr->foc.iq_q14 << 2;
//        debug_dac_2b = motor_ptr->opdcl.opdcl_current_thd_sq >> 4;//motor_ptr->opdcl.Iu_sq_q24;
//        debug_dac_1b = motor_ptr->opdcl.Iu_sq_f_q24 >> 4;
//        debug_dac_1c = motor_ptr->foc.iqref_ramp_q14 << 2;
//        debug_dac_2c = motor_ptr->opdcl.opdcl_current_thd >> 8;
    #endif
    {
        // OPD detection conditions:
        // 1. motor state machine is running
        // 2. speed is higher than the minimum level
        // 3. iq current is higher than the current threshold
        // 4. squared phase current is less than the threshold, which is handled in process_opdcl_fast()
        // 5. iqref needs to be N time of the phase current threshold because of the variable response of outer speed loop
        if ((motor_ptr->state.main == STATE_MAIN_RUN) && (speed_filtered > motor_ptr->opdcl.opdcl_minspeed_radps_q16) && motor_ptr->opdcl.flag_prev_spd_above_min
                && ((absval(motor_ptr->foc.iqref_ramp_q14) << 10) > (OPD_MIN_TORQUE_SCALE * motor_ptr->opdcl.opdcl_current_thd))
                && (((torque_feedback << 10) > (OPD_MIN_TORQUE_SCALE * motor_ptr->opdcl.opdcl_current_thd)) || !OPD_LARGE_INERTIA))
#endif
        {
            motor_ptr->opdcl.flag_prev_spd_above_min = TRUE;
            // Process OPD data
            #if defined(ENABLE_OPD_CL_FAST)
            flag_opdcl_fault = process_opdcl_fast(opdcl_ptr, PHASE_THRESHOLD_BIT_SHIFT);
            #else
            flag_opdcl_fault = process_opdcl(opdcl_ptr, (motor.fault.motor & WARNING_PWM_SATURATED));
            #endif
            if (flag_opdcl_fault)    set_fault(FAULT_OPD);
        }
    #if !defined(ENABLE_OPD_CL_FAST)
        else if (absval(motor_ptr->mpos.mpos_spd_radps) > motor_ptr->opdcl.opdcl_minspeed_radps_q16)
    #else
        else if (speed_filtered > motor_ptr->opdcl.opdcl_minspeed_radps_q16)
    #endif
        {
            motor_ptr->opdcl.flag_prev_spd_above_min = TRUE;
        }
        else
        {
            motor_ptr->opdcl.flag_prev_spd_above_min = FALSE;
            opdcl_ptr->Sum_Iu_p = opdcl_ptr->Sum_Iv_p = opdcl_ptr->Sum_Iw_p = opdcl_ptr->Sum_Iu_m = opdcl_ptr->Sum_Iv_m = opdcl_ptr->Sum_Iw_m = 0;
        }
    }
}


void calc_opdcl_sample_time(Struct_Motor* motor_ptr, int32_t speedref_min, int32_t systick_dt)
{
    fix16_t temp_var;

    motor_ptr->opdcl.opdcl_minspeed_radps_q16 = speedref_min;
    if (motor_ptr->opdcl.opdcl_minspeed_radps_q16 < OPDCL_MINSPEED_RADPS_Q16)
        motor_ptr->opdcl.opdcl_minspeed_radps_q16 = OPDCL_MINSPEED_RADPS_Q16;

    // Calculate Open Phase Detection Sample time depending on min CL speed
    temp_var = fix16_mul(motor_ptr->opdcl.opdcl_minspeed_radps_q16, RAD2HZ_FIX16);             // MIN_CL_SPEED(Hz,f16) = MIN_CL_SPEED(rad/s,f16) * RAD2HZ_FIX16(f16)

    // Check for divide by 0 condition
    if (temp_var <= 0)
        temp_var = 1;

    temp_var = fix16_div(FIX16_1P0, temp_var);                      // sample_time(sec,f16) = 1(f16) / MIN_CL_SPEED(Hz,f16)
    motor_ptr->opdcl.opdcl_one_div_sample_time = temp_var;
    temp_var = fix16_div(temp_var, systick_dt) >> 16;               // systick_counts(f16>>16=q0) = sample_time(f16) / dt_systick(f16)

    if (temp_var <= 0)
    {
        temp_var = 1;
    }

    motor_ptr->opdcl.opdcl_sampletime_systicks = temp_var;
#if defined(ENABLE_OPD_CL_FAST)
    // Effective pwm freq / systemtick to get the filter time const
    motor_ptr->opdcl.filter_coef_q10 = (motor_ptr->control.dt_control_q26 *OPDCL_SYSTEMTICK_CONST) >> 16;  //q26->16 = q10
    // The current threshold for opd is based on the percentage of the maximum controllable current.
    motor_ptr->opdcl.opdcl_current_thd = (motor_ptr->control.imax_q16 * OPDCL_CURRENT_THRESHOLD_Q14) >> 6; //q24
    // square the phase current threshold
    motor_ptr->opdcl.opdcl_current_thd_sq = ((motor_ptr->opdcl.opdcl_current_thd >> 10) * (motor_ptr->opdcl.opdcl_current_thd >> 10)) >> 4; // q14*q14=q28->4=q24
    //debug_var_5 = motor_ptr->opdcl.opdcl_current_thd_sq >> 8;
#endif
}

void opdcl_init(Struct_Motor* motor_ptr)
{
    motor_ptr->opdcl.opdcl_thd_percentage_q3 = OPDCL_THRESHOLD_PCT;
    motor_ptr->opdcl.opd_count_threshold = OPDCL_COUNT_THRESHOLD;
    motor_ptr->opdcl.opdcl_current_thd = OPDCL_CURRENT_THRESHOLD_Q14;
}

void opdcl_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->opdcl.flag_prev_spd_above_min = FALSE;
}

void opdcl_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Calculate detection window
    switch(param_code)
    {
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    case open_loop_speed_setpoint:
#elif (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP)
    case max_cl_speed_limit:
#endif
        calc_opdcl_sample_time(motor_ptr, motor_ptr->app.speedrefcl_min, DT_DIVSYSTICK_Q16);
        break;
    default:
        break;
    }
}

#endif  //#ifdef ENABLE_OPD_CL
