
/**
 * @file hall_funcs.h
 * @brief Use for hall FOC utility functions
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

#ifndef HALL_FUNCS_H
#define HALL_FUNCS_H

#include "pac5xxx.h"
#include "config_app.h"
#include "config_features.h"
#include "driver_funcs.h"
#include "fast_div.h"
#include "fault.h"
#include "motor_struct.h"
#include "utils.h"
#include "mpos_misc.h"
#include "param_funcs.h"
#include "hall_io_config.h"
#include "hall_timer_config.h"

#if defined(ENABLE_SIM)
#include "sim_h_funcs_ext.h"
#endif

extern int Debug_Test_Number1;

#if (MODULE_MPOS_HALL == ENABLED)

// Configuration
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
#define HALL2EST_ANGLEBLEND_TOTALCNT        128
#define HALL2EST_ANGLEBLEND_FACTOR_Q12      QCONST((1.0 / (float)HALL2EST_ANGLEBLEND_TOTALCNT), Q12)
#endif

// Check that hall filter depth is within allowed range
#if (CONFIG_HALLFILTER_SHIFT != 0) && (CONFIG_HALLFILTER_SHIFT != 1) && (CONFIG_HALLFILTER_SHIFT != 2) && (CONFIG_HALLFILTER_SHIFT != 3) && (CONFIG_HALLFILTER_SHIFT != 4) && (CONFIG_HALLFILTER_SHIFT != 5)
    #error: CONFIG_HALLFILTER_SHIFT must be a value between 0 and 5.
#endif

//========================================
// CONFIG: Clock Frequency
//========================================
#if defined(FAMILY_52XX)
    #define HALL_CLK_DIVIDER    (1 << TxCTL_PS_DIV128)
    #define HCLK_FREQ_HZ_HALL   HCLK_FREQ_HZ
#elif defined(FAMILY_55XX)
    #define HALL_CLK_DIVIDER     (1 << 8)
#ifndef HALL_TIMER_DIV1
    #define HCLK_FREQ_HZ_HALL   HCLK_FREQ_HZ/2
#else
    #define HCLK_FREQ_HZ_HALL   HCLK_FREQ_HZ
#endif
#else
    #error No PAC family defined
#endif

// THs = timer_ticks_counts * Tt
// Tt = 1/Ft (timer clock use for hall dedicated timer resource
// Ft = Timer Clock/Timer Divider = 50MHz/128
// Tt = 1/Ft = 1/(50MH/128) = 128/50MHz
// Time_for_one_electrical = Te = 6 * timer_ticks * Tt
// Te = (6*128*timer_ticks)/50MHz
// Convert into Frequency = Fe (Hz) = 1/Te = (1/(6*128*timer_ticks)/50MHz = 50Hz/(6*128*timer_ticks)
// Convert Electrical Freq in RAD/Sec = Fe(Rad/Sec) = 2Pi * 50MHz/(6*128*time_ticks)
// As we know 2*Pi, 50MHz and 6*128 is all constant, we can replace it with C
// C = 2*Pi*50MHz/(6*128)
#define HALL_SPEED_CAL_VALUE_Q0     QCONST(((1.0f * HCLK_FREQ_HZ_HALL) / (1.0f * HALL_CLK_DIVIDER)), 0)
#define HALL_TIMER_TICK_DT_Q26      QCONST(((1.0f * HALL_CLK_DIVIDER) / (1.0f * HCLK_FREQ_HZ_HALL)), Q26)


// Function Declarations
PAC5XXX_RAMFUNC extern void hall_process(Struct_Motor* motor_ptr);
extern void hall_init(Struct_Motor* motor_ptr);
extern void hall_reset(Struct_Motor* motor_ptr);
extern void hall_timer_isr(void);
void hall_sort_create_next_state_array(int32_t* array_ptr_src, char* array_ptr_dest);
void hall_sort_create_prev_state_array(int32_t* array_ptr_src, char* array_ptr_dest);
extern void hall_create_sequence_array(Struct_Motor* motor_ptr);
extern void hall_create_hallstate_angle(Struct_Motor* motor_ptr);
extern void hall_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
extern void hall_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);


/**
 * @brief Use to check hall state change and validity
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_int_hallport_enable(void)
{
    // Clear hall interrupt port flags
    pac5xxx_gpio_int_flag_clear_hallport(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
    //Enable hall port and NVIC interrupts
    pac5xxx_gpio_int_enable_hallport(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
    // Clear hall interrupt port flags
    pac5xxx_gpio_int_flag_clear_hallport(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_int_hallport_disable(void)
{
    //Disable hall port and NVIC interrupts
    pac5xxx_gpio_int_disable_hallport(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
    // Clear hall interrupt port flags
    pac5xxx_gpio_int_flag_clear_hallport(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_check_hall_state(Struct_Motor* motor_ptr)
{
    //=====================================================================================
    /// If read hall value is illegal,then stop motor
    //=====================================================================================
    if((motor_ptr->mpos.hall.SensorStateNew == 0)||(motor_ptr->mpos.hall.SensorStateNew == 7))
    {
        set_fault(FAULT_MOTOR_HALL_INVSTATE);

        // Diagnostics
        motor_ptr->mpos.hall.hall_check_error_code = 1;
        motor_ptr->mpos.hall.hall_check_error_state_new = motor_ptr->mpos.hall.SensorStateNew;
        motor_ptr->mpos.hall.hall_check_error_state_prev = motor_ptr->mpos.hall.SensorStateLast;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to check hall state sequence valid or invalid
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_check_sequence(Struct_Motor* motor_ptr)
{
    //int32_t hall_motor_direction_prev = motor_ptr->mpos.hall.hall_motor_direction;

    //=====================================================================================
    /// If hall state sequence is invalid,then stop motor
    //=====================================================================================
    // Check hall current state with hall next state table using last state as index to make sure this is correct state (checked in both directions)
    if ((motor_ptr->mpos.hall.hall_ptr_next_state[motor_ptr->mpos.hall.SensorStateLast] != motor_ptr->mpos.hall.SensorStateNew)
            && (motor_ptr->mpos.hall.hall_ptr_prev_state[motor_ptr->mpos.hall.SensorStateLast] != motor_ptr->mpos.hall.SensorStateNew))
    {
        set_fault(FAULT_MOTOR_HALL_INVSTATE);

        // Diagnostics
        motor_ptr->mpos.hall.hall_check_error_code = 2;
        motor_ptr->mpos.hall.hall_check_error_state_new = motor_ptr->mpos.hall.SensorStateNew;
        motor_ptr->mpos.hall.hall_check_error_state_prev = motor_ptr->mpos.hall.SensorStateLast;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_determine_direction(Struct_Motor* motor_ptr)
{
    int32_t hall_motor_direction_prev = motor_ptr->mpos.hall.hall_motor_direction;

    // Determine direction
    if (motor_ptr->mpos.hall.SensorStateNew == motor_ptr->mpos.hall.hall_forward_next_state[motor_ptr->mpos.hall.SensorStateLast])
    {
        motor_ptr->mpos.hall.hall_motor_direction = 1;
    }
    else if (motor_ptr->mpos.hall.SensorStateNew == motor_ptr->mpos.hall.hall_forward_prev_state[motor_ptr->mpos.hall.SensorStateLast])
    {
        motor_ptr->mpos.hall.hall_motor_direction = -1;
    }

    // Handle direction change
    #if 0               // The revese offset table has been removed from v4.4.0
    if (motor_ptr->mpos.hall.hall_motor_direction != hall_motor_direction_prev)
    {
        if (motor_ptr->mpos.hall.hall_motor_direction == -1)
        {
            motor_ptr->mpos.hall.ptr_hallstate_angle_dir = &motor_ptr->mpos.hall.hallstate_angle_rev[0];
            motor_ptr->mpos.hall.ptr_angcorr_gain = &motor_ptr->mpos.hall.angcorr_gain_rev_q20;
            motor_ptr->mpos.hall.ptr_angcorr_offset = &motor_ptr->mpos.hall.angcorr_offset_rev;
        }
        else
        {
            motor_ptr->mpos.hall.ptr_hallstate_angle_dir = &motor_ptr->mpos.hall.hallstate_angle_fwd[0];
            motor_ptr->mpos.hall.ptr_angcorr_gain = &motor_ptr->mpos.hall.angcorr_gain_fwd_q20;
            motor_ptr->mpos.hall.ptr_angcorr_offset = &motor_ptr->mpos.hall.angcorr_offset_fwd;
        }
    }
    #endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to check hall state change
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_check_hall_state_changed(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
    if(motor_ptr->mpos.hall.SensorStateGet_flag)
#endif
    {
        motor_ptr->mpos.hall.SensorStateNew = GET_HALL_VALUE;
			Debug_Test_Number1 = GET_HALL_VALUE;

        if (motor_ptr->mpos.hall.SensorStateLast != motor_ptr->mpos.hall.SensorStateNew)
        {
            #if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
            motor_ptr->mpos.hall.SensorStateGet_flag = FALSE;
            #endif
            // calculate the Hall count
#if (HALL_METHOD_SELECT != HALL_METHOD_INTERRUPT)
            motor_ptr->mpos.hall.base_speed = hall_get_counter_value();
            hall_timer_clear();
#endif
            motor_ptr->mpos.hall.transition_flag = TRUE;
        }
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to calculate hall motor angle
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_calc_motor_angle(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_HALL_ANGLE_DELAY_COMPENSATION) && (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)
    int32_t delay_total_q26;
    int32_t angle_delta_q16;

    // Check edge polarity and set flag for skew compensation on rising edge
    if (motor_ptr->mpos.hall.SensorStateNew > motor_ptr->mpos.hall.SensorStateLast)
        motor_ptr->mpos.hall.flag_halledge_comp = 1;
    else
        motor_ptr->mpos.hall.flag_halledge_comp = 0;

    // Angle delay compensation
    // delay_total = Tctl + (ticksD * TtickD)
    delay_total_q26 = motor_ptr->control.dt_control_q26 + (hall_get_counter_value() * HALL_TIMER_TICK_DT_Q26);  // q0 * q26 = q26

    // Hall edge polarity skew compensation
    delay_total_q26 += (motor_ptr->mpos.hall.flag_halledge_comp * motor_ptr->mpos.hall.halledge_polskew_time_q26);

    // delta_angle(rad) = speed(rad/s) * delay_total(s)
    angle_delta_q16 = ((motor_ptr->mpos.hall.hall_spd_radps >> 16) * delay_total_q26) >> 10;        // (q16>>16=q0 * q26 = q26>>10 = q16

    // Read hall angle from commutation table and add delay compensation
    motor_ptr->mpos.hall.angle_fix16 = motor_ptr->mpos.hall.ptr_hallstate_angle_dir[motor_ptr->mpos.hall.SensorStateNew] + angle_delta_q16 + motor_ptr->mpos.hall.hall_sensor_offset_rad;

    // Wraparound hall angle
    wraparound(&motor_ptr->mpos.hall.angle_fix16);

#elif defined(ENABLE_HALL_ANGLE_DELAY_COMPENSATION) && (HALL_METHOD_SELECT == HALL_METHOD_POLLING)
    int32_t delay_total_q26;
    int32_t angle_delta_q16;

    // Angle delay compensation
    // delay_total = Tctl + (ticksD * TtickD)
    delay_total_q26 = motor_ptr->control.dt_control_q26;

    // delta_angle(rad) = speed(rad/s) * delay_total(s)
    angle_delta_q16 = ((motor_ptr->mpos.hall.hall_spd_radps >> 16) * delay_total_q26) >> 10;        // (q16>>16=q0 * q26 = q26>>10 = q16

    // Read hall angle from commutation table and add delay compensation
    motor_ptr->mpos.hall.angle_fix16 = motor_ptr->mpos.hall.ptr_hallstate_angle_dir[motor_ptr->mpos.hall.SensorStateNew] + angle_delta_q16;

    // Wraparound
    wraparound(&motor_ptr->mpos.hall.angle_fix16);

#else
    // Read hall angle from commutation table
    motor_ptr->mpos.hall.angle_fix16 = motor_ptr->mpos.hall.ptr_hallstate_angle_dir[motor_ptr->mpos.hall.SensorStateNew];
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to calculate hall motor speed
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_calc_motor_speed(Struct_Motor* motor_ptr)
{
    int32_t temp;

    // Speed = pi/3 / ((base_speed * dt) +/- edge_pol_comp_time)
    __disable_irq();
    temp = motor_ptr->mpos.hall.base_speed;                                     // q0 * q26 = q26
    __enable_irq();

    // Calculate hall edge interval time
    temp += (temp * HALL_TIMER_TICK_DT_Q26);                                    // q0 * q26 = q26

    // Hall edge polarity skew compensation
    if (motor_ptr->mpos.hall.flag_halledge_comp)
        temp -= motor_ptr->mpos.hall.halledge_polskew_time_q26;                 //q26
    else
        temp += motor_ptr->mpos.hall.halledge_polskew_time_q26;                 //q26

    motor_ptr->mpos.hall.sector_time_sec_q26 = temp;

    temp = div_fast(QCONST(1.0f, Q10), temp);                                  // div16(q10/q26 = q0
    motor_ptr->mpos.hall.hall_spd_prefilt_radps_q16 = (temp * FIX16_PI_DIV_3);  // q16 = q0 * q16

    motor_ptr->mpos.hall.hall_spd_prefilt_radps_q16 = motor_ptr->mpos.hall.hall_spd_prefilt_radps_q16 * motor_ptr->mpos.hall.hall_motor_direction;

    // Initialize the speed filter using the first speed calculation in order to help it converge faster
    if (motor_ptr->mpos.hall.flag_inithallspdfilt)
    {
        motor_ptr->mpos.hall.flag_inithallspdfilt = 0;
        motor_ptr->mpos.hall.hall_spd_radps = motor_ptr->mpos.hall.hall_spd_prefilt_radps_q16;
    }

    // Filter hall speed
    filter_shift2(motor_ptr->mpos.hall.hall_spd_prefilt_radps_q16, &motor_ptr->mpos.hall.hall_spd_radps, CONFIG_HALLFILTER_SHIFT);

    // Calculate interpolation increment
    motor_ptr->mpos.hall.accumulate_degree_per_dt = ((motor_ptr->mpos.hall.hall_spd_radps >> 12) * (motor_ptr->control.dt_control_q26)) >> 14;  // q4 * q26 = q30 >> 14 = q16

    // Calculate speed variants
    motor_ptr->mpos.hall.hall_spd_scaled_q14 = motor_ptr->mpos.hall.hall_spd_radps >> (motor_ptr->app.speed_scale_shift + 2);
    motor_ptr->mpos.hall.hall_spd_scaled_q28 = motor_ptr->mpos.hall.hall_spd_scaled_q14 << 14;

#if 0
//    debug_dac_2c = motor_ptr->mpos.hall.hall_spd_prefilt_radps_q16 >> (motor_ptr->app.speed_scale_shift + 2);     // q16 = mul(q16 * q16)
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to calculate hall interpolate angle
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_angle_interpolate(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_HALL_ANGLE_INTERPOLATION)
    // Set interpolation limit to +/-72 degrees
    int32_t acummulate_limit = FIX16_2PI_DIV_5 - motor_ptr->mpos.hall.accumulate_degree_per_dt;

    // Increment interpolation
    motor_ptr->mpos.hall.accumulate_degree_value = motor_ptr->mpos.hall.accumulate_degree_per_dt + motor_ptr->mpos.hall.accumulate_degree_value;

    // Cap interpolation
    if (motor_ptr->mpos.hall.accumulate_degree_value > acummulate_limit)
    {
        motor_ptr->mpos.hall.accumulate_degree_value = acummulate_limit;
    }
    else if (motor_ptr->mpos.hall.accumulate_degree_value < -acummulate_limit)
    {
        motor_ptr->mpos.hall.accumulate_degree_value = -acummulate_limit;
    }

    // Add interpolation to hall angle
    motor_ptr->mpos.hall.hall_interpolated_angle = motor_ptr->mpos.hall.hall_angle + motor_ptr->mpos.hall.accumulate_degree_value;
    wraparound(&motor_ptr->mpos.hall.hall_interpolated_angle);
#else
    motor_ptr->mpos.hall.hall_interpolated_angle = motor_ptr->mpos.hall.hall_angle;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#if (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)
/**
 * @brief Use to call hall interrupt handler to process interrupt and speed calculation
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_isr_handler(void)
{
    static Struct_Motor* motor_ptr = &motor;
    int32_t hall_int_flag;

#if defined(FAMILY_52XX)
    // Read hall port interrupt flag
    hall_int_flag = PAC5XXX_GPIO_INT_FLAG & (NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
#elif defined(FAMILY_55XX)
    // Read hall port interrupt flag
    hall_int_flag = (PAC5XXX_GPIO_HALL->INTFLAG.w & (NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK));
    #if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
    hall_int_flag &= motor_ptr->mpos.hall.hall_int_set;
    #endif
#endif

    if (hall_int_flag)
    {
        // Store hall timer counts for speed calculation
        motor_ptr->mpos.hall.base_speed = hall_get_counter_value();

        // Clear and restart the hall timer
        hall_timer_clear();

        // Indicate to hall process function that interrupt has occurred
        motor_ptr->mpos.hall.flag_hall_int = TRUE;
        #if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
        motor_ptr->mpos.hall.SensorStateGet_flag = TRUE;
        #endif

#if defined(ENABLE_SIM)
        sim_hallport_intflag_set(motor_ptr);
#endif

#if defined(FAMILY_52XX)
        // Configure the hall interrupt edge polarity depending on hall edge detected
        pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLA_PORT_BIT, PAC5XXX_GPIO_HALLA->IN.b & HALLA_PORT_BIT);
        pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLB_PORT_BIT, PAC5XXX_GPIO_HALLB->IN.b & HALLB_PORT_BIT);
        pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLC_PORT_BIT, PAC5XXX_GPIO_HALLC->IN.b & HALLC_PORT_BIT);
#endif

#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION) && (DEVICE_SELECT == DEVICE_5532)
        // Configure the hall interrupt edge polarity depending on hall edge detected
        pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLA_PORT_BIT, PAC5XXX_GPIO_HALLA->IN.w & HALLA_PORT_BIT);
        pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLB_PORT_BIT, PAC5XXX_GPIO_HALLB->IN.w & HALLB_PORT_BIT);
        pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLC_PORT_BIT, PAC5XXX_GPIO_HALLC->IN.w & HALLC_PORT_BIT);

        switch(hall_int_flag)
        {
            case NIRQA_PIN_MASK:
                motor_ptr->mpos.hall.hall_int_set = (HALLB_PORT_BIT | HALLC_PORT_BIT);
                break;

            case NIRQB_PIN_MASK:
                motor_ptr->mpos.hall.hall_int_set = (HALLA_PORT_BIT | HALLC_PORT_BIT);  
                break;

            case NIRQC_PIN_MASK:
                motor_ptr->mpos.hall.hall_int_set = (HALLA_PORT_BIT | HALLB_PORT_BIT);
                break;

            default:
                motor_ptr->mpos.hall.hall_int_set = (HALLA_PORT_BIT | HALLB_PORT_BIT | HALLC_PORT_BIT);
                break;
        }
#endif
        hall_int_hallport_disable();
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif  //#if (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)

#if defined(ENABLE_ANGLE_CORRECTION) || defined(ENABLE_MTPA)
/**
 * @brief Applies angle correction to the hall angle
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void hall_angle_correction(Struct_Motor* motor_ptr)
{
        // Angle Correction (speed based linear)
        mpos_angle_correction(motor_ptr, motor_ptr->mpos.hall.hall_spd_radps, motor_ptr->mpos.hall.hall_motor_direction, *motor_ptr->mpos.hall.ptr_angcorr_gain, *motor_ptr->mpos.hall.ptr_angcorr_offset, motor_ptr->mpos.hall.angle_fix16, &motor_ptr->mpos.hall.hall_angle);

        motor_ptr->mpos.hall.hall_interpolated_angle = motor_ptr->mpos.hall.hall_angle;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

#endif  //#if (MODULE_MPOS_HALL == ENABLED)

#endif  //#ifndef HALL_FUNCS_H
