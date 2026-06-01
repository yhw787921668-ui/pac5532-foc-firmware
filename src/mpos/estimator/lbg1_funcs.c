
/**
 * @file lbg1_funcs.c
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

#include "config_app.h"
#include "lbg1_funcs.h"
#include "fault.h"


#if (MODULE_MPOS_LBG1 == ENABLED)

/**
 * @brief Use for running complete estimator angle and speed calculation
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC void lbg1_process(Struct_Motor* motor_ptr)
{
    lbg1_prepare_inputs(motor_ptr);

#ifdef ENABLE_PLL_ANGLE_CALCULATION
    lbg1_estimator_pll_lib(&motor_ptr->mpos.est, &motor_ptr->mpos.est_pll);
#else
    lbg1_estimator_lib(&motor_ptr->mpos.est);
#endif

#if defined(ENABLE_ANGLE_CORRECTION) || defined(ENABLE_PHASE_COMPENSATION) || defined(ENABLE_MTPA)
    lbg1_angle_correction(motor_ptr);
#else
    motor_ptr->mpos.est.estimated_angle = motor_ptr->mpos.est.calculated_angle;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief Use for scaling estimator gs and kpe variables
 * @param Struct_Motor* motor_ptr
 */
void lbg1_process_voltage_scale(Struct_Motor* motor_ptr)
{
    lbg1_process_voltage_scale_lib(&motor_ptr->mpos.est, motor_ptr->app.descale_voltage_factor);
}


/**
 * @brief Use for scaling estimator gs and kpe variables
 * @param Struct_Motor* motor_ptr
 */
void lbg1_calculate_coefficients(Struct_Motor* motor_ptr)
{
#ifdef ENABLE_PLL_ANGLE_CALCULATION
    // Calculate PLL coefficients
    lbg1_pll_calculate_coefficients_lib(&motor_ptr->mpos.est, &motor_ptr->mpos.est_pll, motor_ptr->control.dt_control_q26, motor_ptr->app.descale_current_factor, motor_ptr->mparam.rs_q16,
            motor_ptr->mparam.lq_q26, motor_ptr->app.speed_scale_shift, LBG_GAIN_SCALER_Q06, motor_ptr->mparam.nom_spd_q16);
#else
    lbg1_calculate_coefficients_lib(&motor_ptr->mpos.est, motor_ptr->control.dt_control_q26, motor_ptr->app.descale_current_factor, motor_ptr->mparam.rs_q16,
            motor_ptr->mparam.lq_q26, motor_ptr->app.speed_scale_shift, LBG_GAIN_SCALER_Q06, motor_ptr->mparam.nom_spd_q16);
#endif

    // Set flag to indicate calculation is complete so that GUI can read it automatically
    set_status(FLAG_MPOS_UPDATE_CALC);

    // Set or clear warning for high estimator bandwidth
    if (motor_ptr->mpos.est.flag_estbw_high)
        set_warning(WARNING_ESTBW_HIGH);
    else
        clear_warning(WARNING_ESTBW_HIGH);
}


/**
 * @brief Use to initialize and reset estimator variables
 * @param Struct_Motor* motor_ptr
 */
// JAGTAG: Rename lbg1_reset and lbg1_init functions
void lbg1_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.est.starting_w0_q16 = LBG_KGAIN_START_PCT_F16;
    motor_ptr->mpos.est.w0_inc_pct_q16  = LBG_KGAIN_INC_PCT_F16;

#ifdef ENABLE_PLL_ANGLE_CALCULATION
    if (motor_ptr->mparam.nom_spd_q16 > 0)
    {
        motor_ptr->mpos.est_pll.nom_spd_1_div_q26 = fix16_div(QCONST(1.0f, Q26), motor_ptr->mparam.nom_spd_q16);
    }
    motor_ptr->mpos.est_pll.pll_dampingfactor_q16 = LBG_PLL_DAMPING_FACTOR_Q16;
    motor_ptr->mpos.est_pll.pll_gain_scaling_factor_q16 = LBG_PLL_KGAIN_START_PCT_F16;
    motor_ptr->mpos.est_pll.pll_gain_inc_pct_q16 = LBG_PLL_KGAIN_INC_PCT_F16;
    motor_ptr->mpos.est_pll.amplitude_abs_threshold_q16 = QCONST(0.03f, Q16);    // To be moved to config_app.h
    // Initialize the PLL speed and angle
    #ifdef ENABLE_IPD
    lbg1_pll_init(motor_ptr, 0, (motor_ptr->ipd.ipd_starting_angle >> 10));
    #else
    lbg1_pll_init(motor_ptr, 0, FIX16_PI_DIV_2);
    #endif

    // Calculate PLL PI gains
    lbg1_pll_calculate_pigain_lib(&motor_ptr->mpos.est_pll, motor_ptr->control.dt_control_q26, motor_ptr->mparam.nom_spd_q16);
