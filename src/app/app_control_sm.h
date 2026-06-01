
/** 
 * @file app_control_sm.h
 * @brief Application control state machine, use for ADC channel calibration, IPD, SIM, Motor Run
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

#ifndef SM_CONTROL_H
#define SM_CONTROL_H

#include "motor_struct.h"
#include "control_funcs.h"
#include "app_funcs.h"
#include "driver_funcs.h"
#include "foc.h"
#include "mpos_funcs.h"
#include "config_features.h"
#include "control_sm.h"
#include "app_control_sm.h"

#ifdef ENABLE_SIM
#include "sim_funcs.h"
#endif


#ifdef ENABLE_IPD
#include "ipd_config.h"
#include "ipd.h"
#endif

#if defined(ENABLE_STALL_DETECT)
#include "stalldet_funcs.h"
#endif

#ifdef ENABLE_ADCCAL_POWERUP
#include "adccal.h"
#endif

/**
 * @brief This function use to control state flow of FOC like, ADC channel calibration, IPD, SIM, Motor Run
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC  static inline void statemachine_appcontrol (void)
{
    static Struct_Motor* motor_ptr = &motor;
//    int32_t temp;

    switch (motor_ptr->state.appcontrol)
    {
        case STATE_APPCONTROL_IDLE:
            break;

#ifdef ENABLE_ADCCAL_POWERUP
        case STATE_APPCONTROL_ADCCAL:
            statemachine_adccal(motor_ptr);
            break;
#endif

        case STATE_APPCONTROL_IPD:
#ifdef ENABLE_IPD
            // Call IPD State Machine until IPD is done
            ipd_state_machine(&motor.ipd, (void *)&motor);
#endif
            break;

#ifdef ENABLE_SIM
        case STATE_APPCONTROL_SIM_RUN:
            // Run SIM state machine
            statemachine_sim(motor_ptr);

            // Next state and status
            if ((motor_ptr->sim.status_sim & STATUS_SIM_COMPLETE) && (motor_ptr->sim.status_sim & STATUS_SIM_SWITCH2CL))
            {
                // Next state
                motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
            }
            break;
#endif  //#ifdef ENABLE_SIM

        case STATE_APPCONTROL_MOTOR_RUN:
            // Calculate motor phase currents
            process_phase_currents(motor_ptr);

            // Motor control algorithm
            process_motor_control(motor_ptr);

#if defined(ENABLE_AUTOTUNE_MID)
            // Process MID2 control loop operations
            mid2_process_control(motor_ptr);
#endif
            break;

        default:
            break;
    }

#if defined(ENABLE_SIM) && 0
//    debug_dac_2b = motor_ptr->sim.sim_angle >> 5;
#endif

    // Convert and condition duty cycle
    convert_duty_cycle(motor_ptr);

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    switch (motor_ptr->state.appcontrol)
    {
    case STATE_APPCONTROL_MOTOR_RUN:
        // Process single shunt
#if ((MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC) || (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY))
        process_1shunt_asym(&motor_ptr->oneshunt, motor_ptr->control.u_duty_value, motor_ptr->control.v_duty_value, motor_ptr->control.w_duty_value, &motor_ptr->control.mod_sector_sreg);
#elif (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
        process_1shunt_extn(&motor_ptr->oneshunt, &motor_ptr->control.u_duty_value, &motor_ptr->control.v_duty_value, &motor_ptr->control.w_duty_value, &motor_ptr->control.mod_sector_sreg);
#endif
        break;

#ifdef ENABLE_SIM
    case STATE_APPCONTROL_SIM_RUN:
        motor_ptr->oneshunt.taccr0_value = motor_ptr->sim.braking_duty_min_ticks;
        break;
#endif

    default:
        break;
    }
#endif  //#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif

