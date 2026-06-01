
/**
* @file halltune_struct.h
* @brief Hall tuning structure, state definitions, and status definitions
*/

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef HALLTUNE__STRUCT_H
#define HALLTUNE__STRUCT_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "aligngo_struct.h"

#if defined(ENABLE_HALL_TUNING)

/**
* @brief Hall tuning state definitions
*/
typedef enum
{
    STATE_HALLTUNE_RESET = 0,                       // 0: Halltune Controller State - Reset
    STATE_HALLTUNE_RUN,                             // 1: Halltune Controller State - Run
    STATE_HALLTUNE_IDLE                             // 2: Halltune Controller State - Idle
} eStateHallTune;


/**
* @brief Hall tuning status definitions
*/
typedef enum
{
    STATUS_HALLTUNE_RUNNING             = BIT_00,   // 00 : Halltune Status - Running
    STATUS_HALLTUNE_DETECTING_ANGLE     = BIT_01,   // 01 : Halltune Status - Detecting Hall Sensor Angles
    STATUS_HALLTUNE_ANGLE_DETECTED      = BIT_02,   // 02 : Halltune Status - Hall Sensor Angle Mapping Done
    STATUS_HALLTUNE_READY               = BIT_03,   // 00 : Halltune Status - Ready
    FAULT_HALLTUNE_DETECTION_FAILURE    = BIT_16,   // 16 : Halltune Fault - Hall Angle Detection Failure
    FAULT_HALLTUNE_SNAP_FAILURE         = BIT_17    // 17 : Halltune Fault - Snap Calculation Failure **OBSOLETE**
} eStatusHallTune;


/**
* @brief Hall tuning mode definitions
*/
typedef enum
{
    HALLTUNE_MODE_NORMAL = 0,                       // 0: Halltune Mode - Normal
    HALLTUNE_MODE_EDGEPOLSKEW,                      // 1: Halltune Mode - Edge Polarity Skew Compensation
    HALLTUNE_MODE_INDIVIDUALEDGE                    // 2: Halltune Mode - Individual Edge Correction
} eHallTuneMode;


/**
* @brief Structure holding hall sensor tuning variables
*/
typedef struct
{
    // State & Status
    eStateHallTune state;                           // Halltune Controller State
    uint32_t halltune_enable_flag : 1;              // Flag to start hall tuning
    uint32_t flag_halltune_filter_settled : 1;      // Flag to indicate that hall tuning filter has settled
    uint32_t filter_delta_outofrange : 1;           // Flag to indicate that the hall angle filters are out of settling range at any time within the sampling range
    uint32_t unused : 29;                           // Unused bits
    eStatusHallTune status;                         // Halltune controller status
    uint32_t gui_cmd;                               // GUI Command
    eHallTuneMode halltune_mode;                    // Hall tuning mode

    // Running Variables
    fix16_t hallstate_angle_tuning[8];              // Forward hall angle table during tuning
    int32_t hallstate_angle_tuning_filtered_q20[8]; // Table that holds the hall angle filter accumulator
    fix16_t hallstate_angle_ref[8];                 // Forward hall state to commutation angle mapping
    int32_t transition_counter;                     // Counts hall state transitions during the sampling range for filter settling
    fix16_t hall_transition_prevangle;              // Last angle recorded before a hall transition

    // Sub-Structures
    Struct_AlignGoCMD aligngo;                         // Align&Go structure

    // Output Variables
    int32_t halltune_output_1;
    int32_t halltune_output_2;
    int32_t halltune_output_3;
    int32_t halltune_output_4;
    int32_t halltune_output_5;
    int32_t halltune_output_6;
}Struct_HallTune;

#endif      //#if defined(ENABLE_HALL_TUNING)
#endif      //#ifndef HALLTUNE__STRUCT_H

