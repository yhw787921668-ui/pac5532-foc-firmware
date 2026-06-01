
/**
 * @file forceang_struct.h
 * @brief Forced-angle mode Structures
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

#ifndef FORCEANG_STRUCT_H
#define FORCEANG_STRUCT_H

#include "aligngo_struct.h"

//////////////////////////////////////////
// Enum Definitions
//////////////////////////////////////////

typedef enum
{
    STATE_FORCEANG_RESET = 0,                   // 0:
    STATE_FORCEANG_ALIGNGORESET,                // 1:
    STATE_FORCEANG_RUN,                         // 2:
    STATE_FORCEANG_END = STATE_FORCEANG_RUN     // Use to check the range of enumeration
}eStateForceAng;


//////////////////////////////////////////
// Structure Definitions
//////////////////////////////////////////

typedef struct
{
    // State & Status
    int32_t forceang_enable_cmd;
    int32_t forceang_enabled_flag;
    eStateForceAng forceang_state;

    // Sub-Structure
    Struct_AlignGoCMD aligngo;
}Struct_ForceAng;

#endif  //#ifndef FORCEANG_STRUCT_H
