
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

#ifndef DEBUG_DISPLAY_H
#define DEBUG_DISPLAY_H

#include "config_debug.h"

#define MAX_GUI_DEBUG_ARRAY_LENGHTH 25     // max array index is 25, GUI debug variables can go from 0 to 24

typedef struct
{
    int32_t *ptr_gui_debug_value_q14;
}My_GUI_Debug_Q14;

extern int32_t debug1_var_min;
extern int32_t debug1_var_max;
extern int32_t debug2_var_min;
extern int32_t debug2_var_max;
extern int32_t debug3_var_min;
extern int32_t debug3_var_max;

extern My_GUI_Debug_Q14* test_ptr1;
extern My_GUI_Debug_Q14* test_ptr2;
extern My_GUI_Debug_Q14* test_ptr3;
extern const My_GUI_Debug_Q14 GUI_Debug_Var_Q14[MAX_GUI_DEBUG_ARRAY_LENGHTH];

extern unsigned char debug1_index_var;
extern unsigned char debug2_index_var;
extern unsigned char debug3_index_var;

#define Q0_TO_Q16   16
#define Q6_TO_Q16   10
#define Q12_TO_Q16  4
#define Q14_TO_Q16  2
#define Q2_TO_Q16   14
#define Q16_TO_Q16  0

extern const unsigned char Debug_Q_Q16_Conv_Table [MAX_GUI_DEBUG_ARRAY_LENGHTH];

PAC5XXX_RAMFUNC static inline void foc_display_debug_check_min_max_value_Q14 (int32_t* temp_val_ptr UNUSED_PARAM, int32_t* temp_val_max_ptr UNUSED_PARAM, int32_t* temp_val_min_ptr UNUSED_PARAM)
{
#ifdef ENABLE_DEBUG_DISPLAY
    if (*temp_val_ptr > *temp_val_max_ptr)
        *temp_val_max_ptr = *temp_val_ptr;
    else if (*temp_val_ptr < *temp_val_min_ptr)
        *temp_val_min_ptr = *temp_val_ptr;
#endif
}

PAC5XXX_RAMFUNC static inline void run_debug_display (void)
{
#ifdef ENABLE_DEBUG_DISPLAY
#if (MODULE_MPOS_LBG1 == ENABLED)
    Struct_Motor* motor_ptr = &motor;
#endif
    // Calculations for Debug Display

    // Convert angle diff from fix16_t to 3Q12 so it can fit with in 16 bit variable
#if (MODULE_MPOS_LBG1 == ENABLED)
    fix16_t temp_angle_diff = (motor_ptr->mpos.mpos_angle - motor_ptr->mpos.est.estimated_angle) >> 4;
    motor_ptr->debug.angle_diff_value_q14_3q12 = temp_angle_diff;

    // Convert difference between ideal and estimated angle to q12 format
    if (motor_ptr->debug.angle_diff_value_q14_3q12 > (FIX16_PI >> 4))
        motor_ptr->debug.angle_diff_value_q14_3q12 -= (FIX16_2PI >> 4);
    else if (motor_ptr->debug.angle_diff_value_q14_3q12 < ((-FIX16_PI) >> 4))
        motor_ptr->debug.angle_diff_value_q14_3q12 += (FIX16_2PI >> 4);
#else
#endif

    // Update debug display
    foc_display_debug_check_min_max_value_Q14(test_ptr1->ptr_gui_debug_value_q14 , &debug1_var_max, &debug1_var_min);
    foc_display_debug_check_min_max_value_Q14(test_ptr2->ptr_gui_debug_value_q14 , &debug2_var_max, &debug2_var_min);
    foc_display_debug_check_min_max_value_Q14(test_ptr3->ptr_gui_debug_value_q14 , &debug3_var_max, &debug3_var_min);
#endif
}

#endif  //#ifndef DEBUG_DISPLAY_H
