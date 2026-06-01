
/**
* @file sim_i_struct.h
* @brief I-SIM structure definition
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

#ifndef SIM_I_STRUCT_H
#define SIM_I_STRUCT_H

#include "maths.h"
#include "pid.h"


/**
* @brief Structure holding I-SIM variables
*/
typedef struct
{
    uint32_t signal_detected: 1;                  // Bit set when BEMF is high enough to be detected
    uint32_t motion_detected: 1;                  // Bit set when BEMF is high enough to be detected
    uint32_t motion_notdetected: 1;                  // Bit set when BEMF is high enough to be detected
    uint32_t transition_complete: 1;
    uint32_t temp: 28;                          // Unused bits out of 32 bits
    fix16_t freeInMotion_min_speed_limit;       // Motor free in motion minimum speed limit to go to close loop
    uint32_t state_sim;                         //REVB Release, this is hold SIM states
    uint32_t status_sim;                        //REVB Release, this is return SIM status
    uint32_t state_sim2cl;
    int phase_u_adc;
    int phase_v_adc;
    int phase_w_adc;
    int32_t phase_adc_threshold_q14;            // Minimum threshold value between CT and Phase Voltage that can treat as active bemf voltage
    int32_t iphase_u_q14;
    int32_t iphase_v_q14;
    int32_t iphase_w_q14;
    int32_t sim_alpha_q14;
    int32_t sim_beta_q14;
    int32_t sim_angle;
    int32_t sim_angle_previous;
    int32_t sim_spd_scaled_q28;
    int32_t sim_spd_scaled_q14;
    int32_t sim_spd_radps;
    int32_t sim_spd_prefilt_scaled_q14;
    int32_t multp_conv_adc2volts_q16;
    int32_t multp_conv_adc2vscaled_q16;
    int32_t sim_motor_direction;                   // Direction indicator: +1=forward, -1=reverse, 0=undetermined
    int32_t ctrl_counter_sim2cl;

    int32_t dt_control_q26;
    fix16_t dt_control_1div_q6;                         // 1 / (dt * 1024)
    int32_t speed_scale_shift;
    int32_t kspd_scaled_q14;
    int32_t kspd_radps_q16;
    int32_t filter_alpha_speed_q16;

    int16_t braking_duty_cycle_q14;
    uint32_t braking_timeout_countmax;
    uint32_t braking_timeout_counter;           // Braking timeout counter
    fix16_t align_count_braking_timeout;        // Align count, this is for startup when braking timeouts
    fix16_t OL_Id_ref_braking_mul;   // OL Id_ref setpoint multiplier, this is for startup when braking timeouts
    fix16_t OL_Iq_ref_braking_mul;   // OL Iq_ref setpoint multiplier, this is for startup when braking timeouts
    uint16_t sim_counter;
    uint16_t sim_mdetect_countmax;
    uint16_t sim_detect2_countmax;
    uint16_t braking_duty_min_ticks;
    int16_t braking_duty_min_q14;
    int32_t sim_duty_cycle_dec_q14;
    int32_t braking_duty_cycle_dec_q14;
    int32_t mdetect_duty_cycle_dec_q14;

    int32_t angle_inc_q16;
    int32_t angle_inc_per_pwmdt;
    int32_t angle_blend_cnt;
    int32_t sim2cl_maxspd_radps_q16;
    int32_t ve_mag_scaled_q14;
}Struct_SIM;

#endif  //#ifndef SIM_I_STRUCT_H
