
/**
 * @file driver_funcs.c
 * @brief Use to initialize PWM duty, port mode and enable disable motor, low side PWM
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

#include "config_app.h"
#include "pac5xxx.h"
#include "motor_struct.h"
#include "driver_funcs.h"
#include "pac5xxx_driver_tile.h"


/**
 * @brief  This function use to enable motor
 */
void m1_enable(void)
{
    m1_enable_with_duty(motor.control.motor_period_ticks >> 1);
}


/**
 * @brief  This function use to set all three phases PWM duty cycle
 * @param  duty_cycle
 */
void m1_enable_with_duty(uint32_t duty_cycle)
{
    // Set duty cycles to 50%
    m1_u_set_duty(duty_cycle);
    m1_v_set_duty(duty_cycle);
    m1_w_set_duty(duty_cycle);

    // Select PWM peripheral for Driver Port
    motor_psel_pwm();
    
   // Select GPIO mode for Driver Port
    motor_modesel_gpio();
    
    // Turn on output enables
    motor_outen_enable();

    // Enable driver manager and verify active - need to enable even in PAC5210 to get ENHS pin to work
	enable_driver();
}


/**
 * @brief  This function use to enable PWM, GPIO and OutPut Enable
 */
void m1_enable_no_duty(void)
{
    // Select PWM peripheral for Driver Port
    motor_psel_pwm();

   // Select GPIO mode for Driver Port
    motor_modesel_gpio();
    
    // Turn on output enables
    motor_outen_enable();

    // Enable driver manager and verify active - need to enable even in PAC5210 to get ENHS pin to work
	enable_driver();
}


/**
 * @brief  This function use to set port to GPIO mode and set output to low
 */
void set_drivers_gpio(void)
{
    // Set IO state of all pins to 0
    motor_out_clear();
    // Select GPIO peripheral for Driver Port
    motor_psel_gpio();
    // Select GPIO mode for Driver Port
    motor_modesel_gpio(); 
    // Turn on output enables
    motor_outen_enable();
}


/**
 * @brief  This function use to disable motor
 */
void m1_disable(void)
{
    // Disable driver manager and verify active - need to enable even in PAC5210 to get ENHS pin to work
    pac5xxx_tile_register_write(ADDR_ENDRV, 0);

    // Set drivers to GPIO mode
    set_drivers_gpio();

    // Set duty cycles to 50%
    m1_u_set_duty(motor.control.motor_period_ticks >> 1);
    m1_v_set_duty(motor.control.motor_period_ticks >> 1);
    m1_w_set_duty(motor.control.motor_period_ticks >> 1);
}


/**
 * @brief  This function use to configure low side PWM
 */
void m1_LS_pwm_only_config(void)
{
    // Set IO state of all pins to 0
    motor_out_clear();

    // Select GPIO peripheral for Port A
    motor_psel_gpio();

    // Select PWM peripheral for Port A only low side
    motor_psel_pwm_LS();

    // Turn on output enables
    motor_outen_enable();

    // Enable driver manager
	enable_driver();
}
