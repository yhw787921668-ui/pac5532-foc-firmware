
/**
 * @file mpos_struct.h
 * @brief Use for mpos enums and structure
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

#ifndef MPOS_STRUCT_H
#define MPOS_STRUCT_H

#include "config_app.h"
#include "config_features.h"

///////////////////////////////////////////////////
// MPOS Definitions
///////////////////////////////////////////////////

#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    #define MODULE_MPOS_ALIGNGO                 ENABLED
    #define MODULE_MPOS_LBG1                    ENABLED
    #define MODULE_MPOS_HALL                    DISABLED
    #ifdef ENABLE_QEP_LINEARIZATION_TUNING
        #define MODULE_MPOS_QEP                 ENABLED
    #else
        #define MODULE_MPOS_QEP                 DISABLED
    #endif
#elif (MPOS_SELECT == MPOS_LBG1_START_HALL)
    #ifdef ENABLE_HALL_TUNING
        #define MODULE_MPOS_ALIGNGO             ENABLED
    #else
        #define MODULE_MPOS_ALIGNGO             DISABLED
    #endif
    #define MODULE_MPOS_LBG1                    ENABLED
    #define MODULE_MPOS_HALL                    ENABLED
    #define MODULE_MPOS_QEP                     DISABLED

#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    #ifdef ENABLE_HALL_TUNING
        #define MODULE_MPOS_ALIGNGO             ENABLED
    #else
        #define MODULE_MPOS_ALIGNGO             DISABLED
    #endif
    #define MODULE_MPOS_LBG1                    DISABLED
    #define MODULE_MPOS_HALL                    ENABLED
    #define MODULE_MPOS_QEP                     DISABLED
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    #define MODULE_MPOS_ALIGNGO                 ENABLED
    #define MODULE_MPOS_LBG1                    DISABLED
    #define MODULE_MPOS_HALL                    DISABLED
    #define MODULE_MPOS_QEP                     ENABLED
#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)
    #define MODULE_MPOS_ALIGNGO                 ENABLED
    #define MODULE_MPOS_LBG1                    ENABLED
    #define MODULE_MPOS_HALL                    DISABLED
    #define MODULE_MPOS_QEP                     ENABLED
#elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    #define MODULE_MPOS_ALIGNGO                 ENABLED
    #define MODULE_MPOS_LBG1                    ENABLED
    #define MODULE_MPOS_HALL                    DISABLED
    #define MODULE_MPOS_TRAP                    ENABLED
    #ifdef ENABLE_QEP_LINEARIZATION_TUNING
        #define MODULE_MPOS_QEP                 ENABLED
    #else
        #define MODULE_MPOS_QEP                 DISABLED
    #endif
#else
    #error "MPOS_SELECT has not been defined"
#endif

#if (MODULE_MPOS_ALIGNGO == DISABLED)       \
        && (defined(ENABLE_HALL_TUNING)     \
        || defined(ENABLE_FORCEANG)         \
        || defined(ENABLE_AUTOTUNE_MID)     \
        || defined(ENABLE_BUZZ))
    #undef MODULE_MPOS_ALIGNGO
    #define MODULE_MPOS_ALIGNGO                 ENABLED
#endif

#if (MODULE_MPOS_LBG1 == ENABLED)
    #include "lbg1_struct.h"
#ifdef ENABLE_PLL_ANGLE_CALCULATION
    #include "lbg1_pll_struct.h"
#endif
#endif

#if (MODULE_MPOS_ALIGNGO == ENABLED)
    #include "aligngo_struct.h"
#endif

#if (MODULE_MPOS_HALL == ENABLED)
    #include "hall_struct.h"
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    #include "qep_struct.h"
#endif

#if (MODULE_MPOS_TRAP == ENABLED)
    #include "trap_struct.h"
#endif

// JAGTAG: Move out to dedicated startup file
#if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    #include "startup_aligngo2est_struct.h"
#elif ((MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL))
    #include "startup_hall_struct.h"
#elif ((MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP))
    #include "startup_aligngo2qep_struct.h"
#elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    #include "startup_aligngo2trap_struct.h"
#endif


/**
 * @brief mpos enum
 */
typedef enum
{
    STATE_MPOS_IDLE = 0,                    // 0: MPOS Idle
    STATE_MPOS_ESTIMATED_ANGLE,             // 1: Estimator Mode
    STATE_MPOS_ALIGNGO,                     // 2: Align&Go Mode
    STATE_MPOS_TRANS_ALIGNGO2EST,           // 3: State to transition from Align&GO to Estimator mode
    STATE_MPOS_HALL_ANGLE_INTERPOLATE,      // 4: Hall Sensor Mode
    STATE_MPOS_HALL2EST,                    // 5: State to transition from Hall mode to Estimator mode
    STATE_MPOS_EST2HALL,                    // 6: State to transition from Estimator mode to Hall mode
    STATE_MPOS_TRANS_SIM2EST,               // 7: State to transition from SIM to Estimator mode
    STATE_MPOS_QEP,                         // 8: QEP Sensor Mode
    STATE_MPOS_QEP2EST,                     // 9: State to transition from QEP mode to Estimator mode
    STATE_MPOS_EST2QEP,                     // 10: State to transition from Estimator mode to QEP mode
    STATE_MPOS_TRAP,                        // 11: Trapezoidal sensor-less mode
    STATE_MPOS_TRAP2EST,                    // 12: State to transition from Trap mode to Estimator mode
    STATE_MPOS_EST2TRAP                     // 13: State to transition from Estimator mode to Trap mode
} eStateMPOS;


