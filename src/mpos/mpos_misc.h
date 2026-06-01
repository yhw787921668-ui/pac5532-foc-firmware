
/**
 * @file mpos_misc.h
 * @brief Use for mpos utility functions
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

#ifndef MPOS_MISC_H
#define MPOS_MISC_H

#include "pac5xxx.h"
#include "config_app.h"
#include "motor_struct.h"
#include "utils.h"


/**
 * @brief Use for mpos angle correction
 * @param Struct_Motor* motor_ptr
 * @param int32_t est_spd_radps
 * @param int32_t angcorr_gain_q20
 * @param int32_t angcorr_offset
 * @param int32_t angle_in
 * @param int32_t *angle_out
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void mpos_angle_correction(Struct_Motor* motor_ptr, int32_t spd_radps, int32_t direction, int32_t angcorr_gain_q20, int32_t angcorr_offset, int32_t angle_in, int32_t *angle_out)
{
    fix16_t temp1 = 0;
    fix16_t temp2 = 0;

#if defined(ENABLE_ANGLE_CORRECTION)
    temp2 += angcorr_offset * direction;
    temp2 += (((spd_radps >> 12) * (angcorr_gain_q20 >> 0)) >> 8);        // 1/(2*2*pi*f0)    q16>>12=q4 * q20 = q24>>8=q16
#endif
#if defined(ENABLE_MTPA)
    temp2 += motor_ptr->mtpa.mtpa_angle_rad_q16 * direction;
#endif

    temp1 = angle_in + temp2;

    if(temp1 > FIX16_PI)
        temp1 = temp1 - FIX16_2PI;
    else if(temp1 < -FIX16_PI)
        temp1 = temp1 + FIX16_2PI;

    *angle_out = temp1;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use for mpos angle blending
 * @param int32_t angleblend_ang1_q16
 * @param int32_t angleblend_ang2_q16
 * @param int32_t *angleblend_cnt
 * @param int32_t angleblend_totalcnt
 * @param int32_t angleblend_factor_q12
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline int32_t mpos_angleblend(int32_t angleblend_ang1_q16, int32_t angleblend_ang2_q16, int32_t *angleblend_cnt, int32_t angleblend_totalcnt, int32_t angleblend_factor_q12)
{
    int32_t angleblend_angout_q16;
    int32_t ang2_prime_q16;

    // Manipulate estimated angle wraparound so that both angles are in the same "wrap"
    if ((angleblend_ang2_q16 - angleblend_ang1_q16) > FIX16_PI)
        ang2_prime_q16 = angleblend_ang2_q16 - FIX16_2PI;
    else if ((angleblend_ang2_q16 - angleblend_ang1_q16) < -FIX16_PI)
        ang2_prime_q16 = angleblend_ang2_q16 + FIX16_2PI;
    else
        ang2_prime_q16 = angleblend_ang2_q16;

    // Calculate blended angle
    angleblend_angout_q16 = (((angleblend_ang1_q16 * angleblend_factor_q12) >> 12) * *angleblend_cnt) +
            (((ang2_prime_q16 * angleblend_factor_q12) >> 12) * (angleblend_totalcnt - *angleblend_cnt));  // q16 * q12 = 3q28>>12=3q16
    wraparound(&angleblend_angout_q16);

//    debug_dac_1a = (*angleblend_cnt) << 14;

    // Decrement blend counter
    if (*angleblend_cnt > 0)
    {
        *angleblend_cnt = *angleblend_cnt - 1;
//        debugio2_toggle();
    }

    return angleblend_angout_q16;
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#ifdef ENABLE_SWITCH_OVER_ANGLE_BLEND
/**
 * @brief Use for mpos angle blending for blending time over 10ms
 * @param int32_t angleblend_ang1_q16
 * @param int32_t angleblend_ang2_q16
 * @param int32_t *angleblend_cnt
 * @param int32_t angleblend_totalcnt
 * @param int32_t angleblend_factor_q12
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline int32_t switchover_angleblend(int32_t angleblend_ang1_q16, int32_t angleblend_ang2_q16, int32_t *angleblend_cnt, int32_t angleblend_totalcnt, int32_t angleblend_factor_q12, int32_t *flag)
{
    int32_t angleblend_angout_q16;
    int32_t ang1_prime_q16, ang2_prime_q16;

    // Manipulate estimated angle wraparound so that both angles are in the same "wrap"
    if (angleblend_ang1_q16 < 0) 
    {
        ang1_prime_q16 = angleblend_ang1_q16 + FIX16_2PI;
    }
    else ang1_prime_q16 = angleblend_ang1_q16;
        
    if ((angleblend_ang2_q16 < 0) || (*flag == 1))
    {
        ang2_prime_q16 = angleblend_ang2_q16 + FIX16_2PI;
        *flag = 1;
    }
    else ang2_prime_q16 = angleblend_ang2_q16;

    // Calculate blended angle
    angleblend_angout_q16 = (((ang1_prime_q16 * angleblend_factor_q12) >> 12) * *angleblend_cnt) +
            (((ang2_prime_q16 * angleblend_factor_q12) >> 12) * (angleblend_totalcnt - *angleblend_cnt));  // q16 * q12 = 3q28>>12=3q16
    wraparound(&angleblend_angout_q16);
    
    //debug_dac_2a = angleblend_ang1_q16;
    //debug_dac_2b = angleblend_ang2_q16;
    //debug_dac_2c = ang2_prime_q16;
    //debug_dac_1c = angleblend_angout_q16;
//    debug_dac_1a = (*angleblend_cnt) << 14;

    // Decrement blend counter
    if (*angleblend_cnt > 0)
    {
        *angleblend_cnt = *angleblend_cnt - 1;
//        debugio2_toggle();
    }

    return angleblend_angout_q16;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

#ifdef ENABLE_SWITCH_OVER_SPEED_BLEND
/**
 * @brief Use for mpos speed blending
 * @param int32_t blend_speed1_q16
 * @param int32_t blend_speed2_q16
 * @param int32_t *blend_cnt
 * @param int32_t blend_totalcnt
 * @param int32_t blend_factor_q12
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline int32_t switchover_speedblend(int32_t blend_speed1_q16, int32_t blend_speed2_q16, int32_t *blend_cnt, int32_t blend_totalcnt, int32_t blend_factor_q12)
{
    int32_t blend_speedout_q16;

    // Calculate blended speed
    blend_speedout_q16 = (((blend_speed1_q16 * blend_factor_q12) >> 12) * *blend_cnt) +
            (((blend_speed2_q16 * blend_factor_q12) >> 12) * (blend_totalcnt - *blend_cnt));  // q16 * q12 = 3q28>>12=3q16
    wraparound(&blend_speedout_q16);

    // Decrement blend counter
    if (*blend_cnt > 0)
    {
        *blend_cnt = *blend_cnt - 1;
    }

    return blend_speedout_q16;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif


#if defined(ENABLE_EST_ASSIST)
/**
 * @brief Applies estimator angle assist to estimated angle
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void est_assist(Struct_Motor* motor_ptr, int32_t ref_angle)
{
    int32_t anglediff;
    static int32_t anglediff_filt;

    anglediff = (motor_ptr->mpos.est.estimated_angle - ref_angle);
    if (anglediff > FIX16_PI)
        anglediff -= FIX16_2PI;
    if (anglediff < -FIX16_PI)
        anglediff += FIX16_2PI;

    filter_shift(anglediff, &anglediff_filt, CONFIG_ESTASSIST_FILTERSHIFT);

    if (anglediff > 0)
    {
        // Apply adpative control increment
        if (anglediff > CONFIG_ESTASSIST_CTRL_THRESH)
            motor_ptr->mpos.estassist_angle_corr -= CONFIG_ESTASSIST_CTRL_INC;
        else
            motor_ptr->mpos.estassist_angle_corr -= 1;

        // Cap angle correction
        if (motor_ptr->mpos.estassist_angle_corr < -FIX16_PI_DIV_2)
            motor_ptr->mpos.estassist_angle_corr = -FIX16_PI_DIV_2;
    }
    else
    {
        // Apply adpative control increment
        if (anglediff < -CONFIG_ESTASSIST_CTRL_THRESH)
            motor_ptr->mpos.estassist_angle_corr += CONFIG_ESTASSIST_CTRL_INC;
        else
            motor_ptr->mpos.estassist_angle_corr += 1;

        // Cap angle correction
        if (motor_ptr->mpos.estassist_angle_corr > FIX16_PI_DIV_2)
            motor_ptr->mpos.estassist_angle_corr = FIX16_PI_DIV_2;
    }

#if 0
//    debug_dac_1b = motor_ptr->mpos.estassist_angle_corr >> 5;
//    debug_dac_1b = motor_ptr->mpos.qep.qep_interpolated_angle >> 5;
//    debug_dac_2b = anglediff >> 5;
//    debug_dac_2c = anglediff_filt >> (5 + CONFIG_HALLASSIST_FILTERSHIFT);

//    debug_var_3 = anglediff;
//    debug_var_4 = anglediff_filt >> CONFIG_HALLASSIST_FILTERSHIFT;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

#endif  //#ifndef MPOS_MISC_H
