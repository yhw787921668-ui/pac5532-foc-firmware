
/**
 * @file overmod_struct.h
 * @brief Use for FOC over modulation structure
 */

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

#ifndef OVERMOD_STRUCT_H
#define OVERMOD_STRUCT_H

#include "maths.h"

/**
 * @brief Over modulation structure
 */
typedef struct
{
    int32_t valpha_premod_q14;                             // hold previous Vq value for afterwards processing in q14 format
    int32_t vbeta_premod_q14;                              // hold previous Vq value for afterwards processing in q14 format
    int32_t overmod_flag;
} Struct_Overmod;

#endif
