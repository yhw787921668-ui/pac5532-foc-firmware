
/**
 * @file foc_funcs.h
 * @brief Use for FOC utility functions, IdIq current filter, Id & Iq PI, vqmax
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

#ifndef FOC_FUNCS_H
#define FOC_FUNCS_H

#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"
#include "satmon.h"

#if defined(ENABLE_FIELD_WEAKENING)
#include "fw_funcs.h"
#endif

#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
#include "vbus_funcs.h"
#endif

/**
 * @brief This function use to reset FOC related variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
extern void foc_reset(Struct_Motor* motor_ptr);
extern void foc_systemvar_init(Struct_Motor* motor_ptr);
extern void foc_calc_pwmtimer(Struct_Motor* motor_ptr);


/**
 * @brief Digital filter, use to filter id and iq current
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void filter_idiq_q14 (Struct_Motor* motor_ptr)
{
    int32_t temp_q14;

    // y(i) = y(i-1) + alpha * (x(i) -y(i-1)), where alpha = dt / (tau + dt) and tau = 1/alpha
    temp_q14 = motor_ptr->foc.iq_prefilter_q14 - motor_ptr->foc.iq_q14;
    temp_q14 = (temp_q14 * (motor_ptr->foc.Iq_filter_gain >> 2)) >> 14;     // Q14 * Q14 = Q28 >> 14 = Q14
    temp_q14 = motor_ptr->foc.iq_q14 + temp_q14;
    motor_ptr->foc.iq_q14 = temp_q14;

    // y(i) = y(i-1) + alpha * (x(i) -y(i-1)), where alpha = dt / (tau + dt) and tau = 1/alpha
    temp_q14 = motor_ptr->foc.id_prefilter_q14 - motor_ptr->foc.id_q14;
    temp_q14 = (temp_q14 * (motor_ptr->foc.Id_filter_gain >> 2)) >> 14;     // Q14 * (Q16 >> 2) = Q28 >> 14 = Q14
    temp_q14 = motor_ptr->foc.id_q14 + temp_q14;
    motor_ptr->foc.id_q14 = temp_q14;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use to copy application variables to scaled value
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void update_outerloop_fdbk_scaled(Struct_Motor* motor_ptr)
{
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
    // Scale power feedback
    motor_ptr->control.outerloop_fdbk_scaled_q14 = (motor_ptr->power.power_result >> (motor_ptr->control.outerloop_pi_scale_shift + 2));    // q16>>2=q14
#elif (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
    // Scale speed feedback
    motor_ptr->control.outerloop_fdbk_scaled_q14 = motor_ptr->mpos.mpos_spd_scaled_q14;    // q16>>2=q14
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use to run outer loop PI controller
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void controlmode_outerloop_process(Struct_Motor* motor_ptr)
{
#if (SPEED_PI_LOOP_DIVIDER > 1)
    static fix16_t loop_count = 0;

    if (++loop_count >= SPEED_PI_LOOP_DIVIDER)
    {
        loop_count = 0;
#endif
        // Outer loop ramp and scale
        GENERIC_RAMP(motor_ptr->control.outerloop_ramp, motor_ptr->control.outerloop_target, motor_ptr->control.outerloop_rate);
        motor_ptr->control.outerloop_ramp_scaled_q14 = (motor_ptr->control.outerloop_ramp >> (motor_ptr->control.outerloop_pi_scale_shift + 2));  // q14 = q16>>2

        update_outerloop_fdbk_scaled(motor_ptr);

        // Run Outer Loop PI controller and pass result to FOC loop
        pid_run(&(motor_ptr->control.outerloop_pid), (motor_ptr->control.outerloop_ramp_scaled_q14 - motor_ptr->control.outerloop_fdbk_scaled_q14));
    #if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
        motor_ptr->foc.iqref_ramp_q14 = motor_ptr->control.outerloop_pid.PI_sat * motor_ptr->app.app_motor_direction;
    #else
        motor_ptr->foc.iqref_ramp_q14 = motor_ptr->control.outerloop_pid.PI_sat;
    #endif

#if (SPEED_PI_LOOP_DIVIDER > 1)
    }
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void control_ramp_idref(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_FIELD_WEAKENING)
        // If FW is not active
        if(motor_ptr->fw.fw_active_flag == 0)
        {
            // Ramp Id to CL Id reference
            GENERIC_RAMP(motor_ptr->foc.idref_ramp_f16, motor_ptr->control.idref_target_f16, motor_ptr->control.idiqref_rate_f16);
            motor_ptr->foc.idref_ramp_q14 = (motor_ptr->foc.idref_ramp_f16 >> 2);

            #if (FIELD_WEAKENING_METHOD == PID_CONTROL)
                motor_ptr->fw.fw_pid.I_prev = 0;
            #endif
        }
        // If FW is active
        else
        {
            fw_process(motor_ptr);
        }
#else
        // Ramp Id to Id Target
        GENERIC_RAMP(motor_ptr->foc.idref_ramp_f16, motor_ptr->control.idref_target_f16, motor_ptr->control.idiqref_rate_f16);
        motor_ptr->foc.idref_ramp_q14 = (motor_ptr->foc.idref_ramp_f16 >> 2);
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void control_ramp_iqref(Struct_Motor* motor_ptr)
{
    // Ramp Iq to Iq Target
    GENERIC_RAMP(motor_ptr->foc.iqref_ramp_f16, motor_ptr->control.iqref_target_f16, motor_ptr->control.idiqref_rate_f16);
    motor_ptr->foc.iqref_ramp_q14 = (motor_ptr->foc.iqref_ramp_f16 >> 2);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#ifdef ENABLE_BOOTPROT
/**
 * @brief This function use to check bootstrap time validity
 * @param uint16_t duty_cycle
 * @param int8_t* count
 * @param Struct_Motor* motor_ptr pointer to motor struct
 */
