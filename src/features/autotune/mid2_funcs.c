/**
 * @file mid2_funcs.c
 * @brief Use for the control functions in motor identification 2
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


#include "mid2_funcs.h"


#if defined(ENABLE_AUTOTUNE_MID)

void mid2_init(Struct_Motor *motor_ptr)
{
//    at_reset_state(motor_ptr);

    motor_ptr->atune.anoise_delay_cycles = MID2_ANOISE_DELAY_CYCLES;
    motor_ptr->atune.anoise_threshold_multp = MID2_ANOISE_THRESHOLD_MULTP;
    motor_ptr->atune.anoise_fault_retries = MID2_ANOISE_FAULT_RETRIES;
    motor_ptr->atune.accdet_thresh_q22 = MID2_ACCDET_THRESH_Q22;
    motor_ptr->atune.speed_settled_majfilt_limit = MID2_SPEED_SETTLED_MAJFILT_LIMIT;
//    motor_ptr->atune.iqref_start_pct_q16 = MID2_IQREF_START_PCT_Q16;
    motor_ptr->atune.iqref_inc_pct_q16 = MID2_IQREF_INC_PCT_Q16;

    // Calculate acc-det filter gain
    motor_ptr->atune.filter_alpha_acc2_q16 = calc_filter_alpha(MID2_FILTER_FC_ACCDET_Q16, DT_DIVSYSTICK_Q26);
}

#endif  //#if defined(ENABLE_AUTOTUNE_MID)
