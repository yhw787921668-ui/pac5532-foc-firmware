
/**
 * @file  trig.h
 * @brief Trigonometric support function declarations
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

#ifndef TRIG_H
#define TRIG_H

#include "maths.h"

extern const uint16_t foc_atan_tbl[];

int32_t trig_arctan_90deg_q16(int32_t opposite_q16, int32_t adjacent_q16);

/**
 * @brief  Calculate the sine of the given angle
 * @param  angle input angle
 * @return sin(angle)
 */
static inline int32_t f_sin_q14(fix16_t angle)
{
    int32_t sin_value;

    if (angle <= 0)
        sin_value = -foc_sin_tbl[((-angle) >> 10)];    // scale to 0 to 201 (pi*64)
    else
        sin_value = foc_sin_tbl[(angle >> 10)];        // scale to 0 to 201 (pi*64)

    return (sin_value >> 2);
}

/**
 * @brief  Calculate the cosine of the given angle
 * @param  angle input angle
 * @return cos(angle)
 */
static inline int32_t f_cos_q14(fix16_t angle)
{
    int32_t cos_value;
    int32_t temp_angle;

    // cos(x) = sin(x + pi/2)                           //TODO: replace sin_angle name with just "angle"
    temp_angle = angle + FIX16_PI_DIV_2;              // angle + pi/2 ; Q16 + Q16 = Q16
    if (temp_angle > FIX16_PI)
        temp_angle = angle - FIX16_1P5_PI;        // angle - 1.5*pi; Q16 - Q16 = Q16

    if (temp_angle <= 0)
        cos_value = -foc_sin_tbl[((-(temp_angle)) >> 10)];   // scale to 0 to 201 (pi*64)
    else
        cos_value = foc_sin_tbl[((temp_angle) >> 10)];       // scale to 0 to 201 (pi*64)

    return (cos_value >> 2);
}

/**
 * @brief  Calculate alpha and beta of the given angle and vector magnitude
 * @param  angle input angle
 * @param  vectormag input vector and magnitude
 * @param  a alpha (output)
 * @param  b beta (output)
 */
static inline void calculate_alphabeta_q14(fix16_t angle, int32_t vectormag, int32_t *a, int32_t *b)
{
    // a = sin(angle) * vectormag
    *a = (-f_sin_q14(angle) * vectormag) >> 14;         // q14 * q14 = q28>>14 = q14

    // b = cos(angle) * vectormag
    *b = (f_cos_q14(angle) * vectormag) >> 14;         // q14 * q14 = q28>>14 = q14
}

#endif  //#ifndef TRIG_H
