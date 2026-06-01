/**
 * @file fw_funcs.h
 * @brief Use for field weakening utility functions
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

#ifndef FW_FUNCS_H
#define FW_FUNCS_H

#include "motor_struct.h"
#include "pid.h"
#include "utils.h"
#include "param_funcs.h"
#if defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == PID_CONTROL)
#include "sqrt.h"
#endif

/**
 * @brief  Use to write FIELD WEAKENING parameter data into related variables
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
extern void fw_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);

/**
 * @brief  Use as FIELD WEAKENING controller flow
 * @param Struct_Motor* motor_ptr
 */
extern void fw_controller(Struct_Motor *motor_ptr);

/**
 * @brief  Use to reset FIELD WEAKENING variables and status
 * @param Struct_Motor* motor_ptr
 */
extern void fw_reset(Struct_Motor* motor_ptr);

#if defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == INCREMENTRAL_CONTROL)
/**
 * @brief  Use to process FIELD WEAKENING id current ramp and outerloop pid max value
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_RAMFUNC static inline void fw_process(Struct_Motor* motor_ptr)
{
    int32_t idrefcl_target_f16;


    // Use semaphore to update total current target and FW PI min value
    // If semaphore is red, use stored values
    if (motor_ptr->fw.flag_semaphore)
    {
        idrefcl_target_f16 = motor_ptr->fw.idrefcl_target_store_f16;
        motor_ptr->control.outerloop_pid.max_value = motor_ptr->fw.iqmax_store_q14;
    }
    // If semaphore is green, use last calculated values
    else
    {
        idrefcl_target_f16 = motor_ptr->control.idref_target_f16;
        motor_ptr->control.outerloop_pid.max_value = motor_ptr->fw.iqmax_q14;
    }

    // Ramp Id to Id Target
    GENERIC_RAMP(motor_ptr->foc.idref_ramp_f16, idrefcl_target_f16, motor_ptr->control.idiqref_rate_f16);
    motor_ptr->foc.idref_ramp_q14 = (motor_ptr->foc.idref_ramp_f16 >> 2);
}

#elif defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == PID_CONTROL)
// PID controller for the field weakening control
PAC5XXX_RAMFUNC static inline void fw_process(Struct_Motor* motor_ptr)
{
    int32_t iqmax_limit_q16;
    motor_ptr->fw.vq_vd_squares_sum_q14 = ((motor_ptr->foc.vd_q14 * motor_ptr->foc.vd_q14) >> 14) + ((motor_ptr->foc.vq_q14 * motor_ptr->foc.vq_q14) >> 14);
    motor_ptr->fw.vq_vd_squares_root_q14 = sqrt_q14(motor_ptr->fw.vq_vd_squares_sum_q14);
    // For debug purpose only - Filter the feedback voltage
    motor_ptr->fw.vq_vd_squares_root_filt_q14 = filter_alpha_q14(motor_ptr->fw.vq_vd_squares_root_q14, motor_ptr->fw.vq_vd_squares_root_filt_q14, motor_ptr->fw.vrms_filter_coeff_q16);

    //pid_run(&(motor_ptr->fw.fw_pid), (VMAG_REF_FW_Q14 - motor_ptr->fw.vq_vd_squares_root_q14));
    pid_run(&(motor_ptr->fw.fw_pid), (motor_ptr->fw.vmax_scl_q14 - motor_ptr->fw.vq_vd_squares_root_q14));

    // Update control loop variables
    motor_ptr->foc.idref_ramp_q14 = motor_ptr->fw.fw_pid.PI_sat;                    // q14

    ///////////////////////////
    // Iq_max controller
    ///////////////////////////
    // Run circle limiter to ensure total current limit is not exceeded
    circle_limiter(motor_ptr->control.imax_q16, motor_ptr->control.imax_1div_q16, -(motor_ptr->foc.idref_ramp_q14 << 2), &iqmax_limit_q16);
    #if (OUTERLOOP_CONTROL_SELECT != OUTERLOOP_CONTROL_POWER)
    iqmax_limit_q16 = iqmax_limit_q16 * motor_ptr->app.app_motor_direction;
    #endif
    motor_ptr->control.outerloop_pid.max_value = iqmax_limit_q16 >> 2;              // q14
}
#endif  //#if (FIELD_WEAKENING_METHOD)
#endif  //#ifndef FW_FUNCS_H
