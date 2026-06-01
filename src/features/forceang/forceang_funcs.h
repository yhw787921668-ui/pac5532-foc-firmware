
/**
 * @file forceang_funcs.h
 * @brief Forced-angle mode Functions
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

#ifndef FORCEANG_FUNCS_H
#define FORCEANG_FUNCS_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "aligngo_funcs.h"
#include "control_funcs.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mpos_config.h"


#if defined(ENABLE_FORCEANG)

//////////////////////////////////////////
// Constant Declarations
//////////////////////////////////////////
#define FORCEANG_SPD_HZPS_Q16               QCONST(1.0f, Q16)


//////////////////////////////////////////
// Function Declarations
//////////////////////////////////////////


//////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////

static inline void forceang_state_enter(Struct_Motor *motor_ptr)
{
    motor_ptr->forceang.forceang_enabled_flag = 1;

    //debugio1_off();
    __disable_irq();
    motor_ptr->forceang.forceang_state = STATE_FORCEANG_RESET;
    motor_ptr->state.main = STATE_MAIN_FORCEANG;
    motor_ptr->status.motor |= STATUS_MOTOR_ENABLED;
    motor_ptr->status.motor &= ~STATUS_MOTOR_STOPPED;
    __enable_irq();
}


static inline void forceang_state_exit(Struct_Motor *motor_ptr)
{
    if (!motor_ptr->app.motor_enable_cmd)
    {
        motor_ptr->forceang.forceang_enabled_flag = 0;
        // Next state
        motor_ptr->state.main = STATE_MAIN_DISABLE;
    }
}


static inline void forceang_reset_vars(Struct_Motor* motor_ptr)
{
    // Set aligngo fixed command values
    motor_ptr->forceang.aligngo.aligntime_cmd = 0;
    motor_ptr->forceang.aligngo.speedrefol_ramprate_cmd_q26 = 0;
    motor_ptr->forceang.aligngo.idrefol_cmd_q16 = motor_ptr->forceang.aligngo.itotrefol_cmd_q16;

    // Set aligngo variables
    aligngo_setvars(motor_ptr, &motor_ptr->forceang.aligngo);

    // Set control variables
    motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
    motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
    motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;
}


static inline void forceang_controller(Struct_Motor* motor_ptr)
{
    // Direction command handler
    motor_ptr->app.app_motor_direction = motor_ptr->app.app_motor_direction_cmd;
    if (motor_ptr->app.app_motor_direction == -1)
        motor_ptr->status.motor |= STATUS_MOTOR_REVERSE;
    else
        motor_ptr->status.motor &= ~STATUS_MOTOR_REVERSE;

    switch(motor_ptr->forceang.forceang_state)
    {
    case STATE_FORCEANG_RESET:
        // Reset running variables and sub-states
        control_reset(motor_ptr);
        foc_reset(motor_ptr);
        forceang_reset_vars(motor_ptr);
#ifdef ENABLE_OPDOL
        reset_opdol(motor_ptr);
#endif
        // Configure control loop for FOC mode
        foc_config(motor_ptr, 1);
        mpos_clear_fptr(motor_ptr);

        // Set current ramp rate
        motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
        // Next State
        motor_ptr->forceang.forceang_state = STATE_FORCEANG_ALIGNGORESET;
        break;

    case STATE_FORCEANG_ALIGNGORESET:
        // Reset running variables
        aligngo_reset_vars(motor_ptr);
        // Next control state
        __disable_irq();
        motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
        motor_ptr->state.control = STATE_CONTROL_TORQUE;
        motor_ptr->mpos.state = STATE_MPOS_ALIGNGO;
        motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_ANGLERAMP;
        m1_enable();
        __enable_irq();
        motor_ptr->status.motor |= (STATUS_MOTOR_ENABLED | STATUS_MOTOR_ALIGN);
        motor_ptr->mpos.aligngo.status &= ~STATUS_ALIGNGO_COMPLETE;
        motor_ptr->mpos.aligngo.status |= STATUS_ALIGNGO_RUNNING;
        // Next State
        motor_ptr->forceang.forceang_state = STATE_FORCEANG_RUN;

    case STATE_FORCEANG_RUN:
        break;
    }
}


static inline void forceang_aligngo_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    Struct_AlignGoCMD* aligngo_ptr = &motor_ptr->forceang.aligngo;
    int32_t temp;

    switch(param_code)
    {
    case Forced_angle_mode:
        motor_ptr->forceang.forceang_enable_cmd = rx_data;
        break;

    case Forced_angle:
        aligngo_param_write(motor_ptr, param_aligngo_align_angle, rx_data, aligngo_ptr);
        break;

    case Forced_angle_current:
        temp = fix16_div(rx_data, motor_ptr->app.descale_current_factor);
        aligngo_param_write(motor_ptr, param_aligngo_itot_ref, temp, aligngo_ptr);
        break;

    case Forced_angle_speed:
        temp = fix16_mul(fix16_mul(rx_data, HZ2RAD_FIX16), motor_ptr->foc.dt_control_q26);
        aligngo_param_write(motor_ptr, param_aligngo_speed_target, temp, aligngo_ptr);
        break;
    
    default:
        break;
    }
}


static inline void forceang_aligngo_param_process(Struct_Motor* motor_ptr, ParameterCode param_code)
{
    if (motor_ptr->state.main == STATE_MAIN_FORCEANG)
    {
        switch(param_code)
        {
        case reverse_direction:
        case Forced_angle:
        case Forced_angle_current:
        case Forced_angle_speed:
            motor_ptr->forceang.aligngo.idrefol_cmd_q16 = motor_ptr->forceang.aligngo.itotrefol_cmd_q16;
            aligngo_setvars(motor_ptr, &motor_ptr->forceang.aligngo);

            // Set control variables
            motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
            motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
            motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;
            break;
        default:
            break;
        }
    }
}

#endif  //#if defined(ENABLE_FORCEANG)

#endif  //#ifndef FORCEANG_FUNCS_H
