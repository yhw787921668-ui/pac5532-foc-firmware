/**
 * @file mod_gen_funcs_lib.h
 * @brief Generic modulator library functions
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
#ifndef MOD_GEN_FUNCS_H
#define MOD_GEN_FUNCS_H

#include "maths.h"


// GenMod DPWM Definitions
#define GENMOD_DPWM_DELTA_Q16           FIX16_PI_DIV_2


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function         : mod_gen_zsc_sine
// Description      : Generates modulation signal based on sinusoidal modulation
// Output signals   : Vno
// Output range     : [0]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_RAMFUNC void mod_gen_zsc_sine(int32_t *vno_q14);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function         : mod_gen_zsc_svm
// Description      : Generates modulation signal based on space vector modulation
// Input signals    : Va, Vb, Vc
// Input range      : [+/- 1.15]
// Input format     : q14
// Output signals   : Vno
// Output range     : [+/- 1.15]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_RAMFUNC void mod_gen_zsc_svm(int32_t va_q14, int32_t vb_q14, int32_t vc_q14, int32_t *vno_q14);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function         : mod_gen_zsc_dpwm
// Description      : Generates modulation signal based on discontinuous PWM modulation
// Input signals    : Va, Vb, Vc, Valpha, Vbeta
// Input range      : [+/- 1.15]
// Input format     : q14
// Output signals   : Vno
// Output range     : [+/- 1.15]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_RAMFUNC void mod_gen_zsc_dpwm(int32_t va_q14, int32_t vb_q14, int32_t vc_q14, int32_t valpha_q14, int32_t vbeta_q14, int32_t *vno_q14);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function         : mod_gen_zsc_dpwm
// Description      : Generates modulation signal based on discontinuous PWM modulation
// Input signals    : Va, Vb, Vc
// Input range      : [+/- 1.15]
// Input format     : q14
// Output signals   : Vno
// Output range     : [+/- 1.15]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_RAMFUNC void mod_gen_zsc_dpwmmin(int32_t va_q14, int32_t vb_q14, int32_t vc_q14, int32_t *vno_q14);


#endif  //#ifndef MOD_GEN_FUNCS_H
