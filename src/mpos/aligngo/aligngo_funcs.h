
/**
 * @file aligngo_funcs.h
 * @brief Use for align and go utility function
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

#ifndef ALIGNGO_FUNCS_H
#define ALIGNGO_FUNCS_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "utils.h"
#include "driver_funcs.h"
#include "param_funcs.h"

#if defined(ENABLE_OPDOL)
#include "opd_ol_funcs.h"
#endif
#ifdef ENABLE_SIM
#include "sim_funcs.h"
#endif

#if (MODULE_MPOS_ALIGNGO == ENABLED)

//////////////////////////////////////////
// Function Declarations
//////////////////////////////////////////
extern void aligngo_param_write(Struct_Motor* motor_ptr, ParamCodeAlignGo param_code, int32_t rx_data, Struct_AlignGoCMD* aligngocmd_ptr);
extern void aligngo_setvars(Struct_Motor* motor_ptr, Struct_AlignGoCMD* aligngocmd_ptr);


//////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////
// JAGTAG: Forceang
/**
 * @brief Use for generate open loop angle
 * @param int32_t *alignangle_q26
 * @param int32_t angle_increment_q26
 */
PAC5XXX_RAMFUNC static inline void aligngo_generate_angle (Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.aligngo.alignangle_q26 += motor_ptr->mpos.aligngo.speedrefol_ramp_q26;

    // check for angle overflow and wrap
    if (motor_ptr->mpos.aligngo.alignangle_q26 > FIX16_PI_X_1024)
        motor_ptr->mpos.aligngo.alignangle_q26 -= FIX16_2PI_X_1024;
    else if (motor_ptr->mpos.aligngo.alignangle_q26 < -FIX16_PI_X_1024)
        motor_ptr->mpos.aligngo.alignangle_q26 += FIX16_2PI_X_1024;
}


/**
 * @brief Use for generate outer loop ramp open loop
 * @param Struct_Motor* motor_ptr
 * @param int32_t olspeed
 */
static inline void aligngo_update_outerloop_ramp_ol(Struct_Motor* motor_ptr, int32_t olspeed)
{
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
    // Update outerloop ramp value with calculated power
    motor_ptr->control.outerloop_ramp = motor_ptr->power.power_result;
#elif (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
    // Update outerloop ramp value with calculated speed
    motor_ptr->control.outerloop_ramp = conv_ol2cl_speed(motor_ptr->control.dt_control_1div_q6, olspeed);
#endif
    motor_ptr->control.outerloop_ramp_scaled_q14 = (motor_ptr->control.outerloop_ramp >> (motor_ptr->control.outerloop_pi_scale_shift + 2));
}


// JAGTAG: Forceang
/**
 * @brief Use to reset align and go variables
 * @param Struct_AlignGo* aligngo_ptr
 */
static inline void aligngo_reset_vars(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = 0;
    motor_ptr->mpos.aligngo.aligntime_counter = 0;
    motor_ptr->mpos.aligngo.alignangle_q26 = motor_ptr->mpos.aligngo.alignangle_target_q26;
}


/**
 * @brief Use to reset align and go state
 * @param Struct_AlignGo* aligngo_ptr
 */
static inline void aligngo_reset_state(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.aligngo.state_main = STATE_ALIGNGO_MAIN_RESET;
}


/**
 * @brief Use for align and go controller
 * @param Struct_Motor* motor_ptr
 * @param Struct_AlignGo* aligngo_ptr
 */
