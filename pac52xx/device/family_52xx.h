
/**
 * @file family_52xx.h
 * @brief Use for 52xx family hardware resources initialization
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
 
#ifndef FAMILY_52XX_H
#define FAMILY_52XX_H

#define FAMILY_52XX 

#include "pac5xxx.h"
#include "device_options.h"
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_timer.h"

// INFOROM VMS100 and VMS200 constants
#define VMS100_ADDRESS        0x0010003C        // VMS100 Address
#define VMS200_ADDRESS        0x0010003E        // VMS200 Address
#define ADC_OFFSET_ADDRESS    0x00100024        // ADC offset address
#define ADC_GAIN_ADDRESS      0x00100022        // ADC gain address

// INFOROM DEVICE
#define DEVICE_INFO_ADDRESS2   0x00100044       // 52XX device info
#define DEVICE_INFO_ADDRESS_SHUNT_NUM   0x00100074       // 527XX device info: 0 for 52710, 1 or 52711 at the fourth byte (Address: 0x00100077)

// ADC BITS CONFIGURATION
#define ADC_BITS_HARDWARE_CAPABILITY                      10
#define ADC_BITS_FIRMWARE_SELECTION                       10

// MCU Port Assignment: AFE
#define ADC_CHANNEL_MASK                    (1L << ADCCTL_ADMUX_AD0)                                    // Mask of ADC channels to perform conversions for

// nIRQ1 inputs from CAFE: PB0 (fault interrupt)
#define NIRQ1_PIN                           0                                                           // nIRQ1 pin bit number
#define NIRQ1_PIN_MASK                      (1 << NIRQ1_PIN)                                            // nIRQ1 pin bit mask

// INFOROMATION USE FOR PRE-COMPILE OPTIONS
#define TIMER_A    0
#define TIMER_B    1
#define TIMER_C    2
#define TIMER_D    3


/**
 * @brief clear_oc_interrupt, use to clear port over current interrupt flag
 */
static inline void clear_oc_interrupt() { pac5xxx_gpio_int_flag_clear_b(NIRQ1_PIN_MASK); }


static inline int16_t readseq_raw(int32_t slot)
{
    int32_t value;

    switch(slot)
    {
    case 0:
        value = PAC5XXX_ADC->AS0R0.VAL;
        break;
    case 1:
        value = PAC5XXX_ADC->AS0R1.VAL;
        break;
    case 2:
        value = PAC5XXX_ADC->AS0R2.VAL;
        break;
    case 3:
        value = PAC5XXX_ADC->AS0R3.VAL;
        break;
    case 4:
        value = PAC5XXX_ADC->AS0R4.VAL;
        break;
    case 5:
        value = PAC5XXX_ADC->AS0R5.VAL;
        break;
    case 6:
        value = PAC5XXX_ADC->AS0R6.VAL;
        break;
    case 7:
        value = PAC5XXX_ADC->AS0R7.VAL;
        break;
    case 10:
        value = PAC5XXX_ADC->AS1R0.VAL;
        break;
    case 11:
        value = PAC5XXX_ADC->AS1R1.VAL;
        break;
    case 12:
        value = PAC5XXX_ADC->AS1R2.VAL;
        break;
    case 13:
        value = PAC5XXX_ADC->AS1R3.VAL;
        break;
    case 14:
        value = PAC5XXX_ADC->AS1R4.VAL;
        break;
    case 15:
        value = PAC5XXX_ADC->AS1R5.VAL;
        break;
    case 16:
        value = PAC5XXX_ADC->AS1R6.VAL;
        break;
    case 17:
        value = PAC5XXX_ADC->AS1R7.VAL;
        break;
    default:
        value = 0;
        break;
    }

    return value;
}


void init_system(void);
void configure_cafe_fault_interrupt(void);
void configure_adc_emux_engine(void);
void configure_pwm_dac(void);
void uart_scratch_write(int32_t value);
int32_t uart_scratch_read(void);
void initialize_foc_timer_seq_trigger_value(int32_t ccr_trig_value);
void initialize_foc_timer_seq_trigger_value_ccr0_ccr1(int32_t ccr_trig_value);
void initialize_socbridge_clk_divider(void);
void pac5xxx_start_adc_sequencer(void);
extern void deconfig_seq(void);

#endif  //#ifndef FAMILY_52XX_H
