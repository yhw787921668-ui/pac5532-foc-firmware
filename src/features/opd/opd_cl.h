/**
 * @file opd_cl.h
 * @brief Use to process control functions in OPD
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

#ifndef OPDCL_H
#define OPDCL_H

#include "opd_cl_struct.h"

/**
 * @brief Use to process the regular OPD function calls
 * @param Struct_OPDCL* opdcl_ptr
 * @param int32_t dutysat_flag
 */
extern uint8_t process_opdcl(Struct_OPDCL* opdcl_ptr, int32_t dutysat_flag);

/**
 * @brief Use to process the fast OPD algorithm
 * @param Struct_OPDCL* opdcl_ptr
 * @param uint8_t bit_shift
 */
extern uint8_t process_opdcl_fast(Struct_OPDCL* opdcl_ptr, uint8_t bit_shift);

#endif


