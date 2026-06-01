
/**
 * @file speed_check.c
 * @brief Use for check speed min - max limit and generate speed fault
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

#ifndef SPEED_CHECK_H
#define SPEED_CHECK_H

#include "motor_struct.h"

extern void check_speedtargetol_warning(Struct_Motor* motor_ptr);
/**
 * @brief  Use for checking speed min - max limit and generate speed fault
 * @param Struct_Motor* motor_ptr
 */
extern void check_speedlimitcl_fault(Struct_Motor* motor_ptr);

#endif  //#ifndef SPEED_CHECK_H
