
/**
 * @file config_features.h
 * @brief Use for different FOC firmware features selection
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
 
#ifndef CONFIG_FEATURES_H
#define CONFIG_FEATURES_H

#include "pac5xxx.h"
#include "maths.h"
#include "config_app.h"

//=============================================================================
//
// Config Features
//
//=============================================================================

//====================================
// CONFIG: Startup
//====================================
//#define ENABLE_POWERUP_AUTOSTART
//#define ENABLE_IPD
//#define ENABLE_COASTING                                                     // Enables coasting state before motor is disabled. FOC remains engaged with zero torque until motor speed drops below switchover speed. If timeout expires, the motor is disabled.
//#define ENABLE_NEGATIVE_IQ_COASTING                                         // Enables negative IQ during coasting state.
//#define ENABLE_BRAKE_WHILE_STOPPED


//=====================================
// SIM (Start-In-Motion)
//=====================================
//#define ENABLE_SIM
#if defined(ENABLE_SIM)

    // --------------------------------------------
    // SIM Method Selection
    // --------------------------------------------
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    #define SEL_SIM_METHOD                              SIM_METHOD_VOLTAGE_BEMF     // Select V-based SIM mode using measured BEMF to calculate the SIM2CL voltage vector
//    #define SEL_SIM_METHOD                              SIM_METHOD_VOLTAGE_KE       // Select V-based SIM mode using motor Ke parameter to calculate the SIM2CL voltage vector
//    #define SEL_SIM_METHOD                              SIM_METHOD_CURRENT          // Select I-based SIM mode (using motor Ke parameter to calculate the SIM2CL voltage vector)
#elif (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_HALL_ONLY)
    #define SEL_SIM_METHOD                              SIM_METHOD_HALL             // Select V-based SIM mode using measured BEMF to calculate the SIM2CL voltage vector
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP)
    #define SEL_SIM_METHOD                              SIM_METHOD_QEP             // Select V-based SIM mode using measured BEMF to calculate the SIM2CL voltage vector
#endif

    // --------------------------------------------
    // SIM Operation Modes
    // --------------------------------------------
//    #define ENABLE_SIMBRAKE_MOTORDISABLE                                    // SIM will activate braking every time that the motor is disabled. If this feature is not enabled, motor will not brake, just free spin to a stop.
//    #define ENABLE_SIMBRAKE_REVDIRECTION                                    // SIM will activate braking every time that reverse direction is detected when starting the motor. If this feature is not enabled, motor will not brake, just free spin and wait until motor stops before it starts.
//    #define ENABLE_SIMBRAKE_SLOWSPEED                                       // SIM will activate braking every time that slow speed is detected when starting the motor. If this feature is not enabled, motor will not brake, just free spin and wait until motor stops before it starts.

    // --------------------------------------------
    // SIM Configuration
    // --------------------------------------------
#if ((MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO))
    #define ENABLE_SIMANGLEBLEND                                            // When SIM2CL is called, the SIM angle is gradually blended with the estimated angle during a transition period for a smoother transition.
#endif

    // --------------------------------------------
    // SIM Parameters
    // --------------------------------------------
    #define SIM_CONTROL_DIVIDER                         2                   // Divider for control loop when SIM is running
    #define SIM_ANGLE_BLEND_MAX_CNT                     8                   // Number of cycles to blend angle during SIM2CL transition
    #define SIM_BRAKING_DUTYRAMPTIME_SEC_Q8             QCONST(0.5f, Q08)   // Braking duty cycle time to ramp to maximum braking force (min/max = 0.1/5 sec)
    #define SIM_IQREF_MIN                               0.01f               // Minimum iq reference allowed during SIM2CL to prevent negative torque
    #define SIM_SIM2CL_MAXSPD_PCT_Q16                   QCONST(0.9, Q16)    // Maximum speed as percentage of maximum CL speed that SIM2CL is allowed to trigger.  If speed is higher than this limit SIM will wait until speed is below this threshold before triggering SIM2CL.

#if ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_CURRENT))
    #define SIM_MDETECT_DETECTTIME_SEC_Q16              QCONST(0.05f, Q16)  // Length of time during which motion will be detected
    #define SIM_DETECT2_SPEEDFILTER_FC_HZ_Q16           QCONST(20.0f, Q16)  // Cutoff frequency for speed filter during SIM2CL transition. Lower fc will make the detection time longer.
#elif (SEL_SIM_METHOD == SIM_METHOD_HALL)
    #define SIM_MDETECT_HALLEDGENUM                     4                   // Number of edges to detect before declaring motion is detected. Motion not detected when hall timer expires. Hall timer is configured in config_app.h.
#elif (SEL_SIM_METHOD == SIM_METHOD_QEP)
    #define SIM_MDETECT_DETECTTIME_SEC_Q16              QCONST(0.05f, Q16)  // Length of time during which motion will be detected
    #define SIM_MDETECT_QEPCOUNTER_THRESHOLD_CNT        50                  // Number of QEP counts to detect motion during MDETECT time period
    #define SIM_MDETECT_SPEED_THRESHOLD_HZ_Q0           10                  // Speed threshold for motion detection
#endif
#if (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
    #define SIM_MDETECT_DUTYRAMPTIME_SEC_Q8             QCONST(0.15f, Q08)  // Time to ramp motion detection duty cycle to final value before declaring no motion.
    #define SIM_MDETECT_SAMPLING_ADVANCE_TICKS          50                  // Define the advance ticks before Low side pulse turning off
#endif

    // --------------------------------------------
    // SIM Option for generator mode
    // --------------------------------------------
    //#define ENABLE_SIM_GEN_MODE
#endif

//====================================
// CONFIG: Field Weakening
//====================================
//#define ENABLE_FIELD_WEAKENING
#ifdef ENABLE_FIELD_WEAKENING
    #define FIELD_WEAKENING_METHOD                          INCREMENTRAL_CONTROL
    //#define FIELD_WEAKENING_METHOD                          PID_CONTROL
    #if (FIELD_WEAKENING_METHOD == INCREMENTRAL_CONTROL)
        // Field weakening Idref configuration for variable increments
        #define OUTERLOOP_ERR_THRESHOLD                     QCONST(0.001f, Q14)
        #define ID_INCREMENT_BIG                            QCONST(0.001f, Q16)
        #define ID_INCREMENT_SMALL                          QCONST(0.0002f, Q16)
    #elif (FIELD_WEAKENING_METHOD == PID_CONTROL)
    // Voltage magnitude reference during Field weakening: per unit. "1.0f" means the max output voltage without enabling FW
        //#define VMAG_REF_FW_Q14                             (QCONST(0.85f, Q14))
        #define VRMS_FILTER_CUTOFF_FREQ_Q16                 QCONST(50.0f, Q16)        // Unit: Hz, this is for debug purpose only
    #endif
#endif


//====================================
// Config: MTPA
//====================================
//#define ENABLE_MTPA
#if defined(ENABLE_MTPA)
    // MTPA method selection
    #define I_FILTER_CUTOFF_FREQ_Q16                    QCONST(30.0f, Q16)
    #define MTPA_RAMP_RATE_Q16                          QCONST(0.002f, Q16)
    #define MTPA_ANGLE_AUTO_CALC
#endif


//==========================================
// CONFIG: DQ-Decoupling
//==========================================
//#define ENABLE_DQ_DECOUPLING
#if defined(ENABLE_DQ_DECOUPLING)
    #define SEL_DQ_DECOUPLING_METHOD    FEEDFORWARD_DECOUPLING
    //#define SEL_DQ_DECOUPLING_METHOD    FEEDBACK_DECOUPLING
    //#if (SEL_DQ_DECOUPLING_METHOD == FEEDBACK_DECOUPLING)
        #define CURRENT_FILTER_CUTOFF_FREQ_Q16    QCONST(200.0f, Q16)        // Unit: Hz
    //#endif
#endif


//==========================================
// CONFIG: DC Voltage Ripple Compensation
//==========================================
//#define ENABLE_DC_VOLTAGE_COMPENSATION
#if defined(ENABLE_DC_VOLTAGE_COMPENSATION)
    #define DC_VOLTAGE_COMPENSATION_LIMIT_Q26               QCONST(2.0f, Q26)
    #define DC_COMPENSATION_CUTOFF_FREQ_Q16                 QCONST(10.0f, Q16)        // Unit: Hz
    //#define ENABLE_DC_COMPENSATION_OPEN_LOOP                                        // Will increase additional CPU load if enabled
#endif


//====================================
// CONFIG: Initialization
//====================================
// Enable Flash CRC Test
//#define ENABLE_FLASH_CRC_TEST

// Enable PAC5XXX device INFOROM check
#define ENABLE_DEVICE_ID_CHECK

// Enable ADC Offset and gain compensation for PAC55XX
#define ENABLE_ADCCAL_FACTORY

// Enable ADC Powerup Calibration
#define ENABLE_ADCCAL_POWERUP


//====================================
// CONFIG: Faults/Checks
//====================================

// Stall Detection
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    //#define ENABLE_STALL_DETECT
    //#define ENABLE_STALL_DETECT_NEGSPD
    #if defined(ENABLE_STALL_DETECT)
    //#define ENABLE_STALL_DETECT_STEADY_STATE
    #endif
#endif

// OPD
//#define ENABLE_OPD_STARTUP
//#define ENABLE_OPD_CL
#if defined(ENABLE_OPD_CL)
    #define OPDCL_MINSPEED_RADPS_Q16                    QCONST((1.0f * FLT_2PI), Q16)
    //#define ENABLE_OPD_CL_FAST
#endif

// Over-Current
//#define SEL_OC_METHOD                       OC_METHOD_HPDAC     // Define OC proctection method
//#define ENABLE_CBC_CURRENT_LIMIT
#if defined(ENABLE_CBC_CURRENT_LIMIT)
    #define HPDAC_OC_LIMIT_MULTIPLIER                       2.0f
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    #define HPDAC_OC_LIMIT_MULTIPLIER                       2.0f
#endif

// Bus Voltage Check
#define ENABLE_DCBUS_MIN_MAX_CHECK                              // Check bus voltage against min/max. If check fails, the motor is disabled.
#ifdef ENABLE_DCBUS_MIN_MAX_CHECK
    #define MIN_VBUS_FAULT_THRESHOLD_COUNT              10
    #define MAX_VBUS_FAULT_THRESHOLD_COUNT              3
#endif

// Motor Speed Check
#define ENABLE_CLSPEED_MIN_MAX_CHECK                                // Check motor speed against min/max. If check fails, the motor is disabled.
#ifdef ENABLE_CLSPEED_MIN_MAX_CHECK
    #define SPEED_MIN_MAX_FAULT_CHECK_SAMPLE_TIME       1
    #define MOTOR_SPEED_ERRORP_CHECK_THRESHOLD          10
    #define OL_SPEED_CHECK_TIME_CNTS                    20          // Systick counts
#endif

// Watchdog
//#define ENABLE_WATCHDOG                                 TRUE        // Enable watchdog timer to generate POR when WDT not kicked


//====================================
// Tuning Tools
//====================================
//------------------------------
// Motor ID & Controller Tuning
//------------------------------
#define ENABLE_AUTOTUNE_MID
#define ENABLE_AUTOTUNE_PI
#if defined(ENABLE_AUTOTUNE_MID)
    #define CONFIG_MID1_3STEP_MODE
//    #define CONFIG_MID1_EQVABC
#endif


//------------------------------
// Hall Tuning
//------------------------------
#define ENABLE_HALL_TUNING


//------------------------------
// QEP Tuning
//------------------------------
//#define ENABLE_QEP_OFFSET_TUNING

//#define ENABLE_QEP_LINEARIZATION_TUNING
#ifdef ENABLE_QEP_LINEARIZATION_TUNING
    // Resolution per phase
    #define QEP_PULSE_PER_RESOLUTION                    256
    // Get QEP linearization for quadrature mode during normal operation
    #define ENABLE_QEP_QUAD_MODE
    // QEP speed calculation divider for better accuracy at lower speed
    #define QEP_SPEED_CALCULATION_DIVIDER                   1                   // Every ("QEP_SPEED_CALCULATION_DIVIDER" * dT) calcuates speed once
#endif


//====================================
// Flash R/W Tools
//====================================
//------------------------------
// Flash R/W : Demo Mode
//------------------------------
//#define ENABLE_FLASHDEMO

#if defined(ENABLE_FLASHDEMO)
//#define FLASHDEMO_FLASHRW_MODESEL     FLASHRW_MODELSEL_FXADDR    /* Use fixed flash address writing scheme */ 
#define FLASHDEMO_FLASHRW_MODESEL     FLASHRW_MODELSEL_DYADDR    /* Use dynamic flash address writing scheme */ 
#endif
#if defined(ENABLE_FLASHDEMO) && defined(FAMILY_55XX)
//    #define FLASHDEMO_FLASH_WORD_ARRAY_SIZE   (1024)    /* If need more room in array increase size of this define */
//    #define FLASHDEMO_FLASH_START_ADDR        (0x1D000)
//    #define FLASHDEMO_FLASH_END_ADDR          (0x1E000)
    #define FLASHDEMO_FLASH_WORD_ARRAY_SIZE   (14)    /* If need more room in array increase size of this define */
    #if ((FLASHDEMO_FLASH_WORD_ARRAY_SIZE + 2) % 4 != 0)
    #error: (FLASHDEMO_FLASH_WORD_ARRAY_SIZE + 2) must be multiple of 4.
    #endif
    #define FLASHDEMO_FLASH_START_ADDR        (0x7800)
    #define FLASHDEMO_FLASH_END_ADDR          (0x7BFF)
