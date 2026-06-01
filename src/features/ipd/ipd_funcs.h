
/**
 * @file ipd_funcs.h
 * @brief Use for ipd related utility functions
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

#ifndef IPD_FUNCS_H
#define IPD_FUNCS_H

#include "ipd.h"

/**
 * @brief  Use to read motor phase curents
 * @param Struct_IPD *ipd_ptr
 */
extern void ipd_read_phase_currents(Struct_IPD *ipd_ptr);

/**
 * @brief  Use to set u duty cycle
 * @param uint16_t duty
 */
extern void ipd_m1_u_set_duty(uint16_t duty);

/**
 * @brief  Use to set v duty cycle
 * @param uint16_t duty
 */
extern void ipd_m1_v_set_duty(uint16_t duty);

/**
 * @brief  Use to set w duty cycle
 * @param uint16_t duty
 */
extern void ipd_m1_w_set_duty(uint16_t duty);

/**
 * @brief  Use to call IPD reset function
 * @param None
 */
extern void ipd_reset(void);

/**
 * @brief  Use to call IPD exit function
 * @param None
 */
extern void ipd_exit(void);
#endif
