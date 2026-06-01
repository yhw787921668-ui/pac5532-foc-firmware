
/**
 * @file mpos_funcs.h
 * @brief Use for mpos calculator function
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

#ifndef MPOS_FUNCS_H
#define MPOS_FUNCS_H

#include "pac5xxx.h"
#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"
#include "mpos_misc.h"
#include "mpos_config.h"
//#include "debug.h"


#if (MODULE_MPOS_LBG1 == ENABLED)
    #include "lbg1_funcs.h"
#endif

#if (MODULE_MPOS_ALIGNGO == ENABLED)
    #include "aligngo_funcs.h"
#endif

#if (MODULE_MPOS_HALL == ENABLED)
    #include "hall_funcs.h"
#endif

#if defined(ENABLE_HALL_TUNING)
    #include "halltune_funcs.h"
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    #include "qep_funcs.h"
#endif

#if defined(ENABLE_SIM) && defined(ENABLE_SIMANGLEBLEND)
    #include "sim_funcs.h"
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    #include "hall_hall2est.h"
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
    #include "qep_qep2est.h"
#endif

#if defined(ENABLE_MTPA)
#include "mtpa.h"
#endif

#if (MODULE_MPOS_TRAP == ENABLED)
    #include "trap_funcs.h"
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    #include "trap_trap2est.h"
#endif

/**
 * @brief Use for mpos calculation of angle and speed
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void mpos_calculator(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.fptr_mpos_1(motor_ptr);
    motor_ptr->mpos.fptr_mpos_2(motor_ptr);

#if (MODULE_MPOS_TRAP == ENABLED)
    mpos_process_trap(motor_ptr);

    #if 0
        debug_dac_1a = motor_ptr->mpos.est.est_spd_radps >> 4;//motor_ptr->mpos.trap.trap_motor_direction << 13;
        debug_dac_1b = motor_ptr->mpos.trap.BEMFStateNew_startup << 11;
        //debug_dac_1c = motor_ptr->mpos.trap.BEMFStateNew_startup << 11;
        //debug_dac_1c = motor_ptr->control.mod_sector << 11;//(motor_ptr->mpos.trap.phase_x_logic_raw) << 14;
        debug_dac_1c = motor_ptr->mpos.anglediff_pass_flag << 14;//motor_ptr->mpos.trap.BEMFStateLast_startup << 11;
        //debug_dac_2a = motor_ptr->mpos.est.estimated_angle >> 5;
        debug_dac_2a = (motor_ptr->mpos.trap.phase_x_logic_raw) << 14;//motor_ptr->mpos.trap.est2trap_delay_cnt << 10;
        debug_dac_2b = (motor_ptr->mpos.trap.phase_x_logic_filt) << 14;
        //debug_dac_2b = motor_ptr->mpos.trap.trap_speed_control_counter << 11;
        debug_dac_2c = motor_ptr->mpos.trap.trap_interpolated_angle >> 5;

        //debug_var_1 = motor_ptr->mpos.trap.trap_motor_direction << 16;
    #endif
#endif

#if defined(ENABLE_MTPA)
    mtpa_angle_calc(motor_ptr->foc.iq_q14, &motor_ptr->mtpa);
    #if 0
    //debug_dac_1a = motor_ptr->foc.iq_q14;
    //debug_dac_2a = motor_ptr->mtpa.mtpa_angle_rad_q16;
    debug_var_1 = motor_ptr->foc.iq_q14;
    debug_var_2 = motor_ptr->mtpa.mtpa_angle_ramp_rate_q16;
    debug_var_3 = motor_ptr->mtpa.mtpa_current_filter_coeff_q16;
    debug_var_4 = motor_ptr->mtpa.mtpa_gain_rad_q16;
    debug_var_5 = motor_ptr->mtpa.mtpa_angle_rad_q16;
    debug_var_6 = motor_ptr->mtpa.mtpa_iqf_q14;
    #endif
#endif

#if defined(ENABLE_QEP_LINEARIZATION_TUNING)
    // Calculate the mechenical angle from the estimate electric angle, 90% of PI = 2.8 for flipping detection threshold
    if ((motor_ptr->mpos.mpos_est_angle_prev > QCONST(2.8f, Q16)) && (motor_ptr->mpos.est.estimated_angle < QCONST(-2.8f,Q16)))
    {
        motor_ptr->mpos.angle_flip_counter++;
    }
    if (motor_ptr->mpos.angle_flip_counter > (motor_ptr->mparam.pole_pairs - 1))    motor_ptr->mpos.angle_flip_counter = 0;
    // Convert estimated angle to mechenical angle in Q16, ranged from [0, 2PI* Pole_pair]
    motor_ptr->mpos.mpos_mech_angle = motor_ptr->mpos.est.estimated_angle + (motor_ptr->mpos.angle_flip_counter * (FIX16_PI << 1)) + FIX16_PI;
    // Store the previous estimated angle for flipping detection
    motor_ptr->mpos.mpos_est_angle_prev = motor_ptr->mpos.est.estimated_angle;
    // Test use only
    #if 0
    debug_dac_2a =  motor_ptr->mpos.mpos_mech_angle;
    debug_dac_2b = motor_ptr->mpos.angle_flip_counter << 14;
    #endif
#endif

    // MPOS States
    switch(motor_ptr->mpos.state)
    {
    case STATE_MPOS_IDLE:
        break;

#if (MODULE_MPOS_LBG1 == ENABLED)
    case STATE_MPOS_ESTIMATED_ANGLE:
        // Use Estimator Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.est.estimated_angle;
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;
        #ifdef ENABLE_SWITCH_OVER_SPEED_BLEND
        motor_ptr->mpos.est.blend_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        #endif
        break;
#endif

#if (MODULE_MPOS_LBG1 == ENABLED) && defined(ENABLE_SIM) && defined(ENABLE_SIMANGLEBLEND)
    case STATE_MPOS_TRANS_SIM2EST:
        // Use blended Estimator and SIM Angle for FOC Calculations if SIM2CL
        motor_ptr->mpos.mpos_angle = sim_blend_angle(motor_ptr);

        // While angle is blending keep resetting the estimator filter accumulator to prevent noise from initial estimated angle calculations from corrupting the filter sum
        if (motor_ptr->sim.angle_blend_cnt > 0)
        {
            motor_ptr->mpos.est.est_spd_scaled_q28 = motor_ptr->sim.sim_spd_scaled_q28;
        }
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->sim.sim_spd_radps;
        break;

#elif (MODULE_MPOS_LBG1 == ENABLED) && defined(ENABLE_SIM) && !defined(ENABLE_SIMANGLEBLEND)
    case STATE_MPOS_TRANS_SIM2EST:
        // Use Estimator Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.est.estimated_angle;
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->sim.sim_spd_radps;
        break;
#endif

#if (MODULE_MPOS_ALIGNGO == ENABLED)
    case STATE_MPOS_ALIGNGO:
        aligngo_process(motor_ptr);

        #if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
        // Override the bemf state by aligngo angle during Open loop ramping
        // To do: Need to consider how to handle reverse rotation
        trap_calc_sector_from_angle((motor_ptr->mpos.aligngo.alignangle_q26 >> 10), motor_ptr);
        #endif

        // Use forced angle and speed
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.aligngo.alignangle_q26 >> 10;      // angle = alignangle_q26 * (1/(1024)); this removes the x1024
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->control.outerloop_ramp;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->control.outerloop_ramp_scaled_q14;
        #ifdef ENABLE_SWITCH_OVER_SPEED_BLEND
            motor_ptr->mpos.est.blend_spd_radps = motor_ptr->control.outerloop_ramp;
        #endif
        #ifdef ENABLE_SWITCH_OVER_ANGLE_BLEND
            motor_ptr->mpos.mpos_blend_angle = motor_ptr->mpos.aligngo.alignangle_q26 >> 10;
        #endif
        break;
#endif  //#if (MODULE_MPOS_ALIGNGO == ENABLED)

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    case STATE_MPOS_TRANS_ALIGNGO2EST:
        // Use Estimator Angle for FOC Calculations
        #ifdef ENABLE_SWITCH_OVER_SPEED_BLEND
            motor_ptr->mpos.est.blend_spd_radps = switchover_speedblend(motor_ptr->mpos.est.speed_switchover,
                                                                        motor_ptr->mpos.est.est_spd_radps, 
                                                                        &motor_ptr->startup.aligngo2est_speedblend_cnt, 
                                                                        motor_ptr->startup.aligngo2est_blend_total_cnt, 
                                                                        motor_ptr->startup.aligngo2est_blend_total_cnt_1div_q12);
            motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.blend_spd_radps;
        #else
            motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        #endif
        
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;

        
        #ifdef ENABLE_SWITCH_OVER_ANGLE_BLEND
            // Generate forced OL Angle
            aligngo_generate_angle(motor_ptr);
            motor_ptr->mpos.mpos_blend_angle = mpos_angleblend(motor_ptr->mpos.aligngo.alignangle_q26 >> 10,
                                                               motor_ptr->mpos.est.estimated_angle, 
                                                               &motor_ptr->startup.aligngo2est_angleblend_cnt, 
                                                               ALIGNGO2EST_ANGLEBLEND_TOTALCNT, 
                                                               ALIGNGO2EST_ANGLEBLEND_FACTOR_Q12);
            /*  motor_ptr->mpos.mpos_blend_angle = switchover_angleblend(motor_ptr->mpos.aligngo.alignangle_q26 >> 10,
                                                                         motor_ptr->mpos.est.estimated_angle, 
                                                                         &motor_ptr->startup.aligngo2est_angleblend_cnt, 
                                                                         ALIGNGO2EST_ANGLEBLEND_TOTALCNT, 
                                                                         ALIGNGO2EST_ANGLEBLEND_FACTOR_Q12,
                                                                         &motor_ptr->startup.angle_wrap_flag);  */
            motor_ptr->mpos.mpos_angle = motor_ptr->mpos.mpos_blend_angle;
        #else
        #if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
            GENERIC_RAMP(motor_ptr->startup.aligngo2est_angleramp_q16, 0, motor_ptr->startup.aligngo2est_angleramp_inc_radpdt_q16);
