
/**
 * @file hall_sim2hall.h
 * @brief Use for hall port and interrupt initialization and utility function
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

#ifndef HALL_SIM2HALL_H
#define HALL_SIM2HALL_H


#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"


#if (MODULE_MPOS_HALL == ENABLED) && defined(ENABLE_SIM)

/**
 * @brief Use to set state of hall state machine to RUN
 * @param Struct_Motor* motor_ptr
 */
static inline void hall_sim2hall_main(Struct_Motor* motor_ptr)
{
    motor_ptr->sim.transition_complete = TRUE;
}


/**
 * @brief Use to switch form SIM to Hall transition
 * @param Struct_Motor* motor_ptr
 */
static inline void hall_sim2hall_ctrl(Struct_Motor* motor_ptr)
{
}

#endif  //#if (MODULE_MPOS_HALL == ENABLED)
#endif  //#ifndef HALL_SIM2HALL_H
