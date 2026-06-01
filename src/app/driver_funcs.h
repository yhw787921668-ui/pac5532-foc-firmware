
/**
 * @file driver_funcs.h
 * @brief Use to initialize PWM duty, port mode and enable disable motor, low side PWM
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

#ifndef DRIVER_FUNCS_H
#define DRIVER_FUNCS_H

#include "pac5xxx.h"


//=============================================
// Motor Driver Function Declarations
//=============================================
extern void m1_enable(void);
extern void m1_enable_with_duty(uint32_t duty_cycle);
extern void m1_enable_no_duty(void);
extern void set_drivers_gpio(void);
extern void m1_disable(void);
extern void m1_LS_pwm_only_config(void);
extern void enable_driver(void);

#endif  //#ifndef DRIVER_FUNCS_H