//            debug_dac_1a = motor_ptr->startup.aligngo2est_angleramp_q16 >> 5;
        #endif
            motor_ptr->mpos.mpos_angle = motor_ptr->mpos.est.estimated_angle;
        #endif
        break;
#endif  //#if (MODULE_MPOS_ALIGNGO == ENABLED)

#if (MODULE_MPOS_HALL == ENABLED)
    case STATE_MPOS_HALL_ANGLE_INTERPOLATE:
        // Use Interpolated Hall sensor Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.hall.hall_interpolated_angle;
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.hall.hall_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.hall.hall_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.hall.hall_spd_scaled_q28;
        break;
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    case STATE_MPOS_HALL2EST:
//        debugio2_toggle();
        // Use Estimator Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = mpos_angleblend(motor_ptr->mpos.hall.hall_interpolated_angle, motor_ptr->mpos.est.estimated_angle, &motor_ptr->mpos.hall.hall2est_angleblend_cnt, HALL2EST_ANGLEBLEND_TOTALCNT, HALL2EST_ANGLEBLEND_FACTOR_Q12);
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;
        break;

    case STATE_MPOS_EST2HALL:
//        debugio2_toggle();
        // Use Estimator Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = mpos_angleblend(motor_ptr->mpos.est.estimated_angle, motor_ptr->mpos.hall.hall_interpolated_angle, &motor_ptr->mpos.hall.hall2est_angleblend_cnt, HALL2EST_ANGLEBLEND_TOTALCNT, HALL2EST_ANGLEBLEND_FACTOR_Q12);
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;
        break;
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    case STATE_MPOS_QEP:
        // Use QEP sensor angle and QEP speed
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.qep.qep_angle;
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.qep.qep_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.qep.qep_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.qep.qep_spd_scaled_q28;
        motor_ptr->mpos.mpos_motor_direction = motor_ptr->mpos.qep.qep_motor_direction;
        break;
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
    case STATE_MPOS_QEP2EST:
        //debugio2_toggle();
        // Use Estimator Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = mpos_angleblend(motor_ptr->mpos.qep.qep_angle, motor_ptr->mpos.est.estimated_angle, &motor_ptr->mpos.qep.qep2est_angleblend_cnt, QEP2EST_ANGLEBLEND_TOTALCNT, QEP2EST_ANGLEBLEND_FACTOR_Q12);
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;
        break;

    case STATE_MPOS_EST2QEP:
        //debugio2_toggle();
        // Use Estimator Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = mpos_angleblend(motor_ptr->mpos.est.estimated_angle, motor_ptr->mpos.qep.qep_angle, &motor_ptr->mpos.qep.qep2est_angleblend_cnt, QEP2EST_ANGLEBLEND_TOTALCNT, QEP2EST_ANGLEBLEND_FACTOR_Q12);
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;
        break;
