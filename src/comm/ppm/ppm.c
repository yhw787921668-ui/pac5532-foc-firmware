
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

#include "ppm.h"

#include "pac5xxx_timers.h"
#include "motor_struct.h"

#if (COMMSEL_PPM == TRUE)

void config_ppm (TimerInstance timer)
{
    pac5xxx_timer_clock_config(timer, TXCTL_CS_ACLK, TXCTL_PS_DIV2);            // Configure timer clock input for ACLK (100MHz), /1 divider
	  //Set Timer frequency and mode
    pac5xxx_timer_base_config(timer,PPM_TIMER_TIME_PERIOD,0,TxCTL_MODE_UP,0);    // 1 count = 1.28usec,  19531 * 1.28usec = 25 msec = 40 Hz
	  PPM_Timer_Ptr->CTL.MODE = 1;
	  PPM_Timer_Ptr->CTL.BASEIE = 1;
	  PPM_Timer_Ptr->CTL.CLKSRC = 1;
	  PPM_Timer_Ptr->CTL.CLKDIV = TXCTL_PS_DIV128;
	  PPM_Timer_Ptr->INT.w = 0;
	
	  // Configure GPIO pin for PPM input
    PAC55XX_SCC->PPM_IO_MUXSEL.PPM_INPUT_PORTNUM = 2;
	  // configuration as input
    PAC5XXX_GPIO_PPM_IN->MODE.PPM_INPUT_PORTNUM = 3;
	  // configuration pull up
    PAC55XX_SCC->PFPUEN.PPM_INPUT_PORTNUM = 1;
	
	  PPM_Timer_Ptr->PPM_CC_CONTROL.CCINTEDGE = 2;
	  PPM_Timer_Ptr->PPM_CC_CONTROL.CCINTEN = 1;
	  PPM_Timer_Ptr->PPM_CC_CONTROL.CCMODE = 1;
	  PPM_Timer_Ptr->PPM_CC_CONTROL.CCLATCH = 2;

	  PPM_Timer_Ptr->INT.w = 0;
		
		NVIC_SetPriority(PPM_Timer_IRQn,3);
    NVIC_EnableIRQ(PPM_Timer_IRQn);
}


void ppm_init(Struct_Motor* motor_ptr)
{
    motor_ptr->ppm.valid_motor_on_pulse_detect = 0;
    motor_ptr->ppm.invalid_pulse_detect_count = 0;
}
#endif
