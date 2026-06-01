
/**
 * @file startup_aligngo2est_funcs.h
 * @brief Use for hall startup functions
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

#ifndef STARTUP_ALIGNGO2EST_FUNCS_H
#define STARTUP_ALIGNGO2EST_FUNCS_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "aligngo_funcs.h"
#include "param_funcs.h"
#include "control_funcs.h"
#include "lbg1_funcs.h"
#include "anglediff.h"
#include "utils.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mpos_config.h"

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
/**
* @brief Use to check for startup complete
* @param Struct_Motor* motor_ptr
* @return true or false
*/
static inline int32_t startup_complete(Struct_Motor* motor_ptr)
{
#ifdef ENABLE_PLL_ANGLE_CALCULATION
    return ((motor_ptr->startup.state == STATE_STARTUP_ALIGNGO_COMPLETE) && (motor_ptr->mpos.est_pll.est_pll_transition_complete));
#else
    return (motor_ptr->startup.state == STATE_STARTUP_ALIGNGO_COMPLETE);
#endif
}


/**
* @brief Use to set startup state to reset
* @param Struct_Motor* motor_ptr
*/
static inline void startup_reset_state(Struct_Motor* motor_ptr)
{
    motor_ptr->startup.state = STATE_STARTUP_ALIGNGO_RESET;
}

#if defined(ENABLE_SWITCH_OVER_SPEED_BLEND)
static inline void switch_over_blend_time_calc(Struct_Motor* motor_ptr)
{
    int32_t lbg_bandwidth_ramp_time_q26, temp_q16;
    if (motor_ptr->mpos.est.w0_inc_pct_q16 > 0)        
    temp_q16 = fix16_div((QCONST(1.0f, Q16) - motor_ptr->mpos.est.starting_w0_q16), motor_ptr->mpos.est.w0_inc_pct_q16);
    lbg_bandwidth_ramp_time_q26 = (temp_q16 >> 6) * DT_SYSTICK_Q16;                 // Unit: seconds, q10*q16 = q26
    motor_ptr->startup.aligngo2est_blend_total_cnt = fix16_mul(motor_ptr->control.dt_control_1div_q6, lbg_bandwidth_ramp_time_q26) >> 16;
    if (motor_ptr->startup.aligngo2est_blend_total_cnt > 0)
    motor_ptr->startup.aligngo2est_blend_total_cnt_1div_q12 = fix16_div(QCONST(1.0f,Q16) >> 4, (motor_ptr->startup.aligngo2est_blend_total_cnt << 16));
    motor_ptr->startup.aligngo2est_speedblend_cnt = motor_ptr->startup.aligngo2est_blend_total_cnt;

    // For debug only
    #if 0
    debug_var_1 = temp_q16;
    debug_var_2 = lbg_bandwidth_ramp_time_q26 >> 10;
    debug_var_3 = motor_ptr->startup.aligngo2est_blend_total_cnt << 16;
    debug_var_4 = motor_ptr->startup.aligngo2est_blend_total_cnt_1div_q12 << 4;
    #endif
}
#endif

/**
* @brief Use to reset startup variables
* @param Struct_Motor* motor_ptr
*/
static inline void startup_reset_vars(Struct_Motor* motor_ptr)
{
    // Set aligngo fixed command values
#ifdef ENABLE_IPD
    motor_ptr->startup.aligngo.alignangle_cmd_q26 = motor_ptr->ipd.ipd_starting_angle;
#else
    motor_ptr->startup.aligngo.alignangle_cmd_q26 = (FIX16_PI_DIV_2 << 10);
#endif

    // Set aligngo variables
    aligngo_setvars(motor_ptr, &motor_ptr->startup.aligngo);

    // Set control variables
    motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
    motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
    motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;

#ifdef ENABLE_SIM
    fix16_t temp_value;

    // SIM startup
    if (motor_ptr->sim.status_sim & (STATUS_SIM_TIMEOUT | STATUS_SIM_BRAKECOMPLETE | STATUS_SIM_WAITSTOPCOMPLETE))
    {
        // Set extra align time
        motor_ptr->mpos.aligngo.aligntime_target = motor_ptr->sim.align_count_braking_timeout;
        // Set extra current
        motor_ptr->mpos.aligngo.idrefol_target_q16 = fix16_mul(motor_ptr->startup.aligngo.idrefol_cmd_q16, motor_ptr->sim.OL_Id_ref_braking_mul);
        temp_value = fix16_mul(motor_ptr->startup.aligngo.itotrefol_cmd_q16, motor_ptr->sim.OL_Id_ref_braking_mul);
        write_itotstart_target(temp_value, &motor_ptr->mpos.aligngo.itotrefol_target_q16, &motor_ptr->mpos.aligngo.itotrefol_target_1div_q16);
    }
#endif

#if defined(ENABLE_SWITCH_OVER_SPEED_BLEND)
    // For speed or angle blending during switch over
    switch_over_blend_time_calc(motor_ptr);
#endif

#if defined(ENABLE_SWITCH_OVER_ANGLE_BLEND)
    motor_ptr->startup.aligngo2est_angleblend_cnt = ALIGNGO2EST_ANGLEBLEND_TOTALCNT;
    motor_ptr->startup.angle_wrap_flag = 0;
#endif

#if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
    motor_ptr->startup.aligngo2est_angleramp_q16 = 0;
#endif
}


