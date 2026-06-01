
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
 
#include "motor_struct.h"
#include "pac5xxx_tile_SignalManager.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_adc.h"

#if (BOARD_SELECT == BOARD_PAC5256EVK1_REV_AB_1SHUNT) || (BOARD_SELECT == BOARD_PAC5256EVK1_REV_C_1SHUNT)
void init_diff_amp_gain(void)
{
    Struct_Motor* motor_ptr = &motor;

    pac5xxx_tile_register_write(ADDR_CFGAIO0, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_HPROT_PR | AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#else
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO2, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT    
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_HPROT_PR | AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#else    
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO4, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT     
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_HPROT_PR | AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#else    
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_IO << 6) | (AIO6789_OPT_INPUT << 4) | (AIO6789_POL_AH << 3) | AIO6789_MUX_DEFAULT));    // IO Mode
    //pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT));                          // Gain Mode

    pac5xxx_tile_register_write(ADDR_CFGAIO7, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO8, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO9, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
}

void config_seq_foc(void)
{
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1) || !defined(ENABLE_POWERMEASURE)
    unsigned char value;
#endif

#if (INTERNAL_VMS == TRUE)
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R1;
#else
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R2;
#endif

    // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_IND_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode

    // Initialize sequencer triggers
    PAC5XXX_TIMERA->CTR0 = motor.control.motor_period_ticks - 4;                       // Set value for AS0 trigger
    PAC5XXX_TIMERA->CTR1 = motor.control.motor_period_ticks - 4;                       // Set value for AS1 trigger

    // Configure Interrupts

    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH1, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32), ASSEQ_DELAY_0);   // Convert M1-Iu, then change CAFE EMUX to M1-Iv

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1) || !defined(ENABLE_POWERMEASURE)
    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA1);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB11, ASSEQ_DELAY_0);                 // Convert I2, then change CAFE MUX to Vbus internal
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32), ASSEQ_DELAY_0);   // Convert Vbus internal, then change CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(2, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32), ASSEQ_DELAY_0);   // Convert Vbus external, then change CAFE MUX to M1-Iu

    // Configure Power Mux so AB11 = Vout(Vbus)
    value = pac5xxx_tile_register_read(ADDR_PWRCTL);
    value = (value & 0xC7) | (0x05 << 3);
    pac5xxx_tile_register_write(ADDR_PWRCTL, value);     // Select PWRMON Mux = Vbus

#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#error: POWER_MEASURE_METHOD_2 not supported BOARD_PAC5256EVK1_1SHUNT
#endif
}


#ifdef ENABLE_IPD
void config_seq_ipd(uint32_t sequencer_trigger_value)
{
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1) || !defined(ENABLE_POWERMEASURE)
    unsigned char value;
#endif

#if (INTERNAL_VMS == TRUE)
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R1;
#else
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R2;
#endif

    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);                  // Configure sequencers for AS0 trigger only, /4 divider, repeat mode
    pac5xxx_timer_a_ccctr0_value_set(sequencer_trigger_value);                              // Set value for AS0 trigger
    
    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH2, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);   // Convert MI-Iu, 
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32), ASSEQ_DELAY_0);                            // Convert M1-Iu

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1) || !defined(ENABLE_POWERMEASURE)
    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB11, ASSEQ_DELAY_0);    // Convert I2, then change CAFE MUX to Vbus internal
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB11, ASSEQ_DELAY_0);    // then change CAFE MUX to Vbus external
    pac5xxx_adc_as1_sequence_config(2, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32), ASSEQ_DELAY_0);   // Convert Vbus external, then change CAFE MUX to M1-Iu

    // Configure Power Mux so AB11 = Vout(Vbus)
    value = pac5xxx_tile_register_read(ADDR_PWRCTL);
    value = (value & 0xC7) | (0x05 << 3);
    pac5xxx_tile_register_write(ADDR_PWRCTL, value);     // Select PWRMON Mux = Vbus

#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#error: POWER_MEASURE_METHOD_2 not supported BOARD_PAC5256EVK1_1SHUNT
#endif
}
#endif  //#ifdef ENABLE_IPD

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_HALL))
void config_seq_sim(void)
{
    Struct_Motor* motor_ptr = &motor;

#if (INTERNAL_VMS == TRUE)
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R1;
#else
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R2;
#endif

    // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode

    // Set sequencer trigger to start of of LS-off time
#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    motor_ptr->oneshunt.taccr0_value = (motor_ptr->control.motor_period_ticks - 4);
#endif
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger
    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB8, ASSEQ_DELAY_0);                                   // Convert M1-Vu, then change CAFE MUX to M1-Vv
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB9, ASSEQ_DELAY_0);                                   // Convert M1-Vv, then change CAFE MUX to M1-Vw
    pac5xxx_adc_as0_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB11, ASSEQ_DELAY_0);                                  // Convert M1-Vw, then change CAFE MUX to Vout

    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB11, ASSEQ_DELAY_0);                                  // Convert Vout AB11, then change CAFE MUX to M1-Vu
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB11, ASSEQ_DELAY_0);                                  // Convert Vout, then change CAFE MUX to M1-Vu
    pac5xxx_adc_as1_sequence_config(2, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_AB7, ASSEQ_DELAY_0);                                   // Convert Vout, then change CAFE MUX to M1-Vu
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
    PAC5XXX_ADC->EMUXCTL.EMUXC = 0;                     // Set EMUX to manual mode
    PAC5XXX_ADC->EMUXDATA.VAL = SIGMGR_AIO10;           // Set EMUX to AIO10
    PAC5XXX_ADC->EMUXCTL.EMUXC = 1;                     // Set EMUX to ADC DTSE mode
}

/**
 * @brief This function use to enable AFE protection
 */
void protint_enable(void)
{
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_PROTINTM, (0x07 | 0x70));      // PROTINTM: (LPPROT54/32/10 | HPROT54/32/10)
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x70);               // PROTINTM: HPROT54/32/10
#else
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x07);              // PROTINTM: LPPROT54/32/10
#endif
}

#endif // #if (BOARD_SELECT == BOARD_PAC5256EVK1)