#elif defined(ENABLE_FLASHDEMO) && defined(FAMILY_52XX)
    #define FLASHDEMO_FLASH_WORD_ARRAY_SIZE   (100)    /* If need more room in array increase size of this define */
    #define FLASHDEMO_FLASH_START_ADDR        (0x7800)
    #define FLASHDEMO_FLASH_END_ADDR          (0x7BFF)
#endif


//------------------------------
// Flash R/W : QEP Linearity
//------------------------------
//#define ENABLE_QEP_LIN_FLASHRW
#if defined(ENABLE_QEP_LIN_FLASHRW) && defined(FAMILY_55XX)
    #define QEP_LIN_FLASH_WORD_ARRAY_SIZE   (1024)    /* If need more room in array increase size of this define */
    #define QEP_LIN_FLASH_START_ADDR        (0x1E000)
    #define QEP_LIN_FLASH_END_ADDR          (0x1F000)
    #define QEP_LIN_FLASHRW_MODESEL       FLASHRW_MODELSEL_FXADDR
#endif

// Read QEP linearization data from flash during powerup
//#define ENABLE_QEP_LIN_FLASH_READ_POWERUP


//------------------------------
// Flash R/W : Dependencies
//------------------------------
#if defined(ENABLE_FLASHDEMO) || defined(ENABLE_QEP_LIN_FLASHRW)
    #define ENABLE_FLASH_RW
