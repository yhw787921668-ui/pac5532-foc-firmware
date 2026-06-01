/**
 * @file opd_ol_funcs.c
 * @brief Use for control functions in OPD
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

#include "opd_ol_funcs.h"
#include "opd_ol_config.h"
#include "fault.h"
#include "config_features.h"
#include "utils.h"

#ifdef ENABLE_OPDOL

//===================================================
//=====  State Machine: statemachine_opdol_main =====
//===================================================
void statemachine_opdol_main(Struct_Motor* motor_ptr)
{
    Struct_OPDOL* opdol_ptr = &motor_ptr->opdol;

    switch(opdol_ptr->state_opdol_main)
    {
        case STATE_OPDOL_MAIN_IDLE:
            break;

        case STATE_OPDOL_MAIN_START:
            // Need idref = 0 for correct OPD detection
            motor_ptr->mpos.aligngo.idrefol_target_q16 = 0;
            // Next state
            __disable_irq();
            opdol_ptr->state_opdol_ctl = STATE_OPDOL_CTL_START;
            opdol_ptr->state_opdol_main = STATE_OPDOL_MAIN_SAMPLING;
            __enable_irq();
            break;

        case STATE_OPDOL_MAIN_SAMPLING:
            if (opdol_ptr->status_opdol == STATUS_OPDOL_SAMPLEDONE)
            {
                __disable_irq();
                opdol_ptr->state_opdol_main = STATE_OPDOL_MAIN_SAMPLEDONE;
                __enable_irq();
            }
            break;

        case STATE_OPDOL_MAIN_SAMPLEDONE:
            check_opdol_fault(opdol_ptr, (motor_ptr->foc.iqref_ramp_q14 * motor_ptr->app.app_motor_direction));
            if ((motor_ptr->opdol.status_opdol & 0x0F) == STATUS_OPDOL_FAIL)
            {
                set_fault(FAULT_OPD);
            }
            __disable_irq();
            opdol_ptr->state_opdol_main = STATE_OPDOL_MAIN_COMPLETE;
            __enable_irq();
            break;

        case STATE_OPDOL_MAIN_COMPLETE:
            break;

        default:
            __disable_irq();
            opdol_ptr->state_opdol_main = STATE_OPDOL_MAIN_IDLE;
            __enable_irq();
            break;
    }
}

//=================================================
//===== State Machine: statemachine_opdol_ctl =====
//=================================================
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void statemachine_opdol_ctl(Struct_Motor* motor_ptr)
{
    Struct_OPDOL* opdol_ptr = &motor_ptr->opdol;
    static volatile uint32_t ol_opd_counter = 0;

    switch(opdol_ptr->state_opdol_ctl)
    {

    case STATE_OPDOL_CTL_IDLE:
        break;

    case STATE_OPDOL_CTL_START:
        opdol_ptr->opdol_Sum_Iw = opdol_ptr->opdol_Sum_Iv = opdol_ptr->opdol_Sum_Iu = 0;
        ol_opd_counter = 0;
        opdol_ptr->state_opdol_ctl = STATE_OPDOL_CTL_IQRAMPING;
        opdol_ptr->status_opdol = STATUS_OPDOL_SAMPLING;
        break;

    case STATE_OPDOL_CTL_IQRAMPING:
        // Use average Iqref/Idref in case they are very in balanced
        if (absval(motor_ptr->foc.iqref_ramp_f16) >= ((motor_ptr->mpos.aligngo.itotrefol_target_q16 + motor_ptr->mpos.aligngo.idrefol_target_q16) >> 1))
        {
            opdol_ptr->state_opdol_ctl = STATE_OPDOL_CTL_DELAY_STARTSAMPLE;
        }
        break;

    case STATE_OPDOL_CTL_DELAY_STARTSAMPLE:
        if (ol_opd_counter++ > opdol_ptr->olopd_start_sample_delay)
        {
            opdol_ptr->state_opdol_ctl = STATE_OPDOL_CTL_SAMPLING;
            ol_opd_counter = 0;
        }
        break;

    case STATE_OPDOL_CTL_SAMPLING:
        store_current_opdol(opdol_ptr, motor_ptr->control.Iu_measured_q14, motor_ptr->control.Iv_measured_q14, motor_ptr->control.Iw_measured_q14);
        if (ol_opd_counter++ > opdol_ptr->olopd_total_samples)
        {
            opdol_ptr->state_opdol_ctl = STATE_OPDOL_CTL_SAMPLEDONE;
            opdol_ptr->status_opdol = STATUS_OPDOL_SAMPLEDONE;
        }
        break;

    case STATE_OPDOL_CTL_SAMPLEDONE:
        break;

    default:
        break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

void init_opdol(Struct_Motor* motor_ptr)
{
    motor_ptr->opdol.olopd_start_sample_delay = OLOPD_STARTSAMPLEDELAY;
    motor_ptr->opdol.olopd_total_samples = OLOPD_TOTALSAMPLES;
}

void reset_opdol(Struct_Motor* motor_ptr)
{
    // Initialize OPDOL state machines and status
    motor_ptr->opdol.state_opdol_ctl = STATE_OPDOL_CTL_IDLE;
    motor_ptr->opdol.status_opdol = STATUS_OPDOL_IDLE;
    motor_ptr->opdol.state_opdol_main = STATE_OPDOL_MAIN_START;
//    // Set initial angle to pi/2 so that all phases have current during OPD
//    motor_ptr->mpos.aligngo.alignangle_q26 = (FIX16_PI_DIV_2 << 10);
}

#endif  //#ifdef ENABLE_OPDOL
