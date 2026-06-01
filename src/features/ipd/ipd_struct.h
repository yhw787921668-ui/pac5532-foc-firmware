
/**
 * @file ipd_struct.h
 * @brief Use for ipd structure
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

#ifndef IPD_STRUCT_H
#define IPD_STRUCT_H

#include "maths.h"

/**
 * @brief IPD structure
 */
typedef struct
{
    uint32_t ipd_state;

    int32_t ipd_angle_forward_offset;
    int32_t ipd_angle_reverse_offset;
    int32_t angle_decision_threshold;
    uint32_t num_periods_per_pulse;
    uint32_t pwm_charge_ticks;
    uint32_t pwm_discharge_percent;
    uint32_t num_shunts;
    int32_t phase_lost_detect_min_current_count;
    // End of Initialized Variables

    uint32_t status;
    int32_t ipd_motor_direction;
    int32_t adc_offset_u_current;
    int32_t adc_offset_v_current;
    int32_t adc_offset_w_current;

    uint32_t ipd_current_measurement_state;
    uint32_t timer_period;
    uint32_t pwm_discharge_ticks;
    uint32_t calculated_charge_ticks;
    uint32_t calculated_periods_per_pulse;
    uint32_t period_count;
    int32_t u_pos_current;
    int32_t u_neg_current;
    int32_t v_pos_current;
    int32_t v_neg_current;
    int32_t w_pos_current;
    int32_t w_neg_current;

    // Low saliency motor
    int32_t motor_low_saliency_flag;
    int32_t current_accum_temp;
    int32_t u_pos_current0;
    int32_t u_neg_current0;
    int32_t v_pos_current0;
    int32_t v_neg_current0;
    int32_t w_pos_current0;
    int32_t w_neg_current0;
    uint32_t current_accum_flag;

    // Motor disable period between pulses
    uint32_t motor_disable_between_pulses_q0;
    uint32_t delay_count;

    int32_t ipd_angle;
    fix16_t ipd_starting_angle;

    uint32_t angle_sector;
    int32_t max_current;
    int32_t max_to_min_current_delta;
    int32_t same_polarity_current_delta;
    int32_t opposite_polarity_current_delta;
    int32_t Iu_ADC;
    int32_t Iv_ADC;
    int32_t Iw_ADC;
}Struct_IPD;

#endif
