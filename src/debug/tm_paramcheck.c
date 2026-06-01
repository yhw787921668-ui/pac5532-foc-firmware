
//=============================================================================
// Copyright (C) 2018 - 2019, Qorvo, Inc.
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
#include "tm.h"
#include "mpos_funcs.h"

#if defined(ENABLE_TM) && defined(TM_PARAMCHECK)
void parameter_check(ParameterCode param_code, fix16_t rx_data, Struct_Motor* motor_ptr)
{
    switch (param_code)
    {
// Motor Config
        case current_limit:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.current_limit;
            break;
        case dt:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.dt_control_q26;
            break;
        case PWM_to_FOC_control_ratio:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.ctrl_divider_foc;
            break;
        case descale_current_factor:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.descale_current_factor;
            break;
        case diff_amp_gain:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.iphase_amp_gain;
            break;
        case cl_mode_selection:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.speed_mode_flag;
            break;
        case reverse_direction:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.app_motor_direction;
            break;
        case Auto_Mode_Open_Close_Loop:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.auto_close_loop_cmd;
            break;

// Id & Iq
        case id_pi_filter_coeff:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Id_filter_gain;
            break;
        case iq_pi_filter_coeff:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Iq_filter_gain;
            break;
        case idiq_ramp_rate:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.idiqref_rate_f16;
            break;

// Closed Loop
        case closed_loop_speed_slew_rate:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_rate_store;
            break;
        case closed_loop_speed_setpoint:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_target;
            break;
        case id_ref_cl:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.idrefcl_cmd_f16;
            break;
        case iq_ref_cl:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.iqrefcl_cmd_f16;
            break;
        case max_cl_speed_limit:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.speedrefcl_max;
            break;

// Speed PID
        case speed_pid_Kp:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_pid.ka_q16;
            break;
        case speed_pid_Ki:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_pid.kb_q16;
            break;

        case outerloop_pid_min:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_pid.min_value << 2;
            break;
        case outerloop_pid_max:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_pid.max_value << 2;
            break;

// Id PID
        case Id_pid_Kp:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Id_pid.ka_q16;
            break;
        case Id_pid_Ki:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Id_pid.kb_q16;
            break;
        case Id_pid_min:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Id_pid.min_value << 2;
            break;
        case Id_pid_max:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Id_pid.max_value << 2;
            break;

// Iq PID
        case Iq_pid_Kp:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Iq_pid.ka_q16;
            break;
        case Iq_pid_Ki:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Iq_pid.kb_q16;
            break;
        case Iq_pid_min:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Iq_pid.min_value << 2;
            break;
        case Iq_pid_max:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->foc.Iq_pid.max_value << 2;
            break;

// Motor Config
        case Ld:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mparam.ld_q26;
            break;
        case Lq:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mparam.lq_q26;
            break;
        case Ralpha:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mparam.rs_q16;
            break;
        case num_pole_pairs:
            debug_var_1 = rx_data << 16;
            debug_var_2 = motor_ptr->mparam.pole_pairs << 16;
            break;
        case Config_Ke:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mparam.ke_q26;
            break;
        case Confi_J:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mparam.j_q26;
            break;
        case NominalSpeed:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mparam.nom_spd_q16;
            break;


#ifdef ENABLE_STALL_DETECT
        case Motor_BEMF_const:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->stall.bemf_constant;
            break;
        case Stall_Detection_TimeOut:
            debug_var_1 = rx_data;
            {
                fix16_t temp_value;
                temp_value = motor_ptr->stall.timeout_systicks;
                temp_value = (fix16_mul(temp_value << 16, ONE_DIVIDE_BY_5_FIX16)) >> 16;
                debug_var_2 = temp_value;       // Check This
            }
            break;
#endif
#ifdef ENABLE_FIELD_WEAKENING
        case fw_id_sp:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->fw.id_sp;
            break;
        case fw_speed_threshold_sp:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->fw.spd_threshold;
            break;
#endif

#ifdef ENABLE_IPD
        case ipd_charge:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->ipd.pwm_charge_ticks;
            break;
        case ipd_discharge:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->ipd.pwm_discharge_percent;
            break;
        case ipd_num_periods_per_pulse:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->ipd.num_periods_per_pulse;
            break;
        case ipd_angle_offset_forward:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->ipd.ipd_angle_forward_offset;
            break;
        case ipd_angle_offset_reverse:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->ipd.ipd_angle_reverse_offset;
            break;
#endif

#ifdef ENABLE_SIM
        case sim_braking_align:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->sim.align_count_braking_timeout;
            break;
        case sim_braking_ol_id_iq_ref_mul:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->sim.OL_Id_ref_braking_mul;
            break;
        case sim_braking_ol_iq_ref_mul:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->sim.OL_Iq_ref_braking_mul;
            break;
        case sim_braking_timeout:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->sim.braking_timeout_countmax;
            break;
#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
        case sim_current_track_gain:
            debug_var_1 = rx_data;
            break;
#endif
        case sim_bemf_amp_gain:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.vphase_amp_gain;
            break;
#endif
        case close_loop_coast_timeout:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.coast_timeout_target;
            break;

#ifdef ENABLE_POWERCONTROL
        case Power_pid_Kp:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_pid.kp_q11 << 5;
            break;
        case Power_pid_Ki:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_pid.kb_q16;
            break;

        case Power_ramp_rate:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_rate_store;
            break;
        case Power_setpoint:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->control.outerloop_target;
            break;
        case Power_max_limit:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->app.powerrefcl_max;
            break;
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)
        case LBG1_Kpe:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.est.kpe_store_q16;
            break;
        case LBG1_Kpi:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.est.kpi_store_q16;
            break;
        case LBG1_BandWidth:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.est.w0_q06 << 10;
            break;
        case LBG1_DampingFactor:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.est.dampfactor_q16;
            break;
        case LBG1_fc_speed:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.est.filter_fc_speed_q16;
            break;
        case angle_correction_offset:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.est.angcorr_offset;
            break;
        case angle_correction_factor:
            debug_var_1 = rx_data;
            // Value received by GUI in Q20 format
            debug_var_2 = motor_ptr->mpos.est.angcorr_gain_q20;
            break;
#endif

#if ((MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO))
        case open_loop_align_hold_num_steps:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->startup.aligngo.aligntime_cmd;
            break;

        case open_loop_speed_slew_rate:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->startup.aligngo.speedrefol_ramprate_cmd_q26;
            break;
        case open_loop_speed_setpoint:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->startup.aligngo.speedrefol_cmd_q26;
            break;
        case id_ref:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->startup.aligngo.idrefol_cmd_q16;
            break;
        case iq_ref:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->startup.aligngo.itotrefol_cmd_q16;
            break;
#endif

#if (MODULE_MPOS_HALL == ENABLED)
        case HallConfig_Istart:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->startup.i_total_hall_gui;
            break;
        case HallConfig_Gain_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.angcorr_gain_fwd_q20;
            break;
        case HallConfig_Gain_Rev:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.angcorr_gain_rev_q20;
            break;
        case HallConfig_Offset_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.angcorr_offset_fwd;
            break;
        case HallConfig_Offset_Rev:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.angcorr_offset_rev;
            break;
        case HallConfig_HallStateAngleRef_1_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.hallstate_angle_ref[1];
            break;
        case HallConfig_HallStateAngleRef_1_Rev:
            break;
        case HallConfig_HallStateAngleRef_2_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.hallstate_angle_ref[2];
            break;
        case HallConfig_HallStateAngleRef_2_Rev:
            break;
        case HallConfig_HallStateAngleRef_3_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.hallstate_angle_ref[3];
            break;
        case HallConfig_HallStateAngleRef_3_Rev:
            break;
        case HallConfig_HallStateAngleRef_4_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.hallstate_angle_ref[4];
            break;
        case HallConfig_HallStateAngleRef_4_Rev:
            break;
        case HallConfig_HallStateAngleRef_5_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.hallstate_angle_ref[5];
            break;
        case HallConfig_HallStateAngleRef_5_Rev:
            break;
        case HallConfig_HallStateAngleRef_6_Fwd:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.hallstate_angle_ref[6];
            break;
        case HallConfig_HallStateAngleRef_6_Rev:
            break;
#endif

#if (MODULE_MPOS_HALL == ENABLED) && (MODULE_MPOS_LBG1 == ENABLED)
        case HallConfig_Speed:
            debug_var_1 = rx_data;
            debug_var_2 = motor_ptr->mpos.hall.switchover_spd_lo2hi_radps;
            break;
#endif

#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)
        case hall_mech_angle_offset:
             debug_var_1 = rx_data;
             debug_var_2 = motor_ptr->mpos.hall.hall_sensor_offset_rad;
             break;
#endif

#if (MODULE_MPOS_HALL == ENABLED)
        case halledge_pol_skew_time:
             debug_var_1 = rx_data;
             debug_var_2 = (motor_ptr->mpos.hall.halledge_polskew_time_q26 >> 10);
             break;
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    case QEP_Resolution:
        debug_var_1 = rx_data << 16;
        debug_var_2 = motor_ptr->mpos.qep.qep_resolution << 16;
        break;
    case QEP_Offset_Angle:
        debug_var_1 = rx_data;
        debug_var_2 = motor_ptr->mpos.qep.angcorr_offset;
        break;
    case QEP_Fc:
        debug_var_1 = rx_data;
        debug_var_2 = motor_ptr->mpos.qep.filter_fc_speed_q16;
        break;
#endif

#if defined(ENABLE_HALL_TUNING)
    case HallTuning_Command:
        debug_var_1 = rx_data;
        break;
    case HallTuning_Istart:
        debug_var_1 = rx_data;
        debug_var_2 = motor_ptr->halltune.aligngo.itotrefol_cmd_q16;
        break;
    case HallTuning_Speed_Ramp:
        debug_var_1 = rx_data;
        debug_var_2 = motor_ptr->halltune.aligngo.speedrefol_ramprate_cmd_q26;
        break;
    case HallTuning_Target_Speed:
        debug_var_1 = rx_data;
        debug_var_2 = motor_ptr->halltune.aligngo.speedrefol_cmd_q26;
        break;
#endif

#if defined(ENABLE_AUTOTUNE_PI) || defined(ENABLE_AUTOTUNE_MID)
    case ATune_MID2_Istep:
        debug_var_1 = rx_data;
        break;
    case ATune_ATPI_Delta:
        debug_var_1 = rx_data;
        break;
    case ATune_MID2_RunMode:
        debug_var_1 = rx_data;
        break;
    case ATune_MID1_Start:
        debug_var_1 = rx_data;
        break;
    case ATune_MID1_RunMode:
    case ATune_MID1_ManCmd:
        debug_var_1 = rx_data;
        debug_var_2 = rx_data;
        break;
    case ATune_MID1_AlignV1:
        debug_var_1 = rx_data;
        break;
    case ATune_MID1_AlignV2:
        debug_var_1 = rx_data;
        break;
    case ATune_MID1_Vinj:
        debug_var_1 = rx_data;
        break;
    case ATune_MID1_numlimit:
        debug_var_1 = rx_data;
        break;
#endif

         default:
             break;
    }
}
#endif
