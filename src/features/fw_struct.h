
/**
 * @file config_features.h
 * @brief Use for field weakening structure
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

#ifndef FW_H
#define FW_H

#include "config_features.h"
#include "pid.h"

/**
 * @brief field weakening structure
 */
typedef struct
{
    // Config
    fix16_t spd_threshold;                      // Use to hold speed threshold in close loop for entering field weakening
    fix16_t id_sp;                              // Use to hold limit for Id in field weakening
#if defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == INCREMENTRAL_CONTROL)
    // Flags
    int32_t flag_sat_logicand;                  // Flag is high only when all control periods within sampling period are saturated
    int32_t flag_sat_logicor;                   // Flag is high when at lesat 1 control period within sampling period is saturated
    int32_t fw_active_flag;                     // Flag is high when field weakening is active
    fix16_t flag_semaphore;                     // Flag to gate the reading of variables in the control loop when being written at the systick rate

    // Running Variables
    int32_t idrefcl_target_store_f16;           // Semaphore storage variable to be used while calculating new value for idrefcl_target_f16
    int32_t iqmax_q14;                          // Maximum value of iqref for outerloop controller calculated during FW
    int32_t iqmax_store_q14;                    // Semaphore storage variable to be used while calculating new value for iqmax_q14
#elif defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == PID_CONTROL)
    // Flags
    int32_t fw_active_flag;                     // Flag is high when field weakening is active

    // Running Variables
    PID_Data_Type fw_pid;                       // PID controller for feild weakening setting

    fix16_t vq_vd_squares_sum_q14;
    fix16_t vq_vd_squares_root_q14;
    fix16_t vrms_filter_coeff_q16;
    fix16_t vq_vd_squares_root_filt_q14;
    fix16_t vmax_scl_q14;
#endif
} Struct_FW;

#endif  //#ifndef FW_H
