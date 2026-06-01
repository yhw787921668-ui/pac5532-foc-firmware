
/**
 * @file control_loop.c
 * @brief Use for FOC control functions
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

#include "control_loop.h"
#include "app_control_sm.h"

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#ifdef ENABLE_DEBUG_DISPLAY
#include "debug_display.h"
#endif

#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
#include "control_check.h"
#endif

#ifdef ENABLE_DEBUG_DAS
#include "debug_das.h"
#endif

#ifdef ENABLE_DEBUG_RTT
#include "USER_RTT.h"
#endif
#ifdef ENABLE_DEBUG_SAMPLE_RAM
#include "debug_capture.h"
#endif

#if (VMS_DUTY_CYCLE_EXCLUSION == TRUE)
#include "vbus_funcs.h"
#endif

/**
 * @brief This function use to call main FOC control loop
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void control_loop(Struct_Motor* motor_ptr)
{
    pre_control_loop(motor_ptr);

    if (motor_ptr->control.ctrl_counter >= (motor_ptr->control.ctrl_divider))
    {
        //debugio1_on();

#if (INTERNAL_VMS == TRUE) && (VMS_DUTY_CYCLE_EXCLUSION == TRUE)
       check_duty_cycle_exclusion(motor_ptr);
#endif


#if defined (ENABLE_SIM)
        motor_ptr->sim.ctrl_counter_sim2cl = motor_ptr->control.ctrl_counter;
#endif
        // Reset TA counter
        motor_ptr->control.ctrl_counter = 0;

        // Determine rotor angle
        mpos_calculator(motor_ptr);

        // Call state machine
        statemachine_appcontrol();

#ifdef ENABLE_DEBUG_DISPLAY
        // Debug Display
        run_debug_display();
#endif

#ifdef ENABLE_DEBUG_PWMDAC
        // PWM DAC's
        run_debug_pwmdac();
#endif

#ifdef ENABLE_DEBUG_DAS
        // PWM DAS DAC's
        run_debug_das();
#endif

#ifdef ENABLE_DEBUG_RTT
        run_debug_rtt();
#endif
#ifdef ENABLE_DEBUG_SAMPLE_RAM
        run_debug_capture_sample(motor_ptr);
#endif

#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
        check_foc_time_overflow(motor_ptr);
#endif
        //debugio1_off();
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

