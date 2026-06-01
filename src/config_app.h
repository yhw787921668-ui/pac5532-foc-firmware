
/**
 * @file config_app.h
 * @brief Use for selecting different FOC firmware application features
 */

//=============================================================================
// Copyright (C) 2017 - 2022, Qorvo, Inc.
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

#ifndef CONFIG_APP_H
#define CONFIG_APP_H

#include "config_options.h"
#include "board.h"


//====================================
// Parameter Selection
//====================================
//#define PARAM_SELECT                PARAM_DEFAULT
#define PARAM_SELECT                PARAM_USER_1


//=============================================
// CONFIG: Comm Selection
//=============================================
#define COMM_SELECT                 COMM_UART
//#define COMM_SELECT                 COMM_PUSH_BUTTON
//#define COMM_SELECT                 COMM_COMBO_PUSH_BUTTON_UART
//#define COMM_SELECT                 COMM_CAN
//#define COMM_SELECT                 COMM_LIN

#if(COMM_SELECT == COMM_CAN)
#define TARGET_CAN_ID   0x200           // CAN msg ID that Target board accepts  0x200
#define ADAPTER_CAN_ID  0x100           // Can msg id that Adapter accepts       0x100
#define CAN_IO  CAN_PE23
#define CAN_BR  CAN_BAUD_250KHz
#endif

#if(COMM_SELECT == COMM_LIN)
#define LIN_PID_READ        0xF0       // PID in schedule table that indicate slave will read byte from master     
#define LIN_PID_RESPONSE  0xB1       // PID in schedule table that indicate slave will insert byte to lin frame
#define LIN_BR                      19200
#endif


//=============================================
// CONFIG: Motor Position (MPOS) Mode Selection
//=============================================
//#define MPOS_SELECT                 MPOS_LBG1_START_ALIGNGO
#define MPOS_SELECT                 MPOS_HALL_ONLY
//#define MPOS_SELECT                 MPOS_LBG1_START_HALL
//#define MPOS_SELECT                 MPOS_QEP_START_ALIGNGO
//#define MPOS_SELECT                 MPOS_LBG1_START_QEP
//#define MPOS_SELECT                 MPOS_LBG1_START_TRAP_ALIGNGO              // Experimental, For PAC5523EVK only


//=============================================
// MPOS Configuration : AlignGo+Est Mode
//=============================================
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    // CONFIG: Open loop nonlinear ramp
//    #define ENABLE_OPEN_LOOP_NONLINEAR_RAMP
    #ifdef ENABLE_OPEN_LOOP_NONLINEAR_RAMP
        #define OPEN_LOOP_START_SPEED_TARGET_PCT_Q08        QCONST(0.50, Q08)               // Open loop speed target percentage of "aligngo.speedrefol_target"
        #define OPEN_LOOP_START_RAMP_PCT_Q08                QCONST(0.25, Q08)               // Starting ramp rate percentage of "aligngo.speedrefol_rate"
//        #define ENABLE_CURRENT_RAMP_DURING_FIRST_SLOPE                                    // Enable the current ramp while the first speed ramp slope
    #endif

//    #define ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP
    #ifdef ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP
        #define VOL_SWITCH_OVER_CUR_PERCENT_Q04             QCONST(1.0f, Q04)              // Decide the time point to switch from Volt method to Cur method
        #define VOL_RAMP_INC_Q14                            QCONST(0.002f, Q14)             // Voltage incremental step
        #define VOL_MAX_TARGET_Q14                          QCONST(0.15f, Q14)              // Max open loop voltage target
    #endif

    #define ENABLE_OPEN_LOOP_RAMP_NONZERO_START
    #ifdef ENABLE_OPEN_LOOP_RAMP_NONZERO_START
        #define OPEN_LOOP_RAMP_START_PCT_Q16                QCONST(0.25, Q16)               // Open loop start speed percentage of "aligngo.speedrefol_target"
    #endif

    // Apply angle correction to calculated angle consisting of a constant offset component and a speed-proportional component
    #define ENABLE_ANGLE_CORRECTION
    #define ENABLE_PHASE_COMPENSATION
    // These constant definitions are used to configure the transits during switching from OL to CL
    #define LBG_KGAIN_START_PCT_F16                         QCONST(0.2, Q16)                // LBG bandwidth during open loop, per unit
    #define LBG_KGAIN_INC_PCT_F16                           QCONST(0.1, Q16)                // LBG bandwidth ramping rate, per unit

    // Enable LBG angle calculation with PLL, otherwise Arctan method would be used
    #define ENABLE_PLL_ANGLE_CALCULATION
    #ifdef ENABLE_PLL_ANGLE_CALCULATION
        #define LBG_PLL_DAMPING_FACTOR_Q16                  QCONST(0.707, Q16)
        #define LBG_PLL_KGAIN_START_PCT_F16                 QCONST(0.75, Q16)                // LBG PLL bandwidth during open loop, per unit
        #define LBG_PLL_KGAIN_INC_PCT_F16                   QCONST(0.25, Q16)                // LBG PLL bandwidth ramping rate, per unit
    #endif

    // Check that the estimated speed and angle do not differ by a certain error threshold from OL speed and forced angle.  If check fails, a warning is displayed.
    #define ANGLEDIFF_SPEED_ERROR_PCT_Q16               QCONST(0.10, Q16)
    #define ANGLEDIFF_ANGLE_ERROR_DEG_Q12               QCONST((3.14/16), Q12)

    // Enable open loop anglediff auto ranging when switching from AlignGo to Estimator mode
    #define ENABLE_ANGLEDIFF_AUTORANGE
    #ifdef ENABLE_ANGLEDIFF_AUTORANGE
        #define ANGLEDIFF_AUTORANGE_METHOD_SELECT           ANGLEDIFF_AUTORANGE_METHOD_1
