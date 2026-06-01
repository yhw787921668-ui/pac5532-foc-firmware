
/**
 * @file ipd_config.c
 * @brief Use for IPD configuration
 */

//=============================================================================
// Copyright (C) 2021 - 2023, Qorvo, Inc.
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
#include "foc.h"
#include "param_calc.h"
#include "control_funcs.h"
#include "control_loop.h"
#include "foc_config.h"
#include "utils.h"
#include "board.h"

#ifdef ENABLE_IPD
#include "ipd_config.h"
#include "ipd.h"
#endif

#ifdef ENABLE_IPD

static inline void ipd_timer_int_deconfig(void)
{
#if defined(FAMILY_52XX)
    // Stop PWM Timer
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);
    // Disable Interrupts
    NVIC_DisableIRQ(ADC_IRQn);

#elif defined(FAMILY_55XX)
    // Stop PWM Timer
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    // Disable Interrupts
    NVIC_DisableIRQ(ADC_IRQn);

#endif  //defined(FAMILY_XXXX)
}


static inline void config_timer_int_ipd(void)
{
    Struct_Motor* motor_ptr = &motor;

#if defined(FAMILY_52XX)
    // Configure PWM Timer Base
    pac5xxx_timer_clear(FOC_PWM_Timer, 1);
    pac5xxx_timer_clock_config(FOC_PWM_Timer, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                // Configure timer clock input for ACLK (50MHz), /1 divider
    pac5xxx_timer_base_config(FOC_PWM_Timer, motor_ptr->ipd.timer_period, 0, TxCTL_MODE_UP, 0);    // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDL = 1;                                                        // Latch new TAPRD value when timerA counting down, TACTR value = 0x01
    // JAGTAG: Deadtime config has not been changed to PWM timer reference
    pac5xxx_dtg_config(DTGA0, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA0 for phase U
    pac5xxx_dtg_config(DTGA1, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA1 for phase V
    pac5xxx_dtg_config(DTGA2, MOTOR1_LED_TICKS, MOTOR1_TED_TICKS, 0, 0, 0, 0);              // Configure DTGA2 for phase W

    // Configure Interrupts
    pac5xxx_adc_int_enable_as0(1);                                               // Enable ADC interrupts for AS0 complete
    pac5xxx_adc_int_enable_as1(0);                                               // Disable ADC interrupts for AS1 complete

    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);                                                               // Enable ADC interrupts in the NVIC
    NVIC_SetPriority(ADC_IRQn,1);

    // Release Timers
    pac5xxx_timer_clear(FOC_PWM_Timer, 0);

#elif defined(FAMILY_55XX)
    // Configure IPD Timer Base
    FOC_PWM_Timer_Ptr->CTL.CLR = 1;
    pac5xxx_timer_clock_config((TimerInstance) FOC_PWM_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);    // Configure timer clock input for ACLK, /1 divider

    // Initialize Timer for IPD
    pac5xxx_timer_base_config((TimerInstance) FOC_PWM_Timer, motor_ptr->ipd.timer_period, 0, TxCTL_MODE_UP, 0);    // Enable base timer
    FOC_PWM_Timer_Ptr->CTL.PRDLATCH = TXCTL_PRDLATCH_TXCTR_0;

    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL0), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS); // Configure DTGA0 for phase U
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL1), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS); // Configure DTGA1 for phase V-
    pac5xxx_dtg_config2(&(FOC_PWM_Timer_Ptr->DTGCTL2), MOTOR1_LED_TICKS, MOTOR1_TED_TICKS); // Configure DTGA2 for phase W

    // Configure Interrupts
    // Configure NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn,1);
    // Release Timers
    FOC_PWM_Timer_Ptr->CTL.CLR = 0;

#endif  //defined(FAMILY_XXXX)
}


void ipd_fptr_deconfig(Struct_Motor* motor_ptr)
{
    //////////////////////////////////////////////
    // CONFIG_SHUNT_3SHUNT
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // MODE_1SHUNT_EXTENSION
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC_TIMER_55XX
    //////////////////////////////////////////////
    motor_ptr->fptr.fptr_int_ADC                =    (void (*)(void *)) &empty_function;
}

void ipd_deconfig(void)
{
    Struct_Motor* motor_ptr = &motor;

    // De-Configure timers and interrupts
    ipd_timer_int_deconfig();

    // De-Configure sequencer triggers
    deconfig_seq();

    // De-Configure pointers
    ipd_fptr_deconfig(motor_ptr);
}


void ipd_fptr_config(void)
{
    Struct_Motor* motor_ptr = &motor;

    //////////////////////////////////////////////
    // CONFIG_SHUNT_3SHUNT
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // MODE_1SHUNT_EXTENSION
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // MODE_1SHUNT_ASYMMETRIC_TIMER_55XX
    //////////////////////////////////////////////
    motor_ptr->fptr.fptr_int_ADC                =    (void (*)(void *)) &control_loop;
}


