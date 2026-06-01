
/**
 * @file pac_init.h
 * @brief Use to initialize peripheral, PWM, Hall, AFE, QEP, ADC, FOC, IPD, enable protection
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

#ifndef PAC_INIT_H
#define PAC_INIT_H

#include "pac5xxx.h"
#include "pac5xxx_driver_tile.h"
#include "config_features.h"


extern void peripheral_init(void);                                                                      // Initialize peripherals
extern void cafe_init(void);                                                                            // Initialize CAFE registers
extern void config_systick_timer(void);
extern void protint_enable(void);
void config_register_bits(uint8_t address, uint8_t bits_mask, uint8_t bit_value);

#endif  //#ifndef PAC_INIT_H
