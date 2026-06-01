/**
 * @file adc_sample.h
 * @brief ADC channel sample, gain and offset
 */

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef ADC_SAMPLE_H
#define ADC_SAMPLE_H

#include "board.h"


#if defined(FAMILY_55XX) &&  !defined(ENABLE_ADCCAL_FACTORY)

#define EXTRA_OFFSET_MIN  -35
#define EXTRA_OFFSET_MAX  35
#else
#define EXTRA_OFFSET_MIN  0
#define EXTRA_OFFSET_MAX  0
#endif

#define ADC_OFFSET_MIN_DIFFAMP_VALUE                (ADC_OFFSET_MIN_DIFFAMP + EXTRA_OFFSET_MIN)
#define ADC_OFFSET_MAX_DIFFAMP_VALUE                (ADC_OFFSET_MAX_DIFFAMP + EXTRA_OFFSET_MAX)

/**
 * @brief  This function add adc offset and gain to adc raw value.
 *
 * @param adc raw value.
 * @return adc compensated value.
 *
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline int16_t cal_adc_gain_offset(int16_t adc_value)
{
    //////////////////////////////////
    // ADC Factory Calibration
    //////////////////////////////////
    //
    // Ideal ADC:
    // ADC_value = V_in * (ADCRES / VREF)
    //
    // Actual ADC with gain error and offset:
    // ADC_value = V_in * (ADCRES / VREF) * K_gainerror + ADCOFFSET
    // ADC_value = V_in * ADCGAIN + ADCOFFSET
    //      where   ADCGAIN (counts/V) = ((ADCRES / VREF) * K_gainerror)
    //              ADCOFFSET (counts)
    //              NOte: These are the values stored in InfoROM
    //
    // Rearranging:
    // V_in = (1 / ADCGAIN) * (ADC_value - ADCOFFSET)
    //
    // To get calibrated counts from raw counts need to convert V_in to calibrated counts by multiplying both sides by (ADCRES / VREF)
    // ADC_cal = V_in * (ADCRES / VREF) = (1 / ADCGAIN) * (ADC_value - ADCOFFSET) * (ADCRES / VREF)
    //
    // Substituting for ADCGAIN = ((ADCRES / VREF) * K_gainerror) and simplifying:
    // ADC_cal = (1 / K_gainerror) * (ADC_value - ADCOFFSET)
    //
    // Therefore:
    // ADC_cal = gain_multp * (ADC_value - ADCOFFSET)
    //      where: gain_multp = (1 / K_gainerror) = (1 / ADCGAIN) * (ADCRES / VREF)
    //      Note: This is the formula applied in the firmware to raw ADC value

#if defined(FAMILY_55XX) && defined(ENABLE_ADCCAL_FACTORY)
    int32_t value;
    Struct_Motor* motor_ptr = &motor;
    
    value = ((adc_value << 16) - motor_ptr->post.adc_offset_counts_q16) >> 13;                       // 12 bit, (q16 - q16) >> 13 = q3
    return  ((value * motor_ptr->post.adc_gain_multp_q16) >> (31 - ADC_BITS_FIRMWARE_SELECTION));    // q3 * q16 = q19 >> 19 = 12 bit
#else
    return (adc_value >> (ADC_BITS_HARDWARE_CAPABILITY - ADC_BITS_FIRMWARE_SELECTION));              // 10 or 12 bit for 55xx, 10 bit 52xx 
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
#if (CONFIG_OFFSET & CHANNEL_AIO10)
static inline int16_t readseq_aio10(void)
{
    return cal_adc_gain_offset(readseq_aio10_raw());
}   
#endif

/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
#if (CONFIG_OFFSET & CHANNEL_AIO32)
static inline int16_t readseq_aio32(void)
{
    return cal_adc_gain_offset(readseq_aio32_raw());
}
#endif

/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
#if (CONFIG_OFFSET & CHANNEL_AIO54)
static inline int16_t readseq_aio54(void)
{
    return cal_adc_gain_offset(readseq_aio54_raw());
}   
#endif


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_iphase_u(void)
{
    return cal_adc_gain_offset(readseq_iphase_u_raw());
}


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_iphase_v(void)
{
    return cal_adc_gain_offset(readseq_iphase_v_raw());
}


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_iphase_w(void)
{
    return cal_adc_gain_offset(readseq_iphase_w_raw());
}

#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_vphase_u_trap(void)
{
    return cal_adc_gain_offset(readseq_vphase_u_raw_trap());
}

static inline int16_t readseq_vphase_v_trap(void)
{
    return cal_adc_gain_offset(readseq_vphase_v_raw_trap());
}

static inline int16_t readseq_vphase_w_trap(void)
{
    return cal_adc_gain_offset(readseq_vphase_w_raw_trap());
}

static inline int16_t readseq_vphase_x_trap(void)
{
    return cal_adc_gain_offset(readseq_vphase_x_raw_trap());
}
#endif

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE)) && (CAPABLE_VSIM == TRUE) 
/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_vphase_u(void)
{
    return cal_adc_gain_offset(readseq_vphase_u_raw());
}


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_vphase_v(void)
{
    return cal_adc_gain_offset(readseq_vphase_v_raw());
}


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_vphase_w(void)
{
    return cal_adc_gain_offset(readseq_vphase_w_raw());
}
#endif    //#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE)) && (CAPABLE_VSIM == TRUE) 


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline int16_t readseq_vbus(int32_t adc_count)
{
    return cal_adc_gain_offset(adc_count);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
#if (INTERNAL_VMS == TRUE)
static inline int16_t readseq_vbus_vms(int32_t adc_count)
{
    return cal_adc_gain_offset(adc_count);
}
#endif


#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_ssi1(void)
{
    return cal_adc_gain_offset(readseq_ssi1_raw());
}


/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
static inline int16_t readseq_ssi2(void)
{
    return  cal_adc_gain_offset(readseq_ssi2_raw());
}
#endif

/**
 * @brief  This function add adc offset and gain to adc raw value, return updated value.
 *
 * @return adc compensated value.
 *
 */
#if (CONFIG_OFFSET & CHANNEL_AIO6)
static inline int16_t readseq_ibusavg(void)
{
    return  cal_adc_gain_offset(readseq_ibusavg_raw());
}
#endif

#endif  //#ifndef ADC_SAMPLE_H
