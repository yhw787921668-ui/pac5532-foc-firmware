
/**
 * @file  trig.c
 * @brief Trigonometric support functions
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

#include "pac5xxx.h"
#include "maths.h"
#include "trig.h"


int32_t trig_arctan_90deg_q16(int32_t opposite_q16, int32_t adjacent_q16)
{
    int32_t result_q16;
    int32_t arg_q16;

    if (opposite_q16 < adjacent_q16)
    {
        arg_q16 = fix16_div(opposite_q16, adjacent_q16);
        result_q16 = foc_atan_tbl[arg_q16 >> 8];
    }
    else
    {
        arg_q16 = fix16_div(adjacent_q16, opposite_q16);
        result_q16 = FIX16_PI_DIV_2 - foc_atan_tbl[arg_q16 >> 8];
    }

    return result_q16;
}
