
/**
 * @file param_write.c
 * @brief Use for parameter processing
 */

//=============================================================================
// Copyright (C) 2017 - 2022, Qorvo, Inc.
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
#include "param_funcs.h"
#include "fault.h"
#include "driver_funcs.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_tile.h"
#include "pid.h"
#include "control_funcs.h"
#include "mpos_funcs.h"
#include "pac_init.h"
#include "config_features.h"
#include "app_funcs.h"
#include "foc_funcs.h"
#include "foc_config.h"

#ifdef ENABLE_OPD_CL
    #include "opd_cl_funcs.h"
#endif

#ifdef ENABLE_SIM
    #include "sim_funcs.h"
#endif

#ifdef ENABLE_IPD
    #include "ipd_config.h"
#endif

#ifdef ENABLE_STALL_DETECT
    #include "stalldet_funcs.h"
#endif

#if (COMMSEL_PPM == TRUE)
#include "ppm.h"
#endif

#if defined(ENABLE_POWERMEASURE)
#include "control_power_funcs.h"
#endif

#if defined(ENABLE_AUTOTUNE_PI)
#include "atune_funcs.h"
#endif

#if defined(ENABLE_FIELD_WEAKENING)
#include "fw_funcs.h"
#endif

#if defined(ENABLE_DEBUG)
#include "debug_paramwrite.h"
#endif

#if defined(ENABLE_FLASHDEMO)
#include "flashdemo.h"
#endif

#if defined(ENABLE_QEP_LIN_FLASHRW)
#include "qep_lin_flashrw.h"
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    #include "startup_aligngo2est_funcs.h"
#elif ((MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL))
    #include "startup_hall_funcs.h"
#elif ((MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP))
    #include "startup_aligngo2qep_funcs.h"
#elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    #include "startup_aligngo2trap_funcs.h"
#endif


