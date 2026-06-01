
/**
 * @file fw_funcs.c
 * @brief Use for field weakening utility functions
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

#include "motor_struct.h"
#include "fw_funcs.h"
#include "param_funcs.h"
#include "fault.h"

void fw_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Parameter process handler
    switch(param_code)
    {
    case fw_id_sp:
#ifdef ENABLE_FIELD_WEAKENING
        motor_ptr->fw.id_sp = rx_data;
    #if (FIELD_WEAKENING_METHOD == PID_CONTROL)
        motor_ptr->fw.fw_pid.min_value = (motor_ptr->fw.id_sp >> 2);
    #endif
        break;
    case fw_vdrive_threshold:
        break;
    case fw_speed_threshold_sp:
    #ifdef ENABLE_FIELD_WEAKENING
        motor_ptr->fw.spd_threshold = rx_data;
    #endif
        break;
    #if (FIELD_WEAKENING_METHOD == PID_CONTROL)
    case fw_pid_Kp:
        motor_ptr->fw.fw_pid.ka_q16 = rx_data;
        break;
    case fw_pid_Ki:
        motor_ptr->fw.fw_pid.kb_q16 = rx_data;
        break;
    case fw_vmax_scl_factor:
        motor_ptr->fw.vmax_scl_q14 = ((rx_data >> 2) * FIX16_1P0_DIV_100) >> 16;
        break;
    #endif
#endif
    default:
        break;
    }
}

#if defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == INCREMENTRAL_CONTROL)
/**
 * @brief  Use to initialize variables required to enter FIELD WEAKENING
 * @param Struct_Motor* motor_ptr
 */
void fw_enter(Struct_Motor *motor_ptr)
{
    // Set semaphore flag (red)
    motor_ptr->fw.flag_semaphore = 1;

    // Update control loop variables
    motor_ptr->control.idref_target_f16 = motor_ptr->control.idref_target_f16 - QCONST(0.01f, Q16);
    motor_ptr->fw.iqmax_q14 = motor_ptr->foc.iqref_ramp_q14;
    motor_ptr->fw.fw_active_flag = 1;

    // Clear semaphore flag (green)
    motor_ptr->fw.flag_semaphore = 0;

    // Update status
    set_status(STATUS_MOTOR_FW_ACTIVE);
}


/**
 * @brief  Use to initialize variables required to exit FIELD WEAKENING
 * @param Struct_Motor* motor_ptr
 */
void fw_exit(Struct_Motor *motor_ptr)
{
    // Set semaphore flag (red)
    motor_ptr->fw.flag_semaphore = 1;

    // Update control loop variables
    motor_ptr->control.idref_target_f16 = motor_ptr->app.idrefcl_cmd_f16;
    motor_ptr->fw.iqmax_q14 = (motor_ptr->control.imax_q16 >> 2);
    motor_ptr->fw.fw_active_flag = 0;

    // Clear semaphore flag (green)
    motor_ptr->fw.flag_semaphore = 0;

    // Ensure that Outerloop PI max is restored
    motor_ptr->control.outerloop_pid.max_value = motor_ptr->fw.iqmax_q14;

    // Update status
    clear_status(STATUS_MOTOR_FW_ACTIVE);
}


