
/**
 * @file control_loop.h
 * @brief Use for FOC main control loop function
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

#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include "motor_struct.h"
#include "control_funcs.h"


PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void pre_control_loop(Struct_Motor* motor_ptr)
{
    switch (motor_ptr->state.precontrol)
    {
        case STATE_PRECONTROL_IPD:
            break;

        case STATE_PRECONTROL_SIM:
            #ifdef ENABLE_SIM
            // Update duty cycle from previous control loop run
            __disable_irq();
            m1_u_set_duty(motor_ptr->control.u_duty_value);
            m1_v_set_duty(motor_ptr->control.v_duty_value);
            m1_w_set_duty(motor_ptr->control.w_duty_value);
            __enable_irq();
            #endif  //#ifdef ENABLE_SIM
            break;

        case STATE_PRECONTROL_FOC:
            #if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
                control_3shunt_update_dutycycle(motor_ptr);
            #endif

            #if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
                motor_ptr->oneshunt.read_phase_current1 = readseq_ssi1();
                motor_ptr->oneshunt.read_phase_current2 = readseq_ssi2();
            #endif

            #if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
                control_1shunt_extn_update_dutycycle_seqtrigger(motor_ptr);
            #endif

            #if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
                #if (PWM_TIMER == TIMER_B)
                    control_1shunt_asym_update_dutycycleup_seqtrigger(motor_ptr);
                #endif
                #if (PWM_TIMER == TIMER_A)
                    control_1shunt_asym_update_dutycycleup_seqtrigger(motor_ptr);
                #endif
            #endif

            #if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
                 control_1shunt_newasym_update_dutycycle_seqtrigger(motor_ptr);
            #endif
            break;

        default:
            break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

extern void control_loop(Struct_Motor* motor_ptr);

#endif  //#ifndef CONTROL_LOOP_H
