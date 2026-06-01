
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
#include "sim_funcs.h"
#include "pac5xxx_driver_tile.h"
#include "config_app.h"
#include "driver_funcs.h"

//#include "ppm.h"

#include "pac5xxx_timers.h"
//#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "debug_export.h"
#include "flashrw_funcs.h"
#include "flashdemo.h"


#if (COMMSEL_PPM == TRUE)
#include "ppm.h"
#endif

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#include "control_power_funcs.h"
#endif

#include "mpos_funcs.h"
#include "trig.h"

#if defined(ENABLE_AUTOTUNE_MID)
#include "mid1_funcs.h"
#endif

#if defined(ENABLE_BUZZ)
#include "buzz_funcs.h"
#endif


#ifdef ENABLE_DEBUG_PARAMEXPORT
fix16_t gui_parameter_command = 0;
fix16_t parameter_value1 = 0;
fix16_t parameter_value2 = 0;
fix16_t parameter_value3 = 0;

#if defined(ENABLE_DEBUG_FLASHDEMO)
#include "flashrw_crc8.h"
static unsigned int count_num;
extern int32_t flash_test_data[FLASHDEMO_FLASH_WORD_ARRAY_SIZE];
#endif


void export_variables(fix16_t variable_num)
{
    Struct_Motor* motor_ptr = &motor;

    parameter_value1 = - (10 << 16);
    parameter_value2 = - (10 << 16);
    parameter_value3 = - (10 << 16);

    switch (variable_num)
    {
// Configuration
        case 1:
            parameter_value1 = motor_ptr->app.descale_current_factor;
            parameter_value2 = motor_ptr->app.current_limit;
            parameter_value3 = motor_ptr->app.iphase_amp_gain;
            break;
        case 2:
            hpdac_lpdaccombined_param_read(&parameter_value1, &parameter_value2, &parameter_value3);
            break;
        case 3:
            parameter_value1 = motor_ptr->control.dt_control_q26;
            parameter_value2 = motor_ptr->control.dt_control_1div_q6;
            break;
        case 4:
            parameter_value1 = motor_ptr->control.ctrl_divider;
            parameter_value2 = motor_ptr->control.motor_period_ticks;
            parameter_value3 = motor_ptr->control.motor_period_ticks_1div;
            break;
        case 5:
            parameter_value1 = motor_ptr->app.speed_mode_flag;
            parameter_value2 = motor_ptr->app.auto_close_loop_cmd;
            parameter_value3 = motor_ptr->app.app_motor_direction;
            break;
        case 6:
            parameter_value1 = motor_ptr->app.dcbus_voltage;
            parameter_value2 = motor_ptr->app.descale_voltage_factor;
            parameter_value3 = motor_ptr->app.descale_voltage_factor_1div_q16;
            break;

// Current & Speed Control
        case 7:
            parameter_value1 = motor_ptr->app.idrefcl_cmd_f16;
            parameter_value2 = motor_ptr->control.idref_target_f16;
            break;
        case 8:
            parameter_value1 = motor_ptr->foc.Id_filter_gain;
            parameter_value2 = motor_ptr->foc.Iq_filter_gain;
            parameter_value3 = motor_ptr->control.idiqref_rate_f16;
            break;
        case 9:
            parameter_value1 = motor_ptr->foc.idref_ramp_f16;
            parameter_value2 = motor_ptr->foc.iqref_ramp_f16;
            break;
        case 10:
            parameter_value1 = motor_ptr->control.idref_target_f16;
            parameter_value2 = motor_ptr->control.iqref_target_f16;
            parameter_value3 = motor_ptr->app.iqrefcl_cmd_f16;
            break;
        case 11:
            parameter_value1 = motor_ptr->control.outerloop_target;
            parameter_value2 = motor_ptr->control.outerloop_ramp;
            parameter_value3 = motor_ptr->control.outerloop_rate;
            break;
        case 12:
            parameter_value1 = motor_ptr->app.speedrefcl_min;
            parameter_value2 = motor_ptr->app.speedrefcl_max;
            parameter_value3 = motor_ptr->app.speed_scale_shift;
            break;

// PI Controllers
        case 13:
            parameter_value1 = motor_ptr->control.outerloop_pid.ka_q16;
            parameter_value2 = motor_ptr->control.outerloop_pid.kb_q16;
            break;
        case 14:
            parameter_value1 = (motor_ptr->control.outerloop_pid.kp_q11 << 5);
            parameter_value2 = motor_ptr->control.outerloop_pid.kidt_q16;
            break;
        case 15:
            parameter_value1 = (motor_ptr->control.outerloop_pid.min_value << 2);
            parameter_value2 = (motor_ptr->control.outerloop_pid.max_value << 2);
            parameter_value3 = motor_ptr->control.imax_q16;
            break;
        case 16:
            parameter_value1 = motor_ptr->foc.Id_pid.ka_q16;
            parameter_value2 = motor_ptr->foc.Id_pid.kb_q16;
            break;
        case 17:
            parameter_value1 = (motor_ptr->foc.Id_pid.kp_q11 << 5);
            parameter_value2 = motor_ptr->foc.Id_pid.kidt_q16;
            break;
        case 18:
            parameter_value1 = (motor_ptr->foc.Id_pid.min_value << 2);
            parameter_value2 = (motor_ptr->foc.Id_pid.max_value << 2);
            break;
        case 19:
            parameter_value1 = motor_ptr->foc.Iq_pid.ka_q16;
            parameter_value2 = motor_ptr->foc.Iq_pid.kb_q16;
            break;
        case 20:
            parameter_value1 = (motor_ptr->foc.Iq_pid.kp_q11 << 5);
            parameter_value2 = motor_ptr->foc.Iq_pid.kidt_q16;
            break;
        case 21:
            parameter_value1 = (motor_ptr->foc.Iq_pid.min_value << 2);
            parameter_value2 = (motor_ptr->foc.Iq_pid.max_value << 2);
            break;
        case 22:
            parameter_value1 = motor_ptr->app.speed_scale_shift;
            break;

// Motor
        case 23:
            parameter_value1 = motor_ptr->mparam.ld_q26;
            parameter_value2 = motor_ptr->mparam.lq_q26;
            parameter_value3 = motor_ptr->mparam.rs_q16;
            break;
        case 24:
            parameter_value1 = motor_ptr->mparam.pole_pairs << 16;
            parameter_value2 = motor_ptr->mparam.nom_spd_q16;
            parameter_value3 = fix16_mul(motor_ptr->mparam.ke_q26, QCONST(1000.0f, Q16)) >> 10;                    // Units: milli
            break;

// Field Weakening
#ifdef ENABLE_FIELD_WEAKENING
        case 25:
            parameter_value1 = motor_ptr->fw.id_sp;
            parameter_value2 = fix16_mul(motor_ptr->fw.spd_threshold, FIX16_1P0_DIV_2_PI);
            break;
        case 26:
        #if (FIELD_WEAKENING_METHOD == PID_CONTROL)
            parameter_value1 = motor_ptr->fw.fw_pid.ka_q16;
            parameter_value2 = motor_ptr->fw.fw_pid.kb_q16;
            parameter_value3 = motor_ptr->fw.vmax_scl_q14 << 2;
        #endif
            break;
        case 27:
            break;
#endif

// OPDCL
#ifdef ENABLE_OPD_CL
        case 28:
            parameter_value1 = motor_ptr->opdcl.opdcl_sampletime_systicks;
            parameter_value2 = motor_ptr->opdcl.opdcl_minspeed_radps_q16;
            break;
#endif

// Stall Detection
#ifdef ENABLE_STALL_DETECT
        case 29:
            parameter_value1 = motor_ptr->app.vdrvsq_q14 << 2;
            parameter_value2 = motor_ptr->stall.bemf_constant;
            parameter_value3 = motor_ptr->stall.timeout_systicks;
            break;
        case 30:
            parameter_value1 = motor_ptr->stall.ctr_event_1;
            parameter_value2 = motor_ptr->stall.ctr_event_2;
            parameter_value3 = motor_ptr->stall.ctr_event_3;
            break;
        case 31:
            parameter_value1 = motor_ptr->stall.ctr_event_4;
            parameter_value2 = motor_ptr->stall.sampletime_systicks;
            parameter_value3 = motor_ptr->stall.filter_alpha;
            break;
        case 32:
            parameter_value1 = motor_ptr->stall.ctr_event;
            break;
#endif

#if (MODULE_MPOS_ALIGNGO == ENABLED)
        case 40:
            parameter_value1 = (motor_ptr->foc.idref_ramp_f16);
            parameter_value2 = (motor_ptr->mpos.aligngo.idrefol_target_q16);
            parameter_value3 = (motor_ptr->mpos.aligngo.itotrefol_target_q16);
            break;
        case 41:
            parameter_value1 = motor_ptr->mpos.aligngo.itotrefol_target_q16;
#if defined(ENABLE_ANGLEDIFF_AUTORANGE) && (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_1)
            parameter_value2 = ((motor_ptr->mpos.aligngo.itotrefol_target_q16 * QCONST(ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT, Q10)) >> 10);
#endif
            break;
#endif
#if defined(ENABLE_ANGLEDIFF_AUTORANGE) && (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
        case 42:
            parameter_value1 = motor_ptr->startup.aligngo2est_angleramp_inc_radpdt_q16;
            break;
#endif

#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
        case 43:
            motor_ptr->debug.cl_speed_test_enable = TRUE;
            break;
        case 44:
            motor_ptr->debug.cl_speed_test_enable = FALSE;
            break;
#endif
        case 45:
            lpdac_hpdac_param_read(&parameter_value1, &parameter_value2, &parameter_value3);
            break;

// IPD
#ifdef ENABLE_IPD
        case 46:
            parameter_value1 = motor_ptr->ipd.ipd_state << 16;
            parameter_value2 = motor_ptr->ipd.pwm_charge_ticks;
            parameter_value3 = motor_ptr->ipd.pwm_discharge_percent;
            break;
        case 47:
            parameter_value1 = motor_ptr->ipd.num_periods_per_pulse;
            parameter_value2 = motor_ptr->ipd.ipd_angle_forward_offset;
            parameter_value3 = motor_ptr->ipd.ipd_angle_reverse_offset;
            break;
#endif

        case 48:
            break;
        case 49:
            break;

// Miscellaneous
        case 50:
            parameter_value1 = (MOTOR1_LED_TICKS << 16);
            parameter_value2 = (MOTOR1_TED_TICKS << 16);
            break;
        case 51:
            parameter_value1 = (motor_ptr->foc.svm_max_ticks << 16);
            parameter_value2 = (motor_ptr->foc.svm_min_ticks << 16);
            break;
        case 52:
#ifdef ENABLE_BOOTPROT
            parameter_value1 = (motor_ptr->bootprot.bootprot_pwm_max_ticks << 16);
            parameter_value2 = (motor_ptr->bootprot.bootprot_max_counts << 16);
#endif
            break;
        case 53:
            parameter_value1 = motor_ptr->control.outerloop_pi_scale_shift;
            break;
        case 54:
            break;

// ADC & Calibration
        case 55:
#if (CONFIG_OFFSET & CHANNEL_AIO10)
            parameter_value1 = motor_ptr->post.adc_offset_aio10;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO32)
            parameter_value2 = motor_ptr->post.adc_offset_aio32;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO54)
            parameter_value3 = motor_ptr->post.adc_offset_aio54;
