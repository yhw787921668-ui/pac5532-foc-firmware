
/**
 * @file overmod.h
 * @brief Use for FOC over modulation functions
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

#ifndef OVERMOD_H
#define OVERMOD_H

#include "pac5xxx.h"
#include "mod_ovm_struct.h"

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
#define MODULATION_INDEX        1.15f
#endif

#define OVERMOD_LIMIT_FLT           (float)(0.5f * FLT_SQRT_3)                          // 0.8660255
#define OVERMOD_LIMITSQ_FLT         (float)(OVERMOD_LIMIT_FLT * OVERMOD_LIMIT_FLT)      // 0.7500002
#define OVERMOD_1DIV_LIMIT_FLT      (float)(1.0f / OVERMOD_LIMIT_FLT)                   // 1.1547004
#define OVERMOD_LIMIT_Q14           QCONST(OVERMOD_LIMIT_FLT, Q14)
#define OVERMOD_LIMITSQ_Q28         QCONST(OVERMOD_LIMITSQ_FLT, Q28)
#define OVERMOD_SCALE_Q14           QCONST(OVERMOD_1DIV_LIMIT_FLT, Q14)


extern const fix16_t overmod_holdangle_tbl[];

/**
 * @brief process_overmodulation, use for FOC over modulation
 * @param Struct_Overmod *overmod over modulation structure
 * @param int32_t *valpha_q14
 * @param int32_t *vbeta_q14
 * @param int32_t *mod_sector
 * @param int32_t *svm_ta_q14
 * @param int32_t *svm_tb_q14
 * @param int32_t *svm_tc_q14
 */
extern PAC5XXX_RAMFUNC void mod_process_ovm(Struct_Overmod *overmod, int32_t *valpha_q14, int32_t *vbeta_q14, int32_t *mod_sector, int32_t *mod_va_q14, int32_t *mod_vb_q14, int32_t *mod_vc_q14);

#endif
