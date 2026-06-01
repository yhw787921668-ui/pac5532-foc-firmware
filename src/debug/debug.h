
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

#ifndef DEBUG_H
#define DEBUG_H

#include "config_debug.h"

#include "config_features.h"
#ifdef ENABLE_DEBUG
#ifdef ENABLE_DEBUG_PACINFO	
#include "status_struct.h"
#endif
#endif

#if defined(ENABLE_DEBUG) || defined(ENABLE_TM)
typedef struct
{
		int32_t dummy;																	// To avoid empty structure
	
#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined(ENABLE_DEBUG_RTT)
    int32_t estimated_angle_q14;                    // estimated angle use for debugging
    int32_t angle_q14;                              // open loop angle use for debugging
    fix16_t debug_buffer_command1;                  // Use to hold debug command for pulling appropriate data buffer
    fix16_t debug_buffer_command2;                  // Use to hold debug command for pulling appropriate data buffer
    fix16_t debug_buffer_command3;                  // Use to hold debug command for pulling appropriate data buffer
    fix16_t debug_buffer_command4;                  // Use to hold debug command for pulling appropriate data buffer
#endif

#ifdef ENABLE_DEBUG_DISPLAY
    int32_t angle_diff_value_q14_3q12;              // Hold real and estimated angle diff value in open loop in q14 format
    int32_t dcbus_voltage_q14_9q6;                  // Use to hold motor bus DC voltage
#endif

#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
    fix16_t cl_speed_backup;                        // close loop speed backup
    fix16_t cl_speed_test_limit1;                   // close loop speed limit 1 for testing auto speed test
    fix16_t cl_speed_test_limit2;                   // close loop speed limit 1 for testing auto speed test
    fix16_t cl_speed_test_enable;                   // if bit set close loop speed test in enable
#endif

#ifdef ENABLE_TM
    uint8_t testmode_enable;
    int32_t sim_spd_nobemf_radps;
#endif

#ifdef DEBUGDAC_DISPLAYDUTY_TICKS
    int32_t duty_value_u_q14;
    int32_t duty_value_v_q14;
    int32_t duty_value_w_q14;
#endif
#ifdef ENABLE_DEBUG_SAMPLE_RAM
    uint32_t param_selection;
    uint16_t number_variables;
    uint32_t number_samples;
    uint16_t buffer_index;
    uint16_t skip_cycles;
    uint32_t extraction_state;
    uint32_t capture_cnt;
#endif
} Struct_Debug;
#endif

#if defined(ENABLE_DEBUG_PWMDAC) || defined(ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined(ENABLE_DEBUG_RTT)
    extern fix16_t debug_dac_1a;
    extern fix16_t debug_dac_1b;
    extern fix16_t debug_dac_1c;
    extern fix16_t debug_dac_2a;
    extern fix16_t debug_dac_2b;
    extern fix16_t debug_dac_2c;
#endif

#ifdef ENABLE_DEBUG_DISPLAY
    extern fix16_t debug_var_1;
    extern fix16_t debug_var_2;
    extern fix16_t debug_var_3;
    extern fix16_t debug_var_4;
    extern fix16_t debug_var_5;
    extern fix16_t debug_var_6;
#endif

#ifdef ENABLE_DEBUG_INPUT
    extern fix16_t debug_input_1;
    extern fix16_t debug_input_2;
    extern fix16_t debug_input_3;
    extern uint32_t debug_input_4;
    extern uint32_t debug_input_5;
    extern uint32_t debug_input_6;
#endif

#ifdef ENABLE_DEBUG_IO
#include "debug_io.h"
#endif

#if (defined(TM_SIM_SLOWSPEED) || defined(TM_SIM_SWCL) || defined(TM_SIM_REVERSE) || defined(TM_SIM_NODETECT)) && !defined(ENABLE_SIM)
    // SIM testmodes are not functional if SIM is not enabled
    #error: ENABLE_SIM must be enabled when ENABLE_TM is enabled.
#endif

#if (defined(TM_SIM_SLOWSPEED) || defined(TM_SIM_SWCL) || defined(TM_SIM_REVERSE) || defined(TM_SIM_NODETECT)) && defined(ENABLE_COASTING)
    // Enabling ENABLE_COASTING interferes with the behavior of the SIM testmodes
    #error: ENABLE_COASTING must be disabled when ENABLE_TM is enabled.
#endif

#if (defined(TM_SIM_SLOWSPEED) || defined(TM_SIM_SWCL) || defined(TM_SIM_REVERSE) || defined(TM_SIM_NODETECT)) && defined(ENABLE_SIMBRAKE_MOTORDISABLE)
    // Enabling ENABLE_SIMBRAKE_MOTORDISABLE interferes with the behavior of the SIM testmodes
    #error: ENABLE_SIMBRAKE_MOTORDISABLE must be disabled when ENABLE_TM is enabled.
#endif

#if (MODE_1SHUNT_SELECT == MODE_1SHUNT_ASYMMETRIC) && defined(ENABLE_DEBUG_PWMDAC)
    #error ENABLE_DEBUG_PWMDAC is not currently supported in MODE_1SHUNT_ASYMMETRIC.
#endif

#endif // DEBUG_H

