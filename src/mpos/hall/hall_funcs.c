
/**
 * @file hall_funcs.c
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

#include "hall_funcs.h"
#include "maths.h"
#include "param_funcs.h"
#include "hall_io_config.h"
#include "hall_timer_config.h"
#include "mpos_misc.h"

#if defined(ENABLE_HALL_TUNING)
#include "halltune_funcs.h"
#endif


#if (MODULE_MPOS_HALL == ENABLED)

/**
 * @brief This function use for hall processing
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC void hall_process(Struct_Motor* motor_ptr)
{
    int32_t local_flag_hall_state_int = 0;

    __disable_irq();
    // Read hall interrupt flag atomically
    local_flag_hall_state_int = motor_ptr->mpos.hall.flag_hall_int;
    // Clear hall interrupt flag to signal hall ISR transition is processed
    motor_ptr->mpos.hall.flag_hall_int = FALSE;
    __enable_irq();

    // Check if hall transition has occurred after transition flag indicates that previous transition has been processed
    if (!motor_ptr->mpos.hall.transition_flag)
    {
        hall_check_hall_state_changed(motor_ptr);
    }

//    debug_var_1 = motor_ptr->mpos.hall.hall_motor_direction << 16;
//    debug_dac_1a = motor_ptr->mpos.hall.hall_motor_direction << 13;
//    debug_dac_1b = motor_ptr->mpos.hall.SensorStateNew << 11;
//    debug_dac_1c = motor_ptr->mpos.hall.SensorStateLast << 11;

    // Process hall transition if state transition is detected and if hall interrupt has been detected
    if(motor_ptr->mpos.hall.transition_flag) // && local_flag_hall_state_int)        // This helps to solve the high speed issue
    {
        // Clear hall state transition flag
        motor_ptr->mpos.hall.transition_flag = FALSE;

        // Set external hall state transition flag
        motor_ptr->mpos.hall.transition_ext_flag = TRUE;

        // Fault if hall sequence is not valid
#if defined(ENABLE_HALL_SEQUENCE_CHECK) && !defined(ENABLE_HALL_TUNING)
        hall_check_sequence(motor_ptr);
#elif defined(ENABLE_HALL_SEQUENCE_CHECK) && defined(ENABLE_HALL_TUNING)
        if (motor_ptr->halltune.state != STATE_HALLTUNE_RUN)
        {
            hall_check_sequence(motor_ptr);
        }
#endif
        // Fault if read hall value is illegal
        hall_check_hall_state(motor_ptr);

        // Determine motor direction
        hall_determine_direction(motor_ptr);

        // Calculate hall angle
        hall_calc_motor_angle(motor_ptr);

        // Handle the case of kickback by decrementing the interpolation angle back to 0 to avoid a sudden jump in angle
        // JAGTAG: Kickback handling possibly not necessary. Leaving it in case it is still necessary
//        if (motor_ptr->mpos.hall.hall_ptr_prev_state[motor_ptr->mpos.hall.SensorStateLast] == motor_ptr->mpos.hall.SensorStateNew)
        if (0)
        {
            // Skip speed calculation for this transition
            motor_ptr->mpos.hall.do_speed_cal = FALSE;
            // Reset the transition counter to skip speed calculation on the first forward transition
            motor_ptr->mpos.hall.transition_counter = 0;

            // Set interpolation to decrement interpolation angle from current value down to 0
            motor_ptr->mpos.hall.accumulate_degree_per_dt = (QCONST(0.026f, Q16) * -motor_ptr->mpos.hall.hall_motor_direction);

            // Set speed and speed filter to 0
            motor_ptr->mpos.hall.hall_spd_radps = 0;
            motor_ptr->mpos.hall.hall_spd_scaled_q14 = 0;
            motor_ptr->mpos.hall.hall_spd_scaled_q28 = 0;
            motor_ptr->mpos.hall.hall_spd_radps = 0;
            motor_ptr->mpos.hall.hall_motor_direction = 0;
        }
        // Handle the case of normal forward transition
        else
        {
            // Reset interpolation angle and let speed calculator determine interpolation increment
            motor_ptr->mpos.hall.accumulate_degree_value = 0;

            // Skip speed calculation on the first transition because speed reading is not valid
            if (motor_ptr->mpos.hall.transition_counter >= 1)
            {
                motor_ptr->mpos.hall.flag_speed_calc_enabled = TRUE;
                motor_ptr->mpos.hall.do_speed_cal = TRUE;
            }
            // Increment internal hall transition counter
            motor_ptr->mpos.hall.transition_counter++;
        }

#if defined(ENABLE_HALL_TUNING)
        halltune_store_transition_angle(motor_ptr);
#endif

#if defined(ENABLE_ANGLE_CORRECTION) || defined(ENABLE_MTPA)
        hall_angle_correction(motor_ptr);
#else
        motor_ptr->mpos.hall.hall_interpolated_angle = motor_ptr->mpos.hall.angle_fix16;
#endif

#if defined(ENABLE_HALL_TUNING)
        halltune_edgetuning(motor_ptr);
#endif

        // Store the last state
        motor_ptr->mpos.hall.SensorStateLast = motor_ptr->mpos.hall.SensorStateNew;

        __disable_irq();
#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION) && (DEVICE_SELECT == DEVICE_5532)
        // Re-enable hall port interrupts
        PAC5XXX_GPIO_HALL->INTEN.w = motor_ptr->mpos.hall.hall_int_set;
#else
        // Re-enable hall port interrupts
        hall_int_hallport_enable();
#endif
        __enable_irq();
    }
    else
    {
        // Calculate hall speed and interpolation increment
        if (motor_ptr->mpos.hall.do_speed_cal)
        {
            // Calculate speed only in the first cycle after hall state transition
            motor_ptr->mpos.hall.do_speed_cal = FALSE;

            // Calculate motor speed
            hall_calc_motor_speed(motor_ptr);

#if defined(ENABLE_HALL_TUNING)
            halltune_edge_pol_skew_tuning(motor_ptr);
#endif
        }

        // Clear external hall state transition flag
        motor_ptr->mpos.hall.transition_ext_flag = FALSE;

        // Interpolate angle between hall state transitions
        hall_angle_interpolate(motor_ptr);
    }

#if defined(ENABLE_EST_ASSIST)
    est_assist(motor_ptr, motor_ptr->mpos.hall.hall_interpolated_angle);
#endif

#if 0
//    debug_dac_1a = motor_ptr->mpos.hall.SensorStateNew << 13;
//    debug_dac_1a = GET_HALL_VALUE << 11;
//    debug_dac_1b = motor_ptr->mpos.hall.SensorStateNew << 11;
//    debug_dac_1a = motor_ptr->mpos.hall.hall_spd_scaled_q14;
//    debug_dac_2b = motor_ptr->mpos.hall.angle_fix16 >> 5;
//    debug_dac_1c = motor_ptr->mpos.hall.hall_interpolated_angle >> 5;
//    debug_dac_1a = motor_ptr->mpos.hall.accumulate_degree_per_dt;
//    debug_dac_2c = motor_ptr->mpos.hall.accumulate_degree_value >> 5;
//    debug_dac_2a = motor_ptr->mpos.hall.hall_angle >> 5;
//    debug_dac_1a = motor_ptr->control.dt_control_q26;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF




/**
 * @brief Use for hall initialization
 */
