
/**
 * @file config_debug.h
 * @brief Use for different FOC firmware debug option selection
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

#ifndef CONFIG_DEBUG_H
#define CONFIG_DEBUG_H
//=============================================================================
//
// Debug Features
//
//=============================================================================

//#define ENABLE_DEBUG                                // Enable debug features
#ifdef ENABLE_DEBUG
//    #define ENABLE_DEBUG_PWMDAC                     // Enable PWM DAC's for debugging
    #define ENABLE_DEBUG_DISPLAY                    // Enable debug display for debugging
    #define ENABLE_DEBUG_IO
    #define ENABLE_DEBUG_INPUT
//    #define ENABLE_DEBUG_DAS
//    #define ENABLE_DEBUG_RTT                        // Debug with Jscope/Jlink
    #define ENABLE_DEBUG_PARAMEXPORT                // Enable variable read display for debugging
//    #define ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK    // Checks if the FOC control loop exceeds the available time (determined by the set control frequency).  If exceeded, a warning is displayed.
    #define ENABLE_STACK_CHECK      FALSE
//    #define IPD_DEBUG_ENABLE
//    #define ENABLE_DEBUG_VOLTAGEMODE
//    #define ENABLE_TEMPREAD                       // This is for PAC5285 only since it integrates FETs inside the device
//    #define ENABLE_DEBUG_SAMPLE_RAM
    
#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS)
//    #define DEBUGDAC_DISPLAYDUTY_TICKS
#endif

#if defined(ENABLE_DEBUG_SAMPLE_RAM)
//#warning "Debug extraction data to RAM was enabled, please consider RAM size before setting maximum number of sample!"
#define DBG_ARRAY_LENGTH                (24)       // 480x4 = 1920 bytes 
#endif

#define ENABLE_DEBUG_PACINFO
//#define ENABLE_DEBUG_FLASHDEMO
#endif  //#ifdef ENABLE_DEBUG

//====================================
// CONFIG DAS: Version
//====================================
#ifdef ENABLE_DEBUG_DAS
    #define DAS_VERSION_1       1
    #define DAS_VERSION_2       2
    #define DAS_VERSION         DAS_VERSION_1
#endif

//=============================================================================
//
// Test Modes
//
//=============================================================================
//#define ENABLE_TM                                   // Enable test modes
#ifdef ENABLE_TM

//====================================
// CONFIG TM: SIM
//====================================
#define ENABLE_TM_SIM
#if defined(ENABLE_TM_SIM)
//    #define TM_SIM_SLOWSPEED                        // Enable TM for testing SIM behavior during slow speed detection. After disabling motor will slow down then re-enable after a stop is detected.
    #define TM_SIM_SWCL                             // Enable TM for testing SIM switch to CL. SIM should switch directly to CL shortly after it was disabled.
//    #define TM_SIM_REVERSE                          // Enable TM for testing SIM behavior during reversal of direction. Motor will automatically change direction when a stop is detected and re-start.
//    #define TM_SIM_NODETECT                         // Enable this TM to check if motion detection is good. When motor is disabled, the motor will slow down until the motion signal is not detected anymore.  The last valid SIM speed value is stored before motion is not detected and it is displayed in param export. No need to set testmode_enable flag.
#endif

//====================================
// CONFIG TM: Auto Speed
//====================================
//    #define ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
        #define SPEED_CL_LIMIT_CHANGE_CALL_BACK_TIME    50                      // 50 * 10msec = 500 msec, function get called after every 500 msec
        #define CL_SPEED_LIMIT_TEST_COUNTER_MAX_VALUE   20                      // 0.5 sec * 20 = 10 seconds, motor will run for limit1 for around 8 seconds and limit2 for 2 seconds
        #define CL_SPEED_LIMIT_TEST_COUNTER_FOR_SPEED1  (CL_SPEED_LIMIT_TEST_COUNTER_MAX_VALUE - (CL_SPEED_LIMIT_TEST_COUNTER_MAX_VALUE >> 3))
#endif

//====================================
// CONFIG TM: Miscellaneous
//====================================
//    #define TM_PARAMCHECK
//    #define TM_CONTORL_FORCEVQ
//    #define TM_CONTORL_FORCEVDVQ

#endif  //#ifdef ENABLE_TM

//====================================
// DEBUG MODE: VBUS
//====================================
//#define DISABLE_AUTOVDC                                              // Measures the bus voltage on the ADC.  If disabled, a fixed bus voltage can be set through the GUI.
#ifdef DISABLE_AUTOVDC
    #define VBUS_FIXED_Q16                              QCONST(20.0f, Q16)
#endif

//====================================
// DEBUG MODE: Use to read data from flash RAM array
//====================================
//#define ENABLE_FLASHDEMO_DEBUGREAD

#endif  //#ifndef CONFIG_DEBUG_H
