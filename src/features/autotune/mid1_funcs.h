/**
 * @file mid1_funcs.h
 * @brief Use for auto-tuning function definition in motor identification 1
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

#ifndef MID1_FUNCS_H
#define MID1_FUNCS_H

#include "motor_struct.h"
#include "driver_funcs.h"
#include "control_funcs.h"
#include "foc_math.h"
#include "atune_libfuncs.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mod_svm.h"
#include "aligngo_funcs.h"
#include "mpos_config.h"


#if defined(ENABLE_AUTOTUNE_MID)

#define MID1_CURRENT_LIMIT1_PERCENT_Q14         QCONST(0.03f, Q14)
#define MID1_CURRENT_LIMIT2_PERCENT_Q14         QCONST(0.04f, Q14)
#define MID1_MOTIONDET_DELTA_THRESHOLD_PCT_Q14  QCONST(0.05f, Q14)          // To test fail case (0.003f)
#define MID1_MOTIONDET_DELAY_LIMIT_SEC_Q16      QCONST(1.0f, Q16)
#define MID1_MOTIONDET_TO_LIMIT_SEC_Q16         QCONST(10.0f, Q16)
#define MID1_FILTER_SHIFT_Q0                    6
#define MID1_ANGLERAMPRATE_HZPS_Q16             QCONST(1.0f, Q16)
/**
 * @brief Initialize motor identification 1
 * @param Struct_Motor *motor_ptr
 */
void mid1_init(Struct_Motor *motor_ptr);

/**
 * @brief Enable the motor for motor identification 1
 * @param Struct_Motor *motor_ptr
 */
void mid1_process_motor_enable(Struct_Motor *motor_ptr);

/**
 * @brief Reset the state machine control function for motor identification 1
 * @param Struct_ATune *atune_ptr
 */
static inline void reset_amid_flow_state_machine (Struct_ATune *atune_ptr)
{
    atune_ptr->cmd_mid1_sm |= CMD_MID1SM_SETSTATE_RESET;
}

/**
 * @brief Set the state machine as IDLE for motor identification
 * @param Struct_ATune *atune_ptr
 */
static inline void set_amid_flow_state_machine_idle (Struct_ATune *atune_ptr)
{
    atune_ptr->cmd_mid1_sm |= CMD_MID1SM_SETSTATE_IDLE;
}

/**
 * @brief Set the state machine as EXIT for motor identification
 * @param Struct_ATune *atune_ptr
 */
static inline void set_amid_flow_sm_exit (Struct_ATune *atune_ptr)
{
    atune_ptr->cmd_mid1_sm |= CMD_MID1SM_SETSTATE_EXIT;
}

/**
 * @brief Get the state machine state for motor identification
 * @param Struct_ATune *atune_ptr
 */
static inline uint32_t get_amid_flow_state_machine_state (Struct_ATune *atune_ptr)
{
    atune_ptr->cmd_mid1_sm |= CMD_MID1SM_READSTATE;
    return (uint32_t) atune_ptr->state_mid1_external;
}

/**
 * @brief Get the status from motor identification 1
 * @param Struct_ATune *atune_ptr
 */
static inline eStatusMID1 get_amid_flow_status (Struct_ATune *atune_ptr)
{
    return atune_ptr->status_mid1;
}

