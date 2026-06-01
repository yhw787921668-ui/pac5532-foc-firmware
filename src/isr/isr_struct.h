
/**
 * @file foc_funcs_ptr.h
 * @brief Use for FOC fnction pointer structure
 */

//=============================================================================
// Copyright (C) 2021 - 2023, Qorvo, Inc.
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

#ifndef ISR_FPTR_H
#define ISR_FPTR_H

#include "isr_config.h"


typedef struct 
{
#ifdef ENABLE_INT_ADC
    void (*fptr_int_ADC)(void *ptr);
#endif

#ifdef ENABLE_INT_TA_BASE
    void (*fptr_int_TA_base)(void *ptr);
    void (*fptr_int_TA_base_2)(void *ptr);
#endif

#ifdef ENABLE_INT_TA_CCR0
    void (*fptr_int_TA_ccr0)(void *ptr);
    void (*fptr_int_TA_ccr0_2)(void *ptr);
#endif

#ifdef ENABLE_INT_TA_CCR2
    void (*fptr_int_TA_ccr2)(void *ptr);
    void (*fptr_int_TA_ccr2_2)(void *ptr);
#endif

#ifdef ENABLE_INT_TB_BASE
    void (*fptr_int_TB_base)(void *ptr);
    void (*fptr_int_TB_base_2)(void *ptr);
#endif 

#ifdef ENABLE_INT_TB_CCR0
    void (*fptr_int_TB_ccr0)(void *ptr);
    void (*fptr_int_TB_ccr0_2)(void *ptr);
#endif

#ifdef ENABLE_INT_TB_CCR2
    void (*fptr_int_TB_ccr2)(void *ptr);
    void (*fptr_int_TB_ccr2_2)(void *ptr);
#endif

#if defined(FAMILY_52XX)
    volatile PAC5XXX_ADCSEQCTL_Type* pSeq_add;
#endif

#if defined(FAMILY_55XX)
    volatile ADC_RESULT_TYPEDEF* pSeq_add;
#endif

#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    int32_t (*fptr_int_check)(void);
    void (*fptr_int_clr)(void);
    void (*fptr_int_clr_0)(void);
#endif

}Struct_Function_Pointers; 

#endif  //#ifndef ISR_FPTR_H
