
/**
 * @file app_main_sm.c
 * @brief Application main state machine, use to control various motor control state flow
 */

//=============================================================================
// Copyright (C) 2019 - 2022, Qorvo, Inc.
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
#include "driver_funcs.h"
#include "param_funcs.h"
#include "fault.h"
#include "control_funcs.h"
#include "utils.h"
#include "mpos_funcs.h"
#include "mpos_config.h"

extern int Debug_Test_Number4; //by:owz


#ifdef ENABLE_OPD_CL
#include "opd_cl_funcs.h"
#endif

#ifdef ENABLE_OPDOL
#include "opd_ol_funcs.h"
#endif

#ifdef ENABLE_IPD
#include "ipd_config.h"
#endif

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#ifdef ENABLE_SIM
#include "sim_funcs.h"
#include "sim_config.h"
#endif

#if defined(ENABLE_FORCEANG)
#include "forceang_funcs.h"
#endif

#if defined(ENABLE_FLASH_RW)
#include "flashrw_funcs.h"
#endif

#if defined(ENABLE_BUZZ)
#include "buzz_funcs.h"
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    #include "startup_aligngo2est_funcs.h"
#elif ((MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL))
    #include "startup_hall_funcs.h"
#elif ((MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP))
    #include "startup_aligngo2qep_funcs.h"
#elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    #include "startup_aligngo2trap_funcs.h"
#endif


