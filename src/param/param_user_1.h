
/**
 * @file param_user_1.h
 * @brief Use for parameter processing
 */

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef PARAM_USER_1_H
#define PARAM_USER_1_H

//===========================
// PARAMETER DEFINITIONS
//===========================
#if 0
#define PUSER_VALUE_PWM_FREQ_HZ                 20000
#define PUSER_VALUE_CONTROL_FREQ_DIV            1
#define PUSER_VALUE_DIFFAMP_GAIN                4
#define PUSER_VALUE_LD_UH                       21.99411
#define PUSER_VALUE_LQ_UH                       32.99117
#define PUSER_VALUE_R_OHMS                      0.0852f
#define PUSER_VALUE_POLE_PAIRS                  6
#define PUSER_VALUE_OC_DAC                      0
#define PUSER_VALUE_AUTOTUNE_KE                 0.0010f
#define PUSER_VALUE_AUTOTUNE_J                  0.00001f
#define PUSER_VALUE_NOMINAL_SPEED               600.00f
#define PUSER_VALUE_SPEED_MAX_HZ                1000.00f
#define PUSER_VALUE_POWER_MAX_W                 0.00f
#define PUSER_VALUE_IQ_MAX_A                    15.00f
#define PUSER_VALUE_IQ_MIN_A                    0.00f
#define PUSER_VALUE_OC_LIMIT_A                  25.00f
#define PUSER_VALUE_CL_COAST_TIMEOUT_SEC        10.00f
#define PUSER_VALUE_ALIGN_TIME_SEC              0.10f
#define PUSER_VALUE_ID_REF_OL_A                 0.00f
#define PUSER_VALUE_ITOTAL_REF_OL_A             4.00f
#define PUSER_VALUE_SPEED_RAMP_OL_HZPSEC        5.96f
#define PUSER_VALUE_SPEED_SETPOINT_OL_HZ        100.00f
#define PUSER_VALUE_AUTO_CLOSE_LOOP             1
#define PUSER_VALUE_PI_ID_KP                    0.1036f
#define PUSER_VALUE_PI_ID_KI                    3872.5940f
#define PUSER_VALUE_PI_ID_MIN                   -0.9200f
#define PUSER_VALUE_PI_ID_MAX                   0.9200f
#define PUSER_VALUE_FC_ID_HZ                    100
#define PUSER_VALUE_PI_IQ_KP                    0.1555f
#define PUSER_VALUE_PI_IQ_KI                    2581.7340f
#define PUSER_VALUE_PI_IQ_MIN                   0.0000f
#define PUSER_VALUE_PI_IQ_MAX                   1.0000f
#define PUSER_VALUE_FC_IQ_HZ                    100
#define PUSER_VALUE_IDIQ_RAMP_APSEC             1000.00f
#define PUSER_VALUE_PI_SPEED_KP                 0.0045f
#define PUSER_VALUE_PI_SPEED_KI                 4.7148f
#define PUSER_VALUE_SPEED_RAMP_HZPSEC           100.00f
#define PUSER_VALUE_PI_POWER_KP                 0.0000f
#define PUSER_VALUE_PI_POWER_KI                 0.0000f
#define PUSER_VALUE_POWER_RAMP_WPSEC            0.00f
#define PUSER_VALUE_CL_MODE                     1
#define PUSER_VALUE_REVERSE                     0
#define PUSER_VALUE_SPEED_SETPOINT_CL_HZ        200.00f
#define PUSER_VALUE_POWER_SETPOINT_CL_W         0.00f
#define PUSER_VALUE_ID_REF_CL_A                 0.00f
#define PUSER_VALUE_IQ_REF_CL_A                 0.30f
#define PUSER_VALUE_LBG1_KPE                    0.0937f
#define PUSER_VALUE_LBG1_KPI                    0.4039f
#define PUSER_VALUE_LBG1_BANDWIDTH              7539.8f
#define PUSER_VALUE_LBG1_DAMPINGFACTOR          0.7071f
#define PUSER_VALUE_LBG1_FC_SPEED_HZ            75
#define PUSER_VALUE_LBG1_ANGLE_OFFSET_DEG       0.00f
#define PUSER_VALUE_LBG1_ANGLE_FACTOR_DEGPHZ    0.0000f
#define PUSER_VALUE_LBG2_KPI                    0.0000f
#define PUSER_VALUE_LBG2_KPE                    0.0000f
#define PUSER_VALUE_LBG2_FC_SPEED_HZ            750
#define PUSER_VALUE_LBG2_ANGLE_OFFSET_DEG       0.00f
#define PUSER_VALUE_LBG2_ANGLE_FACTOR_DEGPHZ    0.0000f
#define PUSER_VALUE_SIM_ALIGN_SEC               0.00f
#define PUSER_VALUE_SIM_IDIQ_GAIN               1.00f
#define PUSER_VALUE_SIM_BTO_SEC                 20.00f
#define PUSER_VALUE_SIM_AMP_GAIN                1
#define PUSER_VALUE_SIM_CURRENT_TRACK_GAIN      1.0000f
#define PUSER_VALUE_STALL_KE                    0.00000f
#define PUSER_VALUE_STALL_WINDOW_SEC            0.00f
#define PUSER_VALUE_IPD_CHARGE                  500
#define PUSER_VALUE_IPD_DISCHARGE               80
#define PUSER_VALUE_IPD_PERIODS                 1
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
#define PUSER_VALUE_HALL_ISTART_AMPS            0.0000f
#define PUSER_VALUE_HALL_ANGCORR_GAIN_FWD_DEGPHZ 0.000f
#define PUSER_VALUE_HALL_ANGCORR_GAIN_REV_DEGPHZ 0.000f
#define PUSER_VALUE_HALL_ANGCORR_OFFSET_FWD_DEG 0.0f
#define PUSER_VALUE_HALL_ANGCORR_OFFSET_REV_DEG 0.0f
#define PUSER_VALUE_HALL_SNAPANGLE_1_FWD_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_1_REV_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_2_FWD_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_2_REV_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_3_FWD_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_3_REV_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_4_FWD_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_4_REV_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_5_FWD_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_5_REV_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_6_FWD_RAD    0.000f
#define PUSER_VALUE_HALL_SNAPANGLE_6_REV_RAD    0.000f
#define PUSER_VALUE_HALL_HALL2EST_SWITCH_SPEED_HZ 0.0000f
#define PUSER_VALUE_HALLTUNE_ISTART_AMPS        0.050f
#define PUSER_VALUE_HALLTUNE_RAMPRATE_HZPSEC    0.050f
#define PUSER_VALUE_HALLTUNE_SPEED_HZ           0.050f
#define PUSER_VALUE_HALL_MECH_ANGLE_OFFSET_DEG  0.00f 
#define PUSER_VALUE_HALL_EDGE_POL_SKEW_TIME     0.00f
#define PUSER_VALUE_QEP_RESOLUTION              0.05
#define PUSER_VALUE_QEP_FC_HZ                   0.05
#define PUSER_VALUE_QEP_ANGCORR_OFFSET_FWD_DEG  0.05
#define PUSER_VALUE_QEP_ANGCORR_OFFSET_REV_DEG  0.05
#endif