#endif
            break;
        case 56:
#if (CONFIG_OFFSET & CHANNEL_AIO6)
            parameter_value1 = motor_ptr->post.adc_offset_aio6;
#endif
            break;
        case 57:
#if (CONFIG_OFFSET & CHANNEL_AIO7)
            parameter_value1 = motor_ptr->post.adc_offset_aio7;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO8)
            parameter_value1 = motor_ptr->post.adc_offset_aio8;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO9)
            parameter_value1 = motor_ptr->post.adc_offset_aio9;
#endif
            break;

        case 58:
#if defined(FAMILY_55XX) && defined (ENABLE_ADCCAL_FACTORY)
            parameter_value1 = motor_ptr->post.adc_offset_counts_q16;
            parameter_value3 = motor_ptr->post.adc_gain_multp_q16;
#endif
            break;

// States & Status
        case 61:
            parameter_value1 = (motor_ptr->app.motor_enable_cmd);
            parameter_value2 = (motor_ptr->fault.fault_stop_flag);
            break;
        case 62:
            parameter_value1 = ((motor_ptr->status.motor >> 16));
            parameter_value2 = ((motor_ptr->status.motor & 0x0000FFFF));
            break;
        case 63:
            parameter_value1 = (motor_ptr->state.main);
            parameter_value2 = (motor_ptr->state.prestart);
            break;
        case 64:
            parameter_value1 = (motor_ptr->state.appcontrol);
            parameter_value2 = (motor_ptr->state.control);
            break;
        case 65:
            parameter_value1 = (motor_ptr->state.post);
            parameter_value2 = (motor_ptr->state.adccal);
            break;
        case 67:
            parameter_value1 = motor_ptr->fault.fault_terminal;
            break;
        case 68:
#if (PORT_SEL_PA == TRUE)
            parameter_value1 = DRV_MASK_PA_ALL;
            parameter_value2 = DRV_PSEL_PA_ALL;
            parameter_value3 = DRV_PSEL_PA_DRL;
#endif
            break;
        case 69:
#if (PORT_SEL_PD == TRUE)
            parameter_value1 = DRV_MASK_PD_ALL;
            parameter_value2 = DRV_PSEL_PD_ALL;
            parameter_value3 = DRV_PSEL_PD_DRL;
#endif
            break;
#ifdef ENABLE_OPDOL
        case 70:
            parameter_value1 = (motor_ptr->opdol.status_opdol);
            parameter_value2 = (motor_ptr->opdol.state_opdol_main);
            parameter_value3 = (motor_ptr->opdol.state_opdol_ctl);
            break;
#endif
        case 71:
            break;
        case 72:
            parameter_value1 = motor_ptr->app.coast_timeout_target;
#if (COMMSEL_PPM == TRUE)
            parameter_value2 = motor_ptr->ppm.pulse_time << 16;
#endif
            break;

// SIM
#ifdef ENABLE_SIM
        case 73:
            parameter_value1 = motor_ptr->sim.braking_timeout_countmax;
            break;
        case 74:
#if (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF)
            parameter_value1 = motor_ptr->app.vphase_amp_gain;
#endif
            break;
        case 75:
            parameter_value1 = motor_ptr->sim.braking_duty_cycle_dec_q14 << 2;
            break;
        case 76:
            parameter_value1 = motor_ptr->sim.sim_counter;
#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_CURRENT))
            parameter_value2 = motor_ptr->sim.sim_mdetect_countmax;
#elif (SEL_SIM_METHOD == SIM_METHOD_HALL)
            parameter_value2 = SIM_MDETECT_HALLEDGENUM;
#endif
            parameter_value3 = motor_ptr->sim.sim_detect2_countmax;

            break;
        case 77:
            parameter_value1 = (motor_ptr->sim.phase_adc_threshold_q14 << 2);
