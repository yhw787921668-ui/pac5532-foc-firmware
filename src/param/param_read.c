
/**
 * @file param_read.c
 * @brief Use for parameter reading
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
#include "app_funcs.h"

//#include "debug_display.h"
//#include "debug_export.h"

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#ifdef ENABLE_DEBUG_DISPLAY
#include "debug_display.h"
#endif

#ifdef ENABLE_DEBUG_PARAMEXPORT
#include "debug_export.h"
#endif

#ifdef ENABLE_DEBUG_DAS
#include "debug_das.h"
#endif

#ifdef ENABLE_DEBUG_RTT
#include "USER_RTT.h"
#endif
#ifdef ENABLE_AUTOTUNE_PI
#include "atune_funcs.h"
#endif

#ifdef ENABLE_DEBUG_SAMPLE_RAM
#include "debug_capture.h"
#endif

#if defined(ENABLE_FLASHDEMO) && defined(ENABLE_FLASHDEMO_DEBUGREAD)
#include "flashdemo.h"
#endif

void param_read(ParameterCode param_code, Struct_Motor* motor_ptr)
{
    switch (param_code)
    {
// Motor Config
        case status_motor:
#if defined(ENABLE_AUTOTUNE_PI)
            at_uistatus(motor_ptr);
#endif
            int_to_buffer(motor_ptr->status.motor);
            clear_status(STATUS_MOTOR_GRP_CLEARREAD);
#if defined(ENABLE_AUTOTUNE_PI)
//                    at_status_clear_on_guiread(motor_ptr);
#endif
            break;
        case fault_warning:
#if defined(ENABLE_AUTOTUNE_PI)
            at_uistatus(motor_ptr);
#endif
            int_to_buffer(motor_ptr->fault.motor);
            // Clear FOC control over flow warning
            clear_warning(FAULT_MOTOR_GRP_CLEARREAD);
            break;
        case sys_fault:
            int_to_buffer(motor_ptr->fault.fault_terminal);
            break;

        case current_limit:
            int_to_buffer(motor_ptr->app.current_limit);
            break;
        case dt:
            int_to_buffer(motor_ptr->control.dt_control_q26);
            break;
        case pwm_dac_selection1:
#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_RTT)
            int_to_buffer(motor_ptr->debug.debug_buffer_command1);
#endif
            break;
        case pwm_dac_selection2:
#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_RTT)
            int_to_buffer(motor_ptr->debug.debug_buffer_command2);
#endif
            break;
        case pwm_dac_selection3:
#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_RTT)
            int_to_buffer(motor_ptr->debug.debug_buffer_command3);
#endif
            break;
        case pwm_dac_selection4:
#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_RTT)
            int_to_buffer(motor_ptr->debug.debug_buffer_command4);
#endif
            break;
        case descale_current_factor:
            int_to_buffer(motor_ptr->app.descale_current_factor);
            break;
        case diff_amp_gain:
            int_to_buffer(motor_ptr->app.iphase_amp_gain);
            break;
        case cl_mode_selection:
            int_to_buffer(motor_ptr->app.speed_mode_flag);
            break;
        case reverse_direction:
            if (motor_ptr->app.app_motor_direction == -1)
                int_to_buffer(1);
            else
                int_to_buffer(0);
            break;

#if ((MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO))
// Open Loop
        case open_loop_align_hold_num_steps:
            int_to_buffer(motor_ptr->startup.aligngo.aligntime_cmd);
            break;
        case open_loop_speed_slew_rate:
            int_to_buffer(motor_ptr->startup.aligngo.speedrefol_ramprate_cmd_q26);
            break;
        case open_loop_speed_setpoint:
            int_to_buffer(motor_ptr->startup.aligngo.speedrefol_cmd_q26);
            break;
        case id_ref:
            int_to_buffer(motor_ptr->startup.aligngo.idrefol_cmd_q16);
            break;
        case iq_ref:
            int_to_buffer(motor_ptr->startup.aligngo.itotrefol_cmd_q16);
            break;
#endif
// Id & Iq
        case id_pi_filter_coeff:
            int_to_buffer(motor_ptr->foc.Id_filter_gain);
            break;
        case iq_pi_filter_coeff:
            int_to_buffer(motor_ptr->foc.Iq_filter_gain);
            break;
        case id_fdbk:
            int_to_buffer((fix16_t) (motor_ptr->foc.id_q14 << 2));
            break;
        case iq_fdbk:
            int_to_buffer((fix16_t) (motor_ptr->foc.iq_q14 << 2));
            break;
        case idiq_ramp_rate:
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
            int_to_buffer(motor_ptr->control.idiqref_rate_f16 >> 1);
#else
            int_to_buffer(motor_ptr->control.idiqref_rate_f16);
#endif
            break;

// Closed Loop
        case closed_loop_speed_slew_rate:
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
            int_to_buffer(motor_ptr->control.outerloop_rate_store >> 1);
#else
            int_to_buffer(motor_ptr->control.outerloop_rate_store);
#endif
            break;
        case closed_loop_speed_setpoint:
            int_to_buffer(motor_ptr->app.outerloop_target_cmd_f16);
            break;
        case id_ref_cl:
            int_to_buffer(motor_ptr->app.idrefcl_cmd_f16);
            break;
        case iq_ref_cl:
            int_to_buffer(motor_ptr->app.iqrefcl_cmd_f16);
            break;
        case max_cl_speed_limit:
            int_to_buffer(motor_ptr->app.speedrefcl_max);
            break;
// Misc
        case Vdc_gui:
            int_to_buffer(motor_ptr->app.dcbus_voltage);
            break;

// Speed PID
        case speed_pid_Kp:
            int_to_buffer(motor_ptr->control.outerloop_pid.ka_q16);
            break;
        case speed_pid_Ki:
            int_to_buffer(motor_ptr->control.outerloop_pid.kb_q16);
            break;

        case outerloop_pid_min:
            int_to_buffer(motor_ptr->control.outerloop_pid.min_value << 2);
            break;
        case outerloop_pid_max:
            int_to_buffer(motor_ptr->control.outerloop_pid.max_value << 2);
            break;

// Id PID
        case Id_pid_Kp:
            int_to_buffer(motor_ptr->foc.Id_pid.ka_q16);
            break;
        case Id_pid_Ki:
            int_to_buffer(motor_ptr->foc.Id_pid.kb_q16);
            break;
        case Id_pid_min:
            int_to_buffer(motor_ptr->foc.Id_pid.min_value << 2);
            break;
        case Id_pid_max:
            int_to_buffer(motor_ptr->foc.Id_pid.max_value << 2);
            break;

// Iq PID
        case Iq_pid_Kp:
            int_to_buffer(motor_ptr->foc.Iq_pid.ka_q16);
            break;
        case Iq_pid_Ki:
            int_to_buffer(motor_ptr->foc.Iq_pid.kb_q16);
            break;
        case Iq_pid_min:
            int_to_buffer(motor_ptr->foc.Iq_pid.min_value << 2);
            break;
        case Iq_pid_max:
            int_to_buffer(motor_ptr->foc.Iq_pid.max_value << 2);
            break;

// Motor Parameters
        case Ld:
            int_to_buffer(motor_ptr->mparam.ld_q26);
            break;
        case Lq:
            int_to_buffer(motor_ptr->mparam.lq_q26);
            break;
        case Ralpha:
            int_to_buffer(motor_ptr->mparam.rs_q16);
            break;
        case num_pole_pairs:
            int_to_buffer(motor_ptr->mparam.pole_pairs);
            break;
        case Config_Ke:
            int_to_buffer(motor_ptr->mparam.ke_q26);
            break;
        case Confi_J:
            int_to_buffer(motor_ptr->mparam.j_q26);
            break;
        case NominalSpeed:
            int_to_buffer(motor_ptr->mparam.nom_spd_q16);
            break;

// Plot variables?
        case estimated_angle:
            int_to_buffer(motor_ptr->mpos.mpos_angle);
            break;
        case estimated_speed:
            int_to_buffer(motor_ptr->mpos.mpos_spd_radps);
            break;
        case cl_outerloop_setpoint_ramp:
            int_to_buffer(motor_ptr->control.outerloop_ramp);
            break;
#ifdef ENABLE_POWERMEASURE
        case calculated_power:
            // If power result is positive send the value else send 0, GUI will display +ve values or zero
            if (motor_ptr->power.power_result > 0)
            {
                int_to_buffer(motor_ptr->power.power_result);
            }
            else
                int_to_buffer(0);
            break;
#endif
#ifdef ENABLE_DEBUG_DISPLAY
#if defined(IPD_DEBUG_ENABLE)
#ifdef ENABLE_IPD
        case debug1:
            int_to_buffer(motor_ptr->ipd.u_pos_current << 16);
            break;
        case debug2:
            int_to_buffer(motor_ptr->ipd.u_neg_current << 16);
            break;
        case debug3:
            int_to_buffer(motor_ptr->ipd.v_pos_current << 16);
            break;
        case debug4:
            int_to_buffer(motor_ptr->ipd.v_neg_current << 16);
            break;
        case debug5:
            int_to_buffer(motor_ptr->ipd.w_pos_current << 16);
            break;
        case debug6:
            int_to_buffer(motor_ptr->ipd.w_neg_current << 16);
            break;
#endif
#elif defined(ENABLE_HALL_TUNING)
        case debug1:
            int_to_buffer(motor_ptr->halltune.halltune_output_1);
            break;
        case debug2:
            int_to_buffer(motor_ptr->halltune.halltune_output_2);
            break;
        case debug3:
            int_to_buffer(motor_ptr->halltune.halltune_output_3);
            break;
        case debug4:
            int_to_buffer(motor_ptr->halltune.halltune_output_4);
            break;
        case debug5:
            int_to_buffer(motor_ptr->halltune.halltune_output_5);
            break;
        case debug6:
            int_to_buffer(motor_ptr->halltune.halltune_output_6);
            break;
#else
        case debug1:
            int_to_buffer(debug_var_1);
            break;
        case debug2:
            int_to_buffer(debug_var_2);
            break;
        case debug3:
            int_to_buffer(debug_var_3);
            break;
        case debug4:
            int_to_buffer(debug_var_4);
            break;
        case debug5:
            int_to_buffer(debug_var_5);
            break;
        case debug6:
            int_to_buffer(debug_var_6);
            break;
#endif

        case debug1_max:
            if (debug1_index_var < MAX_GUI_DEBUG_ARRAY_LENGHTH)
                int_to_buffer((fix16_t)(debug1_var_max << Debug_Q_Q16_Conv_Table[debug1_index_var]));
            debug1_var_max = 0x80000000;
            break;
        case debug1_min:
            if (debug1_index_var < MAX_GUI_DEBUG_ARRAY_LENGHTH)
                int_to_buffer((fix16_t)(debug1_var_min << Debug_Q_Q16_Conv_Table[debug1_index_var]));
            debug1_var_min = 0x7FFFFFFF;
            break;
        case debug2_max:
            if (debug2_index_var < MAX_GUI_DEBUG_ARRAY_LENGHTH)
                int_to_buffer((fix16_t)(debug2_var_max << Debug_Q_Q16_Conv_Table[debug2_index_var]));
            debug2_var_max = 0x80000000;
            break;
        case debug2_min:
            if (debug2_index_var < MAX_GUI_DEBUG_ARRAY_LENGHTH)
                int_to_buffer((fix16_t)(debug2_var_min << Debug_Q_Q16_Conv_Table[debug2_index_var]));
            debug2_var_min = 0x7FFFFFFF;
            break;
        case debug3_max:
            if (debug3_index_var < MAX_GUI_DEBUG_ARRAY_LENGHTH)
                int_to_buffer((fix16_t)(debug3_var_max << Debug_Q_Q16_Conv_Table[debug3_index_var]));
            debug3_var_max = 0x80000000;
            break;
        case debug3_min:
            if (debug3_index_var < MAX_GUI_DEBUG_ARRAY_LENGHTH)
                int_to_buffer((fix16_t)(debug3_var_min << Debug_Q_Q16_Conv_Table[debug3_index_var]));
            debug3_var_min = 0x7FFFFFFF;
            break;
#endif

// MTPA
#if defined(ENABLE_MTPA)
        case mtpa_gain:
            int_to_buffer(motor_ptr->mtpa.mtpa_gain_degpa_q16);
            break;
#endif

// Misc Vars
        case misc_fix16_1:
            int_to_buffer(0);
            break;
        case misc_fix16_2:
            int_to_buffer(0);
            break;
        case misc_fix16_3:
            int_to_buffer(0);
            break;
        case misc_uint32_1:
            int_to_buffer(0);
            break;
        case misc_uint32_2:
            int_to_buffer(0);
            break;
        case misc_uint32_3:
            int_to_buffer(0);
            break;
#ifdef ENABLE_DEBUG_PARAMEXPORT
        case param_read_command:
            int_to_buffer(gui_parameter_command);
            break;
        case param1:
#if defined(ENABLE_FLASHDEMO) && defined(ENABLE_FLASHDEMO_DEBUGREAD)
#if defined (ENABLE_DEBUG_FLASHDEMO)
            int_to_buffer(parameter_value1); 
#else
            int_to_buffer(flashdemo_param_export(gui_parameter_command));
#endif

#else
            int_to_buffer(parameter_value1);        
#endif
            break;
        case param2:
#if defined(ENABLE_FLASHDEMO) && defined(ENABLE_FLASHDEMO_DEBUGREAD) && defined (ENABLE_DEBUG_FLASHDEMO)
#if defined (ENABLE_DEBUG_FLASHDEMO)
            int_to_buffer(parameter_value2);
#else
            int_to_buffer(flashdemo_param_export(gui_parameter_command + 1));
#endif
#else
            int_to_buffer(parameter_value2);
#endif
            break;
        case param3:
#if defined(ENABLE_FLASHDEMO) && defined(ENABLE_FLASHDEMO_DEBUGREAD)
#if defined (ENABLE_DEBUG_FLASHDEMO)
            int_to_buffer(parameter_value3); 
#else
            int_to_buffer(flashdemo_param_export(gui_parameter_command + 2));
#endif
#else
            int_to_buffer(parameter_value3);        
#endif
            break;
#endif
        case PWM_to_FOC_control_ratio:
            int_to_buffer(motor_ptr->foc.ctrl_divider_foc);
            break;

#ifdef ENABLE_STALL_DETECT
        case Motor_BEMF_const:
            int_to_buffer(motor_ptr->stall.bemf_constant);
            break;
        case Stall_Detection_TimeOut:
            {
                fix16_t temp_value;
                temp_value = motor_ptr->stall.timeout_systicks;
                temp_value = (fix16_mul(temp_value << 16, ONE_DIVIDE_BY_5_FIX16)) >> 16;
                int_to_buffer(temp_value);
            }
            break;
#endif
        case Auto_Mode_Open_Close_Loop:
            int_to_buffer(motor_ptr->app.auto_close_loop_cmd);
            break;

#ifdef ENABLE_FIELD_WEAKENING
        case fw_id_sp:
            int_to_buffer(motor_ptr->fw.id_sp);
            break;
        case fw_speed_threshold_sp:
            int_to_buffer(motor_ptr->fw.spd_threshold);
            break;
    #if (FIELD_WEAKENING_METHOD == PID_CONTROL)
        case fw_pid_Kp:
            int_to_buffer(motor_ptr->fw.fw_pid.ka_q16);
            break;
        case fw_pid_Ki:
            int_to_buffer(motor_ptr->fw.fw_pid.kb_q16);
            break;
    #endif
        case fw_vmax_scl_factor:
            break;
#endif

#ifdef ENABLE_IPD
        case ipd_charge:
            int_to_buffer(motor_ptr->ipd.pwm_charge_ticks);
            break;
        case ipd_discharge:
            int_to_buffer(motor_ptr->ipd.pwm_discharge_percent);
            break;
        case ipd_num_periods_per_pulse:
            int_to_buffer(motor_ptr->ipd.num_periods_per_pulse);
            break;
        case ipd_angle_offset_forward:
            // Descale back to q16 from q0 for GUI read
            int_to_buffer(motor_ptr->ipd.ipd_angle_forward_offset << 16);
            break;
        case ipd_angle_offset_reverse:
            // Descale back to q16 from q0 for GUI read
            int_to_buffer(motor_ptr->ipd.ipd_angle_reverse_offset << 16);
            break;
        case ipd_same_pol_diff:
            int_to_buffer(motor_ptr->ipd.same_polarity_current_delta);
            break;
        case ipd_max_min_diff:
            int_to_buffer(motor_ptr->ipd.max_to_min_current_delta);
            break;
        case ipd_angle:
            int_to_buffer(motor_ptr->ipd.ipd_angle);
            break;
#endif

#ifdef ENABLE_SIM
        case sim_braking_align:
            int_to_buffer(motor_ptr->sim.align_count_braking_timeout);
            break;
        case sim_braking_ol_id_iq_ref_mul:
            int_to_buffer(motor_ptr->sim.OL_Id_ref_braking_mul);
            break;
        case sim_braking_ol_iq_ref_mul:
            int_to_buffer(motor_ptr->sim.OL_Iq_ref_braking_mul);
            break;
        case sim_braking_timeout:
            // Convert counts back to FOC dt counts for GUI read process
            int_to_buffer(fix16_mul(motor_ptr->sim.braking_timeout_countmax << 10, fix16_div((SIM_CONTROL_DIVIDER << 10), (motor_ptr->foc.ctrl_divider_foc << 10))) >> 10);
            break;
        case sim_braking_diff_threshold:
            int_to_buffer(0);
            break;
#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
        case sim_current_track_gain:
            break;
#endif
        case sim_bemf_amp_gain:
            int_to_buffer(motor_ptr->app.vphase_amp_gain);
            break;
#endif
        case close_loop_coast_timeout:
            int_to_buffer(motor_ptr->app.coast_timeout_target);
            break;

        case Phase_Rsense:
            int_to_buffer(PCB_RSENSE_IUVW_OHMS*1000*65536);
            break;
        case OC_Rsense:
            int_to_buffer(PCB_RSENSE_OC_OHMS*1000*65536);
            break;

#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
        case Power_pid_Kp:
            int_to_buffer(motor_ptr->control.outerloop_pid.ka_q16);
            break;
        case Power_pid_Ki:
            int_to_buffer(motor_ptr->control.outerloop_pid.kb_q16);
            break;

        case Power_ramp_rate:
#if defined(ENABLE_CONTROLLER_INTERLEAVE)
            int_to_buffer(motor_ptr->control.outerloop_rate_store >> 1);
#else
            int_to_buffer(motor_ptr->control.outerloop_rate_store);
#endif
            break;
        case Power_setpoint:
            int_to_buffer(motor_ptr->app.outerloop_target_cmd_f16);
            break;
        case Power_max_limit:
            int_to_buffer(motor_ptr->app.powerrefcl_max);
            break;
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)
        case LBG1_Kpe:
            int_to_buffer(motor_ptr->mpos.est.kpe_store_q16);
            break;
        case LBG1_Kpi:
            int_to_buffer(motor_ptr->mpos.est.kpi_store_q16);
            break;
        case LBG1_BandWidth:
            int_to_buffer(motor_ptr->mpos.est.w0_q06 << 10);
            break;
        case LBG1_DampingFactor:
            int_to_buffer(motor_ptr->mpos.est.dampfactor_q16);
            break;
        case LBG1_fc_speed:
            int_to_buffer(motor_ptr->mpos.est.filter_fc_speed_q16);
            break;

#ifdef ENABLE_PLL_ANGLE_CALCULATION
        case LBG1_fc_bemf_mag:
            int_to_buffer(motor_ptr->mpos.est_pll.pll_amplitude_fc_q16);
            break;
#endif
        case angle_correction_offset:
            int_to_buffer(motor_ptr->mpos.est.angcorr_offset);
            break;
        case angle_correction_factor:
            // Value received by GUI in Q20 format
            int_to_buffer(motor_ptr->mpos.est.angcorr_gain_q20);
            break;
#endif

        case firmware_options:
        {
            fix16_t temp_value=0;

            //temp_value |= (MPOS_SELECT);

            temp_value |= (BOARD_SELECT <<4);

#ifdef ENABLE_SIM
            temp_value |= (0x01 <<8);
#endif
#ifdef ENABLE_STALL_DETECT
            temp_value |= (0x02 <<8);
#endif
#ifdef ENABLE_IPD
            temp_value |= (0x04 <<8);
#endif
#ifdef ENABLE_FIELD_WEAKENING
            temp_value |= (0x08 <<8);
#endif
#ifdef ENABLE_POWERMEASURE
            temp_value |= (0x10 <<8);
#endif
#ifdef ENABLE_AUTOTUNE_PI
            temp_value |= (0x20 <<8);
#endif
#ifdef ENABLE_IDIQ_FILTER
            temp_value |= (0x40 <<8);
#endif
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
            temp_value |= (0x80 <<8);
#endif
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
            temp_value |= (0x01 <<16);
#endif
#ifdef ENABLE_DEBUG_SAMPLE_RAM
            temp_value |= (0x02 <<16);
#endif

#if defined(ENABLE_FLASHDEMO) || defined(ENABLE_QEP_LIN_FLASHRW)
            temp_value |= (0x04 <<16);
#endif

#if defined(ENABLE_FORCEANG)
            temp_value |= (0x08 <<16);    // Bit 19
#endif

#if defined (ENABLE_QEP_LINEARIZATION_TUNING)
            temp_value |= (0x010 <<16);    // Bit 20
#endif

#if defined (ENABLE_QEP_OFFSET_TUNING)
            temp_value |= (0x020 <<16);    // Bit 21
#endif

#if defined (ENABLE_PLL_ANGLE_CALCULATION)
            temp_value |= (0x040 <<16);    // Bit 22
#endif

#if (FIELD_WEAKENING_METHOD == PID_CONTROL) && defined (ENABLE_FIELD_WEAKENING)
            temp_value |= (0x080 <<16);    // Bit 23
#endif
            // 4 Bits reserved for Arch definition
            temp_value = arch_type_param_read(&temp_value);
            int_to_buffer(temp_value);
        }
            break;

        case Vdc_Rsense_ratio:
            #if (INTERNAL_VMS == TRUE) && (VMS_FACTORY_CAL != TRUE)
            int_to_buffer(QCONST(VMS_INTERNAL_DIVIDE_RATIO, Q16));
            #else
            int_to_buffer(QCONST(R1_PLUS_R2_BY_R2_RATIO_VBUS, Q16));
            #endif
            break;
        case Vphase_Rsense_ratio:
            int_to_buffer(QCONST(R1_PLUS_R2_BY_R2_RATIO_SIM, Q16));
            break;
        case Vdc_max_limit:
            int_to_buffer(MAX_VBUS);
            break;
#if defined(ENABLE_AUTOTUNE_PI)
        // Auto Tune
        case AutoTune_Kpv:
            int_to_buffer(motor_ptr->control.outerloop_pid.ka_q16);
            break;
        case AutoTune_Kiv:
            int_to_buffer(motor_ptr->control.outerloop_pid.kb_q16);
            break;
        case ATune_Command:
            break;
        case ATune_MID2_Istep:
            int_to_buffer(motor_ptr->atune.istep_q16);
            break;
        case ATune_ATPI_Delta:
            int_to_buffer(motor_ptr->atune.delta_q6);
            break;
        case AutoTune_Status:
            int_to_buffer(motor_ptr->atune.status_atune);
            at_status_clear_on_guiread(motor_ptr);
            break;
            // Read AutoTune storage variables
        case AutoTune_Storage_ke_q26:
            int_to_buffer(motor_ptr->atune.ke_q26);
            break;
        case AutoTune_Storage_j_q26:
            int_to_buffer(motor_ptr->atune.j_q26);
            break;
        case AutoTune_Storage_speed_ka_q16:
            int_to_buffer(motor_ptr->atune.speed_ka_q16);
            break;
        case AutoTune_Storage_speed_kb_q16:
            int_to_buffer(motor_ptr->atune.speed_kb_q16);
            break;
        case AutoTune_Storage_id_ka_q16:
            int_to_buffer(motor_ptr->atune.id_ka_q16);
            break;
        case AutoTune_Storage_id_kb_q16:
            int_to_buffer(motor_ptr->atune.id_kb_q16);
            break;
        case AutoTune_Storage_iq_ka_q16:
            int_to_buffer(motor_ptr->atune.iq_ka_q16);
            break;
        case AutoTune_Storage_iq_kb_q16:
            int_to_buffer(motor_ptr->atune.iq_kb_q16);
            break;
        case AutoTune_MID1_Storage_R_q16:
            int_to_buffer(motor_ptr->atune.rs_q16);
            break;
        case AutoTune_MID1_Storage_Ld_q26:
            int_to_buffer(motor_ptr->atune.ld_q26);
            break;
        case AutoTune_MID1_Storage_Lq_q26:
            int_to_buffer(motor_ptr->atune.lq_q26);
            break;
        case ATune_MID1_AlignV1:
            int_to_buffer(motor_ptr->atune.i_align1_gui_q14 << 2);
            break;
        case ATune_MID1_AlignV2:
            int_to_buffer(motor_ptr->atune.i_align2_gui_q14 << 2);
            break;
        case ATune_MID1_Vinj:
            int_to_buffer(motor_ptr->atune.i_inj_gui_q14 << 2);
            break;
        case ATune_MID1_numlimit:
            int_to_buffer(motor_ptr->atune.num_limit_gui << 0);
            break;
        case ATune_MID1_ManCmd:
            int_to_buffer(motor_ptr->atune.mid1_mancmd);
            break;
        case ATune_MID1_PFactor:
            int_to_buffer(motor_ptr->atune.power_factor_sq_q16);
            break;
#endif
        case Commutation_Mode:
        {
            fix16_t temp_value = MPOS_SELECT;
            if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)    // Note: Had to hack this to get access to Align&Go and Estimator panels
                temp_value = MPOS_LBG1_START_ALIGNGO;
#ifdef ENABLE_HALL_TUNING
            temp_value |= (1L << 8);
#endif
            int_to_buffer(temp_value);
        }
            break;

#if (MODULE_MPOS_HALL == ENABLED)
        case HallConfig_Istart:
            int_to_buffer(motor_ptr->startup.i_total_hall_gui);
            break;
        case HallConfig_Speed:
            int_to_buffer(motor_ptr->mpos.hall.switchover_spd_lo2hi_radps);
            break;
        case HallConfig_Gain_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.angcorr_gain_fwd_q20);
            break;
        case HallConfig_Gain_Rev:
            int_to_buffer(motor_ptr->mpos.hall.angcorr_gain_rev_q20);
            break;
        case HallConfig_Offset_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.angcorr_offset_fwd);
            break;
        case HallConfig_Offset_Rev:
            int_to_buffer(motor_ptr->mpos.hall.angcorr_offset_rev);
            break;
        case HallConfig_HallStateAngleRef_1_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.hallstate_angle_ref[1]);
            break;
        case HallConfig_HallStateAngleRef_2_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.hallstate_angle_ref[2]);
            break;
        case HallConfig_HallStateAngleRef_3_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.hallstate_angle_ref[3]);
            break;
        case HallConfig_HallStateAngleRef_4_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.hallstate_angle_ref[4]);
            break;
        case HallConfig_HallStateAngleRef_5_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.hallstate_angle_ref[5]);
            break;
        case HallConfig_HallStateAngleRef_6_Fwd:
            int_to_buffer(motor_ptr->mpos.hall.hallstate_angle_ref[6]);
            break;
        case HallConfig_HallStateAngleRef_1_Rev:
        case HallConfig_HallStateAngleRef_2_Rev:
        case HallConfig_HallStateAngleRef_3_Rev:
        case HallConfig_HallStateAngleRef_4_Rev:
        case HallConfig_HallStateAngleRef_5_Rev:
        case HallConfig_HallStateAngleRef_6_Rev:
            int_to_buffer(0);
            break;
#endif

#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)
        case hall_mech_angle_offset:
            int_to_buffer(motor_ptr->mpos.hall.hall_sensor_offset_deg_gui);
            break;
#endif

#if (MODULE_MPOS_HALL == ENABLED)
        case halledge_pol_skew_time:
            int_to_buffer(motor_ptr->mpos.hall.halledge_polskew_msec_gui_q16);
            break;
#endif

#if defined(ENABLE_HALL_TUNING)
        case HallTuning_Status:
            int_to_buffer(motor_ptr->halltune.status);
            break;
        case HallTuning_Command:
            int_to_buffer(motor_ptr->halltune.gui_cmd);
            break;
        case HallTuning_Istart:
            int_to_buffer(motor_ptr->halltune.aligngo.itotrefol_cmd_q16);
            break;
        case HallTuning_Speed_Ramp:
            int_to_buffer(motor_ptr->halltune.aligngo.speedrefol_ramprate_cmd_q26);
            break;
        case HallTuning_Target_Speed:
            int_to_buffer(motor_ptr->halltune.aligngo.speedrefol_cmd_q26);
            break;
        case HallTuning_HallStateAngleTuning_1:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_tuning[1]);
            break;
        case HallTuning_HallStateAngleRef_1:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_ref[1]);
            break;
        case HallTuning_HallStateAngleTuning_2:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_tuning[2]);
            break;
        case HallTuning_HallStateAngleRef_2:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_ref[2]);
            break;
        case HallTuning_HallStateAngleTuning_3:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_tuning[3]);
            break;
        case HallTuning_HallStateAngleRef_3:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_ref[3]);
            break;
        case HallTuning_HallStateAngleTuning_4:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_tuning[4]);
            break;
        case HallTuning_HallStateAngleRef_4:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_ref[4]);
            break;
        case HallTuning_HallStateAngleTuning_5:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_tuning[5]);
            break;
        case HallTuning_HallStateAngleRef_5:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_ref[5]);
            break;
        case HallTuning_HallStateAngleTuning_6:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_tuning[6]);
            break;
        case HallTuning_HallStateAngleRef_6:
            int_to_buffer(motor_ptr->halltune.hallstate_angle_ref[6]);
            break;
#endif

#if (MODULE_MPOS_QEP == ENABLED)
        case QEP_Resolution:
            int_to_buffer(motor_ptr->mpos.qep.qep_resolution);
            break;
        case QEP_Offset_Angle_Fwd:
            int_to_buffer(motor_ptr->mpos.qep.angcorr_offset_fwd);
            break;
        case QEP_Offset_Angle_Rev:
            int_to_buffer(motor_ptr->mpos.qep.angcorr_offset_rev);
            break;
        case QEP_Fc:
            int_to_buffer(motor_ptr->mpos.qep.filter_fc_speed_q16);
            break;
        case QEP_Main_Align_Offset:
            int_to_buffer(motor_ptr->mpos.qep.qep_sensor_offset_deg);
            break;
#endif


#if defined(ENABLE_DEBUG_SAMPLE_RAM)
        case capture_mode:
            int_to_buffer(motor_ptr->debug.skip_cycles);
            break;
        
        case capture_param_selection:
            int_to_buffer(motor_ptr->debug.param_selection);
            break;  

        case capture_size:
            int_to_buffer(motor_ptr->debug.number_samples);
            break;

        case capture_buffer_value_at_index:
            int_to_buffer(read_debug_extraction_data(motor_ptr->debug.buffer_index));
            break;

        case capture_status:
            int_to_buffer(motor_ptr->debug.extraction_state);
            break;

        case capture_cnt:
            int_to_buffer(motor_ptr->debug.capture_cnt);
            break;            
        
#endif
 
#ifdef ENABLE_QEP_OFFSET_TUNING
        case QEP_Alignment_Offset:
            int_to_buffer(motor_ptr->mpos.qep.qep_offset_angle_deg_q16);
            break;
#endif

#if defined(ENABLE_POWERMEASURE)
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
        case Power_Measure_Iphase_Offset:
            int_to_buffer(motor_ptr->power.power_iphase_offset_q14 << 2);
            break;     
        
        case Power_Measure_Iphase_Factor:
            int_to_buffer(motor_ptr->power.power_iphase_factor_q14 << 2);
            break;
#endif
#endif
        // Id & Iq bandwidth read from firmware, need to add code to support this feature
        case id_pi_bandwidth:
            int_to_buffer(0);
            break;
            
        case iq_pi_bandwidth:
            int_to_buffer(0);
            break;

        default:
            int_to_buffer(1);
            break;
    }
}
