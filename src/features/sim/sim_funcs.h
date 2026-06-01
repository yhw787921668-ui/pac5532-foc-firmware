
/**
* @file sim_funcs.h
* @brief Contains generic SIM function definitions and includes all SIM functions for the selected SIM method
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

#ifndef SIM_FUNCS_H
#define SIM_FUNCS_H

#include "pac5xxx_driver_adc.h"
#include "config_features.h"
#include "motor_struct.h"
#include "control_funcs.h"
#include "driver_funcs.h"
#include "utils.h"
#include "param_funcs.h"


#if defined(ENABLE_SIM)

#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE))
#include "sim_v_funcs.h"
#elif (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
#include "sim_i_funcs.h"
#elif (SEL_SIM_METHOD == SIM_METHOD_HALL)
#include "sim_h_funcs.h"
#elif (SEL_SIM_METHOD == SIM_METHOD_QEP)
#include "sim_q_funcs.h"
#endif


//=====================================
// Calculations
//=====================================
#define SIM_ANGLE_BLEND_FACTOR_Q12                  QCONST((1.0 / (float)SIM_ANGLE_BLEND_MAX_CNT), Q12)


//=====================================
// Function Declarations (sim_funcs.c)
//=====================================
extern void sim_smstate_reset(Struct_Motor *motor_ptr);


//=====================================
// Function Declaration (sim_sm.c)
//=====================================
extern void statemachine_sim(Struct_Motor* motor_ptr);


//=====================================
// Inline Function Definitions
//=====================================

/**
* @brief Processes detected motor information to decide how SIM will handle the startup
* @param Struct_Motor* motor_ptr
*/
static inline void sim_process(Struct_Motor *motor_ptr)
{
#if defined(ENABLE_SIMBRAKE_MOTORDISABLE)
    // Start braking if SIM is configured to run in brake mode
    motor_ptr->sim.state_sim = STATE_SIM_BRAKE_INIT;

#else
    // If SIM is commanded to run in normal mode
#ifndef TM_SIM_NODETECT
    // Case: Direction is opposite of direction command
    if (motor_ptr->sim.sim_motor_direction != motor_ptr->app.app_motor_direction_cmd)
    {
#if defined(ENABLE_SIMBRAKE_REVDIRECTION)
        // Start braking
        motor_ptr->sim.state_sim = STATE_SIM_BRAKE_INIT;
#else
        // Wait for motor to slow down on its own
        motor_ptr->sim.state_sim = STATE_SIM_WAITSTOP_INIT;
#endif

        // Update status
        motor_ptr->sim.status_sim |= STATUS_SIM_WRONGDIRECTION;
    }

    // Case: Direction is equal to direction command
    else
    {
        // Case: Speed is lower than OL2CL limit
        if (absval(motor_ptr->sim.sim_spd_radps) < motor_ptr->sim.freeInMotion_min_speed_limit)
        {
            // Enter brake or waitstop & update status
#ifdef ENABLE_SIMBRAKE_SLOWSPEED
            motor_ptr->sim.state_sim = STATE_SIM_BRAKE_INIT;
#else
            motor_ptr->sim.state_sim = STATE_SIM_WAITSTOP_INIT;
#endif
            motor_ptr->sim.status_sim |= STATUS_SIM_SLOWSPEED;
            motor_ptr->sim.status_sim &= ~(STATUS_SIM_SWITCH2CL | STATUS_SIM_ABOVESIM2CL);
        }

#ifndef TM_SIM_SLOWSPEED
        // Case: Speed is higher than SIM2CL upper limit
        else if (absval(motor_ptr->sim.sim_spd_radps) > motor_ptr->sim.sim2cl_maxspd_radps_q16)
        {
            // Stay in MDETECT state & update status
            motor_ptr->sim.state_sim = STATE_SIM_DETECT2;
            motor_ptr->sim.status_sim |= (STATUS_SIM_MONITORING | STATUS_SIM_ABOVESIM2CL);
            motor_ptr->sim.status_sim &= ~(STATUS_SIM_SWITCH2CL | STATUS_SIM_SLOWSPEED);
        }

        // Case: Speed is lower than SIM2CL upper limit but higher than OL2CL limit
        else if (absval(motor_ptr->sim.sim_spd_radps) >= motor_ptr->sim.freeInMotion_min_speed_limit)
        {
            // Move to detect stage2 & update status
            motor_ptr->sim.state_sim = STATE_SIM_SIM2CL;
            motor_ptr->sim.status_sim |= STATUS_SIM_SWITCH2CL;
            motor_ptr->sim.status_sim &= ~(STATUS_SIM_SLOWSPEED | STATUS_SIM_ABOVESIM2CL);
        }
#endif          //#ifndef TM_SIM_SLOWSPEED
    }
#else           //#ifndef TM_SIM_NODETECT
    motor_ptr->debug.sim_spd_nobemf_radps = motor_ptr->sim.sim_spd_radps;
#endif          //#ifndef TM_SIM_NODETECT
#endif          //#if defined(ENABLE_SIMBRAKE_MOTORDISABLE)
}