#endif


//====================================
// Other Tools
//====================================
//#define ENABLE_FORCEANG
//#define ENABLE_BUZZ                                                 // Enable Buzz feature


//=============================================================================
//
// Dependencies - DO NOT MODIFY
//
//=============================================================================
#define TIMER_DT_SEC_FLT             (1.0 / TIMERCLK_FREQ_HZ)

// Dep: OPD Start
#ifdef ENABLE_OPD_STARTUP
#ifdef ENABLE_IPD
    #define ENABLE_IPD_OPD
#else
    #define ENABLE_OPDOL    // When ENABLE_IPD there is no need to run Open Phase Detection in Open Loop because IPD is checking Open Phase Detection
#endif
#endif

// Dep: Duty Cycle Min/Max
#define PULSEWIDTH_MIN_HS_TICKS                         QCONST(((float)PULSEWIDTH_MIN_HS_NSEC / 1000000000 / (TIMER_DT_SEC_FLT * 2)), Q00)
#define PULSEWIDTH_MIN_LS_TICKS                         QCONST(((float)PULSEWIDTH_MIN_LS_NSEC / 1000000000 / (TIMER_DT_SEC_FLT * 2)), Q00)

// Dep: Deadtime
#define MOTOR1_LED_TICKS                                QCONST(((float)MOTOR1_LED_NSEC / 1000000000 / TIMER_DT_SEC_FLT), Q00)
#define MOTOR1_TED_TICKS                                QCONST(((float)MOTOR1_TED_NSEC / 1000000000 / TIMER_DT_SEC_FLT), Q00)

