
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

#include "qep_funcs.h"
#include "maths.h"
#include "param_funcs.h"
#include "calc_speed.h"

#include "debug.h"

#if (MODULE_MPOS_QEP == ENABLED)

// Enable QEP offset angle calculation
/* Steps to auto-calculate the offset angle
1. Set "Itotal ref OL" with an initial value (Be sure that Motor is able to rotate with this value) and "Id ref OL" = 0;
2. Set "OL speed setpoint" as a slow speed (i.e., 1 Hz or less);
3. Run the motor in Open loop and make sure the motor is rotating smoothly;
4. Set "Id ref OL" equal to "Itotal ref OL" while Motor is running;
5. The QEP offset angle is "motor_ptr->mpos.qep.qep_offset_angle_deg_q16" in unit of degree, while is updated once detecting each QEP index.
NOTES: This offset angle may be affected by the motor cogging torque!
It is recommended to do the offset angle fine tuning based on the auto-calculated result!
*/

void qep_init(Struct_Motor* motor_ptr)
{
    motor_ptr->mpos.qep.index_counter = 0;
    // Set QEP REady to FALSE for first start after powerup. After first start QEP index should be synced and can skip Align&Go
#ifndef ENABLE_QEP_LINEARIZATION_TUNING
    motor_ptr->startup.qep_ready = FALSE;
#endif
}


void qep_reset(Struct_Motor* motor_ptr)
{
    // Status

    // Outputs
    motor_ptr->mpos.qep.qep_spd_radps = 0;
    motor_ptr->mpos.qep.qep_spd_scaled_q14 = 0;
    motor_ptr->mpos.qep.qep_spd_scaled_q28 = 0;
    motor_ptr->mpos.qep.qep_spd_prefilt_scaled_q14 = 0;
    motor_ptr->mpos.qep.qep_angle = 0;
    motor_ptr->mpos.qep.qep_motor_direction = motor_ptr->app.app_motor_direction;

    // Running Variables
    motor_ptr->mpos.qep.qep_calculated_angle = 0;
    motor_ptr->mpos.qep.qep_previous_angle = 0;
    motor_ptr->mpos.qep.counter_counts = 0;
    motor_ptr->mpos.qep.elec_angle_wrap_around_counts = 0;
    
    if(motor_ptr->app.app_motor_direction == -1)
        motor_ptr->mpos.qep.ptr_angcorr_offset = &motor_ptr->mpos.qep.angcorr_offset_rev;
    else
        motor_ptr->mpos.qep.ptr_angcorr_offset = &motor_ptr->mpos.qep.angcorr_offset_fwd;

    // Enable QEP timer
#if defined(FAMILY_55XX)
    qep_timer_enabled();
    qep_index_event_clr_interrupt();
    NVIC_EnableIRQ(QEP_TIMER_IRQn);
#endif
}

void qep_param_calc(Struct_Motor* motor_ptr)
{
    // Calculate speed filter alpha gain
    fix16_t dt_actual_q26;
    dt_actual_q26 = QEP_SPEED_CALCULATION_DIVIDER * motor_ptr->control.dt_control_q26;      // q0*q26 = q26
    motor_ptr->mpos.qep.filter_alpha_speed_q16 = calc_filter_alpha(motor_ptr->mpos.qep.filter_fc_speed_q16, dt_actual_q26);

    // Calculate speed conversion factors
    calculate_speed_coefficients(dt_actual_q26, motor_ptr->app.speed_scale_shift, &motor_ptr->mpos.qep.kspd_radps_q6, &motor_ptr->mpos.qep.kspd_scaled_q14);

    // Calculate conversion factor from QEP counter to mechanical angle
    motor_ptr->mpos.qep.conv_qep2mech_q24 = fix16_div((FIX16_2PI * motor_ptr->mparam.pole_pairs), (QEP_PULSE_PER_RESOLUTION_FINAL << 8));           // q24 = div(((q16*q0)=q16), (q0<<8)=q8)=q24

#if defined(ENABLE_QEP_STARTUP_TIMEOUT) && !defined(ENABLE_QEP_LINEARIZATION_TUNING)
    // Calculate num of electrical cycles required for AngleDiff Timeout in QEP Mechanical Rotation terms
    motor_ptr->mpos.qep.qep_elec_angle_counts_threshold = motor_ptr->mparam.pole_pairs * NUM_QEP_MECH_ROTATION;
#endif
}


void qep_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case QEP_Resolution:
        motor_ptr->mpos.qep.qep_resolution = QEP_PULSE_PER_RESOLUTION;               // To be changed from R/W to Read only in GUI
        break;
    case QEP_Offset_Angle_Fwd:
        motor_ptr->mpos.qep.angcorr_offset_fwd = rx_data;
        break;
    case QEP_Offset_Angle_Rev:
        motor_ptr->mpos.qep.angcorr_offset_rev = rx_data;
        break;
    case QEP_Fc:
        motor_ptr->mpos.qep.filter_fc_speed_q16 = rx_data;
        break;
    case QEP_Main_Align_Offset:
        motor_ptr->mpos.qep.qep_sensor_offset_deg = rx_data;
        motor_ptr->mpos.qep.qep_sensor_offset_rad = fix16_mul(rx_data, CONV_DEG2RAD_FIX16);
        break;