void param_write(ParameterCode param_code, fix16_t rx_data, Struct_Motor* motor_ptr)
{
#if defined(ENABLE_DEBUG)
    debug_paramwrite((ParameterCode)motor_ptr->comm.param_code, motor_ptr->comm.rx_data, motor_ptr);
#endif

#if defined(ENABLE_AUTOTUNE_PI)
    atpi_paramchange_check(motor_ptr, param_code, rx_data);
#endif

    switch (param_code)
    {
// Motor Config
        case current_limit:
            motor_ptr->app.current_limit = rx_data;
            init_over_current_limit(motor_ptr->app.current_limit);
            break;
        case dt:
            check_param_while_run(motor_ptr);
            motor_ptr->foc.dt_control_q26 = rx_data;
            motor_ptr->foc.dt_control_1div_q6 = fix16_div(FIX16_1P0, rx_data);

        #ifdef ENABLE_PLL_ANGLE_CALCULATION
            // PLL dt
            motor_ptr->mpos.est_pll.pll_pid.dt_q26 = rx_data;
            motor_ptr->mpos.est_pll.pll_dt_q22 = rx_data >> 4;
        #endif
            break;
        case descale_current_factor:
            break;
        case diff_amp_gain:
            check_param_while_run(motor_ptr);
            if (motor_ptr->app.iphase_amp_gain != rx_data)
            {
                diff_amp_gain_param_write(motor_ptr, rx_data);
                init_diff_amp_gain();
                init_over_current_limit(motor_ptr->app.current_limit);
#ifdef ENABLE_ADCCAL_POWERUP
                __disable_irq();
                motor_ptr->state.main = STATE_MAIN_POST;
                motor_ptr->state.post = STATE_POST_RESET;
                __enable_irq();
#endif
            }
#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
            power1_calc_param(motor_ptr);
#endif
            break;
        case cl_mode_selection:
            motor_ptr->app.speed_mode_flag = rx_data;
            break;
        case reverse_direction:
#ifndef ENABLE_NEGATIVE_IQ_COASTING
            if (motor_ptr->state.main != STATE_MAIN_FORCEANG)
                check_param_while_run(motor_ptr);
#endif
            if (rx_data)
                motor_ptr->app.app_motor_direction_cmd = -1;
            else
                motor_ptr->app.app_motor_direction_cmd = 1;
            break;

// Misc
         case Vdc_gui:
             break;

// Id & Iq
         case id_pi_filter_coeff:
             motor_ptr->foc.Id_filter_gain = rx_data;
             break;
         case iq_pi_filter_coeff:
             motor_ptr->foc.Iq_filter_gain = rx_data;
             break;
         case idiq_ramp_rate:
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
             rx_data = rx_data << 1;
#endif
             if (rx_data == 0)
                 rx_data = 1;
             motor_ptr->control.idiqref_rate_gui_f16 = rx_data;
             motor_ptr->control.idiqref_rate_f16 = motor_ptr->control.idiqref_rate_gui_f16;
             break;

// Closed Loop
         case closed_loop_speed_slew_rate:
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
             rx_data = rx_data << 1;
#endif
             motor_ptr->control.outerloop_rate_store = rx_data;
             motor_ptr->control.outerloop_rate = (motor_ptr->control.outerloop_rate_store * SPEED_PI_LOOP_DIVIDER) ;    // Descaled f16
#endif
             break;
         case closed_loop_speed_setpoint:
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
//             motor_ptr->app.outerloop_target_cmd_f16 = rx_data;
             // JAGTAG: Need to remove reference to debug_input_4
//             if (debug_input_4)
//                 motor_ptr->app.outerloop_target_cmd_f16 = -rx_data;
//             else
                 motor_ptr->app.outerloop_target_cmd_f16 = rx_data;
#endif
#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
             motor_ptr->debug.cl_speed_backup = rx_data;
#endif
             break;
         case id_ref_cl:
             motor_ptr->app.idrefcl_cmd_f16 = rx_data;
             break;
         case iq_ref_cl:
             motor_ptr->app.iqrefcl_cmd_f16 = rx_data;
             break;
         case max_cl_speed_limit:
             motor_ptr->app.speedrefcl_max = rx_data;
             calculate_outerloop_scale_shift(motor_ptr->app.speedrefcl_max, &motor_ptr->app.speed_scale_shift, &motor_ptr->app.outerloop_scale_q0);
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
             motor_ptr->control.outerloop_pi_scale_shift = motor_ptr->app.speed_scale_shift;
#endif
             break;

// Speed PID
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
         case speed_pid_Kp:
             motor_ptr->control.outerloop_pid.ka_q16 = rx_data;
             picon_convert_gain(&motor_ptr->control.outerloop_pid, (motor_ptr->app.outerloop_scale_q0 << 16), motor_ptr->app.descale_current_factor);
             break;
         case speed_pid_Ki:
             motor_ptr->control.outerloop_pid.kb_q16 = rx_data;
             picon_convert_gain(&motor_ptr->control.outerloop_pid, (motor_ptr->app.outerloop_scale_q0 << 16), motor_ptr->app.descale_current_factor);
             break;
#endif

         case outerloop_pid_min:
             motor_ptr->control.ineg_q16 = rx_data;

             break;
         case outerloop_pid_max:
             motor_ptr->control.imax_q16 = rx_data;
             motor_ptr->control.imax_1div_q16 = fix16_div(FIX16_ONE, motor_ptr->control.imax_q16);
             break;
// Id PID
         case Id_pid_Kp:
             motor_ptr->foc.Id_pid.ka_q16 = rx_data;
             break;
         case Id_pid_Ki:
             motor_ptr->foc.Id_pid.kb_q16 = rx_data;
             break;
         case Id_pid_min:
             motor_ptr->foc.Id_pid.min_value = (rx_data >> 2);
             break;
         case Id_pid_max:
             motor_ptr->foc.Id_pid.max_value = (rx_data >> 2);
             break;
// Iq PID
         case Iq_pid_Kp:
             motor_ptr->foc.Iq_pid.ka_q16 = rx_data;
             break;
         case Iq_pid_Ki:
             motor_ptr->foc.Iq_pid.kb_q16 = rx_data;
             break;
         case Iq_pid_min:
             motor_ptr->foc.Iq_pid.min_value = (rx_data >> 2);
             motor_ptr->foc.Iq_pid_min_value_gui = (rx_data >> 2);
             break;
         case Iq_pid_max:
             motor_ptr->foc.Iq_pid.max_value = (rx_data >> 2);
             break;

// Motor
         case Ld:
             motor_ptr->mparam.ld_q26 = rx_data;
             break;
         case Lq:
             motor_ptr->mparam.lq_q26 = rx_data;
             motor_ptr->mparam.fpole_q16 = fix16_div((motor_ptr->mparam.rs_q16 << 10), motor_ptr->mparam.lq_q26);
             break;
         case Ralpha:
             motor_ptr->mparam.rs_q16 = rx_data;
             motor_ptr->mparam.fpole_q16 = fix16_div((motor_ptr->mparam.rs_q16 << 10), motor_ptr->mparam.lq_q26);
             break;
         case num_pole_pairs:
             motor_ptr->mparam.pole_pairs = rx_data;
             break;
         case Config_Ke:
             motor_ptr->mparam.ke_q26 = rx_data;
             break;
         case Confi_J:
             motor_ptr->mparam.j_q26 = rx_data;
             break;
         case NominalSpeed:
             motor_ptr->mparam.nom_spd_q16 = rx_data;
         #ifdef ENABLE_PLL_ANGLE_CALCULATION
             motor_ptr->mpos.est_pll.pll_pid.max_value = QCONST(2.0f,Q14);
             motor_ptr->mpos.est_pll.pll_pid.min_value = QCONST(-2.0f,Q14);
             motor_ptr->mpos.est_pll.pll_nom_spd_q2 = rx_data >> 14;
         #endif
             break;
         case PWM_to_FOC_control_ratio:
             check_param_while_run(motor_ptr);
             motor_ptr->foc.ctrl_divider_foc = rx_data;
             break;
         case Auto_Mode_Open_Close_Loop:
             if (rx_data)
             {
                 motor_ptr->app.auto_close_loop_cmd = 1;
             }
             else
             {
                 motor_ptr->app.auto_close_loop_cmd = 0;
             }
             break;
         case Clear_Fault_Flags:
             clear_fault(FAULT_MAIN_GRP_CLEARCMD);
             break;

#ifdef ENABLE_IPD
         case ipd_charge:
             // Load new pwm_charge_ticks from rx_data; calculate new parameters
             motor_ptr->ipd.pwm_charge_ticks = rx_data;
             if (motor_ptr->ipd.pwm_charge_ticks <= IPD_ADC_TRIGGER_OFFSET_TICKS*motor_ptr->ipd.num_periods_per_pulse)
             {
                 motor_ptr->ipd.pwm_charge_ticks = (IPD_ADC_TRIGGER_OFFSET_TICKS+1)*motor_ptr->ipd.num_periods_per_pulse;
             }
             break;
         case ipd_discharge:
             // Load new pwm_discharge_percent from rx_data; calculate new parameters
             motor_ptr->ipd.pwm_discharge_percent = rx_data;
             break;
         case ipd_num_periods_per_pulse:
             motor_ptr->ipd.num_periods_per_pulse = rx_data;
             if (motor_ptr->ipd.pwm_charge_ticks <= IPD_ADC_TRIGGER_OFFSET_TICKS*motor_ptr->ipd.num_periods_per_pulse)
             {
                 motor_ptr->ipd.pwm_charge_ticks = (IPD_ADC_TRIGGER_OFFSET_TICKS+1)*motor_ptr->ipd.num_periods_per_pulse;
             }
             break;
         case ipd_angle_offset_forward:
             // Scale back q0 from q16 format for internal use
             motor_ptr->ipd.ipd_angle_forward_offset = rx_data >> 16;
             break;
         case ipd_angle_offset_reverse:
             // Scale back q0 from q16 format for internal use
             motor_ptr->ipd.ipd_angle_reverse_offset = rx_data >> 16;
             break;
#endif

         case close_loop_coast_timeout:
             motor_ptr->app.coast_timeout_target = rx_data;
             break;

#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
         case Power_pid_Kp:
             motor_ptr->control.outerloop_pid.ka_q16 = rx_data;
             picon_convert_gain(&motor_ptr->control.outerloop_pid, (motor_ptr->app.outerloop_scale_q0 << 16), motor_ptr->app.descale_current_factor);
             break;

         case Power_pid_Ki:
             motor_ptr->control.outerloop_pid.kb_q16 = rx_data;
             picon_convert_gain(&motor_ptr->control.outerloop_pid, (motor_ptr->app.outerloop_scale_q0 << 16), motor_ptr->app.descale_current_factor);
             break;

         case Power_ramp_rate:
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
             rx_data = rx_data << 1;
#endif
             motor_ptr->control.outerloop_rate_store = rx_data;
             motor_ptr->control.outerloop_rate = (motor_ptr->control.outerloop_rate_store * SPEED_PI_LOOP_DIVIDER) ;    // Descaled f16
             break;

         case Power_setpoint:
             motor_ptr->app.outerloop_target_cmd_f16 = rx_data;
             break;

         case Power_max_limit:
             motor_ptr->app.powerrefcl_max = rx_data;
             calculate_outerloop_scale_shift(motor_ptr->app.powerrefcl_max, &motor_ptr->control.outerloop_pi_scale_shift, &motor_ptr->app.outerloop_scale_q0);
             break;
#endif

#if defined(ENABLE_POWERMEASURE)
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
        // JAGTAG: Bring power_iphase_factor_q14 out to GUI?
        case Power_Measure_Iphase_Offset:
            motor_ptr->power.power_iphase_offset_q14 = rx_data >> 2;
            break;     
        
        case Power_Measure_Iphase_Factor:
        // JAGTAG: Bring power_iphase_offset_q14 out to GUI?
            motor_ptr->power.power_iphase_factor_q14 = rx_data >> 2;
            break;
#endif
#endif

        default:
            break;
    }

    //////////////////////////////////////////////////
    // Parameter Write Functions Calls
    //////////////////////////////////////////////////

#ifdef ENABLE_STALL_DETECT
    stalldet_param_write(motor_ptr, param_code, rx_data);
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    startup_aligngo_param_write(motor_ptr, param_code, rx_data);
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)
    lbg1_param_write(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_AUTOTUNE_PI)
    at_param_write(motor_ptr, param_code, rx_data);
