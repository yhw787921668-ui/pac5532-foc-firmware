
/**
 * @file  fast_div.h
 * @brief Fast divider definitions
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

#ifndef FAST_DIV_H
#define FAST_DIV_H

#include "pac5xxx.h"
#include "maths.h"

#define X0A_Q14         (0x0002D2D2 >> 2)       // 48/17 in q14
#define X0B_Q14         (0x0001E1E1 >> 2)       // 32/17 in q14
#define ONE_Q14         (0x00010000 >> 2)       // 1 in q14

/**
 * @brief  Fast Q-Math Divider
 * @param  dividend Dividend
 * @param  divisor Divisor
 */
extern PAC5XXX_RAMFUNC uint32_t div_fast(int32_t dividend, int32_t divisor);

#endif  //#ifndef FAST_DIV_H
