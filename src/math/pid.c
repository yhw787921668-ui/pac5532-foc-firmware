
/**
 * @file  pid.c
 * @brief PID functions
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

#include "pid.h"
#include "fault.h"

//#define ENABLE_PID_OVERFLOW_PREVENTION                          // For potential super high power application

///
/// @brief Initializes the PID controller data
///
/// @param pid_data Pointer to the PID controller data
/// @returns none
///
/// @note Note that the user must make sure that the pointer is valid and that the user-defined
///       fields in the data structure are initialized properly (see the definition for the
///        \a PID_Data_Type data structure).
///
void pid_reset(PID_Data_Type* pid_data)
{
    pid_data->I_prev = 0;
    pid_data->PI = 0;
    pid_data->PI_sat = 0;
}


void picon_convert_gain(PID_Data_Type* pid_data, uint32_t scale_in_q16, int32_t scale_out_q16)
{
#ifndef ENABLE_PID_OVERFLOW_PREVENTION
    int32_t temp1_q16, temp2_q16;
#else
    int32_t temp1_q16, temp2_q14;
#endif

    ///////////////////////////////
    // Calculate Kp
    ///////////////////////////////

    // Calculate Kp = ka * (scale_in/scale_out)
    // q16/q16 = q16, calculate 1/scale_out_q16
    temp1_q16 = fix16_div(FIX16_1P0, scale_out_q16);
    // q16 = q16 * q16, (fix16_1/scale_out_q16) * scale_in_q16
    temp1_q16 = fix16_mul_uint(temp1_q16, scale_in_q16);
    // q16 = q16 * q16
    temp1_q16 = fix16_mul(pid_data->ka_q16, temp1_q16);
    pid_data->kp_q11 = temp1_q16 >> 5;

    ///////////////////////////////
    // Calculate Ki
    ///////////////////////////////

    // Ki = Kb * Kp * Ts
    // q6 = q6 * q16
#ifndef ENABLE_PID_OVERFLOW_PREVENTION
    temp2_q16 = fix16_mul(pid_data->kb_q16, temp1_q16);
    // q16 = q6 * q26
    pid_data->kidt_q16 = (fix16_mul(temp2_q16, pid_data->dt_q26) >> 10);
#else
    temp2_q14 = fix16_mul(pid_data->kb_q16, (temp1_q16 >> 2));
    // q16 = q6 * q26
    pid_data->kidt_q16 = (fix16_mul(temp2_q14, pid_data->dt_q26) >> 8);
#endif

    ///////////////////////////////
    // Check PI Gain Range
    ///////////////////////////////

    if (pid_data->kp_q11 > QCONST(63, Q11))      // Kp max = 2^6=64 to overflow during q-math multiplication
    {
        pid_data->kp_q11 = QCONST(63, Q11);
        set_warning(WARNING_PIGAIN_RANGE);
    }

    if (pid_data->kidt_q16 > QCONST(1, Q16))     // kitd max = 1 to overflow during q-math multiplication
    {
        pid_data->kidt_q16 = QCONST(1, Q16);
        set_warning(WARNING_PIGAIN_RANGE);
    }
}