//        #define ANGLEDIFF_AUTORANGE_METHOD_SELECT           ANGLEDIFF_AUTORANGE_METHOD_2    // Method 2 is only for testing purposes only
        #if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_1)
            #define ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS                        // Run Idref ramping dt to 2ms (original 10ms) for improving fast startup performance
            #ifdef ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS
                #define ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT     0.01f           // AngleDiff autorange method1 increments every systick as % of Itot
            #else
                #define ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT     0.05f           // AngleDiff autorange method1 increments every systick as % of Itot
            #endif
            #define IDREF_RAMP_RATE_SHIFT_Q00       0                           // Idref ramp rate attenuation during switch over
            //#define ENABLE_IDREF_BIDIRECTION                                  // Allow IDREF set as positive and negative value
        #elif (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
            #define ALIGNGO2EST_ANGLERAMP_INCREMENT_DEGPS_Q06       QCONST(2000.0f, Q06)
        #endif
    #endif

    // Enables timeout for estimated speed and angle check.  If timeout is exceeded, the motor is disabled.
    #define ENABLE_ANGLEDIFF_TIMEOUT
    #ifdef ENABLE_ANGLEDIFF_TIMEOUT
        #define ANGLEDIFF_TIMEOUT_SEC                       2               // AngleDiff Timeout (sec)
    #endif
    
    /* Disabled as default! Debug and test use only */
    // Blending speed or angle during switch over
    //#define ENABLE_SWITCH_OVER_SPEED_BLEND
    //#define ENABLE_SWITCH_OVER_ANGLE_BLEND
    #ifdef ENABLE_SWITCH_OVER_ANGLE_BLEND
        #define ALIGNGO2EST_ANGLEBLEND_TOTALCNT             100
        #define ALIGNGO2EST_ANGLEBLEND_FACTOR_Q12           QCONST((1.0 / (float)ALIGNGO2EST_ANGLEBLEND_TOTALCNT), Q12)
    #endif
#endif

//=============================================
// MPOS Configuration : Hall+Est Mode
//=============================================
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    #define ENABLE_HALL_ANGLE_INTERPOLATION
    // Apply angle correction to calculated angle consisting of a constant offset component and a speed-proportional component
    #define ENABLE_ANGLE_CORRECTION
    #define ENABLE_PHASE_COMPENSATION
    #define ENABLE_EST_ASSIST
    #if defined(ENABLE_EST_ASSIST)
        // Estimator assist configuration
        #define CONFIG_ESTASSIST_FILTERSHIFT               8                   // Hall assist filter depth
        #define CONFIG_ESTASSIST_CTRL_INC                  6                   // Increment for hall assist angle convergence controller
        #define CONFIG_ESTASSIST_CTRL_THRESH               QCONST(0.001f, Q16) // Adaptive threshold for hall assist angle controller
    #endif

    // These constant definitions are used to configure the transits during switching from OL to CL
    #define LBG_KGAIN_START_PCT_F16         QCONST(0.1, Q16)                        // LBG bandwidth during open loop, per unit
    #define LBG_KGAIN_INC_PCT_F16           QCONST(0.1, Q16)                        // LBG bandwidth ramping rate, per unit
    #define IDREF_RAMP_RATE_SHIFT_Q00       0                                       // Idref ramp rate attenuation during switch over

    // Enable LBG angle calculation with PLL, otherwise Arctan method would be used
    //#define ENABLE_PLL_ANGLE_CALCULATION
    #ifdef ENABLE_PLL_ANGLE_CALCULATION
        #define LBG_PLL_DAMPING_FACTOR_Q16                  QCONST(0.707, Q16)
        #define LBG_PLL_KGAIN_START_PCT_F16                 QCONST(0.25, Q16)                // LBG PLL bandwidth during open loop, per unit
        #define LBG_PLL_KGAIN_INC_PCT_F16                   QCONST(0.25, Q16)                // LBG PLL bandwidth ramping rate, per unit
    #endif

    // Hall sequence check verifies that the next hall state detected follows the expected hall sequence
    #define ENABLE_HALL_SEQUENCE_CHECK

    // Hall state detection method
    #define HALL_METHOD_SELECT                         HALL_METHOD_INTERRUPT
