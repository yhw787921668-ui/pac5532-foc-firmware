/**
 * @file opd_cl_config.h
 * @brief Configuration for closed-loop open phase detection
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

#ifndef OPDCL_CONFIG_H
#define OPDCL_CONFIG_H

#define OPDCL_COUNT_THRESHOLD                       5
#define OPDCL_THRESHOLD_PCT                         Q3_0P750
#define OPDCL_CURRENT_THRESHOLD_Q14                 QCONST(0.05f, Q14)
#if defined(ENABLE_OPD_CL_FAST)
    #define OPDCL_SYSTEMTICK_CONST                  100
    #define speed_filter_cofficient                 QCONST(0.01f, Q10)
    #define torque_filter_cofficient                QCONST(0.01f, Q10)
    #define PHASE_THRESHOLD_BIT_SHIFT               6 // 2exp6= 64, which means phase current threshold is 1/64 of the index current
    // Multiply "OPDCL_CURRENT_THRESHOLD_Q14" by "OPD_MIN_TORQUE_SCALE" as the minimum torque for OPD detection
    #define OPD_MIN_TORQUE_SCALE                    QCONST(2, Q00)
    #define OPD_LARGE_INERTIA                       0
#endif
#endif
