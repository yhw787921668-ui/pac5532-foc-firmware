
/**
* @file halltune_funcs.h
* @brief Functions used for tuning and calibrating hall sensor angle
*/

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef HALLTUNE_FUNCS_H
#define HALLTUNE_FUNCS_H

#include "pac5xxx.h"
#include "config_features.h"
#include "motor_struct.h"
#include "halltune_struct.h"
#include "aligngo_funcs.h"
#include "foc.h"
#include "control_funcs.h"
#include "param_funcs.h"
#include "driver_funcs.h"
#include "hall_funcs.h"
#include "trig.h"
#include "foc_funcs.h"


#if defined(ENABLE_HALL_TUNING)
#define TRANSITION_COUNTER_LIMIT    300
#define FILTER_DELTA_THRESHOLD      QCONST((2.0f * FLT_2PI / 360.0f), Q16)
#define HALLTUNE_ALIGNTIME_SEC_Q16  QCONST(1.0f, Q16)

// Define angle snapping range
#define ANGLIMIT_HI_RAD_Q16         QCONST((75.0f * FLT_2PI / 360.0f), Q16)
#define ANGLIMIT_LO_RAD_Q16         QCONST((15.0f * FLT_2PI / 360.0f), Q16)
#define ANGLIMIT_HI2_RAD_Q16        QCONST((20.0f * FLT_2PI / 360.0f), Q16)
#define ANGLIMIT_LO2_RAD_Q16        QCONST((20.0f * FLT_2PI / 360.0f), Q16)


///////////////////////////////////////
// Function Declarations
///////////////////////////////////////

/**
* @brief High level hall tuning state machine
* @param Struct_Motor* motor_ptr
*/
void halltune_controller(Struct_Motor* motor_ptr);


///////////////////////////////////////
// Command Handlers
///////////////////////////////////////

/**
* @brief Handles the user command to enable the motor in hall tuning
* @param Struct_Motor* motor_ptr
*/
static inline void halltune_process_cmd_enable(Struct_Motor *motor_ptr)
{
    // Wait for the halltune enable command and then transition to the HALLTUNE state
    if (motor_ptr->halltune.halltune_enable_flag)
    {
        // Update status
        motor_ptr->status.motor |= STATUS_MOTOR_ENABLED;
        motor_ptr->status.motor &= ~STATUS_MOTOR_STOPPED;
        // Next state
        __disable_irq();
        motor_ptr->state.main = STATE_MAIN_HALLTUNE;
        motor_ptr->halltune.state = STATE_HALLTUNE_RESET;
        __enable_irq();
    }
}


/**
* @brief Handles the uswer command to disable the motor in hall tuning
* @param Struct_Motor* motor_ptr
*/
static inline void halltune_process_cmd_disable(Struct_Motor *motor_ptr)
{
    // Wait for the halltune enable command and then transition to the HALLTUNE state
    if (!motor_ptr->halltune.halltune_enable_flag)
    {
        // Restore current ramp rate
        motor_ptr->control.idiqref_rate_f16 = motor_ptr->control.idiqref_rate_gui_f16;

        // Update status
        motor_ptr->status.motor &= ~STATUS_MOTOR_ENABLED;
        motor_ptr->status.motor |= STATUS_MOTOR_STOPPED;
        motor_ptr->halltune.status = 0;
        // Next state
        __disable_irq();
        motor_ptr->state.main = STATE_MAIN_DISABLE;
        motor_ptr->halltune.state = STATE_HALLTUNE_IDLE;
        __enable_irq();
    }
}


/**
* @brief Initialize hall tuning variables at powerup
* @param Struct_Motor* motor_ptr
*/
static inline void halltune_init(Struct_Motor* motor_ptr)
{
    motor_ptr->halltune.status = STATUS_HALLTUNE_READY;
}


/**
* @brief Reset hall tuning variables when starting
* @param Struct_Motor* motor_ptr
*/
static inline void halltune_reset_vars(Struct_Motor *motor_ptr)
{
    int index;

    motor_ptr->halltune.transition_counter = 0;
    motor_ptr->halltune.filter_delta_outofrange = 1;
    motor_ptr->halltune.flag_halltune_filter_settled = 0;

    // Reset arrays
    for (index = 0; index < 8; ++index)
    {
        motor_ptr->halltune.hallstate_angle_tuning_filtered_q20[index] = 0;
        motor_ptr->halltune.hallstate_angle_ref[index] = 0;
    }

    // Reset status
    motor_ptr->halltune.status = STATUS_HALLTUNE_READY;
}


/**
* @brief Processes hall sensor edges to determine relationship to angle during hall tuning
* @param Struct_Motor* motor_ptr
*/
PAC5XXX_RAMFUNC void hall_tuning_process(Struct_Motor *motor_ptr);