#define PUSER_VALUE_PWM_FREQ_HZ                 30000       //用户设定 by:owz
#define PUSER_VALUE_CONTROL_FREQ_DIV            2           //用户设定 by:owz
#define PUSER_VALUE_DIFFAMP_GAIN                2           //用户设定 by:owz
#define PUSER_VALUE_LD_UH                       134         //按GUI自动标定值修改 by:owz
#define PUSER_VALUE_LQ_UH                       221         //按GUI自动标定值修改 by:owz
#define PUSER_VALUE_R_OHMS                      0.1127f     //按GUI自动标定值修改 by:owz
#define PUSER_VALUE_POLE_PAIRS                  8           //用户设定 by:owz
#define PUSER_VALUE_OC_DAC                      0 
#define PUSER_VALUE_AUTOTUNE_KE                 0.0008f 
#define PUSER_VALUE_AUTOTUNE_J                  0.00001f 
#define PUSER_VALUE_NOMINAL_SPEED               1900.00f 
#define PUSER_VALUE_SPEED_MAX_HZ                4500.00f    //用户设定 by:owz
#define PUSER_VALUE_POWER_MAX_W                 400.00f 
#define PUSER_VALUE_IQ_MAX_A                    50.00f      //用户设定 by:owz
#define PUSER_VALUE_IQ_MIN_A                    -50.00f       //用户设定 by:owz
#define PUSER_VALUE_OC_LIMIT_A                  60.00f      //用户设定 by:owz
#define PUSER_VALUE_CL_COAST_TIMEOUT_SEC        1.00f 
#define PUSER_VALUE_ALIGN_TIME_SEC              0.50f 
#define PUSER_VALUE_ID_REF_OL_A                 2.00f 
#define PUSER_VALUE_ITOTAL_REF_OL_A             16.00f 
#define PUSER_VALUE_SPEED_RAMP_OL_HZPSEC        50.00f      //用户设定 by:owz
#define PUSER_VALUE_SPEED_SETPOINT_OL_HZ        200.00f     //用户设定 by:owz
#define PUSER_VALUE_AUTO_CLOSE_LOOP             1 
#define PUSER_VALUE_PI_ID_KP                    0.6314f     //按GUI自动标定值修改 by:owz
#define PUSER_VALUE_PI_ID_KI                    840.9844f   //按GUI自动标定值修改 by:owz  840.9844f
#define PUSER_VALUE_PI_ID_MIN                   -0.9200f 
#define PUSER_VALUE_PI_ID_MAX                   0.9200f 
#define PUSER_VALUE_FC_ID_HZ                    100 
#define PUSER_VALUE_PI_IQ_KP                    1.0414f     //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_PI_IQ_KI                    509.8906f   //按GUI自动标定值修改 by:owz  509.8906f
#define PUSER_VALUE_PI_IQ_MIN                   0.0000f 
#define PUSER_VALUE_PI_IQ_MAX                   1.0000f 
#define PUSER_VALUE_FC_IQ_HZ                    100 
#define PUSER_VALUE_IDIQ_RAMP_APSEC             1.00f        //用户设定,初始值，后续由模型计算 by:owz
#define PUSER_VALUE_PI_SPEED_KP                 0.1000f 
#define PUSER_VALUE_PI_SPEED_KI                 1.0000f 
#define PUSER_VALUE_SPEED_RAMP_HZPSEC           20000.00f 
#define PUSER_VALUE_PI_POWER_KP                 5.0000f 
#define PUSER_VALUE_PI_POWER_KI                 50.0000f 
#define PUSER_VALUE_POWER_RAMP_WPSEC            500.00f 
#define PUSER_VALUE_CL_MODE                     0           //控制模式：速度模式，转矩模式，功率模式，用户设定 by:owz
#define PUSER_VALUE_REVERSE                     0           //正反转设定 by:owz
#define PUSER_VALUE_SPEED_SETPOINT_CL_HZ        1500.00f 
#define PUSER_VALUE_POWER_SETPOINT_CL_W         20.00f 
#define PUSER_VALUE_ID_REF_CL_A                 0.00f 
#define PUSER_VALUE_IQ_REF_CL_A                 5.00f       //用户设定 by:owz
#define PUSER_VALUE_LBG1_KPE                    0.1658f 
#define PUSER_VALUE_LBG1_KPI                    0.7217f 
#define PUSER_VALUE_LBG1_BANDWIDTH              12566.4f 
#define PUSER_VALUE_LBG1_DAMPINGFACTOR          0.7070f 
#define PUSER_VALUE_LBG1_FC_SPEED_HZ            70 
#define PUSER_VALUE_LBG1_ANGLE_OFFSET_DEG       10.40f 
#define PUSER_VALUE_LBG1_ANGLE_FACTOR_DEGPHZ    -0.0269f
#define PUSER_VALUE_LBG1_FC_BEMF_MAG_HZ         100
#define PUSER_VALUE_LBG2_KPI                    0.0000f 
#define PUSER_VALUE_LBG2_KPE                    0.0000f 
#define PUSER_VALUE_LBG2_FC_SPEED_HZ            750 
#define PUSER_VALUE_LBG2_ANGLE_OFFSET_DEG       0.00f 
#define PUSER_VALUE_LBG2_ANGLE_FACTOR_DEGPHZ    0.0000f 
#define PUSER_VALUE_SIM_ALIGN_SEC               1.00f 
#define PUSER_VALUE_SIM_IDIQ_GAIN               1.00f 
#define PUSER_VALUE_SIM_BTO_SEC                 20.00f 
#define PUSER_VALUE_SIM_AMP_GAIN                2 
#define PUSER_VALUE_SIM_CURRENT_TRACK_GAIN      8.0000f 
#define PUSER_VALUE_STALL_KE                    0.00300f 
#define PUSER_VALUE_STALL_WINDOW_SEC            0.50f 
#define PUSER_VALUE_IPD_CHARGE                  500 
#define PUSER_VALUE_IPD_DISCHARGE               60 
#define PUSER_VALUE_IPD_PERIODS                 1 
#define PUSER_VALUE_IPD_OFFSET_FORWARD_DEG      0 
#define PUSER_VALUE_IPD_OFFSET_REVERSE_DEG      0 
#define PUSER_VALUE_FW_ID                       0.00f 
#define PUSER_VALUE_FW_SPEED                    5000.00f 
#define PUSER_VALUE_FW_VDRIVE                   100
#define PUSER_VALUE_FW_PID_KP                   0.1f
#define PUSER_VALUE_FW_PID_KI                   1.0f
#define PUSER_VALUE_FW_VMAX_SCL_FACTOR          1.0f
#define PUSER_VALUE_MISC_FIX16_1                1.0000f 
#define PUSER_VALUE_MISC_FIX16_2                0.0000f 
#define PUSER_VALUE_MISC_FIX16_3                0.0000f 
#define PUSER_VALUE_MTPA_DEGPAMP_FIX16          0.0000f 
#define PUSER_VALUE_MISC_UINT32_1               1 
#define PUSER_VALUE_MISC_UINT32_2               1 
#define PUSER_VALUE_MISC_UINT32_3               0 
#define PUSER_VALUE_HALL_ISTART_AMPS            5.0000f 
#define PUSER_VALUE_HALL_ANGCORR_GAIN_FWD_DEGPHZ 0.000f 
#define PUSER_VALUE_HALL_ANGCORR_GAIN_REV_DEGPHZ 0.000f 
#define PUSER_VALUE_HALL_ANGCORR_OFFSET_FWD_DEG -30.0f      //用户设定 by:owz  -30.0f 
#define PUSER_VALUE_HALL_ANGCORR_OFFSET_REV_DEG -30.0f 
#define PUSER_VALUE_HALL_SNAPANGLE_1_FWD_RAD    1.579f      //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_HALL_SNAPANGLE_1_REV_RAD    -1.692f
#define PUSER_VALUE_HALL_SNAPANGLE_2_FWD_RAD    -0.523f     //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_HALL_SNAPANGLE_2_REV_RAD    0.400f 
#define PUSER_VALUE_HALL_SNAPANGLE_3_FWD_RAD    0.400f      //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_HALL_SNAPANGLE_3_REV_RAD    -0.523f 
#define PUSER_VALUE_HALL_SNAPANGLE_4_FWD_RAD    -2.609f     //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_HALL_SNAPANGLE_4_REV_RAD    2.427f 
#define PUSER_VALUE_HALL_SNAPANGLE_5_FWD_RAD    2.427f      //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_HALL_SNAPANGLE_5_REV_RAD    -2.609f 
#define PUSER_VALUE_HALL_SNAPANGLE_6_FWD_RAD    -1.692f     //按GUI自动标定值修改 by:owz 
#define PUSER_VALUE_HALL_SNAPANGLE_6_REV_RAD    1.579f 
#define PUSER_VALUE_HALL_HALL2EST_SWITCH_SPEED_HZ 50.0000f 
#define PUSER_VALUE_HALLTUNE_ISTART_AMPS        5.000f 
#define PUSER_VALUE_HALLTUNE_RAMPRATE_HZPSEC    50.000f 
#define PUSER_VALUE_HALLTUNE_SPEED_HZ           40.000f 
#define PUSER_VALUE_HALL_MECH_ANGLE_OFFSET_DEG  0.00f 
#define PUSER_VALUE_HALL_EDGE_POL_SKEW_TIME     0.00f

#define PUSER_VALUE_QEP_RESOLUTION              1024 
#define PUSER_VALUE_QEP_FC_HZ                   70 
#define PUSER_VALUE_QEP_ANGCORR_OFFSET_FWD_DEG  0.05 
#define PUSER_VALUE_QEP_ANGCORR_OFFSET_REV_DEG  0.05 

#define PUSER_VALUE_POWER_IPHASE_OFSFET         0.0f
#define PUSER_VALUE_POWER_IPHASE_FACTOR         1.0f

#endif  // #ifndef PARAM_USER_1_H