#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE))
            parameter_value2 = motor_ptr->sim.multp_conv_adc2volts_q16;
            parameter_value3 = motor_ptr->sim.multp_conv_adc2vscaled_q16;
#endif
            break;
        case 78:
            parameter_value2 = motor_ptr->sim.align_count_braking_timeout;
            parameter_value3 = motor_ptr->sim.OL_Id_ref_braking_mul;
            break;
        case 79:
            parameter_value1 = motor_ptr->sim.OL_Iq_ref_braking_mul;
            break;
        case 80:
#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_CURRENT))
            parameter_value1 = motor_ptr->sim.kspd_scaled_q14 <<2;
            parameter_value2 = motor_ptr->sim.filter_alpha_speed_q16;
#endif
            parameter_value3 = motor_ptr->sim.speed_scale_shift;
            break;
        case 81:
            parameter_value1 = motor_ptr->sim.freeInMotion_min_speed_limit;
            parameter_value2 = motor_ptr->sim.sim_motor_direction;
            break;
        case 82:
            parameter_value1 = motor_ptr->sim.braking_duty_min_ticks;
            parameter_value2 = motor_ptr->sim.braking_duty_min_q14 << 2;
            break;
        case 83:
            parameter_value1 = motor_ptr->sim.dt_control_q26;
            parameter_value2 = motor_ptr->sim.sim2cl_maxspd_radps_q16;
            break;
        case 84:
            parameter_value1 = motor_ptr->sim.status_sim;
            parameter_value2 = motor_ptr->sim.state_sim;
            parameter_value3 = motor_ptr->sim.state_sim2cl;
            break;
        case 85:
            parameter_value1 = SIM_ANGLE_BLEND_MAX_CNT << 16;
            parameter_value2 = SIM_ANGLE_BLEND_FACTOR_Q12 << 4;
            break;
        case 86:
#if defined(TM_SIM_NODETECT)
            parameter_value3 = motor_ptr->debug.sim_spd_nobemf_radps;
#endif
            break;
#endif  // #ifdef ENABLE_SIM

#if (COMMSEL_PPM == TRUE)
        case 87:
            parameter_value1 = motor_ptr->ppm.pulse_time;
            parameter_value2 = motor_ptr->ppm.pulse_timeout_in_sys_tick_isr_counts;
            parameter_value3 = motor_ptr->ppm.motor_off_timeout_in_pulse_timeout_counts;
            break;
        case 88:
            parameter_value1 = motor_ptr->ppm.ppm_pulse_delta;
            parameter_value3 = motor_ptr->ppm.pulse_detected;
            break;
        case 89:
            parameter_value1 = MOTOR_SPEED_TO_PULSE_WIDTH_SLOPE;
            parameter_value2 = MOTOR_MINIMUM_CLOSE_LOOP_SPEED;
            parameter_value3 = MOTOR_MAXIMUM_CLOSE_LOOP_SPEED;
            break;
        case 90:
            parameter_value1 = motor_ptr->app.motor_enable_cmd;
            parameter_value2 = motor_ptr->control.outerloop_target;
            parameter_value3 = motor_ptr->ppm.motor_powerup_flag;
            break;
        case 91:
            parameter_value1 =  MIN_PPM_PULSE_WIDTH_TCOUNTS;
            parameter_value2 =  MAX_PPM_PULSE_WIDTH_TCOUNTS;
            parameter_value3 =  MIN_PPM_START_IN_SEC;
            break;
        case 92:
            parameter_value1 = MIN_PPM_START_HYST_TCOUNTS;
            parameter_value2 = MAX_PPM_PULSE_WIDTH_MAX_SPD_TCOUNTS;
            parameter_value3 = PPM_TIMER_SINGLE_COUNT_IN_SECONDS;
            break;
        case 93:
            parameter_value1 = PPM_TIMER_TIME_PERIOD;
            parameter_value2 = PPM_PULSE_VALID_CHECK_IN_SEC_FIX16;
            break;
#endif
        case 94:
            parameter_value1 = (motor_ptr->app.app_motor_direction_cmd << 0);
            parameter_value2 = (motor_ptr->app.app_motor_direction << 0);
            break;
        case 95:
            parameter_value1 = (CONV_Q10TOQ14 << 2);
            parameter_value2 = (COMP_DIFFGAINERROR_Q14 << 2);
            parameter_value3 = (CONV_ADC2VAIO_Q10Q14 << 2);
            break;

        case 96:
#ifdef ENABLE_MTPA
            parameter_value1 = motor_ptr->mtpa.mtpa_gain_degpa_q16;
            parameter_value2 = motor_ptr->mtpa.mtpa_saliency_q16;
            parameter_value3 = motor_ptr->mtpa.mtpa_max_current_ampere_q16;
            break;
        case 97:
            parameter_value1 = motor_ptr->mtpa.mtpa_current_array[1];
            parameter_value2 = motor_ptr->mtpa.mtpa_current_array[2];
            parameter_value3 = motor_ptr->mtpa.mtpa_current_array[10];
            break;
        case 98:
            parameter_value1 = motor_ptr->mtpa.mtpa_angle_array_rad[1];
            parameter_value2 = motor_ptr->mtpa.mtpa_angle_array_rad[2];
            parameter_value3 = motor_ptr->mtpa.mtpa_angle_array_rad[10];
            break;
        case 99:
            parameter_value1 = motor_ptr->mtpa.mtpa_coff_q16[1];
            parameter_value2 = motor_ptr->mtpa.mtpa_coff_q16[0];
            parameter_value3 = motor_ptr->mtpa.mtpa_angle_rad_q16;
#endif
            break;
        
#if (MODULE_MPOS_LBG1 == ENABLED)
        case 100:
            parameter_value1 = (motor_ptr->mpos.est.fs_q15 << 1);
            parameter_value2 = motor_ptr->mpos.est.gs_const_q16;
            parameter_value3 = (motor_ptr->mpos.est.gs_q15 << 1);
            break;
        case 101:
            parameter_value1 = motor_ptr->mpos.est.phscomp_multp_q20 >> 4;
            break;
        case 102:
            parameter_value1 = (motor_ptr->mpos.est.kspd_radps_q16 << 10);
            parameter_value2 = (motor_ptr->mpos.est.kspd_scaled_q14 << 2);
            parameter_value3 = motor_ptr->mpos.est.filter_alpha_speed_q16;
            break;
        case 103:
            parameter_value1 = (motor_ptr->mpos.est.speed_scale_shift << 16);
            parameter_value2 = motor_ptr->mpos.est.filter_fc_speed_q16;
            break;
        case 104:
            parameter_value1 = (motor_ptr->mpos.est.angcorr_gain_q20 >> 4);
            parameter_value2 = motor_ptr->mpos.est.angcorr_offset;
            break;
        case 105:
            parameter_value1 = motor_ptr->mpos.est.kpe_store_q16;
            parameter_value2 = motor_ptr->mpos.est.kpi_store_q16;
            break;
        case 106:
            parameter_value1 = motor_ptr->mpos.est.kpe_const_q14 << 2;
            parameter_value2 = motor_ptr->mpos.est.kpe_vrble_q14 << 2;
            parameter_value3 = motor_ptr->mpos.est.kpi_const_q14 << 2;
            break;

    #ifdef ENABLE_PLL_ANGLE_CALCULATION
        case 107:
            parameter_value1 = motor_ptr->mpos.est_pll.pll_ka_target_q16;
            parameter_value2 = motor_ptr->mpos.est_pll.pll_kb_target_q16;
            parameter_value3 = motor_ptr->mpos.est_pll.est_pll_transition_complete;
            break;

        case 108:
            parameter_value1 = motor_ptr->mpos.est_pll.pll_pid.ka_q16;
            parameter_value2 = motor_ptr->mpos.est_pll.pll_pid.kb_q16;
            parameter_value3 = motor_ptr->mpos.est_pll.nom_spd_1_div_q26 >> 10;
            break;
    #endif

        case 109:
            parameter_value1 = motor_ptr->mpos.est.w0_q06 << 10;
            parameter_value2 = motor_ptr->mpos.est.w0_target_q06 << 10;
            parameter_value3 = motor_ptr->mpos.est.w0_ramp_q06 << 10;
            break;
        case 110:
            parameter_value1 = motor_ptr->control.u_duty_value;
            parameter_value2 = motor_ptr->control.v_duty_value;
            parameter_value3 = motor_ptr->control.w_duty_value;
            break;
