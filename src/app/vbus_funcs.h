
/**
 * @file vbus_funcs.h
 * @brief Use to calculate internal and external vbus and slope factor from flash and duty cycle exclusion
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

#ifndef VBUS_FUNCS_H
#define VBUS_FUNCS_H

#include "pac5xxx_driver_timer.h"
#include "motor_struct.h"
#include "fault.h"
#include "control_funcs.h"
#include "param_funcs.h"
#include "mod.h"

#if (INTERNAL_VMS == TRUE)
#define CONV_ADC2DCBUS_Q14                              ((int32_t) (0.000976563 * 2.5 * VMS_INTERNAL_DIVIDE_RATIO * (16384 >> (ADC_BITS_FIRMWARE_SELECTION - 10))))                  // 1/1024 * 2.5V * (R1 + R2)/R2 in 1Q14 format
#else
#define CONV_ADC2DCBUS_Q14                              ((int32_t) (0.000976563 * 2.5 * R1_PLUS_R2_BY_R2_RATIO_VBUS * (16384 >> (ADC_BITS_FIRMWARE_SELECTION - 10))))                // 1/1024 * 2.5V * (R1 + R2)/R2 in 1Q14 format
#endif

#if defined(ENABLE_POWERMEASURE) && (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#include "control_power_funcs.h"
#endif

#if defined(ENABLE_AUTOTUNE_PI)
#include "atune_funcs.h"
#endif

#if defined(DISABLE_AUTOVDC) && (MODULE_MPOS_LBG1 == ENABLED)
#include "mpos_funcs.h"
#endif

///////////////////////////////////////////////////////
// Function Declarations
///////////////////////////////////////////////////////

extern void sample_dcbus(Struct_Motor* motor_ptr);
#ifdef ENABLE_DC_VOLTAGE_COMPENSATION
extern void read_dc_voltage_fast_init(Struct_Motor* motor_ptr);
extern void read_dc_voltage_fast_reset(Struct_Motor* motor_ptr);
extern void read_dc_voltage_fast(Struct_Motor* motor_ptr);
#endif
extern fix16_t calc_vm(Struct_Motor* motor_ptr, unsigned int adc_counts);


#if (INTERNAL_VMS == TRUE)
extern void cal_vm_const(Struct_Motor* motor_ptr);
#endif

#if defined(ENABLE_DC_VOLTAGE_COMPENSATION)
#include "fast_div.h"
#endif


/**
 * @brief This function use to read value from ADC result
 * @param Struct_Motor* motor_ptr pointer to motor structure
 * @return adc raw count value.
 */
static inline int16_t read_vbus_adc_count(Struct_Motor* motor_ptr)
{
    fix16_t adc_counts;

    adc_counts = (motor.fptr.pSeq_add->w) & 0xFFFF;
    return adc_counts;
}

/**
 * @brief This function use to calculate vbus external
 * @param Struct_Motor* motor_ptr pointer to motor structure
 * @param unsigned int adc_counts
 */
