
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

#include "motor_struct.h"
#include "debug_display.h"

#ifdef ENABLE_DEBUG_DISPLAY

fix16_t debug_var_1 = 0;
fix16_t debug_var_2 = 0;
fix16_t debug_var_3 = 0;
fix16_t debug_var_4 = 0;
fix16_t debug_var_5 = 0;
fix16_t debug_var_6 = 0;

int32_t debug1_var_min;
int32_t debug1_var_max;
int32_t debug2_var_min;
int32_t debug2_var_max;
int32_t debug3_var_min;
int32_t debug3_var_max;

unsigned char debug1_index_var = 0;
unsigned char debug2_index_var = 0;
unsigned char debug3_index_var = 0;

int32_t debug_zero_val_q14 = 0;

const My_GUI_Debug_Q14 GUI_Debug_Var_Q14 [MAX_GUI_DEBUG_ARRAY_LENGHTH] =
{
        // debug_index == 0, Iq_filt
        {&motor.foc.iq_q14},
//        {&motor.control.Iu_measured_q14},
        // debug_index == 1, Id_filt
        {&motor.foc.id_q14},
//        {&motor.Iv_measured_q14},
        // debug_index == 2, Vq
        {&motor.foc.vq_q14},
//        {&motor.Iw_measured_q14},
        // debug_index == 3, Vd
        {&motor.foc.vd_q14},
        // debug_index == 4, Vq_descaled
        {&motor.foc.vq_q14},
        // debug_index == 5, Vd_descaled
        {&motor.foc.vd_q14},
        // debug_index == 6, Valpha
        {&motor.foc.valpha_q14},
        // debug_index == 7, Angle diff
        {&motor.debug.angle_diff_value_q14_3q12},
        // debug_index == 8, Current Valid
        {&debug_zero_val_q14},
        // debug_index == 9, Vdc
//        {&motor.debug.dcbus_voltage_q14_9q6},
        {&motor.app.dcbus_voltage},
        // debug_index == 10, iq_ramp
        {&motor.foc.iqref_ramp_q14},
#if defined(ENABLE_STALL_DETECT)
        // debug_index == 11, Vdrvsq
        {&motor.app.vdrvsq_q14_peak_stored},
#else
        // debug_index == 11,
        {&debug_zero_val_q14},
#endif
#if defined(ENABLE_STALL_DETECT)
        // debug_index == 12, Vbemfsq
        {&motor.stall.ve_sqr_q14},
#else
        // debug_index == 12,
        {&debug_zero_val_q14},
#endif
        // debug_index == 13, u_duty_value
        {&motor.control.u_duty_value},
#ifdef ENABLE_POWERMEASURE
        // debug_index == 14, UserDef4
        {&motor.power.ibus_avg_q16},
        // debug_index == 15, UserDef5
        {&motor.power.power_result},
#else
        // debug_index == 14, UserDef4
        {&debug_zero_val_q14},
        // debug_index == 15, UserDef5
        {&debug_zero_val_q14},
#endif
        // debug_index == 16, UserDef6
//        {&debug_zero_val_q14},
        // JAGTAG: Need to bring this out to GUI
        {&motor.control.outerloop_ramp_scaled_q14},
        // debug_index == 17, UserDef7
//        {&debug_zero_val_q14},
        {&motor.foc.idref_ramp_q14},
        // debug_index == 18, UserDef8
        {&debug_zero_val_q14},
        // debug_index == 19, Debug 1
        {&debug_var_1},
        // debug_index == 20, Debug 2
        {&debug_var_2},
        // debug_index == 21, Debug 3
        {&debug_var_3},
        // debug_index == 22, Debug 4
        {&debug_var_4},
        // debug_index == 23, Debug 5
        {&debug_var_5},
        // debug_index == 24, Debug 6
        {&debug_var_6},
};

const unsigned char Debug_Q_Q16_Conv_Table [MAX_GUI_DEBUG_ARRAY_LENGHTH] =
{
    Q14_TO_Q16,     // debug_index == 0, {&motor.iq_q14}
    Q14_TO_Q16,     // debug_index == 1,  {&motor.id_q14}
    Q14_TO_Q16,     // debug_index == 2, {&motor.vq_q14}
    Q14_TO_Q16,     // debug_index == 3, {&motor.vd_q14}
    Q14_TO_Q16,     // debug_index == 4, {&motor.Vq_descaled_9q6}
    Q14_TO_Q16,     // debug_index == 5, {&motor.Vd_descaled_9q6}
    Q14_TO_Q16,     // debug_index == 6, {&motor.valpha_q14}
    Q12_TO_Q16,     // debug_index == 7, {&motor.debug.angle_diff_value_q14_3q12}
    Q14_TO_Q16,     // debug_index == 8, {&debug_zero_val_q14}
    Q16_TO_Q16,//Q6_TO_Q16,      // debug_index == 9, {&motor.debug.dcbus_voltage_q14_9q6}
    Q14_TO_Q16,     // debug_index == 10, {&motor.foc.iqref_ramp_q14}
#if defined(ENABLE_STALL_DETECT)
    Q14_TO_Q16,     // debug_index == 11, {&motor.app.vdrvsq_q14}
    Q14_TO_Q16,     // debug_index == 12, {&motor.stall.ve_sqr_q14}
#else
    Q16_TO_Q16,     // debug_index == 11, {&debug_zero_val_q14}
    Q16_TO_Q16,     // debug_index == 12, {&debug_zero_val_q14}
#endif
    Q0_TO_Q16,      // debug_index == 13, {&motor.control.u_duty_value}
    Q16_TO_Q16,     // debug_index == 14, UserDef4, {&userdef_var4}
    Q16_TO_Q16,     // debug_index == 15, UserDef5, {&userdef_var5}
    Q16_TO_Q16,     // debug_index == 16, UserDef6, {&userdef_var6}
    Q14_TO_Q16,     // debug_index == 17, UserDef7, {&userdef_var7}
    Q16_TO_Q16,     // debug_index == 18, UserDef8, {&userdef_var8}
    Q16_TO_Q16,     // debug_index == 19, Debug 1, {&debug_var_1}
    Q16_TO_Q16,     // debug_index == 20, Debug 2, {&debug_var_2}
    Q16_TO_Q16,     // debug_index == 21, Debug 3, {&debug_var_3}
    Q16_TO_Q16,     // debug_index == 22, Debug 4, {&debug_var_4}
    Q16_TO_Q16,     // debug_index == 23, Debug 5, {&debug_var_5}
    Q16_TO_Q16,     // debug_index == 24, Debug 6, {&debug_var_6}
};

My_GUI_Debug_Q14* test_ptr1 = (My_GUI_Debug_Q14*) &GUI_Debug_Var_Q14[9].ptr_gui_debug_value_q14;
My_GUI_Debug_Q14* test_ptr2 = (My_GUI_Debug_Q14*) &GUI_Debug_Var_Q14[10].ptr_gui_debug_value_q14;
My_GUI_Debug_Q14* test_ptr3 = (My_GUI_Debug_Q14*) &GUI_Debug_Var_Q14[15].ptr_gui_debug_value_q14;


#endif
