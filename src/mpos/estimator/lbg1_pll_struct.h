/**
 * @file lbg1_pll_struct.h
 * @brief Estimator PLL structure
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

#ifndef LBG1_PLL_STRUCT_H
#define LBG1_PLL_STRUCT_H

#include "pid.h"

/**
 * @brief Estimator structure
 */
typedef struct
{
    // PLL estimator variables
    int32_t pll_dt_q22;
    int32_t nom_spd_1_div_q26;
    int32_t pll_estimated_angle_q16;
    int32_t pll_amplitude_filter_q16;
    int32_t pll_amplitude_fc_q16;
    int32_t pll_amplitude_alpha_q16;
    int32_t pll_calculated_angle_q16;
    int32_t pll_nom_spd_q2;
    int32_t pll_estimated_speed_q16;
    int32_t pll_bandwidth_q16;
    int32_t pll_dampingfactor_q16;
    int32_t pll_ka_target_q16;
    int32_t pll_kb_target_q16;
    int32_t pll_gain_scaling_factor_q16;
    int32_t pll_gain_inc_pct_q16;
    uint32_t pll_gain_increment_ka;
    uint32_t pll_gain_increment_kb;
    uint32_t est_pll_transition_complete;

    PID_Data_Type pll_pid;

    int32_t current_amplitude_q16;
    int32_t amplitude_abs_q16;
    int32_t amplitude_abs_threshold_q16;
    int32_t current_error_q16;
    int32_t pll_current_error_normalize_q14;
    int32_t pll_angle_change_q16;
    int32_t pll_amplitude_change_q16;

}Struct_Est_PLL;

extern Struct_Est_PLL est_pll;

#endif  //#ifndef LBG1_PLL_STRUCT_H
