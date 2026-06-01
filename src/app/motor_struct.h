
/**
 * @file motor_struct.h
 * @brief Main motor structure use to hold complete motor object
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

#ifndef MOTOR_STRUCT_H
#define MOTOR_STRUCT_H

#include "pac5xxx.h"
#include "config_app.h"
#include "config_features.h"
#include "config_debug.h"
#include "state_struct.h"
#include "status_struct.h"
#include "foc_struct.h"
#include "mpos_struct.h"
#include "app_struct.h"
#include "control_struct.h"
#include "fault_struct.h"
#include "mparam_struct.h"
#include "post_struct.h"
#include "comm_struct.h"
#include "isr_struct.h"

#ifdef ENABLE_OPD_CL
#include "opd_cl_struct.h"
#endif

#ifdef ENABLE_OPDOL
#include "opd_ol_struct.h"
#endif

#if defined(ENABLE_DEBUG) || defined(ENABLE_TM)
#include "debug.h"
#endif

#if defined(ENABLE_FIELD_WEAKENING)
#include "fw_struct.h"
#endif

#ifdef ENABLE_STALL_DETECT
#include "stalldet.h"
#endif

#if (COMMSEL_PPM == TRUE)
#include "ppm_struct.h"
#endif

#ifdef ENABLE_SIM
#include "sim_struct.h"
#endif

#ifdef ENABLE_IPD
#include "ipd_struct.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
#include "mod_ovm_struct.h"
#endif

#ifdef ENABLE_MTPA
#include "mtpa_struct.h"
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    #include "control_3shunt_struct.h"
#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    #include "control_1shunt_struct.h"
#endif

#ifdef ENABLE_POWERMEASURE
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
    #include "control_power1_struct.h"
#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
    #include "control_power2_struct.h"
#endif
#endif  // #ifdef ENABLE_POWERMEASURE

#ifdef ENABLE_AUTOTUNE_PI
#include "atune_struct.h"
#endif

#if defined(ENABLE_HALL_TUNING)
#include "halltune_struct.h"
#endif

#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
#include "harmonic_inj_struct.h"
#endif

#if defined(ENABLE_FORCEANG)
#include "forceang_struct.h"
#endif

#if defined(ENABLE_FLASH_RW)
#include "flashrw_struct.h"
#endif

#if defined(ENABLE_BUZZ)
#include "buzz_struct.h"
#endif


typedef struct
{
    int16_t bootprot_pwm_max_ticks;
    int8_t bootprot_max_counts;
    int8_t bootprot_count1;
    int8_t bootprot_count2;
    int8_t bootprot_count3;
} Struct_BootProt;


typedef struct
{
    Struct_State state;                 // States
    Struct_Status status;               // Status
    Struct_Fault fault;                 // Fault and Warning
    Struct_App app;                     // Application
    Struct_Control control;             // Control
    Struct_FOC foc;                     // FOC
    Struct_MPos mpos;                   // MPOS
    Struct_MParam mparam;               // Motor Parameters
    Struct_POST post;                   // POST
    Struct_Comm comm;                   // Communication

    Struct_Function_Pointers fptr;      // Function Pointers

    #if (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    Struct_StartupAlignGo startup;
    #elif ((MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL))
    Struct_StartupHall startup;
    #elif ((MPOS_SELECT == MPOS_QEP_START_ALIGNGO) || (MPOS_SELECT == MPOS_LBG1_START_QEP))
    Struct_StartupAlignGo2QEP startup;
    #elif (MPOS_SELECT == MPOS_LBG1_START_TRAP_ALIGNGO)
    Struct_StartupAlignGo2Trap startup;
    #endif

    #if (COMMSEL_PPM == TRUE)
    Struct_PPM ppm;
    #endif

    #ifdef ENABLE_OPD_CL
    Struct_OPDCL opdcl;                 // OPDCL
    #endif

    #ifdef ENABLE_OPDOL
    Struct_OPDOL opdol;                 // OPDOL
    #endif

    #ifdef ENABLE_STALL_DETECT
    Struct_StallDet stall;              // Stall Detection
    #endif

    #if defined(ENABLE_FIELD_WEAKENING)
    Struct_FW fw;                       // Field Weakening
    #endif

    #if defined(ENABLE_DEBUG_PWMDAC)  || defined(ENABLE_DEBUG_DAS) || defined(ENABLE_DEBUG_RTT) || defined(ENABLE_DEBUG_DISPLAY) || defined(ENABLE_CLSPEED_LIMIT_CHANGE_CHECK) || defined(ENABLE_TM) || defined(ENABLE_DEBUG_SAMPLE_RAM)
    Struct_Debug debug;                 // Debug
    #endif

    #ifdef ENABLE_SIM
    Struct_SIM sim;
    #endif

    #ifdef ENABLE_IPD
    Struct_IPD ipd;
    #endif

    #if (MODULATION_SELECT == MODULATION_SVM_OVERMOD)
    Struct_Overmod overmod;
    #endif
    
    #ifdef ENABLE_MTPA
    Struct_Mtpa mtpa;
    #endif

    #if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
    Struct_HarmonicInj harmonicinj;
    #endif

    #if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && (MODE_1SHUNT_SELECT == MODE_1SHUNT_EXTENSION)
    Struct_OneShunt_Extn oneshunt;

    #elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT) && ((MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC) || (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY))
    Struct_OneShunt_Asym oneshunt;

    #elif (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT)
    Struct_ThreeShunt threeshunt;
    #endif

    #ifdef ENABLE_BOOTPROT
    Struct_BootProt bootprot;
    #endif

    #ifdef ENABLE_POWERMEASURE
    Struct_Power power;
    #endif

    #ifdef ENABLE_LCORR_TEMP
    fix16_t temperature_degree_value;
    #endif

    #ifdef ENABLE_AUTOTUNE_PI
    Struct_ATune atune;
    #endif
    
    #if defined(ENABLE_HALL_TUNING)
    Struct_HallTune halltune;
    #endif

#if (INTERNAL_VMS == TRUE)
    int32_t vms_v100;
    int32_t vms_v200;
    int32_t one_div_vms200_minus_vms100;
    int32_t u_duty_cycle_max_limit;
    int32_t vms_adc_vlaue;
#endif

#if defined(ENABLE_FORCEANG)
    Struct_ForceAng forceang;
#endif

#if defined(ENABLE_FLASH_RW)
    Struct_Flash_Read_Wr flash_read_wr;
#endif

#if defined(ENABLE_BUZZ)
    Struct_Buzz buzz;
#endif
} Struct_Motor;

extern Struct_Motor motor;

#endif  //#ifndef MOTOR_STRUCT_H
