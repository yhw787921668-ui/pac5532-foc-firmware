
/**
 * @file satmon.h
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

#ifndef PWMSAT_CHECK_H
#define PWMSAT_CHECK_H

#include "motor_struct.h"


//////////////////////////////////
// Function Declarations
//////////////////////////////////

/**
 * @brief  Use to check is PWM saturated
 * @param Struct_Motor* motor_ptr
 */
extern void check_pwm_sat(Struct_Motor* motor_ptr);


//////////////////////////////////
// Inline Functions
//////////////////////////////////

/**
 * @brief  Use to check is Iq PI saturated and act on it
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void pi_sat_mon(Struct_Motor* motor_ptr)
{
    // If not saturated. Note: vq_q14 is used instead of Iq_pid.PI_sat if dq decoupling is defined
#ifdef ENABLE_DQ_DECOUPLING
    if ((motor_ptr->foc.vq_q14 * motor_ptr->app.app_motor_direction) < motor_ptr->foc.Iq_pid.max_value)
#else
    if ((motor_ptr->foc.Iq_pid.PI_sat * motor_ptr->app.app_motor_direction) < motor_ptr->foc.Iq_pid.max_value)
#endif
    {
        motor_ptr->foc.flag_sat_logicand = 0;
#if defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == INCREMENTRAL_CONTROL)
        motor_ptr->fw.flag_sat_logicand = 0;
#endif
//        debugio1_off();
    }
    // If saturated
    else
    {
        motor_ptr->foc.flag_sat_logicor = 1;
#if defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == INCREMENTRAL_CONTROL)
        motor_ptr->fw.flag_sat_logicor = 1;
#endif
//        debugio1_on();
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting

#endif  //#ifndef PWMSAT_CHECK_H
