/**************************************************************************//**
 * @file     pac5xxx_driver_system_ccs_config.c
 * @brief    Firmware driver for the PAC5XXX System Clock Control
 *
 * @note
 * Copyright (C) 2015-2019, Qorvo, Inc.
 *
 * THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
 * AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
 * APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
 * (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
 * QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
 * DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
 * ONLY BY CERTAIN AUTHORIZED PERSONS.
 *
 ******************************************************************************/

#include "PAC5XXX_driver_system.h"

RAMFUNC_SYSTEM void pac5xxx_sys_ccs_config(CCSCTL_CLKIN_Type clock_select, CCSCTL_ACLKDIV_Type aclk_div, CCSCTL_HCLKDIV_Type fclk_div)
{
    PAC5XXX_SYSCTL->CCSCTL.CLKIN = clock_select;
    PAC5XXX_SYSCTL->CCSCTL.ACLKDIV = aclk_div;
    PAC5XXX_SYSCTL->CCSCTL.HCLKDIV = fclk_div;
}


//=====================================================================================
// pac_recovery() provides a mechanism to recover a misconfigured PAC device
// Ground PE2 at power up to add a 6 second delay so that the Flash can be erased
//=====================================================================================
// NOTE:  #define PAC_RECOVERY should be commented out for a production system
// When PAC_RECOVERY is defined, then a 6 Second delay will be added if PE2 is connected to ground at PAC power up
// The 6 second delay will give the user time to erase the device using SWD, which will eliminate the code that is misconfiguring the device
// On PAC52xx EVKs, PE2 is normally the GUI UART PAC RX input pin (Host TX); a delay will not normally be inserted even with GUI running
#define PAC_RECOVERY    // Define to enable PAC recovery mechanism

#ifdef PAC_RECOVERY

#warning PAC_RECOVERY enabled; Ground PE2 to add 6 second boot delay to allow recovery of misconfigured device; remove for production system

void pac_recovery(void)
{
    volatile uint32_t i;
    uint32_t perform_delay = 1;

    // PE2 is already an input, just need to Enable Pull Up
    PAC5XXX_GPIOE->PU.P2 = 1;                         // Enable Pull Up

    // If PE2 is high a delay is not needed
    // Must check over the length of a full UART packet at 115kbps (78uS) in case UART is talking and making PE2 low for short durations
    for (i=0; i < 12; i++)  // Value of 12 = at least 140 uS, which is enough for 1 UART packet
    {
        // If PE2 is high at any time, then a delay is not needed
        if (PAC5XXX_GPIOE->IN.P2 == 1)
        {
            perform_delay = 0;
        }
    }

    // If PE2 was low, delay required, add 6 second delay
    if(perform_delay)
    {
        pac_delay_asm(1400000);
    }

    // Restore PE2 to reset state
    PAC5XXX_GPIOE->PU.P2 = 0;                         // Disable Pull Up
}

#else

void pac_recovery(void)
{
    // No Recovery required for production system
}

#endif  // #ifdef PAC_RECOVERY

