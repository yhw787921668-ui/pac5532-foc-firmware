
/**
 * @file vbus_funcs.c
 * @brief Use to calculate internal and external vbus and slope factor from flash
 */

//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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

#include "motor_struct.h"
#include "vbus_funcs.h"
#include "foc.h"
#include "mpos_funcs.h"
#include "param_calc.h"
#include "fault.h"
#include "control_funcs.h"
#include "foc_funcs.h"

#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS) || defined(ENABLE_DEBUG_RTT)
#include "debug_dac.h"
#endif

#ifdef ENABLE_DEBUG_DISPLAY
#include "debug_display.h"
#endif

#ifdef ENABLE_OPDOL
#include "opd_ol_funcs.h"
#endif

#ifdef ENABLE_STALL_DETECT
#include "stalldet_funcs.h"
#endif

#if (COMMSEL_PPM == TRUE)
#include "comm.h"
#endif

#ifdef ENABLE_IPD
#include "ipd_config.h"
#endif

#ifdef ENABLE_FLASH_CRC_TEST
#include "pac5xxx_flash_test.h"
#endif

#if defined(ENABLE_POWERMEASURE)
#include "control_power_funcs.h"
#endif

#if defined(ENABLE_AUTOTUNE_PI)
#include "atune_funcs.h"
#endif

#if defined(ENABLE_OPD_CL)
#include "opd_cl_funcs.h"
#endif


/**
 * @brief This function use to calculate vms constant
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#if (INTERNAL_VMS == TRUE)
void cal_vm_const (Struct_Motor* motor_ptr)
{
    // Convert VMS100 and VMS200 in mV to q16
    motor_ptr->vms_v100 = READ_UINT16(VMS100_ADDRESS) << 16;
    motor_ptr->vms_v200 = READ_UINT16(VMS200_ADDRESS) << 16;
    motor_ptr->one_div_vms200_minus_vms100 = fix16_div(FIX16_ONE, fix16_sub(motor_ptr->vms_v200, motor_ptr->vms_v100));
}
#endif


/**
 * @brief This function use to calculate average value of dc bus voltage
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#ifndef DISABLE_AUTOVDC
void sample_dcbus(Struct_Motor* motor_ptr)
{
    long int vbus_vms;
    volatile unsigned int adc_counts;

#if (INTERNAL_VMS == TRUE)
#if (VMS_FACTORY_CAL == TRUE)
    adc_counts = motor_ptr->vms_adc_vlaue;
#else
    adc_counts = readseq_vbus_vms(read_vbus_adc_count(motor_ptr));
#endif
    vbus_vms = calc_vbus_internal(motor_ptr, adc_counts);
#else
    adc_counts = readseq_vbus(read_vbus_adc_count(motor_ptr));

    vbus_vms = calc_vbus_external(motor_ptr, adc_counts);
#endif

    // Filter bus voltage
    filter_shift2(vbus_vms, &motor_ptr->app.dcbus_voltage, 2);

    // Calculate voltage factors
    vbus_calc_vfactors(motor_ptr);
}
#endif

/**
 * @brief This function use to measure dc bus voltage in control loop
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
PAC5XXX_OPTIMIZE_SPEED_ON
void read_dc_voltage_fast(Struct_Motor* motor_ptr)
{
    long int vbus_vms, temp_q16;

#if (INTERNAL_VMS == TRUE)
    vbus_vms = calc_vbus_internal(motor_ptr, motor_ptr->vms_adc_vlaue);
#else
    vbus_vms = calc_vbus_external(motor_ptr, readseq_vbus(read_vbus_adc_count(motor_ptr)));
#endif
    //debugio1_on();
    // Filter bus voltage
    filter_shift2(vbus_vms >> 2, &motor_ptr->app.dcbus_voltage_fast_q14, 2);
    motor_ptr->app.dcbus_voltage_ave_q14 = filter_alpha_q14(motor_ptr->app.dcbus_voltage_fast_q14, 
                                                        motor_ptr->app.dcbus_voltage_ave_q14, 
                                                        motor_ptr->app.dc_voltage_filter_coff_q16);
    temp_q16 = div_fast(FIX16_1P0, motor_ptr->app.dcbus_voltage_fast_q14 << 2);
    motor_ptr->app.dcbus_voltage_compensation_coff_q26 = (temp_q16 * (motor_ptr->app.dcbus_voltage_ave_q14 >> 4));    // q16*q10 = q26
    if (motor_ptr->app.dcbus_voltage_compensation_coff_q26 > DC_VOLTAGE_COMPENSATION_LIMIT_Q26)
    {
        motor_ptr->app.dcbus_voltage_compensation_coff_q26 = DC_VOLTAGE_COMPENSATION_LIMIT_Q26;
    }
    //debugio1_off();
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief This function use to initialize dc voltage measure variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void read_dc_voltage_fast_init(Struct_Motor* motor_ptr)
{
    long int temp;
#if (INTERNAL_VMS == TRUE)
    temp = calc_vbus_internal(motor_ptr, motor_ptr->vms_adc_vlaue);
#else
    temp = calc_vbus_external(motor_ptr, readseq_vbus(read_vbus_adc_count(motor_ptr)));
#endif
    motor_ptr->app.dcbus_voltage_fast_q14 = motor_ptr->app.dcbus_voltage_ave_q14 = (temp >> 2);
}

/**
 * @brief This function use to reset dc voltage measure variables
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
void read_dc_voltage_fast_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->app.dcbus_voltage_compensation_coff_q26 = QCONST(1.0f, Q26);
#ifdef ENABLE_CONTROLLER_INTERLEAVE
    motor_ptr->app.dc_voltage_filter_coff_q16 = calc_filter_alpha(DC_COMPENSATION_CUTOFF_FREQ_Q16, motor_ptr->control.dt_control_q26 << 1);
    motor_ptr->app.gear_mode = 1;
#else
    motor_ptr->app.dc_voltage_filter_coff_q16 = calc_filter_alpha(DC_COMPENSATION_CUTOFF_FREQ_Q16, motor_ptr->control.dt_control_q26);
#endif
}
#endif

