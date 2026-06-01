
/**
 * @file control_funcs.c
 * @brief Use for FOC control functions
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

#ifndef CONTORL_FUNCS_H
#define CONTORL_FUNCS_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "utils.h"

#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    #include "control_3shunt_funcs.h"
#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    #include "control_1shunt_funcs.h"
#endif
#ifdef ENABLE_BOOTPROT
    #include "foc_funcs.h"
#endif

/* This macro use to check is duty cycle within the range of min and max */
#ifndef ENABLE_DUTY_LIMIT_BANDEXCLUDE  // Absolute min/max
#define DUTY_MIN_MAX(duty_cycle, duty_min_ticks, duty_max_ticks, motor_period_ticks) do{\
    if (duty_cycle < duty_min_ticks) \
        duty_cycle = duty_min_ticks; \
    else if (duty_cycle > duty_max_ticks) \
        duty_cycle = duty_max_ticks; \
} while(0)
#else
#define DUTY_MIN_MAX(duty_cycle, duty_min_ticks, duty_max_ticks, motor_period_ticks) do{\
    if (duty_cycle < duty_min_ticks) \
    { \
        duty_cycle = DUTY_CYCLE_MIN_TICK; \
    } \
    else if (duty_cycle > duty_max_ticks) \
        duty_cycle = motor_period_ticks; \
} while(0)
#endif


void control_reset(Struct_Motor* motor_ptr);
void calculate_outerloop_scale_shift(fix16_t value_max, uint16_t *shift, uint32_t *outerloop_scale_q0);
void control_param_process(Struct_Motor* motor_ptr, ParameterCode param_code);
void config_ctrlctr(Struct_Motor* motor_ptr);
void deconfig_ctrlctr(Struct_Motor* motor_ptr);


