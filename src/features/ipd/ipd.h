
/**
 * @file ipd.h
 * @brief Use for ipd basic functions
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

#ifndef IPD_H
#define IPD_H

#include "ipd_struct.h"

/**
 * @brief IPD_STATUS enum
 */
typedef enum {
    IPD_SUCCESS             = 0,
    IPD_FAILED              = 1 << 0,
    IPD_PHASE_LOST_DETECTED = 1 << 1,
    IPD_INSUFFICENT_CURRENT_DELTA = 1 << 2
} IPD_STATUS;

/**
 * @brief IPD_STATE enum
 */
typedef enum {
    IPD_START,
    IPD_WAIT_TIMER_SYNC,
    IPD_WAIT_TIMER_DUTY_CYCLE_SYNC,
    IPD_MEASURE_CURRENTS,
    IPD_DONE
} IPD_STATE;

/**
 * @brief Use run ipd state machine
 * @param Struct_IPD *ipd_ptr
 * @param void *motor_ptr 
 */
extern void ipd_state_machine(Struct_IPD *ipd_ptr, void *motor_ptr);

/**
 * @brief Use for ipd state machine function
 * @param Struct_IPD *ipd_ptr
 * @param void *motor_ptr
 */
extern void (*ipd_state_machine_ptr)(Struct_IPD *ipd_ptr, void *motor_ptr);

#endif
