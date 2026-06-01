
/**
 * @file control_power1_struct.h
 * @brief Use for control power measurement storage
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

#ifndef CONTROL_POWER1_STRUCT_H
#define CONTROL_POWER1_STRUCT_H

#include "maths.h"

/**
 * @brief This structure use for power control
 */
typedef struct
{
    int32_t power_iphase_offset_q14;
    int32_t power_iphase_factor_q14;
#ifdef POWER_DTCOMP
    int32_t dtcomp_thr_pos;                             // Positive deadtime compensation for power calculation
    int32_t dtcomp_thr_neg;                             // Negative deadtime compensation for power calculation
#endif
    int32_t power_out_1divva_q14;
    int32_t filter_alpha;
    int32_t power_out_1divva_filter_q14;
    int32_t power_out_1divva_filter_q30;
    int32_t ibus_avg_q16;
    int32_t power_preresult;
    int32_t power_result;
    int32_t current_descale_factor_q16;
} Struct_Power;

#endif  //#ifndef CONTROL_POWER1_STRUCT_H
