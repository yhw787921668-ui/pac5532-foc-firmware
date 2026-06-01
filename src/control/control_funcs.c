
/**
 * @file control_funcs.c
 * @brief Use for FOC control functions
 */

//=============================================================================
// Copyright (C) 2017 - 2020, Qorvo, Inc.
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
#include "control_funcs.h"
#include "param_funcs.h"
#include "utils.h"

#if defined(ENABLE_FIELD_WEAKENING)
#include "fw_funcs.h"
#endif


/**
 * @brief This function use to reset FOC related variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void control_reset(Struct_Motor* motor_ptr)
{
    pid_reset(&(motor_ptr->control.outerloop_pid));                             // Reset PID controller data

    // Reset duty cycle variables
    motor_ptr->control.u_duty_value = (motor_ptr->control.motor_period_ticks >> 1);                                    // hold u duty in 16 bit sign short format
    motor_ptr->control.v_duty_value = (motor_ptr->control.motor_period_ticks >> 1);                                    // hold v duty in 16 bit sign short format
    motor_ptr->control.w_duty_value = (motor_ptr->control.motor_period_ticks >> 1);                                    // hold w duty in 16 bit sign short format
#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && ((MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC) || (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY))
    motor_ptr->oneshunt.duty_u_up = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.duty_u_dn = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.duty_v_up = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.duty_v_dn = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.duty_w_up = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.duty_w_dn = (motor_ptr->control.motor_period_ticks >> 1);
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    motor_ptr->oneshunt.duty_comp_sign_1 = 1;
    motor_ptr->oneshunt.duty_comp_sign_2 = 1;
    motor_ptr->oneshunt.duty_comp_sign_3 = 1;

    motor_ptr->oneshunt.flag_current_valid = 1;
    motor_ptr->oneshunt.flag_current_valid_sreg = 1;
    motor_ptr->oneshunt.svm_sector_prev = 2;

    motor_ptr->oneshunt.taccr0_value = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.taccr1_value = (motor_ptr->control.motor_period_ticks >> 1);
#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && ((MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC) || (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY))
    motor_ptr->oneshunt.taccr0_value = (motor_ptr->control.motor_period_ticks >> 1);
    motor_ptr->oneshunt.taccr1_value = (motor_ptr->control.motor_period_ticks >> 1);
#endif

    motor_ptr->control.outerloop_ramp = 0;
    motor_ptr->control.outerloop_ramp_scaled_q14 = 0;
    motor_ptr->control.outerloop_fdbk_scaled_q14 = 0;

    motor_ptr->control.iqref_target_f16 = 0;
    motor_ptr->control.idref_target_f16 = 0;

    #ifdef ENABLE_BOOTPROT
    motor_ptr->bootprot.bootprot_count1 = 0;
    motor_ptr->bootprot.bootprot_count2 = 0;
    motor_ptr->bootprot.bootprot_count3 = 0;
    #endif

#if defined(ENABLE_FIELD_WEAKENING)
    fw_reset(motor_ptr);
#endif

    motor_ptr->control.mod_sector = 2;
    motor_ptr->control.mod_sector_sreg = 2;
    motor_ptr->control.mod_va_q14 = 0;
    motor_ptr->control.mod_vb_q14 = 0;
    motor_ptr->control.mod_vc_q14 = 0;
}


/**
 * @brief This function use to calculate outer loop scale factor
 * @param fix16_t value_max
 * @param uint16_t *shift
 * @param uint32_t *outerloop_scale_q0
 */
void calculate_outerloop_scale_shift(fix16_t value_max, uint16_t *shift, uint32_t *outerloop_scale_q0)
{
    fix16_t value_max_padded;

    // Absolute value
    if (value_max < 0)
        value_max = -value_max;

    // Cap max value at 16383 so that 2x does not overflow
    if (value_max > QCONST(16383.0f, Q16))
        value_max = QCONST(16383.0f, Q16);

    // Pad max value to 2x (to allow for overshoots and noise)
    value_max_padded = (value_max << 1);

    // Generate scaling shift applied to outer loop PI controller
    if (value_max_padded >= QCONST(16384, Q16))
        *shift = 15;              // Shift=15, Res=0.5, Min=16384, Max=32768<
    else if (value_max_padded >= QCONST(8192, Q16))
        *shift = 14;              // Shift=14, Res=0.25, Min=8192, Max=16384<
    else if (value_max_padded >= QCONST(4096, Q16))
        *shift = 13;              // Shift=13, Res=0.125, Min=4096, Max=8192<
    else if (value_max_padded >= QCONST(2048, Q16))
        *shift = 12;              // Shift=12, Res=0.0625, Min=2048, Max=4096<
    else if (value_max_padded >= QCONST(1024, Q16))
        *shift = 11;              // Shift=11, Res=0.03125, Min=1024, Max=2048<
    else if (value_max_padded >= QCONST(512, Q16))
        *shift = 10;              // Shift=10, Res=0.015625, Min=512, Max=1024<
    else if (value_max_padded >= QCONST(256, Q16))
        *shift = 9;               // Shift=9, Res=0.0078125, Min=256, Max=512<
    else if (value_max_padded >= QCONST(128, Q16))
        *shift = 8;               // Shift=8, Res=0.00390625, Min=128, Max=256<
    else if (value_max_padded >= QCONST(64, Q16))
        *shift = 7;               // Shift=7, Res=0.001953125, Min=64, Max=128<
    else if (value_max_padded >= QCONST(32, Q16))
        *shift = 6;               // Shift=6, Res=0.0009765625, Min=32, Max=64<
    else if (value_max_padded >= QCONST(16, Q16))
        *shift = 5;               // Shift=5, Res=0.00048828125, Min=16, Max=32<
    else if (value_max_padded >= QCONST(8, Q16))
        *shift = 4;               // Shift=4, Res=0.000244140625, Min=8, Max=16<
    else if (value_max_padded >= QCONST(4, Q16))
        *shift = 3;               // Shift=3, Res=0.0001220703125, Min=4, Max=8<
    else if (value_max_padded >= QCONST(2, Q16))
        *shift = 2;               // Shift=2, Res=0.00006103515625, Min=2, Max=4<
    else if (value_max_padded >= QCONST(1, Q16))
        *shift = 1;               // Shift=1, Res=0.000030517578125, Min=1, Max=2<
    else   // <= 0
        *shift = 0;               // Shift=0, Res=0.0000152587890625, Min=0, Max=1<

    *outerloop_scale_q0 = (1 << *shift);
}