PAC5XXX_OPTIMIZE_SPEED_ON
/**
* @brief Control loop hall tuning state machine
* @param Struct_Motor* motor_ptr
*/
PAC5XXX_RAMFUNC static inline void hall_tuning_statemachine_control(Struct_Motor* motor_ptr)
{
    // Set iqref = 0
    motor_ptr->foc.iqref_ramp_q14 = 0;

    // Ramp Id to Target
    GENERIC_RAMP(motor_ptr->foc.idref_ramp_f16, motor_ptr->mpos.aligngo.itotrefol_target_q16, motor_ptr->control.idiqref_rate_f16);
    motor_ptr->foc.idref_ramp_q14 = (motor_ptr->foc.idref_ramp_f16 >> 2);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
* @brief Snaps the collected angle information to the pre-defined angle table
* @param Struct_Motor* motor_ptr
*/
static inline void hall_tuning_calcangletable(Struct_Motor* motor_ptr)
{
    int32_t hallstate, temp_angle, index_angle;

    for (hallstate = 1; hallstate < 7; ++hallstate)
    {
        if(motor_ptr->app.app_motor_direction_cmd == 1)
        {
            // Use state 1 as the electrical rotation index
            index_angle = motor_ptr->halltune.hallstate_angle_tuning[1];

            temp_angle = motor_ptr->halltune.hallstate_angle_tuning[hallstate] - index_angle;
            wraparound(&temp_angle);
            motor_ptr->halltune.hallstate_angle_ref[hallstate] = temp_angle;
        }
    }
}


/**
* @brief Checks if the hall tuning angle filter has settled
* @param Struct_Motor* motor_ptr
*/
static inline void halltune_check_filter_settled(Struct_Motor* motor_ptr)
{
    // Check if detection interval is complete
    if (motor_ptr->halltune.transition_counter >= TRANSITION_COUNTER_LIMIT)
    {
        // Check if filter delta was out of range at any point during detection interval and report status
        if (motor_ptr->halltune.filter_delta_outofrange)
        {
            motor_ptr->halltune.filter_delta_outofrange = 0;
            motor_ptr->halltune.flag_halltune_filter_settled = 0;
            motor_ptr->halltune.status |= FAULT_HALLTUNE_DETECTION_FAILURE;
        }
        else
        {
            motor_ptr->halltune.flag_halltune_filter_settled = 1;
            motor_ptr->halltune.status &= ~FAULT_HALLTUNE_DETECTION_FAILURE;
        }

        // Clear transition counter to restart detection interval
        motor_ptr->halltune.transition_counter = 0;
    }
}


/**
* @brief Resets the Align&Go state machine used during hall tuning
* @param Struct_Motor* motor_ptr
*/
static inline void halltune_aligngo_reset(Struct_Motor* motor_ptr)
{
    // Set aligngo fixed command values
    motor_ptr->halltune.aligngo.aligntime_cmd = ((fix16_mul(HALLTUNE_ALIGNTIME_SEC_Q16, motor_ptr->control.dt_control_1div_q6)) >> 6);  // q0 = mul(q16,q6)=q6>>6
    motor_ptr->halltune.aligngo.idrefol_cmd_q16 = 0;
    motor_ptr->halltune.aligngo.alignangle_cmd_q26 = (FIX16_PI_DIV_2 << 10);

    // Set aligngo variables
    aligngo_setvars(motor_ptr, &motor_ptr->halltune.aligngo);

    // Set control variables
    motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
    motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
    motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;

}


static inline void halltune_store_transition_angle(Struct_Motor* motor_ptr)
{
    // Store last angle before hall transition
    motor_ptr->halltune.hall_transition_prevangle = motor_ptr->mpos.hall.hall_interpolated_angle;
}


static inline void halltune_edge_pol_skew_tuning(Struct_Motor* motor_ptr)
{
    static int32_t hall_transition_delta_angle_filt_comp, hall_transition_delta_angle_filt_nocomp;


    // Hall edge polarity skew compensation tuning
    if (motor_ptr->halltune.halltune_mode == HALLTUNE_MODE_EDGEPOLSKEW)
    {
        if (motor_ptr->mpos.hall.flag_halledge_comp)
            filter_shift(motor_ptr->mpos.hall.sector_time_sec_q26, &hall_transition_delta_angle_filt_comp, 8);
        else
            filter_shift(motor_ptr->mpos.hall.sector_time_sec_q26, &hall_transition_delta_angle_filt_nocomp, 8);

        motor_ptr->halltune.halltune_output_3 = fix16_mul((hall_transition_delta_angle_filt_comp >> 8), QCONST((1000.0f/1024.0f),Q16));
        motor_ptr->halltune.halltune_output_4 = fix16_mul((hall_transition_delta_angle_filt_nocomp >> 8), QCONST((1000.0f/1024.0f),Q16));
        motor_ptr->halltune.halltune_output_1 = motor_ptr->halltune.halltune_output_3 - motor_ptr->halltune.halltune_output_4;
    }
}


static inline void halltune_edgetuning(Struct_Motor* motor_ptr)
{
    int32_t hall_transition_delta_angle;
    static int32_t hall_transition_delta_angle_filt_s1, hall_transition_delta_angle_filt_s2, hall_transition_delta_angle_filt_s3;
    static int32_t hall_transition_delta_angle_filt_s4, hall_transition_delta_angle_filt_s5, hall_transition_delta_angle_filt_s6;
    static int32_t hall_transition_delta_angle_filt_sall;


    if (motor_ptr->halltune.halltune_mode == HALLTUNE_MODE_INDIVIDUALEDGE)
    {
        hall_transition_delta_angle = motor_ptr->mpos.hall.hall_interpolated_angle - motor_ptr->halltune.hall_transition_prevangle;
        if (hall_transition_delta_angle < -FIX16_PI)
            hall_transition_delta_angle = hall_transition_delta_angle + FIX16_2PI;
//        debug_dac_1a = hall_transition_delta_angle >> 5;

        // Hall individual edge correction tuning
        filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_sall, 5);
//        debug_var_5 = hall_transition_delta_angle_filt_sall >> 5;

        switch(motor_ptr->mpos.hall.SensorStateNew)
        {
        case 1:
            filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_s1, 5);
            motor_ptr->halltune.halltune_output_1 = (hall_transition_delta_angle_filt_s1 - hall_transition_delta_angle_filt_sall) >> 5;
            break;
        case 2:
            filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_s2, 5);
            motor_ptr->halltune.halltune_output_2 = (hall_transition_delta_angle_filt_s2 - hall_transition_delta_angle_filt_sall)>> 5;
            break;
        case 3:
            filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_s3, 5);
            motor_ptr->halltune.halltune_output_3 = (hall_transition_delta_angle_filt_s3 - hall_transition_delta_angle_filt_sall)>> 5;
            break;
        case 4:
            filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_s4, 5);
            motor_ptr->halltune.halltune_output_4 = (hall_transition_delta_angle_filt_s4 - hall_transition_delta_angle_filt_sall)>> 5;
            break;
        case 5:
            filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_s5, 5);
            motor_ptr->halltune.halltune_output_5 = (hall_transition_delta_angle_filt_s5 - hall_transition_delta_angle_filt_sall)>> 5;
            break;
        case 6:
            filter_shift(hall_transition_delta_angle, &hall_transition_delta_angle_filt_s6, 5);
            motor_ptr->halltune.halltune_output_6 = (hall_transition_delta_angle_filt_s6 - hall_transition_delta_angle_filt_sall)>> 5;
            break;
        default:
            break;
        }
    }
}


