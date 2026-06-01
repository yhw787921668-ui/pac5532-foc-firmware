
/**
 * @file  sqrt.h
 * @brief Square root function declarations
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

#ifndef SQRT_Q14_H
#define SQRT_Q14_H

#include "pac5xxx.h"
#include "maths.h"

#define SQRT_ITER       2
#define K1_Q14          QCONST(1.1033f, Q14)
#define K2_Q14          QCONST(0.6666f, Q14)
#define K3_Q14          QCONST(1.5f, Q14)

/**
 * @brief Calculate the square root of the given number
 * @param input_q14 input number in Q14 format
 * @return square root
 */
extern PAC5XXX_RAMFUNC int32_t sqrt_q14 (int32_t input_q14);

#endif  //#ifndef SQRT_Q14_H
