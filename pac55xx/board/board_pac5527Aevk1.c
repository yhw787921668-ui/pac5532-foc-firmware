
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
#include "pac5xxx_tile_signal_manager.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_adc.h"
#include "pac5xxx_driver_timer.h"

#if (BOARD_SELECT == BOARD_PAC5527AEVK1)
void init_diff_amp_gain(void)
{
    Struct_Motor* motor_ptr = &motor;
    
    pac5xxx_tile_register_write(ADDR_CFGAIO0, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_HPROT_PR | AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_HPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_2US));
#else
    pac5xxx_tile_register_write(ADDR_CFGAIO1, (AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO2, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT    
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_HPROT_PR | AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_HPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_2US));
#else    
    pac5xxx_tile_register_write(ADDR_CFGAIO3, (AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO4, ((((AIO50_MODE_DIFFAMP << 6) | (AIO50_GAIN_X1 << 3) | AIO50_LPCOMP_2US) & 0xC7) | (motor_ptr->app.iphase_amp_gain << 3)));
#ifdef ENABLE_CBC_CURRENT_LIMIT     
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_HPROT_PR | AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_1US));
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_HPROT_PR | (AIO50_OFFSET_ENABLED << 3) | AIO50_HPCOMP_2US));
#else    
    pac5xxx_tile_register_write(ADDR_CFGAIO5, (AIO50_LPROT_PR | (AIO50_OFFSET_ENABLED << 3)));
#endif    

    pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_IO << 6) | (AIO6789_OPT_INPUT << 4) | (AIO6789_POL_AH << 3) | AIO6789_MUX_DEFAULT));    // IO Mode
//    pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT));                          // Gain Mode

    pac5xxx_tile_register_write(ADDR_CFGAIO7, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO8, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO9, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
}

void config_seq_foc(void)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES8;

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);            // PG7 | PF6

    FOC_PWM_Timer_Ptr->CCTR0.CTR = motor.control.motor_period_ticks - 6;

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;            // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;       // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 1;         // Trigger rising edge

    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC0, EMUX_AIO10 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(1, ADC0, EMUX_AIO10 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(2, ADC0, EMUX_AIO32 | DIFFAMPS_HOLD, 0,           0);     // Convert M1-Iu, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AIO32 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AIO54 | DIFFAMPS_HOLD, 0,           0);     // Convert M1-Iv, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(5, ADC0, EMUX_AIO54 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iw
    pac5xxx_dtse_seq_config(6, ADC0, EMUX_AB13, ADC_IRQ0_EN, 0);                      // Convert Iw, then change CAFE MUX vms, generate ADC interrupt
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AB13, 0, 0);                                // Dummy Conversion, change CAFE MUX to vms
    pac5xxx_dtse_seq_config(8, ADC0, EMUX_AIO10, 0, SEQ_END);                         // Convert vms, then change CAFE MUX to M1-Iu
}

#ifdef ENABLE_IPD
void config_seq_ipd(uint32_t sequencer_trigger_value)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES8;

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);               // PG7 | PF6

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;     // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 2;       // Trigger falling edge

    pac5xxx_timer_a_ccctr0_value_set(sequencer_trigger_value);                              // Set value for AS0 trigger
    
    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC0, EMUX_AIO10 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(1, ADC0, EMUX_AIO10 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(2, ADC0, EMUX_AIO32 | DIFFAMPS_HOLD, 0,           0);     // Convert M1-Iu, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AIO32 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AIO54 | DIFFAMPS_HOLD, 0,           0);     // Convert M1-Iv, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(5, ADC0, EMUX_AIO54 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iw
    pac5xxx_dtse_seq_config(6, ADC0, EMUX_AB13, ADC_IRQ0_EN, 0);                      // Convert M1-Iw, then change CAFE MUX to M1-Iu, generate ADC interrupt
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AB13, 0, 0);                                // Dummy Conversion, change CAFE MUX to vms
    pac5xxx_dtse_seq_config(8, ADC0, EMUX_AIO10, 0, SEQ_END);                         // Convert vms, then change CAFE MUX to M1-Iu

}
#endif  //#ifdef ENABLE_IPD

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_HALL) || (SEL_SIM_METHOD == SIM_METHOD_QEP))
void config_seq_sim(void)
{
    Struct_Motor* motor_ptr = &motor;

    // Set Vbus sequencer slot
#if (INTERNAL_VMS == TRUE)
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES8;
#else
    #error
#endif

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);            // PG7 | PF6

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;     // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 1;       // Trigger rising edge
    
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger

    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC0, EMUX_AB7, 0,           0);      // Dummy Conversion, then change CAFE MUX to M1-Vu
    pac5xxx_dtse_seq_config(1, ADC0, EMUX_AB7, 0,           0);      // Dummy Conversion, then change CAFE MUX to M1-Vu
    pac5xxx_dtse_seq_config(2, ADC0, EMUX_AB8, 0,           0);      // Convert M1-Vu, then change CAFE MUX to M1-Vv
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AB8, 0,           0);      // Dummy Conversion, then change CAFE MUX to M1-Vv
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AB9, 0,           0);      // Convert M1-Vv, then change CAFE MUX to M1-Vw
    pac5xxx_dtse_seq_config(5, ADC0, EMUX_AB9, 0,           0);      // Dummy Conversion, then change CAFE MUX to M1-Vw
    pac5xxx_dtse_seq_config(6, ADC0, EMUX_AB13, ADC_IRQ0_EN, 0);     // Convert M1-Iw, then change CAFE MUX to M1-Iu, generate ADC interrupt
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AB13, 0, 0);               // Dummy Conversion, change CAFE MUX to vms
    pac5xxx_dtse_seq_config(8, ADC0, EMUX_AIO10, 0, SEQ_END);        // Convert vms, then change CAFE MUX to M1-Vu
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
#if (INTERNAL_VMS == TRUE)
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES8;
#else
    #error
#endif

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);            // PG7 | PF6

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;     // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 1;       // Trigger rising edge
    
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger

    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC0, EMUX_AIO10 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(1, ADC0, EMUX_AIO10 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(2, ADC0, EMUX_AIO32 | DIFFAMPS_HOLD, 0,           0);     // Convert M1-Iu, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AIO32 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AIO54 | DIFFAMPS_HOLD, 0,           0);     // Convert M1-Iv, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(5, ADC0, EMUX_AIO54 | DIFFAMPS_HOLD, 0,           0);     // Dummy Conversion, then change CAFE MUX to M1-Iw
    pac5xxx_dtse_seq_config(6, ADC0, EMUX_AB13, ADC_IRQ0_EN, 0);                      // Convert M1-Iw, then change CAFE MUX to M1-Iu, generate ADC interrupt
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AB13, 0, 0);                                // Dummy Conversion, change CAFE MUX to vms
    pac5xxx_dtse_seq_config(8, ADC0, EMUX_AIO10, 0, SEQ_END);                         // Convert vms, then change CAFE MUX to M1-Iu
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
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x07);               // PROTINTM: LPPROT54/32/10
#endif
}

#endif // #if (BOARD_SELECT == BOARD_PAC5527AEVK1)
