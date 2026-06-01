
/**
 * @file trap_struct.h
 * @brief Use for trap structure definition
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

#ifndef TRAP_STRUCT_H
#define TRAP_STRUCT_H

#include "config_app.h"

/*****************************************************************************
Notes:
    1) SIM is has not been tested with Trap+Est and may need certain changes for example the threshold speed definition for SIM2CL
    2) Est2Trap transition is not supported yet. Currently an OC occurs when attempting this transition
    3) Reverse direction is not yet supported. Support will require updates such as adding the direction flag handling and the reverse angle table definition.
    4) Anglediff is not currently used for any of the transitions (Align&Go2Trap and Trap2Est). These transitions may benefit from an anglediff check and autoranging for reliability.
    5) Dynamic mpos pointers can be implemented during transitions to avoid adding a permanent call for the trap mpos process.
    6) Duty cycle minimum is currently set to 1 tick. Thsi could be problematic with the pulse extension issue and may need to be restored to the standard minimum. This may introduce distortion to the current waveform during DPWMMIN operation and therefore needs to be evaluated.
    7) Transition from DPWMMIN to SVM needs to be developed
    8) Over-modulation is not supported in the generic SVM modulator and needs to be developed

*****************************************************************************/

/**
 * @brief trap structure
 */
typedef struct
{
//    // State & Status
    uint32_t transition_flag : 1;                   // Internal flag to indicate when a trap state transition has occurred
    uint32_t transition_flag_startup : 1;
    uint32_t do_speed_cal : 1;                      // Flag to indicate when a trap state transition has occurred and we can do speed cal
    uint32_t flag_speed_calc_enabled : 1;           // Flag to indicate that speed calculation has been enabled (after the first trap transition has occurred)
//    uint32_t trap_check_error_code : 2;
//    uint32_t trap_check_error_state_prev : 3;
//    uint32_t trap_check_error_state_new : 3;
    uint32_t flag_inittrapspdfilt : 1;
    uint32_t transition_ext_flag : 1;               // External flag to indicate when a trap state transition has occurred
//    uint32_t flag_trapedge_comp : 1;                // Flag that indicates if trap edge has rising polarity in order to apply skew compensation
    uint32_t temp : 29;                             // Unused bits
//    uint32_t flag_trap_int;
//
//    // Running Variables
    uint8_t BEMFStateNew;                           // New state at trap state transition
    uint8_t BEMFStateNew_startup;                   // Check if the startup is successful
    uint8_t BEMFStateLast;                          // Previous state at trap state transition
    uint8_t BEMFStateLast_startup;
    uint32_t startup_stall_cnt;                     // Stall detection Counter
    uint8_t BEMFStateStall_prev;                    // BEMF state for stall detection

    fix16_t accumulate_degree_value;                // Summing variable used in angle interpolation calculation
    fix16_t accumulate_degree_per_dt;               // Interpolation angle increment
//    fix16_t base_speed;                           // Timer counts between trap sensor transitions (filtered)
    int32_t trap_speed_control_counter;
    int32_t trap_speed_control_storecounter;
    int32_t trap_spd_prefilt_radps_q16;             // Pre-filter trap speed in rad/s
    fix16_t angle_fix16;                            // trap angle calculated directly from the angle table
    int32_t trap2est_angleblend_cnt;                // Angle blending counter during trap to estimator transition
    int32_t est2trap_delay_cnt;                     // Time delay from EST to TRAP to wait the current decayed
    int32_t transition_counter;                     // Transition counter used to skip speed calculation on the first transition after startup
    int32_t sector_time_sec_q26;                    // Time between trap edge transitions

    // Outputs
    int32_t trap_spd_radps;                         // Filtered descaled trap speed
    int32_t trap_spd_scaled_q14;                    // Filtered scaled trap speed
    int32_t trap_spd_scaled_q28;                    // trap speed filter summing variable
    int32_t trap_spd_prefilt_scaled_q14;            // Pre-filter scaled trap speed
    int32_t trap_angle;                             // trap angle after applying angle correction to angle_fix16
    int32_t trap_interpolated_angle;                // trap angle after applying interpolation to trap_angle
    int32_t trap_motor_direction;                   // Direction indicator: +1=forward, -1=reverse, 0=undetermined
    int32_t trap_est_angle_diff;                    // trap angle after applying interpolation to trap_angle

//    // Configuration
//    uint32_t speed_scale_shift;                     // Speed scale shift
//    int32_t angcorr_gain_fwd_q20;                   // Angle correction speed gain in forward direction
//    int32_t angcorr_offset_fwd;                     // Angle correction offset in forward direction
//    int32_t angcorr_gain_rev_q20;                   // Angle correction speed gain in reverse direction
//    int32_t angcorr_offset_rev;                     // Angle correction offset in reverse direction
    int32_t switchover_spd_lo2hi_radps;              // Switchover speed from trap to estimator mode
    int32_t switchover_spd_hi2lo_radps;              // Switchover speed from estimator to trap mode
//
//    fix16_t trapstate_angle_ref[8];                 // Forward trap state to commutation angle mapping
    fix16_t trapstate_angle_fwd[8];
//    fix16_t trapstate_angle_rev[8];
    fix16_t* ptr_trapstate_angle_dir;               // Pointer to trapstate_angle_fwd or trapstate_angle_rev depending on direction
//    fix16_t* ptr_angcorr_gain;                      // Pointer to angcorr_gain_fwd_q20 or angcorr_gain_rev_q20 depending on direction
//    fix16_t* ptr_angcorr_offset;                    // Pointer to angcorr_offset_fwd or angcorr_offset_rev depending on direction
//    char* trap_ptr_next_state;                      // Pointer to next state sequence table depending on direction
//    char* trap_ptr_prev_state;                      // Pointer to previous state sequence table depending on direction
    char trap_forward_next_state[8];                // Forward trap next state table use to check correct state transition
//    char trap_forward_prev_state[8];                // Forward trap next state table use to check correct state transition
//    int32_t trapedge_polskew_time_q26;              // Polarity skew time delay applied to rising trap edge in seconds
//    int32_t trap_sensor_offset_rad;                 // Offset that configures sensor alignment relative to FOC angle

    // Input Variables
    int32_t bemf_u_scaled_q16;                          // Phase U bemf (Per unit) reading from ADC
    int32_t bemf_v_scaled_q16;                          // Phase V bemf (Per unit) reading from ADC
    int32_t bemf_w_scaled_q16;                          // Phase W bemf (Per unit) reading from ADC

    // Outputs
    int32_t phase_u_logic_raw;                          // Phase U logical signal vs Central tap: 1 means high, 0 means low
    int32_t phase_v_logic_raw;                          // Phase V logical signal vs Central tap: 1 means high, 0 means low
    int32_t phase_w_logic_raw;                          // Phase W logical signal vs Central tap: 1 means high, 0 means low

    int32_t phase_u_filter_cnt;                         // Phase U majority filter count
    int32_t phase_v_filter_cnt;                         // Phase V majority filter count
    int32_t phase_w_filter_cnt;                         // Phase W majority filter count
    int32_t phase_u_logic_filt;                         // Phase U logical signal vs Central tap with majority filter
    int32_t phase_v_logic_filt;                         // Phase V logical signal vs Central tap with majority filter
    int32_t phase_w_logic_filt;                         // Phase W logical signal vs Central tap with majority filter

#ifdef ENABLE_BEMF_SAMPLE_SINGLE_PHASE
    int32_t bemf_x_scaled_q16;                          // Phase X (U, V or W) bemf (Per unit) reading from ADC
    int32_t phase_x_logic_raw;                          // Phase X logical signal vs Central tap: 1 means high, 0 means low
    int32_t phase_x_filter_cnt;                         // Phase X majority filter count
    int32_t phase_x_logic_filt;                         // Phase X logical signal vs Central tap with majority filter
#endif

    // Configuration
    int32_t bemf_conv_adc2volts_scaled_q16;
}Struct_Trap;

#endif  //#ifndef TRAP_STRUCT_H