void fw_controller(Struct_Motor *motor_ptr)
{
    // Internal variable declarations
    int32_t flag_sat_logicand_internal;
    int32_t flag_sat_logicor_internal;
    int32_t idrefcl_target_temp;
    int32_t iqmax_temp_q14;
    int32_t iqmax_limit_q16;
    int32_t delta;


    // Read variables from control loop and reset
    flag_sat_logicand_internal = motor_ptr->fw.flag_sat_logicand;
    flag_sat_logicor_internal = motor_ptr->fw.flag_sat_logicor;
    motor_ptr->fw.flag_sat_logicand = 1;
    motor_ptr->fw.flag_sat_logicor = 0;

    ///////////////////////////////
    // Field-Weakening Controller
    ///////////////////////////////
    if (motor_ptr->state.main == STATE_MAIN_RUN)
    {
        // If speed is above FW threshold
        if (absval(motor_ptr->mpos.mpos_spd_radps) >= motor_ptr->fw.spd_threshold)
        {
            ///////////////////////////
            // Next State
            ///////////////////////////
            // If FW is not active
            if (motor_ptr->fw.fw_active_flag == 0)
            {
                if (flag_sat_logicand_internal)
                {
                    // Enter field weakening
                    fw_enter(motor_ptr);

                    //debugio2_on();
                }
            }
            // If FW is active
            else
            {
                ///////////////////////////
                // Id_ref controller
                ///////////////////////////
                // Increment/decrement id_ref
                delta = (motor_ptr->control.outerloop_ramp_scaled_q14 - motor_ptr->control.outerloop_fdbk_scaled_q14) * motor_ptr->app.app_motor_direction;
                if ((delta >= OUTERLOOP_ERR_THRESHOLD) && flag_sat_logicor_internal)
                {
                    idrefcl_target_temp = motor_ptr->control.idref_target_f16 - ID_INCREMENT_BIG;
                    //debugio1_toggle();
                }
                else if ((delta >= 0) && flag_sat_logicor_internal)
                    idrefcl_target_temp = motor_ptr->control.idref_target_f16 - ID_INCREMENT_SMALL;
                else if (delta >= (-OUTERLOOP_ERR_THRESHOLD))
                    idrefcl_target_temp = motor_ptr->control.idref_target_f16 + ID_INCREMENT_SMALL;
                else
                    idrefcl_target_temp = motor_ptr->control.idref_target_f16 + ID_INCREMENT_BIG;

                // Bound idref
                if (idrefcl_target_temp < QCONST(-0.95f, Q16))
                    idrefcl_target_temp = QCONST(-0.95f, Q16);
                else if (idrefcl_target_temp > 0)
                    idrefcl_target_temp = 0;

                // Limit id_ref to user value
                if (idrefcl_target_temp < motor_ptr->fw.id_sp)
                    idrefcl_target_temp = motor_ptr->fw.id_sp;

                ///////////////////////////
                // Iq_max controller
                ///////////////////////////
                // Increment/decrement iq_max
                if (flag_sat_logicand_internal == 1)
                    iqmax_temp_q14 = motor_ptr->fw.iqmax_q14 - QCONST(0.001f, Q14);
                else
                    iqmax_temp_q14 = motor_ptr->fw.iqmax_q14 + QCONST(0.001f, Q14);

                // Run circle limiter to ensure total current limit is not exceeded
                circle_limiter(motor_ptr->control.imax_q16, motor_ptr->control.imax_1div_q16, -motor_ptr->foc.idref_ramp_f16, &iqmax_limit_q16);
                if (iqmax_temp_q14 > (iqmax_limit_q16 >> 2))
                    iqmax_temp_q14 = (iqmax_limit_q16 >> 2);
                iqmax_temp_q14 = iqmax_temp_q14 * motor_ptr->app.app_motor_direction;


                //////////////////////////////////////////////////////////
                // Use semaphore to update values for control loop
                //////////////////////////////////////////////////////////
                // Store semaphore values
                motor_ptr->fw.idrefcl_target_store_f16 = motor_ptr->control.idref_target_f16;
                motor_ptr->fw.iqmax_store_q14 = motor_ptr->control.outerloop_pid.max_value;

                // Set semaphore flag (red)
                motor_ptr->fw.flag_semaphore = 1;

                // Update control loop variables
                motor_ptr->control.idref_target_f16 = idrefcl_target_temp;
                motor_ptr->fw.iqmax_q14 = iqmax_temp_q14;

                // Clear semaphore flag (green)
                motor_ptr->fw.flag_semaphore = 0;

                ///////////////////////////
                // Next State
                ///////////////////////////
                // Exit FW if id_ref has ramped back to CL target
                if (motor_ptr->foc.idref_ramp_q14 >= (motor_ptr->app.idrefcl_cmd_f16 >> 2))
                {
                    // Exit field weakening
                    fw_exit(motor_ptr);

                    //debugio2_off();
                }
            }
        }
        ///////////////////////////
        // Next State
        ///////////////////////////
        // If speed is below FW threshold
        else
        {
            // Exit field weakening
            fw_exit(motor_ptr);

            //debugio2_off();
        }
    }

#if 0
//    debug_dac_1a = motor_ptr->fw.iqmax_q14;
//    debug_dac_1b = flag_sat_logicor_internal << 14;
//    debug_dac_1c = idrefcl_target_temp;
//    debug_var_1 = motor_ptr->control.outerloop_ramp_scaled_q14 << 2;
//    debug_var_2 = motor_ptr->control.outerloop_fdbk_scaled_q14 << 2;
#endif
}

void fw_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->fw.fw_active_flag = 0;
    clear_status(STATUS_MOTOR_FW_ACTIVE);
    motor_ptr->fw.flag_sat_logicand = 0;
    motor_ptr->fw.flag_sat_logicor = 0;
}
#elif defined(ENABLE_FIELD_WEAKENING) && (FIELD_WEAKENING_METHOD == PID_CONTROL)


void fw_controller(Struct_Motor *motor_ptr)
{
    if(motor_ptr->state.control == STATE_CONTROL_OUTERLOOP)
    {
        // Field Weakening control
        if ((motor_ptr->mpos.mpos_spd_radps * motor_ptr->app.app_motor_direction) >= motor_ptr->fw.spd_threshold)
        {
            motor_ptr->fw.fw_active_flag = 1;
            // Update status
            set_status(STATUS_MOTOR_FW_ACTIVE);
        }
        else
        {
            motor_ptr->fw.fw_active_flag = 0;
            motor_ptr->control.outerloop_pid.max_value = motor_ptr->control.imax_q16 >> 2;              // q14
            // Update status
            clear_status(STATUS_MOTOR_FW_ACTIVE);
        }
    }
}

void fw_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->fw.fw_pid.dt_q26 = motor_ptr->control.outerloop_pid.dt_q26;
    picon_convert_gain(&motor_ptr->fw.fw_pid, motor_ptr->app.v_scale_q16, motor_ptr->app.descale_current_factor);
    motor_ptr->fw.fw_pid.min_value = (motor_ptr->fw.id_sp >> 2);
    motor_ptr->fw.fw_pid.max_value = 0;

    pid_reset(&(motor_ptr->fw.fw_pid));
    motor_ptr->fw.fw_active_flag = 0;
    clear_status(STATUS_MOTOR_FW_ACTIVE);
    motor_ptr->fw.vrms_filter_coeff_q16 = calc_filter_alpha(VRMS_FILTER_CUTOFF_FREQ_Q16, motor_ptr->foc.dt_control_q26);
    motor_ptr->fw.vq_vd_squares_root_filt_q14 = 0;
}
#endif  //#ifdef ENABLE_FIELD_WEAKENING
