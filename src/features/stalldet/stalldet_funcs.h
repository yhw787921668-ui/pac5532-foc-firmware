
/**
 * @file stalldet_funcs.h
 * @brief Use for stall detect utility functions
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

#ifndef STALLDET_FUNCS_H
#define STALLDET_FUNCS_H

#include "stalldet.h"
#include "motor_struct.h"
#include "param_funcs.h"


/**
 * @brief  Use for stall detector filter
 * @param Struct_Motor* motor_ptr
 * @param fix16_t outerloop_ramp
 * @param fix16_t outerloop_fdbk
 */
extern void stall_detect_filter(Struct_StallDet* stall_ptr, fix16_t outerloop_ramp, fix16_t outerloop_fdbk);

/**
 * @brief  Use for stall detection processing
 * @param Struct_Motor* motor_ptr
 */
extern void stall_detect_process(Struct_Motor* motor_ptr);

/**
 * @brief  Use to calculate stall detection sample time
 * @param Struct_Motor* motor_ptr
 * @param int32_t sample_window_freq_radps_q16
 */
void stall_detect_calculate_stall_sample_time(Struct_Motor* motor_ptr, int32_t sample_window_freq_radps_q16);

/**
 * @brief  Use to reset stall detect variables
 * @param Struct_Motor* motor_ptr
 */
extern void stalldet_reset(Struct_Motor* motor_ptr);

/**
 * @brief  Use to stall detect filter parameters
 * @param Struct_Motor* motor_ptr
 */
extern void stalldet_calc_filter_alpha(Struct_Motor* motor_ptr);

/**
 * @brief  Use to write stall detect parameters
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
extern void stalldet_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);

/**
 * @brief  Use to process stall detect parameters
 * @param Struct_Motor* motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
extern void stalldet_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);

#endif
