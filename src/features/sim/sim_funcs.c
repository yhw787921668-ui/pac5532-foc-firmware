
/**
 * @file sim_funcs.c
 * @brief Use for SIM sequencer and state initialization
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
#include "sim_funcs.h"
#include "control_loop.h"

#if defined(ENABLE_SIM)

/**
 * @brief This function use to reset SIM state, braking and state variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_smstate_reset(Struct_Motor *motor_ptr)
{
    // Reset SIM status register and state machine
    motor_ptr->sim.status_sim = 0;
    motor_ptr->sim.braking_timeout_counter = 0;
    motor_ptr->sim.state_sim = STATE_SIM_RESET;
}

#endif //#if defined(ENABLE_SIM)
