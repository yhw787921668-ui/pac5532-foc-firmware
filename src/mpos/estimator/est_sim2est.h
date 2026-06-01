
/**
 * @file est_sim2est.h
 * @brief Use for making system variables ready for SIM to EST transition
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

#ifndef EST_SIM2EST_H
#define EST_SIM2EST_H


#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"
#include "lbg1_funcs.h"

#if (MODULE_MPOS_LBG1 == ENABLED) && defined(ENABLE_SIM)

/**
 * @brief Use for making system ready to do transition from SIM to EST
 * @param Struct_Motor* motor_ptr
 */
static inline void est_sim2est_main(Struct_Motor* motor_ptr)
{
    // Check if transitions are complete
    lbg1_check_transition_ready(&motor_ptr->mpos.est);

    // Update outer loop ramp and feedback (for stall detection and display)
    update_outerloop_ramp_sw(motor_ptr);
}


/**
 * @brief Use for making transition ready from SIM to EST
 * @param Struct_Motor* motor_ptr
 */
static inline void est_sim2est_ctrl(Struct_Motor* motor_ptr)
{
    // Initialize rotor angle and mpos
    lbg1_update_state(motor_ptr, motor_ptr->sim.sim_angle, motor_ptr->sim.sim_spd_radps);

    // Update E and I
    motor_ptr->mpos.est.est_ialpha_q14 = 0;
    motor_ptr->mpos.est.est_ibeta_q14 = 0;
    motor_ptr->mpos.est.est_ealpha_q14 = motor_ptr->sim.sim_alpha_q14;
    motor_ptr->mpos.est.est_ebeta_q14 = motor_ptr->sim.sim_beta_q14;
    motor_ptr->mpos.est.valpha_sreg0_q14 = motor_ptr->sim.sim_alpha_q14;
    motor_ptr->mpos.est.vbeta_sreg0_q14 = motor_ptr->sim.sim_beta_q14;

    motor_ptr->mpos.est.w0_target_q06 = motor_ptr->mpos.est.w0_q06;
#ifndef ENABLE_SIM_GEN_MODE
    motor_ptr->mpos.est.w0_ramp_q06 = motor_ptr->mpos.est.w0_target_q06;
#else
    // Make sure the estimator bandwidth is recalcualted accordingly before switching into CL
    motor_ptr->mpos.est.w0_ramp_q06 = fix16_mul(QCONST(0.99f, Q16), motor_ptr->mpos.est.w0_target_q06);
    lbg1_statemon(motor_ptr);
    lbg1_process_voltage_scale(motor_ptr);
#endif

    motor_ptr->mpos.state = STATE_MPOS_TRANS_SIM2EST;
}

#endif  //(MODULE_MPOS_LBG1 == ENABLED)
#endif  //#ifndef EST_SIM2EST_H
