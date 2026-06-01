
/**
 * @file aligngo_struct.h
 * @brief Use for align and go enums and structure
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

#ifndef ALIGNGO_STRUCT_H
#define ALIGNGO_STRUCT_H


/**
 * @brief align and go enum
 */
typedef enum
{
    STATE_ALIGNGO_MAIN_RESET = 0,                   // 0: Align&Go Controller - Reset State
    STATE_ALIGNGO_MAIN_ALIGN,                       // 1: Align&Go Controller - Align State
    STATE_ALIGNGO_MAIN_RAMPING,                     // 2: Align&Go Controller - Speed Ramp State
    STATE_ALIGNGO_MAIN_RAMPCOMPLETE,                // 3: Align&Go Controller - Speed Ramp Complete State
} eStateAlignGoMain;

/**
 * @brief align and go enum
 */
typedef enum
{
    STATE_ALIGNGO_CONTROL_ALIGN = 0,                // 0: Align&Go Control Loop - Align Mode
    STATE_ALIGNGO_CONTROL_SPEEDRAMP,                // 1: Align&Go Control Loop - Speed Ramp Mode
    STATE_ALIGNGO_CONTROL_ANGLERAMP,                // 2: Align&Go Control Loop - Angle Ramp Mode
    STATE_ALIGNGO_CONTROL_IDLE
} eStateAlignGoControl;

/**
 * @brief align and go enum
 */
typedef enum
{
    STATUS_ALIGNGO_RUNNING              = BIT_00,   // 00 : Align&Go is running
    STATUS_ALIGNGO_COMPLETE             = BIT_01    // 01 : Align&Go is complete
} eStatusAlignGo;

/**
 * @brief align and go structure
 */
typedef struct
{
    // State & Status
    eStateAlignGoMain state_main;                   // Align&Go controller state
    eStateAlignGoControl state_control;             // Align&Go control loop mode
    eStatusAlignGo status;                          // Align&Go status

    // Running Variables
    fix16_t alignangle_q26;                         // Ideal (calculated) angle from -PI*1024 to PI*1024
    fix16_t aligntime_counter;                      // Open loop soft start align count temp value, need to hold motor in align in PWM cycle counts
    fix16_t speedrefol_ramp_q26;                    // Angular increment per control period * 1024

    // Target Variables
    fix16_t alignangle_target_q26;
    fix16_t aligntime_target;                       // Open loop soft start align count comes from GUI, fix 16, 0.0001 is equal to 6 counts
    fix16_t speedrefol_target_q26;                  // Current control Open loop angle increment * 1024
    fix16_t speedrefol_target_radps_q16;            // OL speed in rad/s
    fix16_t speedrefol_ramprate_q26;                // Open loop current control soft start frequency increment (angle increment during SS * 1024)
    fix16_t idrefol_target_q16;                     // Id_ol setpoint
    fix16_t iqrefol_target_q16;                     // Iq_ol setpoint
    fix16_t itotrefol_target_q16;                   // Total open loop current target
    fix16_t itotrefol_target_1div_q16;              // Reciprocal of total open loop current target
    fix16_t irefol_ramprate_q16;                    // OL current ramping rate during non-linear speed ramp
    fix16_t speedrefol_target_stg1_q26;             // OL speed target for the beginning ramping rate

    //#ifdef ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP    // This #define is removed becauses of Lib dependency
    fix16_t startup_volt_reach_target_flag;         // Flag to freeze the voltage ref for OL
    fix16_t startup_volt_ramp_q14;
    fix16_t startup_volt_inc_q14;
    //#endif
}Struct_AlignGoRun;


typedef struct
{
    // Command Variables
    fix16_t alignangle_cmd_q26;
    fix16_t aligntime_cmd;                          // Open loop soft start align count comes from GUI, fix 16, 0.0001 is equal to 6 counts
    fix16_t speedrefol_cmd_q26;                         // Current control Open loop angle increment * 1024
    fix16_t speedrefol_ramprate_cmd_q26;                        // Open loop current control soft start frequency increment (angle increment during SS * 1024)
    fix16_t idrefol_cmd_q16;                     // Id_ol setpoint
    fix16_t itotrefol_cmd_q16;                   // Total open loop current target
}Struct_AlignGoCMD;


typedef enum
{
    param_aligngo_align_angle,
    param_aligngo_align_time,
    param_aligngo_speed_ramprate,
    param_aligngo_speed_target,
    param_aligngo_id_ref,
    param_aligngo_itot_ref
} ParamCodeAlignGo;

#endif  //#ifndef ALIGNGO_STRUCT_H
