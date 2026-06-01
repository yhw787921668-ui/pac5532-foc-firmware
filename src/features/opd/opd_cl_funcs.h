/**
 * @file opd_cl_funcs.h
 * @brief Use for defining control functions in OPD
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

#ifndef OPD_CL_FUNCS_H
#define OPD_CL_FUNCS_H

#include "opd_cl.h"
#include "motor_struct.h"
#include "param_funcs.h"
//#include "debug.h"
#ifdef ENABLE_OPD_CL

/**
 * @brief Use to check if OPD fault happens
 * @param Struct_Motor *motor_ptr
 */
extern void check_opdcl_fault(Struct_Motor* motor_ptr);

/**
 * @brief Use to calculate OPD sample time
 * @param Struct_Motor *motor_ptr
 * @param int32_t speedref_min
 * @param int32_t systick_dt
 */
extern void calc_opdcl_sample_time(Struct_Motor* motor_ptr, int32_t speedref_min, int32_t systick_dt);

/**
 * @brief Initialize OPD function
 * @param Struct_Motor *motor_ptr
 */
extern void opdcl_init(Struct_Motor* motor_ptr);

/**
 * @brief Reset OPD function
 * @param Struct_Motor *motor_ptr
 */
extern void opdcl_reset(Struct_Motor* motor_ptr);

/**
 * @brief Use to process parameters in OPD function
 * @param Struct_Motor *motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
extern void opdcl_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);

/**
 * @brief Use for sampling and recording phase current
 * @param Struct_Motor *motor_ptr
 */
PAC5XXX_RAMFUNC static inline void integrate_and_log_phase_current(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_OPD_CL_FAST)
    //debugio1_toggle();
    motor_ptr->opdcl.Iu_sq_q24 = ((motor_ptr->control.Iu_measured_q14 >> 2)* (motor_ptr->control.Iu_measured_q14 >> 2)); // q12*q12 = q24
    motor_ptr->opdcl.Iv_sq_q24 = ((motor_ptr->control.Iv_measured_q14 >> 2)* (motor_ptr->control.Iv_measured_q14 >> 2)); // q12*q12 = q24
    motor_ptr->opdcl.Iw_sq_q24 = ((motor_ptr->control.Iw_measured_q14 >> 2)* (motor_ptr->control.Iw_measured_q14 >> 2)); // q12*q12 = q24
    // filtering the sampled currents
    motor_ptr->opdcl.Iu_sq_f_q24 += (motor_ptr->opdcl.filter_coef_q10 * ((motor_ptr->opdcl.Iu_sq_q24 - motor_ptr->opdcl.Iu_sq_f_q24) >> 4)) >> 6; // q10*q20= q30->6=q24
    motor_ptr->opdcl.Iv_sq_f_q24 += (motor_ptr->opdcl.filter_coef_q10 * ((motor_ptr->opdcl.Iv_sq_q24 - motor_ptr->opdcl.Iv_sq_f_q24) >> 4)) >> 6;
    motor_ptr->opdcl.Iw_sq_f_q24 += (motor_ptr->opdcl.filter_coef_q10 * ((motor_ptr->opdcl.Iw_sq_q24 - motor_ptr->opdcl.Iw_sq_f_q24) >> 4)) >> 6;
#if 0
    //debugio1_toggle();
//    debug_dac_1a = motor_ptr->opdcl.Iu_sq_q24 >> 8;
//    debug_dac_2a = motor_ptr->opdcl.Iu_sq_f_q24 >> 8;
#endif
#else
    fix16_t temp_motor;
    //debugio1_toggle();
    temp_motor = motor_ptr->control.Iu_measured_q14;
    if (temp_motor < 0)
        motor_ptr->opdcl.Sum_Iu_m += temp_motor;
    else
        motor_ptr->opdcl.Sum_Iu_p += temp_motor;

    temp_motor = motor_ptr->control.Iv_measured_q14;
    if (temp_motor < 0)
        motor_ptr->opdcl.Sum_Iv_m += temp_motor;
    else
        motor_ptr->opdcl.Sum_Iv_p += temp_motor;

    temp_motor = motor_ptr->control.Iw_measured_q14;
    if (temp_motor < 0)
        motor_ptr->opdcl.Sum_Iw_m += temp_motor;
    else
        motor_ptr->opdcl.Sum_Iw_p += temp_motor;
    //debugio1_toggle();
#endif
}
#endif  //#ifdef ENABLE_OPD_CL

#endif
