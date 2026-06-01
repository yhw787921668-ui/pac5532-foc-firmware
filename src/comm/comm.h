
/**
 * @file comm.h
 * @brief Use for communication initialization UART, PPM, Push Button
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

#ifndef COMM_H
#define COMM_H

#include "config_app.h"
#include "config_features.h"
#include "config_debug.h"


// Include comm files
#if (COMMSEL_UART == TRUE)
    #include "uart_gui.h"
#endif

#if (COMMSEL_PPM == TRUE)
    #include "ppm.h"
#endif
#if (COMMSEL_PUSH_BUTTON == TRUE)
    #include "push_button.h"
#endif

#if (COMMSEL_CAN == TRUE)
		#include "can_gui.h"
#endif

#if (COMMSEL_LIN == TRUE)
		#include "lin_gui.h"
#endif

// Check if comm is not defined
#if (COMMSEL_UART == FALSE) && (COMMSEL_PPM == FALSE) && (COMMSEL_PUSH_BUTTON == FALSE) && (COMMSEL_CAN == FALSE) && (COMMSEL_LIN == FALSE)
    #error "COMM has not been defined"
#endif

// Check if PPM and PWMDac's are enabled at the same time
#if (COMMSEL_PPM == TRUE) && defined(ENABLE_DEBUG_PWMDAC)
    #error "PWM DAC's cannot be enabled when PPM is selected."
#endif


/**
 * @brief  Initialize PPM, UART, Push Button
 */
static inline void config_comm(void)
{
#if (COMMSEL_UART == TRUE)
    config_uart();
#endif

#if (COMMSEL_CAN == TRUE)
    config_can();
#endif
	
#if (COMMSEL_LIN == TRUE)
    config_lin();
		config_lin_timer();	
		lin_slave_init();
#endif

#if (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_B)
    config_ppm(TimerC);
#elif (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_C)
    config_ppm(TimerC);
#elif (COMMSEL_PPM == TRUE) && (PPM_TIMER_SELECT == PPM_TIMER_D)
    config_ppm(TimerD);
#endif

#if (COMMSEL_PUSH_BUTTON == TRUE)
    push_button_config();
    push_button_init_state();
#endif
}

#endif  //#ifndef COMM_H