/**
* @brief Use for startup controller
* @param Struct_Motor* motor_ptr
*/
static inline void startup_controller(Struct_Motor* motor_ptr)
{
    switch(motor_ptr->startup.state)
    {
        case STATE_STARTUP_ALIGNGO_RESET:
            // Reset running variables and sub-states
            control_reset(motor_ptr);
            lbg1_reset(motor_ptr);
            startup_reset_vars(motor_ptr);
            aligngo_reset_state(motor_ptr);
#ifdef ENABLE_OPDOL
            reset_opdol(motor_ptr);
#endif
            // Configure control loop for FOC mode
            foc_config(motor_ptr, 1);
            mpos_config_fptr(motor_ptr);

            // Next state
            __disable_irq();
            motor_ptr->startup.state = STATE_STARTUP_ALIGNGO_RUN;
            __enable_irq();
            break;

        case STATE_STARTUP_ALIGNGO_RUN:
            // Run Align&Go controller
            aligngo_controller(motor_ptr);

            // Next state
            if ((motor_ptr->mpos.aligngo.status == STATUS_ALIGNGO_COMPLETE) && motor_ptr->app.auto_close_loop_cmd)
            {
                motor_ptr->startup.state = STATE_STARTUP_ALIGNGO_ANGLEDIFF;
            }
            break;

        case STATE_STARTUP_ALIGNGO_ANGLEDIFF:
            // AngleDiff and SpeedDiff Check for CL Switchover
            anglediff_check(motor_ptr, (motor_ptr->mpos.aligngo.alignangle_q26 >> 10), motor_ptr->mpos.est.estimated_angle, motor_ptr->mpos.aligngo.speedrefol_target_radps_q16, motor_ptr->mpos.est.est_spd_radps);

            // Only switch from OL to CL if auto closed loop is enabled and OL to CL conditions are satisfied (angle diff and speed)
            if (motor_ptr->mpos.anglediff_pass_flag)
            {
                motor_ptr->mpos.anglediff_pass_flag = 0;
                __disable_irq();
                // Set estimator to run in CL mode
                lbg1_switch2cl(&motor_ptr->mpos.est);
                // Initialize Speed PID before switching to CL mode: Keep Iqref unchanged during switching-over
                #if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
                init_pi(&motor_ptr->control.outerloop_pid, (motor_ptr->foc.iqref_ramp_q14 * motor_ptr->app.app_motor_direction));
                #else
                init_pi(&motor_ptr->control.outerloop_pid, motor_ptr->foc.iqref_ramp_q14);
                #endif

                // Set transition parameters
#if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_1)
                motor_ptr->control.idiqref_rate_f16 = (motor_ptr->control.idiqref_rate_gui_f16 >> IDREF_RAMP_RATE_SHIFT_Q00);
#endif
                motor_ptr->control.outerloop_rate = 0;

                #if defined(ENABLE_SWITCH_OVER_SPEED_BLEND)
                // Store the switchover speed for blending the speed while lbg bandwidth is ramping
                motor_ptr->mpos.est.speed_switchover = motor_ptr->mpos.est.est_spd_radps;
                #endif
                #if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
                motor_ptr->startup.aligngo2est_angleramp_inc_radpdt_q16 = fix16_mul(fix16_mul(ALIGNGO2EST_ANGLERAMP_INCREMENT_DEGPS_Q06, CONV_DEG2RAD_FIX16), motor_ptr->control.dt_control_q26);
                if (motor_ptr->startup.aligngo2est_angleramp_inc_radpdt_q16 < 10)
                    motor_ptr->startup.aligngo2est_angleramp_inc_radpdt_q16 = 10;
                #endif
                motor_ptr->status.motor &= ~(STATUS_MOTOR_OL_RAMPUP | STATUS_MOTOR_CL_RAMPDOWN | STATUS_MOTOR_OPEN_LOOP);
                motor_ptr->status.motor |= STATUS_MOTOR_CLOSED_LOOP;
                motor_ptr->mpos.state = STATE_MPOS_TRANS_ALIGNGO2EST;
                motor_ptr->control.idref_target_f16 = motor_ptr->app.idrefcl_cmd_f16;

                motor_ptr->startup.state = STATE_STARTUP_ALIGNGO_ALIGNGO2EST;
                __enable_irq();
            }
            else
            {
#ifdef ENABLE_ANGLEDIFF_AUTORANGE
                anglediff_autorange(motor_ptr);
#endif
#ifdef ENABLE_ANGLEDIFF_TIMEOUT
                anglediff_timeout_check(motor_ptr);
#endif
            }
            break;

        case STATE_STARTUP_ALIGNGO_ALIGNGO2EST:
            // Check if transitions are complete
            lbg1_check_transition_ready(&motor_ptr->mpos.est);

            // Next state if transitions are complete
#if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_1)
            if (motor_ptr->mpos.est.est_transition_complete)
#elif (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
            if ((motor_ptr->mpos.est.est_transition_complete) && (motor_ptr->startup.aligngo2est_angleramp_q16 == 0))
#endif
            {
                #ifdef ENABLE_PLL_ANGLE_CALCULATION
                if (motor_ptr->mpos.est_pll.est_pll_transition_complete)
                #endif
                {
                    __disable_irq();
                    if (motor_ptr->app.speed_mode_flag)
                    {
                        #if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
                            // Update outerloop ramp value with calculated power
                            motor_ptr->control.outerloop_ramp = motor_ptr->power.power_result;
                        #elif (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
                            // Update outerloop ramp value with switchover speed
                            motor_ptr->control.outerloop_ramp = motor_ptr->mpos.mpos_spd_radps;
                        #endif
                        motor_ptr->state.control = STATE_CONTROL_OUTERLOOP;
                    }
                    else
                    {
                        motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16 * motor_ptr->app.app_motor_direction;
                        motor_ptr->state.control = STATE_CONTROL_TORQUE;
                    }
                    // Restore parameters
                    motor_ptr->control.idiqref_rate_f16 = motor_ptr->control.idiqref_rate_gui_f16;
                    motor_ptr->control.outerloop_rate = motor_ptr->control.outerloop_rate_store;
                    motor_ptr->mpos.state = STATE_MPOS_ESTIMATED_ANGLE;
                    motor_ptr->startup.state = STATE_STARTUP_ALIGNGO_COMPLETE;
                    __enable_irq();
                }
            }
            break;

        case STATE_STARTUP_ALIGNGO_COMPLETE:
            break;

        default:
            break;
    }

    // Update outer loop ramp and feedback (for stall detection and display)
    switch(motor_ptr->startup.state)
    {
    case STATE_STARTUP_ALIGNGO_RUN:
        aligngo_update_outerloop_ramp_ol(motor_ptr, motor_ptr->mpos.aligngo.speedrefol_ramp_q26);
        break;

    case STATE_STARTUP_ALIGNGO_ALIGNGO2EST:
        // JAGTAG: Clean this up across startups and SIM
//        update_outerloop_ramp_sw(motor_ptr);      //JAGTAG: Commented out so that speed ramp can start from OL speed not estimated speeed
        break;

    default:
        break;
    }

    update_outerloop_fdbk_scaled(motor_ptr);
}


