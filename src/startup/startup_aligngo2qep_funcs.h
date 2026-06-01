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

#ifndef STARTUP_ALIGNGO2QEP_FUNCS_H
#define STARTUP_ALIGNGO2QEP_FUNCS_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "aligngo_funcs.h"
#include "param_funcs.h"
#include "control_funcs.h"
#include "qep_funcs.h"
#include "fault.h"
#include "utils.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mpos_config.h"

#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
#include "lbg1_funcs.h"
#endif


#if (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP)

static inline int32_t startup_complete(Struct_Motor* motor_ptr)
{
    return (motor_ptr->startup.state == STATE_STARTUP_ALIGNGO2QEP_COMPLETE);
}


static inline void startup_reset_state(Struct_Motor* motor_ptr)
{
    motor_ptr->startup.state = STATE_STARTUP_ALIGNGO2QEP_RESET;
}


static inline void qep_check_ready(Struct_Motor* motor_ptr, int32_t ref_speed_radps, int32_t check_speed_radps)
{
    if (motor_ptr->mpos.qep.index_counter >= 1)
    {
        motor_ptr->startup.qep_ready = TRUE;
    }
}


#ifdef ENABLE_QEP_STARTUP_TIMEOUT
static inline void startup_qep_timeout_check(Struct_Motor *motor_ptr)
{
    if (motor_ptr->app.auto_close_loop_cmd)
    {
        if (motor_ptr->mpos.qep.elec_angle_wrap_around_counts > motor_ptr->mpos.qep.qep_elec_angle_counts_threshold)
        {
            set_fault(FAULT_SPDANGLDIF_TIMEOUT);            // Set motor stall status
            motor_ptr->mpos.qep.elec_angle_wrap_around_counts = 0;
        }
    }
    else
        motor_ptr->mpos.qep.elec_angle_wrap_around_counts = 0;
}
#endif

static inline void startup_reset_vars(Struct_Motor* motor_ptr)
{
    // Set aligngo fixed command values
    motor_ptr->startup.aligngo.alignangle_cmd_q26 = (FIX16_PI_DIV_2 << 10);

    // Set aligngo variables
    aligngo_setvars(motor_ptr, &motor_ptr->startup.aligngo);

    // Set control variables
    motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
    motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
    motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;
}


static inline void startup_controller(Struct_Motor* motor_ptr)
{
    switch(motor_ptr->startup.state)
    {
    case STATE_STARTUP_ALIGNGO2QEP_RESET:
        // Reset running variables and sub-states
        control_reset(motor_ptr);

#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
        lbg1_reset(motor_ptr);
        lbg1_switch2cl(&motor_ptr->mpos.est);
        motor_ptr->mpos.mposmode = STATE_MPOSMODE_QEP;
#endif
#ifdef ENABLE_OPDOL
        reset_opdol(motor_ptr);
#endif
        // Configure control loop for FOC mode
        foc_config(motor_ptr, 1);
        mpos_config_fptr(motor_ptr);

        __disable_irq();
        if (!motor_ptr->startup.qep_ready)
        {
            // Start with Align&Go
            qep_reset(motor_ptr);
            startup_reset_vars(motor_ptr);
            aligngo_reset_state(motor_ptr);
            // Next state
            motor_ptr->startup.state = STATE_STARTUP_ALIGNGO2QEP_ALIGNGO;
        }
        else
        {
            // Start directly with QEP
            motor_ptr->status.motor |= (STATUS_MOTOR_CLOSED_LOOP | STATUS_MOTOR_MPOSMODE_QEP);
            motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
            motor_ptr->mpos.state = STATE_MPOS_QEP;
            if (motor_ptr->app.speed_mode_flag)
                motor_ptr->state.control = STATE_CONTROL_OUTERLOOP;
            else
            {
                motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16;
                motor_ptr->state.control = STATE_CONTROL_TORQUE;
            }
            // Next state
            motor_ptr->startup.state = STATE_STARTUP_ALIGNGO2QEP_COMPLETE;
        }
        m1_enable();
        __enable_irq();
        break;

    case STATE_STARTUP_ALIGNGO2QEP_ALIGNGO:
        // Run Align&Go controller
        aligngo_controller(motor_ptr);

        // Timeout check
        startup_qep_timeout_check(motor_ptr);

        // Check if QEP is ready for switchover
        qep_check_ready(motor_ptr, motor_ptr->mpos.aligngo.speedrefol_target_radps_q16, motor_ptr->mpos.qep.qep_spd_radps);

        // Next state if QEP is ready to transition
        if ((motor_ptr->startup.qep_ready) && (motor_ptr->app.auto_close_loop_cmd))
        {
            __disable_irq();
            motor_ptr->status.motor &= ~(STATUS_MOTOR_OL_RAMPUP | STATUS_MOTOR_CL_RAMPDOWN | STATUS_MOTOR_OPEN_LOOP);
            motor_ptr->status.motor |= (STATUS_MOTOR_CLOSED_LOOP | STATUS_MOTOR_MPOSMODE_QEP);
            motor_ptr->mpos.state = STATE_MPOS_QEP;
            if (motor_ptr->app.speed_mode_flag)
                motor_ptr->state.control = STATE_CONTROL_OUTERLOOP;
            else
            {
                motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16;
                motor_ptr->state.control = STATE_CONTROL_TORQUE;
            }
            motor_ptr->startup.state = STATE_STARTUP_ALIGNGO2QEP_TRANSITION;
            __enable_irq();
        }
        break;

    case STATE_STARTUP_ALIGNGO2QEP_TRANSITION:
        // Restore current ramp rate
        motor_ptr->control.idiqref_rate_f16 = motor_ptr->control.idiqref_rate_gui_f16;
        motor_ptr->control.idref_target_f16 = motor_ptr->app.idrefcl_cmd_f16;

        // Next state
        motor_ptr->startup.state = STATE_STARTUP_ALIGNGO2QEP_COMPLETE;
        break;

    case STATE_STARTUP_ALIGNGO2QEP_COMPLETE:
        break;

    default:
        break;
    }

    // Update outer loop ramp and feedback (for stall detection and display)
    switch(motor_ptr->startup.state)
    {
    case STATE_STARTUP_ALIGNGO2QEP_ALIGNGO:
        aligngo_update_outerloop_ramp_ol(motor_ptr, motor_ptr->mpos.aligngo.speedrefol_ramp_q26);
        break;

    case STATE_STARTUP_ALIGNGO2QEP_TRANSITION:
        update_outerloop_ramp_sw(motor_ptr);
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

#endif  //#if (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP)
#endif  //#ifndef STARTUP_ALIGNGO2QEP_FUNCS_H
