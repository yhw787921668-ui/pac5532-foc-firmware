
/**
* @file sim_sm.c
* @brief SIM state machine
*/

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

#include "motor_struct.h"
#include "sim_funcs.h"
#include "driver_funcs.h"
#include "mpos_funcs.h"
#include "pac5xxx_driver_tile.h"
#include "config_app.h"

#ifdef ENABLE_SIM

void statemachine_sim(Struct_Motor* motor_ptr)
{
    Struct_SIM* sim_ptr = &motor_ptr->sim;

#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_CURRENT))
    // Sample phase values
    sim_read_inputs(motor_ptr);
#endif
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger

    // SIM timeout check
    if (sim_ptr->braking_timeout_counter++ > sim_ptr->braking_timeout_countmax)
    {
        motor_ptr->sim.state_sim = STATE_SIM_EXIT;
        sim_ptr->status_sim |= STATUS_SIM_TIMEOUT;
    }

    // State machine cases
    switch (motor_ptr->sim.state_sim)
    {
    case STATE_SIM_RESET:
        // Reset SIM variables
        sim_reset(motor_ptr);
        // JAGTAG: Need to move sim2cl state reset back into sim_reset() but need to standardize sim_reset() across sim methods
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_CYCLE_1;

        // Next state and update status
#if (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
        motor_ptr->sim.state_sim = STATE_SIM_ENLS;
#else
        motor_ptr->sim.state_sim = STATE_SIM_MDETECT;
        motor_ptr->sim.status_sim |= STATUS_SIM_MONITORING;
#endif
        break;

#if (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
    case STATE_SIM_ENLS:
        // Enable LS drivers for SIM detection & braking
        m1_LS_pwm_only_config();

        // Next state and update status
        motor_ptr->sim.state_sim = STATE_SIM_MDETECT;
        motor_ptr->sim.status_sim |= STATUS_SIM_MONITORING;
        break;
#endif

    case STATE_SIM_MDETECT:
       // Motion detection state operations
        sim_mdetect(motor_ptr);

        // If motion is not detected exit SIM
        if (motor_ptr->sim.motion_notdetected)
        {
            // Next state and update status
            motor_ptr->sim.state_sim = STATE_SIM_EXIT;
            motor_ptr->sim.status_sim |= STATUS_SIM_MOTIONNOTDETECTED;
            motor_ptr->sim.status_sim &= ~STATUS_SIM_MONITORING;
        }

        // If motion is detected then proceed to Detect2 state
        else if (motor_ptr->sim.motion_detected)
        {
            // Reset counter for detect2
            motor_ptr->sim.sim_counter = 0;

            // Next state and update status
            motor_ptr->sim.state_sim = STATE_SIM_DETECT2;
            motor_ptr->sim.status_sim |= STATUS_SIM_MOTIONDETECTED;

            sim_mdetect_true(motor_ptr);
        }
        break;

    case STATE_SIM_DETECT2:
        // Measure SIM values
        sim_detect2(motor_ptr);

        // Decide next state after fixed delay
        if (motor_ptr->sim.sim_counter > motor_ptr->sim.sim_detect2_countmax)
        {
            // Process SIM measurements and decide the next state
            sim_process(motor_ptr);

#if (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
            // Disable outputs if SIM2CL in iSIM. They get re-enabled at the end of SIM2CL SM
            if (motor_ptr->sim.status_sim & STATUS_SIM_SWITCH2CL)
                m1_disable();
#endif

            // Next status
            motor_ptr->sim.status_sim &= ~STATUS_SIM_MONITORING;
        }
        break;

    case STATE_SIM_SIM2CL:
        // SIM2CL state operations
        sim_sim2cl(motor_ptr);

        // Next state when SIM2CL state machine is complete
        if (motor_ptr->sim.state_sim2cl == STATE_SIM2CL_COMPLETE)
        {
            motor_ptr->sim.state_sim = STATE_SIM_COMPLETE;
            motor_ptr->sim.status_sim |= STATUS_SIM_COMPLETE;
        }
        break;

    case STATE_SIM_WAITSTOP_INIT:
        // Reset waitstop variables
        sim_waitstop_reset(motor_ptr);

        // Next state and update status
        motor_ptr->sim.state_sim = STATE_SIM_WAITSTOP_RUN;
        motor_ptr->sim.status_sim |= STATUS_SIM_WAITSTOP;
        motor_ptr->sim.status_sim &= ~STATUS_SIM_MONITORING;
        motor_ptr->status.motor |= STATUS_MOTOR_WAITSTOP;
        break;

    case STATE_SIM_WAITSTOP_RUN:
        // Detect if motor has stopped during waitstop
        sim_detect_waitstop(motor_ptr);

        // Next state if motion is not detected
        if (sim_ptr->motion_notdetected)
        {
            motor_ptr->sim.state_sim = STATE_SIM_EXIT;
            motor_ptr->sim.status_sim |= STATUS_SIM_WAITSTOPCOMPLETE;
            motor_ptr->sim.status_sim &= ~STATUS_SIM_WAITSTOP;
            motor_ptr->status.motor &= ~STATUS_MOTOR_WAITSTOP;
        }
        break;

    case STATE_SIM_BRAKE_INIT:
        // Reset braking variables
        sim_brake_reset(motor_ptr);

#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_HALL)|| (SEL_SIM_METHOD == SIM_METHOD_QEP))
        // Configure sampling point during braking
        config_seq_simbrake();
#endif

        // Reset counter for counting cycles before enabling LS
        motor_ptr->sim.sim_counter = 0;

        // Next state and update status
        motor_ptr->sim.state_sim = STATE_SIM_BRAKE_ENLS;
        motor_ptr->sim.status_sim |= STATUS_SIM_BRAKING;
        motor_ptr->sim.status_sim &= ~STATUS_SIM_MONITORING;
        motor_ptr->status.motor |= STATUS_MOTOR_BRAKING;
        break;

    case STATE_SIM_BRAKE_ENLS:
        // Need to wait 2 cycles before enabling the gate drivers (1 cycle to allow the new duty to be converted + 1 cycle to allow the new duty register to latch)
        if (sim_ptr->sim_counter++ > 1)
        {
#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_CURRENT))
            // Reset counter for motion detection
            motor_ptr->sim.sim_counter = (motor_ptr->sim.sim_mdetect_countmax >> 1);
#endif

            // Enable LS drivers for SIM detection & braking
            m1_LS_pwm_only_config();

            // Next state and update status
            motor_ptr->sim.state_sim = STATE_SIM_BRAKE_RUN;
        }
        break;

    case STATE_SIM_BRAKE_RUN:
        // Detect if motor has stopped during braking
        sim_detect_brakestop(motor_ptr);

        // Next state if motion is not detected
        if (sim_ptr->motion_notdetected)
        {
            motor_ptr->sim.state_sim = STATE_SIM_EXIT;
            motor_ptr->sim.status_sim |= STATUS_SIM_BRAKECOMPLETE;
            motor_ptr->sim.status_sim &= ~STATUS_SIM_BRAKING;
            motor_ptr->status.motor &= ~STATUS_MOTOR_BRAKING;
        }
        break;

    case STATE_SIM_EXIT:
        // Disable outputs when SIM is complete
        m1_disable();

        // Next state and update status
        motor_ptr->sim.state_sim = STATE_SIM_COMPLETE;
        motor_ptr->sim.status_sim |= STATUS_SIM_COMPLETE;
        break;

    case STATE_SIM_COMPLETE:
        break;

    default:
        break;
    }

#if 0       // For Debugging
//    debug_dac_1a = motor_ptr->sim.sim_alpha_q14;
//    debug_dac_1b = motor_ptr->sim.sim_beta_q14;
//    debug_dac_1c = motor_ptr->sim.braking_duty_cycle_q14;
//    debug_dac_1a = motor_ptr->sim.sim_spd_scaled_q14;
//    debug_dac_2b = (motor_ptr->sim.sim_angle >> 5);
//    debug_dac_2c =  motor_ptr->sim.sim_spd_prefilt_scaled_q14;
//    debug_dac_1a = sim_ptr->vphase_u_q14;
//    debug_dac_1b = sim_ptr->vphase_v_q14;
//    debug_dac_1c = sim_ptr->vphase_w_q14;
//    debug_dac_1a = sim_ptr->iphase_u_q14;
//    debug_dac_1b = sim_ptr->iphase_v_q14;
//    debug_dac_1c = sim_ptr->iphase_w_q14;
//    debug_dac_2c = sim_ptr->phase_adc_threshold_q14;
//    debug_dac_1b = motor_ptr->sim.sim_counter;
//    debug_dac_1b = motor_ptr->sim.sim_counter << 9;
#endif

}

#endif  //#ifdef ENABLE_SIM