//    #define HALL_METHOD_SELECT                         HALL_METHOD_POLLING

    // Add delay compensation to hall angle due to control loop time and control loop start time relative to hall edge
    #define ENABLE_HALL_ANGLE_DELAY_COMPENSATION

    // Configure hall transition timeout, minimum timeout frequency is 1 Hz
    #define CONFIG_HALL_TIMEOUT_EFREQ_HZ                5.0f
//    #define CONFIG_HALL_TIMEOUT_MDISABLE                // Motor is disabled when hall timeout occurs

    // Define depth of hall filter shift
    #define CONFIG_HALLFILTER_SHIFT                     2                   // Hall speed filter shift value

    // Hysteresis band for switching from Estimator to Hall mode as a percentage of user defined switchover speed
    #define SWITCHOVER_SPD_HALLEST_HYST_PCT_Q16             QCONST(0.15f, Q16)

    // Check that the angle and speed are within an error threshold before switching between estimator and hall modes.  If check fails, a warning is displayed.
    #define ANGLEDIFF_SPEED_ERROR_PCT_Q16                   QCONST(0.10, Q16)
    #define ANGLEDIFF_ANGLE_ERROR_DEG_Q12                   QCONST((3.14/16), Q12)
#endif

//=============================================
// MPOS Configuration : Hall Only Mode
//=============================================
#if (MPOS_SELECT == MPOS_HALL_ONLY)
    #define ENABLE_HALL_ANGLE_INTERPOLATION

    // Apply angle correction to calculated angle consisting of a constant offset component and a speed-proportional component
    #define ENABLE_ANGLE_CORRECTION

    // Hall sequence check verifies that the next hall state detected follows the expected hall sequence
    #define ENABLE_HALL_SEQUENCE_CHECK

    // Hall state detection method
    #define HALL_METHOD_SELECT                         HALL_METHOD_INTERRUPT
//    #define HALL_METHOD_SELECT                         HALL_METHOD_POLLING

    // Add delay compensation to hall angle due to control loop time and control loop start time relative to hall edge
    #define ENABLE_HALL_ANGLE_DELAY_COMPENSATION

    // Configure hall transition timeout
    #define CONFIG_HALL_TIMEOUT_EFREQ_HZ                5.0f
//    #define CONFIG_HALL_TIMEOUT_MDISABLE                // Motor is disabled when hall timeout occurs

    #define IDREF_RAMP_RATE_SHIFT_Q00                   0                   // Idref ramp rate attenuation during switch over

    // Handle the hall motor with defective hall signals
//    #define ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION
    #if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
        #undef ENABLE_HALL_SEQUENCE_CHECK
        // Define depth of hall filter shift
        #define CONFIG_HALLFILTER_SHIFT                     4                // Hall speed filter shift value
    #else
        // Define depth of hall filter shift
        #define CONFIG_HALLFILTER_SHIFT                     2                // Hall speed filter shift value
    #endif
#endif

//=============================================
// MPOS Configuration : AlignGo+QEP Mode
//=============================================
#if (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    // Parameter configuration: Resolution per phase
    #define QEP_PULSE_PER_RESOLUTION                    2048
    // Run QEP in quadrature mode
    #define ENABLE_QEP_QUAD_MODE

    // Apply angle correction to calculated angle consisting of a constant offset component
    #define ENABLE_ANGLE_CORRECTION

    // Enables timeout for estimated speed and angle check.  If timeout is exceeded, the motor is disabled.
    #define ENABLE_QEP_STARTUP_TIMEOUT
    #ifdef ENABLE_QEP_STARTUP_TIMEOUT
        #define NUM_QEP_MECH_ROTATION                       3                   // AngleDiff Timeout in QEP Mechanical Rotation
    #endif

    #define IDREF_RAMP_RATE_SHIFT_Q00                       0                   // Idref ramp rate attenuation during switch over
    // QEP speed calculation divider for better accuracy at lower speed
    #define QEP_SPEED_CALCULATION_DIVIDER                   1                   // Every ("QEP_SPEED_CALCULATION_DIVIDER" * dT) calcuates speed once
#endif


