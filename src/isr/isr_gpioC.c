
//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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
#include "pac_init.h"
#include "fault.h"
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_tile.h"

#if (MODULE_MPOS_HALL == ENABLED) & (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)
#include "hall_funcs.h"
#endif


PAC5XXX_RAMFUNC void GpioC_IRQHandler(void)
{
#if (MODULE_MPOS_HALL == ENABLED) & (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT) & (Hall_Gpio_IRQn == GpioC_IRQn)
    hall_isr_handler();
#endif
}
