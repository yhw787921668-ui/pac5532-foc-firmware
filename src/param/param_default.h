
/**
 * @file param_default.h
 * @brief Use for parameter processing
 */

//=============================================================================
// Copyright (C) 2020 - 2022, Qorvo, Inc.
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

#ifndef PARAMFILE_H
#define PARAMFILE_H


//===========================
// PARAMETER DEFINITIONS
//===========================

#define PUSER_VALUE_PWM_FREQ_HZ                 20000
#define PUSER_VALUE_CONTROL_FREQ_DIV            1

#define PUSER_VALUE_DIFFAMP_GAIN                4

#define PUSER_VALUE_LD_UH                       1000
#define PUSER_VALUE_LQ_UH                       1000
#define PUSER_VALUE_R_OHMS                      0.1000f
#define PUSER_VALUE_POLE_PAIRS                  1
#define PUSER_VALUE_NOMINAL_SPEED               1884.96f
#define PUSER_VALUE_OC_DAC                      0

#define PUSER_VALUE_SPEED_MAX_HZ                1500.00f
#define PUSER_VALUE_POWER_MAX_W                 100.00f
#define PUSER_VALUE_IQ_MAX_A                    1.00f
#define PUSER_VALUE_IQ_MIN_A                    0.00f
#define PUSER_VALUE_OC_LIMIT_A                  25.00f
#define PUSER_VALUE_CL_COAST_TIMEOUT_SEC        10.00f

#define PUSER_VALUE_ALIGN_TIME_SEC              1.00f
#define PUSER_VALUE_ID_REF_OL_A                 0.00f
#define PUSER_VALUE_ITOTAL_REF_OL_A             0.00f
#define PUSER_VALUE_SPEED_RAMP_OL_HZPSEC        50.00f
#define PUSER_VALUE_SPEED_SETPOINT_OL_HZ        100.00f
#define PUSER_VALUE_AUTO_CLOSE_LOOP             0

#define PUSER_VALUE_PI_ID_KP                    0.0000f
#define PUSER_VALUE_PI_ID_KI                    0.0000f
#define PUSER_VALUE_PI_ID_MIN                   -0.9200f
#define PUSER_VALUE_PI_ID_MAX                   0.9200f
#define PUSER_VALUE_FC_ID_HZ                    3000
#define PUSER_VALUE_PI_IQ_KP                    0.0000f
#define PUSER_VALUE_PI_IQ_KI                    0.0000f
#define PUSER_VALUE_PI_IQ_MIN                   0.0000f
#define PUSER_VALUE_PI_IQ_MAX                   1.0000f
#define PUSER_VALUE_FC_IQ_HZ                    3000
#define PUSER_VALUE_IDIQ_RAMP_APSEC             100.00f

#define PUSER_VALUE_PI_SPEED_KP                 0.0000f
#define PUSER_VALUE_PI_SPEED_KI                 0.0000f
#define PUSER_VALUE_SPEED_RAMP_HZPSEC           100.00f

#define PUSER_VALUE_PI_POWER_KP                 0.0000f
#define PUSER_VALUE_PI_POWER_KI                 0.0000f
#define PUSER_VALUE_POWER_RAMP_WPSEC            10.00f

#define PUSER_VALUE_CL_MODE                     0
#define PUSER_VALUE_REVERSE                     0
#define PUSER_VALUE_SPEED_SETPOINT_CL_HZ        100.00f
#define PUSER_VALUE_POWER_SETPOINT_CL_W         50.00f
#define PUSER_VALUE_ID_REF_CL_A                 0.00f
#define PUSER_VALUE_IQ_REF_CL_A                 0.00f

#define PUSER_VALUE_LBG1_KPE                    0.4000f
#define PUSER_VALUE_LBG1_KPI                    0.0000f
#define PUSER_VALUE_LBG1_BANDWIDTH              7539.8f
#define PUSER_VALUE_LBG1_DAMPINGFACTOR          0.7071f

#define PUSER_VALUE_LBG1_FC_SPEED_HZ            100
#define PUSER_VALUE_LBG1_ANGLE_OFFSET_DEG       0.00f
#define PUSER_VALUE_LBG1_ANGLE_FACTOR_DEGPHZ    0.00f
#define PUSER_VALUE_LBG1_FC_BEMF_MAG_HZ         100

#define PUSER_VALUE_LBG2_KPI                    0.4000f
#define PUSER_VALUE_LBG2_KPE                    0.4000f
#define PUSER_VALUE_LBG2_FC_SPEED_HZ            100
#define PUSER_VALUE_LBG2_ANGLE_OFFSET_DEG       0.00f
#define PUSER_VALUE_LBG2_ANGLE_FACTOR_DEGPHZ    0.00f

