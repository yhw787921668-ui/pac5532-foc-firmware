
/**
 * @file isr_timerC.c
 * @brief Use for timerC timer interrupt handler to run critical FOC functions
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
#include "comm.h"


/**
 * @brief Process interrupt for TimerC_IRQHandler to run critical FOC functions
 */
PAC5XXX_RAMFUNC void TimerC_IRQHandler(void)
{
#if (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_C)
    ppm_process_capture_timer_events();
#elif (COMMSEL_LIN == TRUE)
		lin_process_capture_timer_event();
#endif
}
