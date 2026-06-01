/**
 * @file adccal.h
 * @brief ADC channel calibration state machine
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

#ifndef ADCCAL_H
#define ADCCAL_H

#include "motor_struct.h"
#include "control_funcs.h"
#include "control_power_funcs.h"
#include "fault.h"
#include "adc_sample.h"

#define ADC_SAMPLE_IGNORE_COUNTS                    1024
#define ADC_AVERAGE_SAMPLE_SHIFT                    12
#define ADC_AVERAGE_SAMPLE_COUNTS                   1<<ADC_AVERAGE_SAMPLE_SHIFT         // 2^12 samples is equal to 4096, the we do average

#if defined(FAMILY_55XX)
#define MIN_LIMIT1    1                                           // ADC count min limit   
#define MAX_LIMIT1    100                                         // ADC count max limit
#define ADC_55XX_OFFSETMIN_LIMIT_POS   QCONST(MIN_LIMIT1, 16)     // this offset is in ADC count fix 16 format
#define ADC_55XX_OFFSETMAX_LIMIT_POS   QCONST(MAX_LIMIT1, 16)     // this offset is in ADC count fix 16 format
#define ADC_55XX_GAINMIN_LIMIT   QCONST(1310.4f, 16)
#define ADC_55XX_GAINMAX_LIMIT   QCONST(1965.6f, 16)
#endif

/**
 * @brief This function use calibrate ADC channels offset
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#if defined(FAMILY_55XX) && defined (ENABLE_ADCCAL_FACTORY)
static inline void read_devinfo_adc_offset(Struct_Motor* motor_ptr)
{
    int32_t temp;

    // Read ADC offset in counts
    motor_ptr->post.adc_offset_counts_q16 = READ_INT32(ADC_OFFSET_ADDRESS);

    // Absolute value
    temp = motor_ptr->post.adc_offset_counts_q16;
    if (temp < 0)
        temp = -temp;

    // Check limit range
    if ((temp < ADC_55XX_OFFSETMIN_LIMIT_POS) || // 1
        (temp > ADC_55XX_OFFSETMAX_LIMIT_POS))   // 100
    {
        set_fault_terminal(FAULT_TERMINAL_ADCCAL_FACTORY);
    }
}
#endif

/**
 * @brief This function use calibrate ADC channels offset
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
#if defined(FAMILY_55XX) && defined (ENABLE_ADCCAL_FACTORY)
static inline void read_devinfo_adc_gain(Struct_Motor* motor_ptr)
{
    int32_t temp_value = 0;
    int32_t adc_gain;

    // Read ADC gain in counts/V
    adc_gain = READ_INT32(ADC_GAIN_ADDRESS);

    if (((fix16_t) adc_gain < ADC_55XX_GAINMIN_LIMIT) ||
        ((fix16_t) adc_gain > ADC_55XX_GAINMAX_LIMIT))
    {
        set_fault_terminal(FAULT_TERMINAL_ADCCAL_FACTORY);
    }

    // Calculate gain as a unitless multiplier (counts in -> counts out)
    // ADC_cal = gain_multp * (ADC_value - ADCOFFSET)
    //      where   : gain_multp = (1 / K_gainerror) = (1 / ADCGAIN) * (ADCRES / VREF)
    //      or      : gain_multp = 1 / (ADCGAIN * (VREF / ADCRES))
    //
    // To preserve caluclation precision:
    //      gain_multp = 1000 / (ADCGAIN * (VREF*1000 / ADCRES))

    temp_value = fix16_mul(adc_gain, Q16_mV_PER_ADC_12_BIT_COUNT);
    motor_ptr->post.adc_gain_multp_q16 = fix16_div((1000 << 16), temp_value);
}
#endif
    
/**
 * @brief This function use calibrate ADC channels offset
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void statemachine_adccal(Struct_Motor* motor_ptr)
{
    static volatile unsigned int adc_offset_Sample_Count = 0;
    int32_t adcfault_flag = 0;

#if defined(ENABLE_ADCCAL_AIO10)
    static volatile unsigned long int adc_offset_AIO10 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO32)
    static volatile unsigned long int adc_offset_AIO32 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO54)
    static volatile unsigned long int adc_offset_AIO54 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO6)
    static volatile unsigned long int adc_offset_AIO6 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO7)
    static volatile unsigned long int adc_offset_AIO7 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO8)
    static volatile unsigned long int adc_offset_AIO8 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO9)
    static volatile unsigned long int adc_offset_AIO9 = 0;
#endif

    switch (motor_ptr->state.adccal)
    {
        case STATE_ADCCAL_RESET:
            // Initialize parameters for calculating ADC offsets
#if defined(ENABLE_ADCCAL_AIO10)
            adc_offset_AIO10 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO32)
            adc_offset_AIO32 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO54)
            adc_offset_AIO54 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO6)
            adc_offset_AIO6 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO7)
            adc_offset_AIO7 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO8)
            adc_offset_AIO8 = 0;
#endif
#if defined(ENABLE_ADCCAL_AIO9)
            adc_offset_AIO9 = 0;
#endif

            adc_offset_Sample_Count = 0;

            motor_ptr->state.adccal = STATE_ADCCAL_SAMPLEDELAY;
            break;

        // Motor Ignore ADC offset Sample State
        case STATE_ADCCAL_SAMPLEDELAY:
            // Wait for specified number of counts to ignore the first set of ADC samples
            if (adc_offset_Sample_Count++ >= ADC_SAMPLE_IGNORE_COUNTS)
            {
                // Once the count is complete, transition to the state to to start sampling ADC
                adc_offset_Sample_Count = 0;
                motor_ptr->state.adccal = STATE_ADCCAL_SAMPLEPROCESS;
            }
            break;

        case STATE_ADCCAL_SAMPLEPROCESS:
            // Sum the ADC samples over the specified number of counts
            if (adc_offset_Sample_Count++ < ADC_AVERAGE_SAMPLE_COUNTS )
            {
#if (CONFIG_OFFSET & CHANNEL_AIO10)
                adc_offset_AIO10 = adc_offset_AIO10 + readseq_aio10();
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO32)
                adc_offset_AIO32 = adc_offset_AIO32 + readseq_aio32();
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO54)
                adc_offset_AIO54 = adc_offset_AIO54 + readseq_aio54();
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO6)
//                adc_offset_AIO6 = adc_offset_AIO6 + PAC5XXX_ADC->SEQRD_CAL_AIO6.VAL;
                adc_offset_AIO6 = adc_offset_AIO6 + readseq_ibusavg();
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO7)
                adc_offset_AIO7 = adc_offset_AIO7 + PAC5XXX_ADC->SEQRD_CAL_AIO7.VAL;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO8)
                adc_offset_AIO8 = adc_offset_AIO8 + PAC5XXX_ADC->SEQRD_CAL_AIO8.VAL;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO9)
                adc_offset_AIO9 = adc_offset_AIO9 + PAC5XXX_ADC->SEQRD_CAL_AIO9.VAL;
#endif
            }
            else  // Once the samples are complete, calculate the offsets
            {
#if (CONFIG_OFFSET & CHANNEL_AIO10)
#if defined(ENABLE_ADCCAL_AIO10)
                adc_offset_AIO10 = adc_offset_AIO10 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio10 = adc_offset_AIO10;
#else
                motor_ptr->post.adc_offset_aio10 = ADC_OFFSET_DEFAULT_DIFFAMP << 4;
#endif
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO32)
#if defined(ENABLE_ADCCAL_AIO32)
                adc_offset_AIO32 = adc_offset_AIO32 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio32 = adc_offset_AIO32;
#else
                motor_ptr->post.adc_offset_aio32 = ADC_OFFSET_DEFAULT_DIFFAMP << 4;
#endif
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO54)
#if defined(ENABLE_ADCCAL_AIO54)
                adc_offset_AIO54 = adc_offset_AIO54 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio54 = adc_offset_AIO54;
#else
                motor_ptr->post.adc_offset_aio54 = ADC_OFFSET_DEFAULT_DIFFAMP << 4;
#endif
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO6)
#if defined(ENABLE_ADCCAL_AIO6)
                adc_offset_AIO6 = adc_offset_AIO6 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio6 = adc_offset_AIO6;
#else
                motor_ptr->post.adc_offset_aio6 = ADC_OFFSET_DEFAULT_AIO6 << 4;
#endif
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO7)
                adc_offset_AIO7 = adc_offset_AIO7 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio7 = adc_offset_AIO7;
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO8)
#if defined(ENABLE_ADCCAL_AIO8)
                adc_offset_AIO8 = adc_offset_AIO8 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio8 = adc_offset_AIO8;
#else
                motor_ptr->post.adc_offset_aio8 = ADC_OFFSET_DEFAULT_AIO8 << 4;
#endif
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO9)
#if defined(ENABLE_ADCCAL_AIO9)
                adc_offset_AIO9 = adc_offset_AIO9 >> (ADC_AVERAGE_SAMPLE_SHIFT - (14 - ADC_BITS_FIRMWARE_SELECTION));
                motor_ptr->post.adc_offset_aio9 = adc_offset_AIO9;
#else
                motor_ptr->post.adc_offset_aio9 = ADC_OFFSET_DEFAULT_AIO9 << 4;
#endif
#endif

// Check ADC offset limits
#if defined(ENABLE_ADCCAL_AIO10)
                if ((motor_ptr->post.adc_offset_aio10 < (ADC_OFFSET_MIN_DIFFAMP_VALUE << 4)) || (motor_ptr->post.adc_offset_aio10 > (ADC_OFFSET_MAX_DIFFAMP_VALUE << 4)))
                    adcfault_flag = 1;
#endif
#if defined(ENABLE_ADCCAL_AIO32)
                if ((motor_ptr->post.adc_offset_aio32 < (ADC_OFFSET_MIN_DIFFAMP_VALUE << 4)) || (motor_ptr->post.adc_offset_aio32 > (ADC_OFFSET_MAX_DIFFAMP_VALUE << 4)))
                    adcfault_flag = 1;
#endif
#if defined(ENABLE_ADCCAL_AIO54)
                if ((motor_ptr->post.adc_offset_aio54 < (ADC_OFFSET_MIN_DIFFAMP_VALUE << 4)) || (motor_ptr->post.adc_offset_aio54 > (ADC_OFFSET_MAX_DIFFAMP_VALUE << 4)))
                    adcfault_flag = 1;
#endif
#if defined(ENABLE_ADCCAL_AIO6)
                if ((motor_ptr->post.adc_offset_aio6 < ((ADC_OFFSET_MIN_AIO6 * AMPGAIN_AIO6) << 4)) || (motor_ptr->post.adc_offset_aio6 > ((ADC_OFFSET_MAX_AIO6 * AMPGAIN_AIO6) << 4)))
                    adcfault_flag = 1;
#endif
#if defined(ENABLE_ADCCAL_AIO7)
                if ((motor_ptr->post.adc_offset_aio7 < ((ADC_OFFSET_MIN_AIO7 * AMPGAIN_AIO7) << 4)) || (motor_ptr->post.adc_offset_aio7 > ((ADC_OFFSET_MAX_AIO7 * AMPGAIN_AIO7) << 4)))
                    adcfault_flag = 1;
#endif
#if defined(ENABLE_ADCCAL_AIO8)
                if ((motor_ptr->post.adc_offset_aio8 < ((ADC_OFFSET_MIN_AIO8 * AMPGAIN_AIO8) << 4)) || (motor_ptr->post.adc_offset_aio8 > ((ADC_OFFSET_MAX_AIO8 * AMPGAIN_AIO8) << 4)))
                    adcfault_flag = 1;
#endif
#if defined(ENABLE_ADCCAL_AIO9)
                if ((motor_ptr->post.adc_offset_aio9 < ((ADC_OFFSET_MIN_AIO9 * AMPGAIN_AIO9) << 4)) || (motor_ptr->post.adc_offset_aio9 > ((ADC_OFFSET_MAX_AIO9 * AMPGAIN_AIO9) << 4)))
                    adcfault_flag = 1;
#endif

                if (adcfault_flag)
                    set_fault_terminal(FAULT_TERMINAL_ADCCAL_POWERUP);

                motor_ptr->state.adccal = STATE_ADCCAL_COMPLETE;
            }
            break;

        case STATE_ADCCAL_COMPLETE:
            break;

        default:
            motor_ptr->state.adccal = STATE_ADCCAL_RESET;
            break;
    }
}

#endif  //#ifndef ADCCAL_H