PAC5XXX_RAMFUNC static inline uint16_t check_duty_cycle_bootstrap_time(uint16_t duty_cycle, int8_t* count, Struct_Motor* motor_ptr)
{
    if (duty_cycle > motor_ptr->bootprot.bootprot_pwm_max_ticks)
    {
        *count = *count + 1;
        if (*count > motor_ptr->bootprot.bootprot_max_counts)
        {
            duty_cycle = motor_ptr->bootprot.bootprot_pwm_max_ticks;
            *count = motor_ptr->bootprot.bootprot_max_counts;
        }
    }
    else
    {
        *count = *count - 1;
        if (*count <= 0)
        {
            *count = 0;
        }
    }

    return duty_cycle;
}
#endif


/**
 * @brief Calculate Vq max, use to calculate vq max
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void calculate_vqmax(Struct_Motor* motor_ptr)
{
    // Scale circle table index and result so that max circle limit is 0.92 instead of 0.5.  New SVM is scaled differently where input 1 -> 100% duty.  Previously 0.58 -> 100% duty.
    if (motor_ptr->foc.vd_q14 < 0)
    {
#ifdef CONTROL_VDVQ_VECTOR_LIMIT
        motor_ptr->foc.Iq_pid.max_value = (((foc_circle_tbl[(- ((fix16_t) (((motor_ptr->foc.vd_q14 * motor_ptr->control.duty_max_pct_1div_q14) >> 14) << 2))) >> 8] >> 2) * motor_ptr->control.duty_max_pct_q14) >> 14);
        motor_ptr->foc.Iq_pid.min_value = - motor_ptr->foc.Iq_pid.max_value;
        #else
        motor_ptr->foc.Iq_pid.max_value = (foc_circle_tbl[(- ((fix16_t) (motor_ptr->foc.vd_q14 << 2))) >> 8] >> 2);
#endif
    }
    else
    {
#ifdef CONTROL_VDVQ_VECTOR_LIMIT
        motor_ptr->foc.Iq_pid.max_value = (((foc_circle_tbl[(((fix16_t) (((motor_ptr->foc.vd_q14 * motor_ptr->control.duty_max_pct_1div_q14) >> 14) << 2))) >> 8] >> 2) * motor_ptr->control.duty_max_pct_q14) >> 14);
        motor_ptr->foc.Iq_pid.min_value = - motor_ptr->foc.Iq_pid.max_value;
        #else
        motor_ptr->foc.Iq_pid.max_value = (foc_circle_tbl[((fix16_t) (motor_ptr->foc.vd_q14 << 2)) >> 8] >> 2);
#endif
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#if defined(ENABLE_DQ_DECOUPLING)
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline int32_t decoupling_term_calc(int32_t motor_spd_radps_q16, 
                                                           int32_t motor_inductance_q26, 
                                                           int32_t motor_current_ref_q14, 
                                                           int32_t motor_current_feedback_q14,
                                                           int32_t current_filter_coeff_q16,
                                                           int32_t* idq_dec_q14)
{
    fix16_t temp1, temp2;
    temp1 = ((motor_spd_radps_q16 >> 10) * (motor_inductance_q26 >> 4)) >> 12;     // ((q16>>10)=q6 * q22) >> 12 = q16
    // Filter the current for the decoupling term
    #if (SEL_DQ_DECOUPLING_METHOD == FEEDBACK_DECOUPLING)
    *idq_dec_q14 = filter_alpha_q14(motor_current_feedback_q14, *idq_dec_q14, current_filter_coeff_q16);
    temp2 = (temp1 * (*idq_dec_q14 >> 4)) >> 12;                                   // (q16 * q10) >> 12 = q14
    #elif (SEL_DQ_DECOUPLING_METHOD == FEEDFORWARD_DECOUPLING)
    temp2 = (temp1 * (motor_current_ref_q14 >> 4)) >> 12;                          // (q16 * q10) >> 12 = q14
    #endif
    return temp2;                                                                  // q14
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void foc_controller_id(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_DQ_DECOUPLING)
    fix16_t id_decouple_q14, vd_decouple_q14, temp_q14;
    // calculate w*Lq*iq
    id_decouple_q14 = decoupling_term_calc(motor_ptr->mpos.mpos_spd_radps, motor_ptr->mparam.lq_q26, motor_ptr->foc.iqref_ramp_q14, 
                                           motor_ptr->foc.iq_q14, motor_ptr->foc.current_filter_coeff_q16, &motor_ptr->foc.iq_dec_q14);
    // convert current scale to Volt
    temp_q14 = ((motor_ptr->app.descale_current_factor >> 4) * id_decouple_q14) >> 12;               // q14 = (q12 * q14) >> 12
    // convert voltage in Volt to scaled voltage
    vd_decouple_q14 = (temp_q14 * (motor_ptr->app.descale_voltage_factor_1div_q16 >> 2)) >> 14;      // q14 = (q14 * q14) >> 14
    pid_run_decoupled(&motor_ptr->foc.Id_pid, (motor_ptr->foc.idref_ramp_q14 - motor_ptr->foc.id_q14), -vd_decouple_q14);
#else
    pid_run(&motor_ptr->foc.Id_pid, (motor_ptr->foc.idref_ramp_q14 - motor_ptr->foc.id_q14));
#endif
    
#ifndef ENABLE_DC_VOLTAGE_COMPENSATION
    motor_ptr->foc.vd_q14 = motor_ptr->foc.Id_pid.PI_sat;
#else
    // q14->2 = q12*q16 = q28 >> 14 = q14
    motor_ptr->foc.vd_q14 = (motor_ptr->foc.Id_pid.PI_sat >> 2) * (motor_ptr->app.dcbus_voltage_compensation_coff_q26 >> 10) >> 14;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void foc_controller_iq(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_DQ_DECOUPLING)
    fix16_t iq_decouple_q14, vemf_q14, vq_decouple_q14, temp_q14;
    // calculate w*PM_flux + w*Ld*id
    vemf_q14 = ((motor_ptr->mpos.mpos_spd_radps >> 10) * (motor_ptr->mparam.ke_q26 >> 8)) >> 10;         // q16>>10 = q6 * (q26>>8=q18) >> 10 = q14
    iq_decouple_q14 = decoupling_term_calc(motor_ptr->mpos.mpos_spd_radps, motor_ptr->mparam.ld_q26, motor_ptr->foc.idref_ramp_q14, 
                                           motor_ptr->foc.id_q14, motor_ptr->foc.current_filter_coeff_q16, &motor_ptr->foc.id_dec_q14);
    // convert current scale to Volt
    temp_q14 = ((motor_ptr->app.descale_current_factor >> 4) * iq_decouple_q14) >> 12;                            // q14 = (q12 * q14) >> 12
    // convert voltage in Volt to scaled voltage
    vq_decouple_q14 = ((vemf_q14 + temp_q14) * (motor_ptr->app.descale_voltage_factor_1div_q16 >> 2)) >> 14;      // q14 = (q14 * q14) >> 14
    pid_run_decoupled(&motor_ptr->foc.Iq_pid, (motor_ptr->foc.iqref_ramp_q14 - motor_ptr->foc.iq_q14), vq_decouple_q14);
#else
    pid_run(&motor_ptr->foc.Iq_pid, (motor_ptr->foc.iqref_ramp_q14 - motor_ptr->foc.iq_q14));
#endif

#ifndef ENABLE_DC_VOLTAGE_COMPENSATION
    motor_ptr->foc.vq_q14 = motor_ptr->foc.Iq_pid.PI_sat;
#else
    // q14->2 = q12*q16 = q28 >> 14 = q14
    motor_ptr->foc.vq_q14 = (motor_ptr->foc.Iq_pid.PI_sat >> 2) * (motor_ptr->app.dcbus_voltage_compensation_coff_q26 >> 10) >> 14;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Id,Iq PI controller, use to control motor Id, Iq currents
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void idiq_pi_controllers (Struct_Motor* motor_ptr)
{
#ifdef ENABLE_IDIQ_FILTER
    // Apply digital filter to Id, Iq feedback values
    filter_idiq_q14(motor_ptr);
#else
motor_ptr->foc.id_q14 =  motor_ptr->foc.id_prefilter_q14;
motor_ptr->foc.iq_q14 = motor_ptr->foc.iq_prefilter_q14;
#endif


#if defined(ENABLE_CONTROLLER_INTERLEAVE)
    switch(++motor_ptr->app.gear_mode)
    {
    case 1:
#if (MODULATION_SELECT != MODULATION_HYBRID_TRAP_DMIN)
        // Ramp id reference
        control_ramp_idref(motor_ptr);

        // Id Controller
        foc_controller_id(motor_ptr);
#else
        motor_ptr->foc.vd_q14 = 0;
#endif

        // Calculate max Vq for unit circle
        calculate_vqmax(motor_ptr);

#if defined(ENABLE_DC_VOLTAGE_COMPENSATION)
        // Measure instantaneous DC bus voltage
        read_dc_voltage_fast(motor_ptr);
#endif
        break;

    case 2:
        // Ramp or control iq reference
        switch(motor_ptr->state.control)
        {
        case STATE_CONTROL_OUTERLOOP:
            // Run outerloop control process
            controlmode_outerloop_process(motor_ptr);
            break;

        case STATE_CONTROL_TORQUE:
            // Ramp iq reference
            control_ramp_iqref(motor_ptr);

            // Update outerloop value
            update_outerloop_fdbk_scaled(motor_ptr);
            break;
        case STATE_CONTROL_IDLE:
        default:
            break;
        }

        // Iq Controller
        foc_controller_iq(motor_ptr);

        // No break!!!

    default:
        motor_ptr->app.gear_mode = 0;
        break;
    }

#else   //#if defined(ENABLE_CONTROLLER_INTERLEAVE)
#if (MODULATION_SELECT != MODULATION_HYBRID_TRAP_DMIN)
    // Ramp id reference
    control_ramp_idref(motor_ptr);

    // Id Controller
    foc_controller_id(motor_ptr);
#else

    if (motor_ptr->mpos.mposmode != STATE_MPOSMODE_TRAP)
    {
        // Ramp id reference
        control_ramp_idref(motor_ptr);

        // Id Controller
        foc_controller_id(motor_ptr);
    }
    else
    {
        motor_ptr->foc.vd_q14 = 0;
    }
#endif

    // Calculate max Vq for unit circle
    calculate_vqmax(motor_ptr);

    // Ramp or control iq reference
    switch(motor_ptr->state.control)
    {
    case STATE_CONTROL_OUTERLOOP:
        // Run outerloop control process
        controlmode_outerloop_process(motor_ptr);
        break;

    case STATE_CONTROL_TORQUE:
        // Ramp iq reference
        control_ramp_iqref(motor_ptr);

        // Update outerloop value
        update_outerloop_fdbk_scaled(motor_ptr);
        break;

    case STATE_CONTROL_IDLE:
    default:
        break;
    }

    // Iq Controller
    foc_controller_iq(motor_ptr);

#if defined(ENABLE_DC_VOLTAGE_COMPENSATION)
    // JAGTAG: Need to verify dt if not interlacing
    // Measure instantaneous DC bus voltage
    read_dc_voltage_fast(motor_ptr);

#endif

#endif  //#if defined(ENABLE_CONTROLLER_INTERLEAVE)

#if 0
    // Test codes only
    debug_dac_1a = vd_decouple_q14;
    debug_dac_1b = vq_decouple_q14;
    debug_var_1 = id_decouple_q14;
    debug_var_2 = iq_decouple_q14;
    debug_var_3 = motor_ptr->foc.vd_q14;
    debug_var_4 = motor_ptr->foc.vq_q14;
#endif

#if 0 && defined(ENABLE_DC_VOLTAGE_COMPENSATION)
// Test use only
debug_dac_1a = motor_ptr->app.dcbus_voltage_fast_q14;   // q16 -> 2 = q14
debug_dac_1b = motor_ptr->app.dcbus_voltage_ave_q14;   // q16 -> 2 = q14
debug_dac_1c = motor_ptr->app.dcbus_voltage_compensation_coff_q26 >> 12;   // q26 -> 12 = q14
#endif

#if defined(ENABLE_DEBUG_VOLTAGEMODE)
   // Override PI controllers and force Vd/Vq
   motor_ptr->foc.vd_q14 = 0;
   motor_ptr->foc.vq_q14 = debug_input_3 >> 2;
#endif

   pi_sat_mon(motor_ptr);
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef FOC_FUNCS_H
