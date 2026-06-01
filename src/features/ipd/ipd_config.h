
/**
 * @file ipd_config.h
 * @brief Use for ipd configuration
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

#ifndef IPD_CONFIG_H
#define IPD_CONFIG_H

#include "ipd_struct.h"

#define TIMERA_MHz                                  50.0
#define ADC_ISR_uS                                  45.0
#define ADC_ISR_TICKS                               (uint32_t)(ADC_ISR_uS * TIMERA_MHz)

#define MAX_PWM_TIMER_COUNTS                        65535
#define IPD_MINIMUM_DUTY                            20

#define IPD_ANGLE_DECISION_THRESHOLD                0
#define IPD_PHASE_LOST_DETECT_MIN_CURRENT_COUNT     15
#define IPD_ADC_TRIGGER_OFFSET_TICKS                10
#define MOTOR_DISABLE_BETWEEN_PULSE_BY_PWM_COUNT    3     // The number of PWM cycles to be disabled during IPD

#define IPD_LOW_SALIENCY_MOTOR                      FALSE // TRUE
// Current accumulation during each period
#define ENABLE_IPD_CUR_ACCUM_COMPARE                TRUE  // FALSE

extern void ipd_config(void);
extern void ipd_deconfig(void);

#endif
