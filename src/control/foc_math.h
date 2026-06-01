
/**
 * @file foc_math.h
 * @brief Use for basic FOC math functions, like park, clarke, inverse park, SVM
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

#ifndef FOC_MATH_H
#define FOC_MATH_H

#include "pac5xxx.h"

/**
 * @brief SVM Enum
 */
typedef enum
{
    SVM_SECTOR_NOT_VALID = -1,
    SVM_SECTOR_1 = 1,
    SVM_SECTOR_2 = 2,
    SVM_SECTOR_3 = 3,
    SVM_SECTOR_4 = 4,
    SVM_SECTOR_5 = 5,
    SVM_SECTOR_6 = 6,
}SVSector;


/**
 * @brief park inverse park math table
 * @param fix16_t* f_sin
 * @param fix16_t* f_cos
 * @param fix16_t sin_angle
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void park_inverse_park_basic_table_cal (fix16_t* f_sin, fix16_t* f_cos, fix16_t sin_angle)
{
    fix16_t f_temp1;

    // cos(x) = sin(x + pi/2)                           //TODO: replace sin_angle name with just "angle"
    f_temp1 = sin_angle + FIX16_PI_DIV_2;              // angle + pi/2 ; Q16 + Q16 = Q16
    if (f_temp1 > fix16_pi)
        f_temp1 = sin_angle - FIX16_1P5_PI;        // angle - 1.5*pi; Q16 - Q16 = Q16

    if (f_temp1 <= 0)
        *f_cos = -foc_sin_tbl[((-(f_temp1)) >> 10)];   // scale to 0 to 201 (pi*64)
    else
        *f_cos = foc_sin_tbl[((f_temp1) >> 10)];       // scale to 0 to 201 (pi*64)

    // sin
    if (sin_angle <= 0)
        *f_sin = -foc_sin_tbl[((-sin_angle) >> 10)];    // scale to 0 to 201 (pi*64)
    else
        *f_sin = foc_sin_tbl[(sin_angle >> 10)];        // scale to 0 to 201 (pi*64)
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief clarke transformation
 * @param int32_t Iu
 * @param int32_t Iv
 * @param int32_t Iw
 * @param int32_t* a
 * @param int32_t* b
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void clarke_transform_q14(int32_t Iu, int32_t Iv, int32_t Iw, int32_t* a, int32_t* b)
{
    *a = Iu;
    *b = ((Iv - Iw) * ZERO_POINT_5773_IN_Q14) >> 14;    // Q14 - Q14 = Q14, Q14 * Q14 = Q28 >> 14 = Q14
}
PAC5XXX_OPTIMIZE_SPEED_OFF



/**
 * @brief park transformation
 * @param fix16_t sin_angle
 * @param int32_t f_alpha
 * @param int32_t f_beta
 * @param int32_t* f_d
 * @param int32_t* f_q
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void park_transform_q14(fix16_t sin_angle, int32_t f_alpha, int32_t f_beta, int32_t* f_d, int32_t* f_q)
{
    fix16_t f_sin;
    fix16_t f_cos;
    int32_t q_temp1, q_temp2, q_sin, q_cos;


    park_inverse_park_basic_table_cal(&f_sin, &f_cos, sin_angle);

    q_cos = f_cos >> 2;                 // Q16 >> 2 = Q14
    q_sin = f_sin >> 2;                 // Q16 >> 2 = Q14

    // PARK transform
    q_temp1 = (f_alpha * q_cos) >> 14;  // alpha * cos(angle); Q14 * Q14 = Q28 >> 14 = Q14
    q_temp2 = (f_beta * q_sin) >> 14;   // beta * sin(angle); Q14 * Q14 = Q28 >> 14 = Q14
    *f_d = q_temp1 + q_temp2;           // d = alpha * cos(angle) + beta * sin(angle); Q14 + Q14 = Q14
    q_temp1 = (f_alpha * q_sin) >> 14;  // alpha * sin(angle); Q14 * Q14 = Q28 >> 14 = Q14
    q_temp2 = (f_beta * q_cos) >> 14;   // beta * cos(angle); Q14 * Q14 = Q28 >> 14 = Q14
    *f_q = q_temp2 - q_temp1;           // q = beta *cos(angle) - alpha * sin(angle); Q14 - Q14 = Q14
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief inverse_park_transform
 * @param fix16_t sin_angle
 * @param int32_t d
 * @param int32_t q
 * @param int32_t* a
 * @param int32_t* b
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void inverse_park_transform_q14(fix16_t sin_angle, int32_t d, int32_t q, int32_t* a, int32_t* b)
{
    fix16_t f_sin, f_cos;
    int32_t temp3_q14, q_sin, q_cos;

    park_inverse_park_basic_table_cal(&f_sin, &f_cos, sin_angle);

    q_cos = f_cos >> 2;                 // Q16 >> 2 = Q14
    q_sin = f_sin >> 2;                 // Q16 >> 2 = Q14

    // a = cos(angle) * d - sin(angle) * q
    *a = (q_cos * d) >> 14;             // a = cos(angle) * d; Q14 * Q14 = Q28 >> 14 = Q14
    temp3_q14 = (q_sin * q) >> 14;      // temp3 = sin(angle) * q; Q14 * Q14 = Q28 >> 14 = Q14
    *a = (*a) - temp3_q14;              // a = cos(angle) * d - sin(angle) * q; Q14 - Q14 = Q14

    // b = sin(angle) * d + cos(angle) * q
    *b = (q_sin * d) >> 14;             // b = sin(angle) * d; Q14 * Q14 = Q28 >> 14 = Q14
    temp3_q14 = (q_cos *q) >> 14;       // temp3 = cos(angle) * q; Q14 * Q14 = Q28 >> 14 = Q14
    *b = (*b) + temp3_q14;              // b = sin(angle) * d + cos(angle) * q;  Q14 + Q14 = Q14
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function         : inverse_clarke_transform
// Description      : Calculates the inverse Clarke Transform
// Input signals    : alpha, beta
// Input range      : [+/- 1.0]
// Input format     : q14
// Output signals   : a, b, c
// Output range     : [+/- 1.0]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_RAMFUNC static inline void inverse_clarke_transform(int32_t alpha_q14, int32_t beta_q14, int32_t *a_q14, int32_t *b_q14, int32_t *c_q14)
{
    // a = alpha
    *a_q14 = alpha_q14;

    // b = (-1/2 * alpha) + (sqrt(3)/2 * beta)
    *b_q14 = (-alpha_q14 >> 1) + ((Q14_SQRT3_DIV_2 * beta_q14) >> 14);

    // c = (-1/2 * alpha) - (sqrt(3)/2 * beta)
    *c_q14 = (-alpha_q14 >> 1) - ((Q14_SQRT3_DIV_2 * beta_q14) >> 14);
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef FOC_MATH_H

