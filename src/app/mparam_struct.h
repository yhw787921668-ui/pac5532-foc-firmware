
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

#ifndef MPARAM_STRUCT_H
#define MPARAM_STRUCT_H


typedef struct
{
    // Motor Parameters
    fix16_t ld_q26;                     // Ld
    fix16_t lq_q26;                     // Lq
    fix16_t rs_q16;                     // Ralpha
    int32_t ke_q26;                     // Motor BEMF constant
    int32_t j_q26;                      // Load inertia
    uint8_t pole_pairs;                 // Motor pole pairs
    int32_t nom_spd_q16;                // Motor pole pairs
    int32_t fpole_q16;                  // Motor pole frequency
} Struct_MParam;

#endif  //#ifndef MPARAM_STRUCT_H