#endif // #if (MPOS_SELECT == MPOS_LBG1)
        
// Power
#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
        case 111:
            parameter_value1 = motor_ptr->power.current_descale_factor_q16;
            break;

        case 112:
            parameter_value1 = motor_ptr->power.filter_alpha;
            break;

        case 113:
            parameter_value1 = motor_ptr->power.power_iphase_offset_q14 << 2;
            parameter_value2 = motor_ptr->power.power_iphase_factor_q14 << 2;
            break;

        case 114:
            break;
#elif defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)

        case 111:
            parameter_value1 = motor_ptr->power.current_descale_factor_q16;
            parameter_value2 = AMPGAIN_AIO6 << 16;
            break;

        case 112:
            parameter_value1 = motor_ptr->power.filter_alpha;
            parameter_value2 = motor_ptr->power.adc_offset_ibusavg_q14;
            break;

        case 113:
            parameter_value1 = ADC_OFFSET_MIN_AIO6 << 16;
            parameter_value2 = AMPGAIN_AIO6 << 16;
            parameter_value3 = (ADC_OFFSET_MIN_AIO6 * AMPGAIN_AIO6) << 16;
            break;

        case 114:
            parameter_value1 = ADC_OFFSET_MAX_AIO6 << 16;
            parameter_value2 = AMPGAIN_AIO6 << 16;
            parameter_value3 = (ADC_OFFSET_MAX_AIO6 * AMPGAIN_AIO6) << 16;
            break;
#endif

            // Shunts
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
        case 120:
            parameter_value1 = motor_ptr->threeshunt.adc_offset_iphase_u_q14;
            parameter_value2 = motor_ptr->threeshunt.adc_offset_iphase_v_q14;
            parameter_value3 = motor_ptr->threeshunt.adc_offset_iphase_w_q14;
            break;
#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
        case 120:
            parameter_value1 = motor_ptr->oneshunt.adc_offset_iphase_ss_q14;
            break;
#endif

        case 125:
            parameter_value1 = motor_ptr->control.duty_max_pct_q14 << 2;
            parameter_value2 = motor_ptr->control.duty_max_pct_1div_q14 << 2;
            break;

        case 126:
            parameter_value1 = DT_DIVSYSTICK_Q16;
            break;

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) && defined(ENABLE_ANGLEDIFF_TIMEOUT)
        case 127:
            parameter_value1 = ANGLEDIFF_TIMEOUT_COUNT;
            break;
#endif

#if (INTERNAL_VMS == TRUE)
        case 128:
            parameter_value1 = motor_ptr->vms_v100;
            parameter_value2 = motor_ptr->vms_v200;
            parameter_value3 = motor_ptr->one_div_vms200_minus_vms100;
            break;
        case 129:
//            parameter_value1 = READ_UINT16(VMS100_ADDRESS);
//            parameter_value2 = READ_UINT16(VMS200_ADDRESS);
//            parameter_value3 = readseq_vbus();
            break;
#endif

#if defined(ENABLE_HALL_TUNING)
        case 130:
            parameter_value1 = motor_ptr->halltune.gui_cmd;
            break;
        case 131:
            parameter_value1 = motor_ptr->halltune.state << 16;
            parameter_value2 = motor_ptr->mpos.aligngo.state_main << 16;
            parameter_value3 = motor_ptr->mpos.aligngo.state_control << 16;
            break;
        case 132:
            parameter_value1 = motor_ptr->halltune.aligngo.aligntime_cmd;
            parameter_value2 = motor_ptr->halltune.aligngo.speedrefol_cmd_q26;
            parameter_value3 = motor_ptr->halltune.aligngo.speedrefol_ramprate_cmd_q26;
            break;
        case 133:
            parameter_value1 = motor_ptr->halltune.aligngo.itotrefol_cmd_q16;
            parameter_value2 = motor_ptr->halltune.aligngo.idrefol_cmd_q16;
            break;
        case 134:
            break;
        case 135:
            parameter_value1 = motor_ptr->halltune.hallstate_angle_tuning[1];
            parameter_value2 = motor_ptr->halltune.hallstate_angle_tuning[2];
            parameter_value3 = motor_ptr->halltune.hallstate_angle_tuning[3];
            break;
        case 136:
            parameter_value1 = motor_ptr->halltune.hallstate_angle_tuning[4];
            parameter_value2 = motor_ptr->halltune.hallstate_angle_tuning[5];
            parameter_value3 = motor_ptr->halltune.hallstate_angle_tuning[6];
            break;
        case 137:
            break;
        case 138:
            break;
        case 139:
            parameter_value1 = motor_ptr->halltune.hallstate_angle_ref[1];
            parameter_value2 = motor_ptr->halltune.hallstate_angle_ref[2];
            parameter_value3 = motor_ptr->halltune.hallstate_angle_ref[3];
            break;
        case 140:
            parameter_value1 = motor_ptr->halltune.hallstate_angle_ref[4];
            parameter_value2 = motor_ptr->halltune.hallstate_angle_ref[5];
            parameter_value3 = motor_ptr->halltune.hallstate_angle_ref[6];
            break;
        case 141:
            break;
        case 142:
            break;
        case 143:
            break;
        case 144:
            break;
#endif

#if (MODULE_MPOS_HALL == ENABLED)
        case 145:
            parameter_value1 = motor_ptr->mpos.hall.hallstate_angle_ref[1];
            parameter_value2 = motor_ptr->mpos.hall.hallstate_angle_ref[2];
            parameter_value3 = motor_ptr->mpos.hall.hallstate_angle_ref[3];
            break;
        case 146:
            parameter_value1 = motor_ptr->mpos.hall.hallstate_angle_ref[4];
            parameter_value2 = motor_ptr->mpos.hall.hallstate_angle_ref[5];
            parameter_value3 = motor_ptr->mpos.hall.hallstate_angle_ref[6];
            break;
        case 147:
            parameter_value1 = fix16_mul(motor_ptr->mpos.hall.hall_sensor_offset_rad, CONV_RAD2DEG_FIX16);
            parameter_value2 = fix16_mul((motor_ptr->mpos.hall.halledge_polskew_time_q26 >> 10), QCONST(1000, Q16));
            break;
        case 148:
            break;
        case 149:
            parameter_value1 = motor_ptr->startup.i_total_hall_gui;
            parameter_value2 = motor_ptr->mpos.hall.switchover_spd_lo2hi_radps;
            parameter_value3 = motor_ptr->mpos.hall.switchover_spd_hi2lo_radps;
            break;
        case 150:
            parameter_value1 = motor_ptr->startup.i_total_hall_gui;
            parameter_value2 = motor_ptr->startup.state_main << 16;
            parameter_value3 = HALL_TIMER_TICK_DT_Q26;
            break;
        case 151:
            parameter_value1 = motor_ptr->mpos.hall.angcorr_gain_fwd_q20;
            parameter_value2 = motor_ptr->mpos.hall.angcorr_offset_fwd;
