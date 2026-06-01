
/**
 * @file trap_funcs.h
 * @brief Use for trap FOC utility functions
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

#ifndef TRAP_FUNCS_H
#define TRAP_FUNCS_H

#include "pac5xxx.h"
#include "config_app.h"
#include "config_features.h"
#include "motor_struct.h"
#include "fast_div.h"
#include "utils.h"

#ifdef ENABLE_BEMF_SAMPLE_SINGLE_PHASE
#include "pac5xxx_driver_adc.h"
#endif

#if (MODULE_MPOS_TRAP == ENABLED)

// Constant Definitions
#define FILTER_COUNT_MAX            6


// Function Declarations
PAC5XXX_RAMFUNC extern void mpos_process_trap(Struct_Motor* motor_ptr);
extern void trap_init(Struct_Motor* motor_ptr);
extern void trap_reset(Struct_Motor* motor_ptr);
extern void trap_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
extern void trap_stall_detection(Struct_Motor* motor_ptr);
extern void trap_calc_sector_from_angle(fix16_t angle_q16, Struct_Motor* motor_ptr);

/**
 * @brief Use to determine if BEMF signal is changed or not
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void trap_check_state_change(Struct_Motor* motor_ptr)
{
    if (motor_ptr->mpos.trap.BEMFStateLast != motor_ptr->mpos.trap.BEMFStateNew)
    {
        motor_ptr->mpos.trap.transition_flag = TRUE;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to switch from Aligngo to Trap mode
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void trap_check_state_change_startup(Struct_Motor* motor_ptr)
{
    if (motor_ptr->mpos.trap.BEMFStateLast_startup != motor_ptr->mpos.trap.BEMFStateNew_startup)
    {
        motor_ptr->mpos.trap.transition_flag_startup = TRUE;
    }
    if (motor_ptr->mpos.trap.transition_flag_startup == TRUE)
    {
        // Check the next bemf state is matching with the target
        if ((motor_ptr->mpos.trap.BEMFStateLast_startup == motor_ptr->mpos.trap.BEMFStateNew)
            && (motor_ptr->mpos.trap.BEMFStateNew_startup == motor_ptr->mpos.trap.trap_forward_next_state[motor_ptr->mpos.trap.BEMFStateNew]))
        {
            motor_ptr->mpos.anglediff_pass_flag = 1;
        }
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief Use to calculate angle from table
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void trap_calc_motor_angle(Struct_Motor* motor_ptr)
{
    // Read hall angle from commutation table
    motor_ptr->mpos.trap.angle_fix16 = motor_ptr->mpos.trap.ptr_trapstate_angle_dir[motor_ptr->mpos.trap.BEMFStateNew];
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to calculate TRAP BEMF motor speed
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void trap_calc_motor_speed(Struct_Motor* motor_ptr)
{
    int32_t temp;

    // speed_rad/s = (pi/3 / sector_time), sector_time = (#controlcycles * dt)
    motor_ptr->mpos.trap.sector_time_sec_q26 = motor_ptr->mpos.trap.trap_speed_control_storecounter * motor_ptr->control.dt_control_q26;
    temp = div_fast(QCONST(1.0f, Q10), motor_ptr->mpos.trap.sector_time_sec_q26);
    motor_ptr->mpos.trap.trap_spd_prefilt_radps_q16 = (temp * FIX16_PI_DIV_3);  // q16 = q0 * q16

//    motor_ptr->mpos.trap.trap_spd_prefilt_radps_q16 = motor_ptr->mpos.trap.trap_spd_prefilt_radps_q16 * motor_ptr->mpos.trap.trap_motor_direction;

    // Initialize the speed filter using the first speed calculation in order to help it converge faster
    if (motor_ptr->mpos.trap.flag_inittrapspdfilt)
    {
        motor_ptr->mpos.trap.flag_inittrapspdfilt = 0;
        motor_ptr->mpos.trap.trap_spd_radps = motor_ptr->mpos.trap.trap_spd_prefilt_radps_q16;
    }

//    motor_ptr->mpos.trap.trap_speed_control_counter = 0;


    // Filter trap speed
    filter_shift2(motor_ptr->mpos.trap.trap_spd_prefilt_radps_q16, &motor_ptr->mpos.trap.trap_spd_radps, CONFIG_TRAPFILTER_SHIFT);

    // Calculate interpolation increment
    motor_ptr->mpos.trap.accumulate_degree_per_dt = ((motor_ptr->mpos.trap.trap_spd_radps >> 12) * (motor_ptr->control.dt_control_q26)) >> 14;  // q4 * q26 = q30 >> 14 = q16

    // Calculate speed variants
    motor_ptr->mpos.trap.trap_spd_scaled_q14 = motor_ptr->mpos.trap.trap_spd_radps >> (motor_ptr->app.speed_scale_shift + 2);
    motor_ptr->mpos.trap.trap_spd_scaled_q28 = motor_ptr->mpos.trap.trap_spd_scaled_q14 << 14;

#if 0
//    debug_dac_2c = motor_ptr->mpos.trap.trap_spd_prefilt_radps_q16 >> (motor_ptr->app.speed_scale_shift + 2);     // q16 = mul(q16 * q16)
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to calculate TRAP BEMF interpolate angle
 * @param Struct_Motor* motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void trap_angle_interpolate(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_TRAP_ANGLE_INTERPOLATION)
    // Set interpolation limit to +/-72 degrees
    int32_t acummulate_limit = FIX16_2PI_DIV_5 - motor_ptr->mpos.trap.accumulate_degree_per_dt;

    // Increment interpolation
    motor_ptr->mpos.trap.accumulate_degree_value = motor_ptr->mpos.trap.accumulate_degree_per_dt + motor_ptr->mpos.trap.accumulate_degree_value;

    // Cap interpolation
    if (motor_ptr->mpos.trap.accumulate_degree_value > acummulate_limit)
    {
        motor_ptr->mpos.trap.accumulate_degree_value = acummulate_limit;
    }
    else if (motor_ptr->mpos.trap.accumulate_degree_value < -acummulate_limit)
    {
        motor_ptr->mpos.trap.accumulate_degree_value = -acummulate_limit;
    }

    // Add interpolation to Trap angle
    motor_ptr->mpos.trap.trap_interpolated_angle = motor_ptr->mpos.trap.trap_angle + motor_ptr->mpos.trap.accumulate_degree_value;
    wraparound(&motor_ptr->mpos.trap.trap_interpolated_angle);
#else
    motor_ptr->mpos.trap.trap_interpolated_angle = motor_ptr->mpos.trap.trap_angle;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#if 0
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void trap_angle_emulator(Struct_Motor* motor_ptr)
{
    if (motor_ptr->mpos.hall.hall_angle >= (FIX16_PI_DIV_3 * 3))        // >= 180
        motor_ptr->mpos.trap.BEMFStateNew = 5;
    else if (motor_ptr->mpos.hall.hall_angle >= (FIX16_PI_DIV_3 * 2))   // >= 120
        motor_ptr->mpos.trap.BEMFStateNew = 4;
    else if (motor_ptr->mpos.hall.hall_angle >= FIX16_PI_DIV_3)         // >= 60
        motor_ptr->mpos.trap.BEMFStateNew = 6;
    else if (motor_ptr->mpos.hall.hall_angle >= 0)                      // >= 0
        motor_ptr->mpos.trap.BEMFStateNew = 2;
    else if (motor_ptr->mpos.hall.hall_angle >= -FIX16_PI_DIV_3)        // >= -60
        motor_ptr->mpos.trap.BEMFStateNew = 3;
    else if (motor_ptr->mpos.hall.hall_angle >= (-FIX16_PI_DIV_3 * 2))  // >= -120
        motor_ptr->mpos.trap.BEMFStateNew = 1;
    else if (motor_ptr->mpos.hall.hall_angle >= (-FIX16_PI_DIV_3 * 3))  // >= -180
        motor_ptr->mpos.trap.BEMFStateNew = 5;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif


PAC5XXX_OPTIMIZE_SPEED_ON
static inline void trap_bemf_majority_filter(int32_t phase_logic_raw, int32_t* filter_cnt, int32_t* phase_logic_filt)
{
    if (phase_logic_raw == 1)
    {
        if ((*filter_cnt) < FILTER_COUNT_MAX)
            (*filter_cnt)++;
        else
            *phase_logic_filt = 1;
    }
    else
    {
        if ((*filter_cnt) > 0)
            (*filter_cnt)--;
        else
            *phase_logic_filt = 0;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#ifdef ENABLE_BEMF_SAMPLE_SINGLE_PHASE
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void trap_process_bemf_sensing(int32_t bemf_x_cnt_q0, int32_t conv_adc2volts_scaled_q16)
{
    Struct_Motor* motor_ptr = &motor;
    int32_t bemf_centertap_q16;// bemf_high_side_threshold_q16;

    // Read and scale the bemf voltage
    motor_ptr->mpos.trap.bemf_x_scaled_q16 = bemf_x_cnt_q0 * conv_adc2volts_scaled_q16;

    // Central tap voltage - Mid rail
    bemf_centertap_q16 = QCONST(0.5f, Q16);

    // Phase X zero crossing detection
    if (motor_ptr->mpos.trap.bemf_x_scaled_q16 > bemf_centertap_q16)
        motor_ptr->mpos.trap.phase_x_logic_raw = 1;
    else
        motor_ptr->mpos.trap.phase_x_logic_raw = 0;

    // Majority filter
    trap_bemf_majority_filter(motor_ptr->mpos.trap.phase_x_logic_raw, &motor_ptr->mpos.trap.phase_x_filter_cnt, &motor_ptr->mpos.trap.phase_x_logic_filt);

    switch (motor_ptr->control.mod_sector)
    {
    case 1:
        // Float V, LSON W, PWM U
        if (motor_ptr->mpos.trap.phase_x_logic_filt == 1)
            motor_ptr->mpos.trap.phase_v_logic_filt = motor_ptr->mpos.trap.phase_x_logic_filt;
        break;
    case 2:
        // Float U, LSON W, PWM V
        if (motor_ptr->mpos.trap.phase_x_logic_filt == 0)
            motor_ptr->mpos.trap.phase_u_logic_filt = motor_ptr->mpos.trap.phase_x_logic_filt;
        break;
    case 3:
        // Float W, LSON U, PWM V
        if (motor_ptr->mpos.trap.phase_x_logic_filt == 1)
            motor_ptr->mpos.trap.phase_w_logic_filt = motor_ptr->mpos.trap.phase_x_logic_filt;
        break;
    case 4:
        // Float V, LSON U, PWM W
        if (motor_ptr->mpos.trap.phase_x_logic_filt == 0)
            motor_ptr->mpos.trap.phase_v_logic_filt = motor_ptr->mpos.trap.phase_x_logic_filt;
        break;
    case 5:
        // Float U, LSON V, PWM W
        if (motor_ptr->mpos.trap.phase_x_logic_filt == 1)
            motor_ptr->mpos.trap.phase_u_logic_filt = motor_ptr->mpos.trap.phase_x_logic_filt;
        break;
    case 6:
        // Float W, LSON V, PWM U
        if (motor_ptr->mpos.trap.phase_x_logic_filt == 0)
            motor_ptr->mpos.trap.phase_w_logic_filt = motor_ptr->mpos.trap.phase_x_logic_filt;
        break;
    default:
        break;
    }


    // This needs to be set after bemf zero crossing is detected
    if ((motor_ptr->mpos.anglediff_pass_flag == 1) && (motor_ptr->mpos.mposmode == STATE_MPOSMODE_TRAP) && (motor_ptr->mpos.trap.est2trap_delay_cnt == 0))
    //if (motor_ptr->mpos.aligngo.state_main == STATE_ALIGNGO_MAIN_RAMPCOMPLETE)
    {
        // Mapping the sector: WVU format
        //motor_ptr->mpos.trap.BEMFStateNew = (motor_ptr->mpos.trap.phase_w_logic_raw << 2) + (motor_ptr->mpos.trap.phase_v_logic_raw << 1) + (motor_ptr->mpos.trap.phase_u_logic_raw << 0);
        motor_ptr->mpos.trap.BEMFStateNew = (motor_ptr->mpos.trap.phase_w_logic_filt << 2) + (motor_ptr->mpos.trap.phase_v_logic_filt << 1) + (motor_ptr->mpos.trap.phase_u_logic_filt << 0);
        if (motor_ptr->mpos.trap.BEMFStateNew > 6) motor_ptr->mpos.trap.BEMFStateNew = 6;
        if (motor_ptr->mpos.trap.BEMFStateNew < 1) motor_ptr->mpos.trap.BEMFStateNew = 1;
    }

    // This is for switching from Est to Trap, may need to add for reverse direction
    if ((motor_ptr->mpos.mposmode == STATE_MPOSMODE_TRAP2EST) || (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST) || (motor_ptr->mpos.mposmode == STATE_MPOSMODE_EST2TRAP) || (motor_ptr->mpos.trap.est2trap_delay_cnt > 0))
    {
        trap_calc_sector_from_angle(motor_ptr->mpos.est.estimated_angle, motor_ptr);
    }

    // Check if the startup is successful
    motor_ptr->mpos.trap.BEMFStateLast_startup = motor_ptr->mpos.trap.BEMFStateNew_startup;
    motor_ptr->mpos.trap.BEMFStateNew_startup = (motor_ptr->mpos.trap.phase_w_logic_filt << 2) + (motor_ptr->mpos.trap.phase_v_logic_filt << 1) + (motor_ptr->mpos.trap.phase_u_logic_filt << 0);
    // To use cnt (2+) as threhold instead of speed
    if (motor_ptr->mpos.aligngo.state_main == STATE_ALIGNGO_MAIN_RAMPCOMPLETE)
    //if (motor_ptr->mpos.aligngo.speedrefol_ramp_q26 > (motor_ptr->mpos.aligngo.speedrefol_target_q26 >> 1))
    {
        trap_check_state_change_startup(motor_ptr);
    }
    else
    {
        motor_ptr->mpos.anglediff_pass_flag = 0;
    }

#if 0
    debug_dac_1a = motor_ptr->mpos.trap.bemf_x_scaled_q16 >> 2;
    debug_dac_1b = (motor_ptr->mpos.trap.phase_x_logic_raw) << 14;
    debug_dac_1c = (motor_ptr->mpos.trap.phase_x_logic_filt) << 14;
    debug_dac_2a = (motor_ptr->mpos.trap.phase_u_logic_filt) << 14;
    debug_dac_2b = (motor_ptr->mpos.trap.phase_v_logic_filt) << 14;
    debug_dac_2c = (motor_ptr->mpos.trap.phase_w_logic_filt) << 14;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
static inline void trap_config_seq(int32_t active_sector)
{
    static int32_t sector_prev = 0;


    if (active_sector != sector_prev)
    {
        switch (active_sector)
        {
        case 1:     // Float V, LSON W, PWM U
        case 4:     // Float V, LSON U, PWM W
            pac5xxx_dtse_seq_config(10, ADC0, EMUX_AB8, 0,           0);       // Dummy Conversion, then change CAFE MUX to M1-Vw
            pac5xxx_dtse_seq_config(11, ADC0, EMUX_AB8, 0,     SEQ_END);       // Convert M1-Vw, then change CAFE MUX to M1-Vu, generate ADC interrupt
            break;

        case 2:     // Float U, LSON W, PWM V
        case 5:     // Float U, LSON V, PWM W
            pac5xxx_dtse_seq_config(10, ADC0, EMUX_AB7, 0,           0);       // Dummy Conversion, then change CAFE MUX to M1-Vw
            pac5xxx_dtse_seq_config(11, ADC0, EMUX_AB7, 0,     SEQ_END);       // Convert M1-Vw, then change CAFE MUX to M1-Vu, generate ADC interrupt
            break;

        case 3:     // Float W, LSON U, PWM V
        case 6:     // Float W, LSON V, PWM U
            pac5xxx_dtse_seq_config(10, ADC0, EMUX_AB9, 0,           0);       // Dummy Conversion, then change CAFE MUX to M1-Vw
            pac5xxx_dtse_seq_config(11, ADC0, EMUX_AB9, 0,     SEQ_END);       // Convert M1-Vw, then change CAFE MUX to M1-Vu, generate ADC interrupt
            break;

        default:
            break;
        }

        // Store the previous sector
        sector_prev = active_sector;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#else   //#ifdef ENABLE_BEMF_SAMPLE_SINGLE_PHASE, SAMPLE/HOLD on AIO789 is required to support this option
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void trap_process_bemf_sensing(int32_t bemf_u_cnt_q0, int32_t bemf_v_cnt_q0, int32_t bemf_w_cnt_q0, int32_t conv_adc2volts_scaled_q16)
{
    Struct_Motor* motor_ptr = &motor;
    int32_t bemf_centertap_q16;// bemf_high_side_threshold_q16;
    static int32_t sector_bemf_q0, sector_bemf_prev_q0, sector_bemf_corr_q0;

    // Read and scale the bemf voltage
    motor_ptr->mpos.trap.bemf_u_scaled_q16 = bemf_u_cnt_q0 * conv_adc2volts_scaled_q16;
    motor_ptr->mpos.trap.bemf_v_scaled_q16 = bemf_v_cnt_q0 * conv_adc2volts_scaled_q16;
    motor_ptr->mpos.trap.bemf_w_scaled_q16 = bemf_w_cnt_q0 * conv_adc2volts_scaled_q16;
    // Center tap voltage
//    bemf_centertap_q16 = ((motor_ptr->mpos.trap.bemf_u_scaled_q16 + motor_ptr->mpos.trap.bemf_v_scaled_q16 + motor_ptr->mpos.trap.bemf_w_scaled_q16) * QCONST(0.333f, Q10)) >> 10;        // Q16 * Q10 = Q26 -> 10 = Q16
    bemf_centertap_q16 = QCONST(0.5f, Q16);

    // Phase U zero crossing detection
    if (motor_ptr->mpos.trap.bemf_u_scaled_q16 > bemf_centertap_q16)
        motor_ptr->mpos.trap.phase_u_logic_raw = 1;
    else
        motor_ptr->mpos.trap.phase_u_logic_raw = 0;

    // Phase V zero crossing detection
    if (motor_ptr->mpos.trap.bemf_v_scaled_q16 > bemf_centertap_q16)
        motor_ptr->mpos.trap.phase_v_logic_raw = 1;
    else
        motor_ptr->mpos.trap.phase_v_logic_raw = 0;

    // Phase W zero crossing detection
    if (motor_ptr->mpos.trap.bemf_w_scaled_q16 > bemf_centertap_q16)
        motor_ptr->mpos.trap.phase_w_logic_raw = 1;
    else
        motor_ptr->mpos.trap.phase_w_logic_raw = 0;

    // Majority filter
    trap_bemf_majority_filter(motor_ptr->mpos.trap.phase_u_logic_raw, &motor_ptr->mpos.trap.phase_u_filter_cnt, &motor_ptr->mpos.trap.phase_u_logic_filt);
    trap_bemf_majority_filter(motor_ptr->mpos.trap.phase_v_logic_raw, &motor_ptr->mpos.trap.phase_v_filter_cnt, &motor_ptr->mpos.trap.phase_v_logic_filt);
    trap_bemf_majority_filter(motor_ptr->mpos.trap.phase_w_logic_raw, &motor_ptr->mpos.trap.phase_w_filter_cnt, &motor_ptr->mpos.trap.phase_w_logic_filt);

    // Mapping the sector: WVU format
    //motor_ptr->mpos.trap.BEMFStateNew = (motor_ptr->mpos.trap.phase_w_logic_raw << 2) + (motor_ptr->mpos.trap.phase_v_logic_raw << 1) + (motor_ptr->mpos.trap.phase_u_logic_raw << 0);
    motor_ptr->mpos.trap.BEMFStateNew = (motor_ptr->mpos.trap.phase_w_logic_filt << 2) + (motor_ptr->mpos.trap.phase_v_logic_filt << 1) + (motor_ptr->mpos.trap.phase_u_logic_filt << 0);

    // For test use only
#if 0
    switch (motor_ptr->mpos.trap.BEMFStateNew)
    {
        case 5:     //  101
            sector_bemf_q0 = 5;
            break;
        case 1:     //  001
            sector_bemf_q0 = 6;
            break;
        case 3:     //  011
            sector_bemf_q0 = 1;
            break;
        case 2:     //  010
            sector_bemf_q0 = 2;
            break;
        case 6:     //  110
            sector_bemf_q0 = 3;
            break;
        case 4:     //  100
            sector_bemf_q0 = 4;
            break;
        default:
            break;
    }

    // For forward rotating, sector would be increased
    // Edge happening
    if (sector_bemf_q0 != sector_bemf_prev_q0)
    {
        if (sector_bemf_q0 > sector_bemf_prev_q0)    sector_bemf_corr_q0 = sector_bemf_q0;
        if ((sector_bemf_q0 == 1) && (sector_bemf_prev_q0 >= 5))    sector_bemf_corr_q0 = sector_bemf_q0;
        if ((sector_bemf_q0 == 6) && (sector_bemf_prev_q0 == 1))    sector_bemf_corr_q0 = sector_bemf_prev_q0;
        if ((sector_bemf_q0 == 2) && (sector_bemf_prev_q0 >= 5))    sector_bemf_corr_q0 = sector_bemf_q0;
    }

    // store the previous sector
    sector_bemf_prev_q0 = sector_bemf_q0;
#endif

#if 1
    debug_dac_1a = (motor_ptr->mpos.trap.bemf_u_scaled_q16) >> 2;
    if (debug_input_6 == 0)
    {
        debug_dac_2a = (motor_ptr->mpos.trap.bemf_v_scaled_q16) >> 2;
        debug_dac_2b = (motor_ptr->mpos.trap.bemf_w_scaled_q16) >> 2;
        debug_dac_1c = bemf_centertap_q16 >> 2;
    }
    else if (debug_input_6 == 1)
    {
        debug_dac_2a = (motor_ptr->mpos.trap.phase_u_logic_raw) << 14;
        debug_dac_2b = (motor_ptr->mpos.trap.phase_u_logic_filt) << 14;
    }
//    debug_dac_2c = sector_bemf_q0 << 14;//(motor_ptr->mpos.trap.phase_w_logic_filt) << 14;
    debug_var_1 = motor_ptr->mpos.trap.phase_u_logic_filt;
    debug_var_2 = motor_ptr->mpos.trap.phase_u_filter_cnt;
    debug_var_3 = motor_ptr->mpos.trap.bemf_w_scaled_q16;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif  //#ifdef ENABLE_BEMF_SAMPLE_SINGLE_PHASE


PAC5XXX_OPTIMIZE_SPEED_ON
static inline void trap_current_mapping(int32_t mod_sector, int32_t Iu, int32_t Iv, int32_t Iw, int32_t* i_d, int32_t* i_q)
{

    // Set id = 0
    *i_d = 0;

    switch(mod_sector)
    {
        case 1:
        case 2:
            *i_q = -Iw;
            break;

        case 3:
        case 4:
            *i_q = -Iu;
            break;

        case 5:
        case 6:
            *i_q = -Iv;
            break;

        default:
            break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


#if defined(ENABLE_MTPA)
#error
#endif

#endif  //#if (MODULE_MPOS_TRAP == ENABLED)

#endif  //#ifndef TRAP_FUNCS_H
