
/**
* @file sim_struct.h
* @brief Includes the structure definition for selected SIM method
*/

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef SIM_STRUCT_H
#define SIM_STRUCT_H

#include "pac5xxx.h"
#include "maths.h"
#include "config_app.h"
#include "config_features.h"


#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE))
#include "sim_v_struct.h"
#elif (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
#include "sim_i_struct.h"
#elif (SEL_SIM_METHOD == SIM_METHOD_HALL)
#include "sim_h_struct.h"
#elif (SEL_SIM_METHOD == SIM_METHOD_QEP)
#include "sim_q_struct.h"
#endif

#endif  //#ifndef SIM_STRUCT_H
