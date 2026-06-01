
/**
 * @file lbg1_pll.h
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

#ifndef LBG1_PLL_H
#define LBG1_PLL_H

#include "pac5xxx.h"
#include "maths.h"
#include "lbg1_struct.h"
#include "lbg1_pll_struct.h"

// Function Declarations
extern void lbg1_pll_calculate_pigain_lib(Struct_Est_PLL* est_pll_ptr, int32_t dt_control_q26, int32_t spd_nom_q16);
extern void lbg1_pll_calculate_coefficients_lib(Struct_Est* est_ptr, Struct_Est_PLL* est_pll_ptr, fix16_t dt_control_q26, fix16_t factor_current_descale, fix16_t motorparam_r,
        int32_t motorparam_l_x1024, uint16_t speed_scale_shift, int32_t gain_scaler_q06, int32_t nom_spd_q16);
extern PAC5XXX_RAMFUNC void lbg1_estimator_pll_lib(Struct_Est* est_ptr, Struct_Est_PLL* est_pll_ptr);

#endif // #ifndef LBG1_PLL_H

