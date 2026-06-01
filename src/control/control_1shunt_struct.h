

/**
 * @file control_1shunt_struct.h
 * @brief Use for single shunt control functions and structures
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

#ifndef CONTROL_1SHUNT_H
#define CONTROL_1SHUNT_H

#include "pac5xxx.h"
#include "maths.h"

#define S1_SAMPLE_ADVANCE_TICKS         15
#define S1_MINGAP                       90
//#define SELECT_I2SAMPLE_MIDDLEPWM


typedef struct
{
    int32_t taccr0_value;
    int32_t taccr1_value;
    int32_t flag_current_valid;
    int32_t flag_current_valid_sreg;
    int32_t duty_comp_sign_1;
    int32_t duty_comp_sign_2;
    int32_t duty_comp_sign_3;
    uint32_t svm_sector_prev;
    int32_t phase_current1;
    int32_t phase_current2;
    uint32_t adc_offset_iphase_ss_q14;
    uint32_t s1_min_gap;
    uint32_t s1_sample_ticks;
    int32_t read_phase_current1;
    int32_t read_phase_current2;
} Struct_OneShunt_Extn;


typedef struct
{
    int32_t taccr0_value;
    int32_t taccr1_value;
    int32_t phase_current1;
    int32_t phase_current2;
    uint32_t adc_offset_iphase_ss_q14;
    int32_t duty_u_up;
    int32_t duty_u_dn;
    int32_t duty_u_dn_store;
    int32_t duty_v_up;
    int32_t duty_v_dn;
    int32_t duty_v_dn_store;
    int32_t duty_w_up;
    int32_t duty_w_dn;
    int32_t duty_w_dn_store;
    uint32_t s1_min_gap;
    uint32_t s1_sample_ticks;
    int32_t read_phase_current1;
    int32_t read_phase_current2;
} Struct_OneShunt_Asym;


PAC5XXX_RAMFUNC void process_phase_currents_1shunt_extn(Struct_OneShunt_Extn *oneshunt_ptr, int32_t *Iu_measured_q14, int32_t *Iv_measured_q14, int32_t *Iw_measured_q14, int32_t mod_sector);
PAC5XXX_RAMFUNC void process_1shunt_extn(Struct_OneShunt_Extn *oneshunt_ptr, int32_t *u_duty_value, int32_t *v_duty_value, int32_t *w_duty_value, int32_t *mod_sector);
PAC5XXX_RAMFUNC void process_phase_currents_1shunt_asym(Struct_OneShunt_Asym *oneshunt_ptr, int32_t *Iu_measured_q14, int32_t *Iv_measured_q14, int32_t *Iw_measured_q14, int32_t mod_sector);
PAC5XXX_RAMFUNC void process_1shunt_asym(Struct_OneShunt_Asym *oneshunt_ptr, int32_t u_duty_value, int32_t v_duty_value, int32_t w_duty_value, int32_t *mod_sector);
#endif  //#ifndef CONTROL_1SHUNT_H