//=============================================
// MPOS Configuration : Est+QEP Mode
//=============================================
#if (MPOS_SELECT == MPOS_LBG1_START_QEP)
    // Apply angle correction to calculated angle consisting of a constant offset component and a speed-proportional component
    #define ENABLE_ANGLE_CORRECTION
    #define ENABLE_PHASE_COMPENSATION
    #define ENABLE_EST_ASSIST
    #if defined(ENABLE_EST_ASSIST)
        // Estimator assist configuration
        #define CONFIG_ESTASSIST_FILTERSHIFT                8                   // EStimator assist filter depth
        #define CONFIG_ESTASSIST_CTRL_INC                   6                   // Increment for estimator assist angle convergence controller
        #define CONFIG_ESTASSIST_CTRL_THRESH                QCONST(0.001f, Q16) // Adaptive threshold for estimator assist angle controller
    #endif

    // These constant definitions are used to configure the transits during switching from OL to CL
    #define LBG_KGAIN_START_PCT_F16         QCONST(0.1, Q16)                        // LBG bandwidth during open loop, per unit
    #define LBG_KGAIN_INC_PCT_F16           QCONST(0.1, Q16)                        // LBG bandwidth ramping rate, per unit
    #define IDREF_RAMP_RATE_SHIFT_Q00       0                                       // Idref ramp rate attenuation during switch over

    // Parameter configuration: Resolution per phase
    #define QEP_PULSE_PER_RESOLUTION                    2048
    // Run QEP in quadrature mode
    #define ENABLE_QEP_QUAD_MODE

    // Hysteresis band for switching from Estimator to Hall mode as a percentage of user defined switchover speed
    #define SWITCHOVER_SPD_QEPEST_HYST_PCT_Q16             QCONST(0.15f, Q16)

    #define ENABLE_ANGLEDIFF_CHECK_QEP2EST

    // Check that the estimated speed and angle do not differ by a certain error threshold from OL speed and forced angle.  If check fails, a warning is displayed.
    #define ANGLEDIFF_SPEED_ERROR_PCT_Q16                   QCONST(0.10, Q16)   // Entered as decimal equivalent of percentage
    #define ANGLEDIFF_ANGLE_ERROR_DEG_Q12                   QCONST((3.14/16), Q12)

    // Enables timeout for estimated speed and angle check.  If timeout is exceeded, the motor is disabled.
    #define ENABLE_QEP_STARTUP_TIMEOUT
    #ifdef ENABLE_QEP_STARTUP_TIMEOUT
        #define NUM_QEP_MECH_ROTATION                       3                   // AngleDiff Timeout in QEP Mechanical Rotation
    #endif

    #define IDREF_RAMP_RATE_SHIFT_Q00                       0                   // Idref ramp rate attenuation during switch over
    // QEP speed calculation divider for better accuracy at lower speed
    #define QEP_SPEED_CALCULATION_DIVIDER                   1                   // Every ("QEP_SPEED_CALCULATION_DIVIDER" * dT) calcuates speed once
#endif


//=============================================
// MPOS Configuration : AlignGo+Trap+Est Mode
//=============================================
#if (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    // CONFIG: Open loop nonlinear ramp
//    #define ENABLE_OPEN_LOOP_NONLINEAR_RAMP
    #ifdef ENABLE_OPEN_LOOP_NONLINEAR_RAMP
        #define OPEN_LOOP_START_SPEED_TARGET_PCT_Q08        QCONST(0.50, Q08)               // Open loop speed target percentage of "aligngo.speedrefol_target"
        #define OPEN_LOOP_START_RAMP_PCT_Q08                QCONST(0.25, Q08)               // Starting ramp rate percentage of "aligngo.speedrefol_rate"