void control_param_process(Struct_Motor* motor_ptr, ParameterCode param_code)
{
    switch(param_code)
    {
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
    case outerloop_pid_min:
    case outerloop_pid_max:
        if (motor_ptr->app.app_motor_direction == -1)
        {
            motor_ptr->control.outerloop_pid.min_value = (-motor_ptr->control.imax_q16 >> 2);
            motor_ptr->control.outerloop_pid.max_value = (-motor_ptr->control.ineg_q16 >> 2);
        }
        else
        {
            motor_ptr->control.outerloop_pid.min_value = (motor_ptr->control.ineg_q16 >> 2);
            motor_ptr->control.outerloop_pid.max_value = (motor_ptr->control.imax_q16 >> 2);
        }
        break;
#elif (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER)
    case outerloop_pid_min:
        motor_ptr->control.outerloop_pid.min_value = (motor_ptr->control.ineg_q16 >> 2);
        break;

    case outerloop_pid_max:
        motor_ptr->control.outerloop_pid.max_value = (motor_ptr->control.imax_q16 >> 2);
        break;

    case Power_setpoint:
        motor_ptr->control.outerloop_target = motor_ptr->app.outerloop_target_cmd_f16;
        break;
#endif

    case id_ref_cl:
        if (motor_ptr->state.main == STATE_MAIN_RUN)
            motor_ptr->control.idref_target_f16 = motor_ptr->app.idrefcl_cmd_f16;
        break;

    case iq_ref_cl:
#ifdef ENABLE_AUTOTUNE_REGULATE_IQREF
        if ((!motor_ptr->atune.flag_iqref_reg_active) || (motor_ptr->atune.flag_iqref_reg_active && (motor_ptr->app.iqrefcl_cmd_f16 >= motor_ptr->control.iqref_target_f16)))
        {
            motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16;
        }
#else
        if ((motor_ptr->state.main == STATE_MAIN_RUN) && (motor_ptr->state.control == STATE_CONTROL_TORQUE))
                motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16 * motor_ptr->app.app_motor_direction;
#endif
        break;

    case closed_loop_speed_setpoint:
#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_SPEED)
        motor_ptr->control.outerloop_target = motor_ptr->app.outerloop_target_cmd_f16 * motor_ptr->app.app_motor_direction;
#endif
        break;

    default:
        break;
    }
}


void config_ctrlctr(Struct_Motor* motor_ptr)
{
    // Configure Function Pointers
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    //////////////////////////////////////////////
    // CONFIG_SHUNT_3SHUNT
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_ccr0_2          =    (void (*)(void *)) &control_counter_increment;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_ccr0_2          =    (void (*)(void *)) &control_counter_increment;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    //////////////////////////////////////////////
    // MODE_1SHUNT_EXTENSION
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_base_2          =    (void (*)(void *)) &control_counter_increment;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_base_2          =    (void (*)(void *)) &control_counter_increment;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_base_2          =    (void (*)(void *)) &control_counter_increment;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_base_2          =    (void (*)(void *)) &control_counter_increment;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC_TIMER_55XX
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_base            =    (void (*)(void *)) &control_counter_increment;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_base            =    (void (*)(void *)) &control_counter_increment;

    #endif

#endif

    // Configure Interrupts
    config_int_ctrlctr();
}


void deconfig_ctrlctr(Struct_Motor* motor_ptr)
{
    // Deconfigure Interrupts
    deconfig_int_ctrlctr();

    // Deconfigure Function Pointers
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    //////////////////////////////////////////////
    // CONFIG_SHUNT_3SHUNT
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_ccr0_2          =    (void (*)(void *)) &empty_function;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_ccr0_2          =    (void (*)(void *)) &empty_function;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    //////////////////////////////////////////////
    // MODE_1SHUNT_EXTENSION
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_base_2          =    (void (*)(void *)) &empty_function;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_base_2          =    (void (*)(void *)) &empty_function;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_base_2          =    (void (*)(void *)) &empty_function;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_base_2          =    (void (*)(void *)) &empty_function;
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC_TIMER_55XX
    //////////////////////////////////////////////
    #if (PWM_TIMER == TIMER_A)
        motor_ptr->fptr.fptr_int_TA_base            =    (void (*)(void *)) &empty_function;
    #elif (PWM_TIMER == TIMER_B)
        motor_ptr->fptr.fptr_int_TB_base            =    (void (*)(void *)) &empty_function;
    #endif

#endif
}