#if (MODULE_MPOS_LBG1 == ENABLED)
/**
* @brief Blends SIM angle and estimated angle during a SIM2EST transition
* @param Struct_Motor* motor_ptr
* @return int32_t blend_angle
*/
PAC5XXX_RAMFUNC static inline int32_t sim_blend_angle(Struct_Motor *motor_ptr)
{
    int32_t blend_angle;
    int32_t est_ang_prime;

    // Generate SIM angle for blending
    motor_ptr->sim.sim_angle = motor_ptr->sim.sim_angle + motor_ptr->sim.angle_inc_q16;
    if(motor_ptr->sim.sim_angle > FIX16_PI)
        motor_ptr->sim.sim_angle = motor_ptr->sim.sim_angle - FIX16_2PI;
    else if(motor_ptr->sim.sim_angle < -FIX16_PI)
        motor_ptr->sim.sim_angle = motor_ptr->sim.sim_angle + FIX16_2PI;

    // Manipulate estimated angle wraparound so that both SIM blend angle and estimated angle are in the same "wrap"
    if ((motor_ptr->mpos.est.estimated_angle - motor_ptr->sim.sim_angle) > FIX16_PI)
        est_ang_prime = motor_ptr->mpos.est.estimated_angle - FIX16_2PI;
    else if ((motor_ptr->mpos.est.estimated_angle - motor_ptr->sim.sim_angle) < -FIX16_PI)
        est_ang_prime = motor_ptr->mpos.est.estimated_angle + FIX16_2PI;
    else
        est_ang_prime = motor_ptr->mpos.est.estimated_angle;

    // Calculate blended angle
    blend_angle = (((est_ang_prime * SIM_ANGLE_BLEND_FACTOR_Q12) >> 12) * (SIM_ANGLE_BLEND_MAX_CNT - motor_ptr->sim.angle_blend_cnt))+
            (((motor_ptr->sim.sim_angle * SIM_ANGLE_BLEND_FACTOR_Q12) >> 12) * motor_ptr->sim.angle_blend_cnt);  // q16 * q12 = 3q28>>12=3q16
    wraparound(&blend_angle);

    // Decrement blend counter
    if (motor_ptr->sim.angle_blend_cnt > 0)
    {
        motor_ptr->sim.angle_blend_cnt = motor_ptr->sim.angle_blend_cnt - 1;
//        debugio2_toggle();
    }

    return blend_angle;
}
#endif


static inline void sim_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case sim_braking_align:
        // This align value will use during open loop align start-up in case of Active Reverse Braking Start-up Sequence
        motor_ptr->sim.align_count_braking_timeout = rx_data;
        break;
    case sim_braking_ol_id_iq_ref_mul:
        motor_ptr->sim.OL_Id_ref_braking_mul = rx_data;
        break;
    case sim_braking_ol_iq_ref_mul:
        motor_ptr->sim.OL_Iq_ref_braking_mul = rx_data;
        break;
    case sim_braking_timeout:
        // Convert counts from FOC dt ticks to SIM dt ticks for SIM braking to work properly
        // FOC param write must be called first for this calculation
        motor_ptr->sim.braking_timeout_countmax = fix16_mul(rx_data << 10, fix16_div((motor_ptr->foc.ctrl_divider_foc << 10), (SIM_CONTROL_DIVIDER << 10))) >> 10;
        break;
    case sim_braking_diff_threshold:
        // Q30 coming from GUI
        break;
#if (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
    case sim_current_track_gain:
        break;
#endif
#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE))
    case sim_bemf_amp_gain:
	{
        int32_t temp;

        check_param_while_run(motor_ptr);
        motor_ptr->app.vphase_amp_gain = rx_data;
        init_diff_amp_gain();
        temp = fix16_div(QCONST(R1_PLUS_R2_BY_R2_RATIO_SIM, Q16), (((fix16_t)sim_conv_ampgainval2gain(motor_ptr->app.vphase_amp_gain))) << 16);
        temp = fix16_mul(temp, QCONST(2.5f, Q16));
        motor_ptr->sim.multp_conv_adc2volts_q16 = fix16_div(temp, QCONST(((1 << ADC_BITS_FIRMWARE_SELECTION) - 1.0f), Q16));
        break;
	}
#endif
    default:
        break;
    }
}


/**
* @brief Calculates SIM parameters based on user defined values
* @param Struct_Motor* motor_ptr
* @param ParameterCode param_code
* @param int32_t rx_data
*/
static inline void sim_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case dt:
    case iq_ref_cl:
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    case open_loop_speed_setpoint:
#endif
#if (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
    case sim_current_track_gain:
#endif
        sim_calculate_parameters(motor_ptr);
        break;
    default:
        break;
    }
}
#endif
#endif