#endif

    lbg1_init(&motor_ptr->mpos.est, motor_ptr->mparam.lq_q26, motor_ptr->mparam.rs_q16, motor_ptr->control.dt_control_q26, motor_ptr->app.descale_current_factor, motor_ptr->mparam.nom_spd_q16);

    motor_ptr->mpos.est.angle_diff_timeout_counter = 0;

    motor_ptr->mpos.est.lbg_valid_fault_flag = 0;
    motor_ptr->mpos.est.lbg_valid_check_counter = 0;
}


/**
 * @brief Use for estimator variable ramps and ialpha, valpha calculation
 * @param Struct_Motor* motor_ptr
 */
void lbg1_statemon(Struct_Motor* motor_ptr)
{
    int32_t ialpha_sq_q28;
    int32_t valpha_sq_q28;

    lbg1_statemon_lib(&motor_ptr->mpos.est, motor_ptr->mparam.lq_q26, motor_ptr->mparam.rs_q16, motor_ptr->control.dt_control_q26, motor_ptr->app.descale_current_factor, motor_ptr->mparam.nom_spd_q16);

#ifdef ENABLE_PLL_ANGLE_CALCULATION
    if (motor_ptr->mpos.est.est_transition_complete)
    {
        // Ramp PLL gains to target after switching over to close loop
        GENERIC_RAMP(motor_ptr->mpos.est_pll.pll_pid.ka_q16, motor_ptr->mpos.est_pll.pll_ka_target_q16, motor_ptr->mpos.est_pll.pll_gain_increment_ka);
        GENERIC_RAMP(motor_ptr->mpos.est_pll.pll_pid.kb_q16, motor_ptr->mpos.est_pll.pll_kb_target_q16, motor_ptr->mpos.est_pll.pll_gain_increment_kb);
        // PLL gain converter
        picon_convert_gain(&motor_ptr->mpos.est_pll.pll_pid, QCONST(1, Q16), QCONST(1, Q16));
        if (motor_ptr->mpos.est_pll.pll_pid.ka_q16 == motor_ptr->mpos.est_pll.pll_ka_target_q16) 
            motor_ptr->mpos.est_pll.est_pll_transition_complete = TRUE;
        else
            motor_ptr->mpos.est_pll.est_pll_transition_complete = FALSE;
    }
    else    motor_ptr->mpos.est_pll.est_pll_transition_complete = FALSE;
#endif

//    debug_dac_1a = (motor_ptr->mpos.est.w0_target_q06 >> 6);
//    debug_dac_1b = (motor_ptr->mpos.est.w0_ramp_q06 >> 6);

//    debug_dac_2a = motor_ptr->mpos.est.kpe_const_q14;
//    debug_dac_2b = motor_ptr->mpos.est.kpi_const_q14;

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    if (motor_ptr->state.main == STATE_MAIN_RUN)
#elif (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_LBG1_START_QEP) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    if ((motor_ptr->state.main == STATE_MAIN_RUN) && (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST))
#endif
    {
        // Check that estimator inputs are valid
        ialpha_sq_q28 = ((motor_ptr->mpos.est.ialpha1_q14 * motor_ptr->mpos.est.ialpha1_q14) + (motor_ptr->mpos.est.ibeta1_q14 * motor_ptr->mpos.est.ibeta1_q14));
        valpha_sq_q28 = ((motor_ptr->mpos.est.valpha1_q14 * motor_ptr->mpos.est.valpha1_q14) + (motor_ptr->mpos.est.vbeta1_q14 * motor_ptr->mpos.est.vbeta1_q14));

        if ((ialpha_sq_q28 < LBG_CHK_THRESHOLD_CURRENT_SQ_Q28) && (valpha_sq_q28 < LBG_CHK_THRESHOLD_VOLTAGE_SQ_Q28))
        {
            if (motor_ptr->mpos.est.lbg_valid_check_counter++ > LBG_VALID_CHECK_COUNT_MAX)
            {
                motor_ptr->mpos.est.lbg_valid_fault_flag = 1;
                set_fault(FAULT_EST_INVALID_INPUT);
            }
        }
        else
        {
            if (motor_ptr->mpos.est.lbg_valid_check_counter-- < 0)
            {
                motor_ptr->mpos.est.lbg_valid_check_counter = 0;
                motor_ptr->mpos.est.lbg_valid_fault_flag = 0;
            }

        }
    }

//    debug_var_1 = motor_ptr->mpos.est.lbg_valid_fault_flag << 16;
//
//    debug_dac_1a = LBG_CHK_THRESHOLD_CURRENT_SQ_Q28 >> 14;
//    debug_dac_1b = ialpha_sq_q28 >> 14;
//    debug_dac_2a = LBG_CHK_THRESHOLD_VOLTAGE_SQ_Q28 >> 14;
//    debug_dac_2b = valpha_sq_q28 >> 14;
}

