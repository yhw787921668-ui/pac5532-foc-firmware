/**
 * @file lbg1_funcs.h
 * @brief Estimator utility functions
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

#ifndef LBG1_FUNCS_H
#define LBG1_FUNCS_H

#include "motor_struct.h"
#include "control_funcs.h"
#include "param_funcs.h"
#include "lbg1.h"
#include "mpos_misc.h"
#ifdef ENABLE_PLL_ANGLE_CALCULATION
#include "lbg1_pll.h"
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)


//////////////////////////////////////////
// Constant Definitions
//////////////////////////////////////////
#define LBG_CHK_THRESHOLD_CURRENT_SQ_Q28    QCONST(0.0009f, Q28)
#define LBG_CHK_THRESHOLD_VOLTAGE_SQ_Q28    QCONST(0.0009f, Q28)
#define LBG_VALID_CHECK_COUNT_MAX           5


//////////////////////////////////////////
// Function Declarations
//////////////////////////////////////////
extern void lbg1_process_voltage_scale(Struct_Motor* motor_ptr);
extern void lbg1_calculate_coefficients(Struct_Motor* motor_ptr);
extern void lbg1_reset(Struct_Motor* motor_ptr);
extern void lbg1_statemon(Struct_Motor* motor_ptr);
extern void lbg1_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
extern void lbg1_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
#ifdef ENABLE_PLL_ANGLE_CALCULATION
extern void lbg1_pll_init(Struct_Motor* motor_ptr, int32_t speed_q16, int32_t angle_q16);
#endif

//////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////

/**
 * @brief Use for preparing estimator inputs for angle and speed calculation
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void lbg1_prepare_inputs(Struct_Motor* motor_ptr)
{
    // Pass current values
    motor_ptr->mpos.est.ialpha1_q14 = motor_ptr->foc.ialpha_q14;
    motor_ptr->mpos.est.ibeta1_q14 = motor_ptr->foc.ibeta_q14;

    // Pass voltage values from shift register
    motor_ptr->mpos.est.valpha1_q14 = motor_ptr->mpos.est.valpha_sreg0_q14;
    motor_ptr->mpos.est.vbeta1_q14 = motor_ptr->mpos.est.vbeta_sreg0_q14;

    // Shift register so that estimator uses Valpha/beta from 1 skipped control cycle
    motor_ptr->mpos.est.valpha_sreg0_q14 = motor_ptr->foc.valpha_q14;
    motor_ptr->mpos.est.vbeta_sreg0_q14 = motor_ptr->foc.vbeta_q14;

#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    motor_ptr->mpos.est.est_motor_direction = motor_ptr->mpos.hall.hall_motor_direction;
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
    motor_ptr->mpos.est.est_motor_direction = motor_ptr->mpos.qep.qep_motor_direction;
#else
    motor_ptr->mpos.est.est_motor_direction = motor_ptr->app.app_motor_direction;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#if defined(ENABLE_ANGLE_CORRECTION) || defined(ENABLE_PHASE_COMPENSATION) || defined(ENABLE_MTPA)
/**
 * @brief Applies angle correction to the lbg1 angle
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void lbg1_angle_correction(Struct_Motor* motor_ptr)
{
    fix16_t temp2 = 0;

#if defined(ENABLE_ANGLE_CORRECTION)
    temp2 += motor_ptr->mpos.est.angcorr_offset * motor_ptr->app.app_motor_direction;
    temp2 += (((motor_ptr->mpos.est.est_spd_radps >> 12) * (motor_ptr->mpos.est.angcorr_gain_q20 >> 0)) >> 8);        // 1/(2*2*pi*f0)    q16>>12=q4 * q20 = q24>>8=q16
#endif
#if defined(ENABLE_PHASE_COMPENSATION)
    #ifdef ENABLE_SWITCH_OVER_SPEED_BLEND
    temp2 += (((motor_ptr->mpos.est.blend_spd_radps >> 12) * (motor_ptr->mpos.est.phscomp_multp_q20 >> 0)) >> 8);       // 1/(2*2*pi*f0)    q16>>12=q4 * q20 = q24>>8=q16
    #else
    temp2 += (((motor_ptr->mpos.est.est_spd_radps >> 12) * (motor_ptr->mpos.est.phscomp_multp_q20 >> 0)) >> 8);       // 1/(2*2*pi*f0)    q16>>12=q4 * q20 = q24>>8=q16
    #endif
#endif
#if defined(ENABLE_MTPA)
    temp2 += motor_ptr->mtpa.mtpa_angle_rad_q16 * motor_ptr->app.app_motor_direction;
#endif
#if defined(ENABLE_EST_ASSIST)
    temp2 += motor_ptr->mpos.estassist_angle_corr;
#endif
#if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
    temp2 += motor_ptr->startup.aligngo2est_angleramp_q16;
#endif

// For smooth transition from Trap to estimator
#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    if ((motor_ptr->mpos.mposmode == STATE_MPOSMODE_TRAP2EST) || (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST))
    {
        temp2 += motor_ptr->mpos.trap.trap_est_angle_diff;
    }
    if (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST)
    {
        // Ramp down by 0.1 degree (0.00175 rad) per dt
        GENERIC_RAMP(motor_ptr->mpos.trap.trap_est_angle_diff, 0, QCONST(0.00175f, Q16));
    }
#endif
    
    motor_ptr->mpos.est.estimated_angle = motor_ptr->mpos.est.calculated_angle + temp2;
    wraparound(&motor_ptr->mpos.est.estimated_angle);

    // Debug for reading phase compensation in degrees
//    debug_var_6 = ((((temp2 >> 4) * (FIX16_1P0_DIV_2_PI >> 4)) >> 8) * QCONST(360.0f, Q08)) >> 8;          // q16>>4=q12 * q16>>4=q12 = q24>>8=q16 * q8 = q24>>8
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif


/**
 * @brief Use for updating estimator angle and speed related variables
 * @param Struct_Motor* motor_ptr
 * @param fix16_t update_angle_q16
 * @param fix16_t update_speed_radps_q16
 */
static inline void lbg1_update_state(Struct_Motor* motor_ptr, fix16_t update_angle_q16, fix16_t update_speed_radps_q16)
{
    lbg1_update_state_lib(&motor_ptr->mpos.est, update_angle_q16, update_speed_radps_q16, motor_ptr->app.speed_scale_shift, motor_ptr->control.dt_control_q26);
}



/**
 * @brief Use for running complete estimator angle and speed calculation
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_RAMFUNC void lbg1_process(Struct_Motor* motor_ptr);

#endif  //#if (MODULE_MPOS_LBG1 == ENABLED)
#endif  //#ifndef LBG1_FUNCS_H
