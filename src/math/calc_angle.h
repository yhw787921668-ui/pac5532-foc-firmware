
/**
 * @file calc_angle.h
 * @brief Definitions for angle calculation
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

#ifndef CALC_ANGLE_H
#define CALC_ANGLE_H

#include "pac5xxx.h"
#include "maths.h"

extern const uint16_t foc_atan_tbl[];

/**
 * @brief  Calculates the angle based on alpha and beta parameters
 * @param  Ealpha Estimated alpha
 * @param  Ebeta Estimated beta
 */
extern PAC5XXX_RAMFUNC void calculate_angle(int32_t Ealpha, int32_t Ebeta, fix16_t *calculated_angle);

#endif  //#ifndef CALC_ANGLE_H
