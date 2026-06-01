
/**
 * @file app_funcs.h
 * @brief Use to reset variables associated to FOC, Control, Faults, Stall, OPD, PPM
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

#ifndef APP_FUNCS_H
#define APP_FUNCS_H

#include "pac5xxx_driver_timer.h"
#include "motor_struct.h"
#include "fault.h"
#include "control_funcs.h"
#include "param_funcs.h"

#ifdef ENABLE_SIM
#include "sim_funcs.h"
#endif

#ifdef ENABLE_MTPA
#include "mtpa_funcs.h"
#endif

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#include "control_power_funcs.h"
#endif

#if defined(ENABLE_AUTOTUNE_PI)
#include "atune_funcs.h"
#endif

#if defined(ENABLE_HALL_TUNING)
#include "halltune_funcs.h"
#endif

#ifdef DISABLE_AUTOVDC
#include "vbus_funcs.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
#include "harmonic_inj_funcs.h"
#endif

#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
#include "vbus_funcs.h"
#endif

///////////////////////////////////////////////////////
// Function Declarations
///////////////////////////////////////////////////////

/**
 * @brief  This function use to reset variables associated to FOC, Control, Faults, Stall, OPD, PPM
 * @param  Struct_Motor* motor_ptr pointer to motor structure
 */
void motor_reset(Struct_Motor* motor_ptr);
extern void systemvar_init(Struct_Motor* motor_ptr);
extern void sm_mainsub_post(Struct_Motor* motor_ptr);
extern void sm_mainsub_prestart(void);
extern void statemachine_appmain(void);
extern void store_peak_speed_vdrive_value(Struct_Motor* motor_ptr);
extern void app_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data);
extern void diff_amp_gain_param_write(Struct_Motor* motor_ptr, fix16_t rx_data);
extern void hpdac_param_write(uint32_t lpdac_value, char lpdac_factor, uint32_t current_limit);
extern void lpdac_param_write(uint32_t lpdac_value);
extern fix16_t arch_type_param_read(fix16_t* value);
extern void brake_while_stopped_enter(Struct_Motor* motor_ptr);
extern void brake_while_stopped_exit(Struct_Motor* motor_ptr);


///////////////////////////////////////////////////////
// Function Definitions
///////////////////////////////////////////////////////

/**
 * @brief  This function use to reset variables associated to FOC, Control, Faults, Stall, OPD, PPM
 * @param  Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void motor_init(Struct_Motor *motor_ptr)
{
    motor_reset(motor_ptr);
    // Initilize the OL speed target while powering up at the first time
    motor_ptr->app.app_motor_direction = 0;
    oc_reset();

#ifdef ENABLE_SIM
    sim_init(motor_ptr);
#endif

#ifdef ENABLE_MTPA
    mtpa_init(motor_ptr);
#endif

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
    power2_calc_param(motor_ptr);
#endif

#if defined(ENABLE_AUTOTUNE_PI)
    at_reset_state(motor_ptr);

#if defined(ENABLE_AUTOTUNE_MID)
    mid1_init(motor_ptr);
    mid2_init(motor_ptr);
#endif
    atpi_reset_vars(motor_ptr);
#endif

#if defined(ENABLE_HALL_TUNING)
    halltune_init(motor_ptr);
#endif

#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
    hinj_init(motor_ptr);
#endif

#ifdef DISABLE_AUTOVDC
    vbus_fixed_init(motor_ptr);
#endif
}


/**
 * @brief  This function use to initialize ADC channel offset variables to default value
 * @param  Struct_Motor* motor_ptr pointer to motor structure
 */
#ifndef ENABLE_ADCCAL_POWERUP
static inline void set_default_adc_offset(Struct_Motor *motor_ptr)
{
#if (CONFIG_OFFSET & CHANNEL_AIO10)
    motor_ptr->post.adc_offset_aio10 = ADC_OFFSET_DEFAULT_DIFFAMP << 4;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO32)
    motor_ptr->post.adc_offset_aio32 = ADC_OFFSET_DEFAULT_DIFFAMP << 4;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO54)
    motor_ptr->post.adc_offset_aio54 = ADC_OFFSET_DEFAULT_DIFFAMP << 4;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO6)
    motor_ptr->post.adc_offset_aio6 = (ADC_OFFSET_DEFAULT_AIO6 * AMPGAIN_AIO6) << 4;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO7)
    motor_ptr->post.adc_offset_aio7 = (ADC_OFFSET_DEFAULT_AIO7 * AMPGAIN_AIO7) << 4;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO8)
    motor_ptr->post.adc_offset_aio8 = (ADC_OFFSET_DEFAULT_AIO8 * AMPGAIN_AIO8) << 4;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO9)
    motor_ptr->post.adc_offset_aio9 = (ADC_OFFSET_DEFAULT_AIO9 * AMPGAIN_AIO9) << 4;
#endif
}
#endif  //#ifndef ENABLE_ADCCAL_POWERUP

/**
 * @brief  This function use read motor phase current and power measure current ADC channel offset value
 * @param  Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void read_adc_offsets(Struct_Motor *motor_ptr)
{
    read_adcoffset_iphase(motor_ptr);

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
    power2_read_adcoffset(motor_ptr);
#endif
}


#if defined(ENABLE_TEMPREAD)
static inline void temp_read(void)
{
    // Convert ADC value to voltage
    debug_var_1 = readseq_temperature() * CONV_ADC2VAIO_Q10Q16;
}
#endif


#endif  //#ifndef APP_FUNCS_H
