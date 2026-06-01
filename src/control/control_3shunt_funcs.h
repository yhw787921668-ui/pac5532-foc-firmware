
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

#ifndef CONTROL_3SHUNT_H
#define CONTROL_3SHUNT_H

#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_adc.h"
#include "motor_struct.h"
#include "adc_sample.h"
#include "isr_funcs.h"

PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void foc_config_duty(Struct_Motor *motor_ptr)
{
    // Duty min can get to 100% in 3-shunt but limited due to pulse extension issue
    motor_ptr->foc.svm_max_ticks = (motor_ptr->control.motor_period_ticks - PULSEWIDTH_MIN_LS_TICKS);               // Timer ticks in SVM max for this motor (for SVM)

    // Duty min can get to 0% in 3-shunt but limited due to pulse extension issue
#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
    motor_ptr->foc.svm_min_ticks = 1;                                                         // Timer ticks in SVM min for this motor (for SVM)
#else
    motor_ptr->foc.svm_min_ticks = PULSEWIDTH_MIN_HS_TICKS;                                                         // Timer ticks in SVM min for this motor (for SVM)
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline void read_adcoffset_iphase(Struct_Motor *motor_ptr)
{
    motor_ptr->threeshunt.adc_offset_iphase_u_q14 = motor_ptr->post.adc_offset_aio10;
    motor_ptr->threeshunt.adc_offset_iphase_v_q14 = motor_ptr->post.adc_offset_aio32;
    motor_ptr->threeshunt.adc_offset_iphase_w_q14 = motor_ptr->post.adc_offset_aio54;
}

PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting

PAC5XXX_RAMFUNC static inline void read_phase_currents(Struct_Motor *motor_ptr)
{
    // Read phase currents from sequencer buffer, convert to q14 scaled value, subtract offset, and compensate differential amp gain error
    motor_ptr->control.Iu_measured_q14 = ((readseq_iphase_u() * ADC_SCALE_CONST) >> 14);          //0q14 = "0q14" * 5q16 = 5q30>>16
    motor_ptr->control.Iu_measured_q14 = (-(motor_ptr->control.Iu_measured_q14 - motor_ptr->threeshunt.adc_offset_iphase_u_q14) << 1);
    motor_ptr->control.Iu_measured_q14 = ((motor_ptr->control.Iu_measured_q14 * COMP_DIFFGAINERROR_Q14) >> 14);                       // q14 = 0q14 * 1q14 = 1q28>>14

    motor_ptr->control.Iv_measured_q14 = ((readseq_iphase_v() * ADC_SCALE_CONST) >> 14);          //0q14 = "0q14" * 5q16 = 5q30>>16
    motor_ptr->control.Iv_measured_q14 = (-(motor_ptr->control.Iv_measured_q14 - motor_ptr->threeshunt.adc_offset_iphase_v_q14) << 1);
    motor_ptr->control.Iv_measured_q14 = ((motor_ptr->control.Iv_measured_q14 * COMP_DIFFGAINERROR_Q14) >> 14);                       // q14 = 0q14 * 1q14 = 1q28>>14

    motor_ptr->control.Iw_measured_q14 = ((readseq_iphase_w() * ADC_SCALE_CONST) >> 14);          //0q14 = "0q14" * 5q16 = 5q30>>16
    motor_ptr->control.Iw_measured_q14 = (-(motor_ptr->control.Iw_measured_q14 - motor_ptr->threeshunt.adc_offset_iphase_w_q14) << 1);
    motor_ptr->control.Iw_measured_q14 = ((motor_ptr->control.Iw_measured_q14 * COMP_DIFFGAINERROR_Q14) >> 14);                       // q14 = 0q14 * 1q14 = 1q28>>14
}
PAC5XXX_OPTIMIZE_SPEED_OFF


// Use to extract sector number in rev direction, index by sector calculated in SVM
static volatile int sector_info_reverse_dir[7] =
{
    0, 4, 3, 2, 1, 6, 5
};


///
/// @brief  Calculate motor Iu, Iv and Iw phase currents from ADC readings
///
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void process_phase_currents(Struct_Motor* motor_ptr)
{
    read_phase_currents(motor_ptr);

#if defined(ENABLE_CURRENT_RECONSTRUCTION)
    // Reconstruct 3 currents from 2 current measurements in order to reach 100% duty cycle
    switch(motor_ptr->control.mod_sector)
    {
        case 1:     //Measure Iv & Iw
        case 6:     //Measure Iv & Iw
            motor_ptr->control.Iu_measured_q14 = (-motor_ptr->control.Iv_measured_q14 - motor_ptr->control.Iw_measured_q14);
            break;
        case 2:     //Measure Iu & Iw
        case 3:     //Measure Iu & Iw
            motor_ptr->control.Iv_measured_q14 = (-motor_ptr->control.Iu_measured_q14 - motor_ptr->control.Iw_measured_q14);
            break;
        case 4:     //Measure Iu & Iv
        case 5:     //Measure Iu & Iv
            motor_ptr->control.Iw_measured_q14 = (-motor_ptr->control.Iu_measured_q14 - motor_ptr->control.Iv_measured_q14);
            break;

        default:
            break;
    }
#endif

    // Shift sector register
    motor_ptr->control.mod_sector = motor_ptr->control.mod_sector_sreg;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
static inline void control_3shunt_update_dutycycle(Struct_Motor *motor_ptr)
{
    // Update duty cycle from previous control loop run
    __disable_irq();
    m1_u_set_duty(motor_ptr->control.u_duty_value);
    m1_v_set_duty(motor_ptr->control.v_duty_value);
    m1_w_set_duty(motor_ptr->control.w_duty_value);
    __enable_irq();
}
PAC5XXX_OPTIMIZE_SPEED_OFF


static inline void config_timer_foc(Struct_Motor *motor_ptr)
{
#if defined(FAMILY_52XX)
    // Configure PWM Timer Base
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);
    pac5xxx_timer_clock_config(FOC_PWM_Timer, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                                // Configure timer clock input for ACLK (50MHz), /1 divider
    pac5xxx_timer_base_config(FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 0);    // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDL = 1;                                                        // Latch new TAPRD value when timerA counting down, TACTR value = 0x01
    // JAGTAG: Deadtime config has not been changed to PWM timer reference
    pac5xxx_dtg_config(DTGA0, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA0 for phase U
    pac5xxx_dtg_config(DTGA1, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA1 for phase V
    pac5xxx_dtg_config(DTGA2, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA2 for phase W

    // Release Timers
    pac5xxx_timer_clear(FOC_PWM_Timer, 0);

#elif defined(FAMILY_55XX)
    // Configure PWM Timer Base
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config((TimerInstance) FOC_PWM_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                                // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config((TimerInstance) FOC_PWM_Timer, motor_ptr->control.motor_period_ticks, 0, TxCTL_MODE_UPDOWN, 0);    // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDLATCH = 0;                                                    // Update duty cycle when TACTR counts from 1 to 0
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL0), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS); // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL1), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS); // Configure DTGA1 for phase V
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL2), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS); // Configure DTGA2 for phase W

    // Release Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 0;

#endif  //defined(FAMILY_XXXX)
}


static inline void deconfig_timer_foc(void)
{
#if defined(FAMILY_52XX)
    // Stop PWM Timer
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);

#elif defined(FAMILY_55XX)
    // Stop PWM Timer
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;

#endif  //defined(FAMILY_XXXX)
}


static inline void config_int_foc(Struct_Motor *motor_ptr)
{
#if defined(FAMILY_52XX)
    // Configure Interrupts
    pac5xxx_adc_int_enable_as0(1);                                               // Enable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(0);                                               // Disable ADC interrupts for AS1 complete
    clear_adc_int_flag();

    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn,1);

#elif defined(FAMILY_55XX)
    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    #ifdef ENABLE_QEP_LINEARIZATION_TUNING
    NVIC_SetPriority(ADC_IRQn,2);
    #else
    NVIC_SetPriority(ADC_IRQn,1);
    #endif
#endif  //defined(FAMILY_XXXX)
}