static inline void startup_aligngo_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    Struct_AlignGoCMD* aligngo_ptr = &motor_ptr->startup.aligngo;

    switch(param_code)
    {
    case open_loop_align_hold_num_steps:
        aligngo_param_write(motor_ptr, param_aligngo_align_time, rx_data, aligngo_ptr);
        break;
    case open_loop_speed_slew_rate:
        aligngo_param_write(motor_ptr, param_aligngo_speed_ramprate, rx_data, aligngo_ptr);
        break;
    case open_loop_speed_setpoint:
        aligngo_param_write(motor_ptr, param_aligngo_speed_target, rx_data, aligngo_ptr);
        break;
    case id_ref:
        aligngo_param_write(motor_ptr, param_aligngo_id_ref, rx_data, aligngo_ptr);
        break;
    case iq_ref:
        aligngo_param_write(motor_ptr, param_aligngo_itot_ref, rx_data, aligngo_ptr);
        break;
    default:
        break;
    }
}


static inline void startup_aligngo_param_process(Struct_Motor* motor_ptr, ParameterCode param_code)
{
    if (motor_ptr->state.main == STATE_MAIN_STARTUP)
    {
        switch(param_code)
        {
        case open_loop_align_hold_num_steps:
        case open_loop_speed_slew_rate:
        case open_loop_speed_setpoint:
        case id_ref:
        case iq_ref:
            aligngo_setvars(motor_ptr, &motor_ptr->startup.aligngo);

            if (motor_ptr->state.main == STATE_MAIN_STARTUP)
            {
                // Set control variables
                motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
                motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
                motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;
            }
            break;
        default:
            break;
        }
    }
}


#endif  //#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
#endif  //#ifndef STARTUP_ALIGNGO2EST_FUNCS_H
