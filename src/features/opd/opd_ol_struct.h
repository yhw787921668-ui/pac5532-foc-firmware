/**
 * @file opd_ol_struct.h
 * @brief Use for defining Open-loop OPD structure
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

#ifndef OPDOL_STRUCT_H
#define OPDOL_STRUCT_H

#include "maths.h"

typedef struct
{
    uint8_t status_opdol;
    uint8_t state_opdol_ctl;
    uint8_t state_opdol_main;
    fix16_t opdol_Sum_Iw;
    fix16_t opdol_Sum_Iv;
    fix16_t opdol_Sum_Iu;
    uint32_t olopd_start_sample_delay;
    uint32_t olopd_total_samples;
}Struct_OPDOL;

#endif