//        #define ENABLE_CURRENT_RAMP_DURING_FIRST_SLOPE                                    // Enable the current ramp while the first speed ramp slope
    #endif

    #define ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP
    #ifdef ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP
        #define VOL_SWITCH_OVER_CUR_PERCENT_Q04             QCONST(0.5f, Q04)              // Decide the time point to switch from Volt method to Cur method
        #define VOL_RAMP_INC_Q14                            QCONST(0.002f, Q14)             // Voltage incremental step
        #define VOL_MAX_TARGET_Q14                          QCONST(0.05f, Q14)              // Max open loop voltage target
    #endif

    #define ENABLE_OPEN_LOOP_RAMP_NONZERO_START
    #ifdef ENABLE_OPEN_LOOP_RAMP_NONZERO_START
        #define OPEN_LOOP_RAMP_START_PCT_Q16                QCONST(0.25, Q16)               // Open loop start speed percentage of "aligngo.speedrefol_target"
    #endif

    // Apply angle correction to calculated angle consisting of a constant offset component and a speed-proportional component
    #define ENABLE_ANGLE_CORRECTION
    #define ENABLE_PHASE_COMPENSATION
    // These constant definitions are used to configure the transits during switching from OL to CL
    #define LBG_KGAIN_START_PCT_F16                         QCONST(0.2, Q16)                // LBG bandwidth during open loop, per unit
    #define LBG_KGAIN_INC_PCT_F16                           QCONST(0.1, Q16)                // LBG bandwidth ramping rate, per unit

    // Enable LBG angle calculation with PLL, otherwise Arctan method would be used
    #define ENABLE_PLL_ANGLE_CALCULATION
    #ifdef ENABLE_PLL_ANGLE_CALCULATION
        #define LBG_PLL_DAMPING_FACTOR_Q16                  QCONST(0.707, Q16)
        #define LBG_PLL_KGAIN_START_PCT_F16                 QCONST(0.75, Q16)                // LBG PLL bandwidth during open loop, per unit
        #define LBG_PLL_KGAIN_INC_PCT_F16                   QCONST(0.25, Q16)                // LBG PLL bandwidth ramping rate, per unit
    #endif

    // Check that the estimated speed and angle do not differ by a certain error threshold from OL speed and forced angle.  If check fails, a warning is displayed.
    #define ANGLEDIFF_SPEED_ERROR_PCT_Q16               QCONST(0.10, Q16)
    #define ANGLEDIFF_ANGLE_ERROR_DEG_Q12               QCONST((3.14/16), Q12)

    // This is not needed from Aligngo to Trap, but may be useful for current increase during stall
    //#define ENABLE_ANGLEDIFF_AUTORANGE
    #ifdef ENABLE_ANGLEDIFF_AUTORANGE
        #define ANGLEDIFF_AUTORANGE_METHOD_SELECT           ANGLEDIFF_AUTORANGE_METHOD_1
//        #define ANGLEDIFF_AUTORANGE_METHOD_SELECT           ANGLEDIFF_AUTORANGE_METHOD_2    // Method 2 is only for testing purposes only
        #if (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_1)
            //#define ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS                        // Run Idref ramping dt to 2ms (original 10ms) for improving fast startup performance
            #ifdef ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS
                #define ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT     0.01f           // AngleDiff autorange method1 increments every systick as % of Itot
            #else
                #define ANGLEDIFF_AUTORANGE_INC_METHOD1_PCT     0.05f           // AngleDiff autorange method1 increments every systick as % of Itot
            #endif
            #define IDREF_RAMP_RATE_SHIFT_Q00       0                           // Idref ramp rate attenuation during switch over
            //#define ENABLE_IDREF_BIDIRECTION                                  // Allow IDREF set as positive and negative value
        #elif (ANGLEDIFF_AUTORANGE_METHOD_SELECT == ANGLEDIFF_AUTORANGE_METHOD_2)
            #define ALIGNGO2EST_ANGLERAMP_INCREMENT_DEGPS_Q06       QCONST(2000.0f, Q06)
        #endif
    #endif

    // Enables timeout for estimated speed and angle check.  If timeout is exceeded, the motor is disabled.
    //#define ENABLE_ANGLEDIFF_TIMEOUT
    #ifdef ENABLE_ANGLEDIFF_TIMEOUT
        #define ANGLEDIFF_TIMEOUT_SEC                       2               // AngleDiff Timeout (sec)
    #endif

    /* Disabled as default! Debug and test use only */
    // Blending speed or angle during switch over
    //#define ENABLE_SWITCH_OVER_SPEED_BLEND
    //#define ENABLE_SWITCH_OVER_ANGLE_BLEND
    #ifdef ENABLE_SWITCH_OVER_ANGLE_BLEND
        #define ALIGNGO2EST_ANGLEBLEND_TOTALCNT             100
        #define ALIGNGO2EST_ANGLEBLEND_FACTOR_Q12           QCONST((1.0 / (float)ALIGNGO2EST_ANGLEBLEND_TOTALCNT), Q12)
    #endif

    // Hysteresis band for switching from Estimator to Trap mode as a percentage of user defined switchover speed
    #define SWITCHOVER_SPD_TRAPEST_HYST_PCT_Q16             QCONST(0.15f, Q16)

//    #define CONFIG_TRAPFILTER_SHIFT                     2                   // Hall speed filter shift value
    #define CONFIG_TRAPFILTER_SHIFT                     1                   // Hall speed filter shift value
    #define ENABLE_TRAP_ANGLE_INTERPOLATION
    #define ENABLE_BEMF_SAMPLE_SINGLE_PHASE             // This MUST be defined since no sample/hold on AIO789
#endif

//====================================
// CONFIG: Current Sampling & Control
//====================================
// 1-Shunt Current Sampling Method
#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && defined(FAMILY_52XX)
//    #define MODE_1SHUNT_SELECT              MODE_1SHUNT_EXTENSION
    #define MODE_1SHUNT_SELECT              MODE_1SHUNT_ASYMMETRIC
#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && defined(FAMILY_55XX)
//    #define MODE_1SHUNT_SELECT              MODE_1SHUNT_EXTENSION
    #define MODE_1SHUNT_SELECT              MODE_1SHUNT_ASYMMETRIC