static inline void aligngo_controller(Struct_Motor* motor_ptr)
{
    switch(motor_ptr->mpos.aligngo.state_main)
    {
        case STATE_ALIGNGO_MAIN_RESET:
            // Reset running variables
            aligngo_reset_vars(motor_ptr);
            // Next state
            __disable_irq();
            motor_ptr->status.motor |= STATUS_MOTOR_ALIGN;
            motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
            motor_ptr->state.control = STATE_CONTROL_TORQUE;
            motor_ptr->mpos.state = STATE_MPOS_ALIGNGO;
            motor_ptr->mpos.aligngo.state_main = STATE_ALIGNGO_MAIN_ALIGN;
            // In case of zero align time, it directly goes to Ramping state
            if (motor_ptr->mpos.aligngo.aligntime_counter < motor_ptr->mpos.aligngo.aligntime_target)
                motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_ALIGN;
            else
            {
                #ifdef ENABLE_OPEN_LOOP_RAMP_NONZERO_START
                motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = fix16_mul(motor_ptr->mpos.aligngo.speedrefol_target_q26, OPEN_LOOP_RAMP_START_PCT_Q16);
                #endif
                // Update state and status
                motor_ptr->status.motor &= ~STATUS_MOTOR_ALIGN;
                motor_ptr->status.motor |= STATUS_MOTOR_OL_RAMPUP;
                motor_ptr->mpos.aligngo.state_main = STATE_ALIGNGO_MAIN_RAMPING;
                motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_SPEEDRAMP;
            }
            motor_ptr->mpos.aligngo.status &= ~STATUS_ALIGNGO_COMPLETE;
            motor_ptr->mpos.aligngo.status |= STATUS_ALIGNGO_RUNNING;
            m1_enable();
            __enable_irq();
            break;

        case STATE_ALIGNGO_MAIN_ALIGN:
#ifdef ENABLE_OPDOL
            statemachine_opdol_main(motor_ptr);

            if ((motor_ptr->mpos.aligngo.aligntime_counter >= motor_ptr->mpos.aligngo.aligntime_target)
                    && (motor_ptr->opdol.state_opdol_main == STATE_OPDOL_MAIN_COMPLETE))
#else
            if (motor_ptr->mpos.aligngo.aligntime_counter >= motor_ptr->mpos.aligngo.aligntime_target)
#endif
            {
                #ifdef ENABLE_OPEN_LOOP_RAMP_NONZERO_START
                motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = fix16_mul(motor_ptr->mpos.aligngo.speedrefol_target_q26, OPEN_LOOP_RAMP_START_PCT_Q16);
                #endif
                __disable_irq();
                // Update state and status
                motor_ptr->status.motor &= ~STATUS_MOTOR_ALIGN;
                motor_ptr->status.motor |= STATUS_MOTOR_OL_RAMPUP;
                motor_ptr->mpos.aligngo.state_main = STATE_ALIGNGO_MAIN_RAMPING;
                motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_SPEEDRAMP;
                __enable_irq();
            }
            break;

        case STATE_ALIGNGO_MAIN_RAMPING:
            // Next state and status if ramp is complete
            if (motor_ptr->mpos.aligngo.speedrefol_ramp_q26 == motor_ptr->mpos.aligngo.speedrefol_target_q26)
            {
                __disable_irq();
                motor_ptr->status.motor |= STATUS_MOTOR_OL_RAMPUP;
                motor_ptr->mpos.aligngo.status &= ~STATUS_ALIGNGO_RUNNING;
                motor_ptr->mpos.aligngo.status |= STATUS_ALIGNGO_COMPLETE;
                motor_ptr->mpos.aligngo.state_main = STATE_ALIGNGO_MAIN_RAMPCOMPLETE;
                motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_SPEEDRAMP;
                __enable_irq();
            }
            break;

        case STATE_ALIGNGO_MAIN_RAMPCOMPLETE:
            // Return to ramping state if target changes
            if (motor_ptr->mpos.aligngo.speedrefol_ramp_q26 != motor_ptr->mpos.aligngo.speedrefol_target_q26)
            {
                __disable_irq();
                motor_ptr->status.motor &= ~STATUS_MOTOR_OL_RAMPUP;
                motor_ptr->mpos.aligngo.status &= ~STATUS_ALIGNGO_COMPLETE;
                motor_ptr->mpos.aligngo.status |= STATUS_ALIGNGO_RUNNING;
                motor_ptr->mpos.aligngo.state_main = STATE_ALIGNGO_MAIN_RAMPING;
                __enable_irq();
            }
            break;

        default:
            break;
    }
}


// JAGTAG: Forceang
static inline void aligngo_process(Struct_Motor *motor_ptr)
{
    int32_t delta;

    switch(motor_ptr->mpos.aligngo.state_control)
    {
        case STATE_ALIGNGO_CONTROL_ALIGN:
            // Increment counter to keep track of align time
            motor_ptr->mpos.aligngo.aligntime_counter++;

            #ifdef ENABLE_OPDOL
            statemachine_opdol_ctl(motor_ptr);
            #endif
            break;

        case STATE_ALIGNGO_CONTROL_SPEEDRAMP:
            // Ramp speed to target
            GENERIC_RAMP(motor_ptr->mpos.aligngo.speedrefol_ramp_q26, motor_ptr->mpos.aligngo.speedrefol_target_q26, motor_ptr->mpos.aligngo.speedrefol_ramprate_q26);

            #ifdef ENABLE_OPEN_LOOP_NONLINEAR_RAMP
            // JAGTAG: Does this dupport negative speed
            if ((absval(motor_ptr->mpos.aligngo.speedrefol_ramp_q26) >= motor_ptr->mpos.aligngo.speedrefol_target_stg1_q26) &&
                    (motor_ptr->mpos.aligngo.speedrefol_ramprate_q26 != motor_ptr->startup.aligngo.speedrefol_ramprate_cmd_q26))
            {
                motor_ptr->mpos.aligngo.speedrefol_ramprate_q26 = motor_ptr->startup.aligngo.speedrefol_ramprate_cmd_q26;
            }
            #endif
            break;

        case STATE_ALIGNGO_CONTROL_ANGLERAMP:
            delta = motor_ptr->mpos.aligngo.alignangle_target_q26 - motor_ptr->mpos.aligngo.alignangle_q26;
            if (absval(delta) < absval(motor_ptr->mpos.aligngo.speedrefol_target_q26))
            {
                motor_ptr->mpos.aligngo.alignangle_q26 = motor_ptr->mpos.aligngo.alignangle_target_q26;
                motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = 0;
            }
            else
                motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = motor_ptr->mpos.aligngo.speedrefol_target_q26;// * motor_ptr->app.app_motor_direction;
            break;

        case STATE_ALIGNGO_CONTROL_IDLE:
        default:
            break;
     }

    aligngo_generate_angle(motor_ptr);
}
#endif  //#if (MODULE_MPOS_ALIGNGO == ENABLED)

#endif  //#ifndef ALIGNGO_FUNCS_H
