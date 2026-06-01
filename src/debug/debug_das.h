
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

#ifndef DEBUG_DAS_H
#define DEBUG_DAS_H

#include "motor_struct.h"
#include "pac5xxx.h"
#include "config_debug.h"
#include "config_app.h"
#include "board_options.h"
#include "debug_dac.h"
#include "debug_io.h"

#ifdef ENABLE_DEBUG_DAS
#if (DAS_VERSION == DAS_VERSION_1)
typedef struct
{
    unsigned short offset[4];
    fix16_t scale[4];
    unsigned short value[6];
} debug_pointer;
extern debug_pointer debug;
extern int debug_channel_offset;

void das_debug_init(void);

PAC5XXX_RAMFUNC static inline void run_debug_das(void)
{
#ifdef ENABLE_DEBUG_DAS
    int32_t temp_value1_das;
    int32_t temp_value2_das;
    int32_t temp_value3_das;
    int32_t temp_value4_das;

    Struct_Motor *motor_ptr = &motor;

    // Calculations for Debug DAC's
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.hall.hall_interpolated_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.est.estimated_angle >> 5;
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.mpos_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.qep.qep_angle >> 5;
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.hall.hall_interpolated_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->debug.angle_q14;
#elif (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.mpos_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.est.estimated_angle >> 5;
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.qep.qep_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.est.estimated_angle >> 5;
#endif

#ifdef DEBUGDAC_DISPLAYDUTY_TICKS
    motor_ptr->debug.duty_value_u_q14 = ((motor_ptr->control.u_duty_value - (motor_ptr->control.motor_period_ticks >> 1)) * motor_ptr->control.motor_period_ticks_1div) >> 2;
    motor_ptr->debug.duty_value_v_q14 = ((motor_ptr->control.v_duty_value - (motor_ptr->control.motor_period_ticks >> 1)) * motor_ptr->control.motor_period_ticks_1div) >> 2;
    motor_ptr->debug.duty_value_w_q14 = ((motor_ptr->control.w_duty_value - (motor_ptr->control.motor_period_ticks >> 1)) * motor_ptr->control.motor_period_ticks_1div) >> 2;
#endif

    temp_value1_das = (PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr1 * debug.scale[0]) >> 22); // q0 = q14 * q8 = q22>>22
    temp_value2_das = (PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr2 * debug.scale[1]) >> 22); // q0 = q14 * q8 = q22>>22
    temp_value3_das = (PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr3 * debug.scale[2]) >> 22); // q0 = q14 * q8 = q22>>22
    temp_value4_das = (PWMDAC_PERIOD_TICKS >> 1) + ((*dac_ptr4 * debug.scale[3]) >> 22); // q0 = q14 * q8 = q22>>22

    debug.value[debug_channel_offset] = temp_value1_das << 2;
    debug.value[debug_channel_offset + 1] = temp_value2_das << 2;
    debug.value[debug_channel_offset + 2] = temp_value3_das << 2;
    debug.value[debug_channel_offset + 3] = temp_value4_das << 2;

    //////// trigger DAS pin toggle///////////////////////////////
    debugDASio_on();
    debugDASio_off();
    //////// trigger DAS pin toggle///////////////////////////////
#endif
}

#elif (DAS_VERSION == DAS_VERSION_2)
#define DAS_DAC_MAX_VALUE           4095
#define DAS_SCALE_COEFFICIENT       1024

typedef union
{
    struct
    {
        uint32_t dac0 : 12;
        uint32_t reserved : 4;
        uint32_t dac1 : 12;
        uint32_t key : 4;
    };   
    uint32_t w;
} swd_payload_t;

typedef struct
{
    swd_payload_t word0;
    swd_payload_t word1;
    fix16_t scale[4];
} das_data_t;

extern das_data_t debug;

void das_debug_init(void);