void hall_init(Struct_Motor* motor_ptr)
{
    // Initialize hall state to current state
    motor_ptr->mpos.hall.SensorStateLast = motor_ptr->mpos.hall.SensorStateNew = GET_HALL_VALUE;
}


/**
 * @brief Use for reset FOC hall related variables, interrupt, ports
 * @param Struct_Motor* motor_ptr
 */
void hall_reset(Struct_Motor* motor_ptr)
{
    // JAGTAG: Maybe move to hall_init()
    NVIC_EnableIRQ(Hall_Gpio_IRQn);

    motor_ptr->mpos.hall.ptr_hallstate_angle_dir = &motor_ptr->mpos.hall.hallstate_angle_fwd[0];
    motor_ptr->mpos.hall.ptr_angcorr_gain = &motor_ptr->mpos.hall.angcorr_gain_fwd_q20;
    motor_ptr->mpos.hall.ptr_angcorr_offset = &motor_ptr->mpos.hall.angcorr_offset_fwd;

    motor_ptr->mpos.hall.hall_ptr_next_state = &motor_ptr->mpos.hall.hall_forward_next_state[0];
    motor_ptr->mpos.hall.hall_ptr_prev_state = &motor_ptr->mpos.hall.hall_forward_prev_state[0];

    // Reset hall transition counter
    motor_ptr->mpos.hall.transition_counter = 0;

    // Reset hall speed and filter
    motor_ptr->mpos.hall.hall_spd_radps = 0;
    motor_ptr->mpos.hall.hall_spd_scaled_q14 = 0;
    motor_ptr->mpos.hall.hall_spd_scaled_q28 = 0;
    motor_ptr->mpos.hall.hall_spd_radps = 0;
    motor_ptr->mpos.hall.hall_motor_direction = 0;

    // Reset interpolation
    motor_ptr->mpos.hall.accumulate_degree_value = 0;
    motor_ptr->mpos.hall.accumulate_degree_per_dt = 0;

    // Reset state flags
    motor_ptr->mpos.hall.transition_flag = FALSE;
    motor_ptr->mpos.hall.transition_ext_flag = FALSE;
    motor_ptr->mpos.hall.do_speed_cal = FALSE;
    motor_ptr->mpos.hall.flag_speed_calc_enabled = FALSE;
    motor_ptr->mpos.hall.flag_hall_int = FALSE;
    motor_ptr->mpos.hall.flag_inithallspdfilt = TRUE;

    // Reset hall state to current state
    motor_ptr->mpos.hall.SensorStateLast = motor_ptr->mpos.hall.SensorStateNew = GET_HALL_VALUE;

    // Calculate hall angle for current state
    hall_calc_motor_angle(motor_ptr);
#ifdef ENABLE_ANGLE_CORRECTION
    // Angle Correction (only offset)
    motor_ptr->mpos.hall.hall_angle = motor_ptr->mpos.hall.angle_fix16 + (*motor_ptr->mpos.hall.ptr_angcorr_offset * motor_ptr->app.app_motor_direction);
#endif

    // Enable hall timer
    hall_timer_clear();

#if defined(FAMILY_52XX)
    // Configure the hall interrupt edge polarity depending on hall port input
    pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLA_PORT_BIT, PAC5XXX_GPIO_HALLA->IN.b & HALLA_PORT_BIT);
    pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLB_PORT_BIT, PAC5XXX_GPIO_HALLB->IN.b & HALLB_PORT_BIT);
    pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLC_PORT_BIT, PAC5XXX_GPIO_HALLC->IN.b & HALLC_PORT_BIT);
