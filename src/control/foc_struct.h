
/**
 * @file foc_struct.h
 * @brief Use for basic FOC structure
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

#ifndef FOCSTRUCT_H
#define FOCSTRUCT_H

#include "maths.h"
#include "pid.h"


// ENUM Definitions
typedef enum
{
    STATE_FOC_IDLE                          = 0,
    STATE_FOC_FULLFLOW,                    // 1
    STATE_FOC_BACKENDFLOW                  // 2
} eStateFOC;


/**
 * @brief FOC structure
 */
typedef struct
{
    // State & Status
    int32_t state_foc;

    // FOC Running Variables
    PID_Data_Type Id_pid;                           // PID controller for Id
    PID_Data_Type Iq_pid;                           // PID controller for Iq
    int32_t ialpha_q14;                             // Iaplha in q14 format
    int32_t ibeta_q14;                              // Ibeta in q14 format
    int32_t iq_prefilter_q14;               // Iq previous value use for backup as good last non filtered value in q1p14 format
    int32_t id_prefilter_q14;               // Id previous value use for backup as good last non filtered value in q1p14 format
    int32_t iq_q14;                                 // Iq current value use for digital filtering in q14 format
    int32_t id_q14;                                 // Id current value use for digital filtering in q14 format
    int32_t vd_q14;                               // hold previous Vd value for afterwards processing
    int32_t vq_q14;                               // hold previous Vq value for afterwards processing in q14 format
    int32_t vd_feedforward_q14;
    int32_t vq_feedforward_q14;
    int32_t valpha_q14;                             // hold previous Vq value for afterwards processing in q14 format
    int32_t vbeta_q14;                              // hold previous Vq value for afterwards processing in q14 format

    fix16_t idref_ramp_q14;                         // Id_ramp
    fix16_t idref_ramp_f16;
    fix16_t iqref_ramp_q14;                         // Iq_ramp
    fix16_t iqref_ramp_f16;

    int32_t flag_sat_logicor;                       // Flag is high when any control period within sampling period are saturated
    int32_t flag_sat_logicand;                      // Flag is high only when all control periods within sampling period are saturated

    // FOC Configuration
    fix16_t Id_filter_gain;
    fix16_t Iq_filter_gain;
    int16_t svm_min_ticks;                          // Timer ticks in SVM min for this motor (for SVM)
    int16_t svm_max_ticks;                          // Timer ticks in SVM max for this motor (for SVM)
    int32_t dt_control_q26;
    fix16_t dt_control_1div_q6;                         // 1 / (dt * 1024)
    uint32_t ctrl_divider_foc;                      // this is the ratio between PWM Frequency and FOC loop
    uint32_t overflow_counter;                      // this is use to count FOC time base overflow
    uint32_t motor_period_ticks;
    int32_t motor_period_ticks_1div;
#ifdef ENABLE_DQ_DECOUPLING
    //#if (SEL_DQ_DECOUPLING_METHOD == FEEDBACK_DECOUPLING)
    int32_t current_filter_coeff_q16;               // For filtering the DQ decoupling current related components
    int32_t id_dec_q14;
    int32_t iq_dec_q14;
    //#endif
#endif
    int32_t Iq_pid_min_value_gui;
} Struct_FOC;

#endif  //#ifndef FOCSTRUCT_H
