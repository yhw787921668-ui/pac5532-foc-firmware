
/**
 * @file isr_timerD.c
 * @brief Use for timerD timer interrupt handler to run critical FOC functions
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

#if (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_D)
#include "comm.h"
#endif

#if (MODULE_MPOS_HALL == ENABLED)
#include "hall_funcs.h"
#endif


/**
 * @brief Process interrupt for TimerD_IRQHandler to run critical FOC functions
 */
PAC5XXX_RAMFUNC void TimerD_IRQHandler(void)
{
#if (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_D)
    ppm_process_capture_timer_events();
#endif

#if (MODULE_MPOS_HALL == ENABLED)
#if defined(FAMILY_52XX)
    hall_timer_isr();
#endif
#endif
}