//            parameter_value3 = 0;
            break;
        case 152:
            parameter_value1 = motor_ptr->mpos.hall.angcorr_gain_rev_q20;
            parameter_value2 = motor_ptr->mpos.hall.angcorr_offset_rev;
//            parameter_value3 = 0;
            break;
        case 153:
            parameter_value1 = motor_ptr->mpos.hall.hall_forward_next_state[1];
            parameter_value2 = motor_ptr->mpos.hall.hall_forward_next_state[2];
            parameter_value3 = motor_ptr->mpos.hall.hall_forward_next_state[3];
            break;
        case 154:
            parameter_value1 = motor_ptr->mpos.hall.hall_forward_next_state[4];
            parameter_value2 = motor_ptr->mpos.hall.hall_forward_next_state[5];
            parameter_value3 = motor_ptr->mpos.hall.hall_forward_next_state[6];
            break;
        case 155:
            parameter_value1 = motor_ptr->mpos.hall.hall_forward_prev_state[1];
            parameter_value2 = motor_ptr->mpos.hall.hall_forward_prev_state[2];
            parameter_value3 = motor_ptr->mpos.hall.hall_forward_prev_state[3];
            break;
        case 156:
            parameter_value1 = motor_ptr->mpos.hall.hall_forward_prev_state[4];
            parameter_value2 = motor_ptr->mpos.hall.hall_forward_prev_state[5];
            parameter_value3 = motor_ptr->mpos.hall.hall_forward_prev_state[6];
            break;
        case 157:
            parameter_value1 = motor_ptr->mpos.hall.hallstate_angle_fwd[1];
            parameter_value2 = motor_ptr->mpos.hall.hallstate_angle_fwd[2];
            parameter_value3 = motor_ptr->mpos.hall.hallstate_angle_fwd[3];
            break;
        case 158:
            parameter_value1 = motor_ptr->mpos.hall.hallstate_angle_fwd[4];
            parameter_value2 = motor_ptr->mpos.hall.hallstate_angle_fwd[5];
            parameter_value3 = motor_ptr->mpos.hall.hallstate_angle_fwd[6];
            break;
        case 159:
            parameter_value1 = motor_ptr->mpos.hall.hallstate_angle_rev[1];
            parameter_value2 = motor_ptr->mpos.hall.hallstate_angle_rev[2];
            parameter_value3 = motor_ptr->mpos.hall.hallstate_angle_rev[3];
            break;
        case 160:
            parameter_value1 = motor_ptr->mpos.hall.hallstate_angle_rev[4];
            parameter_value2 = motor_ptr->mpos.hall.hallstate_angle_rev[5];
            parameter_value3 = motor_ptr->mpos.hall.hallstate_angle_rev[6];
            break;
        case 161:
            parameter_value1 = motor_ptr->mpos.hall.hall_check_error_code << 16;
            parameter_value2 = motor_ptr->mpos.hall.hall_check_error_state_new << 16;
            parameter_value3 = motor_ptr->mpos.hall.hall_check_error_state_prev << 16;
            break;
#endif

#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
        case 170:
            parameter_value1 = motor_ptr->app.dcbus_voltage_fast_q14;
            parameter_value2 = motor_ptr->app.dcbus_voltage_ave_q14 << 2;
            parameter_value3 = motor_ptr->app.dcbus_voltage_compensation_coff_q26 >> 10;
            break;
        case 171:
            parameter_value1 = motor_ptr->app.dc_voltage_filter_coff_q16;
            //parameter_value2 = motor_ptr->app.dcbus_voltage_ave_q14 << 2;
            //parameter_value3 = motor_ptr->app.dcbus_voltage_compensation_coff_q26 >> 10;
            break;
        
        
#endif
        
#if (MODULE_MPOS_ALIGNGO == ENABLED)
        case 175:
            parameter_value2 = motor_ptr->mpos.aligngo.state_main << 16;
            parameter_value3 = motor_ptr->mpos.aligngo.state_control << 16;
            break;
        case 176:
            parameter_value1 = motor_ptr->mpos.aligngo.aligntime_target;
            parameter_value2 = motor_ptr->mpos.aligngo.aligntime_counter;
            parameter_value3 = motor_ptr->mpos.aligngo.aligntime_target;
            break;
        case 177:
            parameter_value1 = motor_ptr->mpos.aligngo.idrefol_target_q16;
            parameter_value2 = motor_ptr->mpos.aligngo.itotrefol_target_q16;
            parameter_value3 = motor_ptr->mpos.aligngo.speedrefol_target_q26;
            break;
        case 178:
            parameter_value1 = motor_ptr->mpos.aligngo.speedrefol_target_q26;
            parameter_value2 = motor_ptr->mpos.aligngo.speedrefol_ramp_q26;
            parameter_value3 = motor_ptr->mpos.aligngo.speedrefol_ramprate_q26;
            break;
        case 179:
            parameter_value1 = (motor_ptr->foc.idref_ramp_f16);
            parameter_value2 = (motor_ptr->mpos.aligngo.idrefol_target_q16);
            parameter_value3 = (motor_ptr->mpos.aligngo.itotrefol_target_q16);
            break;
        case 180:
            parameter_value1 = motor_ptr->mpos.aligngo.itotrefol_target_q16;
            parameter_value2 = motor_ptr->app.app_motor_direction_cmd;
            parameter_value3 = motor_ptr->app.app_motor_direction;
#ifdef ENABLE_ANGLEDIFF_AUTORANGE
            //parameter_value2 = ((motor_ptr->startup.aligngo.itotrefol_target_q16 * QCONST(ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT, Q10)) >> 10);
#endif
            break;
        case 181:
            parameter_value3 = motor_ptr->mpos.aligngo.itotrefol_target_1div_q16;
            break;

        case 182:
            parameter_value1 = motor_ptr->mpos.aligngo.alignangle_target_q26 >> 10;
            parameter_value2 = motor_ptr->mpos.aligngo.alignangle_q26 >> 10;
            break;

#endif

