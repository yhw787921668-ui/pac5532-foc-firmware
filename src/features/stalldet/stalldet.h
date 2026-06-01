
/**
 * @file stalldet.h
 * @brief Use for stall detect structure
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

#ifndef STALLDET_H
#define STALLDET_H

#include "fix16.h"
#include "maths.h"

/**
 * @brief Struct_StallDet structure
 */

typedef struct
{
    uint16_t bemf_constant;                   /*!< motor bemf constant measured during open loop and no load */
    unsigned int timeout_systicks;     /*!< Check for stall condition for multiple of dt ticks then set stall detect variable */
    unsigned int sampletime_systicks;     /*!< Check for stall condition for multiple of dt ticks then set stall detect variable */
    unsigned long systick_stamp;                  /*!< Bit set when motor need to switch from close loop FOC to open loop mode to speed down the motor */
    q1p14_t ve_sqr_q14;                /* Hold average filtered value of vdrive for low voltage */
    unsigned int ctr_stamp;
    uint8_t flag_event1 : 1;        /*!< Bit set means system detect 1 stall condition */
    uint8_t flag_event2 : 1;        /*!< Bit set means system detect 1 stall condition */
    uint8_t flag_event3 : 1;        /*!< Bit set means system detect 1 stall condition */
    uint8_t flag_event4 : 1;        /*!< Bit set means system detect 1 stall condition */
    uint8_t flag_process : 1;
    uint8_t temp2 : 3;                       /*!< Unused 28 bits out of 32 bits */
    uint32_t ctr_event_1 : 8;
    uint32_t ctr_event_2 : 8;
    uint32_t ctr_event_3 : 8;
    uint32_t ctr_event_4 : 8;
    uint8_t ctr_event;
    int32_t speed_est;          // q28
    int32_t speed_cmd;          // q28
    fix16_t filter_alpha;
    fix16_t flag_cbc_triggered;
} Struct_StallDet;

/**
 * @brief  Use to check stall detection
 * @param Struct_StallDet* stall_ptr
 * @param uint32_t systick_div_cntr
 * @param fix16_t motor_speed_peak
 * @param fix16_t descale_volt_factor
 * @param q1p14_t vdrvsq_peak
 * @param int32_t iq_q14
 * @param fix16_t est_speed
 */
extern void stall_detect_1(Struct_StallDet* stall_ptr, uint32_t systick_div_cntr, fix16_t motor_speed_peak, fix16_t descale_volt_factor,
                             q1p14_t vdrvsq_peak, int32_t iq_q14);
extern void stall_detect_2(Struct_StallDet* stall_ptr, fix16_t est_speed, int32_t motor_direction);

#endif
