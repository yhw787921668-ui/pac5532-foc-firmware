
/**
 * @file harmonic_inj_funcs.h
 * @brief Use for SVM harmonic injection
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

#ifndef HARMONIC_INJ_FUNCS_H
#define HARMONIC_INJ_FUNCS_H

#include "motor_struct.h"
#include "harmonic_inj_libfuncs.h"


// Compile Definitions
#define CONFIG_HINJ_ORDER_Q0        5               // Order of harmonic to be injected

// Function Definitions
static inline void hinj_init(Struct_Motor* motor_ptr)
{
    motor_ptr->harmonicinj.harmonicinj_order_q0 = CONFIG_HINJ_ORDER_Q0;
}

#endif  //#ifndef HARMONIC_INJ_FUNCS_H