#if (MODULE_MPOS_QEP == ENABLED)
        case 185:
            parameter_value1 = motor_ptr->mpos.qep.counter_counts;
            parameter_value2 = motor_ptr->mpos.qep.conv_qep2mech_q24;
            parameter_value3 = motor_ptr->mpos.qep.qep_resolution;
            break;

        case 186:
            parameter_value1 = *motor_ptr->mpos.qep.ptr_angcorr_offset;
            parameter_value2 = motor_ptr->mpos.qep.angcorr_offset_fwd;
            parameter_value3 = motor_ptr->mpos.qep.angcorr_offset_rev;
            break;

    #if defined(ENABLE_QEP_LINEARIZATION_TUNING)
        case 187:
            parameter_value1 = motor_ptr->mpos.qep.index_count << 16;
            parameter_value2 = motor_ptr->mpos.qep.qep_linearization_start_flag << 16;
            parameter_value3 = motor_ptr->mpos.qep.qep_linearization_done_flag << 16;
            break;

        case 188:
            parameter_value1 =  motor_ptr->mpos.qep.convert_rad_to_qep_count_q10 << 6;
            parameter_value2 = motor_ptr->mpos.qep.qep_linearization_enable_cmd;
            //parameter_value3 = motor_ptr->mpos.qep.qep_linearization_done_flag << 16;
            break;
    #endif

    #if (defined(ENABLE_QEP_LINEARIZATION_TUNING) || defined(ENABLE_QEP_LIN_FLASHRW))
        case 189:
            parameter_value1 = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[0] << 16;
            parameter_value2 = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[1] << 16;
            parameter_value3 = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[2] << 16;
            break;
    #endif

    #if (defined(ENABLE_FLASH_RW) && defined(ENABLE_QEP_LIN_FLASHRW))
        case 190:
            parameter_value1 = READ_INT32(QEP_LIN_FLASH_START_ADDR);
            parameter_value2 = READ_INT32(QEP_LIN_FLASH_START_ADDR + 1);
            parameter_value3 = READ_INT32(QEP_LIN_FLASH_START_ADDR + 2);
            break;

        case 191:
            parameter_value1 = READ_INT32(QEP_LIN_FLASH_START_ADDR + 3);
            parameter_value2 = READ_INT32(QEP_LIN_FLASH_START_ADDR + 4);
            parameter_value3 = READ_INT32(QEP_LIN_FLASH_START_ADDR + 5);
            break;

        case 192:
            parameter_value1 = READ_INT32(QEP_LIN_FLASH_END_ADDR - 2);
            parameter_value2 = READ_INT32(QEP_LIN_FLASH_END_ADDR - 1);
            parameter_value3 = READ_INT32(QEP_LIN_FLASH_END_ADDR);
            break;
    #endif

    #if defined(ENABLE_QEP_OFFSET_TUNING)
        case 193:
            parameter_value1 = motor_ptr->mpos.qep.qep_offset_tuning_enable_cmd;
            parameter_value2 = motor_ptr->mpos.qep.qep_offset_angle_deg_q16;
            break;
    #endif

        case 194:
            parameter_value1 = motor_ptr->startup.state << 16;
            break;
        case 195:
            parameter_value1 = motor_ptr->startup.aligngo.alignangle_cmd_q26;
            parameter_value2 = motor_ptr->startup.aligngo.aligntime_cmd;
            parameter_value3 = motor_ptr->startup.aligngo.speedrefol_cmd_q26;
            break;
        case 196:
            parameter_value1 = motor_ptr->startup.aligngo.speedrefol_ramprate_cmd_q26;
            parameter_value2 = motor_ptr->startup.aligngo.idrefol_cmd_q16;
            parameter_value3 = motor_ptr->startup.aligngo.itotrefol_cmd_q16;
            break;
#endif
#if defined(ENABLE_AUTOTUNE_PI)
        case 228:
            parameter_value1 = motor_ptr->foc.Iq_pid.ka_q16;
            parameter_value2 = motor_ptr->foc.Iq_pid.kb_q16;
            break;

        case 229:
            parameter_value1 = motor_ptr->foc.Id_pid.ka_q16;
            parameter_value2 = motor_ptr->foc.Id_pid.kb_q16;
            break;

        case 230:
            parameter_value1 = motor_ptr->control.outerloop_pid.ka_q16;
            parameter_value2 = motor_ptr->control.outerloop_pid.kb_q16;
            break;

        case 231:
            parameter_value1 = motor_ptr->foc.Id_pid.kp_q11 << 5;
            parameter_value2 = motor_ptr->foc.Id_pid.kidt_q16;
            break;

        case 232:
            parameter_value1 = motor_ptr->foc.Iq_pid.kp_q11 << 5;
            parameter_value2 = motor_ptr->foc.Iq_pid.kidt_q16;
            break;

        case 233:
            parameter_value1 = motor_ptr->control.outerloop_pid.kp_q11 << 5;
            parameter_value2 = motor_ptr->control.outerloop_pid.kidt_q16;
            break;

        case 234:
            parameter_value1 = fix16_mul(motor_ptr->mparam.ke_q26, QCONST(1000.0f, Q16)) >> 10;                    // Units: milli
            parameter_value2 = fix16_mul(motor_ptr->mparam.j_q26, QCONST(1000.0f, Q16)) >> 10;                     // Units: milli
            parameter_value3 = fix16_mul(motor_ptr->atune.filter_tau_speed_q26, QCONST(1000.0f, Q16)) >> 10;        // Units: msec
            break;

        case 235:
            parameter_value1 = fix16_mul(motor_ptr->atune.dt_control_q26, QCONST(1000.0f, Q16)) >> 10;  // Units: msec
            parameter_value2 = fix16_mul(motor_ptr->atune.dt_speed_q26, QCONST(1000.0f, Q16)) >> 10;     // Units: msec
            break;

        case 236:
            parameter_value1 = motor_ptr->app.v_scale_q16;
            parameter_value2 = motor_ptr->app.outerloop_scale_q0 << 16;
            break;

        case 237:
            parameter_value1 = motor_ptr->atune.istep_q16;
            parameter_value2 = motor_ptr->atune.delta_q6 << 10;
            parameter_value3 = motor_ptr->atune.anoise_thd_q22 >> 6;
            break;

        case 238:
            parameter_value2 = motor_ptr->atune.filter_alpha_dc_q16;
            break;

        case 240:
            parameter_value1 = motor_ptr->atune.ke_q26 >> 10;
            parameter_value2 = motor_ptr->atune.j_q26 >> 10;
            break;

        case 241:
            parameter_value1 = motor_ptr->atune.id_ka_q16;
            parameter_value2 = motor_ptr->atune.id_kb_q16;
            break;

        case 242:
            parameter_value1 = motor_ptr->atune.iq_ka_q16;
            parameter_value2 = motor_ptr->atune.iq_kb_q16;
            break;

        case 243:
            parameter_value1 = motor_ptr->atune.speed_ka_q16;
            parameter_value2 = motor_ptr->atune.speed_kb_q16;
            break;

        case 245:
            parameter_value1 = motor_ptr->atune.state_mid2 << 16;
            parameter_value2 = motor_ptr->atune.status_atune;
//            parameter_value3 = motor_ptr->atune.uicmd_atune << 16;
            parameter_value3 = motor_ptr->atune.anoise_warn_ctr << 16;
            break;

        case 246:
            parameter_value1 = motor_ptr->atune.mid1_mancmd << 16;
            parameter_value2 = motor_ptr->atune.mid1_injmode << 16;
            parameter_value3 = motor_ptr->atune.mid1_runmode << 16;
            break;

        case 247:
            parameter_value1 = motor_ptr->atune.v_led_scaled_q14 << 2;
            parameter_value2 = motor_ptr->atune.v_ted_scaled_q14 << 2;
            break;

        case 248:
            parameter_value1 = motor_ptr->atune.v_diode_scaled_q14 << 2;
            parameter_value2 = motor_ptr->atune.v_ce_scaled_q14 << 2;
            parameter_value3 = motor_ptr->atune.v_dc_q14 << 2;
            break;

        case 249:
            parameter_value1 = motor_ptr->atune.i_dc_q14 << 2;
            parameter_value2 = motor_ptr->atune.i_align_limit1_q14 << 2;
            parameter_value3 = motor_ptr->atune.v1_align_q14 << 2;
            break;

        case 250:
//            parameter_value1 = motor_ptr->atune.v_align_inc_q14 << 16;
            parameter_value1 = motor_ptr->atune.status_atune << 16;
            parameter_value2 = motor_ptr->atune.status_mid1 << 16;
            parameter_value3 = motor_ptr->state.main << 16;
            break;

#if defined(ENABLE_AUTOTUNE_MID)
        case 251:
//            parameter_value1 = motor_ptr->atune.v_dc1_q14 << 2;
//            parameter_value2 = motor_ptr->atune.i_dc1_q14 << 2;
            parameter_value2 = motor_ptr->atune.state_mainsub_mid1 << 16;
            parameter_value3 = get_amid_flow_state_machine_state(&motor_ptr->atune) << 16;
            break;
