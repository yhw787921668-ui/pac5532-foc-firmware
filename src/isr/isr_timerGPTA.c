
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

#if (MODULE_MPOS_HALL == ENABLED)
#include "hall_funcs.h"
#endif


#if defined(FAMILY_55XX)
PAC5XXX_RAMFUNC void GPTimerA_IRQHandler(void)
{

volatile uint32_t dummy = PAC55XX_GPTIMERA->CTL.w;

#if (MODULE_MPOS_HALL == ENABLED)
    hall_timer_isr();
#endif
}
#endif
