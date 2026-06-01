
/**
 * @file hall_hall2est.h
 * @brief Use for hall FOC utility functions
 */

//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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

#ifndef HALL_HALL2EST_H
#define HALL_HALL2EST_H

#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"

#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
#include "sim_funcs.h"
#include "hall_funcs.h"
#include "driver_funcs.h"
#include "anglediff.h"


/**
 * @brief Use for switching between hall to est and est to hall transition
 * @param Struct_Motor* motor_ptr
 */
static inline void hall_hall2est_controller(Struct_Motor *motor_ptr)
{
    switch(motor_ptr->mpos.mposmode)
    {
    case STATE_MPOSMODE_HALL:
        if (absval(motor_ptr->mpos.mpos_spd_radps) > motor_ptr->mpos.hall.switchover_spd_lo2hi_radps)
        {
            // AngleDiff and SpeedDiff Check for CL Switchover
            anglediff_check(motor_ptr, motor_ptr->mpos.mpos_angle, motor_ptr->mpos.est.estimated_angle, motor_ptr->mpos.mpos_spd_radps, motor_ptr->mpos.est.est_spd_radps);

            if (motor_ptr->mpos.anglediff_pass_flag)
            {
                motor_ptr->mpos.anglediff_pass_flag = 0;
                motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
                motor_ptr->mpos.hall.hall2est_angleblend_cnt = HALL2EST_ANGLEBLEND_TOTALCNT;
                motor_ptr->mpos.state = STATE_MPOS_HALL2EST;
                motor_ptr->mpos.mposmode = STATE_MPOSMODE_HALL2EST;
                motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_HALL;
                motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_EST;
            }
//            debugio1_on();
        }
        else
        {
            clear_warning(WARNING_ANGLDIF);
            clear_warning(WARNING_SPDDIF);
//            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_EST:
        if (absval(motor_ptr->mpos.mpos_spd_radps) < motor_ptr->mpos.hall.switchover_spd_hi2lo_radps)
        {
            // AngleDiff and SpeedDiff Check for CL Switchover
            anglediff_check(motor_ptr, motor_ptr->mpos.mpos_angle, motor_ptr->mpos.hall.hall_interpolated_angle, motor_ptr->mpos.mpos_spd_radps, motor_ptr->mpos.hall.hall_spd_radps);

            if (motor_ptr->mpos.anglediff_pass_flag)
            {
                motor_ptr->mpos.anglediff_pass_flag = 0;
                motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
                motor_ptr->mpos.hall.hall2est_angleblend_cnt = HALL2EST_ANGLEBLEND_TOTALCNT;
                motor_ptr->mpos.state = STATE_MPOS_EST2HALL;
                motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST2HALL;
                motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_EST;
                motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_HALL;
            }
//            debugio1_on();
        }
        else
        {
            clear_warning(WARNING_ANGLDIF);
            clear_warning(WARNING_SPDDIF);
//            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_HALL2EST:
        if (motor_ptr->mpos.hall.hall2est_angleblend_cnt <= 0)
        {
            motor_ptr->mpos.hall.hall2est_angleblend_cnt = HALL2EST_ANGLEBLEND_TOTALCNT;
            motor_ptr->mpos.state = STATE_MPOS_ESTIMATED_ANGLE;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST;
//            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_EST2HALL:
        if (motor_ptr->mpos.hall.hall2est_angleblend_cnt <= 0)
        {
            motor_ptr->mpos.hall.hall2est_angleblend_cnt = HALL2EST_ANGLEBLEND_TOTALCNT;
            motor_ptr->mpos.state = STATE_MPOS_HALL_ANGLE_INTERPOLATE;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_HALL;
//            debugio1_off();
        }
        break;
    }
}
#endif

#endif  //#ifndef HALL_HALL2EST_H