/**
 * @brief Use to handle the control sequences for motor identification 1
 * @param Struct_Motor *motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void mid1_process_control(Struct_Motor *motor_ptr)
{
    // MID1 control algorithm
    mid1_process_control_lib(&motor_ptr->atune, motor_ptr->control.mod_va_q14, motor_ptr->control.mod_vb_q14, motor_ptr->control.mod_vc_q14, motor_ptr->control.Iu_measured_q14, motor_ptr->control.Iv_measured_q14, motor_ptr->control.Iw_measured_q14);
    motor_ptr->foc.vd_q14 = motor_ptr->atune.vd_q14;
    motor_ptr->foc.vq_q14 = motor_ptr->atune.vq_q14;

    // JAGTAG: Possibly eliminate this because it may not work as intended
#if defined(CONFIG_MID1_EQVABC)
    // Ensure that duty cycles are balanced between phases for resistance measurement
    if (motor_ptr->atune.mid1_eqvabc_flag)
    {
        if (motor_ptr->control.mod_va_q14 > 0)
        {
            motor_ptr->control.mod_vb_q14 = -motor_ptr->control.mod_va_q14;
            motor_ptr->control.mod_vc_q14 = -motor_ptr->control.mod_va_q14;
        }
        else if (motor_ptr->control.mod_vb_q14 > 0)
        {
            motor_ptr->control.mod_va_q14 = -motor_ptr->control.mod_vb_q14;
            motor_ptr->control.mod_vc_q14 = -motor_ptr->control.mod_vb_q14;
        }
        else if (motor_ptr->control.mod_vc_q14 > 0)
        {
            motor_ptr->control.mod_va_q14 = -motor_ptr->control.mod_vc_q14;
            motor_ptr->control.mod_vb_q14 = -motor_ptr->control.mod_vc_q14;
        }
    }
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief Use to handle the functions in systick timelevel for motor identification 1
 * @param Struct_Motor *motor_ptr
 */
static inline void mid1_process_systick(Struct_Motor *motor_ptr)
{
#if (INVERTER_TYPE_SEL == INVERTER_TYPE_MOS)
    mid1_calc_inverter_mos(&motor_ptr->atune, motor_ptr->app.dcbus_voltage_1div_q16, QCONST(INVERTER_MOS_VDIODE_VOLTS_FLT, Q14), motor_ptr->app.descale_current_factor, QCONST(INVERTER_MOS_RDSON_OHMS_FLT, Q16));
#elif (INVERTER_TYPE_SEL == INVERTER_TYPE_IGBT)
    mid1_calc_inverter_igbt(&motor_ptr->atune, motor_ptr->app.dcbus_voltage_1div_q16, QCONST(INVERTER_IGBT_VDIODE_VOLTS_FLT, Q14), QCONST(INVERTER_IGBT_VCE_VOLTS_FLT, Q14));
#endif

#if 0
    // Display results from manual mode
    switch (motor_ptr->atune.mid1_mancmd)
    {
    default:
    case MID1_MANCMD_IDLE:
        debug_var_1 = 0;
        debug_var_2 = 0;
        debug_var_3 = 0;
        debug_var_6 = 0;
        break;
    case MID1_MANCMD_MEAS_RD:
    case MID1_MANCMD_MEAS_RQ:
        debug_var_1 = motor_ptr->atune.v_dc_q14 << 2;
        debug_var_2 = motor_ptr->atune.i_dc_q14 << 2;
        debug_var_6 = motor_ptr->atune.rs_q16;
        break;
    case MID1_MANCMD_MEAS_LD:
        debug_var_1 = motor_ptr->atune.psq_app_out_q40 << 16;
        debug_var_2 = motor_ptr->atune.psq_real_out_q40 << 16;
        debug_var_3 = motor_ptr->atune.isq_rms_out_q28 << 16;
        debug_var_6 = motor_ptr->atune.ld_q26;
        break;
    case MID1_MANCMD_MEAS_LQ:
        debug_var_1 = motor_ptr->atune.psq_app_out_q40 << 16;
        debug_var_2 = motor_ptr->atune.psq_real_out_q40 << 16;
        debug_var_3 = motor_ptr->atune.isq_rms_out_q28 << 16;
        debug_var_6 = motor_ptr->atune.lq_q26;
        break;
    }
#endif
}

/**
 * @brief Use to handle the main state machine for motor identification 1
 * @param Struct_Motor *motor_ptr
 */