#endif

#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION) && (DEVICE_SELECT == DEVICE_5532)
    // Configure the hall interrupt edge polarity depending on hall port input
    pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLA_PORT_BIT, PAC5XXX_GPIO_HALLA->IN.w & HALLA_PORT_BIT);
    pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLB_PORT_BIT, PAC5XXX_GPIO_HALLB->IN.w & HALLB_PORT_BIT);
    pac5xxx_gpio_int_polarity_hallport_bit_set_value(HALLC_PORT_BIT, PAC5XXX_GPIO_HALLC->IN.w & HALLC_PORT_BIT);
    motor_ptr->mpos.hall.hall_int_set = (HALLA_PORT_BIT | HALLB_PORT_BIT | HALLC_PORT_BIT);
#endif

    // Reset diagnostics
    motor_ptr->mpos.hall.hall_check_error_code = 0;
    motor_ptr->mpos.hall.hall_check_error_state_new = 0;
    motor_ptr->mpos.hall.hall_check_error_state_prev = 0;

    __disable_irq();
    // Enable hall port interrupts
    hall_int_hallport_enable();
    __enable_irq();
}


/**
 * @brief Use for processing hall timer ISR
 */
void hall_timer_isr(void)
{
    static Struct_Motor* motor_ptr = &motor;

#if defined(CONFIG_HALL_TIMEOUT_MDISABLE)
    // Disable motor on hall timeout event
    if (motor_ptr->state.main == STATE_MAIN_RUN)
    {
        set_fault(FAULT_HALL_TIMEOUT);
    }
#else
    // Handle hall timeout by treating as 0 speed
    // Reset hall transition counter
    motor_ptr->mpos.hall.transition_counter = 0;

    // Set speed and speed filter to 0
    motor_ptr->mpos.hall.hall_spd_radps = 0;
    motor_ptr->mpos.hall.hall_spd_scaled_q14 = 0;
    motor_ptr->mpos.hall.hall_spd_scaled_q28 = 0;
    motor_ptr->mpos.hall.hall_spd_radps = 0;
    motor_ptr->mpos.hall.hall_motor_direction = 0;

#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
    // if timerout, then forces to active to update hall value, solve the motor stuck issue.
    motor_ptr->mpos.hall.flag_hall_int = TRUE;
    motor_ptr->mpos.hall.SensorStateGet_flag = TRUE;
#endif

    // Reset interpolation
    motor_ptr->mpos.hall.accumulate_degree_per_dt = 0;
#endif

#if defined(ENABLE_SIM)
    sim_halltimer_intflag_set(motor_ptr);
#endif

    // Clear interrupt bit
    hall_timer_clr_interrupt();
}


