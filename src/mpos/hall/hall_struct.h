
/**
 * @file hall_struct.h
 * @brief Use for hall structure definition
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

#ifndef HALL_STRUCT_H
#define HALL_STRUCT_H

#include "config_app.h"


/**
 * @brief hall structure
 */
typedef struct
{
    // State & Status
    uint32_t transition_flag : 1;                   // Internal flag to indicate when a hall state transition has occurred
    uint32_t do_speed_cal : 1;                      // Flag to indicate when a hall state transition has occurred and we can do speed cal
    uint32_t flag_speed_calc_enabled : 1;           // Flag to indicate that speed calculation has been enabled (after the first hall transition has occurred)
    uint32_t hall_check_error_code : 2;
    uint32_t hall_check_error_state_prev : 3;
    uint32_t hall_check_error_state_new : 3;
    uint32_t flag_inithallspdfilt : 1;
    uint32_t transition_ext_flag : 1;               // External flag to indicate when a hall state transition has occurred
    uint32_t flag_halledge_comp : 1;                // Flag that indicates if hall edge has rising polarity in order to apply skew compensation
    uint32_t temp : 18;                             // Unused bits
    uint32_t flag_hall_int;

    // Running Variables
    uint8_t hall_int_set;                           // Hall port interrupt setting value
    uint8_t SensorStateGet_flag;                    // New state changed flag
    uint8_t SensorStateNew;                         // New state at hall state transition
    uint8_t SensorStateLast;                        // Previous state at hall state transition
    fix16_t accumulate_degree_value;                // Summing variable used in angle interpolation calculation
    fix16_t accumulate_degree_per_dt;               // Interpolation angle increment
    fix16_t base_speed;                             // Timer counts between hall sensor transitions (filtered)
    int32_t hall_spd_prefilt_radps_q16;             // Pre-filter hall speed in rad/s
    fix16_t angle_fix16;                            // Hall angle calculated directly from the angle table
    int32_t hall2est_angleblend_cnt;                // Angle blending counter during hall to estimator transition
    int32_t transition_counter;                     // Transition counter used to skip speed calculation on the first transition after startup
    int32_t sector_time_sec_q26;                    // Time between hall edge transitions

    // Outputs
    int32_t hall_spd_radps;                         // Filtered descaled hall speed
    int32_t hall_spd_scaled_q14;                    // Filtered scaled hall speed
    int32_t hall_spd_scaled_q28;                    // Hall speed filter summing variable
    int32_t hall_spd_prefilt_scaled_q14;            // Pre-filter scaled hall speed
    int32_t hall_angle;                             // Hall angle after applying angle correction to angle_fix16
    int32_t hall_interpolated_angle;                // Hall angle after applying interpolation to hall_angle
    int32_t hall_motor_direction;                   // Direction indicator: +1=forward, -1=reverse, 0=undetermined

    // Configuration
    uint32_t speed_scale_shift;                     // Speed scale shift
    int32_t angcorr_gain_fwd_q20;                   // Angle correction speed gain in forward direction
    int32_t angcorr_offset_fwd;                     // Angle correction offset in forward direction
    int32_t angcorr_gain_rev_q20;                   // Angle correction speed gain in reverse direction
    int32_t angcorr_offset_rev;                     // Angle correction offset in reverse direction
    int32_t switchover_spd_lo2hi_radps;             // Switchover speed from hall to estimator mode
    int32_t switchover_spd_hi2lo_radps;             // Switchover speed from estimator to hall mode

    fix16_t hallstate_angle_ref[8];                 // Forward hall state to commutation angle mapping
    fix16_t hallstate_angle_fwd[8];
    fix16_t hallstate_angle_rev[8];
    fix16_t* ptr_hallstate_angle_dir;               // Pointer to hallstate_angle_fwd or hallstate_angle_rev depending on direction
    fix16_t* ptr_angcorr_gain;                      // Pointer to angcorr_gain_fwd_q20 or angcorr_gain_rev_q20 depending on direction
    fix16_t* ptr_angcorr_offset;                    // Pointer to angcorr_offset_fwd or angcorr_offset_rev depending on direction
    char* hall_ptr_next_state;                      // Pointer to next state sequence table depending on direction
    char* hall_ptr_prev_state;                      // Pointer to previous state sequence table depending on direction
    char hall_forward_next_state[8];                // Forward hall next state table use to check correct state transition
    char hall_forward_prev_state[8];                // Forward hall next state table use to check correct state transition
    int32_t halledge_polskew_time_q26;              // Polarity skew time delay applied to rising hall edge in seconds
    int32_t hall_sensor_offset_rad;                 // Offset that configures sensor alignment relative to FOC angle
    int32_t hall_sensor_offset_deg_gui;             // For GUI display
    int32_t halledge_polskew_msec_gui_q16;          // For GUI display
}Struct_Hall;

#endif  //#ifndef HALL_STRUCT_H
