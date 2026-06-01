
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

#ifndef MOD_SVM_H
#define MOD_SVM_H

#include "pac5xxx.h"

#if (MODULATION_SELECT == MODULATION_SVM_STANDARD) || (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
#define MODULATION_INDEX        1.15f
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:        : mod_process_svm
// Description      : Converts rotating 2-axis vector to 3-axis equivalent using space vector modulation
// Input signals    : Valpha, Vbeta
// Input range      : [+/- 1.0]
// Input format     : q14
// Output signals   : Sector, Va, Vb, Vc
// Output range     : [+/- 1.0]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void mod_process_svm(int32_t valpha_q14, int32_t vbeta_q14,
        int32_t *mod_sector, int32_t *mod_va_q14, int32_t *mod_vb_q14, int32_t *mod_vc_q14)
{
    fix16_t temp1_q14, temp2_q14, temp3_q14;
    int active_sector;

    temp1_q14 = vbeta_q14;
    temp2_q14 = ((vbeta_q14 >> 1) + ((valpha_q14 * ZERO_POINT_866_IN_Q14)>> 14));    // Q14 + Q14 = Q14, Q14 * Q14 = Q28 >> 14 = Q14
    temp3_q14 = temp2_q14 - temp1_q14;

    // Calculate sector for sector PWM
    active_sector = 3;
    if (temp2_q14 > 0)
        active_sector = active_sector - 1;
    if (temp3_q14 > 0)
        active_sector = active_sector - 1;
    if (temp1_q14 < 0)
        active_sector = 7 - active_sector;

    *mod_sector = active_sector;

    switch (*mod_sector)
    {
        case 1:
        case 4:
            *mod_va_q14 = temp2_q14;
            *mod_vb_q14 = temp1_q14 - temp3_q14;
            *mod_vc_q14 = -temp2_q14;
            break;
        case 2:
        case 5:
            *mod_va_q14 = temp3_q14 + temp2_q14;
            *mod_vb_q14 = temp1_q14;
            *mod_vc_q14 = -temp1_q14;
            break;
        case 3:
        case 6:
            *mod_va_q14 = temp3_q14;
            *mod_vb_q14 = -temp3_q14;
            *mod_vc_q14 = -(temp1_q14 + temp2_q14);
            break;
        default:
            break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef MOD_SVM_H