//    #define MODE_1SHUNT_SELECT              MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY
#endif

// Filter applied to measured Id/Iq
//#define ENABLE_IDIQ_FILTER

// Reconstruct high duty cycle phase current from the other 2 lower duty cycle phase currents. Enables system to reach 100% duty cycle.
//#define ENABLE_CURRENT_RECONSTRUCTION

// Cycle-By-Cycle Current Limit
//#define ENABLE_CBC_CURRENT_LIMIT
#ifdef ENABLE_CBC_CURRENT_LIMIT
    #define HPDAC_OC_LIMIT_MULTIPLIER                   2.0f
#endif


//====================================
// CONFIG: Modulation Method
//====================================
#define MODULATION_SELECT               MODULATION_SVM_STANDARD
//#define MODULATION_SELECT               MODULATION_SVM_OVERMOD
//#define MODULATION_SELECT               MODULATION_SVM_HARMONICINJ              // Experimental
//#define MODULATION_SELECT               MODULATION_GENMOD_SINE                  // Experimental
//#define MODULATION_SELECT               MODULATION_GENMOD_SVM                   // Experimental
//#define MODULATION_SELECT               MODULATION_GENMOD_DPWM                  // Experimental
//#define MODULATION_SELECT               MODULATION_GENMOD_DPWMMIN               // Experimental
//#define MODULATION_SELECT               MODULATION_HYBRID_TRAP_DMIN             // Experimental


//====================================
// CONFIG: Controllers
//====================================
// Outerloop control frequency divider
#define SPEED_PI_LOOP_DIVIDER                           1

// Outerloop Control Options
#define OUTERLOOP_CONTROL_SELECT                        OUTERLOOP_CONTROL_SPEED
// #define OUTERLOOP_CONTROL_SELECT                        OUTERLOOP_CONTROL_POWER

// Interleave controllers to reduce control loop time
//#define ENABLE_CONTROLLER_INTERLEAVE


//====================================
// CONFIG: Power Measurement
//====================================
//#define ENABLE_POWERMEASURE
#if defined(ENABLE_POWERMEASURE)
    // Power measurement method selection
    #define POWER_MEASURE_SELECT                        POWER_MEASURE_METHOD_1
//    #define POWER_MEASURE_SELECT                        POWER_MEASURE_METHOD_2

    // Power measurement options
//    #define ENABLE_LCORR_IBUS
#ifdef ENABLE_LCORR_IBUS
    #define LCORR_IBUS_M                                QCONST(1.0, Q16)        // Linear model slope for IbusAvg as a function of IbusAvg
    #define LCORR_IBUS_B                                QCONST(0.0, Q16)        // Linear model y-intercept for IbusAvg as a function of IbusAvg
#endif

//    #define ENABLE_LCORR_PWR
#ifdef ENABLE_LCORR_PWR
    #define LCORR_PWR_M                                 QCONST(1.0, Q16)        // Linear model slope for Pin as a function of Pout
    #define LCORR_PWR_B                                 QCONST(0.0, Q16)        // Linear model y-intercept for Pin as a function of Pout
#endif

//    #define ENABLE_LCORR_VBUS
#ifdef ENABLE_LCORR_VBUS
    //<=40W
    #define POWER_TARGET_0                             QCONST(40.0, Q16)
    #define LCORR_VBUS_M_0                             QCONST(0.00, Q16)        // Linear model slope for Pin as a function of Pout
    #define LCORR_VBUS_B_0                             QCONST(0.0, Q16)         // Linear model y-intercept for Pin as a function of Pout
    //<=60W
    #define POWER_TARGET_1                             QCONST(60.0, Q16)
    #define LCORR_VBUS_M_1                             QCONST(0.000, Q16)       // Linear model slope for Pin as a function of Pout
    #define LCORR_VBUS_B_1                             QCONST(0.0, Q16)         // Linear model y-intercept for Pin as a function of Pout
    //<=80W
    #define POWER_TARGET_2                             QCONST(80.0, Q16)
    #define LCORR_VBUS_M_2                             QCONST(0.0, Q16)         // Linear model slope for Pin as a function of Pout
    #define LCORR_VBUS_B_2                             QCONST(0.0, Q16)         // Linear model y-intercept for Pin as a function of Pout
    //<=100W
    #define POWER_TARGET_3                             QCONST(100.0, Q16)
    #define LCORR_VBUS_M_3                             QCONST(0.0, Q16)         // Linear model slope for Pin as a function of Pout
    #define LCORR_VBUS_B_3                             QCONST(0.0, Q16)         // Linear model y-intercept for Pin as a function of Pout
    //<=120W
    #define POWER_TARGET_4                             QCONST(120.0, Q16)
    #define LCORR_VBUS_M_4                             QCONST(0.0, Q16)         // Linear model slope for Pin as a function of Pout
    #define LCORR_VBUS_B_4                             QCONST(0.0, Q16)         // Linear model y-intercept for Pin as a function of Pout
    //<=250W
    #define POWER_TARGET_5                             QCONST(250.0, Q16)
    #define LCORR_VBUS_M_5                             QCONST(0.275, Q16)       // Linear model slope for Pin as a function of Pout
    #define LCORR_VBUS_B_5                             QCONST(-1.5, Q16)        // Linear model y-intercept for Pin as a function of Pout
