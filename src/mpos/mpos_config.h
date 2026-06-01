
/**
 * @file mpos_config.h
 * @brief Use for mpos configuration functions
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

#ifndef MPOS_CONFIG_H
#define MPOS_CONFIG_H

#include "pac5xxx.h"
#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"

#if (MODULE_MPOS_LBG1 == ENABLED)
    #include "lbg1_funcs.h"
#endif

//#if (MODULE_MPOS_ALIGNGO == ENABLED)
//    #include "aligngo_funcs.h"
//#endif

#if (MODULE_MPOS_HALL == ENABLED)
    #include "hall_funcs.h"
#endif

//#if defined(ENABLE_HALL_TUNING)
//    #include "halltune_funcs.h"
//#endif

#if (MODULE_MPOS_QEP == ENABLED)
    #include "qep_funcs.h"
#endif


static inline void mpos_config_fptr(Struct_Motor* motor_ptr)
{
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &lbg1_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &hall_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
#elif (MPOS_SELECT == MPOS_LBG1_START_HALL)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &hall_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &lbg1_process;
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &qep_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &empty_function;
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
    motor_ptr->mpos.fptr_mpos_1 = (void (*)(void *)) &qep_process;
    motor_ptr->mpos.fptr_mpos_2 = (void (*)(void *)) &lbg1_process;
#endif
}


static inline void mpos_clear_fptr(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.state = STATE_MPOS_IDLE;
    motor_ptr->mpos.fptr_mpos_1 = &empty_function;
    motor_ptr->mpos.fptr_mpos_2 = &empty_function;
}

#endif  //#ifndef MPOS_CONFIG_H
