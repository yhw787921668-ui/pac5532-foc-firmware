
/**
* @file sim_config.h
* @brief Contains generic SIM config definitions and includes all SIM functions for the selected SIM method
*/

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include "motor_struct.h"


extern void sim_config(Struct_Motor* motor_ptr);
extern void sim_deconfig(Struct_Motor* motor_ptr, int32_t flag_pwm_timer_deconfig);
void sim_fptr_config(Struct_Motor* motor_ptr);
void sim_fptr_deconfig(Struct_Motor* motor_ptr, int32_t flag_pwm_timer_deconfig);
void config_int_sim(void);
void deconfig_int_sim(void);

#endif  //#ifndef SIM_CONFIG_H
