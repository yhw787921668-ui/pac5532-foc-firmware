/**
 * @file opd_ol_funcs.h
 * @brief Use for defining control functions in OPD
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

#ifndef OPD_OL_FUNCS_H
#define OPD_OL_FUNCS_H

#include "opd_ol.h"
#include "motor_struct.h"

/**
 * @brief Use for the control-level state machine in open-loop OPD
 * @param Struct_Motor *motor_ptr
 */
extern PAC5XXX_RAMFUNC void statemachine_opdol_ctl(Struct_Motor* motor_ptr);

/**
 * @brief Use for the main state machine in open-loop OPD
 * @param Struct_Motor *motor_ptr
 */
extern void statemachine_opdol_main(Struct_Motor* motor_ptr);

/**
 * @brief Initialize the open-loop OPD
 * @param Struct_Motor *motor_ptr
 */
extern void init_opdol(Struct_Motor* motor_ptr);

/**
 * @brief Reset the open-loop OPD
 * @param Struct_Motor *motor_ptr
 */
extern void reset_opdol(Struct_Motor* motor_ptr);

#endif