/**
 * @brief  This function use to FOC motor control state flow, FAULT, RUN, START, STOP, RESET, COAST, DISABLE
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
void statemachine_appmain(void)
{
    static Struct_Motor* motor_ptr = &motor;
	
//		  Debug_Test_Number4 = Debug_Test_Number4+1;
//	if(Debug_Test_Number4>10)
//	{
//		Debug_Test_Number4 = 0;
//	}

    // Handle terminal faults
    if (motor_ptr->fault.fault_terminal_flag)
    {
        // Clear flag
        motor_ptr->fault.fault_terminal_flag = 0;

        // Clear motor enable command
        motor_ptr->app.motor_enable_cmd = 0;

        // Send to terminal state
        __disable_irq();
        mpos_clear_fptr(motor_ptr);
        motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
        motor_ptr->state.main = STATE_MAIN_TERMINAL;
        __enable_irq();

        // Clear status
        motor_ptr->status.motor &= ~STATUS_MAIN_ALLSTATUS;

        // Disable motor and PWMDAC's
        m1_disable();
#ifdef ENABLE_DEBUG_PWMDAC
        pwmdac_disable();
#endif
    }
    // Handle state faults except when in terminal state
    if ((motor_ptr->fault.fault_state_flag) && (motor_ptr->state.main != STATE_MAIN_TERMINAL)
            // Wait until sm_main is not in one of the states below before processing fault so that power up sequence is not interrupted and bypassed
            && (motor_ptr->state.main != STATE_MAIN_RESET)
            && (motor_ptr->state.main != STATE_MAIN_POST)
            && (motor_ptr->state.main != STATE_MAIN_PRESTART)
            && (motor_ptr->state.main != STATE_MAIN_MID1))
    {
        // Clear flag
        motor_ptr->fault.fault_state_flag = 0;

        // Clear motor enable command
        motor_ptr->app.motor_enable_cmd = 0;

        // Send to FAULT state
        __disable_irq();
        mpos_clear_fptr(motor_ptr);
        motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
        motor_ptr->state.main = STATE_MAIN_FAULT;
        __enable_irq();

        // Clear status
        motor_ptr->status.motor &= ~STATUS_MAIN_ALLSTATUS;

        // Disable motor (Special handling for over-current fault)
        if (motor_ptr->fault.motor & FAULT_CAFE_OC)
            set_drivers_gpio();
        else
            m1_disable();
    }
    // Handle stop faults except when in terminal or fault states
    else if ((motor_ptr->fault.fault_stop_flag) && (motor_ptr->state.main != STATE_MAIN_TERMINAL) && (motor_ptr->state.main != STATE_MAIN_FAULT))
    {
        __disable_irq();

        // Clear flag
        motor_ptr->fault.fault_stop_flag = FALSE;

        // Clear motor enable command
        motor_ptr->app.motor_enable_cmd = 0;

        // Send to DISABLE state
        switch (motor_ptr->state.main)
        {
            case STATE_MAIN_STARTUP:
            case STATE_MAIN_RUN:
            case STATE_MAIN_COASTING:
                motor_ptr->state.main = STATE_MAIN_DISABLE;
                break;
            default:
                break;
        }

        __enable_irq();
    }

#if defined(ENABLE_BRAKE_WHILE_STOPPED)
    if ((motor_ptr->state.main == STATE_MAIN_STOPPED) && (motor_ptr->state.brake_stop_mode == 0))
    {
        motor_ptr->state.brake_stop_mode = 1;

        brake_while_stopped_enter(motor_ptr);
        //debugio1_on();
    }

    if ((motor_ptr->state.main != STATE_MAIN_STOPPED) && (motor_ptr->state.brake_stop_mode == 1))
    {
        motor_ptr->state.brake_stop_mode = 0;

        brake_while_stopped_exit(motor_ptr);
        //debugio1_off();
    }
#endif

    switch (motor_ptr->state.main)
    {
        case STATE_MAIN_RESET:
            __disable_irq();
            // Initialize variables for operation
            init_default_parameters(motor_ptr);
            motor_init(motor_ptr);

#if (MODULE_MPOS_HALL == ENABLED)
            hall_init(motor_ptr);
#endif

#if (MODULE_MPOS_TRAP == ENABLED)
            trap_init(motor_ptr);
#endif

#if (MODULE_MPOS_QEP == ENABLED)
            qep_init(motor_ptr);
#endif

#ifdef ENABLE_OPD_CL
            opdcl_init(motor_ptr);
#endif

#ifdef ENABLE_DEBUG_PWMDAC
            pwmdac_enable();
#endif

#if defined(ENABLE_POWERUP_AUTOSTART)
            motor_ptr->app.flag_first_start = 1;
#endif

#if defined(ENABLE_FLASH_RW)
            init_flash_at_pwr_up();
#endif

#if defined(ENABLE_BUZZ)
            init_buzz(motor_ptr);
#endif

            // Transition Main state machine to POST state
            motor_ptr->state.main = STATE_MAIN_POST;
            motor_ptr->state.post = STATE_POST_RESET;
            __enable_irq();
            break;

        case STATE_MAIN_POST:
            // Execute Power On Self Test until complete
            sm_mainsub_post(motor_ptr);
            // If POST is complete, transition to DISABLE state
            if (motor_ptr->state.post == STATE_POST_COMPLETE)
            {
                __disable_irq();
                mpos_clear_fptr(motor_ptr);
                motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;

                #if defined(ENABLE_QEP_LIN_FLASH_READ_POWERUP)
                flash_status_put_startread(TRUE);
                #endif
                motor_ptr->state.main = STATE_MAIN_DISABLE;
                __enable_irq();

                #ifdef ENABLE_DC_VOLTAGE_COMPENSATION
                // Need to put here otherwise the DC voltage can not be initialized
                read_dc_voltage_fast_init(motor_ptr);
                #endif
            }
            break;

        case STATE_MAIN_DISABLE:
//            debugio1_off();
            __disable_irq();
            // Disable the motor and PWM DACs
            m1_disable();

#if (MODULE_MPOS_HALL == ENABLED)
            // Hall disable routine
            hall_timer_disable();
            hall_int_hallport_disable();
#endif

#if defined(ENABLE_HALL_TUNING)
            motor_ptr->halltune.status = STATUS_HALLTUNE_READY;
#endif

#if defined(ENABLE_AUTOTUNE_PI)
            at_reset_state(motor_ptr);
#endif

            // Trigger a direction dependency reset in the stopped state
            motor_ptr->app.app_motor_direction = 0;

            // Next state and status
            motor_ptr->fault.fault_stop_flag = FALSE;
            motor_ptr->state.control = STATE_CONTROL_IDLE;
            motor_ptr->status.motor &= ~(STATUS_MAIN_ALLSTATUS);
            motor_ptr->state.main = STATE_MAIN_STOPPED;
            mpos_clear_fptr(motor_ptr);
            motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
            motor_ptr->status.motor |= STATUS_MOTOR_STOPPED;
            // Initialize sub-state machines
            motor_ptr->state.prestart = STATE_PRESTART_RESET;
            startup_reset_state(motor_ptr);

#if defined(TM_SIM_REVERSE)
            // Handle SIM_Reverse test mode
            if (motor_ptr->debug.testmode_enable != 0)
            {
                if (motor_ptr->app.app_motor_direction_cmd == 1)
                    motor_ptr->app.app_motor_direction_cmd = -1;
                else
                    motor_ptr->app.app_motor_direction_cmd = 1;
            }
#endif
            __enable_irq();
            break;

#ifdef ENABLE_SIMBRAKE_MOTORDISABLE
        case STATE_MAIN_BRAKE:
            // Exit brake state if SIM is complete
            if (motor_ptr->sim.status_sim & STATUS_SIM_COMPLETE)
            {
                motor_ptr->state.main = STATE_MAIN_DISABLE;
                config_control_dt(motor_ptr, motor_ptr->foc.dt_control_q26, motor_ptr->foc.dt_control_1div_q6, motor_ptr->foc.ctrl_divider_foc);
                motor_ptr->status.motor &= ~STATUS_MOTOR_BRAKING;
            }
            break;
#endif

        case STATE_MAIN_STOPPED:
#if defined(ENABLE_TM) && defined(ENABLE_TM_SIM)
            // Handle testmode
            if (motor_ptr->debug.testmode_enable != 0)
            {
                motor_ptr->app.motor_enable_cmd = 1;
            }
#endif
            // Logic to reverse direction only when the motor is stopped
            if (motor_ptr->app.app_motor_direction != motor_ptr->app.app_motor_direction_cmd)
            {
                motor_ptr->app.app_motor_direction = motor_ptr->app.app_motor_direction_cmd;
                if (motor_ptr->app.app_motor_direction == -1)
                    motor_ptr->status.motor |= STATUS_MOTOR_REVERSE;
                else
                    motor_ptr->status.motor &= ~STATUS_MOTOR_REVERSE;

                // Update variables dependent on direction
                control_param_process(motor_ptr, outerloop_pid_min);
                control_param_process(motor_ptr, iq_ref_cl);
                control_param_process(motor_ptr, closed_loop_speed_setpoint);
            }

#if defined(ENABLE_HALL_TUNING)
            halltune_process_cmd_enable(motor_ptr);
#endif

#if defined(ENABLE_AUTOTUNE_MID)
            mid1_process_motor_enable(motor_ptr);
#endif

#if defined(ENABLE_POWERUP_AUTOSTART)
            if (motor_ptr->app.flag_first_start)
            {
                motor_ptr->app.motor_enable_cmd = 1;
                motor_ptr->app.flag_first_start = 0;
            }
#endif

#if defined(ENABLE_FORCEANG)
            if ((motor_ptr->app.motor_enable_cmd) && motor_ptr->forceang.forceang_enable_cmd)
            {
                forceang_state_enter(motor_ptr);
            }
            else
#endif
            if (motor_ptr->app.motor_enable_cmd)
            {
                // Wait for the motor enable command and then transition to the STARTUP state
                //debugio1_off();
                __disable_irq();
                motor_ptr->state.main = STATE_MAIN_PRESTART;
                motor_ptr->status.motor |= STATUS_MOTOR_ENABLED;
                motor_ptr->status.motor &= ~STATUS_MOTOR_STOPPED;
                __enable_irq();
            }

#if defined(ENABLE_BUZZ)
            // Handle buzz start command
            if (motor_ptr->buzz.cmd_buzz.start)
            {
                motor_ptr->state.main = STATE_MAIN_BUZZ;
            }
#endif

#if defined(ENABLE_FLASH_RW)
            // Detect FlashRW command. This code need to execute at the end of the STOPPED state so that it can override any other state changes in this state.
            flashrw_state_enter(motor_ptr);
#endif
            break;

        case STATE_MAIN_PRESTART:
            // Execute Startup state machine until complete
            sm_mainsub_prestart();
            // If startup is complete and no faults, then go to Open Loop; otherwise go to Disable state
            if (motor_ptr->state.prestart == STATE_PRESTART_COMPLETE)
            {
                if (motor_ptr->status.prestart == STATUS_PRESTART_COMPLETED)
                {
                    __disable_irq();
                    motor_ptr->state.main = STATE_MAIN_STARTUP;
                    __enable_irq();
                }
                else if (motor_ptr->status.prestart == STATUS_PRESTART_FAILED)
                {
                    motor_ptr->app.motor_enable_cmd = 0;
                    motor_ptr->state.main = STATE_MAIN_DISABLE;
                }
            }
            break;

        case STATE_MAIN_STARTUP:
            // Call run state machine
            startup_controller(motor_ptr);

            // Next state
            if (startup_complete(motor_ptr))
            {
                motor_ptr->state.main = STATE_MAIN_RUN;
            }
            else if (!motor_ptr->app.motor_enable_cmd)
                motor_ptr->state.main = STATE_MAIN_DISABLE;
            break;

        case STATE_MAIN_RUN:
            // If motor is no longer enabled, transition to the COASTING state
            if (!motor_ptr->app.motor_enable_cmd)
            {
                __disable_irq();
#ifdef ENABLE_SIM
                //JAGTAG: Move this to inside SIM
                motor_ptr->sim.sim_spd_scaled_q28 = motor_ptr->mpos.mpos_spd_scaled_q28;

#endif
#ifdef ENABLE_COASTING
                // Need to write ramp as well to bypass ramp, in case iqref was at ripple peak when stop is commanded
                motor_ptr->control.iqref_target_f16 = motor_ptr->foc.iqref_ramp_q14 = motor_ptr->foc.iqref_ramp_f16 = 0;
                motor_ptr->app.coast_timeout_counter = 0;
                motor_ptr->status.motor |= STATUS_MOTOR_CL_RAMPDOWN;
                motor_ptr->state.main = STATE_MAIN_COASTING;
                motor_ptr->state.control = STATE_CONTROL_TORQUE;
#elif defined(ENABLE_SIMBRAKE_MOTORDISABLE)
                // Disable gate drivers
                m1_disable();
                // Reset sim state machine
                sim_smstate_reset(motor_ptr);
                // Initialize SIM speed
                motor_ptr->sim.sim_spd_scaled_q28 = 0;
                // Next state and configure sequencers and control divider for SIM mode
                motor_ptr->mpos.state = STATE_MPOS_IDLE;
                motor_ptr->state.control = STATE_CONTROL_IDLE;
                sim_config(motor_ptr);
                motor_ptr->state.appcontrol = STATE_APPCONTROL_SIM_RUN;
                motor_ptr->state.main = STATE_MAIN_BRAKE;
                motor_ptr->status.motor |= STATUS_MOTOR_BRAKING;
#else
                // Next state
                motor_ptr->state.main = STATE_MAIN_DISABLE;
#endif
                __enable_irq();
            }
            else
            {
                // Switch to speed mode if motor is in torque mode and speed mode is commanded
                if ((motor_ptr->app.speed_mode_flag) && (motor_ptr->state.control != STATE_CONTROL_OUTERLOOP))
                {
                    __disable_irq();
                    // Initialize outer loop before switching to CL mode
                    init_pi(&motor_ptr->control.outerloop_pid, motor_ptr->foc.iqref_ramp_q14);
                    update_outerloop_ramp_sw(motor_ptr);
                    motor_ptr->state.control = STATE_CONTROL_OUTERLOOP;
                    __enable_irq();
                }
                // Switch to torque mode if motor is in speed mode and torque mode is commanded
                else if ((!motor_ptr->app.speed_mode_flag) && (motor_ptr->state.control != STATE_CONTROL_TORQUE))
                {
                    __disable_irq();
                    motor_ptr->foc.iqref_ramp_f16 = (motor_ptr->foc.iqref_ramp_q14 << 2);
                    motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16 * motor_ptr->app.app_motor_direction;
                    motor_ptr->state.control = STATE_CONTROL_TORQUE;
                    __enable_irq();
                }

#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
                hall_hall2est_controller(motor_ptr);
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
                qep_qep2est_controller(motor_ptr);
#elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
                trap_trap2est_controller(motor_ptr);
#endif

#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
                // Check if the stall is detected, if so, to repeat the startup

                // To do: Check how to better implement stall detection
                // To do: This could be replaced by inductance sensing
                if (absval(motor_ptr->mpos.mpos_spd_radps) < motor_ptr->mpos.aligngo.speedrefol_target_radps_q16)
                trap_stall_detection(motor_ptr);
#endif
            }
            break;

#ifdef ENABLE_COASTING
        case STATE_MAIN_COASTING:
            // Update outer loop ramp and feedback (for stall detection and display)
            update_outerloop_ramp_sw(motor_ptr);
            update_outerloop_fdbk_scaled(motor_ptr);
            // Handle case when motor is re-enabled while coasting but direction was changed
#ifdef ENABLE_NEGATIVE_IQ_COASTING
#error: ENABLE_NEGATIVE_IQ_COASTING feature not supported
            // ENABLE_NEGATIVE_IQ_COASTING is not supported due to incompatibility with the negative speed implementation
            if ((motor_ptr->app.motor_enable_cmd) && (motor_ptr->app.app_motor_direction_cmd != motor_ptr->app.app_motor_direction))
            {
                // Need to write ramp as well to bypass ramp, in case iqref was at ripple peak when stop is commanded
                motor_ptr->control.iqref_target_f16 = fix16_mul((0 - motor_ptr->control.outerloop_pid.max_value), NEGATIVE_IQ_CURRENT_PER_FOR_COAST);
                // Check if iqrefcl_target is +ve than set it to 0
                if (motor_ptr->control.iqref_target_f16 > 0)
                    motor_ptr->control.iqref_target_f16 = 0;
                motor_ptr->control.idiqref_rate_f16 = fix16_mul(motor_ptr->control.idiqref_rate_gui_f16, IDIQ_RAMP_RATE_PER_FOR_COAST);
                // Check if idiqref_rate is 0 than set it to default
                if (motor_ptr->control.idiqref_rate_f16 <= 0)
                    motor_ptr->control.idiqref_rate_f16 = DEFAULT_IDIQ_RAMP_RATE_PER_FOR_COAST;
            }

            // Handle case when motor is re-enabled while coasting
            if ((motor_ptr->app.motor_enable_cmd) && (motor_ptr->app.app_motor_direction_cmd == motor_ptr->app.app_motor_direction))
#else
            // Handle case when motor is re-enabled while coasting
            if (motor_ptr->app.motor_enable_cmd)
#endif
            {
                __disable_irq();
                if (absval(motor_ptr->mpos.mpos_spd_radps) >= conv_ol2cl_speed(motor_ptr->control.dt_control_1div_q6, motor_ptr->startup.aligngo.speedrefol_cmd_q26))
                {
                    motor_ptr->status.motor &= ~STATUS_MOTOR_CL_RAMPDOWN;
                    motor_ptr->state.main = STATE_MAIN_RUN;
                    if (motor_ptr->app.speed_mode_flag)
                    {
                        update_outerloop_ramp_sw(motor_ptr);
                        motor_ptr->state.control = STATE_CONTROL_OUTERLOOP;
                    }
                    else
                    {
                        motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16 * motor_ptr->app.app_motor_direction;
                        motor_ptr->state.control = STATE_CONTROL_TORQUE;
                    }
                }
                else
                    motor_ptr->state.main = STATE_MAIN_DISABLE;
#ifdef ENABLE_NEGATIVE_IQ_COASTING
                // Restore value because it is overwritten by coast mode
                motor_ptr->control.idiqref_rate_f16 = motor_ptr->control.idiqref_rate_gui_f16;
#endif
                __enable_irq();
            }
            // Check if estimated speed less than switchover speed then disable motor
            else if (absval(motor_ptr->mpos.mpos_spd_radps) < conv_ol2cl_speed(motor_ptr->control.dt_control_1div_q6, motor_ptr->startup.aligngo.speedrefol_cmd_q26))
            {
                motor_ptr->state.main = STATE_MAIN_DISABLE;
            }
            // If coast has timed out, then go to DISABLE state
            else if (motor_ptr->app.coast_timeout_counter++ >= motor_ptr->app.coast_timeout_target)
            {
#ifdef ENABLE_NEGATIVE_IQ_COASTING
                if (!motor_ptr->app.motor_enable_cmd)
                    set_fault(FAULT_COAST_TIMEOUT);
#else
                set_fault(FAULT_COAST_TIMEOUT);
#endif
                motor_ptr->state.main = STATE_MAIN_DISABLE;
            }
            break;
#endif

        case STATE_MAIN_FAULT:
            // Exit fault state if all fault conditions are clear
            if ((motor_ptr->fault.motor & FAULT_MAIN_GRP_STATE) == 0)
            {
                __disable_irq();
                motor_ptr->state.main = STATE_MAIN_DISABLE;
                __enable_irq();
            }
            break;

        case STATE_MAIN_TERMINAL:
            break;

#if defined(ENABLE_HALL_TUNING)
        case STATE_MAIN_HALLTUNE:
            // Hall tuning controller
            halltune_controller(motor_ptr);

            // Next state
            halltune_process_cmd_disable(motor_ptr);
            break;
#endif

#if defined(ENABLE_AUTOTUNE_MID)
        case STATE_MAIN_MID1:
            // MID1 main state machine
            mid1_main_subsm(motor_ptr);

            // Next state
            if (motor_ptr->atune.state_mainsub_mid1 & STATE_MAINSUB_MID1_COMPLETE)
//            if (motor_ptr->atune.status_atune & (STATUS_ATUNE_MID1_COMPLETE | STATUS_ATUNE_MID1_FAILED))
            {
                // Next state
                motor_ptr->state.main = STATE_MAIN_DISABLE;
            }
            break;
#endif

#if defined(ENABLE_FORCEANG)
        case STATE_MAIN_FORCEANG:
            forceang_controller(motor_ptr);

            // Next state
            forceang_state_exit(motor_ptr);
            break;
#endif

#if defined(ENABLE_FLASH_RW)
        case STATE_MAIN_FLASHRW:
            // Run flash RW controller
            flashrw_controller(motor_ptr);

            // Next state
            flashrw_state_exit(motor_ptr);
            break;
#endif

#if defined(ENABLE_BUZZ)
        case STATE_MAIN_BUZZ:
            // Buzz state machine
            statemachine_buzz(motor_ptr);

            // Next state
            if (motor_ptr->buzz.state_buzz == STATE_BUZZ_DONE)
            {
                motor_ptr->buzz.state_buzz = STATE_BUZZ_IDLE;
#ifdef ENABLE_BRAKE_WHILE_STOPPED
                motor_ptr->state.main = STATE_MAIN_BRAKE_WHILE_STOPPED_INIT;
#else
                motor_ptr->state.main = STATE_MAIN_DISABLE;
#endif
            }
            break;
#endif

        default:
            // If the state is not one of the above, then it's an error, so go to DISABLE state
            if (motor_ptr->app.motor_enable_cmd)
            {
                __disable_irq();
                motor_ptr->app.motor_enable_cmd = 0;
                motor_ptr->state.main = STATE_MAIN_DISABLE;
                __enable_irq();
            }
            break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF
