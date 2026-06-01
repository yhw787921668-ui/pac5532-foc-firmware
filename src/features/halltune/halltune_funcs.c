
/**
* @file halltune_funcs.c
* @brief Functions used for tuning and calibrating hall sensor angle
*/

//=============================================================================
// Copyright (C) 2020, Qorvo, Inc.
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

#include "pac5xxx.h"
#include "config_features.h"
#include "motor_struct.h"
#include "halltune_funcs.h"
#include "app_funcs.h"
#include "foc_config.h"


#if defined(ENABLE_HALL_TUNING)
void halltune_controller(Struct_Motor* motor_ptr)
{
    switch(motor_ptr->halltune.state)
    {
    case STATE_HALLTUNE_RESET:
        // Reset running variables and sub-states
        motor_reset(motor_ptr);
        hall_reset(motor_ptr);
        halltune_reset_vars(motor_ptr);
        halltune_aligngo_reset(motor_ptr);
        aligngo_reset_state(motor_ptr);
#ifdef ENABLE_OPDOL
        reset_opdol(motor_ptr);
#endif
        // Configure control loop for FOC mode
        foc_config(motor_ptr, 1);
        motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &hall_tuning_process;
        motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;

        // Next State
        motor_ptr->halltune.status |= STATUS_HALLTUNE_RUNNING;
        motor_ptr->halltune.state = STATE_HALLTUNE_RUN;
        break;

    case STATE_HALLTUNE_RUN:
        // Run Align&Go controller
        aligngo_controller(motor_ptr);

        if (motor_ptr->mpos.aligngo.state_main == STATE_ALIGNGO_MAIN_RAMPCOMPLETE)
        {
            // Constantly calculate snap angle table with current hall angle values
            hall_tuning_calcangletable(motor_ptr);

            // Update speed ramp and speed for GUI display
            aligngo_update_outerloop_ramp_ol(motor_ptr, motor_ptr->mpos.aligngo.speedrefol_ramp_q26);
            update_outerloop_fdbk_scaled(motor_ptr);

            halltune_check_filter_settled(motor_ptr);

            // Update status
            if (motor_ptr->halltune.status & FAULT_HALLTUNE_DETECTION_FAILURE)
            {
                motor_ptr->halltune.status &= ~STATUS_HALLTUNE_ANGLE_DETECTED;
                motor_ptr->halltune.status |= STATUS_HALLTUNE_DETECTING_ANGLE;
            }
            else
            {
                motor_ptr->halltune.status &= ~STATUS_HALLTUNE_DETECTING_ANGLE;
                motor_ptr->halltune.status |= STATUS_HALLTUNE_ANGLE_DETECTED;
            }
        }

//        debug_var_5 = motor_ptr->halltune.status << 16;
//        debug_var_6 = motor_ptr->halltune.status & 0xFFFF0000;
        break;

    case STATE_HALLTUNE_IDLE:
    default:
        break;
    }
}


PAC5XXX_OPTIMIZE_SPEED_ON
/**
* @brief Processes hall sensor edges to determine relationship to angle during hall tuning
* @param Struct_Motor* motor_ptr
*/
PAC5XXX_RAMFUNC void hall_tuning_process(Struct_Motor *motor_ptr)
{
    int32_t temp, hallstate_angle_tuning_filtered_prev;

    // Check if hall state transition occurred
    hall_check_hall_state_changed(motor_ptr);

    // If a transition occurred then proceed with hall tuning calculations
    if (motor_ptr->mpos.hall.transition_flag)
    {
        // Clear hall state transition flag
        motor_ptr->mpos.hall.transition_flag = FALSE;

        // Increment transition counter for filter settled check
        motor_ptr->halltune.transition_counter++;

        // Store hall tuning values according to motor direction motor direction
        if(motor_ptr->app.app_motor_direction_cmd == 1)
        {
            // Store previous filtered hall angle value for settle detection
            hallstate_angle_tuning_filtered_prev = motor_ptr->halltune.hallstate_angle_tuning[motor_ptr->mpos.hall.SensorStateNew];
            // Filter hall angle
            motor_ptr->halltune.hallstate_angle_tuning[motor_ptr->mpos.hall.SensorStateNew] = filter_shift(motor_ptr->mpos.mpos_angle, &motor_ptr->halltune.hallstate_angle_tuning_filtered_q20[motor_ptr->mpos.hall.SensorStateNew], 4);
            // Calculate filter delta for settle detection
            temp = motor_ptr->halltune.hallstate_angle_tuning[motor_ptr->mpos.hall.SensorStateNew] - hallstate_angle_tuning_filtered_prev;
        }

        // Check if filter delta is above threshold during settle detection interval
        if ((temp > FILTER_DELTA_THRESHOLD) || (temp < -FILTER_DELTA_THRESHOLD))
        {
            motor_ptr->halltune.filter_delta_outofrange = 1;
            motor_ptr->halltune.flag_halltune_filter_settled = 0;
        }

        // Store the last state
        motor_ptr->mpos.hall.SensorStateLast = motor_ptr->mpos.hall.SensorStateNew;

        #if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
        __disable_irq();
        // Re-enable hall port interrupts
        PAC5XXX_GPIO_HALL->INTEN.w = motor_ptr->mpos.hall.hall_int_set;
        __enable_irq();
        #endif
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#if defined(ENABLE_HALL_TUNING)
