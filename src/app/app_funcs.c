/**
 * @file app_funcs.c
 * @brief Use to reset variables associated to FOC, Control, Faults, Stall, OPD, PPM
 */

//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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
#include "app_funcs.h"
#include "foc.h"
#include "mpos_funcs.h"
#include "param_calc.h"
#include "fault.h"
#include "control_funcs.h"
#include "foc_funcs.h"
#include "isr_funcs.h"

#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined(ENABLE_DEBUG_RTT)
#include "debug_dac.h"
#endif

#ifdef ENABLE_DEBUG_DISPLAY
#include "debug_display.h"
#endif

#ifdef ENABLE_OPDOL
#include "opd_ol_funcs.h"
#endif

#ifdef ENABLE_STALL_DETECT
#include "stalldet_funcs.h"
#endif

#if (COMMSEL_PPM == TRUE)
#include "comm.h"
#endif

#ifdef ENABLE_IPD
#include "ipd_config.h"
#include "ipd.h"
#endif

#ifdef ENABLE_FLASH_CRC_TEST
#include "pac5xxx_flash_test.h"
#endif

#if defined(ENABLE_POWERMEASURE)
#include "control_power_funcs.h"
#endif

#if defined(ENABLE_AUTOTUNE_PI)
#include "atune_funcs.h"
#endif

#if defined(ENABLE_OPD_CL)
#include "opd_cl_funcs.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
#include "harmonic_inj_funcs.h"
#endif

#if defined(ENABLE_QEP_LINEARIZATION_TUNING)
#include "qep_funcs.h"
#endif

Struct_Motor motor;

void motor_reset(Struct_Motor* motor_ptr)
{
    foc_reset(motor_ptr);
    control_reset(motor_ptr);
    clear_fault(FAULT_MAIN_GRP_CLEARSTART);

    motor_ptr->app.app_motor_direction = motor_ptr->app.app_motor_direction_cmd;
    if (motor_ptr->app.app_motor_direction == -1)
        motor_ptr->status.motor |= STATUS_MOTOR_REVERSE;
    else
        motor_ptr->status.motor &= ~STATUS_MOTOR_REVERSE;

#ifdef ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP
    motor_ptr->mpos.aligngo.startup_volt_reach_target_flag = 0;
    motor_ptr->mpos.aligngo.startup_volt_ramp_q14 = 0;
    motor_ptr->mpos.aligngo.startup_volt_inc_q14 = VOL_RAMP_INC_Q14;
#endif

#if defined(ENABLE_STALL_DETECT)
    motor_ptr->app.vdrvsq_q14_peak_stored = 0;
    motor_ptr->app.vdrvsq_q14_peak = 0;
#endif

#ifdef ENABLE_STALL_DETECT
    stalldet_reset(motor_ptr);
#endif

#ifdef ENABLE_OPDOL
    init_opdol(motor_ptr);
#endif

#if (COMMSEL_PPM == TRUE)
    ppm_init(motor_ptr);
#endif

#ifdef ENABLE_IPD
    motor_ptr->ipd.ipd_state = IPD_DONE;
#endif

#if defined(ENABLE_POWERMEASURE)
    power_reset(motor_ptr);
#endif

#if defined(ENABLE_AUTOTUNE_PI)
    at_reset_state(motor_ptr);
#endif

#if defined(ENABLE_OPD_CL)
    opdcl_reset(motor_ptr);
#endif

#ifdef ENABLE_MTPA
    mtpa_reset(motor_ptr);
#endif

#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
    read_dc_voltage_fast_reset(motor_ptr);
#endif

#if defined(ENABLE_QEP_LINEARIZATION_TUNING)
    uint32_t temp_q16;
    // convert_rad_to_qep_count_q10 = QEP_PULSE_PER_RESOLUTION / (pole_pairs*2pi)
    temp_q16 = QEP_PULSE_PER_RESOLUTION * FIX16_1P0_DIV_2_PI;
    motor_ptr->mpos.qep.convert_rad_to_qep_count_q10 = fix16_div(temp_q16, (motor_ptr->mparam.pole_pairs << 16)) >> 6;
    motor_ptr->mpos.qep.qep_linearization_start_flag = FALSE;
    motor_ptr->mpos.qep.qep_linearization_done_flag =  FALSE;
#endif
}


