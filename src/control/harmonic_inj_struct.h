
/**
 * @file harmonic_inj_struct.h
 * @brief Use for SVM harmonic injection
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

#ifndef HARMONIC_INJ_STRUCT_H
#define HARMONIC_INJ_STRUCT_H

#include "maths.h"

/**
 * @brief Harmonic injection structure
 */
typedef struct
{
    int32_t harmonicinj_vmag_pct_q16;                               // Magnitude of injected harmonic as a percentage of the drive vector magnitude
    int32_t harmonicinj_offset_rad_q16;                             // Offset of injected harmonic as in radians
    int32_t harmonicinj_order_q0;                                   // Order of harmonic to be injected
} Struct_HarmonicInj;

#endif  //#ifndef HARMONIC_INJ_STRUCT_H