/**
 * @brief Use to create next state table using hall state index information
 * @param int32_t* array_ptr_src
 * @param char* array_ptr_dest
 */
void hall_create_next_state_array(int32_t* array_ptr_src, char* array_ptr_dest)
{
// Function use to create next state table using hall state index information from ascending order angle table array
#define HALL_NEXT_STATE_ARRAY_LENGTH    8

    volatile uint8_t index, value, counter;

    for (counter = 1; counter < HALL_NEXT_STATE_ARRAY_LENGTH - 1; counter++)
    {
        index = array_ptr_src[counter];

        if (counter == (HALL_NEXT_STATE_ARRAY_LENGTH - 2))
            value = array_ptr_src[1];
        else
            value = array_ptr_src[counter + 1];

        array_ptr_dest[index] = (uint8_t) (value & 0xFF);
    }
}


/**
 * @brief Use to sort and create next state table using source hall state array
 * @param int32_t* array_ptr_src
 * @param char* array_ptr_dest
 */
void hall_sort_create_next_state_array(int32_t* array_ptr_src, char* array_ptr_dest)
{
#define START_INDEX_1    1
#define END_INDEX_1      7

    int32_t temp_angle_array[8] = {0,0,0,0,0,0,0,0};
    int32_t temp_index_array[8] = {0,0,0,0,0,0,0,0};
    volatile uint8_t count, count_element;

    // Copy angle info from 1D array to 1D array for sorting
    // Copy only data from index 1 to 6, indexes 0 and 7 are invalid
    for (count = START_INDEX_1; count < END_INDEX_1; count++)
        temp_angle_array[count] = array_ptr_src[count];

    // Sort array in ascending order for hall angles, smaller to bigger hall angle values and store in temporary 1D array
    // Sort function is a generic bubble sort also used in IPD
    bubble_sort_array(&temp_angle_array[START_INDEX_1], sizeof(temp_angle_array)/sizeof(temp_angle_array[0]) - 2);

    // Do a search and find hall states corresponding to hall angle values and save in temporary 1D array
    for (count = START_INDEX_1; count < END_INDEX_1; count++)
    {
        for (count_element = START_INDEX_1; count_element < END_INDEX_1; count_element++)
        {
            // Check if the hall angle value of sorted array matches the default hall angle array
            // If yes copy the array index for final hall angle, index array creation, use for next hall state calculation
            if(temp_angle_array[count] == array_ptr_src[count_element])
            {
                temp_index_array[count] = count_element;
                break;
            }
        }
    }

    // Call use to create next state table using hall state index information from ascending order angle table array
    hall_create_next_state_array(&temp_index_array[0], array_ptr_dest);
}

/**
 * @brief Use to sort in descending order and create next state table using source hall state array
 * @param int32_t* array_ptr_src
 * @param char* array_ptr_dest
 */
