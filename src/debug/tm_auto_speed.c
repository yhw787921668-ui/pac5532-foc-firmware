
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

#include "motor_struct.h"
#include "tm.h"

#if defined(ENABLE_TM) && defined(ENABLE_CLSPEED_LIMIT_CHANGE_CHECK)
void check_speedlimitcl_need_to_change(Struct_Motor* motor_ptr UNUSED_PARAM)
{
    static unsigned long int speed_limit_need_to_change_last_sample_time_value = 0;
    static unsigned char speed_limit_switch_counter = 0;

    // Check for overflow
    if (motor_ptr->app.systick_div_counter < speed_limit_need_to_change_last_sample_time_value)
        speed_limit_need_to_change_last_sample_time_value = motor_ptr->app.systick_div_counter;

    if ((motor_ptr->app.systick_div_counter - speed_limit_need_to_change_last_sample_time_value) >= SPEED_CL_LIMIT_CHANGE_CALL_BACK_TIME)
    {
        speed_limit_need_to_change_last_sample_time_value = motor_ptr->app.systick_div_counter;

        if (motor_ptr->debug.cl_speed_test_enable)
        {
            //Check in close loop
            if ((motor_ptr->state.main == STATE_MAIN_RUN) && motor_ptr->app.speed_mode_flag)
            {
                __disable_irq();
                if (speed_limit_switch_counter <= CL_SPEED_LIMIT_TEST_COUNTER_FOR_SPEED1)
                    motor_ptr->control.outerloop_target = motor_ptr->debug.cl_speed_test_limit1;
                else
                    motor_ptr->control.outerloop_target = motor_ptr->debug.cl_speed_test_limit2;
                __enable_irq();

                if (speed_limit_switch_counter++ > CL_SPEED_LIMIT_TEST_COUNTER_MAX_VALUE)
                    speed_limit_switch_counter = 0;
            }
            else
            {
                if (motor_ptr->app.speed_mode_flag)
                    motor_ptr->control.outerloop_target = motor_ptr->debug.cl_speed_test_limit1;
                speed_limit_switch_counter = 0;
            }
        }
        else
        {
            motor_ptr->control.outerloop_target = motor_ptr->debug.cl_speed_backup;
            speed_limit_switch_counter = 0;
        }
    }
}
#endif
