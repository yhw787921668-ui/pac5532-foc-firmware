/**
 * @file mid2_funcs.h
 * @brief Use for the function definitions in mid2
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

#ifndef MID2_FUNCS_H
#define MID2_FUNCS_H

#include "motor_struct.h"
#include "atune_libfuncs.h"


#if defined(ENABLE_AUTOTUNE_MID)

// Definitions
//#define ENABLE_AUTOTUNE_REGULATE_IQREF            // For testing purposes only

// Value Definitions
#define MID2_ANOISE_DELAY_CYCLES            200                 // Delay after triggering Step1 during which acc noise floor is measured (Units: systick cycles)
#define MID2_ANOISE_THRESHOLD_MULTP         5                   // Acc noise threshold multiplier: Threshold = acc_noise_floor * multiplier (Units: None)
#define MID2_ANOISE_FAULT_RETRIES           6                   // Number of times that the user will be able to retry if the acc noise check fails before the ATPI sequence fails (Units: None)
#define MID2_FILTER_FCDIV_Q16               QCONST(3.0f, Q16)   // Divider for MID2 filter cutoff frequency: fc = f0 / divider
#define MID2_FILTER_FC_ACCDET_Q16           QCONST(0.1f, Q16)   // Filter cutoff frequency applied to acceleration calculation
#define MID2_ACCDET_THRESH_Q22              QCONST(0.001f, Q22) // Acceleration value threshold when detecting if motor speed has settled
#define MID2_SPEED_SETTLED_MAJFILT_LIMIT    200                 // Count in systick clocks for blanking the motor speed settling detection
#define MID2_IQREF_INC_PCT_Q16              QCONST(0.015f, Q16) // Increment for applied to iqref when stepping the current and for current step retries
//#define MID2_IQREF_START_PCT_Q16            QCONST(0.030f, Q16)
//#define MID2_FILTER_FC_ANGLEDIFF_Q16        QCONST(0.1f, Q16)

/**
 * @brief Initialize motor identification 2
 * @param Struct_Motor *motor_ptr
 */
void mid2_init(Struct_Motor *motor_ptr);

/**
 * @brief Process the control sequence for motor identification 2
 * @param Struct_Motor *motor_ptr
 */
static inline void mid2_process_control(Struct_Motor *motor_ptr)
{
    // Process ATMID control loop operations
    mid2_process_control_lib(&motor_ptr->atune, motor_ptr->mpos.mpos_spd_scaled_q14, motor_ptr->foc.vq_q14, motor_ptr->foc.vd_q14, motor_ptr->foc.iq_q14, motor_ptr->mpos.mpos_angle);

    // JAGTAG: For debugging and monitoring, need to formally integrate
    motor_ptr->control.outerloop_fdbk_scaled_q14 = motor_ptr->mpos.mpos_spd_scaled_q14;
}

/**
 * @brief Process the systick timelevel functions for motor identification 2
 * @param Struct_Motor *motor_ptr
 */
static inline void mid2_process_systick(Struct_Motor *motor_ptr)
{
    if (motor_ptr->state.main != STATE_MAIN_MID1)
    {
        // MID2 Controller
        mid2_controller(&motor_ptr->atune, &motor_ptr->control, &motor_ptr->app, &motor_ptr->foc, &motor_ptr->mparam, &motor_ptr->state);

        // Process MID2 systick ISR operations
        mid2_process_systick_lib(&motor_ptr->atune);

        #ifdef ENABLE_AUTOTUNE_REGULATE_IQREF
        regulate_iqref(motor_ptr);
        #endif
    }
}

#endif  //#if defined(ENABLE_AUTOTUNE_MID)
#endif  //#ifndef MID2_FUNCS_H