#endif

#ifdef ENABLE_SIM
    sim_param_write(motor_ptr, param_code, rx_data);
#endif

#if (MODULE_MPOS_HALL == ENABLED)
    hall_param_write(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_HALL_TUNING)
    halltune_param_write(motor_ptr, param_code, rx_data);
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    qep_param_write(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_FIELD_WEAKENING)
    fw_param_write(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_MTPA)
    mtpa_param_write(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_FORCEANG)
    forceang_aligngo_param_write(motor_ptr, param_code, rx_data);
#endif

#if defined (ENABLE_QEP_LINEARIZATION_TUNING) || defined (ENABLE_QEP_OFFSET_TUNING)
    qep_offset_linearization_param_write(motor_ptr, param_code, rx_data);
#endif

#if (MODULE_MPOS_TRAP == ENABLED)
    trap_param_write(motor_ptr, param_code, rx_data);
#endif

    //////////////////////////////////////////////////
    // Parameter Process Functions Calls
    //////////////////////////////////////////////////

    //FOC param process
    switch(param_code)
    {
    case dt:
    case PWM_to_FOC_control_ratio:
        // Calculate PWM timer period ticks
        foc_calc_pwmtimer(motor_ptr);
        // Configure control loop for FOC mode
        foc_config(motor_ptr, 1);
        // Update PI dt's
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
        motor_ptr->foc.Id_pid.dt_q26 = rx_data << 1;
        motor_ptr->foc.Iq_pid.dt_q26 = rx_data << 1;
        motor_ptr->control.outerloop_pid.dt_q26 = fix16_mul((rx_data << 1), fix16_from_int(SPEED_PI_LOOP_DIVIDER));
#else
        motor_ptr->foc.Id_pid.dt_q26 = rx_data;
        motor_ptr->foc.Iq_pid.dt_q26 = rx_data;
        motor_ptr->control.outerloop_pid.dt_q26 = fix16_mul(rx_data, fix16_from_int(SPEED_PI_LOOP_DIVIDER));
#endif
        picon_convert_gain(&motor_ptr->control.outerloop_pid, (motor_ptr->app.outerloop_scale_q0 << 16), motor_ptr->app.descale_current_factor);
#if (COMMSEL_PPM == TRUE)
        // Convert into ADC ISR ticks count
        motor_ptr->ppm.pulse_timeout_in_sys_tick_isr_counts = (fix16_div((PPM_PULSE_VALID_CHECK_IN_SEC_FIX16), DT_SYSTICK_Q16)) >> 16;
        motor_ptr->ppm.motor_off_timeout_in_pulse_timeout_counts = (fix16_div((PPM_MOTOR_OFF_TIMEOUT_IN_SEC_FIX16), DT_SYSTICK_Q16)) >> 16;
        motor_ptr->ppm.motor_off_timeout_in_pulse_timeout_counts = (motor_ptr->ppm.motor_off_timeout_in_pulse_timeout_counts/motor_ptr->ppm.pulse_timeout_in_sys_tick_isr_counts);

#endif
#if defined(ENABLE_POWERMEASURE)
        power_calc_filter_alpha(motor_ptr, POWER_FILTER_FREQ_CUTOFF_HZ_Q16);
#endif
#if (INTERNAL_VMS == TRUE)
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
        motor_ptr->u_duty_cycle_max_limit = (motor_ptr->control.motor_period_ticks * VMS_VALUE_EXCLUDE_BAND_LIMIT_3SHUNT) >> 16;
#endif
#endif
        break;

    default:
        break;
    }

    // Application parameter process function always needs to run first
    app_param_process(motor_ptr, param_code, rx_data);

    control_param_process(motor_ptr, param_code);

