
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

#ifndef TM_H
#define TM_H

#include "motor_struct.h"

#if defined(ENABLE_TM) && defined(ENABLE_CLSPEED_LIMIT_CHANGE_CHECK)
extern void check_speedlimitcl_need_to_change(Struct_Motor* motor_ptr);
#endif

#if defined(ENABLE_TM) && defined(TM_PARAMCHECK)
#include "param_funcs.h"

extern void parameter_check(ParameterCode param_code, fix16_t rx_data, Struct_Motor* motor_ptr);
#endif

#if defined(TM_PARAMCHECK) && (!defined(ENABLE_DEBUG_DISPLAY) || !defined(ENABLE_DEBUG_INPUT))
    #error "TM_PARAMCHECK requires ENABLE_DEBUG_DISPLAY and ENABLE_DEBUG_INPUT options."
#endif

#endif  //#ifndef TM_H
