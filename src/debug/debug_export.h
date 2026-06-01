
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

#ifndef DEBUG_EXPORT_H
#define DEBUG_EXPORT_H

#include "maths.h"

extern fix16_t gui_parameter_command;
extern fix16_t parameter_value1;
extern fix16_t parameter_value2;
extern fix16_t parameter_value3;

extern void export_variables(fix16_t variable_num);
extern void hpdac_lpdaccombined_param_read(fix16_t* value1, fix16_t* value2, fix16_t* value3);
extern void lpdac_hpdac_param_read(fix16_t* value1, fix16_t* value2, fix16_t* value3);

#endif  //#ifndef DEBUG_EXPORT_H
