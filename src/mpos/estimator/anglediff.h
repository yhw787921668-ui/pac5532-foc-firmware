
/**
 * @file anglediff.h
 * @brief Use to check for angle difference between open loop and est angle and est speed thrd
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

#ifndef ANGLEDIFF_H
#define ANGLEDIFF_H

#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"
#include "anglediff_lib.h"


#ifdef ENABLE_IDREF_BIDIRECTION
    #define ENABLE_IDREF_BIDIRECTION_FLAG   TRUE
#else
    #define ENABLE_IDREF_BIDIRECTION_FLAG   FALSE
#endif


#if (MODULE_MPOS_LBG1 == ENABLED)
void anglediff_check(Struct_Motor* motor_ptr, int32_t ref_angle, int32_t check_angle, int32_t ref_speed_radps, int32_t check_speed_radps);
void anglediff_autorange(Struct_Motor *motor_ptr);
void anglediff_timeout_check(Struct_Motor *motor_ptr);
#endif

#endif  //#ifndef ANGLEDIFF_H
