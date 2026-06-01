
/**
 * @file isr_gpioB.c
 * @brief Use for PortB ISR
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
#include "pac_init.h"
#include "fault.h"
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_tile.h"

///
/// @brief Process GPIOB interrupts (CAFE interrupt on PB0)
///
/**
 * @brief Process GPIOB interrupts (CAFE interrupt on PB0) to handle over current
 */
void GpioB_IRQHandler(void)
{
    volatile uint8_t overcurrent_fault;
	volatile uint8_t temperature_fault;
	volatile uint8_t driver_fault;
    
#ifdef ENABLE_CBC_CURRENT_LIMIT
    volatile uint8_t cbc;
#endif

    // clear interrupt
    clear_oc_interrupt();
	
#ifdef ENABLE_CBC_CURRENT_LIMIT
    cbc = (pac5xxx_tile_register_read(ADDR_PROTSTAT) & 0x07);
	
    // Respond to CBC current condition
    if (cbc)
    {
        motor.stall.flag_cbc_triggered = 1;
        cbc_current_reset();
        set_warning(WARNING_CBC_OC);
    }
#endif

	driver_fault = pac5xxx_get_driver_fault();
    #if (SEL_OC_METHOD == OC_METHOD_HPDAC)
    overcurrent_fault = pac5xxx_get_over_current_status_from_hpdac();
    #else
    overcurrent_fault = pac5xxx_get_over_current_status();
    #endif
    
    temperature_fault = pac5xxx_get_over_temperature_status();
	
	if (overcurrent_fault | temperature_fault | driver_fault)
    {
        // Set OC flag
        set_fault(FAULT_CAFE_OC);
    }		
}

