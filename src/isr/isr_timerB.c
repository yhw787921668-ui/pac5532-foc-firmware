/**
 * @file isr_timerB.c
 * @brief Use for timerB timer interrupt handler to run critical FOC functions
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

#include "isr_config.h"
#include "isr_funcs.h"
#include "motor_struct.h"

#if (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_B)
#include "comm.h"
#endif


/**
 * @brief Process interrupt for TimerB_IRQHandler to run critical FOC functions
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC void TimerB_IRQHandler(void)
{
//    debugio1_toggle();

#if defined(ENABLE_INT_TB_BASE) || defined(ENABLE_INT_TB_CCR0) || defined(ENABLE_INT_TB_CCR2)
    static Struct_Motor* motor_ptr = &motor;

    int32_t intflags = read_timer_b_interrupt_flags();
    clear_timer_b_interrupt_flags();
#endif

    #ifdef ENABLE_INT_TB_BASE
    if ((intflags & BIT_08) && read_timer_b_inten_base())
    {
        // Call TB base interrupt function pointer
        motor_ptr->fptr.fptr_int_TB_base(motor_ptr);
        motor_ptr->fptr.fptr_int_TB_base_2(motor_ptr);
    }
    #endif

    #ifdef ENABLE_INT_TB_CCR0
    if ((intflags & BIT_00) && read_timer_b_inten_ccr0())
    {
        // Call TB CCR0 interrupt function pointer 
        motor_ptr->fptr.fptr_int_TB_ccr0(motor_ptr);
        motor_ptr->fptr.fptr_int_TB_ccr0_2(motor_ptr);
    }
    #endif

    #ifdef ENABLE_INT_TB_CCR2
    if ((intflags & BIT_02) && read_timer_b_inten_ccr2())
    {
        // Call TB CCR2 interrupt function pointer 
        motor_ptr->fptr.fptr_int_TB_ccr2(motor_ptr);
        motor_ptr->fptr.fptr_int_TB_ccr2_2(motor_ptr);
    }
    #endif

#if ((DEVICE_SELECT != DEVICE_5556) && (DEVICE_SELECT != DEVICE_5556A))
#if (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_B)
    ppm_process_capture_timer_events();
#endif
#endif
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
}
PAC5XXX_OPTIMIZE_SPEED_OFF