#endif

//    #define ENABLE_LCORR_TEMP
#ifdef ENABLE_LCORR_TEMP
    #define LCORR_TEMP_M                               QCONST(0.0, Q16)         // Linear model slope for Ptemp as a function of Temp
    #define LCORR_TEMP_B                               QCONST(0.0, Q16)         // Linear model y-intercept for Ptemp as a function of Temp
#endif

    // Power measurement parameters
    #define POWER_FILTER_FREQ_CUTOFF_HZ_Q16             QCONST(100.0, Q16)      // Power filter cutoff frequency in Hertz
#endif


//====================================
// CONFIG: Duty Cycle Control
//====================================
//#define ENABLE_DUTY_LIMIT_BANDEXCLUDE
// NOTE: This condition assumes PRDL=1.  If PRDL=0 then LS needs to be changes instead of HS.
#ifndef ENABLE_DUTY_LIMIT_BANDEXCLUDE
    #define PULSEWIDTH_MIN_HS_NSEC                      (400 + MOTOR1_LED_NSEC)
    #define CONTROL_VDVQ_VECTOR_LIMIT                   // Limit magnitude of VdVq vector to maximum duty cycle allowed in order to reduce distortion of voltage vector
#else
    #define PULSEWIDTH_MIN_HS_NSEC                      (1200 + MOTOR1_LED_NSEC)
    #define CONTROL_VDVQ_VECTOR_LIMIT                   // Limit magnitude of VdVq vector to maximum duty cycle allowed in order to reduce distortion of voltage vector
#endif
#define PULSEWIDTH_MIN_LS_NSEC                          (400 + MOTOR1_TED_NSEC)

//#define ENABLE_BOOTPROT
#ifdef ENABLE_BOOTPROT
    #define BOOTPROT_DUTY_MAX_PCT_FLT                   0.97f
    #define BOOTPROT_TIME_LIMIT_SEC                     0.000250f       // Rounded to the lowest multiple of the control period (Resolution=1us)
    // NOTE: Time limit depends on device and bootstrap cap value.
    // NOTE: Duty cycle limit depends on Rgate, FET's and bootstrap cap values
#endif

// Zero duty and 100% duty support
//#define ENABLE_ZERO_DUTY_CYCLE_ENHANCEMENT

//========================================
// CONFIG: Clock Frequency
//========================================
#if defined(FAMILY_52XX)
    #define HCLK_FREQ_HZ                50000000
//    #define HCLK_FREQ_HZ                20000000
//    #define HCLK_FREQ_HZ                15000000
//    #define HCLK_FREQ_HZ                54824561

#define ADCEMUXCTRL_EMUXDIV ADCEMUXCTL_EMUXDIV_DIV2

#if (HCLK_FREQ_HZ == 50000000)
    #define UART_DIVISOR             27
    #define UART_FRACTIONAL_DIVISOR  32
    #define SOC_BRIDGE_CLK_DIVIDER    1
    #define FLASH_WAIT_STATE FLASH_WSTATE_25MHZ_LT_HCLK_LTE_50MHZ    
        #define LIN_DIVISOR                         HCLK_FREQ_HZ/(16*LIN_BR)
#elif (HCLK_FREQ_HZ == 54824561) //55928411)
    #define UART_DIVISOR             31
    #define UART_FRACTIONAL_DIVISOR  20
    #define SOC_BRIDGE_CLK_DIVIDER    1
    #define FLASH_WAIT_STATE FLASH_WSTATE_25MHZ_LT_HCLK_LTE_50MHZ
#elif (HCLK_FREQ_HZ == 20000000)
    #define UART_DIVISOR             10
    #define UART_FRACTIONAL_DIVISOR 217
    #define SOC_BRIDGE_CLK_DIVIDER    0
    #define FLASH_WAIT_STATE FLASH_WSTATE_HCLK_LTE_25MHZ
#elif (HCLK_FREQ_HZ == 15000000)
    #define UART_DIVISOR              8
    #define UART_FRACTIONAL_DIVISOR  35
    #define SOC_BRIDGE_CLK_DIVIDER    0
    #define FLASH_WAIT_STATE FLASH_WSTATE_HCLK_LTE_25MHZ
