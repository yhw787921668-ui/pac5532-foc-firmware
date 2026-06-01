
/**
 * @file foc_funcs.c
 * @brief Use for FOC initialization
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
#include "foc.h"
#include "param_calc.h"
#include "control_funcs.h"
#include "control_loop.h"
#ifdef ENABLE_MTPA
#include "mtpa_funcs.h"
#include "maths.h"
#endif

void foc_reset(Struct_Motor* motor_ptr)
{
    pid_reset(&motor_ptr->foc.Id_pid);                                // Reset PID controller data
    pid_reset(&motor_ptr->foc.Iq_pid);                                // Reset PID controller data
#ifdef ENABLE_PLL_ANGLE_CALCULATION
    pid_reset(&motor_ptr->mpos.est_pll.pll_pid);                          // Reset PID controller for PLL estimator
    motor_ptr->mpos.est_pll.pll_amplitude_filter_q16 = 0;
#endif

    motor_ptr->foc.idref_ramp_q14 = 0;
    motor_ptr->foc.idref_ramp_f16 = 0;
    motor_ptr->foc.iqref_ramp_q14 = 0;
    motor_ptr->foc.iqref_ramp_f16 = 0;

    motor_ptr->foc.id_q14 = 0;
    motor_ptr->foc.iq_q14 = 0;
    motor_ptr->foc.vd_q14 = 0;
    motor_ptr->foc.vq_q14 = 0;

    motor_ptr->foc.flag_sat_logicor = 0;                            // Drive saturation flag implemented as logic "or" of control cycles in sampling window
    motor_ptr->foc.flag_sat_logicand = 0;

    motor_ptr->foc.valpha_q14 = 0;
    motor_ptr->foc.vbeta_q14 = 0;

#ifdef ENABLE_DQ_DECOUPLING
    //#if (SEL_DQ_DECOUPLING_METHOD == FEEDBACK_DECOUPLING)
    int32_t fc_temp_q16;
    fc_temp_q16 = fix16_div(motor_ptr->foc.Iq_pid.ka_q16 << 10, motor_ptr->mparam.lq_q26);
    // Convert from rad/s to Hz
    fc_temp_q16 = (fc_temp_q16 >> 10) * QCONST(0.159f, Q10);
    if (fc_temp_q16 > CURRENT_FILTER_CUTOFF_FREQ_Q16) fc_temp_q16 = CURRENT_FILTER_CUTOFF_FREQ_Q16;
    motor_ptr->foc.current_filter_coeff_q16 = calc_filter_alpha(fc_temp_q16, motor_ptr->control.dt_control_q26);

    motor_ptr->foc.id_dec_q14 = 0;
    motor_ptr->foc.iq_dec_q14 = 0;
    //#endif
#endif
}


void foc_systemvar_init(Struct_Motor* motor_ptr)
{
    // default freq: 5 kHz
    motor_ptr->foc.dt_control_q26 = QCONST(0.0002f, Q26);
    motor_ptr->foc.ctrl_divider_foc = 3;

    motor_ptr->foc.overflow_counter = 0;

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    motor_ptr->oneshunt.s1_min_gap = S1_MINGAP;
    motor_ptr->oneshunt.s1_sample_ticks = S1_SAMPLE_ADVANCE_TICKS;
#endif
}


void foc_calc_pwmtimer(Struct_Motor* motor_ptr)
{
    fix16_t pwm_counts, temp2;
    uint32_t temp1;

    // Calculate TimerA (PWM) ticks in up/down mode using Timer Clk frequency, foc_dt, and ctrl_divider
    temp1 = (TIMERCLK_FREQ_HZ >> 10);                                                              // clk_div1024 = clk / 1024     (q0 / q0 = q0)
    temp2 = fix16_div(motor_ptr->foc.dt_control_q26, motor_ptr->foc.ctrl_divider_foc);    // pwm_dt_x1024 = dt_control_q26 / ctrl_divider
    temp2 = fix16_mul(temp2, temp1);                                                            // ticks = pwm_dt_x1024 * clk_div1024    (q16 * q0 = q16)
    temp2 = (temp2 >> 1);                                                                       // ticks_updn = ticks / 2
    pwm_counts = ((temp2 + 0x8000) >> 16);                                                      // Round and shift from q16 to q0

    motor_ptr->foc.motor_period_ticks = pwm_counts;
    motor_ptr->foc.motor_period_ticks_1div = fix16_div(FIX16_1P0, (pwm_counts << 16));
}
