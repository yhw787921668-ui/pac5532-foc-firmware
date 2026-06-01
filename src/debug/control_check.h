
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

#ifndef CONTROL_CHECK_H
#define CONTROL_CHECK_H

#include "motor_struct.h"
#include "fault.h"
#include "isr_funcs.h"

#ifdef ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void check_foc_time_overflow(Struct_Motor* motor_ptr)
{
    if (motor_ptr->fptr.fptr_int_check() && 
        (motor_ptr->control.ctrl_counter >= (motor_ptr->control.ctrl_divider)))
    {
        __disable_irq();
        motor_ptr->control.ctrl_counter = 0;
        set_warning(WARNING_FOCCTRL_OVERFLOW);
        __enable_irq();
    }
    motor_ptr->fptr.fptr_int_clr_0();
 }
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

#endif  //#ifndef CONTROL_CHECK_H
