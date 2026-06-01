
/**
 * @file pac_init.c
 * @brief Use to initialize peripheral, PWM, Hall, AFE, QEP, ADC, FOC, IPD
 */

//=============================================================================
// Copyright (C) 2019 - 2022, Qorvo, Inc.
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
#include "config_app.h"
#include "pac_init.h"
#include "driver_funcs.h"
#include "param_calc.h"
#include "param_funcs.h"
#include "comm.h"
#include "fault.h"
#include "pac5xxx_driver_adc.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_socbridge.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_system.h"
#include "pac5xxx_driver_memory.h"
#include "control_funcs.h"
#include "mpos_funcs.h"
#include "app_funcs.h"
#include "vbus_funcs.h"
#include "foc_funcs.h"
#include "foc_config.h"

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#if (ENABLE_WATCHDOG == TRUE)
#include "watchdog.h"
#endif

#if (ENABLE_STACK_CHECK == TRUE)
#include "stack_check.h"
#endif

#ifdef ENABLE_DEBUG_DAS
#include "debug_das.h"
#endif

#ifdef ENABLE_DEBUG_IO
#include "debug_io.h"
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC)
#include "control_loop.h"
#endif

#ifdef ENABLE_DEBUG_SAMPLE_RAM
#include "debug_capture.h"
#endif

#if defined(ENABLE_FLASHDEMO)
#include "flashdemo.h"
#endif

/**
 * @brief This function use to initialize peripheral, PLL, CLK, Ports, UART, AFE, DAS, Debug, calculate misc values
 */
void peripheral_init(void)
{
    Struct_Motor* motor_ptr = &motor;
    volatile int32_t test_count_var = 0;
    volatile unsigned char test_data;
    
    __disable_irq();     // Disable Interrupts
    
    // Initialize PLL, Clocks, Flash
    init_system();

    // Configure communication ports
    config_comm();

    // Configure SOC Bridge for talking to CAFE
    pac5xxx_tile_socbridge_config(1, 0);                                                    // enable, int_enable
#if defined(FAMILY_52XX)
    // Configure SOC Bridge clock divider depending on clok frequency selection
    initialize_socbridge_clk_divider();
#endif
    // Enable driver manager and verify active - need to enable even in PAC5210 to get ENHS pin to work
    pac5xxx_tile_register_write(ADDR_ENDRV, 0);
    if ((pac5xxx_tile_register_read(ADDR_ENDRV) & 0x01) != 0)
    {
        fault();
        while (1);
    }

    // Calculate PWM timer period ticks
    foc_calc_pwmtimer(motor_ptr);
    // Configure control loop for FOC mode
    foc_config(motor_ptr, 1);

    set_drivers_gpio();
    m1_disable();

#ifdef ENABLE_DEBUG_IO
    // Configure DebugIO Ports
    config_debugio();
//    debugio1_on();
//    debugio2_on();
#endif

    configure_cafe_fault_interrupt();

#ifdef ENABLE_DEBUG_PWMDAC
    // Configure PWMDAC timer (45kHz)
    configure_pwm_dac();
    pwmdac_disable();                                                                       // Set all outputs low
#endif

#ifdef ENABLE_DEBUG_DAS
    das_debug_init();
    config_debugDASio();
#endif

#ifdef ENABLE_DEBUG_SAMPLE_RAM
    debug_extraction_init(motor_ptr);
#endif

    // Hall IO and Timer config
#if (MODULE_MPOS_HALL == ENABLED)
    hall_io_config();
    hall_timer_config();
#if (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)
    configure_hall_interrupt_init();
#endif
#endif

    // QEP IO and Timer config
#if (MODULE_MPOS_QEP == ENABLED)
    qep_io_config();
    qep_timer_config();
#endif

    // GPIO config  by:owz
		pac5xxx_gpio_configure_interrupt_f(0x04|0x08,0x04|0x08);

    // Configure ADC and EMUX engine
    configure_adc_emux_engine();

    // Disable ADC
    pac5xxx_adc_enable(0);                                                                  // Disable ADC. Automated sequences will not begin

    // Initialize systick timer
    config_systick_timer();

#if (ENABLE_WATCHDOG == TRUE)
    // Enable Watchdog
    init_Watchdog_time_base();
#endif

    // Initialize start and end of the stack to test stack utilization
#if (ENABLE_STACK_CHECK == TRUE)
    init_stack_vars();
#endif

#if (INTERNAL_VMS == TRUE)
    cal_vm_const(motor_ptr);
#endif

#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
    flashrw_init();
#endif
}

/**
 * @brief This function use to configure Vp selection
 */
void config_register_bits(uint8_t address, uint8_t bits_mask, uint8_t bit_value)
{
    unsigned char value;
    value = pac5xxx_tile_register_read(address);
    value = (value & bits_mask) | bit_value;
    pac5xxx_tile_register_write(address, value);
}
