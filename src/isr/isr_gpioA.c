
/**
 * @file isr_gpioA.c
 * @brief Use for PortA ISR
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

///
/// @brief Process GPIOB interrupts (CAFE interrupt on PA7)
///
/**
 * @brief Process GPIOA interrupts (CAFE interrupt on PA7) to handle over current
 */
void GpioA_IRQHandler(void)
{
    volatile uint8_t overcurrent;
  
#ifdef ENABLE_CBC_CURRENT_LIMIT
    volatile uint8_t cbc;
#endif
	
    // clear interrupt
    clear_oc_interrupt();

#ifdef ENABLE_CBC_CURRENT_LIMIT
    cbc = (pac5xxx_tile_register_read(ADDR_PROTSTAT) & 0x07);
    overcurrent = (pac5xxx_tile_register_read(ADDR_PROTSTAT) & 0x70);
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    overcurrent = pac5xxx_get_over_current_status_from_hpdac();
#else
    overcurrent = pac5xxx_get_over_current_status();
#endif

    // Respond to over-current condition and set flag
    if (overcurrent)
    {
        // Set OC flag
        set_fault(FAULT_CAFE_OC);
    }
#ifdef ENABLE_CBC_CURRENT_LIMIT
    // Respond to CBC current condition
    else if (cbc)
    {
        motor.stall.flag_cbc_triggered = 1;
        cbc_current_reset();
        set_warning(WARNING_CBC_OC);
    }
#endif    
}

