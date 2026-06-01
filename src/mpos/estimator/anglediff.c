
/**
 * @file anglediff.c
 * @brief Use to check for angle difference between open loop and est angle and est speed
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

#include "anglediff.h"
#include "fault.h"
#include "utils.h"


#if (MODULE_MPOS_LBG1 == ENABLED) && (MPOS_SELECT != MPOS_LBG1_START_TRAP_ALIGNGO)

void anglediff_check(Struct_Motor* motor_ptr, int32_t ref_angle, int32_t check_angle, int32_t ref_speed_radps, int32_t check_speed_radps)
{
    int32_t speed_diff_abs;
    int32_t angle_diff_abs_q12;

    // Calculate speed difference and absolute value
    speed_diff_abs = ref_speed_radps - check_speed_radps;
    if (speed_diff_abs < 0)
        speed_diff_abs = -speed_diff_abs;

    // Calculate angle difference
    fix16_t angle_diff_q12 = (ref_angle - check_angle) >> 4;
    if (angle_diff_q12 > (FIX16_PI >> 4))
        angle_diff_q12 -= (FIX16_2PI >> 4);
    else if (angle_diff_q12 < ((-FIX16_PI) >> 4))
        angle_diff_q12 += (FIX16_2PI >> 4);
    motor_ptr->mpos.angle_diff_q12 = angle_diff_q12;

    // Calculate angle difference absolute value
    angle_diff_abs_q12 = angle_diff_q12;
    if (angle_diff_abs_q12 < 0)
        angle_diff_abs_q12 = -angle_diff_abs_q12;

    // Check if speed difference is larger than threshold
    if (speed_diff_abs >= fix16_mul(ANGLEDIFF_SPEED_ERROR_PCT_Q16, motor_ptr->app.speedrefcl_max))       // q16
    {
        // This flag tells that it's a right time to switch from open loop to close loop by check angle difference and speed
        motor_ptr->mpos.anglediff_pass_flag = 0;
        set_warning(WARNING_SPDDIF);
    }
    else
    {
        clear_warning(WARNING_SPDDIF);

        // Check if angle difference is larger than threshold
//        if (angle_diff_abs_q12 <= ANGLEDIFF_ANGLE_ERROR_DEG_Q12)
        if ((angle_diff_abs_q12 <= ANGLEDIFF_ANGLE_ERROR_DEG_Q12) && ((angle_diff_q12 * motor_ptr->app.app_motor_direction) < 0))
        {
            clear_warning(WARNING_ANGLDIF);
            motor_ptr->mpos.anglediff_pass_flag = 1;
        }
        else
        {
            set_warning(WARNING_ANGLDIF);
            motor_ptr->mpos.anglediff_pass_flag = 0;
        }
    }
}


void anglediff_autorange(Struct_Motor *motor_ptr)
{
#ifdef ENABLE_ANGLEDIFF_AUTORANGE
    if (motor_ptr->app.auto_close_loop_cmd)
    {
#if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_1)
        minimize_anglediff(((motor_ptr->mpos.angle_diff_q12 << 4) * motor_ptr->app.app_motor_direction), &motor_ptr->control.idref_target_f16, motor_ptr->mpos.aligngo.itotrefol_target_q16,
                motor_ptr->mpos.aligngo.itotrefol_target_q16, motor_ptr->foc.idref_ramp_f16, QCONST(ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT, Q10), ENABLE_IDREF_BIDIRECTION_FLAG);
        // Run circle function to limit the Iq and Id
        circle_limiter(motor_ptr->mpos.aligngo.itotrefol_target_q16, motor_ptr->mpos.aligngo.itotrefol_target_1div_q16, motor_ptr->control.idref_target_f16, &motor_ptr->control.iqref_target_f16);
        motor_ptr->control.iqref_target_f16 = motor_ptr->control.iqref_target_f16 * motor_ptr->app.app_motor_direction;
#elif (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
        motor_ptr->startup.aligngo2est_angleramp_q16 += (motor_ptr->mpos.angle_diff_q12 << 4);
#endif
    }
#endif
}


void anglediff_timeout_check(Struct_Motor *motor_ptr)
{
#ifdef ENABLE_ANGLEDIFF_TIMEOUT
   if (motor_ptr->app.auto_close_loop_cmd)
   {
        if (motor_ptr->mpos.est.angle_diff_timeout_counter++ >= ANGLEDIFF_TIMEOUT_COUNT)
        {
            set_fault(FAULT_SPDANGLDIF_TIMEOUT);            // Set motor stall status
            motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
        }
    }
    else
        motor_ptr->mpos.est.angle_diff_timeout_counter = 0;
#endif
}

#endif  //#if (MODULE_MPOS_LBG1 == ENABLED)
