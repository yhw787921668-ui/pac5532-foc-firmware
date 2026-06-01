
/**
 * @file app_struct.h
 * @brief App structure use to hold various application variables
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

#ifndef APP_STRUCT_H
#define APP_STRUCT_H
#include "maths.h"

typedef struct
{
    // App (Running Vars)
    fix16_t dcbus_voltage;                          // Use to hold motor bus DC voltage
    fix16_t descale_voltage_factor;                 // Use to hold motor bus DC voltage
    int32_t descale_voltage_factor_1div_q16;
    int32_t dcbus_voltage_1div_q16;
    uint32_t coast_timeout_counter;
    uint32_t systick_counter;                       //this is timer that start as soon RTC ISR called
    uint32_t systick_div_counter;                   //this is timer that start as soon RTC ISR called
    int32_t app_motor_direction;                    // Direction indicator: +1=forward, -1=reverse
    unsigned int systick_divider_cnt;
    uint16_t speed_scale_shift;
    int32_t v_scale_q16;

    fix16_t dcbus_voltage_fast_q14;                 // Use to compensate DC bus ripple
    fix16_t dcbus_voltage_ave_q14;                  // Heavily filtered bus voltage
    fix16_t dcbus_voltage_compensation_coff_q26;    // Coefficient for DC voltage compensation
    uint32_t gear_mode;
    uint32_t dc_voltage_filter_coff_q16;
    
    // App (Command)
    uint32_t motor_enable_cmd;                      // Set to non-zero if motor is enabled
    fix16_t speed_mode_flag;                        // Enable Speed PI mode
    int32_t app_motor_direction_cmd;                // Direction indicator: +1=forward, -1=reverse
    uint32_t auto_close_loop_cmd;                   // Bit set when we need to go from OL to close loop in auto mode
    uint32_t coast_timeout_target;
    fix16_t idrefcl_cmd_f16;                        // Storage for idref_target_f16
    fix16_t iqrefcl_cmd_f16;                        // Storage for iqref_target_f16
    uint32_t flag_first_start;                      // Flag indicates when the controller powers up for autostart
    int32_t outerloop_target_cmd_f16;

    // App (Config)
    int32_t current_limit;                          // Current limit for this motor
    fix16_t descale_current_factor;                 // Descaling factor for current
    uint16_t motor_led_ticks;                       // Timer ticks in LED for this motor (for SVM)
    uint16_t motor_ted_ticks;                       // Timer ticks in TED for this motor (for SVM)
    unsigned char iphase_amp_gain;                  // this use to set differential amp gain
    unsigned char vphase_amp_gain;                  // this use to set sim amp gain
    fix16_t speedrefcl_min;                         // Hold close loop minimum speed limit
    fix16_t speedrefcl_max;                         // Hold close loop maximum speed limit
    fix16_t powerrefcl_max;                         // Hold close loop maximum power limit
    uint32_t outerloop_scale_q0;

    // Stall Detect and Field Weakening             // JAGTAG: Need to find a home for SDet and FW variables
    q1p14_t vdrvsq_q14;                             // Hold value in fix16_t format
    q1p14_t vdrvsq_q14_peak;
    q1p14_t vdrvsq_q14_peak_stored;
    fix16_t motor_speed_peak;
    fix16_t motor_speed_peak_stored;
} Struct_App;

#endif  //#ifndef APP_STRUCT_H
