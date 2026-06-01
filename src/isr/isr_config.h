
/**
 * @file isr_config.h
 * @brief Use for selecting active ISRs
 */

//=============================================================================
// Copyright (C) 2021 - 2023, Qorvo, Inc.
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

#ifndef ISR_CONFIG_H
#define ISR_CONFIG_H


#include "pac5xxx.h"
#include "board.h"
#include "config_app.h"
#include "config_features.h"
//#include "config_debug.h"


//======================================================================
// DECLARATION DEPENDENCIES FOR ISR FUNCTION POINTERS (Do Not Modify!!!)
//======================================================================

// Define Function Pointer Selection Options
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    #define ENABLE_INT_ADC

    // Interrupt for FOC control counter
    #if (PWM_TIMER == TIMER_A)
        #define ENABLE_INT_TA_CCR0
    #endif
    #if (PWM_TIMER == TIMER_B)
        #define ENABLE_INT_TB_CCR0
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    // Interrupt for control loop
    #define ENABLE_INT_ADC

    // Interrupt for FOC control counter
    #if     (PWM_TIMER == TIMER_A)
        #define ENABLE_INT_TA_BASE
    #elif   (PWM_TIMER == TIMER_B)
        #define ENABLE_INT_TB_BASE
    #endif

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
    #if defined(ENABLE_IPD) || defined (ENABLE_SIM)
        #define ENABLE_INT_ADC
    #endif
    // Interrupts for control loop and control counter
    #define ENABLE_INT_TA_BASE
    #define ENABLE_INT_TB_BASE

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    #if defined(ENABLE_IPD) || defined (ENABLE_SIM)
        #define ENABLE_INT_ADC
    #endif

   // Interrupt for control loop
    #if     (PWM_TIMER == TIMER_A)
        #define ENABLE_INT_TB_CCR2
    #elif   (PWM_TIMER == TIMER_B)
        #define ENABLE_INT_TA_CCR2
    #endif

    // Interrupt for FOC control counter
    #if     (PWM_TIMER == TIMER_A)
        #define ENABLE_INT_TA_BASE
    #elif   (PWM_TIMER == TIMER_B)
        #define ENABLE_INT_TB_BASE
    #endif

    #if defined (ENABLE_SIM)
        // Interrupt for SIM control counter
        #if     (PWM_TIMER == TIMER_A)
            #define ENABLE_INT_TA_CCR0
        #elif   (PWM_TIMER == TIMER_B)
            #define ENABLE_INT_TB_CCR0
        #endif
    #endif

#endif

#endif  //#ifndef ISR_CONFIG_H