#ifdef ENABLE_OPD_CL
    opdcl_param_process(motor_ptr, param_code, rx_data);
#endif

#ifdef ENABLE_STALL_DETECT
    stalldet_param_process(motor_ptr, param_code, rx_data);
#endif

#ifdef ENABLE_SIM
    sim_param_process(motor_ptr, param_code, rx_data);
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    startup_aligngo_param_process(motor_ptr, param_code);
#endif

#if defined(ENABLE_HALL_TUNING)
    halltune_aligngo_param_process(motor_ptr, param_code);
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)
    lbg1_param_process(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_AUTOTUNE_PI)
    at_param_process(motor_ptr, param_code, rx_data);
#endif

#if (MODULE_MPOS_HALL == ENABLED)
    hall_param_process(motor_ptr, param_code, rx_data);
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    qep_param_process(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_MTPA)
    mtpa_param_process(motor_ptr, param_code, rx_data);
#endif

#if defined(ENABLE_FORCEANG)
    forceang_aligngo_param_process(motor_ptr, param_code);
#endif

#if defined(ENABLE_FLASHDEMO)
    flashdemo_param_process(param_code, rx_data);
#endif

#if defined(ENABLE_QEP_LIN_FLASHRW)
    qep_lin_flashrw_param_process(param_code, rx_data);
