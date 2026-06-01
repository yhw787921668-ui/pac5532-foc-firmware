
/**
 * @file config_options.h
 * @brief Use for different FOC hardware and firmware option selection
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

#ifndef CONFIG_OPTIONS_H
#define CONFIG_OPTIONS_H

//========================================
// Parameter Definitions for PARAM_SELECT
//========================================
#define PARAM_DEFAULT               1
#define PARAM_USER_1                2

//========================================
// Comm Definitions for COMM_SELECT
//========================================
#define COMM_UART                   1
#define COMM_PPM                    2
#define COMM_COMBO_PPM_UART         3
#define COMM_PUSH_BUTTON            4
#define COMM_COMBO_PUSH_BUTTON_UART 5
#define COMM_CAN                    6
#define COMM_LIN                    7

//========================================
// Parameter Definitions for MPOS_SELECT
//========================================
#define MPOS_LBG1_START_ALIGNGO         0
#define MPOS_LBG1_START_HALL            1
#define MPOS_HALL_ONLY                  2
#define MPOS_QEP_START_ALIGNGO          3
#define MPOS_LBG1_START_QEP             4
#define MPOS_LBG1_START_TRAP_ALIGNGO    5

//========================================
// Enable/Disable Definitions
//========================================
#define DISABLED                    0
#define ENABLED                     1

//==========================================================
// Definitions for ANGLEDIFF_AUTORANGE_METHOD_SELECT
//==========================================================
#define ANGLEDIFF_AUTORANGE_METHOD_1            1
#define ANGLEDIFF_AUTORANGE_METHOD_2            2

//==========================================================
// Definitions for OUTERLOOP_CONTROL_SELECT
//==========================================================
#define OUTERLOOP_CONTROL_SPEED                 1
#define OUTERLOOP_CONTROL_POWER                 2

//==========================================================
// Definitions for Hall Method, Polling or Interrupt
//==========================================================
#define HALL_METHOD_POLLING                     0
#define HALL_METHOD_INTERRUPT                   1

//==========================================================
// Definitions for MODE_1SHUNT_SELECT
//==========================================================
#define MODE_1SHUNT_EXTENSION                           1
#define MODE_1SHUNT_ASYMMETRIC                          2
#define MODE_1SHUNT_ASYMMETRIC_TIMER_55XX_ONLY          3

//==========================================================
// Definitions for SEL_OC_METHOD
//==========================================================
#define OC_METHOD_HPDAC                 1

//==========================================================
// Definitions for SEL_SIM_METHOD
//==========================================================
#define SIM_METHOD_VOLTAGE_BEMF         1
#define SIM_METHOD_VOLTAGE_KE           2
#define SIM_METHOD_CURRENT              3
#define SIM_METHOD_HALL                 4
#define SIM_METHOD_QEP                  5


//==========================================================
// Definitions for MODULATION_SELECT
//==========================================================
#define MODULATION_SVM_STANDARD         1
#define MODULATION_SVM_OVERMOD          2
#define MODULATION_SVM_HARMONICINJ      3
#define MODULATION_GENMOD_SINE          4
#define MODULATION_GENMOD_SVM           5
#define MODULATION_GENMOD_DPWM          6
#define MODULATION_GENMOD_DPWMMIN       7
#define MODULATION_HYBRID_TRAP_DMIN     8

//==========================================================
// Definitions for FIELD WEAKENING CONTROL METHOD
//==========================================================
#define INCREMENTRAL_CONTROL            1
#define PID_CONTROL                     2

//==========================================================
// Definitions for DQ decoupling
//==========================================================
#define FEEDFORWARD_DECOUPLING          1
#define FEEDBACK_DECOUPLING             2

#define READ_INT32(x)             (*((int32_t *)x))
#define READ_UINT16(x)            (*((uint16_t *)x))
#define READ_UINT8(x)             (*((uint8_t *)x))

#define FLASHRW_MODELSEL_FXADDR         1
#define FLASHRW_MODELSEL_DYADDR         2

#endif // #ifndef CONFIG_OPTIONS_H
