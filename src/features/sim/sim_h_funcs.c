
/**
 * @file sim_h_funcs.c
 * @brief Use for SIM hall utility functions
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#include "motor_struct.h"
#include "sim_funcs.h"
#include "calc_speed.h"


#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_HALL)

// JAGTAG: Customize sim_calculate_parameters() for SIM-H

/**
 * @brief This function use to calculate SIM parameters
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_calculate_parameters(Struct_Motor* motor_ptr)
{
    int32_t temp1;

    // Calculate SIM dt independently from foc dt
    motor_ptr->sim.dt_control_q26 = fix16_div(motor_ptr->control.dt_control_q26, motor_ptr->foc.ctrl_divider_foc << 16) * SIM_CONTROL_DIVIDER;
    motor_ptr->sim.dt_control_1div_q6 = fix16_div(FIX16_1P0, motor_ptr->sim.dt_control_q26);

#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)
    // Set min speed for SIM2CL to 0
    motor_ptr->sim.freeInMotion_min_speed_limit = 0;
#endif

    // Set SIM speed scale shift same as the application speed scale shift
    motor_ptr->sim.speed_scale_shift = motor_ptr->app.speed_scale_shift;

    // Calculate the speed calculator coefficients
    calculate_speed_coefficients(motor_ptr->sim.dt_control_q26, motor_ptr->sim.speed_scale_shift, &motor_ptr->sim.kspd_radps_q16, &motor_ptr->sim.kspd_scaled_q14);

    // Set SIM2CL max speed to percentage of application max speed
    motor_ptr->sim.sim2cl_maxspd_radps_q16 = fix16_mul(motor_ptr->app.speedrefcl_max, SIM_SIM2CL_MAXSPD_PCT_Q16);

    // Define sim.sim_detect2_countmax according to hall speed filter depth
    motor_ptr->sim.sim_detect2_countmax = SIM_HALLFILTER_SETTLENUM;

    // Calculate min_duty_limit_ticks = time * clk * 0.5(up-down timer) * period_ticks
    motor_ptr->sim.braking_duty_min_ticks = ((fix16_mul(BRAKING_DUTY_LIMIT_SEC_Q26, (TIMERCLK_FREQ_HZ >> 10))) >> 1);                                                   // q26 * q0>>10=qm10 = q0
    motor_ptr->sim.braking_duty_min_q14 = ((((fix16_div(motor_ptr->sim.braking_duty_min_ticks, motor_ptr->control.motor_period_ticks)) << 1) - Q16_1P0) >> 2);      // q16>>2 = q14

    // Calculate braking duty decrement per SIM dt
    temp1 = fix16_div((SIM_BRAKING_DUTYRAMPTIME_SEC_Q8 << 8), (motor_ptr->sim.dt_control_q26)) >> 6;   // temp1 = Ramp counts in SIM dt's
    motor_ptr->sim.braking_duty_cycle_dec_q14 = (fix16_div((QCONST(1.0, Q14) - motor_ptr->sim.braking_duty_min_q14) << 8, temp1) >> 24);   // decrement = swing / counts
    if(motor_ptr->sim.braking_duty_cycle_dec_q14 <= 0)
        motor_ptr->sim.braking_duty_cycle_dec_q14 = 1;
    else if(motor_ptr->sim.braking_duty_cycle_dec_q14 > QCONST(1.0, Q14))
        motor_ptr->sim.braking_duty_cycle_dec_q14 = QCONST(1.0, Q14);
}


/**
 * @brief This function use to initialize SIM
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_init(Struct_Motor* motor_ptr)
{
}


// JAGTAG: This may be converted to a generic function for all SIM methods

/**
 * @brief This function use to update SIM duty cycle
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_update_duty(Struct_Motor *motor_ptr)
{
    // Update phase voltage values
    motor_ptr->control.mod_va_q14 = motor_ptr->sim.braking_duty_cycle_q14;
    motor_ptr->control.mod_vb_q14 = motor_ptr->sim.braking_duty_cycle_q14;
    motor_ptr->control.mod_vc_q14 = motor_ptr->sim.braking_duty_cycle_q14;
}


/**
 * @brief This function use to detect motion in hall sensor mode
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_h_detect_motion(Struct_Motor *motor_ptr)
{
    motor_ptr->sim.signal_detected = 0;

    if (motor_ptr->mpos.hall.transition_ext_flag)
    {
        motor_ptr->sim.signal_detected = 1;

        if (++motor_ptr->sim.sim_counter >= SIM_MDETECT_HALLEDGENUM)
        {
            motor_ptr->sim.sim_counter = 0;
            motor_ptr->sim.motion_detected = 1;
        }
    }

    // Check if hall timer has expired
    else if (motor_ptr->sim.flag_hall_timer_int)
    {
        motor_ptr->sim.flag_hall_timer_int = FALSE;
        motor_ptr->sim.motion_notdetected = 1;
    }
}


// JAGTAG: This may be converted to a generic function for all SIM methods

/**
 * @brief This function use to ramp SIM braking duty cycle
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void sim_h_ramp_duty(Struct_Motor *motor_ptr)
{
    // Decrement duty cycle
    motor_ptr->sim.braking_duty_cycle_q14 = motor_ptr->sim.braking_duty_cycle_q14 - motor_ptr->sim.sim_duty_cycle_dec_q14;

    // Apply duty min to duty ramp
    if (motor_ptr->sim.braking_duty_cycle_q14 < motor_ptr->sim.braking_duty_min_q14)
    {
        motor_ptr->sim.braking_duty_cycle_q14 = motor_ptr->sim.braking_duty_min_q14;
    }
}

#endif  //#ifdef ENABLE_SIM