static inline void deconfig_int_foc(void)
{
    // De-Configure NVIC
    NVIC_DisableIRQ(ADC_IRQn);
}


static inline void config_int_ctrlctr(void)
{
#if defined(FAMILY_52XX)
    // Configure Interrupts
    pac5xxx_timer_cctrl_int_enable((TimerInstance) FOC_PWM_Timer, 0, 1);         // Enable CCR0 interrupts for control counter
    FOC_PWM_Timer_Ptr->CCTRL0.CCINT = 1;

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#elif defined(FAMILY_55XX)
    // Configure Interrupts
    FOC_PWM_Timer_Ptr->CCTL0.CCINTEN = 1;           // Enable CCR0 interrupts for control counter
    FOC_PWM_Timer_Ptr->INT.CCR0IF = 1;

    // Configure NVIC
    NVIC_EnableIRQ(FOC_PWM_Timer_IRQn);
    NVIC_SetPriority(FOC_PWM_Timer_IRQn,0);

#endif  //defined(FAMILY_XXXX)
}


static inline void deconfig_int_ctrlctr(void)
{
#if defined(FAMILY_52XX)
    // Disable Interrupts
    pac5xxx_timer_cctrl_int_enable((TimerInstance) FOC_PWM_Timer, 0, 0);

#elif defined(FAMILY_55XX)
    // Disable Interrupts
    FOC_PWM_Timer_Ptr->CCTL0.CCINTEN = 0;
    FOC_PWM_Timer_Ptr->INT.CCR0IF = 1;

#endif  //defined(FAMILY_XXXX)
}

#endif  //#ifndef CONTROL_3SHUNT_H
