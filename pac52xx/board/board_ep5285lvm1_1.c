
/** @file board_ep5285vm1_1.c
 *  @brief EP5285LVM1-1 Board Configuration Functions
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
 
#include "motor_struct.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_adc.h"

#if (BOARD_SELECT == BOARD_EP5285LVM1_1)
void init_diff_amp_gain(void)
{
    pac5xxx_tile_register_write(ADDR_CFGAIO10, 0);
    // Enable Vref/2 offset, 1.5 usec HPCOMP blanking time 
    pac5xxx_tile_register_write(ADDR_CFGAIO10,  ((AIO50_OFFSET_ENABLED << 3) | 0x03));
    // Configure AIO6
    pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_IO << 6) | (AIO6789_OPT_INPUT << 4) | (AIO6789_POL_AH << 3) | AIO6789_MUX_DEFAULT));    // IO Mode
}

void config_seq_foc(void)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R2;

   // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_IND_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode

    // Initialize sequencer triggers
    PAC5XXX_TIMERA->CTR0 = motor.control.motor_period_ticks - 4;                       // Set value for AS0 trigger
    PAC5XXX_TIMERA->CTR1 = motor.control.motor_period_ticks - 4;                       // Set value for AS1 trigger

    // Configure Interrupts

    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH1, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);     // Convert Vout, then change CAFE MUX to M1-Vu

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1) || !defined(ENABLE_POWERMEASURE)

    #ifndef SELECT_I2SAMPLE_MIDDLEPWM
    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH4, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA1);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);   // Convert I2, then change CAFE MUX to Vbus
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB13), ASSEQ_DELAY_0);    // Convert Vbus, then change CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB10), ASSEQ_DELAY_0);    // Convert temperature, CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(3, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);   // Convert I2, then change CAFE MUX to M1-Iu

    #else   //#ifndef SELECT_I2SAMPLE_MIDDLEPWM
    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH4, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA1);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);   // Convert I2, then change CAFE MUX to Vbus
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB13), ASSEQ_DELAY_0);    // Convert Vbus, then change CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB10), ASSEQ_DELAY_0);    // Convert temperature, CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(3, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);   // Convert I2, then change CAFE MUX to M1-Iu
   #endif  //#ifndef SELECT_I2SAMPLE_MIDDLEPWM

#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#error: POWER_MEASURE_METHOD_2 not supported BOARD_EP5285LVM1_1_1
#endif
}


#ifdef ENABLE_IPD
void config_seq_ipd(uint32_t sequencer_trigger_value)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R1;

    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);                  // Configure sequencers for AS0 trigger only, /4 divider, repeat mode
    pac5xxx_timer_a_ccctr0_value_set(sequencer_trigger_value);                              // Set value for AS0 trigger
   
    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH2, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);   // Convert I2, then change CAFE MUX to Vbus
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // Convert Vbus, then change CAFE MUX to M1-Iu

    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH2, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB13), ASSEQ_DELAY_0);    // Convert Vbus, then change CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);   // change CAFE MUX to M1-Iu

}
#endif  //#ifdef ENABLE_IPD

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_HALL))
void config_seq_sim(void)
{
    Struct_Motor* motor_ptr = &motor;

    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R1;

    // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode

    // Set sequencer trigger to start of of LS-off time
#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    motor_ptr->oneshunt.taccr0_value = (motor_ptr->control.motor_period_ticks - 4);
#endif
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger

    // JAGTAG: Consider adding level of abstraction where ADCMUX_AS0_N0_PHASE and MSPI_AS0_N1_PHASE are defined in a sequencer config file
    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB8), ASSEQ_DELAY_0);                            // Convert M1-Vu, then change CAFE MUX to M1-Vv
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB9), ASSEQ_DELAY_0);                            // Convert M1-Vv, then change CAFE MUX to M1-Vw
    pac5xxx_adc_as0_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB13), ASSEQ_DELAY_0);                           // Convert M1-Vw, then change CAFE MUX to Vout

    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB13), ASSEQ_DELAY_0);                           // Convert Vout, then change CAFE MUX to Vout
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB13), ASSEQ_DELAY_0);                           // Convert Vout, then change CAFE MUX to Vout
    pac5xxx_adc_as1_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB7), ASSEQ_DELAY_0);                            // Convert M1-Vu, then change CAFE MUX to M1-Vu
}


void config_seq_simbrake(void)
{
    Struct_Motor* motor_ptr = &motor;

    // Configure sequencers and ADC
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->sim.braking_duty_min_ticks);                              // Set value for AS0 trigger
}


#elif defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
void config_seq_sim(void)
{
    #error: Current SIM not supported
}
#endif

void init_emux(void)
{
    // Manually set ADC EMUX so that first sequencer conversion is correct
//    pac5xxx_tile_register_write(ADDR_ADCIN1, 0);        // Set to EMUX to AIO10
}


/**
 * @brief This function use to enable AFE protection
 */
void protint_enable(void)
{
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_PROTINTM, (0x01 | 0x10));      // PROTINTM: (LPPROT10 | HPROT10)
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x10);               // PROTINTM: HPROT10
#else
#ifdef ENABLE_5285_LPDAC_INT
    pac5xxx_tile_register_write(ADDR_PROTINTM, (0x01 | 0x10));      // PROTINTM: LPPROT10, HPPROT10 
#else
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x10);               // PROTINTM: HPPROT10 
#endif
#endif
}

#endif // #if (BOARD_SELECT == BOARD_EP5285LVM1_1)
