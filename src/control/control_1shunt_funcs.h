
/**
 * @file control_1shunt_funcs.h
 * @brief Use for single shunt control functions
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

#ifndef CONTROL_1SHUNT_FUNCS_H
#define CONTROL_1SHUNT_FUNCS_H

#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "control_1shunt_struct.h"
#include "adc_sample.h"
#include "isr_funcs.h"

#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
#include "control_1shunt_funcs_extn.h"
#elif (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
#include "control_1shunt_funcs_asym.h"
#elif (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
#include "control_1shunt_funcs_asym2.h"
#endif


//==========================================================================
//
// ADC Offset Configuration
//
//==========================================================================


// JAGTAG: Need to rename either power or 1shunt and 3shunt ADC read macros/functions

//=====================================
// Seq Read Register Mapping for FOC
//=====================================
#define SEQRD_FOC_I1                    AS0R0
#define SEQRD_FOC_I2                    AS1R0

#define SEQRD_FOC_VBUS                  AS1R1

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#define SEQRD_FOC_IBUSAVG               AS1R2
#endif


//=====================================
// ADC Offset Channel Enable
// Seq Read Register Mapping for ADCCAL
//=====================================
#if defined(SELECT_AIO_ISHUNT)

#if (SELECT_AIO_ISHUNT == CHANNEL_AIO10)
    #define ENABLE_ADCCAL_AIO10
    #define SEQRD_CAL_AIO10                     SEQRD_FOC_I2
#elif (SELECT_AIO_ISHUNT == CHANNEL_AIO32)
    #define ENABLE_ADCCAL_AIO32
    #define SEQRD_CAL_AIO32                     SEQRD_FOC_I2
#elif (SELECT_AIO_ISHUNT == CHANNEL_AIO54)
    #define ENABLE_ADCCAL_AIO54
    #define SEQRD_CAL_AIO54                     SEQRD_FOC_I2
#else
    #error: SELECT_AIO_ISHUNT is set to unsupported channel
#endif

#else   //#if defined(SELECT_AIO_ISHUNT)
    #error: SELECT_AIO_ISHUNT has not been defined in the board file.
#endif  //#if defined(SELECT_AIO_ISHUNT)

// ADC Sequencer Readout Macro Functions
#if 0
static inline int16_t readseq_ssi1(void)                       { return PAC5XXX_ADC->SEQRD_FOC_I1.VAL; }
static inline int16_t readseq_ssi2(void)                       { return PAC5XXX_ADC->SEQRD_FOC_I2.VAL; }
#endif


/**
 * @brief This function use to read adc offset for phase currents
 * @param intStruct_Motor *motor_ptr
 */
static inline void read_adcoffset_iphase(Struct_Motor *motor_ptr)
{
#if defined(SELECT_AIO_ISHUNT)

#if (SELECT_AIO_ISHUNT == CHANNEL_AIO10)
    motor_ptr->oneshunt.adc_offset_iphase_ss_q14 = motor_ptr->post.adc_offset_aio10;
#elif (SELECT_AIO_ISHUNT == CHANNEL_AIO32)
    motor_ptr->oneshunt.adc_offset_iphase_ss_q14 = motor_ptr->post.adc_offset_aio32;
#elif (SELECT_AIO_ISHUNT == CHANNEL_AIO54)
    motor_ptr->oneshunt.adc_offset_iphase_ss_q14 = motor_ptr->post.adc_offset_aio54;
#else
    #error: SELECT_AIO_ISHUNT is set to unsupported channel
#endif

#else   //#if defined(SELECT_AIO_ISHUNT)
    #error: SELECT_AIO_ISHUNT has not been defined in the board file.
#endif  //#if defined(SELECT_AIO_ISHUNT)
}



/**
 * @brief This function use for reading phase currents
 * @param intStruct_Motor *motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void read_phase_currents(Struct_Motor *motor_ptr)
{
    int32_t phase_current1, phase_current2;

    // Read current values from ADC
    phase_current1 = ((motor_ptr->oneshunt.read_phase_current1 * ADC_SCALE_CONST) >> 14);          //0q14 = "0q14" * 5q16 = 5q30>>16
    phase_current1 = ((phase_current1 * COMP_DIFFGAINERROR_Q14) >> 14);                       // q14 = 0q14 * 1q14 = 1q28>>14
    phase_current1 = ((phase_current1 - motor_ptr->oneshunt.adc_offset_iphase_ss_q14) << 1);
    motor_ptr->oneshunt.phase_current1 = phase_current1;

    phase_current2 = ((motor_ptr->oneshunt.read_phase_current2 * ADC_SCALE_CONST) >> 14);          //0q14 = "0q14" * 5q16 = 5q30>>16
    phase_current2 = ((phase_current2 * COMP_DIFFGAINERROR_Q14) >> 14);                       // q14 = 0q14 * 1q14 = 1q28>>14
    phase_current2 = ((phase_current2 - motor_ptr->oneshunt.adc_offset_iphase_ss_q14) << 1);
    motor_ptr->oneshunt.phase_current2 = phase_current2;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use for process phase currents
 * @param intStruct_Motor *motor_ptr
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void process_phase_currents(Struct_Motor* motor_ptr)
{
    read_phase_currents(motor_ptr);

#if ((MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC) || (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY))
    process_phase_currents_1shunt_asym(&motor_ptr->oneshunt, &motor_ptr->control.Iu_measured_q14, &motor_ptr->control.Iv_measured_q14, &motor_ptr->control.Iw_measured_q14, motor_ptr->control.mod_sector);
#elif (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    process_phase_currents_1shunt_extn(&motor_ptr->oneshunt, &motor_ptr->control.Iu_measured_q14, &motor_ptr->control.Iv_measured_q14, &motor_ptr->control.Iw_measured_q14, motor_ptr->control.mod_sector);
#endif

    // Shift sector register
    motor_ptr->control.mod_sector = motor_ptr->control.mod_sector_sreg;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief This function use to calculate min-max foc duty cycle
 * @param intStruct_Motor *motor_ptr
 */
static inline void foc_config_duty(Struct_Motor *motor_ptr)
{
    // Duty min cannot get to 1000% in 1-shunt
    motor_ptr->foc.svm_max_ticks = (motor_ptr->control.motor_period_ticks - PULSEWIDTH_MIN_LS_TICKS - motor_ptr->oneshunt.s1_min_gap);   // Timer ticks in SVM max for this motor (for SVM)

    // Duty min cannot get to 0% in 1-shunt
    motor_ptr->foc.svm_min_ticks = (PULSEWIDTH_MIN_HS_TICKS + motor_ptr->oneshunt.s1_min_gap);                                           // Timer ticks in SVM min for this motor (for SVM)
}

#endif  //#ifndef CONTROL_1SHUNT_FUNCS_H
