
/**
 * @file sim_q_funcs.c
 * @brief Use for SIM QEP utility functions
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

#include "motor_struct.h"
#include "sim_funcs.h"
#include "calc_speed.h"

#define SIM_MDETECT_SPEED_THRESHOLD_RADPS_Q16       SIM_MDETECT_SPEED_THRESHOLD_HZ_Q0 * FIX16_2PI


#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_QEP)

// JAGTAG: Customize sim_calculate_parameters() for SIM-Q

/**
 * @brief This function use to calculate SIM parameters
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_calculate_parameters(Struct_Motor* motor_ptr)
{
    int32_t temp1;

    // Calculate SIM dt independently from foc dt
    motor_ptr->sim.dt_control_q26 = fix16_div(motor_ptr->control.dt_control_q26, motor_ptr->foc.ctrl_divider_foc << 16) * SIM_CONTROL_DIVIDER;
    motor_ptr->sim.dt_control_1div_q6 = fix16_div(FIX16_1P0, motor_ptr->sim.dt_control_q26);

#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP)
    // Set min speed for SIM2CL to 0
    motor_ptr->sim.freeInMotion_min_speed_limit = 0;
#endif

    // Set SIM speed scale shift same as the application speed scale shift
    motor_ptr->sim.speed_scale_shift = motor_ptr->app.speed_scale_shift;

    // Set SIM2CL max speed to percentage of application max speed
    motor_ptr->sim.sim2cl_maxspd_radps_q16 = fix16_mul(motor_ptr->app.speedrefcl_max, SIM_SIM2CL_MAXSPD_PCT_Q16);

    // Calculate sim.sim_detect2_countmax based on the filter rise time (For 99% settling, Tsettle = 4.6 * Tau = 3 / (2pi * fc))
    temp1 = fix16_mul(FIX16_2PI, motor_ptr->mpos.qep.filter_fc_speed_q16);    // q16
    temp1 = fix16_div(QCONST(4.6f, Q16), temp1);                        // q16
    motor_ptr->sim.sim_detect2_countmax = fix16_div(temp1, motor_ptr->sim.dt_control_q26) >> 6;     // q0 = div(q16,q26)=q6 >> 6
    if (motor_ptr->sim.sim_detect2_countmax < 10)
        motor_ptr->sim.sim_detect2_countmax = 10;

    // Calculate min_duty_limit_ticks = time * clk * 0.5(up-down timer) * period_ticks
    motor_ptr->sim.braking_duty_min_ticks = ((fix16_mul(BRAKING_DUTY_LIMIT_SEC_Q26, (TIMERCLK_FREQ_HZ >> 10))) >> 1);                                                   // q26 * q0>>10=qm10 = q0
    motor_ptr->sim.braking_duty_min_q14 = ((((fix16_div(motor_ptr->sim.braking_duty_min_ticks, motor_ptr->control.motor_period_ticks)) << 1) - Q16_1P0) >> 2);      // q16>>2 = q14

    // Calculate braking duty decrement per SIM dt
    temp1 = fix16_div((SIM_BRAKING_DUTYRAMPTIME_SEC_Q8 << 8), (motor_ptr->sim.dt_control_q26)) >> 6;   // temp1 = Ramp counts in SIM dt's
    motor_ptr->sim.braking_duty_cycle_dec_q14 = (fix16_div((QCONST(1.0, Q14) - motor_ptr->sim.braking_duty_min_q14) << 8, temp1) >> 24);   // decrement = swing / counts
    if(motor_ptr->sim.braking_duty_cycle_dec_q14 <= 0)
        motor_ptr->sim.braking_duty_cycle_dec_q14 = 1;
    else if(motor_ptr->sim.braking_duty_cycle_dec_q14 > QCONST(1.0, Q14))
        motor_ptr->sim.braking_duty_cycle_dec_q14 = QCONST(1.0, Q14);

    // Calculate motion detection countmax = (SIM_MDETECT_DETECTTIME_SEC_Q16 / sim_dt) * 2 (>> (Qshift - 1))
    motor_ptr->sim.sim_mdetect_countmax = fix16_div(SIM_MDETECT_DETECTTIME_SEC_Q16, motor_ptr->sim.dt_control_q26) >> 6;         // q0 = div(q16,q26)=q6 >> 6
    if (motor_ptr->sim.sim_mdetect_countmax < 10)
        motor_ptr->sim.sim_mdetect_countmax = 10;

    // Calculate speed filter alpha gain
    fix16_t dt_actual_q26;
    dt_actual_q26 = QEP_SPEED_CALCULATION_DIVIDER * motor_ptr->sim.dt_control_q26;      // q0*q26 = q26
    motor_ptr->sim.filter_alpha_speed_q16 = calc_filter_alpha(motor_ptr->mpos.qep.filter_fc_speed_q16, dt_actual_q26);

    // Calculate speed conversion factors
    calculate_speed_coefficients(dt_actual_q26, motor_ptr->app.speed_scale_shift, &motor_ptr->sim.kspd_radps_q6, &motor_ptr->sim.kspd_scaled_q14);
}


/**
 * @brief This function use to initialize SIM
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_init(Struct_Motor* motor_ptr)
{
}


// JAGTAG: This may be converted to a generic function for all SIM methods

/**
 * @brief This function use to update SIM duty cycle
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_update_duty(Struct_Motor *motor_ptr)
{
    // Update phase voltage values
    motor_ptr->control.mod_va_q14 = motor_ptr->sim.braking_duty_cycle_q14;
    motor_ptr->control.mod_vb_q14 = motor_ptr->sim.braking_duty_cycle_q14;
    motor_ptr->control.mod_vc_q14 = motor_ptr->sim.braking_duty_cycle_q14;
}


/**
 * @brief This function use to detect motion in hall sensor mode
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_q_detect_motion(Struct_Motor *motor_ptr)
{
    int32_t delta_qep_counter = motor_ptr->mpos.qep.counter_counts - motor_ptr->sim.mdetect_qep_counter_start;
    int32_t delta_qepindex_counter = motor_ptr->mpos.qep.index_counter - motor_ptr->sim.mdetect_qepindex_counter_start;

    if ((delta_qep_counter > SIM_MDETECT_QEPCOUNTER_THRESHOLD_CNT) || (delta_qepindex_counter > 0))
    {
        motor_ptr->sim.sim_counter = 0;
        motor_ptr->sim.motion_detected = 1;
    }
    else if (++motor_ptr->sim.sim_counter >= motor_ptr->sim.sim_mdetect_countmax)
    {
        motor_ptr->sim.sim_counter = 0;
        motor_ptr->sim.motion_notdetected = 1;
    }
}


#if 1
// JAGTAG: This may be converted to a generic function for all SIM methods

/**
 * @brief This function use to ramp SIM braking duty cycle
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_q_ramp_duty(Struct_Motor *motor_ptr)
{
    // Decrement duty cycle
    motor_ptr->sim.braking_duty_cycle_q14 = motor_ptr->sim.braking_duty_cycle_q14 - motor_ptr->sim.sim_duty_cycle_dec_q14;

    // Apply duty min to duty ramp
    if (motor_ptr->sim.braking_duty_cycle_q14 < motor_ptr->sim.braking_duty_min_q14)
    {
        motor_ptr->sim.braking_duty_cycle_q14 = motor_ptr->sim.braking_duty_min_q14;
    }
}
#endif

#endif  //#ifdef ENABLE_SIM
