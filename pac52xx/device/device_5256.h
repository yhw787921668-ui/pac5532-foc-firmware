
/**
 * @file device_5256.h
 * @brief Use for 5256 device definition
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
 
#ifndef DEVICE_5256_H
#define DEVICE_5256_H

#include "pac5256.h"
#include "pac5xxx.h"
#include "device_options.h"
#include "family_52xx.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_socbridge.h"

//////////////////////////////////////////////////////////
// Resource: FOC Timer  FOC Timer Interrupt, ADC Interrupt
//////////////////////////////////////////////////////////
#define PWM_TIMER TIMER_A
#define FOC_PWM_Timer TimerA
#define FOC_PWM_Timer_Ptr PAC5XXX_TIMERA
#define FOC_PWM_Timer_IRQn TimerA_IRQn
#define ADC_IRQn ADC_IRQn
#define FOC_ASYM_SUPPLE_Timer TimerB
#define FOC_ASYM_SUPPLE_Timer_Ptr PAC5XXX_TIMERB
#define FOC_ASYM_SUPPLE_Timer_IRQn TimerB_IRQn

#define PUSER_VALUE_DIFFAMP_GAIN_FINAL PUSER_VALUE_DIFFAMP_GAIN
#define PUSER_VALUE_DIFFAMP_GAIN_FIXED                    FALSE

//====================================
// Gate Driver Configuration
//====================================
// Motor
#define MOTOR1_PWM_HS_PIN_PORT_A    0xC0                                                        // Pin mask for HS gate drive PWM for Port A
#define MOTOR1_PWM_LS_PIN_PORT_A    0x07                                                        // Pin mask for LS gate drive PWM for Port A
#define MOTOR1_PWM_PIN_PORT_A       (MOTOR1_PWM_HS_PIN_PORT_A | MOTOR1_PWM_LS_PIN_PORT_A)

#define MOTOR1_PWM_HS_PIN_PORT_D    0x80                                                        // Pin mask for HS gate drive PWM for Port D
#define MOTOR1_PWM_PIN_PORT_D       (MOTOR1_PWM_HS_PIN_PORT_D)

#define MOTOR1_PASEL_MASK           0x5015                                                      // Peripheral select mask for Port A
#define MOTOR1_PDSEL_MASK           0x4000                                                      // Peripheral select mask for Port D
#define MOTOR1_PASEL_LS_MASK        0x0015                                                      // Peripheral select mask for Port A for low side only DTGA0LS, DTGA1LS and DTGA2LS

//==========================================
// Motor Driver Control Macro Functions
//==========================================
/**
 * @brief motor_psel_pwm, use to select port_a and port_d pins as PWM output
 */
static inline void motor_psel_pwm(void)
{
    // Configure Port PSELs to output PWMs to Drivers
    PAC5XXX_GPIOA->PSEL.w |= MOTOR1_PASEL_MASK;
    PAC5XXX_GPIOD->PSEL.w |= MOTOR1_PDSEL_MASK;
}

/**
 * @brief motor_outen_enable, use to enable port_a and port_d output
 */
static inline void motor_outen_enable(void)
{
    PAC5XXX_GPIOA->OUTEN.b |= MOTOR1_PWM_PIN_PORT_A;
    PAC5XXX_GPIOD->OUTEN.b |= MOTOR1_PWM_PIN_PORT_D;
}

/**
 * @brief motor_psel_gpio, use to select port_a and port_d as gpio
 */
static inline void motor_psel_gpio(void)
{
    // Revert PWM Driver outputs to GPIOs
    // Select GPIO peripheral for Port A and D
    PAC5XXX_GPIOA->PSEL.w &= ~MOTOR1_PASEL_MASK;
    PAC5XXX_GPIOD->PSEL.w &= ~MOTOR1_PDSEL_MASK;
}

/**
 * @brief motor_out_clear, use to clear port_a and port_d output
 */
static inline void motor_out_clear(void)
{
    // Set IO state of all pins to 0
     PAC5XXX_GPIOA->OUT.b &= ~MOTOR1_PWM_HS_PIN_PORT_A;
     PAC5XXX_GPIOA->OUT.b &= ~MOTOR1_PWM_LS_PIN_PORT_A;
     PAC5XXX_GPIOD->OUT.b &= ~MOTOR1_PWM_HS_PIN_PORT_D;
}

/**
 * @brief motor_psel_pwm_LS, use to select port_a pa0,pa1,pa2 in PWM mode
 */
static inline void motor_psel_pwm_LS(void)
{
    // Configure Port PSELs to output Low Side PWMs to Drivers
    pac5xxx_timer_io_select_pwma0_pa0();                                                    // Motor-1: UL/PWMA0: PA0
    pac5xxx_timer_io_select_pwma1_pa1();                                                    // Motor-1: VL/PWMA1: PA1
    pac5xxx_timer_io_select_pwma2_pa2();                                                    // Motor-1: WL/PWMA2: PA2
}

// Empty function for pac52xx, just a place holder
/**
 * @brief motor_modesel_gpio, this is empty function as place holder
 */
static inline void motor_modesel_gpio(void)
{
}

/**
 * @brief enable_bbm, this is enable break before make functionality
 */
static inline void enable_bbm()
{
}

// CAFE specific definition
//=====================================
// Low OC Protection Signal (AIOxx_LPROT)
//=====================================
#define AIO50_LPROT_NONE        0
#define AIO50_LPROT_PR          BIT_04

//=====================================
// High OC Protection Signal (AIOxx_HPROT)
//=====================================
#define AIO50_HPROT_NONE        0
#define AIO50_HPROT_PR          BIT_05

//=====================================
// Driver protection while OC 
//=====================================
#define DRV_HIGHSIDE_PR         BIT_03
#define DRV_LOWSIDE_PR          BIT_02

//=====================================
// Vp voltage selection 
//=====================================
#define VP_VOLTAGE_12V       (0x00 << 3)
#define VP_VOLTAGE_15V       (0x01 << 3)
#define VP_CONFIG_BITS_MASK         0xF7

//==========================================================
// Min duty cycle: Version A can be set as 0, otherwise as 1
//==========================================================
#define DUTY_CYCLE_MIN_TICK     0

//==================================================================
// For internal Vms, duty cycle exclusion is true for 5556 and 5256
//==================================================================
#define VMS_DUTY_CYCLE_EXCLUSION           TRUE

//=====================================
// Internal VMS measurement method
//=====================================
#define VMS_FACTORY_CAL          TRUE

#endif  //#ifndef DEVICE_5256_H