#ifndef DISABLE_AUTOVDC
#if (INTERNAL_VMS != TRUE)
PAC5XXX_OPTIMIZE_SPEED_ON
static inline fix16_t calc_vbus_external(Struct_Motor* motor_ptr, unsigned int adc_counts)
{
    return (fix16_t) (adc_counts * (CONV_ADC2DCBUS_Q14 << 2));                        // Q0 * (Q14 << 2) = Q16
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif      // #if (INTERNAL_VMS != TRUE)
#endif      // #ifndef DISABLE_AUTOVDC

// The function below calculates the equation VM = 100V * (VMS_ADC – VMS100)/(VMS200 – VMS100) + 100V
// Note that VMS ADC counts (adc_counts) gets scaled by 2500 mV / 1023 counts to convert it to mV
// Input : Raw adc counts, q0 format
// Output: VM (VBUS) Volts in Q16 format

/**
 * @brief This function use to calculate vbus internal
 * @param Struct_Motor* motor_ptr pointer to motor structure
 * @param unsigned int adc_counts
 */
#if (INTERNAL_VMS == TRUE)
PAC5XXX_OPTIMIZE_SPEED_ON
static inline fix16_t calc_vbus_internal(Struct_Motor* motor_ptr, unsigned int adc_counts)
{
    fix16_t vms_adc;
#if (VMS_FACTORY_CAL == TRUE)
    fix16_t temp_vms, vm;

    vms_adc = adc_counts;
    vms_adc = vms_adc * Q16_mV_PER_ADC_COUNT;
    temp_vms = fix16_mul( (vms_adc - motor_ptr->vms_v100), motor_ptr->one_div_vms200_minus_vms100);
    vm = fix16_mul(Q16_100, temp_vms) + Q16_100;
    return vm;
#else
    vms_adc = adc_counts;
    return (fix16_t) ((vms_adc) * (CONV_ADC2DCBUS_Q14 << 2));                        // Q0 * (Q14 << 2) = Q16
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief This function use for checking is vbus sample valid
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#if (VMS_DUTY_CYCLE_EXCLUSION == TRUE)
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void check_duty_cycle_exclusion(Struct_Motor *motor_ptr)
{
    static int32_t vms_adc_vlaue_q3 = 0;
    static int32_t exclude_flag = FALSE;

    if (!exclude_flag)
    {
          motor_ptr->vms_adc_vlaue = filter_shift(readseq_vbus_vms(read_vbus_adc_count(motor_ptr)), &vms_adc_vlaue_q3, 3);
    }

    exclude_flag = FALSE;

#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    if(motor_ptr->control.u_duty_value >= motor_ptr->u_duty_cycle_max_limit)
    {
       exclude_flag = TRUE;
    }
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    if (((motor_ptr->control.u_duty_value > motor_ptr->control.v_duty_value) &&
        (motor_ptr->control.u_duty_value > motor_ptr->control.w_duty_value))
        ||
        ((motor_ptr->control.v_duty_value > motor_ptr->control.u_duty_value) &&
        (motor_ptr->control.u_duty_value > motor_ptr->control.w_duty_value) &&
        ((motor_ptr->control.v_duty_value - motor_ptr->control.u_duty_value) <  VMS_VALUE_EXCLUDE_BAND_LIMIT_SIGNLE_SHUNT))
        ||
        ((motor_ptr->control.w_duty_value > motor_ptr->control.u_duty_value) &&
        (motor_ptr->control.u_duty_value > motor_ptr->control.v_duty_value) &&
        ((motor_ptr->control.w_duty_value - motor_ptr->control.u_duty_value) <  VMS_VALUE_EXCLUDE_BAND_LIMIT_SIGNLE_SHUNT)))

    {
        exclude_flag = TRUE;
    }
    else
    {
        if (!motor_ptr->app.motor_enable_cmd)
            exclude_flag = FALSE;
    }
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif  // #if (VMS_DUTY_CYCLE_EXCLUSION == TRUE)
#endif  // #if (INTERNAL_VMS == TRUE)

static inline void vbus_calc_vfactors(Struct_Motor* motor_ptr)
{
    motor_ptr->app.descale_voltage_factor = fix16_mul(motor_ptr->app.dcbus_voltage,  QCONST(0.5773469F, 16));     // Q0 * (Q14 << 2) = Q16
    motor_ptr->app.dcbus_voltage_1div_q16 = fix16_div(FIX16_ONE, motor_ptr->app.dcbus_voltage);
    motor_ptr->app.descale_voltage_factor_1div_q16 = fix16_mul(Q16_SQRT_3, (motor_ptr->app.dcbus_voltage_1div_q16));

#if (MODULE_MPOS_TRAP == ENABLED)
    int32_t temp_q16;
    temp_q16 = fix16_div(QCONST((R1_PLUS_R2_BY_R2_RATIO_SIM * 2.5f), Q16), QCONST(((1 << ADC_BITS_FIRMWARE_SELECTION) - 1.0f), Q16));
    motor_ptr->mpos.trap.bemf_conv_adc2volts_scaled_q16 = fix16_mul(temp_q16, motor_ptr->app.dcbus_voltage_1div_q16);
#endif

#if defined(ENABLE_SIM)
    motor_ptr->sim.multp_conv_adc2vscaled_q16 = fix16_div(motor_ptr->sim.multp_conv_adc2volts_q16, motor_ptr->app.dcbus_voltage);
#endif

    // Prevent 0 value readings
    if (motor_ptr->app.dcbus_voltage < QCONST(1, Q16))
        motor_ptr->app.dcbus_voltage = QCONST(1, Q16);

    // Calculate voltage scale
    // V_scale = Vbus * 1/2 * Mod_Gain
    // JAGTAG: Need to point to real vbus voltage
    motor_ptr->app.v_scale_q16 = (fix16_mul(QCONST(MODULATION_INDEX, Q16), motor_ptr->app.dcbus_voltage)) >> 1;
}


#ifdef DISABLE_AUTOVDC
static inline void vbus_fixed_init(Struct_Motor* motor_ptr)
{
    // Define bus voltage
    motor_ptr->app.dcbus_voltage = VBUS_FIXED_Q16;

    // Apply limits to bus voltage and report fault
    if (motor_ptr->app.dcbus_voltage < MIN_VBUS)
    {
     motor_ptr->app.dcbus_voltage = MIN_VBUS;
     set_fault(FAULT_OU_VOLTAGE);
    }
    if (motor_ptr->app.dcbus_voltage > MAX_VBUS)
    {
     motor_ptr->app.dcbus_voltage = MAX_VBUS;
     set_fault(FAULT_OU_VOLTAGE);
    }

    // Calculate voltage factors
    vbus_calc_vfactors(motor_ptr);

#if (MODULE_MPOS_LBG1 == ENABLED)
    lbg1_process_voltage_scale(motor_ptr);
#endif
#ifdef ENABLE_DEBUG_DISPLAY
    motor_ptr->debug.dcbus_voltage_q14_9q6 = motor_ptr->app.dcbus_voltage >> 10;
#endif
}
#endif

#endif  //#ifndef VBUS_FUNCS_H