/**
 * @brief This function use to initialize debug, application, hall and other system variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void systemvar_init(Struct_Motor* motor_ptr)
{
#ifdef ENABLE_DEBUG_DISPLAY
    test_ptr1 = (My_GUI_Debug_Q14*) &GUI_Debug_Var_Q14[0].ptr_gui_debug_value_q14;    // make sure pointer is pointing to array index 0 at power up
    test_ptr2 = (My_GUI_Debug_Q14*) &GUI_Debug_Var_Q14[0].ptr_gui_debug_value_q14;    // make sure pointer is pointing to array index 0 at power up
    test_ptr3 = (My_GUI_Debug_Q14*) &GUI_Debug_Var_Q14[0].ptr_gui_debug_value_q14;    // make sure pointer is pointing to array index 0 at power up
#endif

#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined(ENABLE_DEBUG_RTT)
    motor_ptr->debug.debug_buffer_command1 = 0;                         // make sure index never exceed array length
    motor_ptr->debug.debug_buffer_command2 = 0;                         // make sure index never exceed array length
    motor_ptr->debug.debug_buffer_command3 = 0;                         // make sure index never exceed array length
    motor_ptr->debug.debug_buffer_command4 = 0;                         // make sure index never exceed array length
    dac_ptr1 = (int32_t*) &DAC_ARRAY_Q14[0];                            // make sure pointer points to something
    dac_ptr2 = (int32_t*) &DAC_ARRAY_Q14[0];                            // make sure pointer points to something
    dac_ptr3 = (int32_t*) &DAC_ARRAY_Q14[0];                            // make sure pointer points to something
    dac_ptr4 = (int32_t*) &DAC_ARRAY_Q14[0];                            // make sure pointer points to something
#endif

    motor_ptr->state.main = STATE_MAIN_RESET;
#if defined(ENABLE_BRAKE_WHILE_STOPPED)
    motor_ptr->state.brake_stop_mode = 0;
#endif
    motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
    motor_ptr->app.current_limit = MOTOR_CURRENT_LIMIT_DEFAULT_VALUE;
#if (PUSER_VALUE_DIFFAMP_GAIN_FIXED)
    motor_ptr->app.iphase_amp_gain = UVW_PHASE_RSENSE_GAIN_DEFAULT_VALUE_5285;
#else
    motor_ptr->app.iphase_amp_gain = UVW_PHASE_RSENSE_GAIN_DEFAULT_VALUE;
#endif
    motor_ptr->app.systick_counter = 0;                                 // this is the timer that starts as soon ADC ISR called
    motor_ptr->app.systick_div_counter = 0;                             // this is the timer that starts as soon ADC ISR called
    motor_ptr->app.systick_divider_cnt = 0;                             // this is the timer that starts as soon ADC ISR called
    motor_ptr->app.motor_led_ticks = MOTOR1_LED_TICKS;                  // Timer ticks in LED for this motor (for SVM)
    motor_ptr->app.motor_ted_ticks = MOTOR1_TED_TICKS;                  // Timer ticks in TED for this motor (for SVM)
    motor_ptr->app.motor_enable_cmd = 0;
    motor_ptr->status.motor = (eStatusMotor) 0;
    motor_ptr->fault.fault_stop_flag = FALSE;
    motor_ptr->control.ctrl_counter = 0;

    foc_systemvar_init(motor_ptr);

    isr_fptr_init(motor_ptr);
    mpos_init(motor_ptr);

#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
    motor_ptr->debug.cl_speed_test_enable = FALSE;
    motor_ptr->debug.cl_speed_test_limit1 = FOC_CLOSE_LOOP_SPEED_SETPOINT_DEFAULT_VALUE;
    motor_ptr->debug.cl_speed_test_limit2 = FOC_CLOSE_LOOP_SPEED_SETPOINT_DEFAULT_VALUE;
#endif

#if (COMMSEL_PPM == TRUE)
    // Set power up flag, so we know system just power up, motor will not start until throttle will move back to minimum position
    motor_ptr->ppm.motor_powerup_flag = 1;
#endif

#ifdef ENABLE_FLASH_CRC_TEST
    // Get the results of the Reset Handler CRC Test; Passed = 0, Failed = 1
    // !!!!! Actions taken due to a Flash CRC Test failure are application specific; please modify accordingly !!!!!
    // In this case, the status is set so the FOC GUI can display the CRC failure
    if (uart_scratch_read() != 0)
        set_fault_terminal(FAULT_TERMINAL_FLASH_CRC_TEST);
#endif
}



/**
 * @brief This function use for copying peak value of vdrive and speed for app level processing 
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#if defined(ENABLE_STALL_DETECT)
void store_peak_speed_vdrive_value(Struct_Motor* motor_ptr)
{
    // Reset peak registers
    __disable_irq();
    motor_ptr->app.vdrvsq_q14_peak_stored = motor_ptr->app.vdrvsq_q14_peak;
    motor_ptr->app.vdrvsq_q14_peak = 0;

#ifdef ENABLE_STALL_DETECT
    motor_ptr->app.motor_speed_peak_stored = motor_ptr->app.motor_speed_peak;
    motor_ptr->app.motor_speed_peak = 0;
#endif

    __enable_irq();
}
#endif



/**
 * @brief This function use for copying OL target speed and CL max speed ref to app level variable 
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void app_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    case open_loop_speed_setpoint:
        motor_ptr->app.speedrefcl_min = absval(((motor_ptr->startup.aligngo.speedrefol_cmd_q26 >> 9) * (motor_ptr->foc.dt_control_1div_q6 >> 7))) >> 1;        // f16>>9=8q7 * f16>>7=6q9 = q16 (rad/s)
#elif (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP)
    case max_cl_speed_limit:
        motor_ptr->app.speedrefcl_min = -motor_ptr->app.speedrefcl_max;
#else
    #error "MPOS_SELECT value not valid."
#endif
        break;
    default:
        break;
    }
}

#if defined(ENABLE_BRAKE_WHILE_STOPPED)
void brake_while_stopped_enter(Struct_Motor* motor_ptr)
{
    __disable_irq();

    // Set gate driver IO's to GPIO mode & clear
    set_drivers_gpio();

    // Set LS GPIO's high
    motor_out_LSon();

    // Enable driver manager
    enable_driver();

    __enable_irq();
}


void brake_while_stopped_exit(Struct_Motor* motor_ptr)
{
    __disable_irq();

    // Disable driver manager
    pac5xxx_tile_register_write(ADDR_ENDRV, 0);

    // Set drivers to GPIO mode & clear
    set_drivers_gpio();

    __enable_irq();
}
#endif
