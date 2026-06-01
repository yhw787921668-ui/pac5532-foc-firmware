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

#ifndef QEP_FUNCS_H
#define QEP_FUNCS_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "param_funcs.h"
#include "qep_io_config.h"
#include "qep_timer_config.h"
#include "utils.h"
#include "mpos_misc.h"
#include "calc_speed.h"

#if (MODULE_MPOS_QEP == ENABLED)

//////////////////////////////////////////
// Definitions
//////////////////////////////////////////
#ifdef ENABLE_QEP_LINEARIZATION_TUNING
    // Accumulation shift for filtering the angle ripples from the estimator
    #define ACCUMULATION_NUM_OF_SHIFT     7
    #define TOTAL_ACCUMULATION_CNT        (1 << ACCUMULATION_NUM_OF_SHIFT)
#endif
#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
    #define QEP2EST_ANGLEBLEND_TOTALCNT        128
    #define QEP2EST_ANGLEBLEND_FACTOR_Q12      QCONST((1.0 / (float)QEP2EST_ANGLEBLEND_TOTALCNT), Q12)
#endif

//////////////////////////////////////////
// Function Declarations
//////////////////////////////////////////
void qep_init(Struct_Motor* motor_ptr);
void qep_reset(Struct_Motor* motor_ptr);
void qep_param_calc(Struct_Motor* motor_ptr);
void qep_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
void qep_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
extern void qep_isr(void);
extern void qep_offset_linearization_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
PAC5XXX_RAMFUNC void qep_process(Struct_Motor* motor_ptr);


//////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void qep_read_counter(Struct_Motor* motor_ptr)
{
    // Read QEP counter
    motor_ptr->mpos.qep.counter_counts = QEP_TIMER_COUNT;

    // If counter wraps to 64K then map it down to max counts for QEP
    if (motor_ptr->mpos.qep.counter_counts >= (1 << 15))
    {
        motor_ptr->mpos.qep.counter_counts = motor_ptr->mpos.qep.counter_counts - (1 << 16) + QEP_PULSE_PER_RESOLUTION_FINAL;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void qep_calc_motor_angle(Struct_Motor* motor_ptr)
{
    int32_t qep_elec_angle_q16;

    // Calculate mechanical angle
    motor_ptr->mpos.qep.qep_mech_angle = (motor_ptr->mpos.qep.counter_counts * motor_ptr->mpos.qep.conv_qep2mech_q24) >> 8;      // q16 = (q0*q24)>>8

    #ifdef ENABLE_QEP_LIN_FLASHRW
    // with linearized data
    int32_t temp, temp_qep_angle_corr_q16;
    if (motor_ptr->mpos.qep.counter_counts > (QEP_PULSE_PER_RESOLUTION_FINAL - 1))  temp = QEP_PULSE_PER_RESOLUTION_FINAL - 1;
    else if(motor_ptr->mpos.qep.counter_counts < 0)   temp = 0;
    else temp = motor_ptr->mpos.qep.counter_counts;

    // angle correction from linearization, q16 = (q3*q21)>>8
    temp_qep_angle_corr_q16 = (((motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[temp]) >> 3) * (motor_ptr->mpos.qep.conv_qep2mech_q24 >> 3)) >> 8;
    motor_ptr->mpos.qep.qep_mech_angle += temp_qep_angle_corr_q16;
    #endif

    // Calculate electrical angle
    qep_elec_angle_q16 = motor_ptr->mpos.qep.qep_mech_angle;
    qep_elec_angle_q16 += motor_ptr->mpos.qep.qep_sensor_offset_rad;
    while (qep_elec_angle_q16 > FIX16_2PI)
    {
        qep_elec_angle_q16 = qep_elec_angle_q16 - FIX16_2PI;
    }
    wraparound(&qep_elec_angle_q16);
    motor_ptr->mpos.qep.qep_calculated_angle = qep_elec_angle_q16;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void qep_calc_motor_speed(Struct_Motor* motor_ptr)
{
    calculate_speed(motor_ptr->mpos.qep.qep_calculated_angle, motor_ptr->mpos.qep.kspd_scaled_q14, motor_ptr->mpos.qep.filter_alpha_speed_q16, motor_ptr->app.speed_scale_shift,
            &motor_ptr->mpos.qep.qep_previous_angle, &motor_ptr->mpos.qep.qep_spd_scaled_q28, &motor_ptr->mpos.qep.qep_spd_scaled_q14, &motor_ptr->mpos.qep.qep_spd_radps,
            &motor_ptr->mpos.qep.qep_spd_prefilt_scaled_q14);

    if (motor_ptr->mpos.qep.qep_spd_radps < 0)
        motor_ptr->mpos.qep.qep_motor_direction = -1;
    else
        motor_ptr->mpos.qep.qep_motor_direction = 1;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#if defined(ENABLE_ANGLE_CORRECTION) || defined(ENABLE_MTPA)
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void qep_angle_correction(Struct_Motor* motor_ptr)
{
    // Angle Correction (speed based linear)
    mpos_angle_correction(motor_ptr, motor_ptr->mpos.qep.qep_spd_radps, motor_ptr->mpos.qep.qep_motor_direction, 0, *motor_ptr->mpos.qep.ptr_angcorr_offset, motor_ptr->mpos.qep.qep_calculated_angle, &motor_ptr->mpos.qep.qep_angle);
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void qep_count_elect_wrap_around(Struct_Motor* motor_ptr)
{
    fix16_t temp;
    static volatile fix16_t previous_angle = 0;

    if (motor_ptr->mpos.qep.elec_angle_wrap_around_counts <= motor_ptr->mpos.qep.qep_elec_angle_counts_threshold)
    {
        temp = motor_ptr->mpos.qep.qep_calculated_angle - previous_angle;

        // Handle delta angle calculation over wrap-around
        if (-temp > FIX16_PI)
            motor_ptr->mpos.qep.elec_angle_wrap_around_counts++;
        else if (temp > FIX16_PI)
            motor_ptr->mpos.qep.elec_angle_wrap_around_counts++;

        previous_angle = motor_ptr->mpos.qep.qep_calculated_angle;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#endif  //#if (MODULE_MPOS_QEP == ENABLED)
#endif  //#ifndef QEP_FUNCS_H
