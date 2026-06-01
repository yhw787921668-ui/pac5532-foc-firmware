
//=============================================================================
// Copyright (C) 2017 - 2023, Qorvo, Inc.
//
// THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
// AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
// APPLICATION TO PAC PROCESSORS SUPPLIED BY ACTIVE-SEMI INTERNATIONAL;
// (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
// ACTIVE-SEMICONDUCTOR IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
// DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
// ONLY BY CERTAIN AUTHORIZED PERSONS.
//
//=============================================================================

#ifndef MOD_GEN_H
#define MOD_GEN_H

#include "pac5xxx.h"
#include "config_app.h"
#include "mod_gen_funcs.h"
#include "foc_math.h"

#if (MODULATION_SELECT == MODULATION_GENMOD_SINE)
#define MODULATION_INDEX        1.0f
#elif (MODULATION_SELECT == MODULATION_GENMOD_SVM)
#define MODULATION_INDEX        1.15f
#elif (MODULATION_SELECT == MODULATION_GENMOD_DPWM)
#define MODULATION_INDEX        1.15f
#elif (MODULATION_SELECT == MODULATION_GENMOD_DPWMMIN)
#define MODULATION_INDEX        1.15f
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:        : mod_process_gen
// Description      : Converts rotating 2-axis vector to 3-axis equivalent using a generic modulator
// Input signals    : Valpha, Vbeta
// Input range      : [+/- 1.0]
// Input format     : q14
// Output signals   : Sector, Va, Vb, Vc
// Output range     : [+/- 1.0]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_RAMFUNC static inline void mod_process_gen(int32_t valpha_q14, int32_t vbeta_q14,
        int32_t *sector, int32_t *va_q14, int32_t *vb_q14, int32_t *vc_q14)
{
    int32_t valpha_int_q14, vbeta_int_q14;
    int32_t va_int_q14, vb_int_q14, vc_int_q14;
    int32_t vno_int_q14;

    // Scale Valpha/Vbeta up to genmod index
    valpha_int_q14 = ((valpha_q14 * QCONST(MODULATION_INDEX, Q14)) >> 14);      // q14 * q14 = q28>>14 = q14
    vbeta_int_q14 = ((vbeta_q14 * QCONST(MODULATION_INDEX, Q14)) >> 14);        // q14 * q14 = q28>>14 = q14

    // Transform Valpha/Vbeta from 2-coordinate to 3-coordinate system
    inverse_clarke_transform(valpha_int_q14, vbeta_int_q14, &va_int_q14, &vb_int_q14, &vc_int_q14);

    // Zero Sequence Calculator
#if (MODULATION_SELECT == MODULATION_GENMOD_SINE)
    mod_gen_zsc_sine(&vno_int_q14);
#elif (MODULATION_SELECT == MODULATION_GENMOD_SVM)
    mod_gen_zsc_svm(va_int_q14, vb_int_q14, vc_int_q14, &vno_int_q14);
#elif (MODULATION_SELECT == MODULATION_GENMOD_DPWM)
    mod_gen_zsc_dpwm(va_int_q14, vb_int_q14, vc_int_q14, valpha_int_q14, vbeta_int_q14, &vno_int_q14);
#elif (MODULATION_SELECT == MODULATION_GENMOD_DPWMMIN)
    mod_gen_zsc_dpwmmin(va_int_q14, vb_int_q14, vc_int_q14, &vno_int_q14);
#endif

    // Inject modulation to phase voltage
    *va_q14 = va_int_q14 + vno_int_q14;
    *vb_q14 = vb_int_q14 + vno_int_q14;
    *vc_q14 = vc_int_q14 + vno_int_q14;

    // Generate sector for current reconstruction (necessary for current reconstruction from 2 out of 3 phase currents)
    if ((va_int_q14 >= vb_int_q14) && (va_int_q14 > vc_int_q14))
    {
        // Sector 1
        if (vb_int_q14 > vc_int_q14)
            *sector = 1;
        // Sector 6
        else
            *sector = 6;
    }
    if ((vb_int_q14 >= vc_int_q14) && (vb_int_q14 > va_int_q14))
    {
        // Sector 3
        if (vc_int_q14 > va_int_q14)
            *sector = 3;
        // Sector 2
        else
            *sector = 2;
    }
    if ((vc_int_q14 >= va_int_q14) && (vc_int_q14 > vb_int_q14))
    {
        // Sector 5
        if (va_int_q14 > vb_int_q14)
            *sector = 5;
        // Sector 4
        else
            *sector = 4;
    }
}

#endif  //#ifndef MOD_GEN_H