void hall_sort_create_prev_state_array(int32_t* array_ptr_src, char* array_ptr_dest)
{
#define START_INDEX_1    1
#define END_INDEX_1      7

    int32_t temp_angle_array[8] = {0,0,0,0,0,0,0,0};
    int32_t temp_index_array[8] = {0,0,0,0,0,0,0,0};
    volatile uint8_t count, count_element;

    // Copy angle info from 1D array to 1D array for sorting
    // Copy only data from index 1 to 6, indexes 0 and 7 are invalid
    for (count = START_INDEX_1; count < END_INDEX_1; count++)
        temp_angle_array[count] = array_ptr_src[count];

    // Sort array in ascending order for hall angles, smaller to bigger hall angle values and store in temporary 1D array
    // Sort function is a generic bubble sort also used in IPD
    bubble_sort_array_descending(&temp_angle_array[START_INDEX_1], sizeof(temp_angle_array)/sizeof(temp_angle_array[0]) - 2);

    // Do a search and find hall states corresponding to hall angle values and save in temporary 1D array
    for (count = START_INDEX_1; count < END_INDEX_1; count++)
    {
        for (count_element = START_INDEX_1; count_element < END_INDEX_1; count_element++)
        {
            // Check if the hall angle value of sorted array matches the default hall angle array
            // If yes copy the array index for final hall angle, index array creation, use for next hall state calculation
            if(temp_angle_array[count] == array_ptr_src[count_element])
            {
                temp_index_array[count] = count_element;
                break;
            }
        }
    }

    // Call use to create next state table using hall state index information from ascending order angle table array
    hall_create_next_state_array(&temp_index_array[0], array_ptr_dest);
}


/**
 * @brief Use to copy hall angle form snap angle to for, rev, next and prev state arrays
 * @param Struct_Motor* motor_ptr
 */
void hall_create_sequence_array(Struct_Motor* motor_ptr)
{
    uint8_t count;

    // Reset forward and reverse next state table to zero
    for (count = 0; count < 8; count++)
    {
        motor_ptr->mpos.hall.hall_forward_next_state[count] = 0;
        motor_ptr->mpos.hall.hall_forward_prev_state[count] = 0;
    }

    hall_sort_create_next_state_array(&motor_ptr->mpos.hall.hallstate_angle_ref[0],
            &motor_ptr->mpos.hall.hall_forward_next_state[0]);
    hall_sort_create_prev_state_array(&motor_ptr->mpos.hall.hallstate_angle_ref[0],
            &motor_ptr->mpos.hall.hall_forward_prev_state[0]);
}


void hall_create_hallstate_angle(Struct_Motor* motor_ptr)
{
    int32_t hallstate, index_angle, delta_angle, temp_angle;

    motor_ptr->mpos.hall.hallstate_angle_fwd[0] = 0;
    motor_ptr->mpos.hall.hallstate_angle_fwd[7] = 0;
    motor_ptr->mpos.hall.hallstate_angle_rev[0] = 0;
    motor_ptr->mpos.hall.hallstate_angle_rev[7] = 0;

    // Use state 1 as the electrical rotation index
    index_angle = motor_ptr->mpos.hall.hallstate_angle_ref[1];

    // Calculate difference between index hall angle offset
    delta_angle = motor_ptr->mpos.hall.hall_sensor_offset_rad - index_angle;

//    debug_var_2 = index_angle;
//    debug_var_3 = delta_angle;

    // Apply delta angle to all the angles to generate hallstate-angle array
    for (hallstate = 1; hallstate < 7; ++hallstate)
    {
        temp_angle = motor_ptr->mpos.hall.hallstate_angle_ref[hallstate] + delta_angle;
        wraparound(&temp_angle);
        motor_ptr->mpos.hall.hallstate_angle_fwd[hallstate] = temp_angle;

        temp_angle += FIX16_PI_DIV_3;
        wraparound(&temp_angle);
        motor_ptr->mpos.hall.hallstate_angle_rev[hallstate] = temp_angle;
    }
}


