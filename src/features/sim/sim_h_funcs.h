
/**
 * @file sim_h_funcs.c
 * @brief Use for SIM hall utility functions
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

#ifndef SIM_H_FUNCS_H
#define SIM_H_FUNCS_H

#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "sim_h_defs.h"
#include "control_funcs.h"
#include "driver_funcs.h"
#include "utils.h"
#include "config_features.h"
#include "foc_math.h"
#include "hall_funcs.h"
#include "hall_sim2hall.h"
#include "foc_funcs.h"
#include "mod_svm.h"
#include "sim_config.h"
#include "foc_config.h"
#include "sim_sim2cl.h"

#if (MODULE_MPOS_LBG1 == ENABLED)
#include "est_sim2est.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
#include "mod_ovm.h"
#endif

#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_HALL)

//=====================================
// Configuration
//=====================================
#define BRAKING_DUTY_LIMIT_SEC_Q26                  QCONST(0.000003f, Q26)      // Calculated to center 3us around pwm center point

// Define the number of hall speed samples to reach 90%. (Values derived from spreadsheet.)
#if (CONFIG_HALLFILTER_SHIFT == 0)
    #define SIM_HALLFILTER_SETTLENUM    0
#elif (CONFIG_HALLFILTER_SHIFT == 1)
    #define SIM_HALLFILTER_SETTLENUM    4
#elif (CONFIG_HALLFILTER_SHIFT == 2)
    #define SIM_HALLFILTER_SETTLENUM    9
#elif (CONFIG_HALLFILTER_SHIFT == 3)
    #define SIM_HALLFILTER_SETTLENUM    18
#elif (CONFIG_HALLFILTER_SHIFT == 4)
    #define SIM_HALLFILTER_SETTLENUM    36
#elif (CONFIG_HALLFILTER_SHIFT == 5)
    #define SIM_HALLFILTER_SETTLENUM    73
#endif


//=====================================
// Function Declarations (sim_h_funcs.c)
//=====================================
extern void sim_calculate_parameters(Struct_Motor *motor_ptr);
extern void sim_init(Struct_Motor* motor_ptr);
extern void sim_update_duty(Struct_Motor *motor_ptr);
extern void sim_h_detect_motion(Struct_Motor *motor_ptr);
extern void sim_h_ramp_duty(Struct_Motor *motor_ptr);


//=====================================
// Inline Function Definitions
//=====================================

/**
 * @brief This function use to reset and initialize SIM variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_reset(Struct_Motor* motor_ptr)
{
    // Reset SIM flags
    motor_ptr->sim.status_sim = 0;
    motor_ptr->sim.signal_detected = 0;
    motor_ptr->sim.motion_detected = 0;
    motor_ptr->sim.motion_notdetected = 0;
    motor_ptr->sim.sim_motor_direction = motor_ptr->app.app_motor_direction_cmd;
    motor_ptr->sim.transition_complete = 0;
    motor_ptr->sim.flag_hall_state_int = FALSE;
    motor_ptr->sim.flag_hall_timer_int = FALSE;
    motor_ptr->sim.flag_hall_transition = FALSE;

    // Initialize SIM angle values (may not be necessary)
    motor_ptr->sim.sim_angle = motor_ptr->mpos.mpos_angle;
    motor_ptr->sim.sim_angle_previous = motor_ptr->mpos.mpos_angle;

    // Reset SIM transition variables
    motor_ptr->sim.angle_blend_cnt = SIM_ANGLE_BLEND_MAX_CNT;
    motor_ptr->sim.angle_inc_q16 = 0;
    motor_ptr->sim.angle_inc_per_pwmdt = 0;

    // Reset motion detect
    motor_ptr->sim.sim_counter = 0;

    hall_reset(motor_ptr);

    // Configure duty min/max
    // Duty max should be able to get to 100% duty in SIM-V
    motor_ptr->foc.svm_max_ticks = (motor_ptr->control.motor_period_ticks - PULSEWIDTH_MIN_LS_TICKS);               // Timer ticks in SVM max for this motor (for SVM)

    // Duty min cannot get to 0% in SIM-V because the voltage needs to get sampled during the LS-off time
    motor_ptr->foc.svm_min_ticks = PULSEWIDTH_MIN_HS_TICKS;                                                         // Timer ticks in SVM min for this motor (for SVM)
}


/**
 * @brief This function use for SIM motion detect
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_mdetect(Struct_Motor *motor_ptr)
{
    // Check hall transitions for detecting motion
    sim_h_detect_motion(motor_ptr);
}


static inline void sim_mdetect_true(Struct_Motor *motor_ptr)
{
}


/**
 * @brief This function use to detect hall angle and update hall states changes
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_detect2(Struct_Motor *motor_ptr)
{
    int32_t temp;

    motor_ptr->sim.sim_angle = motor_ptr->mpos.hall.hall_interpolated_angle;
    motor_ptr->sim.sim_spd_scaled_q28 = motor_ptr->mpos.hall.hall_spd_scaled_q28;
    motor_ptr->sim.sim_spd_scaled_q14 = motor_ptr->mpos.hall.hall_spd_scaled_q14;
    motor_ptr->sim.sim_spd_radps = motor_ptr->mpos.hall.hall_spd_radps;
    motor_ptr->sim.sim_motor_direction = motor_ptr->mpos.hall.hall_motor_direction;

    // Calculate Ke-based voltage vector and overwrite re-scaled SIM alpha/beta
    // JAGTAG: Can simplify using scaled speed?
    // Vphase = rad/s * Ke
    temp = fix16_mul(((motor_ptr->sim.sim_spd_radps * motor_ptr->sim.sim_motor_direction) >> 10), motor_ptr->mparam.ke_q26);       // mul(q16>>10=q6, q26) = q16
    // Vphase_scaled = Vphase * Vphase_scale
    motor_ptr->sim.ve_mag_scaled_q14 = (fix16_mul(temp, motor_ptr->app.descale_voltage_factor_1div_q16) >> 2);       // mul(q16, q16) = q16 >>2 = q14

    // Generate SIM alpha/beta with Ke magnitude
    inverse_park_transform_q14(motor_ptr->sim.sim_angle, 0, motor_ptr->sim.ve_mag_scaled_q14, &motor_ptr->sim.sim_alpha_q14, &motor_ptr->sim.sim_beta_q14);
    motor_ptr->sim.sim_alpha_q14 = motor_ptr->sim.sim_alpha_q14 * motor_ptr->sim.sim_motor_direction;
    motor_ptr->sim.sim_beta_q14 = motor_ptr->sim.sim_beta_q14 * motor_ptr->sim.sim_motor_direction;

    // Keep track of Detect2 time using hall transitions
    if (motor_ptr->mpos.hall.transition_ext_flag)
    {
        motor_ptr->sim.sim_counter++;
    }
}


/**
 * @brief This function use to copy hall related variables for SIM to CL
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_sim2cl(Struct_Motor *motor_ptr)
{
    int32_t pwm_counter;

    // Add control overflow time from previous sim2cl cycle to hall interpolation
    pwm_counter = (motor_ptr->sim.ctrl_counter_sim2cl - motor_ptr->control.ctrl_divider);
    if (pwm_counter < 0)
        pwm_counter = 0;
    motor_ptr->mpos.hall.accumulate_degree_value += (motor_ptr->sim.angle_inc_per_pwmdt * pwm_counter);

    // SIM2CL state machine
    sim_sim2cl_sm(motor_ptr);

    // Update SIM operating point
    motor_ptr->sim.sim_angle = motor_ptr->mpos.hall.hall_interpolated_angle;

    // Generate SIM alpha/beta with Ke magnitude
    inverse_park_transform_q14(motor_ptr->sim.sim_angle, 0, motor_ptr->sim.ve_mag_scaled_q14, &motor_ptr->sim.sim_alpha_q14, &motor_ptr->sim.sim_beta_q14);
    motor_ptr->sim.sim_alpha_q14 = motor_ptr->sim.sim_alpha_q14 * motor_ptr->sim.sim_motor_direction;
    motor_ptr->sim.sim_beta_q14 = motor_ptr->sim.sim_beta_q14 * motor_ptr->sim.sim_motor_direction;

    // Generate FOC Valpha/beta
    inverse_park_transform_q14(motor_ptr->sim.sim_angle, motor_ptr->foc.vd_q14, motor_ptr->foc.vq_q14, &motor_ptr->foc.valpha_q14, &motor_ptr->foc.vbeta_q14);

    // Generate SVM modulation vectors
    mod_process_svm(motor_ptr->foc.valpha_q14, motor_ptr->foc.vbeta_q14,
            &motor_ptr->control.mod_sector_sreg, &motor_ptr->control.mod_va_q14, &motor_ptr->control.mod_vb_q14, &motor_ptr->control.mod_vc_q14);
}


/**
 * @brief This function use for SIM brake reset
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_brake_reset(Struct_Motor* motor_ptr)
{
    // Reset motion flag
    motor_ptr->sim.motion_notdetected = 0;

    // Reset motion detect
    motor_ptr->sim.braking_duty_cycle_q14 = QCONST(1.0, Q14);

    // Define motion detection duty cycle decrement
    motor_ptr->sim.sim_duty_cycle_dec_q14 = motor_ptr->sim.braking_duty_cycle_dec_q14;

    // Initialize Phase voltages before enabling LS
    motor_ptr->control.mod_va_q14 = QCONST(1.0, Q14);
    motor_ptr->control.mod_vb_q14 = QCONST(1.0, Q14);
    motor_ptr->control.mod_vc_q14 = QCONST(1.0, Q14);
}


/**
 * @brief This function use for SIM hall detect and braking
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_detect_brakestop(Struct_Motor *motor_ptr)
{
    // Check hall transitions for detecting motion
    sim_h_detect_motion(motor_ptr);

    // Increment braking duty
    sim_h_ramp_duty(motor_ptr);

    // Update braking duty cycle
    sim_update_duty(motor_ptr);
}


/**
 * @brief This function use to initialize SIM brake related variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_waitstop_reset(Struct_Motor* motor_ptr)
{
    // Reset brake counter and flag
    motor_ptr->sim.sim_counter = 0;
    motor_ptr->sim.motion_notdetected = 0;
}


/**
 * @brief This function use to detect motion
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_detect_waitstop(Struct_Motor *motor_ptr)
{
    // Check hall transitions for detecting motion
    sim_h_detect_motion(motor_ptr);
}


/**
 * @brief This function use for SIM exit
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_exit_main(Struct_Motor* motor_ptr)
{
}


/**
 * @brief This function use to set SIM transition flag, generic function
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_check_transition_ready(Struct_Motor *motor_ptr)
{
    motor_ptr->sim.transition_complete = 1;
}


/**
 * @brief This function use to set variables for SIM to Close Loop switch over
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_sim2cl_transition_main(Struct_Motor *motor_ptr)
{
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    if (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST)
    {
        // Check is estimator transition is complete
        est_sim2est_main(motor_ptr);

        // Check if SIM transition is complete
        sim_check_transition_ready(motor_ptr);

        // Next state if transitions are complete
        if (motor_ptr->mpos.est.est_transition_complete && motor_ptr->sim.transition_complete)
            motor_ptr->sim.transition_complete = TRUE;
    }
    else if (motor_ptr->mpos.mposmode == STATE_MPOSMODE_HALL)
    {
        hall_sim2hall_main(motor_ptr);
    }
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    hall_sim2hall_main(motor_ptr);
#endif
}


/**
 * @brief This function use to transition from SIM to Close Loop
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void sim_sim2cl_transition_exit_main(Struct_Motor *motor_ptr)
{
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    if (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST)
    {
        motor_ptr->mpos.state = STATE_MPOS_ESTIMATED_ANGLE;
    }
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
#endif
}

#endif  //#if defined(ENABLE_SIM)
#endif  //#ifndef SIM_H_FUNCS_H
