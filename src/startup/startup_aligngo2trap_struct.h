
/**
 * @file startup_alingo2trap_struct.h
 * @brief Use for startup_aligngo2est enums and structure
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

#ifndef STARTUP_ALIGNGO2EST_STRUCT_H
#define STARTUP_ALIGNGO2EST_STRUCT_H

#include "config_app.h"
#include "aligngo_struct.h"


/**
 * @brief eStateStartupAlignGo2Trap enum
 */
typedef enum
{
    STATE_STARTUP_ALIGNGO_RESET = 0,                // 0: Align&Go Startup Controller - Reset
    STATE_STARTUP_ALIGNGO_RUN,                      // 1: Align&Go Startup Controller - Run State
    STATE_STARTUP_ALIGNGO_ANGLEDIFF,                // 2: Align&Go Startup Controller - AngleDiff Reduction State
    STATE_STARTUP_ALIGNGO_COMPLETE                  // 3: Align&Go Startup Controller - Complete State
}eStateStartupAlignGo2Trap;

/**
 * @brief eStateStartupAlignGo struct
 */
typedef struct
{
    // State & Status
    eStateStartupAlignGo2Trap state;                     // Align&Go Startup Controller state

    // Sub-Structure
    Struct_AlignGoCMD aligngo;

    // Switch over Blending
    int32_t aligngo2est_speedblend_cnt;
    int32_t aligngo2est_angleblend_cnt;
    int32_t aligngo2est_blend_total_cnt;
    fix16_t aligngo2est_blend_total_cnt_1div_q12;
    int32_t angle_wrap_flag;
#if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
    int32_t aligngo2est_angleramp_q16;
    int32_t aligngo2est_angleramp_inc_radpdt_q16;
#endif
}Struct_StartupAlignGo2Trap;

#endif //#ifndef STARTUP_ALIGNGO2EST_STRUCT_H