#if defined(ENABLE_STALL_DETECT)
/**
 * @brief This function use to calculate peak value of vdrive
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_RAMFUNC static inline void calc_vdrivepeak(Struct_Motor* motor_ptr)
{
    fix16_t motor_speed_nr_abs_local;

    if (motor_ptr->mpos.mpos_spd_radps > 0)
        motor_speed_nr_abs_local = motor_ptr->mpos.mpos_spd_radps;
    else
        motor_speed_nr_abs_local = -motor_ptr->mpos.mpos_spd_radps;
    // q14 = q14 * q14 = q28 >> 14 = q14
    motor_ptr->app.vdrvsq_q14 = (((motor_ptr->foc.valpha_q14 * motor_ptr->foc.valpha_q14) + (motor_ptr->foc.vbeta_q14 * motor_ptr->foc.vbeta_q14))  >> 14);
    if (motor_ptr->app.vdrvsq_q14 > motor_ptr->app.vdrvsq_q14_peak)
        motor_ptr->app.vdrvsq_q14_peak = motor_ptr->app.vdrvsq_q14;
#ifdef ENABLE_STALL_DETECT
    if (motor_speed_nr_abs_local > motor_ptr->app.motor_speed_peak)
    #ifdef ENABLE_STALL_DETECT_STEADY_STATE
        motor_ptr->app.motor_speed_peak = motor_ptr->control.outerloop_ramp;
    #else
        motor_ptr->app.motor_speed_peak = motor_speed_nr_abs_local;
    #endif
#endif
}
#endif


/**
 * @brief This function use to copy application variables to ramp variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void update_outerloop_ramp_sw(Struct_Motor* motor_ptr)
{
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
    // Update outerloop ramp value with calculated power
    motor_ptr->control.outerloop_ramp = motor_ptr->power.power_result;
#elif (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
    // Update outerloop ramp value with switchover speed
    motor_ptr->control.outerloop_ramp = motor_ptr->mpos.mpos_spd_radps;
#endif
    motor_ptr->control.outerloop_ramp_scaled_q14 = (motor_ptr->control.outerloop_ramp >> (motor_ptr->control.outerloop_pi_scale_shift + 2));
}

#if defined(ENABLE_ZERO_DUTY_CYCLE_ENHANCEMENT)
PAC5XXX_OPTIMIZE_SPEED_ON 
PAC5XXX_RAMFUNC static inline void switch_Latch_mode(int16_t duty_cycle_u, int16_t duty_cycle_v, int16_t duty_cycle_w)
{
    static uint16_t counter_u = 0;
    static uint16_t counter_v = 0;
    static uint16_t counter_w = 0;
    // Phase U
    if (duty_cycle_u == 0)
    {
        if (counter_u == 1)
        {
            FOC_PWM_Timer_Ptr->CCTL4.CCLATCH = 1;   // Latch at Period
        }
        counter_u++;
    }
    else 
    {
        counter_u = 0;
        FOC_PWM_Timer_Ptr->CCTL4.CCLATCH = 0;       // Latch at Cnt 0
    }

    // Phase V
    if (duty_cycle_v == 0)
    {
        if (counter_v == 1)
        {
            FOC_PWM_Timer_Ptr->CCTL5.CCLATCH = 1;   // Latch at Period
        }
        counter_v++;
    }
    else 
    {
        counter_v = 0;
        FOC_PWM_Timer_Ptr->CCTL5.CCLATCH = 0;       // Latch at Cnt 0
    }

    // Phase W
    if (duty_cycle_w == 0)
    {
        if (counter_w == 1)
        {
            FOC_PWM_Timer_Ptr->CCTL6.CCLATCH = 1;   // Latch at Period
        }
        counter_w++;
    }
    else 
    {
        counter_w = 0;
        FOC_PWM_Timer_Ptr->CCTL6.CCLATCH = 0;       // Latch at Cnt 0
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

/**
 * @brief This function use to convert Ta, Tb and Tc to u,v and w duty cyce and check duty within min-max range
 * @param Struct_Motor* motor_ptr pointer to motor struct
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void convert_duty_cycle(Struct_Motor* motor_ptr)
{
    int16_t u1_duty, v1_duty, w1_duty;
    uint16_t temp_min, temp_max, temp_period;

    temp_period = motor_ptr->control.motor_period_ticks;
    temp_min = motor_ptr->foc.svm_min_ticks;
    temp_max = motor_ptr->foc.svm_max_ticks;
    //change Ta/b/c to duty cycle
    u1_duty = (((motor_ptr->control.mod_va_q14 >> 1) + Q14_0P5) * temp_period) >> 14;     // q16>>2=q14 * q0 = q14>>14 = q0
    v1_duty = (((motor_ptr->control.mod_vb_q14 >> 1) + Q14_0P5) * temp_period) >> 14;     // q16>>2=q14 * q0 = q14>>14 = q0
    w1_duty = (((motor_ptr->control.mod_vc_q14 >> 1) + Q14_0P5) * temp_period) >> 14;     // q16>>2=q14 * q0 = q14>>14 = q0

    //check relationship between duty cycle and applied voltage for estimator scaling oc Vd/Vq
    DUTY_MIN_MAX(u1_duty, temp_min, temp_max, temp_period);
    DUTY_MIN_MAX(v1_duty, temp_min, temp_max, temp_period);
    DUTY_MIN_MAX(w1_duty, temp_min, temp_max, temp_period);

#if defined(ENABLE_ZERO_DUTY_CYCLE_ENHANCEMENT)
    // Enhance Zero duty cycle and 100% duty cycle control
    switch_Latch_mode(u1_duty, v1_duty, w1_duty);
#endif

#ifdef ENABLE_BOOTPROT
    // Bootstrap Protection
    u1_duty = check_duty_cycle_bootstrap_time(u1_duty, &motor_ptr->bootprot.bootprot_count1, motor_ptr);
    v1_duty = check_duty_cycle_bootstrap_time(v1_duty, &motor_ptr->bootprot.bootprot_count2, motor_ptr);
    w1_duty = check_duty_cycle_bootstrap_time(w1_duty, &motor_ptr->bootprot.bootprot_count3, motor_ptr);
#endif

    motor_ptr->control.u_duty_value = u1_duty;
    motor_ptr->control.v_duty_value = v1_duty;
    motor_ptr->control.w_duty_value = w1_duty;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline void config_control_dt(Struct_Motor *motor_ptr, int32_t dt_control_q26, int32_t dt_control_1div_q6, uint32_t ctrl_divider)
{
    motor_ptr->control.dt_control_q26 = dt_control_q26;
    motor_ptr->control.dt_control_1div_q6 = dt_control_1div_q6;
    motor_ptr->control.ctrl_divider = ctrl_divider;
}


/**
 * @brief Increment Counter
 * @param PointervStruct_Motor* motor_ptr pointer to motor structure, not used
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static void control_counter_increment(Struct_Motor* motor_ptr)
{
    // Increment control counter
    motor_ptr->control.ctrl_counter++;
#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
    if (motor_ptr->control.ctrl_counter < (motor_ptr->control.ctrl_divider))
    {
        motor_ptr->fptr.fptr_int_clr_0();
    }
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef CONTORL_FUNCS_H
