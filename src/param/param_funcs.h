
/**
 * @file param_funcs.h
 * @brief Use for parameter processing
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

#ifndef PARAMETER_PROCESS_H
#define PARAMETER_PROCESS_H

#include "param_calc.h"
#include "config_app.h"

#if (AIO10_MODE == AIO50_MODE_IO)
#define VALUE_CFGAIO0       ((AIO10_MODE << 6) | (AIO0_OPT << 4) | (AIO0_POL << 3) | AIO0_MUX)
#define VALUE_CFGAIO1       ((AIO1_OPT << 4) | (AIO1_POL << 3) | AIO1_MUX)
#elif (AIO10_MODE == AIO50_MODE_DIFFAMP)
#define VALUE_CFGAIO0       ((AIO10_MODE << 6) | (AIO10_GAIN << 3) | AIO10_LPCOMP)
#ifdef ENABLE_CBC_CURRENT_LIMIT
#define VALUE_CFGAIO1       ((AIO10_HPROT << 6) | (AIO10_LPROT << 4) | (AIO10_OFFSET << 3) | AIO10_HPCOMP)
#else
#define VALUE_CFGAIO1       ((AIO10_LPROT << 4) | (AIO10_OFFSET << 3))
#endif
#endif

#if (AIO32_MODE == AIO50_MODE_IO)
#define VALUE_CFGAIO2       ((AIO32_MODE << 6) | (AIO2_OPT << 4) | (AIO2_POL << 3) | AIO2_MUX)
#define VALUE_CFGAIO3       ((AIO3_OPT << 4) | (AIO3_POL << 3) | AIO3_MUX)
#elif (AIO32_MODE == AIO50_MODE_DIFFAMP)
#define VALUE_CFGAIO2       ((AIO32_MODE << 6) | (AIO32_GAIN << 3) | AIO32_LPCOMP)
#ifdef ENABLE_CBC_CURRENT_LIMIT
#define VALUE_CFGAIO3       ((AIO32_HPROT << 6) | (AIO32_LPROT << 4) | (AIO32_OFFSET << 3) | AIO32_HPCOMP)
#else
#define VALUE_CFGAIO3       ((AIO32_LPROT << 4) | (AIO32_OFFSET << 3))
#endif
#endif

#if (AIO54_MODE == AIO50_MODE_IO)
#define VALUE_CFGAIO4       ((AIO54_MODE << 6) | (AIO4_OPT << 4) | (AIO4_POL << 3) | AIO4_MUX)
#define VALUE_CFGAIO5       ((AIO5_OPT << 4) | (AIO5_POL << 3) | AIO5_MUX)
#elif (AIO54_MODE == AIO50_MODE_DIFFAMP)
#define VALUE_CFGAIO4       ((AIO54_MODE << 6) | (AIO54_GAIN << 3) | AIO54_LPCOMP)
#ifdef ENABLE_CBC_CURRENT_LIMIT
#define VALUE_CFGAIO5       ((AIO54_HPROT << 6) | (AIO54_LPROT << 4) | (AIO54_OFFSET << 3) | AIO54_HPCOMP)
#else
#define VALUE_CFGAIO5       ((AIO54_LPROT << 4) | (AIO54_OFFSET << 3))
#endif
#endif


#if (AIO6_MODE == AIO6789_MODE_IO)
#define VALUE_CFGAIO6       ((AIO6_MODE << 6) | (AIO6_OPT << 4) | (AIO6_POL << 3) | AIO6_MUX)
#elif (AIO6_MODE == AIO6789_MODE_GAIN)
#define VALUE_CFGAIO6       ((AIO6_MODE << 6) | (AIO6_GAIN << 3) | AIO6_MUX)
#endif

#if (AIO7_MODE == AIO6789_MODE_IO)
#define VALUE_CFGAIO7       ((AIO7_MODE << 6) | (AIO7_OPT << 4) | (AIO7_POL << 3) | AIO7_MUX)
#elif (AIO7_MODE == AIO6789_MODE_GAIN)
#define VALUE_CFGAIO7       ((AIO7_MODE << 6) | (AIO7_GAIN << 3) | AIO7_MUX)
#endif

#if (AIO8_MODE == AIO6789_MODE_IO)
#define VALUE_CFGAIO8       ((AIO8_MODE << 6) | (AIO8_OPT << 4) | (AIO8_POL << 3) | AIO8_MUX)
#elif (AIO8_MODE == AIO6789_MODE_GAIN)
#define VALUE_CFGAIO8       ((AIO8_MODE << 6) | (AIO8_GAIN << 3) | AIO8_MUX)
#endif

#if (AIO9_MODE == AIO6789_MODE_IO)
#define VALUE_CFGAIO9       ((AIO9_MODE << 6) | (AIO9_OPT << 4) | (AIO9_POL << 3) | AIO9_MUX)
#elif (AIO9_MODE == AIO6789_MODE_GAIN)
#define VALUE_CFGAIO9       ((AIO9_MODE << 6) | (AIO9_GAIN << 3) | AIO9_MUX)
#endif


/**
 * @brief Use to write FOC parameters as per function code
 * @param ParameterCode param_code
 * @param fix16_t rx_data
 * @param Struct_Motor* motor_ptr
 */
extern void param_write(ParameterCode param_code, fix16_t rx_data, Struct_Motor* motor_ptr);

/**
 * @brief Use to write FOC parameters as per function code
 * @param ParameterCode param_code
 * @param fix16_t rx_data
 * @param Struct_Motor* motor_ptr
 */
extern void param_read(ParameterCode param_code, Struct_Motor* motor_ptr);

/**
 * @brief Use to initialize default FOC parameters
 * @param Struct_Motor* motor_ptr
 */
extern void init_default_parameters(Struct_Motor* motor_ptr);

/**
 * @brief Use to check motor states before writing parameters, post error if not valid
 * @param Struct_Motor* motor_ptr
 */
extern void check_param_while_run(Struct_Motor* motor_ptr);

/**
 * @brief Use to write itotal current
 * @param int32_t iref_q16
 * @param int32_t *itotref_target_q16
 * @param int32_t *itotref_target_1div_q16
 */
extern void write_itotstart_target(int32_t iref_q16, int32_t *itotref_target_q16, int32_t *itotref_target_1div_q16);

/**
 * @brief Use to initialize diff amp
 */
extern void init_diff_amp_gain(void);

/**
 * @brief Use to initialize over current limit
 * @param uint32_t current_limit
 */
extern void init_over_current_limit(uint32_t current_limit);


/**
 * @brief Use to pass data to TX variable
 * @param uint32_t data
 */
static inline void int_to_buffer(uint32_t data)
{
    Struct_Motor* motor_ptr = &motor;

    motor_ptr->comm.tx_data = (data);
}

#endif


