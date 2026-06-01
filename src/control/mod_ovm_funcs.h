
/**
 * @file overmod_funcs.h
 * @brief Use for FOC over modulation function
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

#ifndef OVERMOD_FUNCS_H
#define OVERMOD_FUNCS_H

#include "motor_struct.h"

/**
 * @brief check_overmod_status, use to check over modulation flag status
 * @param Struct_Motor* motor_ptr motor pointer to FOC structure
 */
extern void check_overmod_status(Struct_Motor *motor_ptr);

#endif
