
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
#include "pac5xxx_tile_signal_manager.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_adc.h"
#include "pac5xxx_driver_timer.h"

#if (BOARD_SELECT == BOARD_PAC5532EVK1_1SHUNT)
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
//    pac5xxx_tile_register_write(ADDR_CFGAIO6, ((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT));                          // Gain Mode

    pac5xxx_tile_register_write(ADDR_CFGAIO7, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO8, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
    pac5xxx_tile_register_write(ADDR_CFGAIO9, ((((AIO6789_MODE_GAIN << 6) | (AIO6789_GAIN_X1 << 3) | AIO6789_MUX_DEFAULT) & 0xC7) | (motor_ptr->app.vphase_amp_gain << 3)));
}

void config_seq_foc(void)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES6;

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);            // PG7 | PF6
    
#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    // Initialize sequencer triggers
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTR0.CTR = motor.control.motor_period_ticks - 4;
    FOC_ASYM_SUPPLE_Timer_Ptr->CCTR1.CTR = motor.control.motor_period_ticks - 4;

    PAC55XX_ADC->DTSETRIGENT8TO11.FORCE8 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG8CFGIDX = 0;     // DTSE Trigger 8 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG8EDGE = 2;       // Trigger falling edge
#else
    // Initialize sequencer triggers
    FOC_PWM_Timer_Ptr->CCTR0.CTR = motor.control.motor_period_ticks - 4;
    FOC_PWM_Timer_Ptr->CCTR1.CTR = motor.control.motor_period_ticks - 4;

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;     // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 2;         // Trigger falling edge
#endif
    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC0, SIGMGR_AIO32, 0, 0);    // Convert M1_Iu, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(1, ADC0, SIGMGR_AIO32, 0, SEQ_END);    // Dummy Conversion, then change CAFE MUX to M1-I

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1) || !defined(ENABLE_POWERMEASURE)

    #ifndef SELECT_I2SAMPLE_MIDDLEPWM
#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    // AS1 configuration
    PAC55XX_ADC->DTSETRIGENT8TO11.FORCE9 = 0;            // Reset to 0
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG9CFGIDX = 3;       // DTSE Trigger 9 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG9EDGE = 2;         // Trigger falling edge
#else
    // AS1 configuration
    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE1 = 0;            // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG1CFGIDX = 3;       // DTSE Trigger 1 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG1EDGE = 2;         // Trigger falling edge
#endif
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AIO32, 0,           0);    // Convert M1-I, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AIO32, ADC_IRQ0_EN, 0);    // Dummy Conversion, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(5, ADC6, EMUX_AIO32, 0,           0);    // Dummy Conversion, read vbus, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(6, ADC6, EMUX_AIO32, 0,           0);    // Dummy Conversion, read vbus, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AIO32, 0,     SEQ_END);    // Dummy Conversion, then change CAFE MUX to M1-I

    #else   //#ifndef SELECT_I2SAMPLE_MIDDLEPWM
    // AS1 configuration
    // AS1 configuration
    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE1 = 0;            // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG1CFGIDX = 3;       // DTSE Trigger 1 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG1EDGE = 1;         // Trigger rising edge

    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AIO32, 0,           0);    // Convert M1-I, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AIO32, ADC_IRQ0_EN, 0);    // Dummy Conversion, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(5, ADC6, EMUX_AIO32, 0,           0);    // Dummy Conversion, read vbus, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(6, ADC6, EMUX_AIO32, 0,           0);    // Dummy Conversion, read vbus, then change CAFE MUX to M1-I
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AIO32, 0,     SEQ_END);    // Dummy Conversion, then change CAFE MUX to M1-I
    #endif  //#ifndef SELECT_I2SAMPLE_MIDDLEPWM

#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
#error: POWER_MEASURE_METHOD_2 not supported BOARD_EP5232EVK1_1SHUNT
#endif   
}

