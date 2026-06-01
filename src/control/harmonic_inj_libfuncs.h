
/**
 * @file harmonic_inj_funcs.h
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

#ifndef HARMONIC_INJ_LIBFUNCS_H
#define HARMONIC_INJ_LIBFUNCS_H

//#include "calc_angle.h"
#include "sqrt.h"
#include "trig.h"

extern PAC5XXX_RAMFUNC void hinj_process(Struct_HarmonicInj* harmonicinj_ptr, int32_t valpha_q14, int32_t vbeta_q14, int32_t *svm_ta_q14, int32_t *svm_tb_q14, int32_t *svm_tc_q14, int32_t angle_e);

#endif  //#ifndef HARMONIC_INJ_LIBFUNCS_H