// Dep: Bootprot
#ifdef ENABLE_BOOTPROT
    #define BOOTPROT_TIME_LIMIT_SEC_x1024               QCONST(((BOOTPROT_TIME_LIMIT_SEC + (0.000001/2)) * 1024), Q16)
    #define BOOTPROT_DUTY_MAX_PCT_Q16                   QCONST(BOOTPROT_DUTY_MAX_PCT_FLT, Q16)
#endif

#define ADC_SCALE_CONST                                 QCONST((16383.0f / ((1 << ADC_BITS_FIRMWARE_SELECTION) - 1.0f)), Q14)                                                          // ((2^14)-1) / ((2^10)-1)
#define Q16_mV_PER_ADC_COUNT                            QCONST(2500.0f/((1 << ADC_BITS_FIRMWARE_SELECTION) - 1.0f), Q16)                                                                    // mV per ADC Count in Q16 or fix16 = 2.4437 mV
#define Q16_mV_PER_ADC_12_BIT_COUNT                     QCONST(2500.0/4095.0, Q16)                                                                                                     // mV per ADC Count in Q16 or fix16 = 0.61758 mV

#define COMP_DIFFGAINERROR_Q14                          QCONST(((13500.0f + AIORES_VALUE) / 13500.0f), Q14)

// Configuration of Iq during coasting
#if defined(ENABLE_NEGATIVE_IQ_COASTING) && defined(ENABLE_COASTING)
#define NEGATIVE_IQ_PERCENTAGE_FOR_COASTING         25.0f     // In percentage, this is the percentage of max iq current, 1% to 75%
#define IDIQ_RAMP_RATE_PERCENTAGE_FOR_COASTING      10.0f     // In percentage, this is the percentage of idiq_ramp_rate, 1% to 25%
#define NEGATIVE_IQ_CURRENT_PER_FOR_COAST           QCONST((NEGATIVE_IQ_PERCENTAGE_FOR_COASTING/100.0f), Q16)
#define IDIQ_RAMP_RATE_PER_FOR_COAST                QCONST((IDIQ_RAMP_RATE_PERCENTAGE_FOR_COASTING/100.0f), Q16)
#define DEFAULT_IDIQ_RAMP_RATE_PER_FOR_COAST        0x0001
#endif

