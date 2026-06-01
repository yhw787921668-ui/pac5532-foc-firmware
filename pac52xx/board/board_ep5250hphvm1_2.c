
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
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_adc.h"

#if (BOARD_SELECT == BOARD_EP5250HPHVM1_2)
void init_diff_amp_gain(void)
{
    Struct_Motor* motor_ptr = &motor;
    
    pac5xxx_tile_register_write(ADDR_CFGAIO0, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_HPROT_PR1 | AIO50_LPROT_PR1 | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#else
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_LPROT_PR1 | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO2, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT    
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_HPROT_PR1 | AIO50_LPROT_PR1 | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#else    
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_LPROT_PR1 | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO4, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT     
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_HPROT_PR1 | AIO50_LPROT_PR1 | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#else    
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_LPROT_PR1 | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    // Configure AIO6
    pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_IO << 6) | (AIO6789_OPT_INPUT << 4) | (AIO6789_POL_AH << 3) | AIO6789_MUX_DEFAULT));    // IO Mode
    //pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT));                          // Gain Mode

    pac5xxx_tile_register_write(ADDR_CFGAIO7, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO8, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO9, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
}

void config_seq_foc(void)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R0;

    // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode

    pac5xxx_timer_a_ccctr0_value_set(motor.control.motor_period_ticks - 6);            // Set value for AS0 trigger

    // Configure ADC Interrupts

    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);   // Convert M1-Iu, then change CAFE EMUX to M1-Iv
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO54) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);   // Convert M1-Iv, then change CAFE MUX to M1-Iw
    pac5xxx_adc_as0_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB4), ASSEQ_DELAY_0);                              // Convert M1-Iw, then change CAFE MUX to M1-Vu

    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH1, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // Convert Vout, then change CAFE MUX to M1-Iu
}

#ifdef ENABLE_IPD
void config_seq_ipd(uint32_t sequencer_trigger_value)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R0;

    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);                  // Configure sequencers for AS0 trigger only, /4 divider, repeat mode
    pac5xxx_timer_a_ccctr0_value_set(sequencer_trigger_value);                              // Set value for AS0 trigger

    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);   // Convert M1-Iu, then change CAFE EMUX to M1-Iv
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO54) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);   // Convert M1-Iv, then change CAFE MUX to M1-Iw
    pac5xxx_adc_as0_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // Convert M1-Iw, then change CAFE MUX to M1-Iu

    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH2, ADCCTLX_TRIGEDGE_HITOLOW, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // Convert Vout, then change CAFE MUX to M1-Iu
    pac5xxx_adc_as1_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // change CAFE MUX to M1-Iu
}

#endif  //#ifdef ENABLE_IPD

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_HALL))
void config_seq_sim(void)
{
    Struct_Motor* motor_ptr = &motor;

    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R0;

    // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger

    // JAGTAG: Consider adding level of abstraction where ADCMUX_AS0_N0_PHASE and MSPI_AS0_N1_PHASE are defined in a sequencer config file
    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB8), ASSEQ_DELAY_0);                            // Convert M1-Vu, then change CAFE MUX to M1-Vv
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB9), ASSEQ_DELAY_0);                            // Convert M1-Vv, then change CAFE MUX to M1-Vw
    pac5xxx_adc_as0_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB7), ASSEQ_DELAY_0);                            // Convert M1-Vw, then change CAFE MUX to M1-Vu

    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH1, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AB7), ASSEQ_DELAY_0);                            // Convert Vout, then change CAFE MUX to M1-Vu
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
    Struct_Motor* motor_ptr = &motor;

    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (PAC5XXX_ADCSEQCTL_Type*) &PAC5XXX_ADC->AS1R0;

    // Configure sequencers and ADC
    pac5xxx_adc_config(ADCCTL_ADSTART_AUTO_01_CHAIN_TRIG, ADCCTL_ADCDIV_DIV4, 1);           // Configure sequencers for AS0 chained triggers, /4 divider, repeat mode
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);                              // Set value for AS0 trigger

    // JAGTAG: Consider adding level of abstraction where ADCMUX_AS0_N0_PHASE and MSPI_AS0_N1_PHASE are defined in a sequencer config file
    // AS0 configuration
    pac5xxx_adc_as0_config_pwm(ADCCTLX_AS1D_DEPTH3, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as0_sequence_config(0, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO32) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);	// Convert M1-Iu, then change CAFE MUX to M1-Iv
    pac5xxx_adc_as0_sequence_config(1, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO54) | SIGMGR_MSPI_SH_DIFFAMP, ASSEQ_DELAY_0);  	// Convert M1-Iv, then change CAFE MUX to M1-Iw
    pac5xxx_adc_as0_sequence_config(2, ADCCTL_ADMUX_AD0, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // Convert M1-Iw, then change CAFE MUX to M1-Iu

    // AS1 configuration
    pac5xxx_adc_as1_config_pwm(ADCCTLX_AS1D_DEPTH1, ADCCTLX_TRIGEDGE_LOWTOHI, ADCCTLX_TRIGPWM_PWMA0);
    pac5xxx_adc_as1_sequence_config(0, ADCCTL_ADMUX_AD2, ASSEQ_MSPI_AFTER_SH, SIGMGR_MSPI(SIGMGR_AIO10), ASSEQ_DELAY_0);                            // Convert Vout, then change CAFE MUX to M1-Iu
}
#endif

void init_emux(void)
{
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

#endif // #if (BOARD_SELECT == BOARD_EP5250HPHVM1_2)
