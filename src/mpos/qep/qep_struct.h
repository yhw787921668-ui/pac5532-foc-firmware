
//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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

#ifndef QEP_STRUCT_H
#define QEP_STRUCT_H

#include "maths.h"
#include "config_app.h"
#include "config_features.h"

// Interplate for the QUAD mode
#if defined(ENABLE_QEP_QUAD_MODE)
    #define QEP_PULSE_PER_RESOLUTION_FINAL          (QEP_PULSE_PER_RESOLUTION << 2)
#else
    #define QEP_PULSE_PER_RESOLUTION_FINAL           QEP_PULSE_PER_RESOLUTION
#endif

typedef struct
{
    // Running Variables
    int32_t qep_mech_angle;                         // Mechanical angle calculated from QEP counter
    int32_t qep_calculated_angle;                   // Electrical angle derived from mechanical angle using poles
    int32_t qep_previous_angle;                     // Previous electrical angle used for speed calculation
    int32_t counter_counts;                         // QEP counter value
    int32_t index_counter;                          // QEP index occurrence counter
    int32_t elec_angle_wrap_around_counts;          // Angle wrap around counter
    int32_t qep2est_angleblend_cnt;                // Angle blending counter during hall to estimator transition

    // Outputs
    int32_t qep_spd_radps;                          // Filtered descaled QEP speed
    int32_t qep_spd_scaled_q14;                     // Filtered scaled QEP speed
    int32_t qep_spd_scaled_q28;                     // QEP speed filter summing variable
    int32_t qep_spd_prefilt_scaled_q14;             // Pre-filter scaled QEP speed
    int32_t qep_angle;                              // QEP angle after applying angle correction to qep_calculated_angle
    int32_t qep_motor_direction;                    // Direction indicator: +1=forward, -1=reverse, 0=undetermined

    // Configuration
    uint32_t qep_resolution;                        // QEP Resulution in ticks/MechRevolution
    int32_t angcorr_offset_fwd;                     // QEP Angle correction offset forward
    int32_t angcorr_offset_rev;                     // QEP Angle correction offset reverse
    int32_t filter_fc_speed_q16;                    // QEP speed filter cutoff frequency in Hz
    int32_t filter_alpha_speed_q16;                 // QEP speed filter gain
    int32_t kspd_radps_q6;                          // Conversion factor from delta angle to speed in rad/s
    int32_t kspd_scaled_q14;                        // Conversion factor from delta angle to scaled speed
    int32_t conv_qep2mech_q24;                      // Conversion factor from QEP counter to mechanical angle in rad
    fix16_t* ptr_angcorr_offset;                    // Pointer to angcorr_offset_fwd or angcorr_offset_rev depending on direction
    int32_t qep_sensor_offset_rad;                  // Offset that configures sensor alignment relative to FOC angle
    int32_t qep_sensor_offset_deg;                  // Offset that configures sensor alignment relative to FOC angle: Degree
    int32_t qep_elec_angle_counts_threshold;        // Angle wrap around threshold
    int32_t switchover_spd_lo2hi_radps;             // Switchover speed from hall to estimator mode
    int32_t switchover_spd_hi2lo_radps;             // Switchover speed from estimator to hall mode

#if defined(ENABLE_QEP_LINEARIZATION_TUNING) || defined(ENABLE_QEP_LIN_FLASHRW)
    // Arrary to store linearization data
    int32_t qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL];
#endif

#if defined(ENABLE_QEP_LINEARIZATION_TUNING)
    uint32_t qep_linearization_enable_cmd;          // Set to 1 if the linearizatin is enabled
    uint32_t convert_rad_to_qep_count_q10;
    uint32_t index_count;
    int32_t index_angle;
    int32_t accurate_angle_ref_idx_aligned;
    uint32_t accurate_qep_count_q6;
    uint32_t qep_physical_count;
    uint32_t qep_linearization_start_flag;
    uint32_t qep_linearization_done_flag;
#endif

#ifdef ENABLE_QEP_OFFSET_TUNING
    uint32_t qep_offset_tuning_enable_cmd;          // Set to 1 if the qep offset tuning is enabled
    int32_t qep_offset_angle_deg_q16;               // QEP mechanical offset angle
#endif
} Struct_QEP;

#endif  //#ifndef QEP_STRUCT_H

