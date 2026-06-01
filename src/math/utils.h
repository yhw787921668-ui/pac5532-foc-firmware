
/**
 * @file  utils.h
 * @brief Utility function declarations
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

#ifndef UTILS_H
#define UTILS_H

#include "pac5xxx.h"

/**
 * @brief  Convert open loop to closed loop speed
 * @param  dt_control_1div_q6 1 / dt
 * @param  ol_speed open loop speed
 * @return closed loop speed
 */
static inline fix16_t conv_ol2cl_speed(fix16_t dt_control_1div_q6, fix16_t ol_speed)
{
    return ((ol_speed >> 9) * (dt_control_1div_q6 >> 7));      // f16>>9=q7 * f16>>7=q9 = q16 (rad/s)
}

/**
 * @brief  Wrap angle
 * @param  angle_q16 angle (input and output)
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void wraparound(int32_t *angle_q16)
{
    if (*angle_q16 > FIX16_PI)
        *angle_q16 = *angle_q16 - FIX16_2PI;
    else if (*angle_q16 < -FIX16_PI)
        *angle_q16 = *angle_q16 + FIX16_2PI;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
/**
 * @brief  Circle limiter
 * @param  vectormag_f16 vector
 * @param  vectormag_1div_f16 1 / vectormag
 * @param  vector_fixed_in_f16 Fixed vector
 * @param  vector_limited_out_f16 Output vector limited
 */
static inline void circle_limiter(int32_t vectormag_f16, int32_t vectormag_1div_f16, int32_t vector_fixed_in_f16, int32_t *vector_limited_out_f16)
{
    // Calculate Iq_ref_ol value that combined with Id_ref_ol does not exceed the limit current of itot_ref_ol_tgt
    // Iq_ref_ol = itot_ref_ol_tgt * circle_table(Id_ref_ol / itot_ref_ol_tgt)
    volatile fix16_t temp1;

    temp1 = ((vector_fixed_in_f16 * (vectormag_1div_f16 >> 2)) >> 14);    // q16 * q16>>2=q14 = q30>>14=q16
    // Generate circle table index (0-256)
    if (temp1 < 0)          // JAGTAG: Need to find a cheap way to smash the sign bit
        temp1 = -temp1;
    temp1 = (temp1 >> 8);
    if (temp1 > 256)
        temp1 = 256;

    temp1 = foc_circle_tbl[temp1];                                              // q16
    temp1 = ((temp1 * (vectormag_f16 >> 2)) >> 14);      // q16 * q16>>2=q14 = q30>>14=q16
    *vector_limited_out_f16 = temp1;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:        : anglewrap
// Description      : Converts angle so that it ranges between +/-pi
// Input signals    : angle (radians)
// Input range      : [+/- 32768]
// Input format     : q16
// Output signals   : angle (radians)
// Output range     : [+/- pi]
// Output format    : q16
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline int32_t anglewrap(int32_t angle_q16)
{
    int32_t temp;

    temp = angle_q16;

    while (temp > FIX16_PI)
    {
        temp = temp - FIX16_2PI;
    }

    while (temp < -FIX16_PI)
    {
        temp = temp + FIX16_2PI;
    }

    return temp;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:        : f_sign_q14
// Description      : Returns the sign of argument
// Input signals    : value
// Input range      : [+/- 32768]
// Input format     : q14
// Output signals   : sign
// Output range     : [+/- 1.0]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline int32_t f_sign_q14(int32_t value)
{
    if (value > 0)
        return Q14_1P0;
    else if (value < 0)
        return -Q14_1P0;
    else
        return 0;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline int32_t absval(int32_t value)
{
    if (value < 0)
        return -value;
    else
        return value;
}


/**
 * @brief Empty function
 * @param Struct_Motor* motor_ptr pointer to motor structure, not used
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static void empty_function(void *v_motor_ptr)
{
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef UTILS_H