void ipd_config(void)
{
    Struct_Motor* motor_ptr = &motor;
    volatile uint32_t adc_trigger;
    uint32_t max_timer_ticks = MAX_PWM_TIMER_COUNTS - ADC_ISR_TICKS;

    // Set ISR pointers to IPD
    ipd_fptr_config();

    motor_ptr->ipd.angle_decision_threshold = IPD_ANGLE_DECISION_THRESHOLD;

#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    motor_ptr->ipd.num_shunts = 3;

#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    motor_ptr->ipd.num_shunts = 0;
#endif

    motor_ptr->ipd.motor_low_saliency_flag = IPD_LOW_SALIENCY_MOTOR;
    motor_ptr->ipd.current_accum_flag = ENABLE_IPD_CUR_ACCUM_COMPARE;
    motor_ptr->ipd.motor_disable_between_pulses_q0 = MOTOR_DISABLE_BETWEEN_PULSE_BY_PWM_COUNT;
    
    motor_ptr->ipd.phase_lost_detect_min_current_count = IPD_PHASE_LOST_DETECT_MIN_CURRENT_COUNT;

    motor_ptr->ipd.status = IPD_SUCCESS;

    // Set reasonable parameters if IPD Enabled, but none entered in GUI
    if (motor_ptr->ipd.num_periods_per_pulse == 0)
        motor_ptr->ipd.num_periods_per_pulse = 1;
    if (motor_ptr->ipd.pwm_charge_ticks == 0)
        motor_ptr->ipd.pwm_charge_ticks = 100;
    if (motor_ptr->ipd.pwm_discharge_percent == 0)
        motor_ptr->ipd.pwm_discharge_percent = 70;

    // Calculate Parameters for Pulse measurement
    motor_ptr->ipd.calculated_charge_ticks = motor_ptr->ipd.pwm_charge_ticks / motor_ptr->ipd.num_periods_per_pulse;
    motor_ptr->ipd.calculated_periods_per_pulse = motor_ptr->ipd.num_periods_per_pulse;
    motor_ptr->ipd.timer_period = motor_ptr->ipd.calculated_charge_ticks + ADC_ISR_TICKS;
    motor_ptr->ipd.period_count = 0;
    motor_ptr->ipd.delay_count = 0;

    // If only 1 period per pulse is desired and charge ticks are too large, then split over multiple periods
    if ( (motor_ptr->ipd.num_periods_per_pulse == 1) && (motor_ptr->ipd.calculated_charge_ticks > max_timer_ticks) )
    {
        motor_ptr->ipd.calculated_periods_per_pulse = 1 + (motor_ptr->ipd.calculated_charge_ticks / max_timer_ticks);
        motor_ptr->ipd.calculated_charge_ticks = motor_ptr->ipd.calculated_charge_ticks/motor_ptr->ipd.calculated_periods_per_pulse;
        motor_ptr->ipd.timer_period = motor_ptr->ipd.calculated_charge_ticks + ADC_ISR_TICKS;
    }

    // Set period longer to add some delay if num_periods_per_pulse is desired more than 1
    if (motor_ptr->ipd.num_periods_per_pulse > 1)
    {
        motor_ptr->ipd.timer_period = (motor_ptr->ipd.calculated_charge_ticks << 2) + ADC_ISR_TICKS;
        // The timer_period can't be greater than the max 16-bit value
        if (motor_ptr->ipd.timer_period > MAX_PWM_TIMER_COUNTS)
            motor_ptr->ipd.timer_period = MAX_PWM_TIMER_COUNTS;
    }

    // Calculate Discharge Ticks based on charge ticks
    motor_ptr->ipd.pwm_discharge_ticks = (motor_ptr->ipd.calculated_charge_ticks * motor_ptr->ipd.pwm_discharge_percent  * FIX16_1P0_DIV_100)>>16;  // Q0 = Q0 * Q0 * Q16 >> 16

    if (motor_ptr->ipd.calculated_charge_ticks < (IPD_ADC_TRIGGER_OFFSET_TICKS << 1))
         motor_ptr->ipd.calculated_charge_ticks =  IPD_ADC_TRIGGER_OFFSET_TICKS << 1;

    // IPD control divider is set to 0 because IPD control is running at PWM frequency 
    // We are not setting motor_ptr->control.dt_control_q26 and motor_ptr->control.dt_control_1div_q6, it set by default
    // using FOC default initialization
    motor_ptr->control.ctrl_divider = 0;

    // Configure sequencers for IPD
    config_seq_ipd(motor_ptr->ipd.calculated_charge_ticks - IPD_ADC_TRIGGER_OFFSET_TICKS);

    // Initialize IPD ADC Offsets
#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    motor_ptr->ipd.adc_offset_u_current = motor_ptr->post.adc_offset_aio10;
    motor_ptr->ipd.adc_offset_v_current = motor_ptr->post.adc_offset_aio32;
    motor_ptr->ipd.adc_offset_w_current = motor_ptr->post.adc_offset_aio54;
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO10)
    motor_ptr->ipd.adc_offset_u_current = motor_ptr->post.adc_offset_aio10;
    motor_ptr->ipd.adc_offset_v_current = motor_ptr->post.adc_offset_aio10;
    motor_ptr->ipd.adc_offset_w_current = motor_ptr->post.adc_offset_aio10;
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO32)
    motor_ptr->ipd.adc_offset_u_current = motor_ptr->post.adc_offset_aio32;
    motor_ptr->ipd.adc_offset_v_current = motor_ptr->post.adc_offset_aio32;
    motor_ptr->ipd.adc_offset_w_current = motor_ptr->post.adc_offset_aio32;
#endif

#if (CONFIG_OFFSET & CHANNEL_AIO54)
    motor_ptr->ipd.adc_offset_u_current = motor_ptr->post.adc_offset_aio54;
    motor_ptr->ipd.adc_offset_v_current = motor_ptr->post.adc_offset_aio54;
    motor_ptr->ipd.adc_offset_w_current = motor_ptr->post.adc_offset_aio54;
#endif

    // Configure timers and interrupts for FOC
    config_timer_int_ipd();
    // Set low duty cycle
    m1_u_set_duty(IPD_MINIMUM_DUTY);
    m1_v_set_duty(IPD_MINIMUM_DUTY);
    m1_w_set_duty(IPD_MINIMUM_DUTY);
}

#endif