#ifdef ENABLE_PLL_ANGLE_CALCULATION
/**
 * @brief Use to initialize and reset pll variables
 * @param Struct_Motor* motor_ptr, initial speed and angle
 */
void lbg1_pll_init(Struct_Motor* motor_ptr, int32_t speed_q16, int32_t angle_q16)
{
    int32_t temp_pll_speed_q14;
    // Initialize PLL Controller
    temp_pll_speed_q14 = ((speed_q16 >> 8) * (motor_ptr->mpos.est_pll.nom_spd_1_div_q26 >> 6)) >> 14; // Q16->8 = Q8 * Q20 = Q28->14 = Q14
    init_pi(&motor_ptr->mpos.est_pll.pll_pid, temp_pll_speed_q14);

    // Initialize PLL angle
    motor_ptr->mpos.est_pll.pll_estimated_angle_q16 = angle_q16 + FIX16_PI;
    wraparound(&motor_ptr->mpos.est_pll.pll_estimated_angle_q16);
}
#endif

/**
 * @brief Use for writing estimator parameters
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void lbg1_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Parameter process handler
    switch(param_code)
    {
    case LBG1_BandWidth:
        motor_ptr->mpos.est.w0_q06 = rx_data >> 10;
        motor_ptr->mpos.est.w0_target_q06 = motor_ptr->mpos.est.w0_q06;
//        debugio1_toggle();
        // JAGTAG: This is getting sent 1x on WAll when QEP+EST
        break;
    case LBG1_DampingFactor:
        motor_ptr->mpos.est.dampfactor_q16 = rx_data;
//        debugio1_toggle();
        break;
    case LBG1_fc_speed:
        motor_ptr->mpos.est.filter_fc_speed_q16 = rx_data;
        #ifdef ENABLE_PLL_ANGLE_CALCULATION
        motor_ptr->mpos.est_pll.pll_bandwidth_q16 = rx_data;
        #endif
//        debugio1_toggle();
        // JAGTAG: This is getting sent 3x on WAll when QEP+EST
        break;
#ifdef ENABLE_PLL_ANGLE_CALCULATION
    case LBG1_fc_bemf_mag:
        motor_ptr->mpos.est_pll.pll_amplitude_fc_q16 = rx_data;
        break;
#endif
    case angle_correction_offset:
        motor_ptr->mpos.est.angcorr_offset = rx_data;
//        debugio1_toggle();
        break;
    case angle_correction_factor:
        // Value sent by GUI in Q20 format
        motor_ptr->mpos.est.angcorr_gain_q20 = rx_data;
//        debugio1_toggle();
        break;

    default:
        break;
    }
}


/**
 * @brief Use for calculating estimator coefficients
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void lbg1_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Parameter cases that require recalculation of estimator coefficients
    switch(param_code)
    {
    case dt:
    case diff_amp_gain:
    case Lq:
    case Ralpha:
    case NominalSpeed:
    case LBG1_BandWidth:
    case LBG1_DampingFactor:
    case LBG1_fc_speed:
        lbg1_calculate_coefficients(motor_ptr);
        break;
    default:
        break;
    }
}
#endif  //#if (MODULE_MPOS_LBG1 == ENABLED)
