
/**
 * @file calc_speed.h
 * @brief Definitions for speed calculation routines
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
#ifndef CALC_SPEED_H
#define CALC_SPEED_H

#include "pac5xxx.h"
#include "maths.h"

extern const uint16_t foc_atan_tbl[];

/**
 * @brief  Calculate speed
 * @param  angle_new New angle_new
 * @param  kspd_scaled_q14 Speed gain
 * @param  filter_alpha_speed_q16 Filtered alpha speed
 * @param  speed_scale_shift Speed gain scaled
 * @param  angle_previous Previous angle_new
 * @param  speed_scaled_q28 Scaled speed
 * @param  speed_scaled_q14 Scaled speed
 * @param  speed_radps_q16 Speed in radians / second
 * @param  speed_scaled_prefilt_q14 Scaled speed before filtering
 * @note   Standard version is meant to handle speed filter cutoff frequency ranging between 1-100Hz
 */
extern PAC5XXX_RAMFUNC void calculate_speed(fix16_t angle_new, fix16_t kspd_scaled_q14, fix16_t filter_alpha_speed_q16, uint16_t speed_scale_shift,
        fix16_t *angle_previous, int32_t *speed_scaled_q28, int32_t *speed_scaled_q14, fix16_t *speed_radps_q16, int32_t *speed_scaled_prefilt_q14);

/**
 * @brief  Calculate speed (high speed version)
 * @param  angle_new New angle_new
 * @param  kspd_scaled_q14 Speed gain
 * @param  filter_alpha_speed_q16 Filtered alpha speed
 * @param  speed_scale_shift Speed gain scaled
 * @param  angle_previous Previous angle_new
 * @param  speed_scaled_q28 Scaled speed
 * @param  speed_scaled_q14 Scaled speed
 * @param  speed_radps_q16 Speed in radians / second
 * @param  speed_scaled_prefilt_q14 Scaled speed before filtering
 * @note   HI version is meant to handle speed filter cutoff frequency ranging between 10-100Hz
 */
extern PAC5XXX_RAMFUNC void calculate_speed_hi(fix16_t angle_new, fix16_t kspd_scaled_q14, fix16_t Kspeed_q11, uint16_t speed_scale_shift,
        fix16_t *angle_previous, int32_t *speed_scaled_q28, int32_t *speed_scaled_q14, fix16_t *speed_radps_q16, int32_t *speed_scaled_prefilt_q14);

/**
 * @brief  Calculate speed (low-speed version)
 * @param  angle_new New angle_new
 * @param  kspd_scaled_q14 Speed gain
 * @param  filter_alpha_speed_q16 Filtered alpha speed
 * @param  speed_scale_shift Speed gain scaled
 * @param  angle_previous Previous angle_new
 * @param  speed_scaled_q28 Scaled speed
 * @param  speed_scaled_q14 Scaled speed
 * @param  speed_radps_q16 Speed in radians / second
 * @param  speed_scaled_prefilt_q14 Scaled speed before filtering
 */
extern PAC5XXX_RAMFUNC void calculate_speed_lo(fix16_t angle_new, fix16_t kspd_scaled_q14, fix16_t Kspeed_q14, uint16_t speed_scale_shift,
        fix16_t *angle_previous, int32_t *speed_scaled_q28, int32_t *speed_scaled_q14, fix16_t *speed_radps_q16, int32_t *speed_scaled_prefilt_q14);

/**
 * @brief  Calculate speed coefficients
 * @param  dt_control_q26 Dt control
 * @param  speed_scale_shift Shifted speed scaled 
 * @param  k1_radps_q6 Gain in radians/second
 * @param  k1_scaled_q14 Scaled gain
 */
extern void calculate_speed_coefficients(int32_t dt_control_q26, uint32_t speed_scale_shift, int32_t *k1_radps_q6, int32_t *k1_scaled_q14);

#endif  //#ifndef CALC_SPEED_H