// Anglediff
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO) && defined(ENABLE_ANGLEDIFF_TIMEOUT)
    #define ANGLEDIFF_TIMEOUT_COUNT                     (ANGLEDIFF_TIMEOUT_SEC / (DT_SYSTICK_FLT * SYSTICK_HANDLER_DIVIDER))
#endif

#if (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE) || (SEL_SIM_METHOD == SIM_METHOD_CURRENT) || (SEL_SIM_METHOD == SIM_METHOD_HALL)
    #define CONFIG_SIM_VOLTAGE_KE                                           // . If disabled, SIM will use the measured voltage vector instead.
#endif

#if defined(ENABLE_AUTOTUNE_MID) && defined(ENABLE_DQ_DECOUPLING)
    #undef ENABLE_DQ_DECOUPLING
#endif


//=============================================================================
//
// Compile Warnings/Errors - DO NOT MODIFY
//
//=============================================================================
#ifdef ENABLE_STALL_DETECT
    #ifndef ENABLE_CBC_CURRENT_LIMIT
//        #warning: Stall Detection may be preempted by an OC if CBC current limit is not enabled.
    #endif
    #ifdef ENABLE_CLSPEED_MIN_MAX_CHECK
//        #warning: Stall Detection may be preempted by CL speed min/max check.
    #endif
#else
    #ifdef ENABLE_CBC_CURRENT_LIMIT
        #error: CBC current limit cannot be enabled when stall detection is disabled.
    #endif
    #ifndef ENABLE_CLSPEED_MIN_MAX_CHECK
        #warning: Both CL speed min/max check and Stall Detection are disabled.
    #endif
#endif

#if defined(ENABLE_SIM) && ((SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_BEMF) || (SEL_SIM_METHOD == SIM_METHOD_VOLTAGE_KE)) && (CAPABLE_VSIM == FALSE)
#error: SIM is enabled but system is not SIM capable.
#endif

#if defined(ENABLE_IPD) && (CAPABLE_IPD == FALSE)
#error: IPD is enabled but system is not IPD capable.
#endif

#if (MIN_VBUS < 0x60000)
#error: MIN_VBUS cannot be less than 6V
#endif

