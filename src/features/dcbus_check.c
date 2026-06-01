
/**
 * @file dcbus_check.c
 * @brief Use to check is DC bus is with-in min-max voltage range and post fault
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

#include "dcbus_check.h"
#include "fault.h"
#include "app_funcs.h"

#ifdef ENABLE_DCBUS_MIN_MAX_CHECK
void check_dcbus_fault(Struct_Motor* motor_ptr)
{
    static unsigned char min_bus_voltage_detect_count = 0;
    static unsigned char max_bus_voltage_detect_count = 0;

    // Run warning and fault check only when in OL, CL, Coasting, or Stopped states
    if ((motor_ptr->state.main == STATE_MAIN_STARTUP) ||
            (motor_ptr->state.main == STATE_MAIN_RUN) ||
            (motor_ptr->state.main == STATE_MAIN_COASTING) ||
            (motor_ptr->state.main == STATE_MAIN_STOPPED))
    {
        // Under/Over Voltage warning: Flags warning if min/max voltage limit is exceeded
        if ((motor_ptr->app.dcbus_voltage < MIN_VBUS) || (motor_ptr->app.dcbus_voltage > MAX_VBUS))
        {
            set_warning(WARNING_OU_VOLTAGE);
            // Check under/over voltage fault only if on OL, CL, or Coasting states
            if (motor_ptr->state.main != STATE_MAIN_STOPPED)
            {
                // Under-Voltage fault: Flags fault if majority filter threshold is reached
                if (motor_ptr->app.dcbus_voltage < MIN_VBUS)
                {
                    if (min_bus_voltage_detect_count++ > MIN_VBUS_FAULT_THRESHOLD_COUNT)
                    {
                        min_bus_voltage_detect_count = 0;
                        set_fault(FAULT_OU_VOLTAGE);
                    }
                }
                else
                {
                    if (min_bus_voltage_detect_count > 0)
                        min_bus_voltage_detect_count--;
                }

                // Over-Voltage fault: Flags fault if majority filter threshold is reached
                if (motor_ptr->app.dcbus_voltage > MAX_VBUS)
                {
                    if (max_bus_voltage_detect_count++ > MAX_VBUS_FAULT_THRESHOLD_COUNT)
                    {
                        max_bus_voltage_detect_count = 0;
                        set_fault(FAULT_OU_VOLTAGE);
                    }
                }
                else
                {
                    if (max_bus_voltage_detect_count > 0)
                        max_bus_voltage_detect_count--;
                }
            }
        }
        // Clear warning if bus voltage within min/max limits
        else
        {
            clear_warning(WARNING_OU_VOLTAGE);
        }
    }
}
#endif
