
/**
 * @file isr_stubs.c
 * @brief Use for NMI, MemCtl, HardFault and Default ISR handlers
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
#include "fault.h"

/**
 * @brief Process interrupt for NMI_Handler
 */
void NMI_Handler(void)
{
    fault();
}

/**
 * @brief Process interrupt for HardFault_Handler
 */
void HardFault_Handler(void)
{
    fault();
}

/**
 * @brief Process interrupt for MemCtl_IRQHandler
 */
void MemCtl_IRQHandler(void)
{
    fault();
}

/**
 * @brief Process interrupt for Default_Handler
 */
void Default_Handler(void)
{
    // Go into an infinite loop.
    fault();
}