#if (OUTERLOOP_CONTROL_SELECT == OUTERLOOP_CONTROL_POWER) && !defined(ENABLE_POWERMEASURE)
    #error: OUTERLOOP_CONTROL_POWER cannot be selected without ENABLE_POWERMEASURE.
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && defined(ENABLE_DUTY_LIMIT_BANDEXCLUDE)
    // It is not possible to hit 100% duty cycle in single shunt configuration
    #error: ENABLE_DUTY_LIMIT_BANDEXCLUDE cannot be defined when CONFIG_SHUNT = CONFIG_SHUNT_1SHUNT
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && defined(ENABLE_COASTING)
    // Coasting does not perform well in single shunt because of the small duty cycle gaps when iqref=0
    #error: ENABLE_COASTING cannot be defined when CONFIG_SHUNT = CONFIG_SHUNT_1SHUNT
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
    // iSIM has not been developed for single shunt operation
    #error: SIM_METHOD_CURRENT cannot be selected when CONFIG_SHUNT = CONFIG_SHUNT_1SHUNT
#endif

#if defined(ENABLE_IPD)
    // IPD functionality is not verified for single and two shunts
//    #error: ENABLE_IPD is not supported in this project, ipd is broken in adc_isr and ipd.c.
#endif

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && (CAPABLE_POWERMETHOD2 != TRUE)
    #error: POWER_MEASURE_METHOD_2 cannot be selected when PCB is not CAPABLE_POWERMETHOD2.
#endif

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && (CAPABLE_TEMPERATURE != TRUE) && defined(ENABLE_LCORR_TEMP)
    #error: POWER_MEASURE_METHOD_2 cannot be selected when PCB is not CAPABLE_TEMPERATURE.
#endif

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
    #ifdef ENABLE_DUTY_LIMIT_BANDEXCLUDE
        // 100% duty cycle is not currently possible when running in over-modulation
        #error: ENABLE_DUTY_LIMIT_BANDEXCLUDE cannot be enabled when MODULATION_SELECT = MODULATION_SVM_OVERMOD.
    #endif
#endif

#if defined(ENABLE_AUTOTUNE_PI) && (OUTERLOOP_CONTROL_SELECT != OUTERLOOP_CONTROL_SPEED)
    #error: ENABLE_AUTOTUNE_PI is only available when OUTERLOOP_CONTROL_SPEED mode is selected.
#endif

#if defined ENABLE_IDIQ_FILTER
    #warning: ENABLE_IDIQ_FILTER is defined.
#endif

#if defined(ENABLE_CURRENT_RECONSTRUCTION) && defined(ENABLE_AUTOTUNE_MID)
    #warning: ENABLE_CURRENT_RECONSTRUCTION cannot be enabled when ENABLE_AUTOTUNE_MID is enabled. Automatically disabling ENABLE_CURRENT_RECONSTRUCTION.
    #undef ENABLE_CURRENT_RECONSTRUCTION
#endif

#if defined(ENABLE_AUTOTUNE_MID) && !defined(ENABLE_AUTOTUNE_PI)
    #error: ENABLE_AUTOTUNE_MID cannot be enabled when ENABLE_AUTOTUNE_PI is not enabled.
#endif

#if defined(ENABLE_NEGATIVE_IQ_COASTING) && !defined(ENABLE_COASTING)
    // negative Iq use for regenerative braking can only be enabled when coasting is enabled
    #error: ENABLE_NEGATIVE_IQ_COASTING cannot be enabled without enabling ENABLE_COASTING
#endif

#if defined(ENABLE_STALL_DETECT) && ((MPOS_SELECT != MPOS_LBG1_START_ALIGNGO) && (MPOS_SELECT != MPOS_LBG1_START_TRAP_ALIGNGO))
//    #error: ENABLE_STALL_DETECT is only supported when MPOS_SELECT == MPOS_LBG1_START_ALIGNGO.
#undef ENABLE_STALL_DETECT
#endif

#if defined(ENABLE_CBC_CURRENT_LIMIT)
    #error: ENABLE_CBC_CURRENT_LIMIT, CBC is not working properly.
#endif

#if defined(ENABLE_AUTOTUNE_MID) && (MODULATION_SELECT != MODULATION_SVM_STANDARD)
    #error: ENABLE_AUTOTUNE_MID can only be enabled when MODULATION_SELECT = MODULATION_SVM_STANDARD.
