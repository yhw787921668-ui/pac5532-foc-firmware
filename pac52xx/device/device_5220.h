
/**
 * @file device_5220.h
 * @brief Use for 5220 device definition
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
 
#ifndef DEVICE_5220_H
#define DEVICE_5220_H
#define PAC5220

#include "device_options.h"
#include "family_52xx.h"

//////////////////////////////////////////////////////////
// Resource: FOC Timer  FOC Timer Interrupt, ADC Interrupt
//////////////////////////////////////////////////////////
#define PWM_TIMER TIMER_A
#define FOC_PWM_Timer TimerA
#define FOC_PWM_Timer_Ptr PAC5XXX_TIMERA
#define FOC_PWM_Timer_IRQn TimerA_IRQn
#define ADC_IRQn ADC_IRQn
#define FOC_ASYM_SUPPLE_Timer TimerB
#define FOC_ASYM_SUPPLE_Timer_Ptr PAC5XXX_TIMERB
#define FOC_ASYM_SUPPLE_Timer_IRQn TimerB_IRQn

#define PUSER_VALUE_DIFFAMP_GAIN_FINAL PUSER_VALUE_DIFFAMP_GAIN
#define PUSER_VALUE_DIFFAMP_GAIN_FIXED                    FALSE

// CAFE specific definition
//=====================================
// Low OC Protection Signal (AIOxx_LPROT)
//=====================================
#define AIO50_LPROT_NONE        0
#define AIO50_LPROT_PR1         BIT_05
#define AIO50_LPROT_PR2         BIT_04
#define AIO50_LPROT_PR1PR2      (BIT_04 | BIT_05)

//=====================================
// High OC Protection Signal (AIOxx_HPROT)
//=====================================
#define AIO50_HPROT_NONE        0
#define AIO50_HPROT_PR1         BIT_07
#define AIO50_HPROT_PR2         BIT_06
#define AIO50_HPROT_PR1PR2      (BIT_06 | BIT_07)

//=====================================
// Driver protection while OC 
//=====================================
#define DRV_HIGHSIDE_PR1        BIT_07
#define DRV_HIGHSIDE_PR2        BIT_06
#define DRV_LOWSIDE_PR1         BIT_05
#define DRV_LOWSIDE_PR2         BIT_04

//=====================================
// Vp voltage selection 
//=====================================
#define VP_VOLTAGE_9V        (0x01 << 6)
#define VP_VOLTAGE_12V       (0x02 << 6)
#define VP_VOLTAGE_15V       (0x03 << 6)
#define VP_CONFIG_BITS_MASK         0x3F

//==========================================================
// Min duty cycle: Version A can be set as 0, otherwise as 1
//==========================================================
#define DUTY_CYCLE_MIN_TICK     0

#endif  //#ifndef DEVICE_5220_H
