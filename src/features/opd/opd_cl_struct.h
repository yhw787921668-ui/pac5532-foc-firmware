/**
 * @file opd_cl_struct.h
 * @brief Use for defining OPD structure
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

#ifndef OPDCL_STRUCT_H
#define OPDCL_STRUCT_H

#include "maths.h"

/**
 * @brief opd cl structure
 */
typedef struct
{
    fix16_t Sum_Iu_updated;
    fix16_t Sum_Iv_updated;
    fix16_t Sum_Iw_updated;
    uint32_t phase_open_fault_last_sample_time_value;
    uint32_t ElectCycleCount_Temp;
    fix16_t diff1;
    fix16_t error_in_current_diff1;
    fix16_t error_in_current_diff2;
    fix16_t Sum_Iu_p;
    fix16_t Sum_Iv_p;
    fix16_t Sum_Iw_p;
    fix16_t Sum_Iu_m;
    fix16_t Sum_Iv_m;
    fix16_t Sum_Iw_m;

    fix16_t Iu_sq_q24;
    fix16_t Iv_sq_q24;
    fix16_t Iw_sq_q24;
    fix16_t Iu_sq_f_q24;
    fix16_t Iv_sq_f_q24;
    fix16_t Iw_sq_f_q24;
    fix16_t filter_coef_q10;

    fix16_t opdcl_thd_percentage_q3;
    uint32_t opd_count_threshold;
    uint32_t opdcl_sampletime_systicks;
    int32_t opdcl_minspeed_radps_q16;
    uint32_t flag_prev_spd_above_min;
    fix16_t opdcl_current_thd;
    fix16_t opdcl_current_thd_sq;
    fix16_t opdcl_one_div_sample_time;
} Struct_OPDCL;

#endif
