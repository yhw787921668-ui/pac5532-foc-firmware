/**************************************************************************//**
 * @file     pac5xxx_driver_system.h
 * @brief    Firmware driver for the PAC5XXX system and clock controllers
 *
 * @note
 * Copyright (C) 2015-2020, Qorvo, Inc.
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
 
#ifndef PAC5XXX_DRIVER_SYSTEM_H
#define PAC5XXX_DRIVER_SYSTEM_H

/*------------- Analog to Digital Converter (ADC) ----------------------*/
/** @addtogroup PAC5XXX_Driver_System PAC5XXX System and Clock Controller
  @{
*/

#include "pac5xxx.h"
#include "pac5xxx_driver_config.h"

#ifdef PAC5XXX_DRIVER_SYSTEM_RAM
#define RAMFUNC_SYSTEM	PAC5XXX_RAMFUNC
#else 
#define RAMFUNC_SYSTEM
#endif


/**
 * @brief  This function configures and enables the PLL with the given frequency output using CLKREF
 * @param  pllindiv PLL input divider
 * @param  pllfbdiv PLL feedback divider
 * @param  plloutdiv PLL output divider
 * @note   This function configures and enables the PLL from the internal reference (CLKREF). After the 
 *         PLL Lock time, the SCLK output is switched from CLKREF to the PLL output.
 */
RAMFUNC_SYSTEM void pac5xxx_sys_pll_config_enable(uint32_t pllindiv, uint32_t pllfbdiv, uint32_t plloutdiv);

/**
 * @brief  When PAC_RECOVERY is defined, this function will insert a 3-second delay on startup if PE3 is low
 */
void pac_recovery(void);

/** 
 * @brief  Implements a delay in assembly
 * @param  counts Number of loop iterations
 */
void pac_delay_asm(uint32_t counts);

/**
 * @brief  Implementes a delay executing from SRAM
 * @param  counts Number of loop iterations
 */
PAC5XXX_RAMFUNC void pac_delay_asm_ramfunc(uint32_t counts);

/*@}*/ /* end of group PAC5XXX_Driver_System */

#endif // PAC5XXX_DRIVER_ADC_H

