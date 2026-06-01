
/**
* @file sim_i_funcs.h
* @brief I-SIM function declarations
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

#ifndef SIM_I_FUNCS_H
#define SIM_I_FUNCS_H

#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "sim_i_defs.h"
#include "control_funcs.h"
#include "driver_funcs.h"
#include "utils.h"
#include "config_features.h"
#include "foc_math.h"
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


#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)

//=====================================
// Configuration
//=====================================
#define BRAKING_DUTY_LIMIT_SEC_Q26                  QCONST(0.000000f, Q26)      // Calculated to center 3us around pwm center point


//=====================================
// Compile Warnings
//=====================================
#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    #error "I-based SIM is not an option in 1-shunt mode"
#endif


//=====================================
// SIM Library Function Declarations
//=====================================

/**
* @brief Detect if motor has stopped during SIM braking
* @param Struct_SIM* sim_ptr
*/
extern void sim_i_detect_brakestop(Struct_SIM* sim_ptr);


/**
* @brief Detect if motor has stopped while SIM waits for motor to stop
* @param Struct_SIM* sim_ptr
*/
extern void sim_i_detect_waitstop(Struct_SIM* sim_ptr);


/**
* @brief Detect is motor is in motion during SIM
* @param Struct_SIM* sim_ptr
*/
extern void sim_i_mdetect(Struct_SIM *sim_ptr);


/**
* @brief Executes functions performed during SIM DETECT2 state
* @param Struct_SIM* sim_ptr
*/
extern void sim_i_detect2(Struct_SIM *sim_ptr);


//=====================================
// Function Declarations (sim_i_funcs.c)
//=====================================

/**
* @brief Calculates all SIM parameters based on user configuration
* @param Struct_Motor* motor_ptr
*/
extern void sim_calculate_parameters(Struct_Motor *motor_ptr);


/**
* @brief Initializes SIM variables at powerup
* @param Struct_Motor* motor_ptr
*/
extern void sim_init(Struct_Motor* motor_ptr);


/**
* @brief Updates SIM braking duty cycle
* @param Struct_Motor* motor_ptr
*/
extern void sim_update_duty(Struct_Motor *motor_ptr);


//=====================================
// ADC Sequencer Readout Macro Functions
//=====================================

/**
* @brief Returns the U-phase current used for SIM detection
* @return int16_t readseq_iphase_u
*/
static inline int16_t readseq_sim_iphase_u(void)               { return readseq_iphase_u(); }


/**
* @brief Returns the V-phase current used for SIM detection
* @return readseq_iphase_v
*/
static inline int16_t readseq_sim_iphase_v(void)               { return readseq_iphase_v(); }


/**
* @brief Returns the W-phase current used for SIM detection
* @return readseq_iphase_w
*/
static inline int16_t readseq_sim_iphase_w(void)               { return readseq_iphase_w(); }


//=====================================
// Inline Function Definitions
//=====================================

static inline void sim_reset(Struct_Motor* motor_ptr)
{
    // Reset SIM flags
    motor_ptr->sim.status_sim = 0;
    motor_ptr->sim.signal_detected = 0;
    motor_ptr->sim.motion_detected = 0;
    motor_ptr->sim.motion_notdetected = 0;
    motor_ptr->sim.sim_motor_direction = motor_ptr->app.app_motor_direction_cmd;
    motor_ptr->sim.transition_complete = 0;

    // Initialize SIM angle values mid-point in order to reduce speed convergence
    motor_ptr->sim.sim_angle = 0;
    motor_ptr->sim.sim_angle_previous = 0;

    // Reset SIM transition variables
    motor_ptr->sim.angle_blend_cnt = SIM_ANGLE_BLEND_MAX_CNT;
    motor_ptr->sim.angle_inc_q16 = 0;
    motor_ptr->sim.angle_inc_per_pwmdt = 0;

    // Reset motion detect
    motor_ptr->sim.braking_duty_cycle_q14 = QCONST(0.8, Q14);   // Cannot start at 100% duty, starting at 90%
    motor_ptr->sim.sim_counter = (motor_ptr->sim.sim_mdetect_countmax >> 1);
    motor_ptr->sim.sim_duty_cycle_dec_q14 = motor_ptr->sim.mdetect_duty_cycle_dec_q14;

    // Initialize Phase voltages before enabling LS
    motor_ptr->control.mod_va_q14 = QCONST(1.0, Q14);
    motor_ptr->control.mod_vb_q14 = QCONST(1.0, Q14);
    motor_ptr->control.mod_vc_q14 = QCONST(1.0, Q14);

    // Duty max cannot get to 100% duty in SIM-I because current can only be measured during LS-on time
    motor_ptr->foc.svm_max_ticks = (motor_ptr->control.motor_period_ticks - PULSEWIDTH_MIN_LS_TICKS);               // Timer ticks in SVM max for this motor (for SVM)

    // Duty min can get to 0% duty in SIM-I except in PAC55xx ticks cannot equal 0 due to duty cycle bug
#if defined(FAMILY_55XX)
    motor_ptr->foc.svm_min_ticks = 1;                                                                               // Timer ticks in SVM min for this motor (for SVM)
#else
    motor_ptr->foc.svm_min_ticks = 0;                                                                               // Timer ticks in SVM min for this motor (for SVM)
#endif
}


