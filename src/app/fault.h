
/**
 * @file fault.c
 * @brief Use to define fault, set fault, clear fault, set warning, clear warning and over current functions
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

#ifndef FAULT_H
#define FAULT_H

#include "motor_struct.h"


extern void oc_reset(void);                                                                     // Reset from hardware over-current condition
extern void fault(void);                                                                        // Go into fault state
extern void set_fault(uint32_t fault_mask);
extern void set_fault_terminal(eFaultTerminal fault_mask);
extern void clear_fault(uint32_t fault_mask);
extern void set_warning(uint32_t fault_mask);
extern void clear_warning(uint32_t fault_mask);
extern int32_t pac5xxx_get_over_current_status(void);
extern int32_t pac5xxx_get_over_current_status_from_hpdac(void);
extern int32_t pac5xxx_get_over_temperature_status(void);
extern int32_t pac5xxx_get_driver_fault(void);
extern void pac5xxx_clr_int_fault_flags(void);

#ifdef ENABLE_CBC_CURRENT_LIMIT
extern void cbc_current_reset(void);
#endif


/**
 * @brief  This function use to set differnt motor status flags
 * @param  uint32_t status_mask
 */
static inline void set_status(uint32_t status_mask)
{
    motor.status.motor |= status_mask;
}


/**
 * @brief  This function use to clear motor status flags
 * @param  uint32_t status_mask
 */
static inline void clear_status(uint32_t status_mask)
{
    motor.status.motor &= ~status_mask;
}


/**
 * @brief This function use calibrate ADC channels offset
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#ifdef ENABLE_DEVICE_ID_CHECK
static inline void check_devid(Struct_Motor* motor_ptr)
{
    #if defined(FAMILY_55XX)
    if ((READ_INT32(DEVICE_INFO_ADDRESS2) & 0xFFFFFF) != DEVICE_SELECT)
    #else
    if ((READ_INT32(DEVICE_INFO_ADDRESS2) & 0xFFFF) != DEVICE_SELECT)
    #endif
    {
    #if defined(FAMILY_52XX)
        if ((READ_INT32(DEVICE_INFO_ADDRESS2) & 0xFF00) == DEVICE_52XXX)
        {
            int32_t read_device_info;
            read_device_info = (READ_INT32(DEVICE_INFO_ADDRESS_SHUNT_NUM) & 0xFFFFFFFF);
            if (read_device_info != DEVICE_SELECT)
            {
                set_fault_terminal(FAULT_TERMINAL_DEVID);
            }
        }
        else
    #endif
        {
            set_fault_terminal(FAULT_TERMINAL_DEVID);
        }
    }
}
#endif

#endif  //#ifndef FAULT_H