/**
 * @brief Use for processing hall FOC related parameters
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void hall_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case HallConfig_Istart:
        motor_ptr->startup.i_total_hall_gui = rx_data;
        break;
    case HallConfig_Gain_Fwd:
        motor_ptr->mpos.hall.angcorr_gain_fwd_q20 = rx_data;
        break;
    case HallConfig_Gain_Rev:
        motor_ptr->mpos.hall.angcorr_gain_rev_q20 = rx_data;
        break;
    case HallConfig_Offset_Fwd:
        motor_ptr->mpos.hall.angcorr_offset_fwd = rx_data;
        break;
    case HallConfig_Offset_Rev:
        motor_ptr->mpos.hall.angcorr_offset_rev = rx_data;
        break;
#if (MODULE_MPOS_HALL == ENABLED) && (MODULE_MPOS_LBG1 == ENABLED)
    case HallConfig_Speed:
        motor_ptr->mpos.hall.switchover_spd_lo2hi_radps = rx_data;
        motor_ptr->mpos.hall.switchover_spd_hi2lo_radps = fix16_mul(motor_ptr->mpos.hall.switchover_spd_lo2hi_radps, (Q16_1P0 - SWITCHOVER_SPD_HALLEST_HYST_PCT_Q16));
        break;
#endif
    case HallConfig_HallStateAngleRef_1_Fwd:
        motor_ptr->mpos.hall.hallstate_angle_ref[1] = rx_data;
        break;
    case HallConfig_HallStateAngleRef_2_Fwd:
        motor_ptr->mpos.hall.hallstate_angle_ref[2] = rx_data;
        break;
    case HallConfig_HallStateAngleRef_3_Fwd:
        motor_ptr->mpos.hall.hallstate_angle_ref[3] = rx_data;
        break;
    case HallConfig_HallStateAngleRef_4_Fwd:
        motor_ptr->mpos.hall.hallstate_angle_ref[4] = rx_data;
        break;
    case HallConfig_HallStateAngleRef_5_Fwd:
        motor_ptr->mpos.hall.hallstate_angle_ref[5] = rx_data;
        break;
    case HallConfig_HallStateAngleRef_6_Fwd:
        motor_ptr->mpos.hall.hallstate_angle_ref[6] = rx_data;
        break;
    case HallConfig_HallStateAngleRef_1_Rev:
    case HallConfig_HallStateAngleRef_2_Rev:
    case HallConfig_HallStateAngleRef_3_Rev:
    case HallConfig_HallStateAngleRef_4_Rev:
    case HallConfig_HallStateAngleRef_5_Rev:
    case HallConfig_HallStateAngleRef_6_Rev:
        break;

#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)    
    case hall_mech_angle_offset:
        motor_ptr->mpos.hall.hall_sensor_offset_deg_gui = rx_data;
        motor_ptr->mpos.hall.hall_sensor_offset_rad = fix16_mul(motor_ptr->mpos.hall.hall_sensor_offset_deg_gui, CONV_DEG2RAD_FIX16);
        hall_create_hallstate_angle(motor_ptr);
        break; 
#endif 

#if (MODULE_MPOS_HALL == ENABLED)
    case halledge_pol_skew_time:
    {
        // Time received in msec and converted to sec in q26
        //int32_t temp = fix16_div(QCONST(1024.0f, Q16), QCONST(1000.0f, Q16));
        motor_ptr->mpos.hall.halledge_polskew_msec_gui_q16 = rx_data;
        motor_ptr->mpos.hall.halledge_polskew_time_q26 = fix16_mul(motor_ptr->mpos.hall.halledge_polskew_msec_gui_q16, QCONST(1.024f, Q16));
        break;
    }
#endif
    
    default:
        break;
    }
}


/**
 * @brief Use for processing hall FOC forward and reverse snap angle
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void hall_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case HallConfig_Istart:
        if (motor_ptr->state.main == STATE_MAIN_STARTUP)
        {
            // Set control variables
            motor_ptr->control.iqref_target_f16 = motor_ptr->startup.i_total_hall_gui;
        }
        break;

    case HallConfig_HallStateAngleRef_1_Fwd:
    case HallConfig_HallStateAngleRef_1_Rev:
    case HallConfig_HallStateAngleRef_2_Fwd:
    case HallConfig_HallStateAngleRef_2_Rev:
    case HallConfig_HallStateAngleRef_3_Fwd:
    case HallConfig_HallStateAngleRef_3_Rev:
    case HallConfig_HallStateAngleRef_4_Fwd:
    case HallConfig_HallStateAngleRef_4_Rev:
    case HallConfig_HallStateAngleRef_5_Fwd:
    case HallConfig_HallStateAngleRef_5_Rev:
    case HallConfig_HallStateAngleRef_6_Fwd:
    case HallConfig_HallStateAngleRef_6_Rev:
        hall_create_sequence_array(motor_ptr);
        hall_create_hallstate_angle(motor_ptr);
        break;
    default:
        break;
    }
}
#endif
