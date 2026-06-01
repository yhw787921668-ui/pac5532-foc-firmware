
/**
 * @file foc_config.h
 * @brief Use for selecting different FOC firmware application features
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

#ifndef FOC_CONFIG_H
#define FOC_CONFIG_H

#include "motor_struct.h"

#ifdef CONTROL_VDVQ_VECTOR_LIMIT
/**
 * @brief This function use to calculate max duty cycle percentage
 * @param Struct_Motor *motor_ptr
 */
static inline void config_vdvq_vector_limit(Struct_Motor *motor_ptr)
{
    uint32_t temp1;

    // Calculate max duty cycle percentage
    if (motor_ptr->foc.svm_min_ticks > -(motor_ptr->foc.svm_max_ticks - motor_ptr->control.motor_period_ticks))
        temp1 = motor_ptr->foc.svm_min_ticks;
    else
        temp1 = -(motor_ptr->foc.svm_max_ticks - motor_ptr->control.motor_period_ticks);

    motor_ptr->control.duty_max_pct_q14 = (fix16_div((motor_ptr->control.motor_period_ticks - temp1), motor_ptr->control.motor_period_ticks) >> 2);
    motor_ptr->control.duty_max_pct_1div_q14 = (fix16_div(Q16_1P0, (motor_ptr->control.duty_max_pct_q14 << 2)) >> 2);
}
#endif

extern void foc_config(Struct_Motor* motor_ptr, int32_t flag_pwm_timer_config);
extern void foc_deconfig(Struct_Motor* motor_ptr);
extern void foc_fptr_config(Struct_Motor* motor_ptr);
extern void foc_fptr_deconfig(Struct_Motor* motor_ptr);

#endif  //#ifndef FOC_CONFIG_H
