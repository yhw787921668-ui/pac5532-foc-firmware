/**
 * @file watchdog.c
 * @brief Use to initialize watchdog timer
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
#include "config_features.h"

#if (ENABLE_WATCHDOG == TRUE)

#include "watchdog.h"
#include "pac5xxx_driver_watchdog.h"

// No need to do any change in all the settings
#define WATCHDOG_CLK_FREQ_SELECTED    4000000.0f        // 4MHz default crystal selected
#define WATCHDOG_DEFAULT_FREQ_DIVIDER    2.0f           // default watchdog divider
#define WATCHDOG_INTERRUPT_DISABLED    0                // No watchdog interrupt
#define WATCHDOG_POWER_ON_RESET_ENABLE    1             // Watchdog power on reset enable


void init_Watchdog_time_base(void)
{
   // FRCLK, 4MHz, clock divider of 2
    pac5xxx_watchdog_config_clock(WDTCTL_CLKSEL_FRCLK, WDTCTL_PS_DIV2);
    // Enable WDT - When the WDT Interrupt is not enabled, a single count down to 0 will trigger a reset
    pac5xxx_watchdog_config(WATCHDOG_INTERRUPT_DISABLED,        // No Interrupt
                            WATCHDOG_POWER_ON_RESET_ENABLE,     // POR Enable
                           // WDT Counter = FRCLK Freq / WDT Clock Divisor / Test Freq * 1.5 for 50% margin
                           ( (uint32_t)(WATCHDOG_CLK_FREQ_SELECTED / WATCHDOG_DEFAULT_FREQ_DIVIDER / WATCHDOG_FREQ_HZ) ) );

}
#endif  //#if (ENABLE_WATCHDOG == TRUE)

