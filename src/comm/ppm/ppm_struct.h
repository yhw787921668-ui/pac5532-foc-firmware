
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

#ifndef PPM_STRUCT_H
#define PPM_STRUCT_H

#include "maths.h"

typedef struct
{
    int32_t ppm_pulse_high;
    int32_t ppm_pulse_low;
    int32_t ppm_pulse_delta;
    uint32_t pulse_time;
    uint32_t valid_pulse_counter_temp;
    uint32_t pulse_detected;
    uint32_t flag_cap_rising_first;
    uint32_t motor_powerup_flag;
    uint32_t pulse_timeout_in_sys_tick_isr_counts;
    uint32_t invalid_pulse_detect_count;
    int32_t valid_motor_on_pulse_detect;
    uint32_t motor_off_timeout_in_pulse_timeout_counts;
    int32_t ppm_rise_ccr_value;
    uint32_t timer_counts;
	  uint32_t ppm_slope;
}Struct_PPM;

#endif  //#ifndef PPM_STRUCT_H
