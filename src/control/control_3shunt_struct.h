
/**
 * @file control_3shunt_struct.h
 * @brief Use for 3 shunt motor current structure
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

#ifndef CONTROL_3SHUNT_STRUCT_H
#define CONTROL_3SHUNT_STRUCT_H

#include "maths.h"
#include "board.h"
#include "pac5xxx_driver_timer.h"


typedef struct
{
    int16_t Iu_ADC;                                 //Stored Iu ADC value
    int16_t Iv_ADC;                                 //Stored Iv ADC value
    int16_t Iw_ADC;                                 //Stored Iw ADC value
    uint32_t adc_offset_iphase_u_q14;
    uint32_t adc_offset_iphase_v_q14;
    uint32_t adc_offset_iphase_w_q14;
} Struct_ThreeShunt;

#endif  //#ifndef CONTROL_3SHUNT_STRUCT_H