#endif

        case 252:
            parameter_value1 = motor_ptr->atune.v_dc2_q14 << 2;
            parameter_value2 = motor_ptr->atune.i_dc2_q14 << 2;
            parameter_value3 = motor_ptr->atune.rs_q16;
            break;

        case 253:
            parameter_value1 = motor_ptr->atune.v2_align_q14 << 2;
            parameter_value2 = motor_ptr->atune.filter_delay_q0;
            parameter_value3 = motor_ptr->atune.filter_delay_config_q0;
            break;

        case 254:
            break;

        case 255:
            parameter_value1 = motor_ptr->atune.rs_q16;
            parameter_value2 = motor_ptr->atune.ld_q26;
            parameter_value3 = motor_ptr->atune.lq_q26;
            break;

        case 256:
            parameter_value1 = motor_ptr->atune.i_align1_gui_q14 << 2;
            parameter_value2 = motor_ptr->atune.i_align2_gui_q14 << 2;
            parameter_value3 = motor_ptr->atune.i_inj_gui_q14 << 2;
            break;

        case 257:
            parameter_value1 = motor_ptr->atune.v1_align_q14 << 2;
            parameter_value2 = motor_ptr->atune.v2_align_q14 << 2;
            parameter_value3 = motor_ptr->atune.v_inj_q14 << 2;
            break;

        case 258:
            parameter_value1 = motor_ptr->atune.num_limit;
            parameter_value2 = motor_ptr->atune.num_limit_gui;
            parameter_value3 = motor_ptr->atune.power_factor_sq_q16;
            break;

        case 259:
            parameter_value1 = motor_ptr->atune.rcheck_rd_q16;
            parameter_value2 = motor_ptr->atune.rcheck_rq_q16;
            break;

        case 260:
            parameter_value1 = motor_ptr->atune.motiondet_delta_threshold_pct_q14;
            parameter_value2 = motor_ptr->atune.motiondet_delay_limit_cnt;
            parameter_value3 = motor_ptr->atune.motiondet_to_limit_cnt;
            break;

#endif

#ifdef INFOROM_DATA
        case 261:
            parameter_value1 = READ_UINT8(0x00100040);  // Read byte at INFO ROM address 0x00100040
            parameter_value2 = READ_UINT8(0x00100041);
            parameter_value3 = READ_UINT8(0x00100042);
            break;
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)
        case 262:
            parameter_value1 = motor_ptr->mpos.est.dampfactor_q16;
            parameter_value2 = motor_ptr->mpos.est.w0_q06 << 10;
            parameter_value3 = motor_ptr->mparam.nom_spd_q16;
            break;
#endif

#if defined(FAMILY_55XX)
        case 263:
            parameter_value1 = READ_INT32(ADC_OFFSET_ADDRESS);
            parameter_value2 = READ_INT32(ADC_GAIN_ADDRESS);
            parameter_value3 = READ_INT32(DEVICE_INFO_ADDRESS2) & (0x00FFFFFF);
            break;
#endif

#if defined(FAMILY_52XX)
        case 263:
            parameter_value1 = READ_INT32(ADC_OFFSET_ADDRESS) & (0x0000FFFF);
            parameter_value2 = READ_INT32(ADC_GAIN_ADDRESS) & (0x0000FFFF);
            parameter_value3 = READ_INT32(DEVICE_INFO_ADDRESS2) & (0x00FFFFFF);
            break;
#endif

#if (COMMSEL_PPM == TRUE)        
            case 264:
                parameter_value1 = PAC55XX_TIMERC->CCTR3.CAP1;
                parameter_value2 = PAC55XX_TIMERC->CCTR3.CAP2;
                parameter_value3 = PPM_TIMER_TIME_PERIOD;
                break;
            case 265:
                parameter_value1 = motor_ptr->ppm.ppm_pulse_delta;
                parameter_value2 = MIN_PPM_PULSE_WIDTH_TCOUNTS;
                parameter_value3 = MAX_PPM_PULSE_WIDTH_TCOUNTS;
                break;
            case 266:
                parameter_value1 = motor_ptr->ppm.pulse_time;
                parameter_value2 = MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS;
                parameter_value3 = MIN_PPM_START_TCOUNTS - MIN_PPM_START_HYST_TCOUNTS;
                break;
            case 267:
                  parameter_value1 = motor_ptr->startup.aligngo.speedrefol_target_radps;
                parameter_value2 = motor_ptr->ppm.valid_pulse_counter_temp;
                  break;
            case 268:
                parameter_value1 = motor_ptr->control.outerloop_target;
                parameter_value2 = motor_ptr->control.outerloop_ramp;
                parameter_value3 = MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS;
                  break;    
#endif
        
            case 271:
#if defined(FAMILY_52XX)
                parameter_value1 = DEVICE_SELECT;
                parameter_value2 = (READ_INT32(DEVICE_INFO_ADDRESS2) & 0xFFFF);
#elif defined(FAMILY_55XX)
                parameter_value1 = DEVICE_SELECT;
                parameter_value2 = (READ_INT32(DEVICE_INFO_ADDRESS2) & 0xFFFFFF);
#endif
                break;

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE))
            case 272:
                parameter_value1 = (motor_ptr->sim.phase_adc_threshold_q14 << 2);
                parameter_value2 = (motor_ptr->sim.vphase_u_q14 << 2);
                parameter_value3 = (motor_ptr->sim.vphase_v_q14 << 2);
                break;
            case 273:
                parameter_value1 = (motor_ptr->sim.phase_adc_threshold_q14 << 2);
                parameter_value2 = (motor_ptr->sim.vphase_w_q14 << 2);
                break;
#elif defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
            case 272:
                parameter_value1 = (motor_ptr->sim.phase_adc_threshold_q14 << 2);
                parameter_value2 = (motor_ptr->sim.iphase_u_q14 << 2);
                parameter_value3 = (motor_ptr->sim.iphase_v_q14 << 2);
                break;
            case 273:
                parameter_value1 = (motor_ptr->sim.phase_adc_threshold_q14 << 2);
                parameter_value2 = (motor_ptr->sim.iphase_w_q14 << 2);
                break;
#endif

#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
            case 290:
                parameter_value1 = readseq_vphase_u_raw_trap();
                parameter_value2 = readseq_vphase_v_raw_trap();
                parameter_value3 = readseq_vphase_w_raw_trap();
                break;

            case 291:
                parameter_value1 = motor_ptr->mpos.trap.bemf_conv_adc2volts_scaled_q16;
                //parameter_value2 = readseq_vphase_v_raw_trap();
                //parameter_value3 = readseq_vphase_w_raw_trap();
                break;
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
            case 292:
                parameter_value2 = motor_ptr->mpos.trap.switchover_spd_lo2hi_radps;
                parameter_value3 = motor_ptr->mpos.trap.switchover_spd_hi2lo_radps;
                break;

            case 293:
                parameter_value1 = debug_input_1 << 16;
                parameter_value2 = motor_ptr->mpos.trap.trap_est_angle_diff;
                break;
