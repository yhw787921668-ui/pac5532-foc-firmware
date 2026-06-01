
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

#ifndef MOD_H
#define MOD_H

#include "motor_struct.h"

#if (MODULATION_SELECT == MODULATION_SVM_STANDARD)
#include "mod_svm.h"
#elif (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
#include "mod_svm.h"
#include "harmonic_inj_funcs.h"
#elif (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
#include "mod_ovm.h"
#elif (MODULATION_SELECT == MODULATION_GENMOD_SINE) || (MODULATION_SELECT == MODULATION_GENMOD_SVM) || (MODULATION_SELECT == MODULATION_GENMOD_DPWM) || (MODULATION_SELECT == MODULATION_GENMOD_DPWMMIN)
#include "mod_gen.h"
#elif (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
#include "mod_trap1.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function:        : process_modulation
// Description      : Converts rotating 2-axis vector to 3-axis equivalent using the chosen modulation method
// Input signals    : Valpha, Vbeta
// Input range      : [+/- 1.0]
// Input format     : q14
// Output signals   : Sector, Va, Vb, Vc
// Output range     : [+/- 1.0]
// Output format    : q14
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void process_modulation(Struct_Motor* motor_ptr)
{
#if (MODULATION_SELECT == MODULATION_SVM_STANDARD)
    // SVM
    mod_process_svm(motor_ptr->foc.valpha_q14, motor_ptr->foc.vbeta_q14,
            &motor_ptr->control.mod_sector_sreg, &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14);
#elif (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
    // SVM
    mod_process_svm(motor_ptr->foc.valpha_q14, motor_ptr->foc.vbeta_q14,
            &motor_ptr->control.mod_sector_sreg, &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14);
    // Harmonic Injection
    hinj_process(&motor_ptr->harmonicinj, motor_ptr->foc.valpha_q14, motor_ptr->foc.vbeta_q14,\
            &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14, motor_ptr->mpos.mpos_angle);
#elif (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
    // SVM with Over modulation
    mod_process_ovm(&motor_ptr->overmod, &motor_ptr->foc.valpha_q14, &motor_ptr->foc.vbeta_q14,
            &motor_ptr->control.mod_sector_sreg, &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14);
#elif (MODULATION_SELECT == MODULATION_GENMOD_SINE) || (MODULATION_SELECT == MODULATION_GENMOD_SVM) || (MODULATION_SELECT == MODULATION_GENMOD_DPWM) || (MODULATION_SELECT == MODULATION_GENMOD_DPWMMIN)
    // Generic Modulator
    mod_process_gen(motor_ptr->foc.valpha_q14, motor_ptr->foc.vbeta_q14,
            &motor_ptr->control.mod_sector_sreg, &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14);
#elif (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
    mod_process_hybrid_trap_dmin((motor_ptr->mpos.mposmode != STATE_MPOSMODE_TRAP), motor_ptr->foc.valpha_q14, motor_ptr->foc.vbeta_q14,
            &motor_ptr->control.mod_sector_sreg, &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14);
//    debug_dac_1c = motor_ptr->control.mod_sector_sreg << 11;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

//============================================
// Compile Warnings/Errors
//============================================

#if (((MODULATION_SELECT == MODULATION_GENMOD_DPWM) || (MODULATION_SELECT == MODULATION_GENMOD_DPWMMIN)) && defined(CONTROL_VDVQ_VECTOR_LIMIT))
#warning: When MODULATION_GENMOD_DPWM or DPWMMIN is selected, CONTROL_VDVQ_VECTOR_LIMIT should be disabled to allow the maximum duty cycle to be reached.
#endif

#if (((MODULATION_SELECT == MODULATION_GENMOD_DPWM) || (MODULATION_SELECT == MODULATION_GENMOD_DPWMMIN)) && !defined(ENABLE_DUTY_LIMIT_BANDEXCLUDE))
#warning: When MODULATION_GENMOD_DPWM or DPWMMIN is selected, ENABLE_DUTY_LIMIT_BANDEXCLUDE should be enabled to avoid the pulse extension issue in some AFEs.
#endif

#if ((MODULATION_SELECT == MODULATION_GENMOD_DPWM) && !defined(ENABLE_BOOTPROT))
#warning: When MODULATION_GENMOD_DPWM is selected, ENABLE_BOOTPROT should be enabled to protect the bootstrap voltage from dropping to invalid levels.
#endif

#if (MODULATION_SELECT == MODULATION_OVERMOD)
    #ifdef ENABLE_DUTY_LIMIT_BANDEXCLUDE
        // 100% duty cycle is not currently possible when running in over-modulation
        #error: ENABLE_DUTY_LIMIT_BANDEXCLUDE cannot be enabled when MODULATION_OVERMOD is selected.
    #endif
#endif

#endif  //#ifndef MOD_H

