/**
 * @file satmon.c
 * @brief Use for PWM_SATURATED check
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

#include "satmon.h"
#include "fault.h"

void check_pwm_sat(Struct_Motor* motor_ptr)
{
    //////////////////////////
    // Handle Logic AND case
    //////////////////////////
#if 0
    // If saturated
    if (motor_ptr->foc.flag_sat_logicand == 1)
    {
        set_warning(WARNING_PWM_SATURATED);
    }
    // If not saturated
    else
    {
        clear_warning(WARNING_PWM_SATURATED);
        motor_ptr->foc.flag_sat_logicand = 1;
    }
#endif

    //////////////////////////
    // Handle Logic OR case
    //////////////////////////
    // If saturated
    if (motor_ptr->foc.flag_sat_logicor == 1)
    {
        set_warning(WARNING_PWM_SATURATED);
        motor_ptr->foc.flag_sat_logicor = 0;
    }
    // If not saturated
    else
    {
        clear_warning(WARNING_PWM_SATURATED);
    }

}
