
/**
 * @file lbg1.h
 * @brief Estimator main functions
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

#ifndef LBG1_H
#define LBG1_H

#include "pac5xxx.h"
#include "maths.h"
#include "lbg1_struct.h"

// Constant Definitions
#define LBG_GAIN_SCALER_Q06             QCONST(20000.0f, Q06)


// Function Declarations
extern void lbg1_process_voltage_scale_lib(Struct_Est* est_ptr, fix16_t descale_voltage_factor);
extern void lbg1_update_state_lib(Struct_Est* est_ptr, fix16_t update_angle_q16, fix16_t update_speed_radps_q16, uint16_t speed_scale_shift, int32_t dt_control_q26);
extern void lbg1_check_transition_ready(Struct_Est* est_ptr);
extern void lbg1_calculate_coefficients_lib(Struct_Est* est_ptr, fix16_t dt_control_q26, fix16_t factor_current_descale, fix16_t motorparam_r,
        int32_t motorparam_l_x1024, uint16_t speed_scale_shift, int32_t gain_scaler_q06, int32_t nom_spd_q16);
extern void lbg1_switch2cl(Struct_Est* est_ptr);
extern void lbg1_statemon_lib(Struct_Est* est_ptr, int32_t lq_q26, int32_t rs_q16, int32_t dt_control_q26, int32_t descale_current_factor, int32_t nom_spd_q16);
extern void lbg1_init(Struct_Est* est_ptr, int32_t lq_q26, int32_t rs_q16, int32_t dt_control_q26, int32_t descale_current_factor, int32_t nom_spd_q16);
extern PAC5XXX_RAMFUNC void lbg1_estimator_lib(Struct_Est* est_ptr);

#endif // #ifndef LBG1_H