static inline void mid1_main_subsm(Struct_Motor *motor_ptr)
{
    // Disable motor is MID1 is started while motor is enabled
    if (motor_ptr->app.motor_enable_cmd)
        motor_ptr->app.motor_enable_cmd = 0;

    // State Machine
    switch(motor_ptr->atune.state_mainsub_mid1)
    {
    case STATE_MAINSUB_MID1_IDLE:
        break;

    case STATE_MAINSUB_MID1_RESET:
        // Configure control loop for FOC mode
        foc_config(motor_ptr, 1);
        mpos_clear_fptr(motor_ptr);

        // Reset forceang variables
        aligngo_reset_vars(motor_ptr);
        // Set forceang fixed command values
        motor_ptr->atune.aligngo.aligntime_cmd = 0;
        motor_ptr->atune.aligngo.speedrefol_cmd_q26 = fix16_mul(fix16_mul(MID1_ANGLERAMPRATE_HZPS_Q16, HZ2RAD_FIX16), motor_ptr->foc.dt_control_q26);
        motor_ptr->atune.aligngo.speedrefol_ramprate_cmd_q26 = 0;
        motor_ptr->atune.aligngo.itotrefol_cmd_q16 = 0;          // Current command comes from CL command
        motor_ptr->atune.aligngo.idrefol_cmd_q16 = motor_ptr->atune.aligngo.itotrefol_cmd_q16;
        // Set forceang variables
        aligngo_setvars(motor_ptr, &motor_ptr->atune.aligngo);
        // Set control variables
        motor_ptr->control.idiqref_rate_f16 = motor_ptr->mpos.aligngo.irefol_ramprate_q16;
        motor_ptr->control.idref_target_f16 = motor_ptr->mpos.aligngo.idrefol_target_q16;
        motor_ptr->control.iqref_target_f16 = motor_ptr->mpos.aligngo.iqrefol_target_q16;

        // Next state
        __disable_irq();
        motor_ptr->state.appcontrol = STATE_APPCONTROL_MOTOR_RUN;
        motor_ptr->atune.mid1_control_mode_flag = 1;
        motor_ptr->foc.state_foc = STATE_FOC_BACKENDFLOW;
        motor_ptr->mpos.state = STATE_MPOS_ALIGNGO;

        motor_ptr->atune.state_mainsub_mid1 = STATE_MAINSUB_MID1_RUN;
        // Set status
        motor_ptr->atune.status_atune &= ~(STATUS_ATUNE_MID1_ALLSTATUS | STATUS_ATUNE_MID1_ALLWARNINGS | STATUS_ATUNE_MID1_ALLFAULTS);
        motor_ptr->atune.status_atune |= STATUS_ATUNE_MID1_INPROGRESS;
        // Reset amid flow state machine and make it ready to go
        mid1_reset_subsm(&motor_ptr->atune);
        reset_amid_flow_state_machine(&motor_ptr->atune);
        // Reset FOC running variables
        foc_reset(motor_ptr);
        motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_ANGLERAMP;
        // Set FOC duty cycle to 50 percent
        motor_ptr->control.u_duty_value = motor_ptr->control.motor_period_ticks >> 1;
        motor_ptr->control.v_duty_value = motor_ptr->control.motor_period_ticks >> 1;
        motor_ptr->control.w_duty_value = motor_ptr->control.motor_period_ticks >> 1;
        m1_u_set_duty(motor_ptr->control.u_duty_value);
        m1_v_set_duty(motor_ptr->control.v_duty_value);
        m1_w_set_duty(motor_ptr->control.w_duty_value);
        m1_enable();
        __enable_irq();
        break;

    case STATE_MAINSUB_MID1_RUN:
        // Update MID1 Controller with Align&Go status
        if ((motor_ptr->atune.flag_waitexit == 0) && (motor_ptr->mpos.aligngo.alignangle_q26 == motor_ptr->mpos.aligngo.alignangle_target_q26))
        {
            motor_ptr->atune.flag_waitexit = 1;
        }

        // MID1 Controller
        mid1_controller(&motor_ptr->atune, &motor_ptr->app);

        // Update Align&Go parameters resulting from mid1_controller
        motor_ptr->app.app_motor_direction = motor_ptr->atune.mid1_angdirection;
        aligngo_setvars(motor_ptr, &motor_ptr->atune.aligngo);

        // MID1 Process
        mid1_process_systick(motor_ptr);

        // Next state if MID1 SM ends
        if ((motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO) || (motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO_MANCFG))
        {
            // Disable the motor when received amid flow complete status
            if (motor_ptr->atune.status_mid1 & (STATUS_MID1_COMPLETE | STATUS_MID1_FAILED))
            {
                // Next state
                motor_ptr->atune.state_mainsub_mid1 = STATE_MAINSUB_MID1_EXIT;
            }
        }

        // Disable the motor when received motor disable command and in manual mode
        if ((!motor_ptr->atune.mid1_motor_enable_cmd) && (motor_ptr->atune.mid1_runmode == MID1_RUNMODE_MANUAL))
        {
            motor_ptr->atune.state_mainsub_mid1 = STATE_MAINSUB_MID1_EXIT;
        }

        // Command handler
        if((!motor_ptr->atune.mid1_motor_enable_cmd) && !(motor_ptr->atune.status_mid1 & (STATUS_MID1_COMPLETE | STATUS_MID1_FAILED)))
        {
            // Next state
            __disable_irq();
            if ((motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO) || (motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO_MANCFG))
                set_amid_flow_sm_exit(&motor_ptr->atune);
            else
                motor_ptr->atune.state_mainsub_mid1 = STATE_MAINSUB_MID1_EXIT;
            __enable_irq();
        }
        break;

    case STATE_MAINSUB_MID1_EXIT:
        // Disable drivers and Next state
        __disable_irq();
        // JAGTAG: Clearing motor_enable_cmd is temporary until dedicated GUI control is added
        motor_ptr->atune.mid1_motor_enable_cmd = FALSE;
        motor_ptr->control.u_duty_value = motor_ptr->control.motor_period_ticks >> 1;
        motor_ptr->control.v_duty_value = motor_ptr->control.motor_period_ticks >> 1;
        motor_ptr->control.w_duty_value = motor_ptr->control.motor_period_ticks >> 1;
        motor_ptr->control.mod_va_q14 = 0;
        motor_ptr->control.mod_vb_q14 = 0;
        motor_ptr->control.mod_vc_q14 = 0;
        motor_ptr->mpos.aligngo.state_control = STATE_ALIGNGO_CONTROL_IDLE;
        // Next state
        set_amid_flow_state_machine_idle (&motor_ptr->atune);
        motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
        motor_ptr->atune.mid1_control_mode_flag = 0;
        motor_ptr->foc.state_foc = STATE_FOC_FULLFLOW;
        motor_ptr->mpos.state = STATE_MPOS_IDLE;

        motor_ptr->atune.state_mainsub_mid1 = STATE_MAINSUB_MID1_COMPLETE;
        // JAGTAG: Is m1_disable necessary here or is it taken care of in the main_sm?
        m1_disable();
        __enable_irq();
        // Report status
        motor_ptr->atune.status_atune &= ~STATUS_ATUNE_MID1_ALLSTATUS;
        if (motor_ptr->atune.status_mid1 & STATUS_MID1_COMPLETE)
            motor_ptr->atune.status_atune |= STATUS_ATUNE_MID1_COMPLETE;
        else if (motor_ptr->atune.status_mid1 & STATUS_MID1_FAILED)
            motor_ptr->atune.status_atune |= STATUS_ATUNE_MID1_FAILED;
        // Report faults and warnings
        if (motor_ptr->atune.status_mid1 & FAULT_MID1_RCHECK)
            motor_ptr->atune.status_atune |= FAULT_ATUNE_MID1_PHASECHK;
        else if (motor_ptr->atune.status_mid1 & WARNING_MID1_RCHECK)
            motor_ptr->atune.status_atune |= WARNING_ATUNE_MID1_PHASECHK;
        else if (motor_ptr->atune.status_mid1 & WARNING_MID1_MOTIONDET)
            motor_ptr->atune.status_atune |= WARNING_ATUNE_MID1_MOTIONDET;
        break;

    case STATE_MAINSUB_MID1_COMPLETE:
        break;

    default:
        break;
    }
}

#endif  //#if defined(ENABLE_AUTOTUNE_MID)
#endif  //#ifndef MID1_FUNCS_H
