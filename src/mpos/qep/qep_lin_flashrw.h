
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

#ifndef QEP_LIN_FLASHRW_H
#define QEP_LIN_FLASHRW_H


#include "pac5xxx.h"
#include "config_app.h"
#include "config_features.h"

#if defined(ENABLE_QEP_LIN_FLASHRW)

// Function Definitions
extern void qep_lin_flashrw_param_process(ParameterCode param_code, int32_t rx_data);

#endif  //#if defined(ENABLE_FLASHDEMO)

#endif  //#ifndef QEP_LIN_FLASHRW_H

