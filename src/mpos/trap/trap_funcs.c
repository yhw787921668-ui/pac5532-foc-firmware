
/**
 * @file trap_funcs.c
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

#include "trap_funcs.h"
#include "adc_sample.h"


#if (MODULE_MPOS_TRAP == ENABLED)

#define NUM_SYSTICK_WAIT_TO_RESTART         10

/**
 * @brief This function use for trap processing
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC void mpos_process_trap(Struct_Motor* motor_ptr)
{
#if 0
    trap_angle_emulator(motor_ptr);
#else
#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
    #ifdef ENABLE_BEMF_SAMPLE_SINGLE_PHASE
    trap_process_bemf_sensing(readseq_vphase_x_trap(), motor_ptr->mpos.trap.bemf_conv_adc2volts_scaled_q16);
    #else
    trap_process_bemf_sensing(readseq_vphase_u_trap(), readseq_vphase_v_trap(), readseq_vphase_w_trap(), motor_ptr->mpos.trap.bemf_conv_adc2volts_scaled_q16);
    #endif
#endif
#endif

    motor_ptr->mpos.trap.trap_speed_control_counter++;

    // Check if TRAP BEMF transition has occurred after transition flag indicates that previous transition has been processed
    if (!motor_ptr->mpos.trap.transition_flag)
    {
        trap_check_state_change(motor_ptr);
    }

    // Process TRAP BEMF transition if state transition is detected and if TRAP BEMF interrupt has been detected
    if(motor_ptr->mpos.trap.transition_flag)
    {
        // Clear TRAP state transition flag
        motor_ptr->mpos.trap.transition_flag = FALSE;

        // Set external TRAP state transition flag
        motor_ptr->mpos.trap.transition_ext_flag = TRUE;

        // Fault if TRAP sequence is not valid
#if defined(ENABLE_TRAP_SEQUENCE_CHECK)
        trap_check_sequence(motor_ptr);
#endif
        // Fault if read trap value is illegal
//        trap_check_hall_state(motor_ptr);

        // Determine motor direction
//        trap_determine_direction(motor_ptr);

        // Calculate hall angle
        trap_calc_motor_angle(motor_ptr);

        // Reset interpolation angle and let speed calculator determine interpolation increment
        motor_ptr->mpos.trap.accumulate_degree_value = 0;

        // Skip speed calculation on the first transition because speed reading is not valid
        if (motor_ptr->mpos.trap.transition_counter >= 1)
        {
            motor_ptr->mpos.trap.flag_speed_calc_enabled = TRUE;
            motor_ptr->mpos.trap.do_speed_cal = TRUE;
            motor_ptr->mpos.trap.trap_speed_control_storecounter = motor_ptr->mpos.trap.trap_speed_control_counter;
            motor_ptr->mpos.trap.trap_speed_control_counter = 0;
        }
        // Increment internal hall transition counter
        motor_ptr->mpos.trap.transition_counter++;

//#if defined(ENABLE_ANGLE_CORRECTION)
//        trap_angle_correction(motor_ptr);
//#else
        motor_ptr->mpos.trap.trap_angle = motor_ptr->mpos.trap.angle_fix16;
//#endif

        motor_ptr->mpos.trap.trap_interpolated_angle = motor_ptr->mpos.trap.trap_angle;

        // Store the last state
        motor_ptr->mpos.trap.BEMFStateLast = motor_ptr->mpos.trap.BEMFStateNew;
    }
    else
    {
        // Calculate trap speed and interpolation increment
        if (motor_ptr->mpos.trap.do_speed_cal)
        {
            // Calculate speed only in the first cycle after trap state transition
            motor_ptr->mpos.trap.do_speed_cal = FALSE;

            // Calculate motor speed
            trap_calc_motor_speed(motor_ptr);
        }

        // Clear external trap state transition flag
        motor_ptr->mpos.trap.transition_ext_flag = FALSE;

        // Interpolate angle between trap state transitions
        trap_angle_interpolate(motor_ptr);
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use for aligngo restart in case of stall
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void trap_stall_detection(Struct_Motor* motor_ptr)
{
     // Only check when the MPOS is in Trap mode
     if (motor_ptr->mpos.state == STATE_MPOS_TRAP)
     {
        // Check if the shaft is rotating as desired
        if (motor_ptr->mpos.trap.BEMFStateStall_prev == motor_ptr->mpos.trap.BEMFStateNew)
        {
            motor_ptr->mpos.trap.startup_stall_cnt++;
        }
        else
        {
            if (motor_ptr->mpos.trap.startup_stall_cnt > 0) motor_ptr->mpos.trap.startup_stall_cnt--;
        }
        if (motor_ptr->mpos.trap.startup_stall_cnt >= NUM_SYSTICK_WAIT_TO_RESTART) 
        {
                __disable_irq();
                motor_ptr->startup.state = STATE_STARTUP_ALIGNGO_RESET;
                motor_ptr->state.main = STATE_MAIN_STARTUP;
                __enable_irq();
        }
        motor_ptr->mpos.trap.BEMFStateStall_prev = motor_ptr->mpos.trap.BEMFStateNew;
    }
}


/**
 * @brief This function use for calculating Trap sector from angle
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void trap_calc_sector_from_angle(fix16_t angle_q16, Struct_Motor* motor_ptr)
{
    if (angle_q16 >= (FIX16_PI_DIV_3 * 2))
    {
        motor_ptr->mpos.trap.BEMFStateNew = 4;
    }
    else if (angle_q16 >= (FIX16_PI_DIV_3))
    {
        motor_ptr->mpos.trap.BEMFStateNew = 6;
    }
    else if (angle_q16 >= 0)
    {
        motor_ptr->mpos.trap.BEMFStateNew = 2;
    }
    else if (angle_q16 >= (-FIX16_PI_DIV_3))
    {
        motor_ptr->mpos.trap.BEMFStateNew = 3;
    }
    else if (angle_q16 >= (-FIX16_PI_DIV_3*2))
    {
        motor_ptr->mpos.trap.BEMFStateNew = 1;
    }
    else
    {
        motor_ptr->mpos.trap.BEMFStateNew = 5;
    }
}

void trap_init(Struct_Motor* motor_ptr)
{
    // Fill state angle table
    motor_ptr->mpos.trap.trapstate_angle_fwd[0] = 0;
    motor_ptr->mpos.trap.trapstate_angle_fwd[1] = -FIX16_PI_DIV_3 * 2;      // -120
    motor_ptr->mpos.trap.trapstate_angle_fwd[2] = 0;                        // 0
    motor_ptr->mpos.trap.trapstate_angle_fwd[3] = -FIX16_PI_DIV_3;          // -60
    motor_ptr->mpos.trap.trapstate_angle_fwd[4] = FIX16_PI_DIV_3 * 2;       // 120
    motor_ptr->mpos.trap.trapstate_angle_fwd[5] = FIX16_PI_DIV_3 * 3;       // 180
    motor_ptr->mpos.trap.trapstate_angle_fwd[6] = FIX16_PI_DIV_3;           // 60
    motor_ptr->mpos.trap.trapstate_angle_fwd[7] = 0;
}


void trap_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.trap.ptr_trapstate_angle_dir = &motor_ptr->mpos.trap.trapstate_angle_fwd[0];
//    motor_ptr->mpos.hall.ptr_angcorr_gain = &motor_ptr->mpos.hall.angcorr_gain_fwd_q20;
//    motor_ptr->mpos.hall.ptr_angcorr_offset = &motor_ptr->mpos.hall.angcorr_offset_fwd;

//    motor_ptr->mpos.hall.hall_ptr_next_state = &motor_ptr->mpos.hall.hall_forward_next_state[0];
//    motor_ptr->mpos.hall.hall_ptr_prev_state = &motor_ptr->mpos.hall.hall_forward_prev_state[0];
    motor_ptr->mpos.trap.trap_forward_next_state[0] = 0;
    motor_ptr->mpos.trap.trap_forward_next_state[1] = 3;
    motor_ptr->mpos.trap.trap_forward_next_state[2] = 6;
    motor_ptr->mpos.trap.trap_forward_next_state[3] = 2;
    motor_ptr->mpos.trap.trap_forward_next_state[4] = 5;
    motor_ptr->mpos.trap.trap_forward_next_state[5] = 1;
    motor_ptr->mpos.trap.trap_forward_next_state[6] = 4;

    // Reset trap transition counter
    motor_ptr->mpos.trap.transition_counter = 0;

    // Reset trap speed and filter
    motor_ptr->mpos.trap.trap_spd_radps = 0;
    motor_ptr->mpos.trap.trap_spd_scaled_q14 = 0;
    motor_ptr->mpos.trap.trap_spd_scaled_q28 = 0;
    motor_ptr->mpos.trap.trap_spd_radps = 0;
    motor_ptr->mpos.trap.trap_motor_direction = 0;
    motor_ptr->mpos.trap.trap_speed_control_counter = 0;
    motor_ptr->mpos.trap.trap_speed_control_storecounter = 0;

    // Reset angle diff between Trap and Estimator
    motor_ptr->mpos.trap.trap_est_angle_diff = 0;
    // Reset count delay from Estimator to Trap
    motor_ptr->mpos.trap.est2trap_delay_cnt = 0;

    // Reset interpolation
    motor_ptr->mpos.trap.accumulate_degree_value = 0;
    motor_ptr->mpos.trap.accumulate_degree_per_dt = 0;

    // Reset state flags
    motor_ptr->mpos.trap.transition_flag = FALSE;
    motor_ptr->mpos.trap.transition_ext_flag = FALSE;
    motor_ptr->mpos.trap.do_speed_cal = FALSE;
    motor_ptr->mpos.trap.flag_speed_calc_enabled = FALSE;
    motor_ptr->mpos.trap.flag_inittrapspdfilt = TRUE;

    // Reset trap state to current state
    motor_ptr->mpos.trap.BEMFStateLast = motor_ptr->mpos.trap.BEMFStateNew = 2;

    // Calculate trap angle for current state
    trap_calc_motor_angle(motor_ptr);
//#ifdef ENABLE_ANGLE_CORRECTION
//    // Angle Correction (only offset)
//    motor_ptr->mpos.hall.hall_angle = motor_ptr->mpos.hall.angle_fix16 + (*motor_ptr->mpos.hall.ptr_angcorr_offset * motor_ptr->app.app_motor_direction);
//#endif

    motor_ptr->mpos.trap.trap_angle = motor_ptr->mpos.trap.angle_fix16;

//    // Reset diagnostics
//    motor_ptr->mpos.hall.hall_check_error_code = 0;
//    motor_ptr->mpos.hall.hall_check_error_state_new = 0;
//    motor_ptr->mpos.hall.hall_check_error_state_prev = 0;
}


void trap_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
#if (MODULE_MPOS_TRAP == ENABLED) && (MODULE_MPOS_LBG1 == ENABLED)
    case misc_uint32_1:
        motor_ptr->mpos.trap.switchover_spd_lo2hi_radps = rx_data * FIX16_2PI;
        motor_ptr->mpos.trap.switchover_spd_hi2lo_radps = fix16_mul(motor_ptr->mpos.trap.switchover_spd_lo2hi_radps, (Q16_1P0 - SWITCHOVER_SPD_TRAPEST_HYST_PCT_Q16));
        break;
#endif
    default:
        break;
    }
}

#endif  //#if (MODULE_MPOS_TRAP == ENABLED)


