/**
 * @file lbg1_struct.h
 * @brief Estimator structure
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

#ifndef LBG1_STRUCT_H
#define LBG1_STRUCT_H

#include "pid.h"

/**
 * @brief Estimator structure
 */
typedef struct
{
    // State & Status
    uint32_t state;
    uint32_t est_transition_complete : 1;
    uint32_t lbg_valid_fault_flag : 1;
    uint32_t flag_estbw_high : 1;
    uint32_t temp : 29;

    // Flow Control
    int32_t angle_diff_timeout_counter;
    int32_t lbg_valid_check_counter;

    // Inputs
    int32_t ialpha1_q14;
    int32_t ibeta1_q14;
    int32_t valpha1_q14;
    int32_t vbeta1_q14;
    int32_t est_motor_direction;

    // Outputs
    int32_t est_spd_radps;
    int32_t est_spd_scaled_q14;
    int32_t est_spd_scaled_q28;
    int32_t est_spd_prefilt_scaled_q14;
    int32_t estimated_angle;
    int32_t calculated_angle;

    // Running Variables
    int32_t est_ialpha_q14;
    int32_t est_ibeta_q14;
    int32_t est_ealpha_q14;
    int32_t est_ebeta_q14;
    int32_t angle_previous;
    int32_t valpha_sreg0_q14;
    int32_t vbeta_sreg0_q14;
    int32_t speed_switchover;
    int32_t blend_spd_radps;

    // Constants
    int32_t fs_q15;
    int32_t gs_const_q16;
    int32_t gs_q15;
    int32_t kpe_store_q16;
    int32_t kpe_const_q14;
    int32_t kpe_vrble_q14;
    int32_t kpi_store_q16;
    int32_t kpi_const_q14;
    int32_t filter_fc_speed_q16;
    int32_t filter_alpha_speed_q16;
    int32_t kspd_radps_q16;
    int32_t kspd_scaled_q14;
    uint32_t speed_scale_shift;
    int32_t angcorr_gain_q20;
    int32_t angcorr_offset;
    int32_t w0_q06;
    int32_t w0_target_q06;
    int32_t starting_w0_q16;
    int32_t w0_inc_pct_q16;
    int32_t w0_ramp_q06;
    int32_t dampfactor_q16;
    int32_t phscomp_multp_q20;
}Struct_Est;

extern Struct_Est est;

#endif  //#ifndef LBG1_STRUCT_H