#ifdef ENABLE_IPD
void config_seq_ipd(uint32_t sequencer_trigger_value)
{
    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES6;

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);               // PG7 | PF6

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;     // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 2;       // Trigger falling edge

    pac5xxx_timer_a_ccctr0_value_set(sequencer_trigger_value);                              // Set value for AS0 trigger

    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC0, EMUX_AIO32, 0,           0);    // Convert Vout, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(1, ADC0, EMUX_AIO32, ADC_IRQ0_EN,           0);    // Dummy Conversion, then change CAFE MUX to M1-Iu
    pac5xxx_dtse_seq_config(2, ADC0, EMUX_AIO32, 0,           0);    // Convert M1-Iu, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AIO32, 0,           0);    // Dummy Conversion, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AIO32, 0,           0);    // Convert M1-Iv, then change CAFE MUX to M1-Iv
    pac5xxx_dtse_seq_config(5, ADC6, EMUX_AIO32, 0,           0);    // Dummy Conversion, then change CAFE MUX to M1-Iw
    pac5xxx_dtse_seq_config(6, ADC6, EMUX_AIO32, 0, 0);    // Convert M1-Iw, then change CAFE MUX to M1-Iu, generate ADC interrupt
    pac5xxx_dtse_seq_config(7, ADC0, EMUX_AIO32, 0,     SEQ_END);                             // Dummy conversion, then change CAFE MUX to M1-Iu
}
#endif  //#ifdef ENABLE_IPD

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_HALL) || (SEL_SIM_METHOD == SIM_METHOD_QEP))
void config_seq_sim(void)
{
    Struct_Motor* motor_ptr = &motor;

    // Set Vbus sequencer slot
    motor.fptr.pSeq_add = (ADC_RESULT_TYPEDEF*) &PAC55XX_ADC->DTSERES0;

    // Configure ADC input pin PG7(AD0) to read analog input from AFE and PF6(AD6) to read VBUS
    pac5xxx_adc_config_io(0x01 | 0x40);            // PG7 | PF6

    PAC55XX_ADC->DTSETRIGENT0TO3.FORCE0 = 0;          // Reset to 0
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0CFGIDX = 0;     // DTSE Trigger 0 Sequence Configuration Entry Index
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 1;       // Trigger rising edge

    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->control.motor_period_ticks - 4);            // Set value for AS0 trigger

    // Dummy conversions are added to each sequence to work around a bug in the DTSE
    pac5xxx_dtse_seq_config(0, ADC6, EMUX_AB7, 0,           0);       // Convert Vout, then change CAFE MUX to M1-Vu
    pac5xxx_dtse_seq_config(1, ADC0, EMUX_AB7, 0,           0);       // Dummy Conversion, then change CAFE MUX to M1-Vu
    pac5xxx_dtse_seq_config(2, ADC0, EMUX_AB8, 0,           0);       // Convert M1-Vu, then change CAFE MUX to M1-Vv
    pac5xxx_dtse_seq_config(3, ADC0, EMUX_AB8, 0,           0);       // Dummy Conversion, then change CAFE MUX to M1-Vv
    pac5xxx_dtse_seq_config(4, ADC0, EMUX_AB9, 0,           0);       // Convert M1-Vv, then change CAFE MUX to M1-Vw
    pac5xxx_dtse_seq_config(5, ADC0, EMUX_AB9, 0,           0);       // Dummy Conversion, then change CAFE MUX to M1-Vw
    pac5xxx_dtse_seq_config(6, ADC0, EMUX_AB7, ADC_IRQ0_EN, 0);       // Convert M1-Vw, then change CAFE MUX to M1-Vu, generate ADC interrupt
    pac5xxx_dtse_seq_config(7, ADC6, EMUX_AB7, 0,     SEQ_END);       // Dummy conversion, then change CAFE MUX to M1-Vu
}


void config_seq_simbrake(void)
{
    Struct_Motor* motor_ptr = &motor;
#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY)
    pac5xxx_timer_b_ccctr0_value_set(motor_ptr->sim.braking_duty_min_ticks);                              // Set value for AS0 trigger
#else
    // Configure sequencers and ADC
    pac5xxx_timer_a_ccctr0_value_set(motor_ptr->sim.braking_duty_min_ticks);                              // Set value for AS0 trigger
#endif
}


#elif defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
void config_seq_sim(void)
{
    #error: Current SIM not supported
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
#else
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x07);              // PROTINTM: LPPROT54/32/10
#endif
}

#endif // #if (BOARD_SELECT == BOARD_PAC5532EVK1_1SHUNT)