#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
    case misc_uint32_2:
        motor_ptr->mpos.qep.switchover_spd_lo2hi_radps = rx_data * FIX16_2PI;
        motor_ptr->mpos.qep.switchover_spd_hi2lo_radps = fix16_mul(motor_ptr->mpos.qep.switchover_spd_lo2hi_radps, (Q16_1P0 - SWITCHOVER_SPD_QEPEST_HYST_PCT_Q16));
        break;
#endif
    default:
        break;
    }
}


void qep_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case dt:
    case num_pole_pairs:
    case max_cl_speed_limit:
    case QEP_Resolution:
    case QEP_Fc:
        qep_param_calc(motor_ptr);
        break;
    default:
        break;
    }
}

PAC5XXX_OPTIMIZE_SPEED_ON 
PAC5XXX_RAMFUNC void qep_isr(void)
{
//debugio1_toggle();
#if defined(FAMILY_55XX)
    static Struct_Motor* motor_ptr = &motor;
    #ifdef ENABLE_QEP_OFFSET_TUNING
        if (qep_check_index_event_interrupt_flag())
        {
            qep_index_event_clr_interrupt();

            if (motor_ptr->mpos.qep.qep_offset_tuning_enable_cmd)
            {
              // Calculate the QEP offset angle and convert the angle from rad to Degree
              motor_ptr->mpos.qep.qep_offset_angle_deg_q16 = ((motor_ptr->mpos.aligngo.alignangle_q26 >> 10) * QCONST(57.3f, Q07)) >> 7;
            }
        }
    #elif defined(ENABLE_QEP_LINEARIZATION_TUNING)
        int32_t temp, temp_value;
        int32_t cnt_diff_temp1_q06;
        int32_t cnt_diff_temp2_q06;
        int32_t cnt_diff_temp3_q06;
        int32_t cnt_diff_temp4_q06;
        static int32_t cnt_diff_temp4_prev_q06;
        
        if (qep_check_index_event_interrupt_flag())
        {
            qep_index_event_clr_interrupt();

            // Accumulate the number of index
            if (motor_ptr->mpos.qep.qep_linearization_start_flag == 1)               // Enable to start accumulating from debug inputs
            {
                motor_ptr->mpos.qep.index_count++;
                if (motor_ptr->mpos.qep.index_count > (TOTAL_ACCUMULATION_CNT + 1))
                {
                    motor_ptr->mpos.qep.index_count = (TOTAL_ACCUMULATION_CNT + 1);
                }
            }
            else  motor_ptr->mpos.qep.index_count = 0;

            if (motor_ptr->mpos.qep.index_count == 0)
            {
                // Reset qep count for linearization
                cnt_diff_temp4_prev_q06 = 0;
                motor_ptr->mpos.qep.index_angle = motor_ptr->mpos.mpos_mech_angle;
            }
        }

        if (qep_check_phase_a_event_interrupt_flag())
        {
            qep_phase_a_event_clr_interrupt();

            // Read QEP counter
            motor_ptr->mpos.qep.qep_physical_count = (((QEP_TIMER->QEPCTL.w) >> 16) & 0xFFFF) - 1;

            // Limit the abs max count from QEP
            if (motor_ptr->mpos.qep.qep_physical_count > (QEP_PULSE_PER_RESOLUTION - 1)) 
            {
                motor_ptr->mpos.qep.qep_physical_count = (QEP_PULSE_PER_RESOLUTION - 1);
            }
            // Treat the estimator mechenical angle as an accurate angle for linearization, start from zero after Index hit
            motor_ptr->mpos.qep.accurate_angle_ref_idx_aligned = motor_ptr->mpos.mpos_mech_angle - motor_ptr->mpos.qep.index_angle;     // Q16
            // Wrap the angle from [0, 2PI*Pole_Pair]
            if (motor_ptr->mpos.qep.accurate_angle_ref_idx_aligned < 0)
            {
                motor_ptr->mpos.qep.accurate_angle_ref_idx_aligned += motor_ptr->mparam.pole_pairs * (FIX16_PI << 1);
            }
            // Convert the angle unit from Rad to Count
            motor_ptr->mpos.qep.accurate_qep_count_q6 = (motor_ptr->mpos.qep.convert_rad_to_qep_count_q10 * (motor_ptr->mpos.qep.accurate_angle_ref_idx_aligned >> 4)) >> 16; // Q06

            // Write the accurate count to the array, which size is based on Quad mode
            if ((motor_ptr->mpos.qep.qep_linearization_start_flag == 1) && (motor_ptr->mpos.qep.index_count != 0))
            {
                temp = motor_ptr->mpos.qep.qep_physical_count << 2;
                // Make sure the arrary is not overflowed
                if (temp == 0) temp = QEP_PULSE_PER_RESOLUTION_FINAL;
                if (temp < QEP_PULSE_PER_RESOLUTION_FINAL)  
                // Write four elements in array because of Quad mode
                // 1/4 of the QEP difference between counts
                {
                    cnt_diff_temp4_q06 = (motor_ptr->mpos.qep.accurate_qep_count_q6 << 2) - (temp << 6);            // q06
                    temp_value = (cnt_diff_temp4_q06 - cnt_diff_temp4_prev_q06) >> 2;
                    cnt_diff_temp3_q06 = cnt_diff_temp4_q06 - temp_value;
                    cnt_diff_temp2_q06 = cnt_diff_temp3_q06 - temp_value;
                    cnt_diff_temp1_q06 = cnt_diff_temp2_q06 - temp_value;
                    // Accumulation
                    if (motor_ptr->mpos.qep.index_count <= TOTAL_ACCUMULATION_CNT)
                    {
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[temp]     += cnt_diff_temp4_q06;
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[temp - 1] += cnt_diff_temp3_q06;
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[temp - 2] += cnt_diff_temp2_q06;
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[temp - 3] += cnt_diff_temp1_q06;
                    }
                    // Store the previous count diff for next interporation
                    cnt_diff_temp4_prev_q06 = cnt_diff_temp4_q06;
                }
                else
                {
                    // Accumulation
                    if (motor_ptr->mpos.qep.index_count > TOTAL_ACCUMULATION_CNT)
                    {
                        temp_value = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 4] >> 2;            // q06
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 3] = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 4];
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 2] = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 4];
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 1] = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 4];
                        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[0] = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[QEP_PULSE_PER_RESOLUTION_FINAL - 4];
                    }
                }
                // Test use only
                #if 0
                debug_dac_1a = motor_ptr->mpos.qep.qep_physical_count << 6;
                debug_dac_1b = motor_ptr->mpos.qep.accurate_angle_ref_idx_aligned; 
                debug_dac_1c = motor_ptr->mpos.qep.accurate_qep_count_q6;
                debug_var_1 =  motor_ptr->mpos.qep.qep_physical_count << 16;
                debug_var_2 =  motor_ptr->mpos.qep.accurate_qep_count_q6 << 10;
                #endif
            }
        }
    #else
        if (qep_check_index_event_interrupt_flag())
        {
            motor_ptr->mpos.qep.index_counter++;
            NVIC_DisableIRQ(QEP_TIMER_IRQn);
        }
    #endif
