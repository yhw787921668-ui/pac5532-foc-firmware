
/**
 * @file mtpa.h
 * @brief Use for FOC mtpa functions
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

#ifndef MTPA_H
#define MTPA_H

#include "pac5xxx.h"
#include "mtpa_struct.h"

/**
 * @brief mtpa_angle_calc, use for FOC MTPA
 * @param Struct_Mtpa *mtpa MTPA structure
 */
extern PAC5XXX_RAMFUNC void mtpa_angle_calc(int32_t iq_q14, Struct_Mtpa *mtpa);
extern PAC5XXX_RAMFUNC void mtpa_gain_calc_lib(Struct_Mtpa *mtpa);
#endif

