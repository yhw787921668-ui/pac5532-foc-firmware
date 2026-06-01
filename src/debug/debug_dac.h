
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

#ifndef DEBUG_DAC_H
#define DEBUG_DAC_H

#include "motor_struct.h"
#include "pac5xxx.h"
#include "config_debug.h"
#include "config_app.h"
#include "board_options.h"

#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined(ENABLE_DEBUG_RTT)

// Configuration: PWMDAC Timers
#define PWMDAC_PERIOD_TICKS         555             // 45kHz, with 50MHz clock input, up/down counter
#define PWM_DEBUG_ARRAY_LENGTH      35              // max array index is 27, PWM debug variables can go from 0 to 34

extern fix16_t debug_scale_factor1;
extern fix16_t debug_scale_factor2;
extern fix16_t debug_scale_factor3;
extern fix16_t debug_scale_factor4;

extern int32_t* dac_ptr1;
extern int32_t* dac_ptr2;
extern int32_t* dac_ptr3;
extern int32_t* dac_ptr4;
extern int32_t* DAC_ARRAY_Q14[PWM_DEBUG_ARRAY_LENGTH];

#endif  //#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS)


// PWM DAC Duty Cycles
#ifdef ENABLE_DEBUG_PWMDAC

//=====================================
// PWM DAC Port Definitions
//=====================================


extern void pwmdac_enable(void);                                    // Enable PWM DAC
extern void pwmdac_disable(void);                                   // Disable PWM DAC

PAC5XXX_RAMFUNC static inline void run_debug_pwmdac(void)
{
    Struct_Motor* motor_ptr = &motor;

    // Calculations for Debug DAC's
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.est.estimated_angle >> 5;
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.mpos_angle >> 5;
#ifdef DEBUGDAC_DISPLAYDUTY_TICKS
    motor_ptr->debug.duty_value_u_q14 = ((motor_ptr->control.u_duty_value - (motor_ptr->control.motor_period_ticks >> 1)) * motor_ptr->control.motor_period_ticks_1div) >> 2;
    motor_ptr->debug.duty_value_v_q14 = ((motor_ptr->control.v_duty_value - (motor_ptr->control.motor_period_ticks >> 1)) * motor_ptr->control.motor_period_ticks_1div) >> 2;
    motor_ptr->debug.duty_value_w_q14 = ((motor_ptr->control.w_duty_value - (motor_ptr->control.motor_period_ticks >> 1)) * motor_ptr->control.motor_period_ticks_1div) >> 2;
#endif

    // Update debug DAC's
    pwmdac_1_set_duty((PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr1 * debug_scale_factor1) >> 22));   // q0 = q14 * q8 = q22>>22
    pwmdac_2_set_duty((PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr2 * debug_scale_factor2) >> 22));   // q0 = q14 * q8 = q22>>22
    pwmdac_3_set_duty((PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr3 * debug_scale_factor3) >> 22));   // q0 = q14 * q8 = q22>>22
}
#endif  //#ifdef ENABLE_DEBUG_PWMDAC

#endif // DEBUG_DAC_H
