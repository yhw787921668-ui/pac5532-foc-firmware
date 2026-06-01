
/**
 * @file aligngo_funcs.c
 * @brief Use for writing align and go variables
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

#include "config_app.h"
#include "aligngo_funcs.h"
#include "opd_cl_funcs.h"


#if (MODULE_MPOS_ALIGNGO == ENABLED)
/**
 * @brief Process parameter write
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void aligngo_param_write(Struct_Motor* motor_ptr, ParamCodeAlignGo param_code, int32_t rx_data, Struct_AlignGoCMD* aligngocmd_ptr)
{
    int32_t temp_q16;
    switch(param_code)
    {
    case param_aligngo_align_angle:
        temp_q16 = fix16_mul(rx_data, CONV_DEG2RAD_FIX16);
        wraparound(&temp_q16);
        aligngocmd_ptr->alignangle_cmd_q26 = (temp_q16 << 10);
        break;
    case param_aligngo_align_time:
        aligngocmd_ptr->aligntime_cmd = rx_data;
        break;
    case param_aligngo_speed_ramprate:
        aligngocmd_ptr->speedrefol_ramprate_cmd_q26 = rx_data;
        break;
    case param_aligngo_speed_target:
        aligngocmd_ptr->speedrefol_cmd_q26 = rx_data;
        break;
    case param_aligngo_id_ref:
        aligngocmd_ptr->idrefol_cmd_q16 = rx_data;
        break;
    case param_aligngo_itot_ref:
        aligngocmd_ptr->itotrefol_cmd_q16 = rx_data;
        break;
    default:
        break;
    }
}


void aligngo_setvars(Struct_Motor* motor_ptr, Struct_AlignGoCMD* aligngocmd_ptr)
{
    motor_ptr->mpos.aligngo.alignangle_target_q26 = aligngocmd_ptr->alignangle_cmd_q26;
    motor_ptr->mpos.aligngo.aligntime_target = aligngocmd_ptr->aligntime_cmd;
    motor_ptr->mpos.aligngo.speedrefol_target_q26 = aligngocmd_ptr->speedrefol_cmd_q26 * motor_ptr->app.app_motor_direction;
    motor_ptr->mpos.aligngo.speedrefol_target_radps_q16 = ((motor_ptr->mpos.aligngo.speedrefol_target_q26 >> 9) * (motor_ptr->control.dt_control_1div_q6 >> 7));        // q26>>9=q17 * q6>>7=qm1 = q16 (rad/s)
    motor_ptr->mpos.aligngo.idrefol_target_q16 = aligngocmd_ptr->idrefol_cmd_q16;
    write_itotstart_target(aligngocmd_ptr->itotrefol_cmd_q16, &motor_ptr->mpos.aligngo.itotrefol_target_q16, &motor_ptr->mpos.aligngo.itotrefol_target_1div_q16);
    // Run circle function to limit the Iq and Id
    circle_limiter(motor_ptr->mpos.aligngo.itotrefol_target_q16, motor_ptr->mpos.aligngo.itotrefol_target_1div_q16, motor_ptr->mpos.aligngo.idrefol_target_q16, &motor_ptr->mpos.aligngo.iqrefol_target_q16);
    motor_ptr->mpos.aligngo.iqrefol_target_q16 = motor_ptr->mpos.aligngo.iqrefol_target_q16 * motor_ptr->app.app_motor_direction;

#ifdef ENABLE_OPEN_LOOP_NONLINEAR_RAMP
    int32_t temp_q26;

    // JAGTAG: Why need absval? Will this work with negative speed?
    motor_ptr->mpos.aligngo.speedrefol_target_stg1_q26 = (absval(aligngocmd_ptr->speedrefol_cmd_q26) * OPEN_LOOP_START_SPEED_TARGET_PCT_Q08) >> 8;         // q16*q8 = q24 -> 8 = q16
    motor_ptr->mpos.aligngo.speedrefol_ramprate_q26 = (absval(aligngocmd_ptr->speedrefol_ramprate_cmd_q26) * OPEN_LOOP_START_RAMP_PCT_Q08) >> 8;

#ifdef ENABLE_CURRENT_RAMP_DURING_FIRST_SLOPE
    // I_ramprate = (I_target * Spd_ramprate) / Spd_target
    temp_q26 = (motor_ptr->mpos.aligngo.itotrefol_target_q16 >> 6) * motor_ptr->mpos.aligngo.speedrefol_ramprate_q26;                                         // q16->6 = q10 *q16 = q26
    // JAGTAG: Why need absval? Will this work with negative speed?
    if (motor_ptr->mpos.aligngo.speedrefol_target_stg1_q26 > 0)
        motor_ptr->mpos.aligngo.irefol_ramprate_q16 = fix16_div(temp_q26, motor_ptr->mpos.aligngo.speedrefol_target_stg1_q26) >> 10;
    // JAGTAG: Review this overflow prevention
    // Set min ramping rate in case of calculation overflow
    if (motor_ptr->mpos.aligngo.irefol_ramprate_q16 <= 0)
        motor_ptr->mpos.aligngo.irefol_ramprate_q16 = 1;
#else
    motor_ptr->mpos.aligngo.irefol_ramprate_q16 = motor_ptr->control.idiqref_rate_gui_f16;
#endif


#else
    motor_ptr->mpos.aligngo.speedrefol_ramprate_q26 = aligngocmd_ptr->speedrefol_ramprate_cmd_q26;
    motor_ptr->mpos.aligngo.irefol_ramprate_q16 = motor_ptr->control.idiqref_rate_gui_f16;
#endif
}
#endif  //#if (MODULE_MPOS_ALIGNGO == ENABLED)