#endif
//debugio1_toggle();
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use for process QEP offset and linearization message 
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void qep_offset_linearization_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
#ifdef ENABLE_QEP_OFFSET_TUNING
    //QEP Offset Linearization Cal
    case QEP_Offset_CalEnable:
        if (rx_data != 0)    motor_ptr->mpos.qep.qep_offset_tuning_enable_cmd = TRUE;
        else    motor_ptr->mpos.qep.qep_offset_tuning_enable_cmd = FALSE;
        break;
#endif

#ifdef ENABLE_QEP_LINEARIZATION_TUNING
    case QEP_Linerization_Enable:
        if (rx_data != 0)    motor_ptr->mpos.qep.qep_linearization_enable_cmd = TRUE;
        else    motor_ptr->mpos.qep.qep_linearization_enable_cmd = FALSE;
        break;
#endif
    default:
        break;
    }
}


PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC void qep_process(Struct_Motor* motor_ptr)
{
#if defined(ENABLE_EST_ASSIST)
    int32_t counter_prev = motor_ptr->mpos.qep.counter_counts;
#endif

    // Read and manipulate QEP counter
    qep_read_counter(motor_ptr);

    // Calculate QEP angle
    qep_calc_motor_angle(motor_ptr);

#if defined(ENABLE_EST_ASSIST)
    if (motor_ptr->mpos.qep.counter_counts != counter_prev)
        est_assist(motor_ptr, motor_ptr->mpos.qep.qep_angle);
#endif

#if (QEP_SPEED_CALCULATION_DIVIDER > 1)
    static fix16_t qep_loop_count = 0;
    if (++qep_loop_count >= QEP_SPEED_CALCULATION_DIVIDER)
    {
        qep_loop_count = 0;
#endif
        // Calculate QEP speed
        qep_calc_motor_speed(motor_ptr);

#if (QEP_SPEED_CALCULATION_DIVIDER > 1)
    }
#endif

#if defined(ENABLE_ANGLE_CORRECTION) || defined(ENABLE_MTPA)
    qep_angle_correction(motor_ptr);
#else
    motor_ptr->mpos.qep.qep_angle = motor_ptr->mpos.qep.qep_calculated_angle;
#endif

    qep_count_elect_wrap_around(motor_ptr);
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#if (MODULE_MPOS_QEP == ENABLED)
