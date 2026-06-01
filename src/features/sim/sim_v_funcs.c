
/**
* @file sim_v_funcs.c
* @brief V-SIM function definitions
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

#include "motor_struct.h"
#include "sim_funcs.h"
#include "calc_speed.h"

// Note that these header files are from outside the SIM library. If they change, the SIM library needs to be recompiled!
#include "foc_math.h"
#include "mpos_funcs.h"


#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE))

void sim_calculate_parameters(Struct_Motor* motor_ptr)
{
    int32_t temp1;

    // Calculate SIM dt independently from foc dt
    motor_ptr->sim.dt_control_q26 = fix16_div(motor_ptr->control.dt_control_q26, motor_ptr->foc.ctrl_divider_foc << 16) * SIM_CONTROL_DIVIDER;
    motor_ptr->sim.dt_control_1div_q6 = fix16_div(FIX16_1P0, motor_ptr->sim.dt_control_q26);

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    // Set min speed for SIM2CL same as the OL2CL speed
    motor_ptr->sim.freeInMotion_min_speed_limit = ((motor_ptr->startup.aligngo.speedrefol_cmd_q26 >> 9) * (motor_ptr->foc.dt_control_1div_q6 >> 7));        // f16>>9=8q7 * f16>>7=6q9 = q16 (rad/s)
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    // Set min speed for SIM2CL to 0
    motor_ptr->sim.freeInMotion_min_speed_limit = 0;
#endif

    // Set SIM speed filter gain
    motor_ptr->sim.filter_alpha_speed_q16 = calc_filter_alpha(SIM_DETECT2_SPEEDFILTER_FC_HZ_Q16, motor_ptr->sim.dt_control_q26);

    // Set SIM speed scale shift same as the application speed scale shift
    motor_ptr->sim.speed_scale_shift = motor_ptr->app.speed_scale_shift;

    // Calculate the speed calculator coefficients
    calculate_speed_coefficients(motor_ptr->sim.dt_control_q26, motor_ptr->sim.speed_scale_shift, &motor_ptr->sim.kspd_radps_q16, &motor_ptr->sim.kspd_scaled_q14);

    // Set SIM2CL max speed to percentage of application max speed
    motor_ptr->sim.sim2cl_maxspd_radps_q16 = fix16_mul(motor_ptr->app.speedrefcl_max, SIM_SIM2CL_MAXSPD_PCT_Q16);

    // Calculate sim.sim_detect2_countmax based on the filter rise time (For 99% settling, Tsettle = 4.6 * Tau = 3 / (2pi * fc))
    temp1 = fix16_mul(FIX16_2PI, SIM_DETECT2_SPEEDFILTER_FC_HZ_Q16);    // q16
    temp1 = fix16_div(QCONST(4.6f, Q16), temp1);                        // q16
    motor_ptr->sim.sim_detect2_countmax = fix16_div(temp1, motor_ptr->sim.dt_control_q26) >> 6;     // q0 = div(q16,q26)=q6 >> 6
    if (motor_ptr->sim.sim_detect2_countmax < 10)
        motor_ptr->sim.sim_detect2_countmax = 10;

    // Calculate motion detection countmax = (SIM_MDETECT_DETECTTIME_SEC_Q16 / sim_dt) * 2 (>> (Qshift - 1))
    motor_ptr->sim.sim_mdetect_countmax = fix16_div(SIM_MDETECT_DETECTTIME_SEC_Q16, motor_ptr->sim.dt_control_q26) >> 5;         // q0 = div(q16,q26)=q6 >> 6
    if (motor_ptr->sim.sim_mdetect_countmax < 10)
        motor_ptr->sim.sim_mdetect_countmax = 10;

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


void sim_init(Struct_Motor* motor_ptr)
{
    // Convert phase ADC threshold from ADC counts to q14 value for use by SIM
    motor_ptr->sim.phase_adc_threshold_q14 = ((SIM_BEMF_DET_THRESHOLD_ADCCNT_V * CONV_Q10TOQ14) >> 14);
}


unsigned char sim_conv_ampgainval2gain (unsigned char amp_gain)
{
    unsigned char rv = 1;

    if ((amp_gain <= 7) && amp_gain)
    {
        if (amp_gain == 7)
            rv = 48;
        else
            rv = 1 << (amp_gain - 1);
    }

    return rv;
}


void sim_update_duty(Struct_Motor *motor_ptr)
{
    // Update phase voltage values
    motor_ptr->control.mod_va_q14 = motor_ptr->sim.braking_duty_cycle_q14;
    motor_ptr->control.mod_vb_q14 = motor_ptr->sim.braking_duty_cycle_q14;
    motor_ptr->control.mod_vc_q14 = motor_ptr->sim.braking_duty_cycle_q14;
}

#endif  //#ifdef ENABLE_SIM
