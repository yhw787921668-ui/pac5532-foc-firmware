
//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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
 
#ifndef FAMILY_55XX_H
#define FAMILY_55XX_H

#define FAMILY_55XX 

#include "pac5xxx.h"
#include "config_debug.h"
#include "pac5xxx_driver_timer.h"

// INFOROM VMS100 and VMS200 constants
#define VMS100_ADDRESS        0x00100410        // VMS100 Address
#define VMS200_ADDRESS        0x00100412        // VMS200 Address
#define ADC_OFFSET_ADDRESS    0x00100414        // ADC offset address
#define ADC_GAIN_ADDRESS      0x00100418        // ADC gain address

// INFOROM DEVICE
#define DEVICE_INFO_ADDRESS1   0x00100404        // 55XX device info1
#define DEVICE_INFO_ADDRESS2   0x00100408        // 55XX device info2

// ADC BITS CONFIGURATION
#define ADC_BITS_HARDWARE_CAPABILITY                      12
//#define ADC_BITS_FIRMWARE_SELECTION                       10
#define ADC_BITS_FIRMWARE_SELECTION                       12

// QEP Port Definition
#define QEP_PORT_C             1
#define QEP_PORT_D             2

// MCU Port Assignment: AFE
#define ADC_CHANNEL_MASK                    (1L << ADCCTL_CHANNEL_ADC0)                                    // Mask of ADC channels to perform conversions for

// INFOROMATION USE FOR PRE-COMPILE OPTIONS
#define TIMER_A    0
#define TIMER_B    1
#define TIMER_C    2
#define TIMER_D    3

#define PAC5XXX_TIMERA  PAC55XX_TIMERA
#define PAC5XXX_TIMERB  PAC55XX_TIMERB

static inline void clear_dtse_emux_complete_int_int_flag() { PAC55XX_ADC->ADCINT.INTFEMUX = 1; }
static inline void clear_oc_interrupt() { PAC55XX_GPIOA->INTCLEAR.P7 = 1; }             // Clear any pending PA7 interrupt 
#if ((DEVICE_SELECT == DEVICE_5556) || (DEVICE_SELECT == DEVICE_5556A))
static inline void clear_timer_b_interrupt_flag() { PAC55XX_TIMERB->INT.CCR0IF = 1; }
#endif

// ADC Sequencer Readout Macro Functions
PAC5XXX_RAMFUNC static inline int16_t Vout(void)            { return PAC55XX_ADC->DTSERES0.VAL >> 2;}
PAC5XXX_RAMFUNC static inline int16_t m1_Iu(void)           { return PAC55XX_ADC->DTSERES2.VAL >> 2;}
PAC5XXX_RAMFUNC static inline int16_t m1_Iv(void)           { return PAC55XX_ADC->DTSERES4.VAL >> 2;}
PAC5XXX_RAMFUNC static inline int16_t m1_Iw(void)           { return PAC55XX_ADC->DTSERES6.VAL >> 2;}
static inline int16_t read_phase_u_bemf(void)               { return PAC55XX_ADC->DTSERES8.VAL >> 2;}
static inline int16_t read_phase_v_bemf(void)               { return PAC55XX_ADC->DTSERES10.VAL >> 2;}
static inline int16_t read_phase_w_bemf(void)               { return PAC55XX_ADC->DTSERES12.VAL >> 2;}


extern void init_system(void);
extern void configure_cafe_fault_interrupt(void);
void configure_adc_emux_engine(void);
void configure_pwm_dac(void);


#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
PAC5XXX_RAMFUNC static inline void pac5xxx_timer_set_ccctr_value(int32_t ccr0_value, int32_t ccr1_value)
{
#if ((DEVICE_SELECT != DEVICE_5556) && (DEVICE_SELECT != DEVICE_5556A))
        pac5xxx_timer_a_ccctr0_value_set(ccr0_value);
        pac5xxx_timer_a_ccctr1_value_set(ccr1_value);
#else
        pac5xxx_timer_b_ccctr0_value_set(ccr0_value);
        pac5xxx_timer_b_ccctr1_value_set(ccr1_value);
#endif
}
#endif


static inline int16_t read_calibrated_adc_counts(int32_t adc_count, int32_t adc_offset, int32_t adc_gain) 
{
    int32_t rv;
        // q0 << 7 = q7, q16 >> 9 = q7, q16 >> 9 = q7, (q7 -q7)*q7 = q14 
        rv = ((adc_count << 7) - (adc_offset >> 9)) * (adc_gain >> 9);  
    return (int16_t) rv;
}


static inline int16_t readseq_raw(int32_t slot)
{
    int32_t value;

    switch(slot)
    {
    case 0:
        value = PAC55XX_ADC->DTSERES0.VAL;
        break;
    case 1:
        value = PAC55XX_ADC->DTSERES1.VAL;
        break;
    case 2:
        value = PAC55XX_ADC->DTSERES2.VAL;
        break;
    case 3:
        value = PAC55XX_ADC->DTSERES3.VAL;
        break;
    case 4:
        value = PAC55XX_ADC->DTSERES4.VAL;
        break;
    case 5:
        value = PAC55XX_ADC->DTSERES5.VAL;
        break;
    case 6:
        value = PAC55XX_ADC->DTSERES6.VAL;
        break;
    case 7:
        value = PAC55XX_ADC->DTSERES7.VAL;
        break;
    case 8:
        value = PAC55XX_ADC->DTSERES8.VAL;
        break;
    case 9:
        value = PAC55XX_ADC->DTSERES9.VAL;
        break;
    case 10:
        value = PAC55XX_ADC->DTSERES10.VAL;
        break;
    
    default:
        value = 0;
        break;
    }

    return value;
}


void uart_scratch_write(int32_t value);
int32_t uart_scratch_read(void);
//extern void sim_update_seq_trigger_point(int32_t value);
extern void pac5xxx_start_adc_sequencer(void);
extern void deconfig_seq(void);

#endif  //#ifndef FAMILY_55XX_H
