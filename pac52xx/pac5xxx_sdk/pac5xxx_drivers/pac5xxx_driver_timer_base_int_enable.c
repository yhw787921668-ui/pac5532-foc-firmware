/**************************************************************************//**
 * @file     pac5xxx_driver_timer_base_int_enable.c
 * @brief    Firmware driver for the PAC5XXX base timer interrupt configuration
 *
 * @note
 * Copyright (C) 2015-2019, Qorvo, Inc.
 *
 * THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
 * AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
 * APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
 * (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
 * QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
 * DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
 * ONLY BY CERTAIN AUTHORIZED PERSONS.
 *
 ******************************************************************************/
 
#include "pac5xxx_driver_timer.h"

RAMFUNC_TIMER void pac5xxx_timer_base_int_enable(TimerInstance timer, int enable)
{
  switch (timer)
  {
  case TimerA:
    PAC5XXX_TIMERA->CTL.INT = 1; // WTC
    PAC5XXX_TIMERA->CTL.INTEN = enable;
    break;
  case TimerB:
    PAC5XXX_TIMERB->CTL.INT = 1; // WTC
    PAC5XXX_TIMERB->CTL.INTEN = enable;
    break;
  case TimerC:
    PAC5XXX_TIMERC->CTL.INT = 1; // WTC
    PAC5XXX_TIMERC->CTL.INTEN = enable;
    break;
  case TimerD:
    PAC5XXX_TIMERD->CTL.INT = 1; // WTC
    PAC5XXX_TIMERD->CTL.INTEN = enable;
    break;
  }  
}

