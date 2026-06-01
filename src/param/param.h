
/**
 * @file param.h
 * @brief Use for parameter processing
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

#ifndef PARAM_H
#define PARAM_H

#include "config_app.h"

#if (PARAM_SELECT == PARAM_DEFAULT)
    #include "param_default.h"
#elif (PARAM_SELECT == PARAM_USER_1)
    #include "param_user_1.h"
#else
    #error "PARAMETER_SELECT has not been defined: aborting compilation."
#endif

#include "param_calc.h"

#endif  // #ifndef PARAM_H
