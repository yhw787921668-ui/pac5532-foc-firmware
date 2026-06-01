/**
 * @file speed_check.c
 * @brief Use for check speed min - max limit and generate speed fault
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

#include "speed_check.h"
#include "fault.h"
#include "utils.h"

#ifdef ENABLE_CLSPEED_MIN_MAX_CHECK
void check_speedlimitcl_fault(Struct_Motor* motor_ptr)
{
    static unsigned long int speed_limit_fault_detection_last_sample_time_value = 0;
    static unsigned char SpeedErrorCount = 0;

    // Check for overflow
    if (motor_ptr->app.systick_div_counter < speed_limit_fault_detection_last_sample_time_value)
        speed_limit_fault_detection_last_sample_time_value = motor_ptr->app.systick_div_counter;

    if ((motor_ptr->app.systick_div_counter - speed_limit_fault_detection_last_sample_time_value) >= SPEED_MIN_MAX_FAULT_CHECK_SAMPLE_TIME)
    {
        speed_limit_fault_detection_last_sample_time_value = motor_ptr->app.systick_div_counter;

        //Check in close loop
#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
        if (((motor_ptr->state.main == STATE_MAIN_RUN) || (motor_ptr->state.main == STATE_MAIN_COASTING)) && (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST))
#else
        if ((motor_ptr->state.main == STATE_MAIN_RUN) || (motor_ptr->state.main == STATE_MAIN_COASTING))
#endif
        {
            if (((motor_ptr->mpos.mpos_spd_radps * motor_ptr->app.app_motor_direction) < motor_ptr->app.speedrefcl_min) ||
                ((motor_ptr->mpos.mpos_spd_radps * motor_ptr->app.app_motor_direction) > motor_ptr->app.speedrefcl_max))
            {
                SpeedErrorCount++;

                if (SpeedErrorCount >= MOTOR_SPEED_ERRORP_CHECK_THRESHOLD)
                {
                    SpeedErrorCount = 0;
                    set_fault(FAULT_MIN_MAX_SPEED);
                }
            }
            else
            {
                SpeedErrorCount = 0;
            }
        }
        else
        {
            SpeedErrorCount = 0;
        }
    }
}
#endif
