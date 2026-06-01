
/**
 * @file fault.c
 * @brief Use to define fault, set fault, clear fault, set warning, clear warning and over current functions
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
#include "driver_funcs.h"
#include "pac_init.h"
#include "fault.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_adc.h"


/**
 * @brief  This function use disable adc, motor disable and be in while loop
 */
void fault(void)
{
    fix16_t delay_count;                            // Use this delay to blink LED during fault

    pac5xxx_adc_enable(0);

    // Disable motor if enter fault
    m1_disable();

    // Go into an infinite loop.
    while (1)
    {
        for (delay_count = 0; delay_count < 0x00010000; delay_count++)
        {
        }
    }
}


/**
 * @brief  This function use to reset over current fault
 */
// Reset AFE after over-current trip
void oc_reset(void)
{
      uint16_t register_val;

    // Disable global interrupts until OC reset is complete
    __disable_irq();

    // Set drivers to GPIO mode
    set_drivers_gpio();

    // Clear int/fault, pro stat flags
    pac5xxx_clr_int_fault_flags();
    
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x00);
    while (pac5xxx_tile_register_read(ADDR_PROTSTAT));
    protint_enable();

    // Disable driver
    do
    {
        pac5xxx_tile_register_write(ADDR_ENDRV, 0);
        register_val = pac5xxx_tile_register_read(ADDR_ENDRV);
    } while ((register_val&0x1) != 0x0);

    // Enable driver
    do {
        pac5xxx_tile_register_write(ADDR_ENDRV, 1);
        register_val = pac5xxx_tile_register_read(ADDR_ENDRV);
    } while ((register_val&0x1) != 0x1);

    // Clear OC flag
    clear_fault(FAULT_CAFE_OC);

    // Enable global interrupts
    __enable_irq();
}


/**
 * @brief  This function use to set fault
 * @param  uint32_t fault_mask
 */
void set_fault(uint32_t fault_mask)
{
    motor.fault.motor |= fault_mask;

    // Decide firmware response to fault
    if (fault_mask & FAULT_MAIN_GRP_STOP)
    {
        motor.fault.fault_stop_flag = 1;
    }
    else if (fault_mask & FAULT_MAIN_GRP_STATE)
    {
        motor.fault.fault_state_flag = 1;
    }
    else if (fault_mask & FAULT_MAIN_GRP_TERMINAL)
    {
        motor.fault.fault_terminal_flag = 1;
    }
}


/**
 * @brief  This function use to set fault
 * @param  uint32_t fault_mask
 */
void set_fault_terminal(eFaultTerminal fault_mask)
{
    motor.fault.motor |= FAULT_TERMINAL;
    motor.fault.fault_terminal |= fault_mask;
    motor.fault.fault_terminal_flag = 1;
}


/**
 * @brief  This function use to clear fault
 * @param  uint32_t fault_mask
 */
void clear_fault(uint32_t fault_mask)
{
    motor.fault.motor &= (~fault_mask);
}


/**
 * @brief  This function use to set warning
 * @param  uint32_t fault_mask
 */
void set_warning(uint32_t fault_mask)
{
    motor.fault.motor |= fault_mask;
}


/**
 * @brief  This function use to clear warning
 * @param  uint32_t fault_mask
 */
void clear_warning(uint32_t fault_mask)
{
    motor.fault.motor &= ~fault_mask;
}
