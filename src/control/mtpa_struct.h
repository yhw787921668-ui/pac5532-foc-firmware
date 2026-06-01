
/**
 * @file mtpa_struct.h
 * @brief Use for FOC mtpa structure
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#ifndef MTPA_STRUCT_H
#define MTPA_STRUCT_H

#include "maths.h"

/**
 * @brief MTPA structure
 */
typedef struct
{
    int32_t mtpa_gain_degpa_q16;                      // MTPA gain, degree per ampere
    int32_t mtpa_gain_rad_q16;                        // MTPA gain, scaled
    int32_t mtpa_current_filter_coeff_q16;
    int32_t mtpa_iqf_q14;
    int32_t mtpa_angle_rad_q16;                       // Calculated MTPA angle
    int32_t mtpa_angle_ramp_rate_q16;
    int32_t mtpa_saliency_q16;
    int32_t mtpa_max_current_ampere_q16;
    int8_t  mtpa_auto_gain_calc_flag;
    int32_t mtpa_current_scale_q16;
    int32_t mtpa_current_array[11];
    int32_t mtpa_angle_array_rad[11];
    int32_t mtpa_coff_q16[2];
} Struct_Mtpa;

#endif
