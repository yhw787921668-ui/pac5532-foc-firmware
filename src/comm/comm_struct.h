
/**
 * @file comm_struct.h
 * @brief Use for Communication structure
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

#ifndef COMMSTRUCT_H
#define COMMSTRUCT_H

#include "maths.h"
#include "pid.h"


/**
 * @brief enum of command codes
 */
typedef enum
{
    cmd_version,
    cmd_set_address,
    cmd_read_word,
    cmd_write_word,
    cmd_set_buffer,
    cmd_parameter_read,
    cmd_parameter_write,
    cmd_motor_control_read,
    cmd_motor_control_write,
    cmd_cafe_reg_read,
    cmd_page_read,
    cmd_page_write
} CommandCode;


/**
 * @brief enum of parameter codes
 */
typedef enum
{
    buffer = 0,

    status_motor,
    fault_warning,
    current_limit,
    dt,
    est_angle_offset,

    open_loop_align_hold_num_steps,
    open_loop_speed_slew_rate,
    open_loop_speed_setpoint,

    closed_loop_speed_slew_rate,
    closed_loop_speed_setpoint,

    id_ref,
    iq_ref,
    id_pi_filter_coeff,
    iq_pi_filter_coeff,
    id_fdbk,
    iq_fdbk,

    Vdc_gui,

    speed_pid_Kp,
    speed_pid_Ki,

    outerloop_pid_min,
    outerloop_pid_max,

    Id_pid_Kp,
    Id_pid_Ki,
    Id_pid_min,
    Id_pid_max,

    Iq_pid_Kp,
    Iq_pid_Ki,
    Iq_pid_min,
    Iq_pid_max,

    // Motor Configuration
    Ld,
    Lq,
    Ralpha,
    num_pole_pairs,
    Config_Ke,
    Confi_J,
    NominalSpeed,

    // Plot variables
    estimated_angle,
    estimated_speed,
    ideal_angle,
    measured_speed,
    motor_current,

    pwm_dac_selection,

    debug1,
    debug2,
    debug3,
    debug4,
    debug5,
    debug6,

    scale_factor1,
    scale_factor2,
    scale_factor3,

    debug1_index,
    debug2_index,
    debug3_index,

    debug1_min,
    debug1_max,
    debug2_min,
    debug2_max,
    debug3_min,
    debug3_max,

    id_ref_cl,
    idiq_ramp_rate,

    torque_scale,
    torque_offset,
    torque_angle_offset,

    descale_current_factor,
    est_lock_assist_vq,
    cl_mode_selection,
    iq_ref_cl,

    reverse_direction,

    cl_outerloop_setpoint_ramp,

    misc_fix16_1,
    misc_fix16_2,
    misc_fix16_3,
    misc_uint32_1,
    misc_uint32_2,
    misc_uint32_3,

    PWM_to_FOC_control_ratio,
    diff_amp_gain,

    Motor_BEMF_const,
    Stall_Detection_TimeOut,

    Auto_Mode_Open_Close_Loop,

    Clear_Fault_Flags,

    fw_id_sp,
    fw_vdrive_threshold,
    fw_speed_threshold_sp,

    param_read_command,
    param1,
    param2,
    param3,

    ipd_charge,
    ipd_discharge,
    ipd_num_periods_per_pulse,
    ipd_angle_offset_forward,
    ipd_angle_offset_reverse,
    ipd_same_pol_diff,
    ipd_max_min_diff,
    ipd_angle,

    sim_braking_align,
    sim_braking_ol_id_iq_ref_mul,
    sim_braking_ol_iq_ref_mul,

    min_cl_speed_limit, // JAGTAG: Consider eliminating min_cl_speed_limit parameter
    max_cl_speed_limit,

    sim_braking_timeout,
    sim_braking_diff_threshold,
    sim_current_track_gain,
    sim_bemf_amp_gain,

    close_loop_coast_timeout,

    pwm_dac_selection1,
    pwm_dac_selection2,
    pwm_dac_selection3,
    pwm_dac_selection4,
    scale_factor4,

    Phase_Rsense,
    OC_Rsense,

    Power_pid_Kp,
    Power_pid_Ki,

    Power_ramp_rate,
    Power_setpoint,
    Power_max_limit,

    SMO_KpI,
    SMO_Kangle,
    SMO_Kslide,
    SMO_Kslf,
    SMO_Kspeed,

    LBG1_Kpe,
    LBG1_Kpi,
    LBG1_BandWidth,
    LBG1_DampingFactor,
    LBG1_fc_speed,

    // JAGTAG: References to LBG2 need to be removed
    LBG2_KpI,
    LBG2_KpE,
    LBG2_Kspeed,

    angle_correction_offset,
    angle_correction_factor,

    firmware_options,

    calculated_power,

    Vdc_Rsense_ratio,
    Vphase_Rsense_ratio,
    Vdc_max_limit,

    // Auto Tune
    AutoTune_Kpv,
    AutoTune_Kiv,
    ATune_Command,
    ATune_MID2_Istep,
    ATune_ATPI_Delta,
    AutoTune_Status,
    ATune_MID2_RunMode,

    // Read AutoTune storage variables
    AutoTune_Storage_ke_q26,
    AutoTune_Storage_j_q26,
    AutoTune_Storage_speed_ka_q16,
    AutoTune_Storage_speed_kb_q16,
    AutoTune_Storage_id_ka_q16,
    AutoTune_Storage_id_kb_q16,
    AutoTune_Storage_iq_ka_q16,
    AutoTune_Storage_iq_kb_q16,
    // Read and write AutoTune storage variables and command for MID1
    ATune_MID1_Start,
    ATune_MID1_RunMode,
    AutoTune_MID1_Storage_R_q16,
    AutoTune_MID1_Storage_Ld_q26,
    AutoTune_MID1_Storage_Lq_q26,
    ATune_MID1_AlignV1,
    ATune_MID1_AlignV2,
    ATune_MID1_Vinj,
    ATune_MID1_numlimit,
    ATune_MID1_ManCmd,
    ATune_MID1_PFactor,

    // Commutation Mode
    Commutation_Mode,

    // Hall Sensor Configuration
    HallConfig_Istart,
    HallConfig_Speed,
    HallConfig_Gain_Fwd,
    HallConfig_Gain_Rev,
    HallConfig_Offset_Fwd,
    HallConfig_Offset_Rev,
    HallConfig_HallStateAngleRef_1_Fwd,
    HallConfig_HallStateAngleRef_1_Rev,
    HallConfig_HallStateAngleRef_2_Fwd,
    HallConfig_HallStateAngleRef_2_Rev,
    HallConfig_HallStateAngleRef_3_Fwd,
    HallConfig_HallStateAngleRef_3_Rev,
    HallConfig_HallStateAngleRef_4_Fwd,
    HallConfig_HallStateAngleRef_4_Rev,
    HallConfig_HallStateAngleRef_5_Fwd,
    HallConfig_HallStateAngleRef_5_Rev,
    HallConfig_HallStateAngleRef_6_Fwd,
    HallConfig_HallStateAngleRef_6_Rev,

    // Hall Tuning Configuration
    HallTuning_Status,
    HallTuning_Command,
    HallTuning_Istart,
    HallTuning_Speed_Ramp,
    HallTuning_Target_Speed,
    HallTuning_HallStateAngleTuning_1,
    HallTuning_HallStateAngleRef_1,
    HallTuning_HallStateAngleTuning_2,
    HallTuning_HallStateAngleRef_2,
    HallTuning_HallStateAngleTuning_3,
    HallTuning_HallStateAngleRef_3,
    HallTuning_HallStateAngleTuning_4,
    HallTuning_HallStateAngleRef_4,
    HallTuning_HallStateAngleTuning_5,
    HallTuning_HallStateAngleRef_5,
    HallTuning_HallStateAngleTuning_6,
    HallTuning_HallStateAngleRef_6,

    // QEP
    QEP_Resolution,
    QEP_Offset_Angle_Fwd,
    QEP_Offset_Angle_Rev,
    QEP_Fc,

    // MTPA
    mtpa_gain,

    // debug extraction data to RAM
    capture_mode,
    capture_size,
    capture_param_selection,
    capture_buffer_index,
    capture_buffer_value_at_index,
    capture_start,
    capture_status,
    capture_cnt,

    // Flash write and read
    FLASH_read,
    FLASH_write,
    FLASH_erase,

    // Forced angle mode
    Forced_angle_mode,
    Forced_angle,
    Forced_angle_current,
    Forced_angle_speed,

    //QEP Offset Linearization Cal
    QEP_Offset_CalEnable,
    QEP_Alignment_Offset,
    QEP_Linerization_Enable,

    // QEP main menu extension
    QEP_Main_Align_Offset,

    // Field weakening PID
    fw_pid_Kp,
    fw_pid_Ki,

    // PLL amplitude filter
    LBG1_fc_bemf_mag,

    // Power Measurement
    Power_Measure_Iphase_Offset,
    Power_Measure_Iphase_Factor,

    // Id & Iq bandwidth read from firmware
    id_pi_bandwidth,
    iq_pi_bandwidth,

    // Field weakening
    fw_vmax_scl_factor,

    // Hall Tuning
    hall_mech_angle_offset,
    halledge_pol_skew_time,

    // System Fault
    sys_fault,

} ParameterCode;

typedef enum
{
    success,                                    // UART message received successfully
    host_write_error,                           // UART error when writing from host to target (receive message)
    host_read_error,                            // UART error when reading from target to host (transmit message)
    checksum_error,                             // UART checksum error
} CommStatus;



/**
 * @brief This structure holds communication related values
 */
typedef struct
{
    CommandCode cmd_code;
    ParameterCode param_code;
    CommStatus status;
    uint32_t tx_data;
    uint32_t rx_data;

} Struct_Comm;

#endif  //#ifndef COMMSTRUCT_H