/**
* @brief Resets variables for SIM braking
* @param Struct_Motor* motor_ptr
*/
static inline void sim_brake_reset(Struct_Motor* motor_ptr)
{
    // Reset motion flag
    motor_ptr->sim.motion_notdetected = 0;

    // Define motion detection duty cycle decrement
    motor_ptr->sim.sim_duty_cycle_dec_q14 = motor_ptr->sim.braking_duty_cycle_dec_q14;

    // Duty cycle is not reset here so that braking starts at the same duty that mdetect left off.  Is is reset instead  in sim_reset()
}


/**
* @brief Resets variables before waiting to stop during SIM
* @param Struct_Motor* motor_ptr
*/
static inline void sim_waitstop_reset(Struct_Motor* motor_ptr)
{
    // Reset brake counter and flag
    motor_ptr->sim.sim_counter = (motor_ptr->sim.sim_mdetect_countmax >> 1);
    motor_ptr->sim.motion_notdetected = 0;
}


/**
* @brief Reads inputs used by SIM
* @param Struct_Motor* motor_ptr
*/
static inline void sim_read_inputs(Struct_Motor *motor_ptr)
{
    int32_t itemp;

    // Read ADC value and calculate scaled phase current

    // Convert to scaled current referenced to 0 (Range = [0, +1])
    itemp = ((readseq_sim_iphase_u() * ADC_SCALE_CONST) >> 14);           //0q14 = "0q14" * 5q16 = 5q30>>16
    // Subtract offset
    itemp = (-(itemp - motor_ptr->threeshunt.adc_offset_iphase_u_q14) << 1);
    // Compensate diffamp gain error
    itemp = ((itemp * COMP_DIFFGAINERROR_Q14) >> 14);                   // q14 = 0q14 * 1q14 = 1q28>>14
    // Result Phase U
    motor_ptr->sim.iphase_u_q14 = itemp;

    // Convert to scaled current referenced to 0 (Range = [0, +1])
    itemp = ((readseq_sim_iphase_v() * ADC_SCALE_CONST) >> 14);           //0q14 = "0q14" * 5q16 = 5q30>>16
    // Subtract offset
    itemp = (-(itemp - motor_ptr->threeshunt.adc_offset_iphase_v_q14) << 1);
    // Compensate diffamp gain error
    itemp = ((itemp * COMP_DIFFGAINERROR_Q14) >> 14);                   // q14 = 0q14 * 1q14 = 1q28>>14
    // Result Phase V
    motor_ptr->sim.iphase_v_q14 = itemp;

    // Convert to scaled current referenced to 0 (Range = [0, +1])
    itemp = ((readseq_sim_iphase_w() * ADC_SCALE_CONST) >> 14);           //0q14 = "0q14" * 5q16 = 5q30>>16

    // Subtract offset
    itemp = (-(itemp - motor_ptr->threeshunt.adc_offset_iphase_w_q14) << 1);
    // Compensate diffamp gain error
    itemp = ((itemp * COMP_DIFFGAINERROR_Q14) >> 14);                   // q14 = 0q14 * 1q14 = 1q28>>14
    // Result Phase W
    motor_ptr->sim.iphase_w_q14 = itemp;
}


/**
* @brief Detect is motor is in motion during SIM
* @param Struct_SIM* sim_ptr
*/
static inline void sim_mdetect(Struct_Motor *motor_ptr)
{
    int32_t sampling_point;

    sim_i_mdetect(&motor_ptr->sim);

    // Update phase voltage values
    sim_update_duty(motor_ptr);

    // Update sampling point to follow duty cycle
    sampling_point = motor_ptr->control.u_duty_value + SIM_MDETECT_SAMPLING_ADVANCE_TICKS;
    if (sampling_point > (motor_ptr->control.motor_period_ticks - 4))
        sampling_point = motor_ptr->control.motor_period_ticks - 4;
    pac5xxx_timer_a_ccctr0_value_set(sampling_point);            // Set value for AS0 trigger
}


