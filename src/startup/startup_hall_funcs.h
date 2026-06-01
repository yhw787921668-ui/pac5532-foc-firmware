
/**
 * @file startup_hall_funcs.h
 * @brief Use for hall startup functions
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

#ifndef STARTUP_HALL_FUNCSH
#define STARTUP_HALL_FUNCSH

#include "pac5xxx.h"
#include "motor_struct.h"
#include "param_funcs.h"
#include "hall_funcs.h"
#include "control_funcs.h"
#include "sim_funcs.h"
#include "utils.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mpos_config.h"

extern int Debug_Test_Number2; //by:owz

#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
#include "lbg1_funcs.h"
#endif


#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)
/**
* @brief Use to check for startup complete
* @param Struct_Motor* motor_ptr
* @return true or false
*/
static inline int32_t startup_complete(Struct_Motor* motor_ptr)
{
    return (motor_ptr->startup.state_main == STATE_HALLSTART_MAIN_COMPLETE);
}



/**
* @brief Use to reset startup variables
* @param Struct_Motor* motor_ptr
*/
static inline void startup_reset_vars(Struct_Motor* motor_ptr)
{
    // Update target currents
    motor_ptr->startup.idrefstart_target_q16 = 0;
    motor_ptr->startup.iqrefstart_target_q16 = motor_ptr->startup.i_total_hall_gui * motor_ptr->app.app_motor_direction;

    // Set control variables
    motor_ptr->control.idiqref_rate_f16 = motor_ptr->control.idiqref_rate_gui_f16;
    motor_ptr->control.idref_target_f16 = motor_ptr->startup.idrefstart_target_q16;
    motor_ptr->control.iqref_target_f16 = motor_ptr->startup.iqrefstart_target_q16;
}


/**
* @brief Use to set startup state to reset
* @param Struct_Motor* motor_ptr
*/
static inline void startup_reset_state(Struct_Motor* motor_ptr)
{
    motor_ptr->startup.state_main = STATE_HALLSTART_MAIN_RESET;
}


/**
* @brief Use for startup controller
* @param Struct_Motor* motor_ptr
*/
static inline void startup_controller(Struct_Motor* motor_ptr)
{
    switch(motor_ptr->startup.state_main)
    {
        case STATE_HALLSTART_MAIN_RESET:
            // Reset running variables
            control_reset(motor_ptr);
            startup_reset_vars(motor_ptr);
            hall_reset(motor_ptr);
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
            lbg1_reset(motor_ptr);
            lbg1_switch2cl(&motor_ptr->mpos.est);
#endif
#ifdef ENABLE_OPDOL
            reset_opdol(motor_ptr);
#endif
            // Configure control loop for FOC mode
            foc_config(motor_ptr, 1);
            mpos_config_fptr(motor_ptr);

            // Next state
            __disable_irq();
            motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_HALL;
            motor_ptr->startup.state_main = STATE_HALLSTART_MAIN_START;
            motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
            motor_ptr->mpos.state = STATE_MPOS_HALL_ANGLE_INTERPOLATE;
            motor_ptr->state.control = STATE_CONTROL_TORQUE;
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_HALL;
#endif
            m1_enable();
            __enable_irq();
            break;

        case STATE_HALLSTART_MAIN_START:
            // Complete startup if hall speed calculation is enabled (after first transition)
            if (motor_ptr->mpos.hall.flag_speed_calc_enabled)
            {
                if (motor_ptr->app.speed_mode_flag)
                {
                    // Initialize speed ramp
                    motor_ptr->control.outerloop_ramp = motor_ptr->mpos.hall.hall_spd_radps;
                    motor_ptr->state.control = STATE_CONTROL_OUTERLOOP;

                    // Initialize outer loop
                    init_pi(&motor_ptr->control.outerloop_pid, motor_ptr->foc.iqref_ramp_q14);
                }
                else
                {
                    motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16;
                    motor_ptr->state.control = STATE_CONTROL_TORQUE;
                }

                // Set final parameters
                motor_ptr->control.idref_target_f16 = motor_ptr->app.idrefcl_cmd_f16;
                // Next state
                motor_ptr->startup.state_main = STATE_HALLSTART_MAIN_COMPLETE;
            }
            break;

        case STATE_HALLSTART_MAIN_COMPLETE:
            break;

        default:
            break;
    }
}

#endif  //#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)

#endif  //#ifndef STARTUP_HALL_FUNCSH
