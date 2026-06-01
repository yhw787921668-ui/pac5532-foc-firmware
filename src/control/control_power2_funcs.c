
/**
 * @file control_power2_func.c
 * @brief Use for control power2 compensation
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

#include "motor_struct.h"

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && defined(ENABLE_LCORR_VBUS)
#include "control_power2_funcs.h"

void power_compensation_coefficient_calc(Struct_Motor *motor_ptr)
{
    if(motor_ptr->state.control == STATE_CONTROL_OUTERLOOP)
    {
        if(absval(motor_ptr->mpos.mpos_spd_radps) >= CL_SPEED_300HZ)
        {
            if(motor_ptr->control.outerloop_target <= POWER_TARGET_0)
            {
                motor_ptr->power.lcorr_power_m = LCORR_VBUS_M_0;
                motor_ptr->power.lcorr_power_b = LCORR_VBUS_B_0;
            }
            else if(motor_ptr->control.outerloop_target <= POWER_TARGET_1)
            {
                motor_ptr->power.lcorr_power_m = LCORR_VBUS_M_1;
                motor_ptr->power.lcorr_power_b = LCORR_VBUS_B_1;
            }
            else if(motor_ptr->control.outerloop_target <= POWER_TARGET_2)
            {
                motor_ptr->power.lcorr_power_m = LCORR_VBUS_M_2;
                motor_ptr->power.lcorr_power_b = LCORR_VBUS_B_2;
            }
            else if(motor_ptr->control.outerloop_target <= POWER_TARGET_3)
            {
                motor_ptr->power.lcorr_power_m = LCORR_VBUS_M_3;
                motor_ptr->power.lcorr_power_b = LCORR_VBUS_B_3;
            }
            else if(motor_ptr->control.outerloop_target <= POWER_TARGET_4)
            {
                motor_ptr->power.lcorr_power_m = LCORR_VBUS_M_4;
                motor_ptr->power.lcorr_power_b = LCORR_VBUS_B_4;
            }
            else if(motor_ptr->control.outerloop_target <= POWER_TARGET_5)
            {
                motor_ptr->power.lcorr_power_m = LCORR_VBUS_M_5;
                motor_ptr->power.lcorr_power_b = LCORR_VBUS_B_5;
            }
        }
    }
}
#endif