/**
* @brief Writes and calculates hall tuning parameters based on user defined values
* @param Struct_Motor* motor_ptr
* @param ParameterCode param_code
* @param int32_t rx_data
*/
static inline void halltune_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    Struct_AlignGoCMD* aligngo_ptr = &motor_ptr->halltune.aligngo;

    switch(param_code)
    {
    case HallTuning_Status:
        // Read Only
        break;
    case HallTuning_Command:
        motor_ptr->halltune.gui_cmd = rx_data;
        switch(motor_ptr->halltune.gui_cmd)
        {
        case 0:
            motor_ptr->halltune.halltune_enable_flag = 1;
            break;
        case 1:
            motor_ptr->halltune.halltune_enable_flag = 0;
            break;
        default:
            break;
        }
        break;
    case HallTuning_Istart:
        aligngo_param_write(motor_ptr, param_aligngo_itot_ref, rx_data, aligngo_ptr);
        break;
    case HallTuning_Speed_Ramp:
        aligngo_param_write(motor_ptr, param_aligngo_speed_ramprate, rx_data, aligngo_ptr);
        break;
    case HallTuning_Target_Speed:
        aligngo_param_write(motor_ptr, param_aligngo_speed_target, rx_data, aligngo_ptr);
        break;
    case HallTuning_HallStateAngleTuning_1:
    case HallTuning_HallStateAngleRef_1:
    case HallTuning_HallStateAngleTuning_2:
    case HallTuning_HallStateAngleRef_2:
    case HallTuning_HallStateAngleTuning_3:
    case HallTuning_HallStateAngleRef_3:
    case HallTuning_HallStateAngleTuning_4:
    case HallTuning_HallStateAngleRef_4:
    case HallTuning_HallStateAngleTuning_5:
    case HallTuning_HallStateAngleRef_5:
    case HallTuning_HallStateAngleTuning_6:
    case HallTuning_HallStateAngleRef_6:
        // Read Only
        break;
    default:
        break;
    }
}


static inline void halltune_aligngo_param_process(Struct_Motor* motor_ptr, ParameterCode param_code)
{
    if (motor_ptr->state.main == STATE_MAIN_HALLTUNE)
    {
        switch(param_code)
        {
        case HallTuning_Istart:
        case HallTuning_Speed_Ramp:
        case HallTuning_Target_Speed:
            aligngo_setvars(motor_ptr, &motor_ptr->halltune.aligngo);

            if (motor_ptr->state.main == STATE_MAIN_HALLTUNE)
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
#endif  //#if defined(ENABLE_HALL_TUNING)

#endif  //#ifndef HALLTUNE_FUNCS_H
