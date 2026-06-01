
/**
* @file sim_v_funcs,h
* @brief V-SIM function declarations
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

#ifndef SIM_SIM2CL_H
#define SIM_SIM2CL_H

#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "sim_v_defs.h"
#include "sim_config.h"
#include "control_funcs.h"
#include "driver_funcs.h"
#include "utils.h"
#include "config_features.h"
#include "foc_math.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mod_svm.h"
#include "trig.h"
#include "mpos_config.h"

#if (MODULE_MPOS_LBG1 == ENABLED)
#include "est_sim2est.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
#include "mod_ovm.h"
#endif

#ifdef ENABLE_PLL_ANGLE_CALCULATION
#include "sqrt.h"
#endif

// SIM2CL States
#define STATE_SIM2CL_CYCLE_1        1
#define STATE_SIM2CL_CYCLE_2        2
#define STATE_SIM2CL_CYCLE_3        3
#define STATE_SIM2CL_CYCLE_4        4
#define STATE_SIM2CL_CYCLE_5        5
#define STATE_SIM2CL_CYCLE_6        6
#define STATE_SIM2CL_COMPLETE       7

// SIM2CL Definitions
#define APPROX_VDADV_B_Q26      QCONST(0.0086f, Q26)
#define APPROX_VDADV_M_Q26      QCONST(0.066f, Q26)


static inline void sim_sim2cl_sm(Struct_Motor* motor_ptr)
{
    int32_t temp_vd, temp_vq;
    #ifndef ENABLE_PLL_ANGLE_CALCULATION
    int32_t phase_rad_q16;
    #endif
    int32_t temp = 0;
    int32_t temp_angle;

    switch(motor_ptr->sim.state_sim2cl)
    {
    case STATE_SIM2CL_CYCLE_1:
        sim_deconfig(motor_ptr, 0);
        foc_config(motor_ptr, 0);
        init_emux();

#if (MPOS_SELECT  == MPOS_HALL_ONLY) || (MPOS_SELECT  == MPOS_LBG1_START_HALL)
        // Calculate interpolation increment after updating dt
        motor_ptr->mpos.hall.accumulate_degree_per_dt = ((motor_ptr->mpos.hall.hall_spd_radps >> 12) * motor_ptr->control.dt_control_q26) >> 14;  // q4 * q26 = q30 >> 14 = q16
#elif (SEL_SIM_METHOD == SIM_METHOD_QEP)
        motor_ptr->mpos.qep.filter_alpha_speed_q16 = motor_ptr->sim.filter_alpha_speed_store_q16;
        motor_ptr->mpos.qep.kspd_radps_q6 = motor_ptr->sim.kspd_store_radps_q6;
        motor_ptr->mpos.qep.kspd_scaled_q14 = motor_ptr->sim.kspd_scaled_store_q14;
#endif

        // Calculate increment for generating SIM angle during SIM2CL transition
        motor_ptr->sim.angle_inc_q16 = (((motor_ptr->sim.sim_spd_radps >> 14) * (motor_ptr->control.dt_control_q26 >> 0)) >> 12);   // q16>>14=q2 * q26 = 3q28>>12=3q16
        motor_ptr->sim.angle_inc_per_pwmdt = fix16_div(motor_ptr->sim.angle_inc_q16, (motor_ptr->control.ctrl_divider << 16));

#ifndef ENABLE_PLL_ANGLE_CALCULATION
        // Cancel angle advance done in detect2
        temp += ((fix16_mul((motor_ptr->sim.dt_control_q26 >> 0), motor_ptr->sim.sim_spd_radps) >> 10) * motor_ptr->app.app_motor_direction);
        // Add angle advance due to motor pole
        // Calculate phase=atan(w/w0)
        phase_rad_q16 = trig_arctan_90deg_q16(motor_ptr->sim.sim_spd_radps, motor_ptr->mparam.fpole_q16);
//        debug_var_2 = fix16_mul(phase_rad_q16, CONV_RAD2DEG_FIX16);
//        debug_var_3 = motor_ptr->sim.sim_spd_radps;
        temp += (((APPROX_VDADV_B_Q26 + fix16_mul(phase_rad_q16, APPROX_VDADV_M_Q26)) >> 10) * motor_ptr->app.app_motor_direction);
//        debug_var_4 = fix16_mul(temp, CONV_RAD2DEG_FIX16);
#else
        // Cancel angle advance done in detect2
        temp += ((fix16_mul((motor_ptr->sim.dt_control_q26 >> 1), motor_ptr->sim.sim_spd_radps) >> 10) * motor_ptr->app.app_motor_direction);
#endif

#if 0
        // Add manual offset
        temp += (fix16_mul(debug_input_3, CONV_DEG2RAD_FIX16) * motor_ptr->app.app_motor_direction);
//        debug_var_5 = fix16_mul(temp, CONV_RAD2DEG_FIX16);
#endif

        // Add angle correction
#if (MPOS_SELECT  == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
        temp -= motor_ptr->mpos.est.angcorr_offset * motor_ptr->app.app_motor_direction;
        temp -= (((motor_ptr->sim.sim_spd_radps >> 12) * (motor_ptr->mpos.est.angcorr_gain_q20 >> 0)) >> 8);        // 1/(2*2*pi*f0)    q16>>12=q4 * q20 = q24>>8=q16
#elif (MPOS_SELECT  == MPOS_HALL_ONLY)
#elif (MPOS_SELECT  == MPOS_LBG1_START_HALL)
#endif

        // Calculate advanced sim angle for voltage vector
        temp_angle = motor_ptr->sim.sim_angle - temp;
        wraparound(&temp_angle);

        // Calculate Vd/Vq voltages (using last calculated sim alpha/beta and angle w/comp
        park_transform_q14(temp_angle, fix16_mul(motor_ptr->sim.sim_alpha_q14, FIX16_1P0), fix16_mul(motor_ptr->sim.sim_beta_q14, FIX16_1P0), &temp_vd, &temp_vq);

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
        // Apply Overmodulation scale
        temp_vd = ((temp_vd * OVERMOD_LIMIT_Q14) >> 14);
        temp_vq = ((temp_vq * OVERMOD_LIMIT_Q14) >> 14);
#endif

        // Initialize current controllers
        init_pi(&motor_ptr->foc.Id_pid, temp_vd);
        init_pi(&motor_ptr->foc.Iq_pid, temp_vq);
        motor_ptr->foc.vd_q14 = temp_vd;
        motor_ptr->foc.vq_q14 = temp_vq;

#ifdef ENABLE_PLL_ANGLE_CALCULATION
        // Initialize the Back EMF
        int32_t temp_v_amplitude_q14;
        temp_v_amplitude_q14 = sqrt_q14(((temp_vd * temp_vd) >> 14) + ((temp_vq * temp_vq) >> 14));
        motor_ptr->mpos.est_pll.pll_amplitude_filter_q16 = (temp_v_amplitude_q14) << 2;
#endif

        // Next state
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_CYCLE_2;
        break;

    case STATE_SIM2CL_CYCLE_2:
        // Next state
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_CYCLE_3;
        break;

    case STATE_SIM2CL_CYCLE_3:
        // Initialize FOC variables
        motor_ptr->foc.ialpha_q14 = 0;
        motor_ptr->foc.ibeta_q14 = 0;
        motor_ptr->foc.idref_ramp_q14 = 0;
        motor_ptr->foc.idref_ramp_f16 = 0;
        motor_ptr->control.idref_target_f16 = 0;
        motor_ptr->foc.iqref_ramp_q14 = QCONST(SIM_IQREF_MIN, Q14);
        motor_ptr->foc.iqref_ramp_f16 = QCONST(SIM_IQREF_MIN, Q16);
        motor_ptr->control.iqref_target_f16 = QCONST(SIM_IQREF_MIN, Q16) * motor_ptr->app.app_motor_direction;

        // Initialize control variables and outerloop controller
        motor_ptr->control.Iu_measured_q14 = 0;
        motor_ptr->control.Iv_measured_q14 = 0;
        motor_ptr->control.Iw_measured_q14 = 0;
        init_pi(&motor_ptr->control.outerloop_pid, motor_ptr->foc.iqref_ramp_q14);

        // Update outer loop ramp and feedback (for switch OL to CL)
        update_outerloop_ramp_sw(motor_ptr);
        update_outerloop_fdbk_scaled(motor_ptr);

        // Next state
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_CYCLE_4;
        break;

    case STATE_SIM2CL_CYCLE_4:
        // Next state
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_CYCLE_5;
        break;

    case STATE_SIM2CL_CYCLE_5:
        // Next state
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_CYCLE_6;
        break;

    case STATE_SIM2CL_CYCLE_6:
        motor_ptr->state.control = STATE_CONTROL_TORQUE;
        mpos_config_fptr(motor_ptr);

#if (MPOS_SELECT  == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
        est_sim2est_ctrl(motor_ptr);

    #ifdef ENABLE_PLL_ANGLE_CALCULATION
        // Initialize pll speed and angle from SIM right before enabling the pulses for angle accuracy
        lbg1_pll_init(motor_ptr, motor_ptr->sim.sim_spd_radps, motor_ptr->sim.sim_angle);
    #endif

        motor_ptr->status.motor &= ~(STATUS_MOTOR_OL_RAMPUP | STATUS_MOTOR_CL_RAMPDOWN | STATUS_MOTOR_OPEN_LOOP);
        motor_ptr->status.motor |= STATUS_MOTOR_CLOSED_LOOP;

#elif (MPOS_SELECT  == MPOS_HALL_ONLY)
        motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_HALL;

#elif (MPOS_SELECT  == MPOS_LBG1_START_HALL)
        motor_ptr->mpos.mposmode = STATE_MPOSMODE_HALL;
        motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_HALL;

#elif (MPOS_SELECT  == MPOS_QEP_START_ALIGNGO)
        motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_QEP;

#elif (MPOS_SELECT  == MPOS_LBG1_START_QEP)
        motor_ptr->status.motor |= STATUS_MOTOR_MPOSMODE_QEP;
        motor_ptr->mpos.mposmode = STATE_MPOSMODE_QEP;
#endif

        // Enable outputs
        m1_enable_no_duty();

        // Next state
        motor_ptr->sim.state_sim2cl = STATE_SIM2CL_COMPLETE;

        break;

    case STATE_SIM2CL_COMPLETE:
        break;
    }
}

#endif  //#ifndef SIM_SIM2CL_H