PAC5XXX_RAMFUNC static inline void run_debug_das(void)
{
    // Calculations for Debug DAC's
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    motor.debug.angle_q14 = motor.mpos.hall.hall_interpolated_angle >> 5;
    motor.debug.estimated_angle_q14 = motor.mpos.est.estimated_angle >> 5;
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    motor.debug.angle_q14 = motor.mpos.mpos_angle >> 5;
    motor.debug.estimated_angle_q14 = motor.mpos.qep.qep_angle >> 5;
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    motor.debug.angle_q14 = motor.mpos.mpos_angle >> 5;
    motor.debug.estimated_angle_q14 = motor.mpos.hall.hall_interpolated_angle >> 5;
#elif (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    motor.debug.angle_q14 = motor.mpos.mpos_angle >> 5;
    motor.debug.estimated_angle_q14 = motor.mpos.est.estimated_angle >> 5;
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
    motor.debug.angle_q14 = motor.mpos.qep.qep_angle >> 5;
    motor.debug.estimated_angle_q14 = motor.mpos.est.estimated_angle >> 5;
//    motor.debug.estimated_angle_q14 = motor.mpos.mpos_angle >> 5;
#endif

#ifdef DEBUGDAC_DISPLAYDUTY_TICKS
    motor.debug.duty_value_u_q14 = ((motor.control.u_duty_value - (motor.control.motor_period_ticks >> 1)) * motor.control.motor_period_ticks_1div) >> 2;
    motor.debug.duty_value_v_q14 = ((motor.control.v_duty_value - (motor.control.motor_period_ticks >> 1)) * motor.control.motor_period_ticks_1div) >> 2;
    motor.debug.duty_value_w_q14 = ((motor.control.w_duty_value - (motor.control.motor_period_ticks >> 1)) * motor.control.motor_period_ticks_1div) >> 2;
#endif

    int32_t temp_value1_das = (DAS_DAC_MAX_VALUE >> 1) + ((*dac_ptr1 * debug.scale[0]) >> 14); // q0 = q14 * q0 = q14>>14
    int32_t temp_value2_das = (DAS_DAC_MAX_VALUE >> 1) + ((*dac_ptr2 * debug.scale[1]) >> 14); // q0 = q14 * q0 = q14>>14
    int32_t temp_value3_das = (DAS_DAC_MAX_VALUE >> 1) + ((*dac_ptr3 * debug.scale[2]) >> 14); // q0 = q14 * q0 = q14>>14
    int32_t temp_value4_das = (DAS_DAC_MAX_VALUE >> 1) + ((*dac_ptr4 * debug.scale[3]) >> 14); // q0 = q14 * q0 = q14>>14

    if (temp_value1_das < 0) temp_value1_das = 0;
    if (temp_value1_das > DAS_DAC_MAX_VALUE) temp_value1_das = DAS_DAC_MAX_VALUE;

    if (temp_value2_das < 0) temp_value2_das = 0;
    if (temp_value2_das > DAS_DAC_MAX_VALUE) temp_value2_das = DAS_DAC_MAX_VALUE;

    if (temp_value3_das < 0) temp_value3_das = 0;
    if (temp_value3_das > DAS_DAC_MAX_VALUE) temp_value3_das = DAS_DAC_MAX_VALUE;

    if (temp_value4_das < 0) temp_value4_das = 0;
    if (temp_value4_das > DAS_DAC_MAX_VALUE) temp_value4_das = DAS_DAC_MAX_VALUE;

    static uint32_t key;
    key++;
    key = key % 16;

    debug.word0.w = (temp_value1_das) | (temp_value2_das << 16) | (key << 28);
    debug.word1.w = (temp_value3_das) | (temp_value4_das << 16) | (key << 28);

    //////// trigger DAS pin toggle///////////////////////////////
    debugDASio_off();
    debugDASio_on();
    //////// trigger DAS pin toggle///////////////////////////////
}
#else
#error "DAS_VERSION has not been selected"
#endif

#endif // #ifdef ENABLE_DEBUG_DAS
#endif //#ifndef DEBUG_DAS_H
