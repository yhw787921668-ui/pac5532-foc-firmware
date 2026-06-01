
/**
 * @file overmod_funcs.c
 * @brief Use for FOC over modulation function
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

#include "mod_ovm_funcs.h"
#include "fault.h"

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
void check_overmod_status(Struct_Motor *motor_ptr)
{
    if (motor_ptr->overmod.overmod_flag == 1)
    {
        set_status(STATUS_MOTOR_OVERMOD);
        motor_ptr->overmod.overmod_flag = 0;
    }
}

#endif
