
/**
 * @file sim_h_funcs_ext.h
 * @brief Use to set Hall SIM flags
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

#ifndef SIM_H_FUNCS_EXT_H
#define SIM_H_FUNCS_EXT_H

#include "motor_struct.h"
//#include "sim_h_struct.h"

#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_HALL)

/**
 * @brief This function use to set SIM hall initialized flag
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_hallport_intflag_set(Struct_Motor *motor_ptr)
{
    __disable_irq();
    motor_ptr->sim.flag_hall_state_int = TRUE;
    __enable_irq();
}


/**
 * @brief This function use to set hall timer initialized flag
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_halltimer_intflag_set(Struct_Motor *motor_ptr)
{
    // Set hall timer interrupt flag
    motor_ptr->sim.flag_hall_timer_int = TRUE;
}

#endif
#endif  //#ifndef SIM_H_FUNCS_EXT_H
