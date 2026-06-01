
/**
 * @file control_sm.h
 * @brief Use for FOC control state machine
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

#ifndef CONTROL_SM_H
#define CONTROL_SM_H

#include "config_options.h"
#include "board.h"
#include "config_app.h"
#include "motor_struct.h"
#include "mpos_funcs.h"
#include "control_funcs.h"
#include "foc.h"
#include "mod.h"

#ifdef ENABLE_OPD_CL
#include "opd_cl_funcs.h"
#endif

#ifdef ENABLE_OPDOL
#include "opd_ol_funcs.h"
#endif

#ifdef ENABLE_POWERMEASURE
#include "control_power_funcs.h"
#endif

#ifdef ENABLE_AUTOTUNE_PI
#include "atune_funcs.h"
#endif

#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
#include "vbus_funcs.h"
#endif

#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
#include "trap_funcs.h"
#endif

/**
 * @brief This function use for main FOC control state flow
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */

PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void statemachine_control(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_AUTOTUNE_MID)
    if (motor_ptr->atune.mid1_control_mode_flag)
    {
        // Process MID1 control loop operations
        mid1_process_control(motor_ptr);
    }
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use to for main FOC critical control
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void process_motor_control(Struct_Motor* motor_ptr)
{
#ifdef ENABLE_POWERMEASURE
    // Measure power
    power_calculator(motor_ptr);
#endif

    // Run motor state machine
    statemachine_control(motor_ptr);

    // Run FOC algorithm sequence
    process_foc(motor_ptr);

    // Modulation
    process_modulation(motor_ptr);

#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN) && defined(ENABLE_BEMF_SAMPLE_SINGLE_PHASE)
    trap_config_seq(motor_ptr->control.mod_sector_sreg);
#endif

#ifdef ENABLE_OPD_CL
    integrate_and_log_phase_current(motor_ptr);
#endif

#if defined(ENABLE_STALL_DETECT)
    calc_vdrivepeak(motor_ptr);
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef CONTROL_SM_H
