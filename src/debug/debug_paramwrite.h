
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

#ifndef DEBUG_PARAMWRITE_H
#define DEBUG_PARAMWRITE_H

#include "motor_struct.h"

//#include "debug_dac.h"
//#include "debug_das.h"
//#include "debug_display.h"
//#include "debug_export.h"

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#ifdef ENABLE_DEBUG_DISPLAY
#include "debug_display.h"
#endif

#ifdef ENABLE_DEBUG_PARAMEXPORT
#include "debug_export.h"
#endif

#ifdef ENABLE_DEBUG_DAS
#include "debug_das.h"
#endif

#ifdef ENABLE_DEBUG_RTT
#include "USER_RTT.h"
#endif
#ifdef ENABLE_DEBUG_SAMPLE_RAM
#include "debug_dac.h"
#include "debug_capture.h"
#endif

#if defined(ENABLE_FORCEANG)
#include "forceang_funcs.h"
#endif


#if defined(ENABLE_DEBUG)
/**
 * @brief This function use for transport parameters form GUI to motor control and debug and other variables
 * @param ParameterCode param_code parameter code
 * @param fix16_t rx_data 32 bit rx data
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static void debug_paramwrite(ParameterCode param_code, fix16_t rx_data UNUSED_PARAM, Struct_Motor *motor_ptr UNUSED_PARAM)
{
    switch (param_code)
    {
#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined (ENABLE_DEBUG_RTT)
    case pwm_dac_selection1:
        if ((rx_data >= 0) && (rx_data < PWM_DEBUG_ARRAY_LENGTH))
        {
            motor_ptr->debug.debug_buffer_command1 = rx_data;
            dac_ptr1 = DAC_ARRAY_Q14[motor_ptr->debug.debug_buffer_command1];
        }
        break;
    case pwm_dac_selection2:
        if ((rx_data >= 0) && (rx_data < PWM_DEBUG_ARRAY_LENGTH))
        {
            motor_ptr->debug.debug_buffer_command2 = rx_data;
            dac_ptr2 = DAC_ARRAY_Q14[motor_ptr->debug.debug_buffer_command2];
        }
        break;
    case pwm_dac_selection3:
        if ((rx_data >= 0) && (rx_data < PWM_DEBUG_ARRAY_LENGTH))
        {
            motor_ptr->debug.debug_buffer_command3 = rx_data;
            dac_ptr3 = DAC_ARRAY_Q14[motor_ptr->debug.debug_buffer_command3];
        }
        break;
    case pwm_dac_selection4:
        if ((rx_data >= 0) && (rx_data < PWM_DEBUG_ARRAY_LENGTH))
        {
            motor_ptr->debug.debug_buffer_command4 = rx_data;
            dac_ptr4 = DAC_ARRAY_Q14[motor_ptr->debug.debug_buffer_command4];
        }
        break;

        // PWM DAC Scale Factors
    case scale_factor1:
        debug_scale_factor1 = ((rx_data * (PWMDAC_PERIOD_TICKS >> 1)) >> 8); // q8 = q16 * q0 = q16>>8
#ifdef ENABLE_DEBUG_DAS
#if (DAS_VERSION == DAS_VERSION_1)
        debug.scale[0] = debug_scale_factor1;
#elif (DAS_VERSION == DAS_VERSION_2)
        debug.scale[0] = (rx_data * DAS_SCALE_COEFFICIENT) >> 16; // q0 = q16 * q0 = q16>>16
#else

#endif

#endif
        break;
    case scale_factor2:
        debug_scale_factor2 = ((rx_data * (PWMDAC_PERIOD_TICKS >> 1)) >> 8); // q8 = q16 * q0 = q16>>8
#ifdef ENABLE_DEBUG_DAS
#if (DAS_VERSION == DAS_VERSION_1)
        debug.scale[1] = debug_scale_factor2;
#elif (DAS_VERSION == DAS_VERSION_2)
        debug.scale[1] = (rx_data * DAS_SCALE_COEFFICIENT) >> 16; // q0 = q16 * q0 = q16>>16
#else

#endif
#endif
        break;
    case scale_factor3:
        debug_scale_factor3 = ((rx_data * (PWMDAC_PERIOD_TICKS >> 1)) >> 8); // q8 = q16 * q0 = q16>>8
#ifdef ENABLE_DEBUG_DAS
#if (DAS_VERSION == DAS_VERSION_1)
        debug.scale[2] = debug_scale_factor3;
#elif (DAS_VERSION == DAS_VERSION_2)
        debug.scale[2] = (rx_data * DAS_SCALE_COEFFICIENT) >> 16; // q0 = q16 * q0 = q16>>16
#else

#endif
#endif
        break;
    case scale_factor4:
        debug_scale_factor4 = ((rx_data * (PWMDAC_PERIOD_TICKS >> 1)) >> 8); // q8 = q16 * q0 = q16>>8
#ifdef ENABLE_DEBUG_DAS	
#if (DAS_VERSION == DAS_VERSION_1)
        debug.scale[3] = debug_scale_factor4;
#elif (DAS_VERSION == DAS_VERSION_2)
        debug.scale[3] = (rx_data * DAS_SCALE_COEFFICIENT) >> 16; // q0 = q16 * q0 = q16>>16
#else

#endif
#endif
        break;
#endif
#ifdef ENABLE_DEBUG_DISPLAY
    case debug1_index:
        if ((rx_data >= 0) && (rx_data < MAX_GUI_DEBUG_ARRAY_LENGHTH))
        {
            debug1_index_var = rx_data & 0xFF;
            test_ptr1 = (My_GUI_Debug_Q14 *)&GUI_Debug_Var_Q14[0 + debug1_index_var].ptr_gui_debug_value_q14;
        }
        break;
    case debug2_index:
        if ((rx_data >= 0) && (rx_data < MAX_GUI_DEBUG_ARRAY_LENGHTH))
        {
            debug2_index_var = rx_data & 0xFF;
            test_ptr2 = (My_GUI_Debug_Q14 *)&GUI_Debug_Var_Q14[0 + debug2_index_var].ptr_gui_debug_value_q14;
        }
        break;
    case debug3_index:
        if ((rx_data >= 0) && (rx_data < MAX_GUI_DEBUG_ARRAY_LENGHTH))
        {
            debug3_index_var = rx_data & 0xFF;
            test_ptr3 = (My_GUI_Debug_Q14 *)&GUI_Debug_Var_Q14[0 + debug3_index_var].ptr_gui_debug_value_q14;
        }
        break;
#endif

    // Misc Vars
    case misc_fix16_1:
#ifdef ENABLE_DEBUG_INPUT
        debug_input_1 = rx_data;
#if (MODULE_MPOS_HALL == ENABLED)
        // Time received in msec and converted to sec in q26
//        int32_t temp = fix16_div(QCONST(1024.0f, Q16), QCONST(1000.0f, Q16));
//        motor_ptr->mpos.hall.halledge_polskew_time_q26 = fix16_mul(debug_input_1, temp);
#endif
#endif
        break;

    case misc_fix16_2:
#ifdef ENABLE_DEBUG_INPUT
            debug_input_2 = rx_data;
#if (MPOS_SELECT == MPOS_HALL_ONLY) || (MPOS_SELECT == MPOS_LBG1_START_HALL)
//            motor_ptr->mpos.hall.hall_sensor_offset_rad = fix16_mul(debug_input_2, CONV_DEG2RAD_FIX16);
//            hall_create_hallstate_angle(motor_ptr);
#endif
#if defined(ENABLE_POWERMEASURE)
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
        // JAGTAG: Bring power_iphase_offset_q14 out to GUI?
//        motor_ptr->power.power_iphase_offset_q14 = (debug_input_2 >> 2);
#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && defined(ENABLE_LCORR_IBUS)
                                                                  // JAGTAG: Bring power_iphase_offset_q14 out to GUI?
                                                                  //            motor_ptr->power.ibusavg_lcor_offset_q16 = (debug_input_2 >> 0);
#endif //POWER_MEASURE_SELECT
#endif //#if defined(ENABLE_POWERMEASURE) && defined(ENABLE_LCORR_IBUS)
#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
        // JAGTAG: Need to bring this out to GUI
        motor_ptr->debug.cl_speed_test_limit1 = fix16_mul(rx_data, FIX16_2PI);
#endif
#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
        // Value received in percentage in q16
        motor_ptr->harmonicinj.harmonicinj_vmag_pct_q16 = debug_input_2;
#endif
#endif
        break;

    case misc_fix16_3:
#ifdef ENABLE_DEBUG_INPUT
        debug_input_3 = rx_data;
#if defined(ENABLE_POWERMEASURE)
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
        // JAGTAG: Bring power_iphase_factor_q14 out to GUI?
//        motor_ptr->power.power_iphase_factor_q14 = (debug_input_3 >> 2);
#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2) && defined(ENABLE_LCORR_IBUS)
                                                                  // JAGTAG: Bring ibusavg_corr_factor_q16 out to GUI?
                                                                  //            motor_ptr->power.ibusavg_lcor_factor_q16 = (debug_input_3 >> 0);
#endif //POWER_MEASURE_SELECT
#endif //#if defined(ENABLE_POWERMEASURE) && defined(ENABLE_LCORR_IBUS)
#ifdef ENABLE_CLSPEED_LIMIT_CHANGE_CHECK
        // JAGTAG: Need to bring this out to GUI
        motor_ptr->debug.cl_speed_test_limit2 = fix16_mul(rx_data, FIX16_2PI);
#endif
#endif
        break;

    case mtpa_gain:
#ifdef ENABLE_MTPA
        // Unit: Degree/A
        motor_ptr->mtpa.mtpa_gain_degpa_q16 = rx_data;
        // Unit: Degree, scaled
        motor_ptr->mtpa.mtpa_gain_rad_q16 = fix16_mul(CONV_DEG2RAD_FIX16, fix16_mul(motor_ptr->mtpa.mtpa_gain_degpa_q16, motor_ptr->app.descale_current_factor));
#endif
        break;

    case misc_uint32_1:
#ifdef ENABLE_DEBUG_INPUT
        debug_input_4 = rx_data;
#endif
        break;

    case misc_uint32_2:
#ifdef ENABLE_DEBUG_INPUT
        debug_input_5 = rx_data;
#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
        // Value received in degrees and converted to radians in q16
        motor_ptr->harmonicinj.harmonicinj_offset_rad_q16 = fix16_mul((debug_input_5 << 16), QCONST(0.017453293f, Q16));
#endif
#endif
        break;

    case misc_uint32_3:
#ifdef ENABLE_DEBUG_INPUT
        debug_input_6 = rx_data;

#if defined(ENABLE_HALL_TUNING)
        motor_ptr->halltune.halltune_mode = debug_input_6;
#endif
#ifdef ENABLE_TM
        // JAGTAG: Need to bring this out to GUI
        motor_ptr->debug.testmode_enable = rx_data;
#endif
#endif
        break;

#ifdef ENABLE_DEBUG_PARAMEXPORT
    case param_read_command:
        gui_parameter_command = rx_data;
        export_variables(gui_parameter_command);
        break;
#endif


#ifdef ENABLE_DEBUG_SAMPLE_RAM
        case capture_mode:
            motor_ptr->debug.skip_cycles = (uint16_t)rx_data;
            break;
        
        case capture_param_selection:
            //debug_dac_1c = rx_data;
            motor_ptr->debug.param_selection = rx_data;
            debug_extraction_param_write(motor_ptr, motor_ptr->debug.param_selection);
            break;
        
        case capture_buffer_index:
            motor_ptr->debug.buffer_index = (uint16_t)rx_data;
            break;

        case capture_start:
            debug_extraction_start(motor_ptr);
            break;

        case capture_cnt:
            motor_ptr->debug.capture_cnt = rx_data;
            break;
#endif
        default:
            break;
    }
}
#endif //#if defined(ENABLE_DEBUG)
#endif // DEBUG_PARAMWRITE_H
