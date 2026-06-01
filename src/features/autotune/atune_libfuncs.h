/**
 * @file atune_libfuncs.h
 * @brief Use for atune lib function definition
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

#ifndef AT_LIBFUNCS_H
#define AT_LIBFUNCS_H

#include "pac5xxx.h"
#include "atune_struct.h"
#include "pid.h"
#include "control_struct.h"
#include "app_struct.h"
#include "foc_struct.h"
#include "mparam_struct.h"
#include "state_struct.h"


// MID1 Funcs Lib
/**
 * @brief Process mid1 control injection process
 * @param Struct_ATune *atune_ptr
 * @param int32_t va2mid_q14
 * @param int32_t vb2mid_q14
 * @param int32_t vb2mid_q14
 * @param int32_t ia_q14
 * @param int32_t ib_q14
 * @param int32_t ic_q14
 */
PAC5XXX_RAMFUNC void mid1_process_control_lib(Struct_ATune *atune_ptr, int32_t va2mid_q14, int32_t vb2mid_q14, int32_t vc2mid_q14, int32_t ia_q14, int32_t ib_q14, int32_t ic_q14);


/**
 * @brief Runh mid1 controller
 * @param Struct_ATune *atune_ptr
 * @param Struct_App *app_ptr
 */
void mid1_controller(Struct_ATune *atune_ptr, Struct_App *app_ptr);


/**
 * @brief Calculate inverter mos compensation parameters
 * @param Struct_ATune *atune_ptr
 * @param int32_t dcbus_voltage_1div_q16
 * @param int32_t v_diode_q14
 * @param int32_t i_scale_q16
 * @param int32_t rdson_q16
 */
void mid1_calc_inverter_mos(Struct_ATune *atune_ptr, int32_t dcbus_voltage_1div_q16, int32_t v_diode_q14, int32_t i_scale_q16, int32_t rdson_q16);


/**
 * @brief Calculate inverter bipolar compensation parameters
 * @param Struct_ATune *atune_ptr
 * @param int32_t dcbus_voltage_1div_q16
 * @param int32_t v_diode_q14
 * @param int32_t v_ce_q14
 */
void mid1_calc_inverter_igbt(Struct_ATune *atune_ptr, int32_t dcbus_voltage_1div_q16, int32_t v_diode_q14, int32_t v_ce_q14);


/**
 * @brief Calculate motor coil inductance
 * @param int32_t finj_q6
 * @param v_q16
 * @param int32_t v_diode_q14
 * @param int32_t v_ce_q14
 */
void mid1_calc_ls(int32_t finj_q6, int32_t p_app_sq_scaled_q40, int32_t p_real_sq_scaled_q40, int32_t i_rms_sq_scaled_q28, int32_t v_scale_q16, int32_t i_scale_q16, int32_t *ls_q26);


/**
 * @brief Calculate motor coil resistance
 * @param int32_t vdc1_scaled_q14
 * @param int32_t vdc2_scaled_q14
 * @param int32_t idc1_scaled_q14
 * @param int32_t idc2_scaled_q14
 * @param int32_t v_scale_q16
 * @param int32_t i_scale_q16
 * @param int32_t *rs_q16
 */
void mid1_calc_req(int32_t vdc1_scaled_q14, int32_t vdc2_scaled_q14, int32_t idc1_scaled_q14, int32_t idc2_scaled_q14, int32_t v_scale_q16, int32_t i_scale_q16, int32_t *req_q16);


/**
 * @brief Reset mid1 sub state machine and variables
 * @param Struct_ATune *atune_ptr
 */
void mid1_reset_subsm(Struct_ATune *atune_ptr);


// MID2 Funcs Lib

/**
 * @brief Run mid2 process in control
 * @param Struct_ATune *atune_ptr
 * @param int32_t speed_scaled_q14
 * @param int32_t vq_q14
 * @param int32_t vd_q14
 * @param int32_t iq_q14
 * @param int32_t angle
 */
PAC5XXX_RAMFUNC void mid2_process_control_lib(Struct_ATune *atune_ptr, int32_t speed_scaled_q14, int32_t vq_q14, int32_t vd_q14, int32_t iq_q14, int32_t angle);


/**
 * @brief Run mid2 controller
 * @param Struct_Control *control_ptr
 * @param Struct_App *app_ptr
 * @param Struct_FOC *foc_ptr
 * @param Struct_MParam *mparam_ptr
 * @param Struct_State *state_ptr
 */
void mid2_controller(Struct_ATune *atune_ptr, Struct_Control *control_ptr, Struct_App *app_ptr, Struct_FOC *foc_ptr, Struct_MParam *mparam_ptr, Struct_State *state_ptr);


/**
 * @brief Run mid2 process in systick
 * @param Struct_ATune *atune_ptr
 */
void mid2_process_systick_lib(Struct_ATune *atune_ptr);


/**
 * @brief Run mid2 calculation to calculate kt and j
 * @param Struct_ATune *atune_ptr
 * @param int32_t v_scale_q16
 * @param int32_t current_scale_q16
 * @param int32_t speed_scale_q0
 * @param int32_t ls_q26
 * @param int32_t rs_q16
 * @param int32_t pole_pairs_q0
 */
void mid2_calc_kt_j(Struct_ATune *atune_ptr, int32_t v_scale_q16, int32_t current_scale_q16, int32_t speed_scale_q0, int32_t ls_q26, int32_t rs_q16, int32_t pole_pairs_q0);


/**
 * @brief Reset mid2 variables
 * @param Struct_ATune *atune_ptr
 */
void mid2_reset_vars(Struct_ATune *atune_ptr);


// ATPI Funcs Lib

/**
 * @brief Run auto tune controller to calculate current and speed PI gains
 * @param Struct_ATune *atune_ptr
 * @param Struct_Control *control_ptr
 * @param Struct_App *app_ptr
 * @param Struct_MParam *mparam_ptr
 */
void atpi_controller(Struct_ATune *atune_ptr, Struct_Control *control_ptr, Struct_App *app_ptr, Struct_MParam *mparam_ptr);


/**
 * @brief Run to calculate current controller gain
 * @param Struct_ATune *atune_ptr
 * @param int32_t ls_q26
 * @param int32_t rs_q16
 * @param int32_t *i_ka_q16
 * @param int32_t *i_kb_q16
 */
void atpi_calc_gain_current(Struct_ATune *atune_ptr, int32_t ls_q26, int32_t rs_q16, int32_t *i_ka_q16, int32_t *i_kb_q16);


/**
 * @brief Run to calculate speed controller gain
 * @param Struct_ATune *atune_ptr
 * @param int32_t ke_q26
 * @param int32_t j_q26
 * @param uint8_t pole_pairs
 */
void atpi_calc_gain_speed(Struct_ATune *atune_ptr, int32_t ke_q26, int32_t j_q26, uint8_t pole_pairs);

#endif  //#ifndef AT_LIBFUNCS_H
