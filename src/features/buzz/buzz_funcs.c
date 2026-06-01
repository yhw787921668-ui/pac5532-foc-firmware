
/**
 * @file buzz.c
 * @brief Buzz function definitions
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


#include "motor_struct.h"
#include "buzz_funcs.h"
#include "buzz_struct.h"
#include "app_funcs.h"
#include "fault.h"
#include "foc_funcs.h"
#include "aligngo_funcs.h"
#include "foc_config.h"
#include "mpos_config.h"
#include "driver_funcs.h"

#if defined(ENABLE_BUZZ)

void init_buzz(Struct_Motor* motor_ptr)
{
    // Initialize buzz state machine and command
    motor_ptr->buzz.state_buzz = STATE_BUZZ_IDLE;
    motor_ptr->buzz.cmd_buzz.word = 0;

    motor_ptr->buzz.aligngo.alignangle_cmd_q26 = 0;
    motor_ptr->buzz.aligngo.aligntime_cmd = 0;
    motor_ptr->buzz.aligngo.speedrefol_cmd_q26 = 0;
    motor_ptr->buzz.aligngo.speedrefol_ramprate_cmd_q26 = 0;
    motor_ptr->buzz.aligngo.idrefol_cmd_q16 = 0;
}

void command_buzz(const melody *ptr_melody, uint8_t cmd_contmode, uint8_t cmd_volume, Struct_Motor* motor_ptr)
{
    __disable_irq();
    motor_ptr->buzz.cmd_buzz.start = 1;
    motor_ptr->buzz.cmd_buzz.stop = 0;
    motor_ptr->buzz.cmd_buzz.contmode = cmd_contmode;
    motor_ptr->buzz.ptr_melody = (melody*) ptr_melody;
    motor_ptr->buzz.cmd_buzz.volume = cmd_volume;
    __enable_irq();

    //debugio1_toggle();
}


void statemachine_buzz(Struct_Motor* motor_ptr)
{
    static volatile int tonelength_target;
    fix16_t itot_ref_buzz;

    if (motor_ptr->buzz.cmd_buzz.stop)
    {
        motor_ptr->buzz.state_buzz = STATE_BUZZ_STOP;
        motor_ptr->buzz.cmd_buzz.stop = 0;
    }
    else if (motor_ptr->buzz.cmd_buzz.start)
    {
        motor_ptr->buzz.state_buzz = STATE_BUZZ_START;
        motor_ptr->buzz.cmd_buzz.start = 0;
    }

    switch(motor_ptr->buzz.state_buzz)
    {

    case STATE_BUZZ_IDLE:
    case STATE_BUZZ_DONE:
        break;

    case STATE_BUZZ_START:
        // Set buzz status
        motor_ptr->buzz.cmd_buzz.running = 1;

        // Reset buzz variables
        motor_ptr->buzz.toneseq_count = 1;
        motor_ptr->buzz.tonelength_counter = 0;
        motor_ptr->buzz.toneseq_target = motor_ptr->buzz.ptr_melody->num_notes;

        // Reset running control variables and set config parameters
        foc_reset(motor_ptr);
        control_reset(motor_ptr);
        aligngo_reset_vars(motor_ptr);
        aligngo_setvars(motor_ptr, &motor_ptr->buzz.aligngo);
    //    update_dt_controldiv(DT_X1024_30KHZ, 1);          // 2237 = 30kHz
        motor_ptr->mpos.aligngo.irefol_ramprate_q16 = BUZZER_IQ_RAMPRATE;
        motor_ptr->foc.Iq_pid.min_value = QCONST(-1.0,Q16);
        motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
        motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
        motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;

        // Configure control loop for FOC mode
        foc_config(motor_ptr, 1);
        mpos_clear_fptr(motor_ptr);

        __disable_irq();
        // Config states
        motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
        motor_ptr->state.control = STATE_CONTROL_TORQUE;
        motor_ptr->mpos.state = STATE_MPOS_ALIGNGO;
        motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_SPEEDRAMP;

        // Enable outputs
        m1_enable();
        __enable_irq();

        // Next state
        motor_ptr->buzz.state_buzz = STATE_BUZZ_CHANGENOTE;
        break;

    case STATE_BUZZ_CHANGENOTE:
        // Retrieve melody note values
        if (motor_ptr->buzz.ptr_melody->notes[motor_ptr->buzz.toneseq_count].pitch == 0)
        {
            motor_ptr->control.iqref_target_f16 = 0;
        }
        else
        {
            motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = motor_ptr->buzz.ptr_melody->notes[motor_ptr->buzz.toneseq_count].pitch * motor_ptr->control.dt_control_q26;   // q26 = q0 * q26)
            itot_ref_buzz = fix16_mul((motor_ptr->buzz.cmd_buzz.volume << 8), motor_ptr->control.imax_q16);        // q16
            motor_ptr->control.iqref_target_f16 = itot_ref_buzz;
        }
        tonelength_target = (fix16_div((motor_ptr->buzz.ptr_melody->notes[motor_ptr->buzz.toneseq_count].duration << 16), (BUZZ_TIMESTEP_Q16 * 1000)) + QCONST(0.5f, Q16)) >> 16;

//        debug_var_1 = tonelength_target << 16;
//        debug_var_2 = BUZZ_TIMESTEP_Q16;
//        debug_var_3 = itot_ref_buzz;
//        debug_dac_1a = debug_var_3 >> 2;

        // Next state
        motor_ptr->buzz.state_buzz = STATE_BUZZ_PLAYNOTE;
        break;

    case STATE_BUZZ_PLAYNOTE:
        // Check if note has played for specified time length
        if (++motor_ptr->buzz.tonelength_counter >= tonelength_target)
        {
            motor_ptr->buzz.tonelength_counter = 0;
            if (++motor_ptr->buzz.toneseq_count >= motor_ptr->buzz.toneseq_target)
            {
                motor_ptr->buzz.toneseq_count = 0;
                if (motor_ptr->buzz.cmd_buzz.contmode)
                {
                    motor_ptr->buzz.state_buzz = STATE_BUZZ_CHANGENOTE;
                }
                else
                {
                    motor_ptr->buzz.state_buzz = STATE_BUZZ_STOP;
                }
            }
            else
            {
                motor_ptr->buzz.state_buzz = STATE_BUZZ_CHANGENOTE;
            }
        }
        break;

    case STATE_BUZZ_STOP:
        motor_ptr->control.iqref_target_f16 = 0;

        if (motor_ptr->foc.iqref_ramp_f16 == 0)
        {
            __disable_irq();
            // Disable outputs
            m1_disable();

            // Reset running variables
            motor_ptr->mpos.aligngo.speedrefol_ramp_q26 = 0;

            // Restore modified values
//            update_dt_controldiv(motor_ptr->foc_dt_cmd, motor_ptr->foc_ctrl_divider_cmd);
            motor_ptr->foc.Iq_pid.min_value = motor_ptr->foc.Iq_pid_min_value_gui;
            motor_ptr->mpos.aligngo.irefol_ramprate_q16 = motor_ptr->control.idiqref_rate_gui_f16;
            motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;

            // Config states
            motor_ptr->mpos.state = STATE_MPOS_IDLE;
            motor_ptr->state.control = STATE_CONTROL_IDLE;
            motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
            __enable_irq();

            // Set buzz status
            motor_ptr->buzz.cmd_buzz.running = 0;

            // Next state
            motor_ptr->buzz.state_buzz = STATE_BUZZ_DONE;
        }
        break;

    default:
        m1_disable();
        break;
    }
}

#endif  //#if defined(ENABLE_BUZZ)
