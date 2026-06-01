
/**
 * @file app_mainsub_sm.c
 * @brief App main sub state machine, use to control ADC Calibration, pre PRESTART_RESET, SIM, IPD states
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
#include "param_funcs.h"
#include "app_funcs.h"
#include "driver_funcs.h"
#include "fault.h"
#include "control_funcs.h"
#include "mpos_funcs.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "mpos_config.h"

#ifdef ENABLE_SIM
#include "sim_funcs.h"
#include "sim_config.h"
#endif

#ifdef ENABLE_IPD
#include "ipd.h"
#include "ipd_config.h"
#include "ipd_funcs.h"
#endif

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#ifdef ENABLE_OPDOL
#include "opd_ol_funcs.h"
#endif

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#include "control_power_funcs.h"
#endif

#if defined(FAMILY_55XX) && defined (ENABLE_ADCCAL_FACTORY)
#include "adccal.h"
#endif


PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting

/**
 * @brief This function use to FOC motor control state flow, FAULT, RUN, START, STOP, RESET, COAST, DISABLE
 */
void sm_mainsub_prestart(void)
{
    static Struct_Motor* motor_ptr = &motor;

    switch(motor_ptr->state.prestart)
    {
        case STATE_PRESTART_RESET:
            __disable_irq();
//            debug_dac_1a = QCONST(1.0f, Q14);
//            debugio1_on();
            motor_reset(motor_ptr);
            motor_ptr->status.prestart = STATUS_PRESTART_RUNNING;
            foc_deconfig(motor_ptr);
#ifdef ENABLE_SIM
            sim_smstate_reset(motor_ptr);
            sim_config(motor_ptr);
            motor_ptr->state.appcontrol = STATE_APPCONTROL_SIM_RUN;
            motor_ptr->state.prestart = STATE_PRESTART_SIM_RUN;
#else
            motor_ptr->state.prestart = STATE_PRESTART_IPD_INIT;
#endif
            __enable_irq();
//            debugio1_off();
            break;

        case STATE_PRESTART_SIM_RUN:
        {
#ifdef ENABLE_SIM
            if ((motor_ptr->sim.status_sim & STATUS_SIM_COMPLETE) && (!(motor_ptr->sim.status_sim & STATUS_SIM_SWITCH2CL)))
            {
//                debug_dac_1a = QCONST(0.0f, Q14);
//                debugio1_off();
                // SIM exit procedure
                sim_exit_main(motor_ptr);
                sim_deconfig(motor_ptr, 1);
                mpos_clear_fptr(motor_ptr);
                // Next state
                __disable_irq();
                motor_ptr->state.prestart = STATE_PRESTART_IPD_INIT;
                motor_ptr->state.appcontrol = STATE_APPCONTROL_IDLE;
                __enable_irq();
            }
            else if ((motor_ptr->sim.status_sim & STATUS_SIM_COMPLETE) && (motor_ptr->sim.status_sim & STATUS_SIM_SWITCH2CL))
            {
//                debug_dac_1a = QCONST(0.0f, Q14);
//                debugio1_off();
                sim_sim2cl_transition_main(motor_ptr);

                // Next state if transitions are complete
                if (motor_ptr->sim.transition_complete)
                {
                    if (!motor_ptr->app.speed_mode_flag)
                        motor_ptr->control.iqref_target_f16 = motor_ptr->app.iqrefcl_cmd_f16 * motor_ptr->app.app_motor_direction;
                    motor_ptr->control.idref_target_f16 = motor_ptr->app.idrefcl_cmd_f16;

                    __disable_irq();
                    motor_ptr->state.main = STATE_MAIN_RUN;
                    motor_ptr->state.control = STATE_CONTROL_TORQUE;
                    sim_sim2cl_transition_exit_main(motor_ptr);
                    __enable_irq();
                }
            }
#endif  // ENABLE_SIM
        }
            break;

        case STATE_PRESTART_IPD_INIT:
        {
            __disable_irq();
#ifdef ENABLE_IPD
            motor_ptr->state.prestart = STATE_PRESTART_IPD_RUN;
            mpos_clear_fptr(motor_ptr);
            motor_ptr->state.appcontrol = STATE_APPCONTROL_IPD;
            ipd_reset();
#else
            motor_ptr->state.prestart = STATE_PRESTART_COMPLETE;
            motor_ptr->status.prestart = STATUS_PRESTART_COMPLETED;
#endif
            __enable_irq();
        }
            break;

        case STATE_PRESTART_IPD_RUN:
        {
#ifdef ENABLE_IPD
            // If IPD Is done, then Start FOC with Angle from IPD
            if (motor_ptr->ipd.ipd_state == IPD_DONE)
            {
                __disable_irq();
#ifdef ENABLE_IPD_OPD
                // If IPD detected phase lost
                if (motor_ptr->ipd.status & IPD_PHASE_LOST_DETECTED)
                {
                    set_fault(FAULT_OPD);
                    motor_ptr->status.prestart = STATUS_PRESTART_FAILED;
                    motor_ptr->state.prestart = STATE_PRESTART_COMPLETE;
                }
                else
#endif
                {
                    motor_ptr->status.prestart = STATUS_PRESTART_COMPLETED;
                    motor_ptr->state.prestart = STATE_PRESTART_COMPLETE;
                }
                ipd_exit();
                __enable_irq();
            }
#endif
        }
            break;

        case STATE_PRESTART_COMPLETE:
//            debug_dac_1a = QCONST(0.0f, Q14);
            break;

        default:
            break;
    }
}

PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use to initialize FOC timing, ADC calibration
 * @param Struct_Motor* motor_ptr motor pointer
 */
void sm_mainsub_post(Struct_Motor* motor_ptr)
{
    switch (motor_ptr->state.post)
    {
        case STATE_POST_RESET:
            // Configure control loop for FOC mode
            foc_config(motor_ptr, 1);
            motor_ptr->state.post = STATE_POST_DEVID;
            break;

        case STATE_POST_DEVID:
#ifdef ENABLE_DEVICE_ID_CHECK
            check_devid(motor_ptr);
#endif
           motor_ptr->state.post = STATE_POST_ADCCAL_FACTORY;
           break;

        case STATE_POST_ADCCAL_FACTORY:
#if defined(FAMILY_55XX) && defined (ENABLE_ADCCAL_FACTORY)
            // Read ADC factory offset error
            read_devinfo_adc_offset(motor_ptr);

            // Read ADC factory gain error
            read_devinfo_adc_gain(motor_ptr);

            // Next state
            if (motor_ptr->fault.fault_terminal & FAULT_TERMINAL_ADCCAL_FACTORY)
                motor_ptr->state.post = STATE_POST_COMPLETE;
            else
                motor_ptr->state.post = STATE_POST_ADCCAL_START;
#else
            motor_ptr->state.post = STATE_POST_ADCCAL_START;
#endif
           break;

        case STATE_POST_ADCCAL_START:
#ifdef ENABLE_ADCCAL_POWERUP
            __disable_irq();
            motor_ptr->state.post = STATE_POST_ADCCAL_RUN;
            motor_ptr->state.adccal = STATE_ADCCAL_RESET;
            mpos_clear_fptr(motor_ptr);
            motor_ptr->state.appcontrol = STATE_APPCONTROL_ADCCAL;
            __enable_irq();
#else
            set_default_adc_offset(motor_ptr);
            read_adc_offsets(motor_ptr);
            motor_ptr->state.post = STATE_POST_COMPLETE;
#endif
            break;

#ifdef ENABLE_ADCCAL_POWERUP
        case STATE_POST_ADCCAL_RUN:
           // Check status of ADC Offset Calibration, is completed yet
           if (motor_ptr->state.adccal == STATE_ADCCAL_COMPLETE)
           {
               read_adc_offsets(motor_ptr);
               __disable_irq();
               motor_ptr->state.post = STATE_POST_COMPLETE;
               __enable_irq();
           }
           break;
#endif

        case STATE_POST_COMPLETE:
           break;

        default:
           break;
    }
}