#endif

            case 300:
                parameter_value1 = motor_ptr->foc.dt_control_q26;
                parameter_value2 = motor_ptr->foc.ctrl_divider_foc << 16;
                break;
            case 301:
                parameter_value1 = motor.fptr.pSeq_add->w;
                break;
            case 302:
                parameter_value1 = motor_ptr->mparam.fpole_q16;
                break;
            case 303:
                break;

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
            case 310:
                aligngo_setvars(motor_ptr, &motor_ptr->startup.aligngo);
                break;
            case 311:
                parameter_value1 = motor_ptr->startup.aligngo.aligntime_cmd;
                parameter_value2 = motor_ptr->mpos.aligngo.aligntime_target;
                parameter_value3 = motor_ptr->mpos.aligngo.aligntime_counter;
                break;
            case 312:
                parameter_value1 = motor_ptr->startup.aligngo.speedrefol_cmd_q26;
                parameter_value2 = motor_ptr->mpos.aligngo.speedrefol_target_q26;
                parameter_value3 = motor_ptr->mpos.aligngo.speedrefol_ramp_q26;
                break;
            case 313:
                parameter_value1 = motor_ptr->mpos.aligngo.speedrefol_target_radps_q16;
                break;
            case 314:
                parameter_value1 = motor_ptr->startup.aligngo.speedrefol_ramprate_cmd_q26;
                parameter_value2 = motor_ptr->mpos.aligngo.speedrefol_ramprate_q26;
                break;
            case 315:
                parameter_value1 = motor_ptr->startup.aligngo.itotrefol_cmd_q16;
                parameter_value2 = motor_ptr->mpos.aligngo.itotrefol_target_q16;
                parameter_value3 = motor_ptr->mpos.aligngo.itotrefol_target_1div_q16;
                break;
            case 316:
                parameter_value1 = motor_ptr->startup.aligngo.idrefol_cmd_q16;
                parameter_value2 = motor_ptr->mpos.aligngo.idrefol_target_q16;
                break;
#endif

            case 317:
                parameter_value1 = motor_ptr->mpos.state;
                break;
#if defined(ENABLE_FORCEANG)
            case 318:
                parameter_value1 = motor_ptr->state.main;
                parameter_value2 = motor_ptr->forceang.forceang_state;
                break;

            case 319:
                parameter_value1 = motor_ptr->forceang.aligngo.alignangle_cmd_q26 >> 10;
                parameter_value2 = motor_ptr->mpos.aligngo.alignangle_target_q26 >> 10;
                parameter_value3 = motor_ptr->mpos.aligngo.alignangle_q26 >> 10;
                break;
#endif

#if defined(ENABLE_HALL_TUNING)
            case 310:
                aligngo_setvars(motor_ptr, &motor_ptr->halltune.aligngo);
                break;
            case 311:
                parameter_value1 = motor_ptr->halltune.aligngo.aligntime_cmd;
                parameter_value2 = motor_ptr->mpos.aligngo.aligntime_target;
                parameter_value3 = motor_ptr->mpos.aligngo.aligntime_counter;
                break;
            case 312:
                parameter_value1 = motor_ptr->halltune.aligngo.speedrefol_cmd_q26;
                parameter_value2 = motor_ptr->mpos.aligngo.speedrefol_target_q26;
                parameter_value3 = motor_ptr->mpos.aligngo.speedrefol_ramp_q26;
                break;
            case 313:
                parameter_value1 = motor_ptr->mpos.aligngo.speedrefol_target_radps_q16;
                break;
            case 314:
                parameter_value1 = motor_ptr->halltune.aligngo.speedrefol_ramprate_cmd_q26;
                parameter_value2 = motor_ptr->mpos.aligngo.speedrefol_ramprate_q26;
                break;
            case 315:
                parameter_value1 = motor_ptr->halltune.aligngo.itotrefol_cmd_q16;
                parameter_value2 = motor_ptr->mpos.aligngo.itotrefol_target_q16;
                parameter_value3 = motor_ptr->mpos.aligngo.itotrefol_target_1div_q16;
                break;
            case 316:
                parameter_value1 = motor_ptr->halltune.aligngo.idrefol_cmd_q16;
                parameter_value2 = motor_ptr->mpos.aligngo.idrefol_target_q16;
                break;
#endif

#if defined(ENABLE_AUTOTUNE_MID)
            case 320:
                parameter_value1 = motor_ptr->atune.reqx_q16[0];
                parameter_value2 = motor_ptr->atune.reqx_q16[1];
                parameter_value3 = motor_ptr->atune.reqx_q16[2];
                break;
            case 321:
                break;
            case 322:
                parameter_value1 = motor_ptr->atune.ldx_q26[0];
                parameter_value2 = motor_ptr->atune.ldx_q26[1];
                parameter_value3 = motor_ptr->atune.ldx_q26[2];
                break;
            case 323:
                parameter_value1 = motor_ptr->atune.lqx_q26[0];
                parameter_value2 = motor_ptr->atune.lqx_q26[1];
                parameter_value3 = motor_ptr->atune.lqx_q26[2];
                break;
            case 324:
                parameter_value1 = motor_ptr->atune.rs_q16;
                parameter_value2 = motor_ptr->atune.ld_q26;
                parameter_value3 = motor_ptr->atune.lq_q26;
                break;
#endif

#if defined(ENABLE_FLASH_RW)
            case 325:
                parameter_value1 = motor_ptr->flash_read_wr.flash_status;    
                break;
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_LBG1_START_QEP)
            case 326:
                parameter_value1 = motor_ptr->mpos.mposmode;
                break;
#endif

#ifdef ENABLE_BUZZ
        case 400:
            parameter_value1 = (motor_ptr->buzz.cmd_buzz.word << 16);
            parameter_value2 = (motor_ptr->buzz.state_buzz << 16);
            break;
        case 401:
            break;
        case 402:
            command_buzz(&melody1, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 403:
            break;
        case 404:
            command_buzz(&melody2, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 405:
            break;
        case 406:
            command_buzz(&melody3, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 407:
            break;
        case 408:
            command_buzz(&melody4, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 409:
            break;
        case 410:
            command_buzz(&melody5, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 411:
            break;
        case 412:
            command_buzz(&melody6, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 413:
            break;
        case 414:
            command_buzz(&melody7, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
        case 415:
            break;
        case 416:
            command_buzz(&melody8, 0, BUZZ_VOLUME_Q08, motor_ptr);
            break;
#endif

#if defined(ENABLE_DEBUG_FLASHDEMO)
#if defined(ENABLE_FLASHDEMO)
#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR) || (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
        case 417:
            flashdemo_data_clear();    
            break;
        case 418:
            flashdemo_data_set();
            parameter_value1 = crc_8_calculation(0x00, (uint8_t*)flash_test_data, (FLASHDEMO_FLASH_WORD_ARRAY_SIZE << 2)) << 16;
            break;
        case 419:
            flashdemo_param_process(FLASH_read, 0);
            break;
        case 420:
            flashdemo_param_process(FLASH_write, 0);
            break;
        case 421:
            if (count_num >= FLASHDEMO_FLASH_WORD_ARRAY_SIZE)
                count_num = 0;
            parameter_value1 = flash_test_data[count_num] << 16; 
            count_num++;
            parameter_value2 = flash_test_data[count_num] << 16;
            count_num++;
            parameter_value3 = flash_test_data[count_num] << 16;
            break;
        case 422:
            parameter_value1 = count_num << 16;
            count_num = 0;
            parameter_value2 = count_num << 16;
            break;
        case 423:
        {
            Struct_Flash_Rd_Wr_DyCtr* ptr = flashdemo_get_address();
            parameter_value1 = ptr->isInit << 16;
            parameter_value2 = ptr->head << 16;
            parameter_value3 = ptr->tail << 16;;
            break;
        }
#endif    //#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR) || (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
#endif    //#if defined(ENABLE_FLASHDEMO)
#endif    //#if defined (ENABLE_DEBUG_FLASHDEMO)
        default:
            break;
    }
}

#endif  // #ifdef ENABLE_DEBUG_PARAMEXPORT
