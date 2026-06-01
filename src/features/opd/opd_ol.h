/**
 * @file opd_ol.h
 * @brief Use to process control functions in Open-loop OPD
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

#ifndef OPDOL_H
#define OPDOL_H

#include "opd_ol_struct.h"

//OL OPD STATUS
#define STATUS_OPDOL_IDLE               0
#define STATUS_OPDOL_SAMPLING           1
#define STATUS_OPDOL_SAMPLEDONE         2
#define STATUS_OPDOL_PASS               3
#define STATUS_OPDOL_FAIL               4
#define STATUSMASK_OPDOL_FAIL_U         0x10
#define STATUSMASK_OPDOL_FAIL_V         0x20
#define STATUSMASK_OPDOL_FAIL_W         0x40

//OPD OL MAIN STATES
#define STATE_OPDOL_MAIN_IDLE               0
#define STATE_OPDOL_MAIN_START              1
#define STATE_OPDOL_MAIN_SAMPLING           2
#define STATE_OPDOL_MAIN_SAMPLEDONE         3
#define STATE_OPDOL_MAIN_COMPLETE           4

//OPD OL CTL STATES
#define STATE_OPDOL_CTL_IDLE                0
#define STATE_OPDOL_CTL_START               1
#define STATE_OPDOL_CTL_IQRAMPING           2
#define STATE_OPDOL_CTL_DELAY_STARTSAMPLE   3
#define STATE_OPDOL_CTL_SAMPLING            4
#define STATE_OPDOL_CTL_SAMPLEDONE          5

/**
 * @brief Use to process the OPD function calls
 * @param Struct_OPDOL* opdol_ptr
 * @param fix16_t iqref_ramp_q14
 */
extern void check_opdol_fault(Struct_OPDOL* opdol_ptr, fix16_t iqref_ramp_q14);

/**
 * @brief Use to store the current
 * @param Struct_OPDOL* opdol_ptr
 * @param int32_t Iu_measured_q14
 * @param int32_t Iv_measured_q14
 * @param int32_t Iw_measured_q14
 */
extern PAC5XXX_RAMFUNC void store_current_opdol(Struct_OPDOL* opdol_ptr, int32_t Iu_measured_q14, int32_t Iv_measured_q14, int32_t Iw_measured_q14);

#endif