#endif
}


void check_param_while_run(Struct_Motor* motor_ptr)
{
    if ((motor_ptr->state.main != STATE_MAIN_STOPPED) &&
            (motor_ptr->state.main != STATE_MAIN_RESET) &&
            (motor_ptr->state.main != STATE_MAIN_POST))
    {
        set_fault(FAULT_PARAM_WHILE_RUN);
    }
}


void write_itotstart_target(int32_t iref_q16, int32_t *itotref_target_q16, int32_t *itotref_target_1div_q16)
{
    // Copy startup itotal and calculate one over itotal
    *itotref_target_q16 = iref_q16;
    *itotref_target_1div_q16 = fix16_div(FIX16_ONE, iref_q16);
}


void init_over_current_limit(uint32_t current_limit)
{
    Struct_Motor* motor_ptr = &motor;
    char lpdac_factor = 1;
    uint32_t lpdac_value = 0x00;


    lpdac_value = current_limit;

    switch (motor_ptr->app.iphase_amp_gain)
    {
        case 0:
            lpdac_factor = 1;
            break;
        case 1:
            lpdac_factor = 1;
            break;
        case 2:
            lpdac_factor = 2;
            break;
        case 3:
            lpdac_factor = 4;
            break;
        case 4:
            lpdac_factor = 8;
            break;
        case 5:
            lpdac_factor = 16;
            break;
        case 6:
            lpdac_factor = 32;
            break;
        case 7:
            lpdac_factor = 48;
            break;
        default:
            lpdac_factor = 1;
            break;
    }

    // Calculate current descale factor
    motor_ptr->app.descale_current_factor = fix16_div(CURRENT_SCALE_FACTOR_X1_Q16, (lpdac_factor << 16));

    // Set LPROT protection threshold

    // Handle LPDAC value calculation overflow
    if (lpdac_value > 0x3FF)
    {
        lpdac_value = 0x3FF;
        set_warning(WARNING_MAIN_OC_VALUE);
    }
    else
    {
        clear_warning(WARNING_MAIN_OC_VALUE);
    }

    // Set LPDACV protection threshold
    // Set HPDAC protection threshold
    lpdac_param_write(lpdac_value);
    hpdac_param_write(lpdac_value, lpdac_factor, current_limit);
}
