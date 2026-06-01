
/**
 * @file qep_qep2est.h
 * @brief Use for qep FOC utility functions
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

#ifndef QEP_QEP2EST_H
#define QEP_QEP2EST_H

#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"

#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
#include "sim_funcs.h"
#include "qep_funcs.h"
#include "driver_funcs.h"
#include "anglediff.h"
#include "fault.h"


/**
 * @brief Use for switching between qep to est and est to qep transition
 * @param Struct_Motor* motor_ptr
 */
static inline void qep_qep2est_controller(Struct_Motor *motor_ptr)
{
    switch(motor_ptr->mpos.mposmode)
    {
    case STATE_MPOSMODE_QEP:
        if (absval(motor_ptr->mpos.mpos_spd_radps) > motor_ptr->mpos.qep.switchover_spd_lo2hi_radps)
        {
#if defined(ENABLE_ANGLEDIFF_CHECK_QEP2EST)
            // AngleDiff and SpeedDiff Check for CL Switchover
            anglediff_check(motor_ptr, motor_ptr->mpos.mpos_angle, motor_ptr->mpos.est.estimated_angle, motor_ptr->mpos.mpos_spd_radps, motor_ptr->mpos.est.est_spd_radps);

            if (motor_ptr->mpos.anglediff_pass_flag)
            {
                motor_ptr->mpos.anglediff_pass_flag = 0;
                motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
                motor_ptr->mpos.qep.qep2est_angleblend_cnt = QEP2EST_ANGLEBLEND_TOTALCNT;
                __disable_irq();
                motor_ptr->mpos.state = STATE_MPOS_QEP2EST;
                motor_ptr->mpos.mposmode = STATE_MPOSMODE_QEP2EST;
                __enable_irq();
                motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_QEP;
                motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_EST;
            }
#else
            motor_ptr->mpos.qep.qep2est_angleblend_cnt = QEP2EST_ANGLEBLEND_TOTALCNT;
            __disable_irq();
            motor_ptr->mpos.state = STATE_MPOS_QEP2EST;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_QEP2EST;
            __enable_irq();
            motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_QEP;
            motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_EST;
#endif
            debugio1_on();
        }
        else
        {
            clear_warning(WARNING_ANGLDIF);
            clear_warning(WARNING_SPDDIF);
            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_EST:
        if (absval(motor_ptr->mpos.mpos_spd_radps) < motor_ptr->mpos.qep.switchover_spd_hi2lo_radps)
        {
#if defined(ENABLE_ANGLEDIFF_CHECK_QEP2EST)
            // AngleDiff and SpeedDiff Check for CL Switchover
            anglediff_check(motor_ptr, motor_ptr->mpos.mpos_angle, motor_ptr->mpos.qep.qep_angle, motor_ptr->mpos.mpos_spd_radps, motor_ptr->mpos.qep.qep_spd_radps);

            if (motor_ptr->mpos.anglediff_pass_flag)
            {
                motor_ptr->mpos.anglediff_pass_flag = 0;
                motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
                motor_ptr->mpos.qep.qep2est_angleblend_cnt = QEP2EST_ANGLEBLEND_TOTALCNT;
                __disable_irq();
                motor_ptr->mpos.state = STATE_MPOS_EST2QEP;
                motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST2QEP;
                __enable_irq();
                motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_EST;
                motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_QEP;
            }
#else
            motor_ptr->mpos.qep.qep2est_angleblend_cnt = QEP2EST_ANGLEBLEND_TOTALCNT;
            __disable_irq();
            motor_ptr->mpos.state = STATE_MPOS_EST2QEP;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST2QEP;
            __enable_irq();
            motor_ptr->status.motor &= ~STATUS_MOTOR_MPOSMODE_EST;
            motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_QEP;
#endif
            debugio1_on();
        }
        else
        {
            clear_warning(WARNING_ANGLDIF);
            clear_warning(WARNING_SPDDIF);
            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_QEP2EST:
        if (motor_ptr->mpos.qep.qep2est_angleblend_cnt <= 0)
        {
            motor_ptr->mpos.qep.qep2est_angleblend_cnt = QEP2EST_ANGLEBLEND_TOTALCNT;
            __disable_irq();
            motor_ptr->mpos.state = STATE_MPOS_ESTIMATED_ANGLE;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_EST;
            __enable_irq();
            debugio1_off();
        }
        break;

    case STATE_MPOSMODE_EST2QEP:
        if (motor_ptr->mpos.qep.qep2est_angleblend_cnt <= 0)
        {
            motor_ptr->mpos.qep.qep2est_angleblend_cnt = QEP2EST_ANGLEBLEND_TOTALCNT;
            __disable_irq();
            motor_ptr->mpos.state = STATE_MPOS_QEP;
            motor_ptr->mpos.mposmode = STATE_MPOSMODE_QEP;
            __enable_irq();
            debugio1_off();
        }
        break;
    }
}
#endif

#endif  //#ifndef QEP_QEP2EST_H
