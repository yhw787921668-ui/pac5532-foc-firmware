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

#ifndef STARTUP_ALIGNGO2QEP_STRUCT_H
#define STARTUP_ALIGNGO2QEP_STRUCT_H

#include "aligngo_struct.h"


typedef enum
{
    STATE_STARTUP_ALIGNGO2QEP_RESET = 0,                // 0: Startup State Align&Go2QEP - Reset
    STATE_STARTUP_ALIGNGO2QEP_ALIGNGO,                  // 1: Startup State Align&Go2QEP - Align&Go
    STATE_STARTUP_ALIGNGO2QEP_TRANSITION,               // 2: Startup State Align&Go2QEP - Switch from Align&Go to QEP
    STATE_STARTUP_ALIGNGO2QEP_COMPLETE                  // 3: Startup State Align&Go2QEP - Complete
}eStateStartupAlignGo2QEP;

typedef struct
{
    // State & Status
    eStateStartupAlignGo2QEP state;                     // Align&Go2QEP Startup Controller state
    uint32_t qep_ready : 1;                             // QEP is ready for transition
    uint32_t unused : 31;

    // Sub-Structure
    Struct_AlignGoCMD aligngo;
}Struct_StartupAlignGo2QEP;

#endif //#ifndef STARTUP_ALIGNGO2QEP_STRUCT_H