#endif

#if defined(ENABLE_AUTOTUNE_MID) && defined(ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP)
    #error: ENABLE_AUTOTUNE_MID can only be enabled when ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP is disabled.
#endif

#if defined(ENABLE_AUTOTUNE_MID) && (CONFIG_SHUNT != CONFIG_SHUNT_3SHUNT)
    #error: ENABLE_AUTOTUNE_MID can only be enabled with 3SHUNT based boards.
#endif

#if defined(ENABLE_AUTOTUNE_MID) && defined(FAMILY_52XX)
    #warning: ENABLE_AUTOTUNE_MID is strongly recommended to disable after completing Motor Identification, especially for PAC52xx because of its limited flash memory.
#endif

#if (SEL_OC_METHOD == OC_METHOD_HPDAC) \
    && !((DEVICE_SELECT == DEVICE_5523) || (DEVICE_SELECT == DEVICE_5523A) || (DEVICE_SELECT == DEVICE_5527) || (DEVICE_SELECT == DEVICE_5556) \
        || (DEVICE_SELECT == DEVICE_5256))
    #error: OC_METHOD_HPDAC can only be enabled when PAC5523, PAC5523A, PAC5527, PAC5556 or PAC5256 is selected.
#endif

#if (defined(ENABLE_QEP_LIN_FLASH_READ_POWERUP) && !defined(ENABLE_QEP_LIN_FLASHRW))
    #error "ENABLE_QEP_LIN_FLASHRW" must be defined if using "ENABLE_QEP_LIN_FLASH_READ_POWERUP".
#endif

#if defined(ENABLE_QEP_LIN_FLASH_READ_POWERUP) && (MPOS_SELECT != MPOS_QEP_START_ALIGNGO)
    #error "MPOS_QEP_START_ALIGNGO" must be selected if using "ENABLE_QEP_LIN_FLASH_READ_POWERUP".
#endif

#if defined(ENABLE_QEP_LIN_FLASHRW) && (MPOS_SELECT != MPOS_QEP_START_ALIGNGO && !defined(ENABLE_QEP_LINEARIZATION_TUNING))
    #error Either "MPOS_QEP_START_ALIGNGO" or "ENABLE_QEP_LINEARIZATION_TUNING" must be selected if using "ENABLE_QEP_LIN_FLASHRW".
#endif

#if defined(ENABLE_FLASHDEMO) && defined(ENABLE_QEP_LIN_FLASHRW)
    #error "ENABLE_FLASHDEMO" and "ENABLE_QEP_LIN_FLASHRW" cannot be enabled simultaneously.
#endif

#if defined(ENABLE_QEP_LINEARIZATION_TUNING) && (MPOS_SELECT != MPOS_LBG1_START_ALIGNGO)
    #error "MPOS_LBG1_START_ALIGNGO" must be selected if using "ENABLE_QEP_LINEARIZATION_TUNING".
#endif

#if defined(ENABLE_BRAKE_WHILE_STOPPED) && (!defined(ENABLE_SIM) || !defined(ENABLE_SIMBRAKE_MOTORDISABLE))
    #error "ENABLE_SIM" & "ENABLE_SIMBRAKE_MOTORDISABLE" must be enabled when "ENABLE_BRAKE_WHILE_STOPPED" is enabled.
#endif

#if defined(ENABLE_BRAKE_WHILE_STOPPED) && !defined(FAMILY_55XX)
    #error "ENABLE_BRAKE_WHILE_STOPPED" is supported in PAC55XX only.
#endif

#if defined(ENABLE_WATCHDOG) && !defined(FAMILY_52XX)
    #error "ENABLE_WATCHDOG" is not implemented in PAC55XX, but please refer to the example codes for Class B, where WDT is explained.
#endif

#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION) && !(DEVICE_SELECT == DEVICE_5532)
    #error "ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION" is ONLY implemented in PAC5532.
#endif

#if defined(ENABLE_STALL_DETECT_STEADY_STATE)
    #warning "ENABLE_STALL_DETECT_STEADY_STATE" is supported in Speed Mode ONLY.
#endif

#endif  // #ifndef CONFIG_FEATURES_H