static inline void sim_mdetect_true(Struct_Motor *motor_ptr)
{
}


/**
* @brief Executes functions performed during SIM DETECT2 state
* @param Struct_SIM* sim_ptr
*/
static inline void sim_detect2(Struct_Motor *motor_ptr)
{
    int32_t sampling_point;
    int32_t temp;

    sim_i_detect2(&motor_ptr->sim);

    // Update phase voltage values
    sim_update_duty(motor_ptr);

    // Update sampling point to follow duty cycle
    sampling_point = motor_ptr->control.u_duty_value + SIM_MDETECT_SAMPLING_ADVANCE_TICKS;
    if (sampling_point > (motor_ptr->control.motor_period_ticks - 4))
        sampling_point = motor_ptr->control.motor_period_ticks - 4;
    pac5xxx_timer_a_ccctr0_value_set(sampling_point);            // Set value for AS0 trigger

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

    // Increment SIM counter to keep track of Detect2 time
    motor_ptr->sim.sim_counter++;

#if 1
    // Calculate angle from control period delay (Need to check this holds for different control periods(freq and div))
    temp = ((fix16_mul((motor_ptr->sim.dt_control_q26 >> 0), motor_ptr->sim.sim_spd_radps) >> 10) * motor_ptr->app.app_motor_direction);
    motor_ptr->sim.sim_angle += temp;
    wraparound(&motor_ptr->sim.sim_angle);
#endif
}


static inline void sim_sim2cl(Struct_Motor *motor_ptr)
{
    int32_t pwm_counter;

    // Add control overflow time from previous sim2cl cycle to hall interpolation
    pwm_counter = (motor_ptr->sim.ctrl_counter_sim2cl - motor_ptr->control.ctrl_divider);
    if (pwm_counter < 0)
        pwm_counter = 0;
    motor_ptr->sim.sim_angle += (motor_ptr->sim.angle_inc_per_pwmdt * pwm_counter);
    wraparound(&motor_ptr->sim.sim_angle);

    // SIM2CL state machine
    sim_sim2cl_sm(motor_ptr);

    // Update SIM operating point
    motor_ptr->sim.sim_angle += motor_ptr->sim.angle_inc_q16;
    wraparound(&motor_ptr->sim.sim_angle);

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


static inline void sim_detect_brakestop(Struct_Motor *motor_ptr)
{
    sim_i_detect_brakestop(&motor_ptr->sim);

    sim_update_duty(motor_ptr);
}


static inline void sim_detect_waitstop(Struct_Motor *motor_ptr)
{
    sim_i_detect_waitstop(&motor_ptr->sim);

    sim_update_duty(motor_ptr);
}


static inline void sim_exit_main(Struct_Motor* motor_ptr)
{
}


static inline void sim_check_transition_ready(Struct_Motor *motor_ptr)
{
#if defined(ENABLE_SIMANGLEBLEND)
    if (motor_ptr->sim.status_sim & STATUS_SIM_SWITCH2CL)
    {
        if (motor_ptr->sim.angle_blend_cnt == 0)
        {
            motor_ptr->sim.transition_complete = 1;
        }
    }
    else
    {
        motor_ptr->sim.transition_complete = 1;
    }
#else
    motor_ptr->sim.transition_complete = 1;
#endif
}


static inline void sim_sim2cl_transition_main(Struct_Motor *motor_ptr)
{
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    // Check is estimator transition is complete
    est_sim2est_main(motor_ptr);

    // Check if SIM transition is complete
    sim_check_transition_ready(motor_ptr);

    // Next state if transitions are complete
    if (motor_ptr->mpos.est.est_transition_complete && motor_ptr->sim.transition_complete)
        motor_ptr->sim.transition_complete = TRUE;
#endif
}


static inline void sim_sim2cl_transition_exit_main(Struct_Motor *motor_ptr)
{
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    motor_ptr->mpos.state = STATE_MPOS_ESTIMATED_ANGLE;
#endif
}

#endif  //#if defined(ENABLE_SIM)
#endif  //#ifndef SIM_I_FUNCS_H