#else
    "Error, Please select correct HCLK_FREQ_HZ, thanks"
#endif

#elif defined(FAMILY_55XX)    
    #define HCLK_FREQ_HZ            150000000
#else
    #error No PAC family defined
#endif    

#if defined(FAMILY_52XX)
#define TIMERCLK_FREQ_HZ            HCLK_FREQ_HZ
#endif

#if defined(FAMILY_55XX)
#define TIMERCLK_FREQ_HZ            50000000
#define LIN_DIVISOR                                 HCLK_FREQ_HZ/(16*LIN_BR)
#endif

//========================================
// CONFIG: Systick Timer
//========================================
#define SYSTICK_HANDLER_FREQ_HZ_FLT 500.0                                                   // Freq in Hz that the SysTick Handler runs at
#define SYSTICK_HANDLER_DIVIDER     5                                                       // Certain operations in SysTick handler are done at a divided down frequency
#define DT_SYSTICK_FLT              (1.0/SYSTICK_HANDLER_FREQ_HZ_FLT)
#define DT_DIVSYSTICK_FLT           (DT_SYSTICK_FLT * SYSTICK_HANDLER_DIVIDER)

#define SYSTICK_TICKS               QCONST((HCLK_FREQ_HZ / SYSTICK_HANDLER_FREQ_HZ_FLT), Q00)
#define DT_SYSTICK_Q16              QCONST(DT_SYSTICK_FLT, Q16)                                     // dt(sec), q0 * f16 = f16
#define DT_DIVSYSTICK_Q16           QCONST(DT_DIVSYSTICK_FLT, Q16)      // dt(sec), q0 * f16 = f16
#define DT_DIVSYSTICK_Q26           QCONST(DT_DIVSYSTICK_FLT, Q26)




//==============================================
// DECLARATION DEPENDENCIES (Do Not Modify!!!)
//==============================================

// Define COMMSEL Options
#if (COMM_SELECT == COMM_UART) || (COMM_SELECT == COMM_COMBO_PPM_UART)    \
    || (COMM_SELECT == COMM_COMBO_PUSH_BUTTON_UART)
#define COMMSEL_UART                   TRUE
#else
#define COMMSEL_UART                   FALSE
#endif

#if (COMM_SELECT == COMM_PPM) || (COMM_SELECT == COMM_COMBO_PPM_UART)
#define COMMSEL_PPM                    TRUE
#else
#define COMMSEL_PPM                    FALSE
#endif

#if (COMM_SELECT == COMM_PUSH_BUTTON) || (COMM_SELECT == COMM_COMBO_PUSH_BUTTON_UART)
#define COMMSEL_PUSH_BUTTON            TRUE
#else
#define COMMSEL_PUSH_BUTTON            FALSE
#endif

#if (COMM_SELECT == COMM_CAN)
#define COMMSEL_CAN                    TRUE
#else
#define COMMSEL_CAN                    FALSE
#endif

#if (COMM_SELECT == COMM_LIN)
#define COMMSEL_LIN                    TRUE
#else
#define COMMSEL_LIN                    FALSE
#endif

#if (COMM_SELECT == COMM_CAN) && !defined(FAMILY_55XX)
    #error "COMMSEL_CAN is only supported in FAMILY_55XX."
#endif

#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY) && !defined(FAMILY_55XX)
    #error "MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY is currently supported in FAMILY_55XX only."
#endif

#if !defined(MODULATION_SELECT)
   #error "MODULATION_SELECT is not defined"
#endif

#if (ADC_BITS_HARDWARE_CAPABILITY < ADC_BITS_FIRMWARE_SELECTION) || (ADC_BITS_FIRMWARE_SELECTION < 10)
    #error: "ADC_BITS_FIRMWARE_SELECTION" must not be large than "ADC_BITS_HARDWARE_CAPABILITY" or "ADC_BITS_FIRMWARE_SELECTION" must not be less than 10.
#endif

#if defined(ENABLE_ZERO_DUTY_CYCLE_ENHANCEMENT) && defined(ENABLE_DUTY_LIMIT_BANDEXCLUDE) && !((DEVICE_SELECT == DEVICE_5523A) || (DEVICE_SELECT == DEVICE_5532B) || (DEVICE_SELECT == DEVICE_5556A))
    #error "ENABLE_ZERO_DUTY_CYCLE_ENHANCEMENT" is only supported with PAC5523A, PAC5532B and PAC5556A.
#endif

#if ((MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN) && (MPOS_SELECT != MPOS_LBG1_START_TRAP_ALIGNGO)) \
    || ((MODULATION_SELECT != MODULATION_HYBRID_TRAP_DMIN) && (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO))
    #error "MODULATION_HYBRID_TRAP_DMIN" is only supported with "MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO".
#endif

#endif  //#ifndef CONFIG_APP_H
