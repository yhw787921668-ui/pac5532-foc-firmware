
/**
 * @file mtpa_funcs.h
 * @brief Use for FOC MTPA function
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

#ifndef MTPA_FUNCS_H
#define MTPA_FUNCS_H

#include "motor_struct.h"
#include "param_funcs.h"

extern void mtpa_init(Struct_Motor* motor_ptr);
extern void mtpa_reset(Struct_Motor* motor_ptr);
extern void mtpa_gain_calc(Struct_Motor* motor_ptr);
extern void mtpa_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
extern void mtpa_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
#endif
