/**
 * @file atune_struct.h
 * @brief Use for auto-tuning structure definition
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#ifndef AUTOTUNE_PI_STRUCT_H
#define AUTOTUNE_PI_STRUCT_H

#include "maths.h"
#include "aligngo_struct.h"


// ENUM Definitions
typedef enum
{
    STATE_MID2_IDLE                     = 0,
    STATE_MID2_RESET,                   // 1
    STATE_MID2_READY,                   // 2
    STATE_MID2_STEP1,                   // 3
    STATE_MID2_ANOISE,                  // 4
    STATE_MID2_RECORDING,               // 5
    STATE_MID2_STEP2,                   // 6
    STATE_MID2_RECORDDONE,              // 7
    STATE_MID2_CALCULATE,               // 8
    STATE_MID2_COMPLETE,                // 9
    STATE_MID2_FAULT                    // 10
} eStateMID2;

typedef enum
{
    UICMD_ATUNE_NEXTCMD                 = 0,
    UICMD_ATUNE_RESET,                  // 1
    UICMD_ATUNE_ISTEP,                  // 2
    UICMD_ATUNE_CALCMID,                // 3
    UICMD_ATUNE_CALCGAIN_CURRENT,       // 4
    UICMD_ATUNE_CALCGAIN_SPEED,         // 5
    UICMD_ATUNE_MID2_STOP,              // 6
} eUICmdATune;

typedef enum
{
    STATUS_ATUNE_MID1_READY             = BIT_00,   // Status MID1: Ready
    STATUS_ATUNE_MID1_INPROGRESS        = BIT_01,   // Status MID1: In Progress
    STATUS_ATUNE_MID1_COMPLETE          = BIT_02,   // Status MID1: Complete
    STATUS_ATUNE_MID1_FAILED            = BIT_03,   // Status MID1: Failed
    STATUS_ATUNE_MID2_READY             = BIT_04,   // Status MID2: Waiting -> Click "IStep" when motor speed has settled
    STATUS_ATUNE_MID2_INPROGRESS        = BIT_05,   // Status MID2: Stepping Current -> Click "Calculate" when motor speed has settled
    STATUS_ATUNE_MID2_SPEED_SETTLED     = BIT_06,   // Status MID2: Motor speed has settled
    STATUS_ATUNE_MID2_WAITSETTLE        = BIT_07,   // Status MID2: Waiting for motor speed to settle...
    STATUS_ATUNE_MID2_ISTEPWAIT         = BIT_08,   // Status MID2: Stepping current and waiting for motor speed to settle...
    STATUS_ATUNE_MID2_COMPLETE          = BIT_09,   // Status MID2: Complete
    STATUS_ATUNE_MID2_FAILED            = BIT_10,   // Status MID2: Failed
    STATUS_ATUNE_UNUSED_11              = BIT_11,   // Status: Unused
    STATUS_ATUNE_UNUSED_12              = BIT_12,   // Status: Unused
    STATUS_ATUNE_UNUSED_13              = BIT_13,   // Status: Unused
    STATUS_ATUNE_UNUSED_14              = BIT_14,   // Status: Unused
    STATUS_ATUNE_UNUSED_15              = BIT_15,   // Status: Unused
    STATUS_ATUNE_UNUSED_16              = BIT_16,   // Status: Unused
    STATUS_ATUNE_UNUSED_17              = BIT_17,   // Status: Unused
    STATUS_ATUNE_UNUSED_18              = BIT_18,   // Status: Unused
    FAULT_ATUNE_MID1_PHASECHK           = BIT_19,   // Fault MID1: Phase Check Failed
    WARNING_ATUNE_MID1_PHASECHK         = BIT_20,   // Warning MID1: Phase Check Abnormal
    WARNING_ATUNE_MID1_MOTIONDET        = BIT_21,   // Warning MID1: Motion detected while sensing current
    WARNING_ATUNE_MID2_CTRLMODE         = BIT_22,   // Warning MID2: Control mode must be set to Torque
    WARNING_ATUNE_MID2_NOISECHK         = BIT_23,   // Warning MID2: Increase Istep and repeat Step1
    WARNING_ATUNE_ATPI_CALCNEEDED_I     = BIT_24,   // Warning ATPI: Current PI Calculation Necessary
    WARNING_ATUNE_ATPI_CALCNEEDED_SPD   = BIT_25,   // Warning ATPI: Speed PI Calculation Necessary
    FLAG_ATUNE_MID1_UPDATE_CALC         = BIT_26,   // Flag MID1: Update Calculated Values
    FLAG_ATUNE_MID1_UPDATE_CFG          = BIT_27,   // Flag MID1: Update Configuration Values
    FLAG_ATUNE_MID2_UPDATE_CALC         = BIT_28,   // Flag MID2: Update Calculated Values
    FLAG_ATUNE_MID2_UPDATE_CFG          = BIT_29,   // Flag MID2: Update Configuration Values
    FLAG_ATUNE_ATPI_UPDATE_I            = BIT_30,   // Flag ATPI: Update Current PI Values
    FLAG_ATUNE_ATPI_UPDATE_SPD          = BIT_31,   // Flag ATPI: Update Speed PI Values
} eStatusATune;

typedef enum
{
    STATEREC_MID2_VALUES_STORED         = 0,
    STATEREC_MID2_VALUES_READ           // 1
} eStateRecMID2;

typedef enum
{
    STATE_MAINSUB_MID1_IDLE             = 0,
    STATE_MAINSUB_MID1_RESET,           // 1
    STATE_MAINSUB_MID1_RUN,             // 2
    STATE_MAINSUB_MID1_EXIT,            // 3
    STATE_MAINSUB_MID1_COMPLETE         // 4
} eStateMainSubMID1;

typedef enum
{
    MID1_INJMODE_NOINJ = 0,             // No Injection
    MID1_INJMODE_DDC_Q00,               // Only DC in d-axis
    MID1_INJMODE_D00_QDC,               // Only DC in q-axis
    MID1_INJMODE_DDCAC_Q00,             // DC + AC Injection in d-axis
    MID1_INJMODE_DDC_QAC                // DC in d-axis + AC Injection in q-axis
} eMID1InjMode;

typedef enum
{
    MID1_INVERTER_TYPE_MOS,             // 0
    MID1_INVERTER_TYPE_IGBT,            // 1
} eMID1InvType;

typedef enum
{
    STATUS_MID1_IDLE                    = BIT_00,   // Status: Idle, no doing any thing
    STATUS_MID1_IN_PROCESS              = BIT_01,   // Status: Motor Id detection in process
    STATUS_MID1_COMPLETE                = BIT_02,   // Status: Complete
    STATUS_MID1_FAILED                  = BIT_03,   // Status: Motor Id Sequence Failed
    FAULT_MID1_RCHECK                   = BIT_04,   // Fault: Phase currents grossly imbalanced
    WARNING_MID1_RCHECK                 = BIT_05,   // Warning: Phase currents slightly imbalanced
    WARNING_MID1_MOTIONDET              = BIT_06,   // Warning: Motion detected while sensing current
    UNUSED_07                           = BIT_07,   // Unused
    FAULT_MID1_HALTED                   = BIT_08    // Fault: MID1 sequence halted by user
} eStatusMID1;

typedef enum
{
    CMD_MID1SM_SETSTATE_RESET           = BIT_00,   // Set state reset
    CMD_MID1SM_SETSTATE_IDLE            = BIT_01,   // Set state idle
    CMD_MID1SM_SETSTATE_EXIT            = BIT_02,   // Set state exit
    CMD_MID1SM_READSTATE                = BIT_03    // Read state into register
} eCmdMID1SM;

typedef enum
{
    CMD_MID2SM_SETSTATE_RESET           = BIT_00,    // Set state reset
    CMD_MID2SM_SETSTATE_ISTEP           = BIT_01,    // Set state Istep
    CMD_MID2SM_SETSTATE_CALC            = BIT_02,    // Set state Calc
} eCmdMID2SM;

typedef enum
{
    MID1_RUNMODE_AUTO = 0,              // Auto sequence with auto config
    MID1_RUNMODE_AUTO_MANCFG,           // Auto sequence with manual config
    MID1_RUNMODE_MANUAL                 // Manual mode
} eMID1RunMode;

typedef enum
{
    MID2_RUNMODE_AUTO = 0,              // Auto sequence with auto config
    MID2_RUNMODE_AUTO_MANCFG,           // Auto sequence with manual config
    MID2_RUNMODE_MANUAL                 // Manual mode
} eMID2RunMode;

typedef enum
{
    MID1_MANCMD_IDLE = 0,               // No measurement
    MID1_MANCMD_MEAS_RD,                // Measure R in D-axis
    MID1_MANCMD_MEAS_RQ,                // Measure R in Q-Axis
    MID1_MANCMD_MEAS_LD,                // Measure L in D-axis
    MID1_MANCMD_MEAS_LQ                 // Measure L in D-axis
} eMID1ManCmd;


// Groups
#define STATUS_ATUNE_ALLSTATUS_RESET (\
        STATUS_ATUNE_MID2_READY \
        | STATUS_ATUNE_MID2_INPROGRESS \
        | STATUS_ATUNE_MID2_COMPLETE \
        | STATUS_ATUNE_MID2_FAILED \
        | STATUS_ATUNE_MID2_SPEED_SETTLED \
        )

#define STATUS_ATUNE_ALLWARNINGS_FAULTSTATE (\
        WARNING_ATUNE_MID2_CTRLMODE \
        | WARNING_ATUNE_MID2_NOISECHK \
        )

#define STATUS_ATUNE_ALLWARNINGS_RESET (\
        STATUS_ATUNE_MID2_FAILED \
        | WARNING_ATUNE_MID2_CTRLMODE \
        | WARNING_ATUNE_MID2_NOISECHK \
        )

#define STATUS_ATUNE_ALLWARNINGS_MAINSTAT (\
        STATUS_ATUNE_MID2_FAILED \
        | WARNING_ATUNE_MID2_CTRLMODE \
        | WARNING_ATUNE_MID2_NOISECHK \
        | WARNING_ATUNE_ATPI_CALCNEEDED_I \
        | WARNING_ATUNE_ATPI_CALCNEEDED_SPD \
        | WARNING_ATUNE_MID1_PHASECHK \
        | FAULT_ATUNE_MID1_PHASECHK \
        | WARNING_ATUNE_MID1_MOTIONDET \
        )

#define STATUS_ATUNE_MID1_ALLSTATUS (\
        STATUS_ATUNE_MID1_READY \
        | STATUS_ATUNE_MID1_INPROGRESS \
        | STATUS_ATUNE_MID1_COMPLETE \
        | STATUS_ATUNE_MID1_FAILED \
        )

#define STATUS_ATUNE_MID1_ALLWARNINGS (\
        WARNING_ATUNE_MID1_PHASECHK \
        | WARNING_ATUNE_MID1_MOTIONDET \
        )

#define STATUS_ATUNE_MID1_ALLFAULTS (\
        FAULT_ATUNE_MID1_PHASECHK \
        )

#define STATUS_MID1_ALLSTATUS (\
        STATUS_MID1_IDLE \
        | STATUS_MID1_IN_PROCESS \
        | STATUS_MID1_COMPLETE \
        | STATUS_MID1_FAILED \
        )

#if 0
#define STATUS_MID1_ALLFAULTS (\
        FAULT_MID1_RCHECK \
        )
#else
#define STATUS_MID1_ALLFAULTS (\
        FAULT_MID1_RCHECK \
        | FAULT_MID1_HALTED \
        )
#endif

#define STATUS_MID1_ALLWARNINGS (\
        WARNING_MID1_RCHECK \
        | WARNING_MID1_MOTIONDET \
        )

// Structure Definition
typedef struct
{
    // State and flags
    eStateMID2 state_mid2;
    eUICmdATune uicmd_atune;
    eUICmdATune uicmd_atune_previous;
    eStatusATune status_atune;
    eStateRecMID2 staterec_mid2;
    eStateMainSubMID1 state_mainsub_mid1;
    eMID1RunMode mid1_runmode;
    eMID2RunMode mid2_runmode;
    eMID1ManCmd mid1_mancmd;
    eMID1InjMode mid1_injmode;
    eStatusMID1 status_mid1;
    eCmdMID1SM cmd_mid1_sm;
    eCmdMID2SM cmd_mid2_sm;
    uint32_t state_mid1_external : 7;
    uint32_t mid1_motor_enable_cmd : 1;
    uint32_t motion_detected_flag : 1;
    uint32_t motion_detection_complete : 1;
    uint32_t motiondet_to_flag : 1;
    uint32_t mid2_speed_settled : 1;
    uint32_t config_mid1_3step_mode : 1;
    uint32_t mid1_eqvabc_flag : 1;
    uint32_t temp : 18;
    int32_t flag_waitexit;
    int32_t mid1_control_mode_flag;

    // Running variables for Kt & J calculation
    int32_t sum_speed_q14;
    int32_t sum_iq_q14;
    int32_t sum_vpsq_q18;
    int32_t sum_samplenum;
    int32_t sum_speed_store_q14;
    int32_t sum_iq_store_q14;
    int32_t sum_vpsq_store_q18;
    int32_t sum_samplenum_store;
    int32_t mid2_vpsq_q22;
    int32_t mid2_iq_q22;
    int32_t mid2_spd_q22;
    int32_t mid2_acc_q22;
    int32_t mid2_filtacc2_q22;
    int32_t mid2_amax_q22;
    int32_t anoise_thd_q22;
    int32_t anoise_delay_ctr;
    int32_t anoise_warn_ctr;
    int32_t speed_settled_majfilt_cnt;
    int32_t sum_speed2_q14;
    int32_t sum_samplenum2;

    // Configuration
    int32_t dt_control_q26;
    int32_t dt_control_1div_q6;
    int32_t dt_speed_q26;
    int32_t dt_systickdiv_q16;
    int32_t delta_q6;
    int32_t filter_fc_speed_q16;
    int32_t filter_tau_speed_q26;
    int32_t filter_alpha_acc2_q16;
    int32_t filter_alpha_dc_q16;
    int32_t istep_q16;
    int32_t anoise_delay_cycles;
    int32_t anoise_threshold_multp;
    int32_t anoise_fault_retries;
    int32_t accdet_thresh_q22;
    int32_t speed_settled_majfilt_limit;

    int32_t num_limit;
    int32_t num_limit_gui;
    int32_t period_fraction_q16;
    int32_t v_inj_q14;
    int32_t v_align_q14;
    int32_t one_div_numlimit_q16;
    int32_t filter_shift;
    int32_t rdson_ivscale_q16;
    int32_t v_led_scaled_q14;
    int32_t v_ted_scaled_q14;
    eMID1InvType mid1_inverter_type;
    int32_t v_diode_scaled_q14;
    int32_t v_ce_scaled_q14;
    int32_t comp_term_scaled_q14;

    // Running variables for MID1
    int32_t vd_q14;
    int32_t vq_q14;
    int32_t mid1_angdirection;
    int32_t mid1_iter;
    int32_t reqx_q16[3];
    int32_t v_pp_q14;
    int32_t v_pp1_q14;
    int32_t v_pp2_q14;
    int32_t ldx_q26[3];
    int32_t lqx_q26[3];

    // Current target for MID1
    int32_t i_max_control_q14;
    int32_t iac_ref_q14;
    int32_t isq_rms_q28;
    int32_t v_dc_q14;
    int32_t i_dc_q14;
    int32_t psq_app_out_q40;
    int32_t psq_real_out_q40;
    int32_t isq_rms_out_q28;
    int32_t vsq_rms_out_q24;
    int32_t psq_app_q40;
    int32_t psq_real_q40;

    // Configuration
    int32_t i1_align_q14;
    int32_t i2_align_q14;
    int32_t v1_align_q14;
    int32_t v2_align_q14;
    int32_t v_align_inc_q14;
    int32_t i_align_limit1_q14;
    int32_t i_align_limit2_q14;
    uint32_t filter_delay_config_q0;
    int32_t i_inj_gui_q14;
    int32_t i_align1_gui_q14;
    int32_t i_align2_gui_q14;
    Struct_AlignGoCMD aligngo;

    // Running variable use to calculate v1 and v2 dc offsets
    int32_t v_dc1_q14;
    int32_t i_dc1_q14;
    int32_t v_dc2_q14;
    int32_t i_dc2_q14;
    uint32_t filter_delay_q0;

    // Storage
    int32_t mid2_spd_store_q22;
    int32_t mid2_iq1_store_q22;
    int32_t mid2_iq2_store_q22;
    int32_t mid2_vpsq_store_q22;
    int32_t mid2_amax_store_q22;
    int32_t ke_q26;
    int32_t j_q26;
    int32_t speed_ka_q16;
    int32_t speed_kb_q16;
    int32_t id_ka_q16;
    int32_t id_kb_q16;
    int32_t iq_ka_q16;
    int32_t iq_kb_q16;
    int32_t ld_q26;
    int32_t lq_q26;
    int32_t rs_q16;

    // Temp
    int32_t flag_iqref_reg_active;
    int32_t iqref_start_pct_q16;
    int32_t iqref_inc_pct_q16;
    int32_t filter_alpha_anglediff;
    int32_t rcheck_rd_q16;
    int32_t rcheck_rq_q16;
    int32_t motion_max_q14;
    int32_t motion_min_q14;
    int32_t motiondet_delay_limit_cnt;
    int32_t motiondet_to_limit_cnt;
    int32_t motiondet_delta_threshold_pct_q14;
    int32_t power_factor_sq_q16;
} Struct_ATune;

#endif  //#ifndef AUTOTUNE_PI_STRUCT_H