#endif

#if (MODULE_MPOS_TRAP == ENABLED)
    case STATE_MPOS_TRAP:
        //debugio1_toggle();
        // Use Interpolated BEMF zero-crossing Angle for FOC Calculations
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.trap.trap_interpolated_angle;
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.trap.trap_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.trap.trap_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.trap.trap_spd_scaled_q28;
        // Calculate angle diff and compensate during switching from Trap to Est 
        calc_angle_diff_trap_lbg1(motor_ptr);
        // Delay start to use bemf signal during switching from Est to Trap
        if (motor_ptr->mpos.trap.est2trap_delay_cnt > 0) motor_ptr->mpos.trap.est2trap_delay_cnt--;
        break;
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    case STATE_MPOS_TRAP2EST:
        //debugio1_toggle();
        // Use Estimator Angle with angle blending for FOC Calculations
        //motor_ptr->mpos.trap.trap_interpolated_angle += motor_ptr->mpos.trap.accumulate_degree_per_dt;
        motor_ptr->mpos.mpos_angle = mpos_angleblend(motor_ptr->mpos.trap.trap_interpolated_angle, motor_ptr->mpos.est.estimated_angle, &motor_ptr->mpos.trap.trap2est_angleblend_cnt, TRAP2EST_ANGLEBLEND_TOTALCNT, TRAP2EST_ANGLEBLEND_FACTOR_Q12);

        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.est.est_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.est.est_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.est.est_spd_scaled_q28;
        break;

    case STATE_MPOS_EST2TRAP:
        //debugio1_toggle();
        // Use the estimator Angle during the transition from FOC to Trap
        motor_ptr->mpos.mpos_angle = motor_ptr->mpos.est.estimated_angle;
        motor_ptr->mpos.mpos_spd_radps = motor_ptr->mpos.trap.trap_spd_radps;
        motor_ptr->mpos.mpos_spd_scaled_q14 = motor_ptr->mpos.trap.trap_spd_scaled_q14;
        motor_ptr->mpos.mpos_spd_scaled_q28 = motor_ptr->mpos.trap.trap_spd_scaled_q28;
        break;
#endif

    default:
        break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


void static inline mpos_init(Struct_Motor* motor_ptr)
{
    mpos_clear_fptr(motor_ptr);
}

#endif  //#ifndef MPOS_FUNCS_H
