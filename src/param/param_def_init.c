
/**
 * @file param_def_init.c
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

/**
 * @brief Motor_Parameter_Default struct
 */
typedef struct
{
    ParameterCode enum_value;
    int32_t value;
}Motor_Parameter_Default;


/**
 * @brief Motor_Parameter_Default List
 */
const Motor_Parameter_Default motor_parameters [] =
{
    {PWM_to_FOC_control_ratio, FOC_DIVIDER},
    {dt, FOC_DT_Q26},
    {diff_amp_gain, UVW_PHASE_RSENSE_GAIN_DEFAULT_VALUE},

    {Ld, FOC_LD_DEFAULT_VALUE},
    {Lq, FOC_LQ_DEFAULT_VALUE},
    {Ralpha, FOC_RALPHA_DEFAULT_VALUE},
    {num_pole_pairs, FOC_NUM_POLE_PAIRS_DEFAULT_VALUE},
    {Config_Ke, AUTOTUNE_KE_DEFAULT_VALUE},
    {Confi_J, AUTOTUNE_J_DEFAULT_VALUE},
    {NominalSpeed, NOMINAL_SPEED_DEFAULT_VALUE},

    {max_cl_speed_limit, CLOSE_LOOP_MAX_SPEED_LIMIT_DEFAULT_VALUE},
    {Power_max_limit, CLOSE_LOOP_MAX_POWER_LIMIT_DEFAULT_VALUE},
    {outerloop_pid_min, SPEED_PID_MIN_DEFAULT_VALUE},
    {outerloop_pid_max, SPEED_PID_MAX_DEFAULT_VALUE},
    {current_limit, MOTOR_CURRENT_LIMIT_DEFAULT_VALUE},
    {close_loop_coast_timeout, COAST_TIMEOUT_DEFAULT_VALUE},

    {Id_pid_Kp, ID_PID_KP_REF_DEFAULT_VALUE},
    {Id_pid_Ki, ID_PID_KI_REF_DEFAULT_VALUE},
    {Id_pid_min, ID_PID_MIN_DEFAULT_VALUE},
    {Id_pid_max, ID_PID_MAX_DEFAULT_VALUE},
    {id_pi_filter_coeff, ID_FILTER_GAIN_DEFAULT_VALUE},
    {Iq_pid_Kp, IQ_PID_KP_REF_DEFAULT_VALUE},
    {Iq_pid_Ki, IQ_PID_KI_REF_DEFAULT_VALUE},
    {Iq_pid_min, IQ_PID_MIN_DEFAULT_VALUE},
    {Iq_pid_max, IQ_PID_MAX_DEFAULT_VALUE},
    {iq_pi_filter_coeff, IQ_FILTER_GAIN_DEFAULT_VALUE},
    {idiq_ramp_rate, IDIQ_RAMP_RATE_DEFAULT_VALUE},

#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
    {speed_pid_Kp, SPEED_PID_KP_REF_DEFAULT_VALUE},
    {speed_pid_Ki, SPEED_PID_KI_REF_DEFAULT_VALUE},
    {closed_loop_speed_slew_rate, FOC_CLOSE_LOOP_SPEED_SLEW_RATE_DEFAULT_VALUE},
#endif
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
    {Power_pid_Kp, POWER_PID_KP_REF_DEFAULT_VALUE},
    {Power_pid_Ki, POWER_PID_KI_REF_DEFAULT_VALUE},
    {Power_ramp_rate, FOC_CLOSE_LOOP_POWER_SLEW_RATE_DEFAULT_VALUE},
#endif
    {cl_mode_selection, CLOSE_LOOP_SPEED_MODE_DEFAULT_VALUE},
    {reverse_direction, MOTOR_DIRECTION_REVERSE_FLAG_DEFAULT_VALUE},
    {closed_loop_speed_setpoint, FOC_CLOSE_LOOP_SPEED_SETPOINT_DEFAULT_VALUE},
    {Power_setpoint, FOC_CLOSE_LOOP_POWER_SETPOINT_DEFAULT_VALUE},
    {id_ref_cl, ID_REF_CLOSE_LOOP_REF_DEFAULT_VALUE},
    {iq_ref_cl, IQ_REF_CLOSE_LOOP_REF_DEFAULT_VALUE},

#if (MODULE_MPOS_LBG1 == ENABLED)
    {LBG1_BandWidth, EST_LBG1_BANDWIDTH_DEFAULT_VALUE},
    {LBG1_DampingFactor, EST_LBG1_DAMPINGFACTOR_DEFAULT_VALUE},
    {LBG1_fc_speed, EST_LBG1_KSPEED_DEFAULT_VALUE},
    {angle_correction_offset, EST_LBG1_ANGLE_OFFSET_DEFAULT_VALUE},
    {angle_correction_factor, EST_LBG1_ANGLE_FACTOR_DEFAULT_VALUE},

    #ifdef ENABLE_PLL_ANGLE_CALCULATION
    {LBG1_fc_bemf_mag, EST_LBG1_FC_BEMF_MAGNITUDE_DEFAULT_VALUE},
    #endif
#endif

#ifdef ENABLE_SIM
    {sim_braking_align, SIM_BRAKING_ALIGN_TIME_DEFAULT_VALUE},
    {sim_braking_ol_id_iq_ref_mul, SIM_BRAKING_ID_REF_MUL_DEFAULT_VALUE},
    {sim_braking_ol_iq_ref_mul, SIM_BRAKING_IQ_REF_MUL_DEFAULT_VALUE},
    {sim_braking_timeout, SIM_BEMF_BRAKING_TIMEOUT_COUNT_DEFAULT_VALUE},
    {sim_bemf_amp_gain, SIM_BEMF_AMP_GAIN_DEFAULT_VALUE},
    {sim_current_track_gain, SIM_CURRENT_TRACK_GAIN_DEFAULT_VALUE},
#endif

#ifdef ENABLE_STALL_DETECT
    {Motor_BEMF_const, STALL_BEMF_CONSTANT_DEFAULT_VALUE},
    {Stall_Detection_TimeOut, STALL_DETECT_TIME_DEFAULT_VALUE},
#endif

#ifdef ENABLE_IPD
    {ipd_charge, IPD_CHARGE_TICKS_DEFAULT_VALUE},
    {ipd_discharge, IPD_DISCHARGE_PERCENT_DEFAULT_VALUE},
    {ipd_num_periods_per_pulse, IPD_PERIODS_PER_PULSE_DEFAULT_VALUE},
    {ipd_angle_offset_forward, IPD_ANGLE_FORWARD_OFFSET_DEFAULT_VALUE},
    {ipd_angle_offset_reverse, IPD_ANGLE_REVERSE_OFFSET_DEFAULT_VALUE},
#endif

#ifdef ENABLE_FIELD_WEAKENING
    {fw_speed_threshold_sp, FIELD_WEAKENING_SPEED_THRESHOLD_VALUE},
    {fw_id_sp, FIELD_WEAKENING_ID_REF_VALUE},
    {fw_vdrive_threshold, FIELD_WEAKENING_VDRIVE_THRESHOLD_VALUE},

    #if (FIELD_WEAKENING_METHOD == PID_CONTROL)
    {fw_pid_Kp, FIELD_WEAKENING_PID_KP_REF_DEFAULT_VALUE},
    {fw_pid_Ki, FIELD_WEAKENING_PID_KI_REF_DEFAULT_VALUE},
    {fw_vmax_scl_factor, FIELD_WEAKENING_VMAX_SCL_FACTOR_DEFAULT_VALUE},
    #endif
#endif

#if (MODULE_MPOS_HALL == ENABLED)
    {HallConfig_Istart, DEFAULT_VALUE_HALL_ISTART_Q16},
    {HallConfig_Gain_Fwd, DEFAULT_VALUE_HALL_ANGCORR_GAIN_FWD_Q20},
    {HallConfig_Gain_Rev, DEFAULT_VALUE_HALL_ANGCORR_GAIN_REV_Q20},
    {HallConfig_Offset_Fwd, DEFAULT_VALUE_HALL_ANGCORR_OFFSET_FWD_Q16},
    {HallConfig_Offset_Rev, DEFAULT_VALUE_HALL_ANGCORR_OFFSET_REV_Q16},
    {hall_mech_angle_offset, DEFAULT_VALUE_HALL_MECH_ANGLE_OFFSET_DEG_Q16},
    {halledge_pol_skew_time, DEFAULT_VALUE_HALL_EDGE_POL_SKEW_TIME_Q16},
    {HallConfig_HallStateAngleRef_1_Fwd, DEFAULT_VALUE_HALL_SNAPANGLE_1_FWD_Q16},
    {HallConfig_HallStateAngleRef_1_Rev, DEFAULT_VALUE_HALL_SNAPANGLE_1_REV_Q16},
    {HallConfig_HallStateAngleRef_2_Fwd, DEFAULT_VALUE_HALL_SNAPANGLE_2_FWD_Q16},
    {HallConfig_HallStateAngleRef_2_Rev, DEFAULT_VALUE_HALL_SNAPANGLE_2_REV_Q16},
    {HallConfig_HallStateAngleRef_3_Fwd, DEFAULT_VALUE_HALL_SNAPANGLE_3_FWD_Q16},
    {HallConfig_HallStateAngleRef_3_Rev, DEFAULT_VALUE_HALL_SNAPANGLE_3_REV_Q16},
    {HallConfig_HallStateAngleRef_4_Fwd, DEFAULT_VALUE_HALL_SNAPANGLE_4_FWD_Q16},
    {HallConfig_HallStateAngleRef_4_Rev, DEFAULT_VALUE_HALL_SNAPANGLE_4_REV_Q16},
    {HallConfig_HallStateAngleRef_5_Fwd, DEFAULT_VALUE_HALL_SNAPANGLE_5_FWD_Q16},
    {HallConfig_HallStateAngleRef_5_Rev, DEFAULT_VALUE_HALL_SNAPANGLE_5_REV_Q16},
    {HallConfig_HallStateAngleRef_6_Fwd, DEFAULT_VALUE_HALL_SNAPANGLE_6_FWD_Q16},
    {HallConfig_HallStateAngleRef_6_Rev, DEFAULT_VALUE_HALL_SNAPANGLE_6_REV_Q16},
#endif

#if (MODULE_MPOS_HALL == ENABLED) && (MODULE_MPOS_LBG1 == ENABLED)
    {HallConfig_Speed, DEFAULT_VALUE_HALL_HALL2EST_SWITCH_SPEED_Q16},
#endif

#if defined(ENABLE_HALL_TUNING)
    {HallTuning_Istart, DEFAULT_VALUE_HALLTUNE_ISTART_Q16},
    {HallTuning_Speed_Ramp, DEFAULT_VALUE_HALLTUNE_RAMPRATE_Q26},
    {HallTuning_Target_Speed, DEFAULT_VALUE_HALLTUNE_SPEED_Q26},
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    {QEP_Resolution, DEFAULT_VALUE_QEP_RESOLUTION_Q00},
    {QEP_Offset_Angle_Fwd, DEFAULT_VALUE_QEP_ANGCORR_OFFSET_FWD_Q16},
    {QEP_Offset_Angle_Rev, DEFAULT_VALUE_QEP_ANGCORR_OFFSET_REV_Q16},
    {QEP_Fc, DEFAULT_VALUE_QEP_FC_SPEED_HZ_Q16},
    {QEP_Main_Align_Offset, DEFAULT_VALUE_QEP_ALIGNOFFSET_DEG},
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    {open_loop_align_hold_num_steps, FOC_OPEN_LOOP_SS_ALIGN_COUNT_DEFAULT_VALUE},
    {id_ref, ID_REF_SP_DEFAULT_VALUE},
    {iq_ref, IQ_REF_SP_DEFAULT_VALUE},
    {open_loop_speed_slew_rate, FOC_CUR_CTRL_OPEN_LOOP_SLEW_RATE_DEFAULT_VALUE_Q26},
    {open_loop_speed_setpoint, FOC_OPEN_LOOP_SPEED_SET_POINT_DEFAULT_VALUE_Q26},
    {Auto_Mode_Open_Close_Loop, AUTO_CLOSE_LOOP_DEFAULT_VALUE},
#endif

#if defined(ENABLE_MTPA)
    {mtpa_gain, MTPA_DEGPAMP_DEFAULT_VALUE},
#endif

//#if (MODULE_MPOS_QEP == ENABLED)
    {Power_Measure_Iphase_Offset, DEFAULT_VALUE_POWER_IPHASE_OFSFET_Q16},
    {Power_Measure_Iphase_Factor, DEFAULT_VALUE_POWER_IPHASE_FACTOR_Q16},
//#endif
};


void init_default_parameters (Struct_Motor* motor_ptr)
{
    uint32_t count, array_total_elements;

    array_total_elements = (sizeof(motor_parameters)/sizeof(motor_parameters[0]));

    for (count = 0; count < array_total_elements; count++)
    {
        param_write(motor_parameters[count].enum_value, motor_parameters[count].value, motor_ptr);
    }
}
