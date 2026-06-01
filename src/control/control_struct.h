
/**
 * @file control_struct.h
 * @brief Use for FOC control related variables
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

#ifndef CONTROL_STRUCT_H
#define CONTROL_STRUCT_H

#include "pid.h"

/**
 * @brief This structure use for FOC control related variables
 */
typedef struct
{
    // Control (Running Vars)
    int32_t u_duty_value;                           // hold u duty in 16 bit sign short format
    int32_t v_duty_value;                           // hold v duty in 16 bit sign short format
    int32_t w_duty_value;                           // hold w duty in 16 bit sign short format
    PID_Data_Type outerloop_pid;                        // PID controller for speed setting
    int32_t Iu_measured_q14;                        // Iu measured phase current in q14 format
    int32_t Iv_measured_q14;                        // Iv measured phase current in q14 format
    int32_t Iw_measured_q14;                        // Iw measured phase current in q14 format
    fix16_t outerloop_ramp;                             // Closed loop speed set-point ramp (rad/s) use to control speed slope
    fix16_t outerloop_ramp_scaled_q14;
    int32_t outerloop_fdbk_scaled_q14;
    int32_t ctrl_counter;
    int32_t mod_sector;
    int32_t mod_sector_sreg;
    int32_t mod_va_q14;                                 // Hold SVM u duty in q14 format debugging
    int32_t mod_vb_q14;                                 // Hold SVM v duty in q14 format debugging
    int32_t mod_vc_q14;                                 // Hold SVM w duty in q14 format debugging

    // Control (Command)
    fix16_t outerloop_target;
    fix16_t idref_target_f16;                         // Id_close loop setpoint
    fix16_t iqref_target_f16;                         // Iq close loop setpoint
    fix16_t idiqref_rate_gui_f16;                   // Id/Iq ref Ramp Rate original from GUI

    // Control (Config)
    uint16_t motor_period_ticks;                    // Timer ticks in period for this motor (for SVM)
    uint16_t motor_period_ticks_1div;                    // Timer ticks in period for this motor (for SVM)
    uint16_t duty_max_pct_q14;
    uint16_t duty_max_pct_1div_q14;
    fix16_t dt_control_q26;                                 // dt * 1024
    fix16_t dt_control_1div_q6;                         // 1 / (dt * 1024)
    uint32_t ctrl_divider;                      // this is the ratio between PWM Frequency and FOC loop
    uint16_t outerloop_pi_scale_shift;                  // Shift applied to speed error passed into speed PI controller
    uint16_t scalefactor_speed;
    fix16_t outerloop_rate;
    fix16_t outerloop_rate_store;
    fix16_t idiqref_rate_f16;                       // Id/Iq ref Ramp Rate
    int32_t imax_q16;                               // Maximum allowed regulated total phase current
    int32_t imax_1div_q16;                          // Reciprocal of imax_q16
    int32_t ineg_q16;                               // Maximum allowed regulated total phase current
} Struct_Control;

#endif  //#ifndef CONTROL_STRUCT_H
