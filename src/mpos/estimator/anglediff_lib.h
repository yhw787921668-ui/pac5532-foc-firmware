
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

#ifndef ANGLEDIFF_LIB_H
#define ANGLEDIFF_LIB_H

#include "maths.h"

extern void minimize_anglediff(fix16_t angle_diff_f16, fix16_t *idrefol_target_f16, fix16_t iqrefol_target_f16, fix16_t i_total_ol_target_f16, fix16_t idref_ramp_f16, int32_t autotune_inc_pct_q10, uint32_t idref_bidirection_flag);
extern void minimize_anglediff_2(fix16_t anglediff_f16, fix16_t *idrefol_target_f16, fix16_t *i_total_ol_target_f16, fix16_t idref_ramp_f16, int32_t autotune_inc_pct_q10, int32_t itot_max_q16, int32_t filter_alpha_anglediff);

#endif  //#ifndef ANGLEDIFF_LIB_H
