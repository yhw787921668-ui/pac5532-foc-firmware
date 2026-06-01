
/**
 * @file isr_adc.c
 * @brief Use for ADC ISR
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


/**
 * @brief This function use ADC Interrupt Handler to run main FOC control loop
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void ADC_IRQHandler(void)
{
//    debugio1_toggle();
#ifdef ENABLE_INT_ADC
    static Struct_Motor* motor_ptr = &motor;

#if defined (FAMILY_52XX)
    if ((PAC5XXX_ADC->ADCINT.AS0INT && PAC5XXX_ADC->ADCINT.AS0INT_EN) || (PAC5XXX_ADC->ADCINT.AS1INT && PAC5XXX_ADC->ADCINT.AS1INT_EN))
#endif
    {
        // Clear interrupt flag
        clear_adc_int_flag();
        motor_ptr->fptr.fptr_int_ADC(motor_ptr);

    }
		
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_off();

#endif
}

PAC5XXX_OPTIMIZE_SPEED_OFF
