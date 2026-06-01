
/**
 * @file ipd_funcs.c
 * @brief Use for ipd related utility functions
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

#include "ipd_funcs.h"
#include "ipd_config.h"
#include "driver_funcs.h"
#include "app_funcs.h"
#include "pac_init.h"
#include "pac5xxx_driver_timer.h"
#include "param_funcs.h"
#include "foc_funcs.h"
#include "foc_config.h"
#include "control_loop.h"
#include "utils.h"

#ifdef ENABLE_IPD

void ipd_m1_u_set_duty(uint16_t duty)
{
    m1_u_set_duty(duty);
}


void ipd_m1_v_set_duty(uint16_t duty)
{
    m1_v_set_duty(duty);
}


void ipd_m1_w_set_duty(uint16_t duty)
{
    m1_w_set_duty(duty);
}


void ipd_read_phase_currents(Struct_IPD *ipd_ptr)
{
#if (ADC_BITS_FIRMWARE_SELECTION == 10)
    ipd_ptr->Iu_ADC = readseq_iphase_u();
    ipd_ptr->Iv_ADC = readseq_iphase_v();
    ipd_ptr->Iw_ADC = readseq_iphase_w();
#elif (ADC_BITS_FIRMWARE_SELECTION == 12)
    ipd_ptr->Iu_ADC = readseq_iphase_u() >> 2;
    ipd_ptr->Iv_ADC = readseq_iphase_v() >> 2;
    ipd_ptr->Iw_ADC = readseq_iphase_w() >> 2; 
#else
    "Error"     
#endif
    
}


void ipd_reset(void)
{
    static Struct_Motor* motor_ptr = &motor;

    ipd_config();
    motor_ptr->ipd.ipd_state = IPD_START;
    // Enable the motor
    m1_enable();
}


void ipd_exit(void)
{
    // Disable the motor
    m1_disable();
    ipd_deconfig();
}

#endif