#define PUSER_VALUE_SMO_KPI                     0.4000f
#define PUSER_VALUE_SMO_KANGLE                  0
#define PUSER_VALUE_SMO_KSLIDE                  1.0000f
#define PUSER_VALUE_SMO_FC_SLIDE_HZ             100
#define PUSER_VALUE_SMO_FC_SPEED_HZ             100
#define PUSER_VALUE_SMO_ANGLE_OFFSET_DEG        0.00f
#define PUSER_VALUE_SMO_ANGLE_FACTOR_DEGPHZ     0.00f

#define PUSER_VALUE_SIM_ALIGN_SEC               1.00f
#define PUSER_VALUE_SIM_IDIQ_GAIN               1.50f
#define PUSER_VALUE_SIM_BTO_SEC                 20.00f
#define PUSER_VALUE_SIM_AMP_GAIN                1

#define PUSER_VALUE_SIM_CURRENT_TRACK_GAIN      10.000f

#define PUSER_VALUE_STALL_KE                    0.00000f
#define PUSER_VALUE_STALL_WINDOW_SEC            0.25f

#define PUSER_VALUE_IPD_CHARGE                  0
#define PUSER_VALUE_IPD_DISCHARGE               0
#define PUSER_VALUE_IPD_PERIODS                 0
#define PUSER_VALUE_IPD_OFFSET_FORWARD_DEG      0
#define PUSER_VALUE_IPD_OFFSET_REVERSE_DEG      0

#define PUSER_VALUE_FW_ID                       0.00f
#define PUSER_VALUE_FW_SPEED                    5000.00f
#define PUSER_VALUE_FW_VDRIVE                   100
#define PUSER_VALUE_FW_PID_KP                   0.1f
#define PUSER_VALUE_FW_PID_KI                   1.0f
#define PUSER_VALUE_FW_VMAX_SCL_FACTOR          1.0f

#define PUSER_VALUE_MISC_FIX16_1                0.0000f
#define PUSER_VALUE_MISC_FIX16_2                0.0000f
#define PUSER_VALUE_MISC_FIX16_3                0.0000f
#define PUSER_VALUE_MTPA_DEGPAMP_FIX16          0.0000f
#define PUSER_VALUE_MISC_UINT32_1               0
#define PUSER_VALUE_MISC_UINT32_2               0
#define PUSER_VALUE_MISC_UINT32_3               0

#define PUSER_VALUE_AUTOTUNE_KE                 0.0000f
#define PUSER_VALUE_AUTOTUNE_J                  0.0000f

#define PUSER_VALUE_HALL_ISTART_AMPS                0.0f
#define PUSER_VALUE_HALL_ANGCORR_GAIN_FWD_DEGPHZ    0.0f
#define PUSER_VALUE_HALL_ANGCORR_GAIN_REV_DEGPHZ    0.0f
#define PUSER_VALUE_HALL_ANGCORR_OFFSET_FWD_DEG     0.0f
#define PUSER_VALUE_HALL_ANGCORR_OFFSET_REV_DEG     0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_1_FWD_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_1_REV_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_2_FWD_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_2_REV_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_3_FWD_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_3_REV_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_4_FWD_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_4_REV_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_5_FWD_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_5_REV_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_6_FWD_RAD        0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_6_REV_RAD        0.0f

#define PUSER_VALUE_HALL_HALL2EST_SWITCH_SPEED_HZ   100.0f

#define PUSER_VALUE_HALLTUNE_ISTART_AMPS            0.0f
#define PUSER_VALUE_HALLTUNE_RAMPRATE_HZPSEC        0.0f
#define PUSER_VALUE_HALLTUNE_SPEED_HZ               0.0f
#define PUSER_VALUE_HALL_MECH_ANGLE_OFFSET_DEG      0.00f 
#define PUSER_VALUE_HALL_EDGE_POL_SKEW_TIME         0.00f

#define PUSER_VALUE_QEP_RESOLUTION                  2048
#define PUSER_VALUE_QEP_ANGCORR_OFFSET_FWD_DEG      50.0f
#define PUSER_VALUE_QEP_ANGCORR_OFFSET_REV_DEG      0.0f
#define PUSER_VALUE_QEP_FC_HZ                       70.0f
#define PUSER_VALUE_QEP_ALIGNOFFSET_DEG             0.0f
//#define PUSER_VALUE_QEP_ALIGN_TIME_SEC              1.0f
//#define PUSER_VALUE_QEP_ALIGNGO_ISTART_AMPS         0.0f
//#define PUSER_VALUE_QEP_ALIGNGO_SPDRAMP_HZPSEC      0.0f
//#define PUSER_VALUE_QEP_ALIGNGO_SPDTARGET_HZ        0.0f

#define PUSER_VALUE_POWER_IPHASE_OFSFET             0.0f
#define PUSER_VALUE_POWER_IPHASE_FACTOR             1.0f

#endif      //#ifndef PARAMFILE_H
