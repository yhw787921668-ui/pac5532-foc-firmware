
/**
 * @file isr_funcs.c
 * @brief Use for FOC initialization
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#include "isr_config.h"
#include "isr_funcs.h"
#include "utils.h"


void isr_fptr_init(Struct_Motor* motor_ptr)
{
#ifdef ENABLE_INT_ADC
    motor_ptr->fptr.fptr_int_ADC = &empty_function;
#endif

#ifdef ENABLE_INT_TA_BASE
    motor_ptr->fptr.fptr_int_TA_base = &empty_function;
    motor_ptr->fptr.fptr_int_TA_base_2 = &empty_function;
#endif

#ifdef ENABLE_INT_TA_CCR0
    motor_ptr->fptr.fptr_int_TA_ccr0 = &empty_function;
    motor_ptr->fptr.fptr_int_TA_ccr0_2 = &empty_function;
#endif

#ifdef ENABLE_INT_TA_CCR2
    motor_ptr->fptr.fptr_int_TA_ccr2 = &empty_function;
    motor_ptr->fptr.fptr_int_TA_ccr2_2 = &empty_function;
#endif

#ifdef ENABLE_INT_TB_BASE
    motor_ptr->fptr.fptr_int_TB_base = &empty_function;
    motor_ptr->fptr.fptr_int_TB_base_2 = &empty_function;
#endif

#ifdef ENABLE_INT_TB_CCR0
    motor_ptr->fptr.fptr_int_TB_ccr0 = &empty_function;
    motor_ptr->fptr.fptr_int_TB_ccr0_2 = &empty_function;
#endif

#ifdef ENABLE_INT_TB_CCR2
    motor_ptr->fptr.fptr_int_TB_ccr2 = &empty_function;
    motor_ptr->fptr.fptr_int_TB_ccr2_2 = &empty_function;
#endif
}
