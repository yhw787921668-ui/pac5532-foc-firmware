
/**
 * @file trap_trap2est.h
 * @brief Use for trap FOC utility functions
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

#ifndef TRAP_TRAP2EST_H
#define TRAP_TRAP2EST_H

#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"

#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
#include "sim_funcs.h"
#include "trap_funcs.h"
#include "driver_funcs.h"
#include "anglediff.h"
#include "fault.h"

#define TRAP2EST_ANGLEBLEND_TOTALCNT        8
#define TRAP2EST_ANGLEBLEND_FACTOR_Q12      QCONST((1.0 / (float)TRAP2EST_ANGLEBLEND_TOTALCNT), Q12)
#define TRAP2EST_ANGLE_DIFF_MAX             FIX16_PI_DIV_6

#define EST2TRAP_DELAY_TOTALCNT             200

/**
 * @brief Use for calculating angle diff between Trap mode and estimator mode, which helps to achieve smooth swithching over
 * @param Struct_Motor* motor_ptr
 */
static inline void calc_angle_diff_trap_lbg1(Struct_Motor *motor_ptr)
{
    int32_t temp_q16;
    temp_q16 = motor_ptr->mpos.trap.trap_interpolated_angle - motor_ptr->mpos.est.estimated_angle;
    wraparound(&temp_q16);
    // Filter angle diff
    filter_shift2(temp_q16, &motor_ptr->mpos.trap.trap_est_angle_diff, 3);
    // Limit the angle diff
    if (motor_ptr->mpos.trap.trap_est_angle_diff > TRAP2EST_ANGLE_DIFF_MAX)
    motor_ptr->mpos.trap.trap_est_angle_diff = TRAP2EST_ANGLE_DIFF_MAX;
    else if (motor_ptr->mpos.trap.trap_est_angle_diff < -TRAP2EST_ANGLE_DIFF_MAX)
    motor_ptr->mpos.trap.trap_est_angle_diff = -TRAP2EST_ANGLE_DIFF_MAX;
}

/**
 * @brief Use for switching between trap to est and est to trap transition
 * @param Struct_Motor* motor_ptr
 */
static inline void trap_trap2est_controller(Struct_Motor *motor_ptr)
{
    switch(motor_ptr->mpos.mposmode)
    {
    case STATE_MPOSMODE_TRAP:
    // To do: Somehow the speed from trap is not correct in some cases
        if (absval(motor_ptr->mpos.est.est_spd_radps) > motor_ptr->mpos.trap.switchover_spd_lo2hi_radps)
        {
            // AngleDiff and SpeedDiff Check for CL Switchover
// Note: Bypassing anglediff for switchover straight into a blend. Need to see if this is asequate or if there are issues when switching with a large anglediff and if we need to do some autoranging.
//            anglediff_check(motor_ptr, motor_ptr->mpos.mpos_angle, motor_ptr->mpos.est.estimated_angle, motor_ptr->mpos.mpos_spd_radps, motor_ptr->mpos.est.est_spd_radps);

            // To do: Angle diff pass flag needs to be simplified
            // if (motor_ptr->mpos.anglediff_pass_flag)
            {
                #if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
                // Update outerloop ramp value with switchover speed
                motor_ptr->control.outerloop_ramp = motor_ptr->mpos.mpos_spd_radps;
                #endif
                motor_ptr->mpos.anglediff_pass_flag = 1;
                motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
                motor_ptr->mpos.trap.trap2est_angleblend_cnt = TRAP2EST_ANGLEBLEND_TOTALCNT;
                motor_ptr->mpos.state = STATE_MPOS_TRAP2EST;
                motor_ptr->mpos.mposmode = STATE_MPOSMODE_TRAP2EST;
                motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_TRAP;
                motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_EST;
            }
//            debugio1_on();
        }
        break;

    case STATE_MPOSMODE_EST:
        if (absval(motor_ptr->mpos.est.est_spd_radps) < motor_ptr->mpos.trap.switchover_spd_hi2lo_radps)
        {
            // AngleDiff and SpeedDiff Check for CL Switchover
//            anglediff_check(motor_ptr, motor_ptr->mpos.mpos_angle, motor_ptr->mpos.trap.trap_interpolated_angle, motor_ptr->mpos.mpos_spd_radps, motor_ptr->mpos.trap.trap_spd_radps);

//            if (motor_ptr->mpos.anglediff_pass_flag)
            {
                motor_ptr->mpos.anglediff_pass_flag = 1;
                motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
                motor_ptr->mpos.trap.trap2est_angleblend_cnt = TRAP2EST_ANGLEBLEND_TOTALCNT;
                motor_ptr->mpos.state = STATE_MPOS_EST2TRAP;
                motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST2TRAP;
                motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_EST;
                motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_TRAP;
            }
//            debugio1_on();
        }
        break;

    case STATE_MPOSMODE_TRAP2EST:
        if (motor_ptr->mpos.trap.trap2est_angleblend_cnt <= 0)
        {
            motor_ptr->mpos.trap.trap2est_angleblend_cnt = TRAP2EST_ANGLEBLEND_TOTALCNT;
            motor_ptr->mpos.state = STATE_MPOS_ESTIMATED_ANGLE;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST;
//            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_EST2TRAP:
        motor_ptr->mpos.trap.est2trap_delay_cnt = EST2TRAP_DELAY_TOTALCNT;
        motor_ptr->mpos.state = STATE_MPOS_TRAP;
        motor_ptr->mpos.mposmode = STATE_MPOSMODE_TRAP;
        pid_reset(&motor_ptr->foc.Id_pid);                                // Reset PID controller data
//            debugio1_off();
        break;
    }
}
#endif

#endif  //#ifndef TRAP_TRAP2EST_H