#if (MPOS_SELECT == MPOS_LBG1_START_HALL)

/**
 * @brief mpos enum
 */
typedef enum
{
    STATE_MPOSMODE_HALL = 0,                // 0:
    STATE_MPOSMODE_EST,                     // 1:
    STATE_MPOSMODE_HALL2EST,                // 2:
    STATE_MPOSMODE_EST2HALL                 // 3:
} eStateMPOSMode;

#elif (MPOS_SELECT == MPOS_LBG1_START_QEP)

/**
 * @brief mpos enum
 */
typedef enum
{
    STATE_MPOSMODE_QEP = 0,                 // 0:
    STATE_MPOSMODE_EST,                     // 1:
    STATE_MPOSMODE_QEP2EST,                 // 2:
    STATE_MPOSMODE_EST2QEP                  // 3:
} eStateMPOSMode;

#elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)

/**
 * @brief mpos enum
 */
typedef enum
{
    STATE_MPOSMODE_TRAP = 0,                 // 0:
    STATE_MPOSMODE_EST,                     // 1:
    STATE_MPOSMODE_TRAP2EST,                 // 2:
    STATE_MPOSMODE_EST2TRAP                  // 3:
} eStateMPOSMode;
#endif


/**
 * @brief mpos struct
 */
typedef struct
{
    eStateMPOS state;
    void (*fptr_mpos_1)(void *ptr);
    void (*fptr_mpos_2)(void *ptr);
#if (MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_LBG1_START_QEP) || (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    eStateMPOSMode mposmode;
#endif

    // Outputs
    int32_t mpos_spd_radps;
    int32_t mpos_spd_scaled_q14;
    int32_t mpos_spd_scaled_q28;
    int32_t mpos_angle;
    int32_t mpos_motor_direction;
    int32_t mpos_blend_angle;

    int32_t anglediff_pass_flag;
    int32_t angle_diff_q12;

#if (MODULE_MPOS_LBG1 == ENABLED)
    Struct_Est est;
#ifdef ENABLE_PLL_ANGLE_CALCULATION
    Struct_Est_PLL est_pll;
#endif
#if defined(ENABLE_EST_ASSIST)
    int32_t estassist_angle_corr;
#endif
#endif

#if (MODULE_MPOS_HALL == ENABLED)
    Struct_Hall hall;
#endif

#if (MODULE_MPOS_QEP == ENABLED)
    Struct_QEP qep;
#endif

#if (MODULE_MPOS_TRAP == ENABLED)
    Struct_Trap trap;
#endif

#if (MODULE_MPOS_ALIGNGO == ENABLED)
    Struct_AlignGoRun aligngo;
#endif

#if defined(ENABLE_QEP_LINEARIZATION_TUNING)
    int32_t mpos_mech_angle;
    int32_t mpos_est_angle_prev;
    uint32_t angle_flip_counter;
#endif
}Struct_MPos;


///////////////////////////////////////////////////
// MPOS Compile Errors
///////////////////////////////////////////////////

//#if (MODULE_MPOS_ALIGNGO == ENABLED) && (MODULE_MPOS_HALL == ENABLED)
//    #error "MODULE_MPOS_ALIGNGO & MODULE_MPOS_HALL cannot be defined simultaneously."
//#endif

#if (MODULE_MPOS_ALIGNGO == DISABLED) && (MODULE_MPOS_HALL == DISABLED)
    #error "No startup method selected."
#endif

#if (MODULE_MPOS_LBG1 == DISABLED) && (MODULE_MPOS_HALL == DISABLED) && (MODULE_MPOS_QEP == DISABLED)
    #error "No MPOS method selected."
#endif

#if defined(ENABLE_HALL_TUNING) && (MODULE_MPOS_HALL == DISABLED)
    #error "MODULE_MPOS_HALL must be enabled if ENABLE_HALL_TUNING is defined."
#endif

#if (MODULE_MPOS_QEP == ENABLED) && !defined(FAMILY_55XX)
    #error "MODULE_MPOS_QEP is only supported in FAMILY_55XX."
#endif

#if (MODULE_MPOS_QEP == ENABLED) && (DEVICE_SELECT == DEVICE_5556A) && defined(VER1)
    #error: "QEP with PAC5556A device needs to comment 'VER1'."
#endif

#if ((MPOS_SELECT == MPOS_LBG1_START_HALL) || (MPOS_SELECT == MPOS_HALL_ONLY)) && (HALL_METHOD_SELECT == HALL_METHOD_POLLING)
   #error "HALL_METHOD_POLLING is no longer supported"
#endif

#if defined(ENABLE_IPD) && (MODULE_MPOS_ALIGNGO == DISABLED)
    #error: ENABLE_IPD is not supported when MODULE_MPOS_ALIGNGO == DISABLED.
#endif

#if defined(ENABLE_OPDOL) && (MODULE_MPOS_ALIGNGO == DISABLED)
    #error: ENABLE_OPDOL is not supported when MODULE_MPOS_ALIGNGO == DISABLED.
#endif

#if defined(ENABLE_COASTING) && (MODULE_MPOS_ALIGNGO == DISABLED)
    #error: ENABLE_COASTING is not supported when MODULE_MPOS_ALIGNGO == DISABLED.
#endif

#endif  //#ifndef MPOS_STRUCT_H
