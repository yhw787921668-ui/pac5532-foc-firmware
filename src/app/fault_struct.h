
/**
 * @file fault_struct.h
 * @brief Use to define fault type, status, warning, bits
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

#ifndef FAULT_STRUCT_H
#define FAULT_STRUCT_H

#include "maths.h"


typedef enum
{
    WARNING_MAIN_OC_VALUE       = BIT_00,           // 00 : OC limit higher than measurable current
    WARNING_MTPA_PARAM_INVAILD  = BIT_01,           // 01 : WARNING_CBC_OC              = BIT_01
    WARNING_PWM_SATURATED       = BIT_02,           // 02 : PWM duty cycle is saturated
    WARNING_STACK_OOR           = BIT_03,           // 03 : Stack overflow or underflow
    WARNING_OLSPEED_MIN         = BIT_04,           // 04 : Open loop speed is lower than closed loop minimum speed limit
    WARNING_ANGLDIF             = BIT_05,           // 05 : Estimator AngleDiff out of range
    WARNING_FOCCTRL_OVERFLOW    = BIT_06,           // 06 : FOC control time has been exceeded
    WARNING_OU_VOLTAGE          = BIT_07,           // 07 : Bus Voltage Min/Max limit exceeded
    WARNING_PIGAIN_RANGE        = BIT_08,           // 08 : PI controller gain out of range
    WARNING_STALLFILTGAIN       = BIT_09,           // 09 : Stall filter value floored at minimum(1)
    WARNING_SPDDIF              = BIT_10,           // 10 : Estimator SpeedDiff out of range
    WARNING_ATUNE_CHECK_STATUS  = BIT_11,           // 11 : ATPI Warning -> Check ATPI Status
    WARNING_ESTBW_HIGH          = BIT_12,           // 12 : Ratio of control frequency to estimator bandwidth is higher than 10:1
    WARN_HALL_TIM_PERD_INVALID  = BIT_13,           // 13 : Hall Timer Period Invalid

    FAULT_COAST_TIMEOUT         = BIT_14,           // 14 : Motor Coast timeout
    FAULT_SPDANGLDIF_TIMEOUT    = BIT_15,           // 15 : EstSpeed/AngleDiff timeout
    FAULT_CAFE_OC               = BIT_16,           // 16 : Motor over-current
    FAULT_OU_VOLTAGE            = BIT_17,           // 17 : Motor disabled due to bus voltage min/max fault
    FAULT_MIN_MAX_SPEED         = BIT_18,           // 18 : Motor Min/Max Speed
    FAULT_OPD                   = BIT_19,           // 19 : Motor Open Phase Detection
    FAULT_UNUSED_20             = BIT_20,           // 20 :
    FAULT_PARAM_WHILE_RUN       = BIT_21,           // 21 : Attempt to change critical parameters while motor is running
    FAULT_UNUSED_22             = BIT_22,           // 22 :
    FAULT_MOTOR_STALLED         = BIT_23,           // 23 : Motor Stall Detection
    FAULT_PPM_PULSE_TIMEOUT     = BIT_24,           // 24 : PPM pulse timeout, no received valid PPM pulse
    FAULT_UNUSED_25             = BIT_25,           // 25 :
    FAULT_MOTOR_HALL_INVSTATE   = BIT_26,           // 26 : Hall Angle Sequence or State is Invalid
    FAULT_EST_INVALID_INPUT     = BIT_27,           // 27 : Estimator inputs are invalid
    FAULT_HALL_TIMEOUT          = BIT_28,           // 28 : Hall timer expired without detecting a hall transition
    FAULT_TERMINAL              = BIT_29,           // 29 : Use to show debug related faults
    WARNING_KE_VALUE            = BIT_30,           // 30 : If KE value is set to 0, need to config KE correctly
} eFaultMotor;


typedef enum
{
    FAULT_TERMINAL_DEVID            = BIT_00,           // 00 : Device ID does not match selected device
    FAULT_TERMINAL_ADCCAL_FACTORY   = BIT_01,           // 01 : Factory ADC calibration parameters out of range
    FAULT_TERMINAL_ADCCAL_POWERUP   = BIT_02,           // 02 : Powerup ADC calibration out of range
    FAULT_TERMINAL_AFEINIT          = BIT_03,           // 03 : AFE not initialized properly
    FAULT_TERMINAL_FLASH_CRC_TEST   = BIT_04,           // 04 : Flash CRC Test Status Failed = 1
    FAULT_TERMINAL_UNUSED_5         = BIT_05,           // 05 :
    FAULT_TERMINAL_UNUSED_6         = BIT_06,           // 06 :
    FAULT_TERMINAL_UNUSED_7         = BIT_07,           // 07 :
    FAULT_TERMINAL_UNUSED_8         = BIT_08,           // 08 :
    FAULT_TERMINAL_UNUSED_9         = BIT_09,           // 09 :
    FAULT_TERMINAL_UNUSED_10        = BIT_10,           // 10 :
    FAULT_TERMINAL_UNUSED_11        = BIT_11,           // 11 :
    FAULT_TERMINAL_UNUSED_12        = BIT_12,           // 12 :
    FAULT_TERMINAL_UNUSED_13        = BIT_13,           // 13 :
    FAULT_TERMINAL_UNUSED_14        = BIT_14,           // 14 :
    FAULT_TERMINAL_UNUSED_15        = BIT_15,           // 15 :
    FAULT_TERMINAL_UNUSED_16        = BIT_16,           // 16 :
    FAULT_TERMINAL_UNUSED_17        = BIT_17,           // 17 :
    FAULT_TERMINAL_UNUSED_18        = BIT_18,           // 18 :
    FAULT_TERMINAL_UNUSED_19        = BIT_19,           // 19 :
    FAULT_TERMINAL_UNUSED_20        = BIT_20,           // 20 :
    FAULT_TERMINAL_UNUSED_21        = BIT_21,           // 21 :
    FAULT_TERMINAL_UNUSED_22        = BIT_22,           // 22 :
    FAULT_TERMINAL_UNUSED_23        = BIT_23,           // 23 :
    FAULT_TERMINAL_UNUSED_24        = BIT_24,           // 24 :
    FAULT_TERMINAL_UNUSED_25        = BIT_25,           // 25 :
    FAULT_TERMINAL_UNUSED_26        = BIT_26,           // 26 :
    FAULT_TERMINAL_UNUSED_27        = BIT_27,           // 27 :
    FAULT_TERMINAL_UNUSED_28        = BIT_28,           // 28 :
    FAULT_TERMINAL_UNUSED_29        = BIT_29,           // 29 :
    FAULT_TERMINAL_UNUSED_30        = BIT_30            // 30 :
} eFaultTerminal;


typedef struct
{
    eFaultMotor motor;                           // Fault and Warning mask for this motor
    eFaultTerminal fault_terminal;
    uint8_t fault_stop_flag : 1;
    uint8_t fault_state_flag : 1;
    uint8_t fault_terminal_flag : 1;
    uint8_t temp : 5;
} Struct_Fault;


//============================================================================================
//
// Groupings for Clearing Method
//
//============================================================================================

//========================================================================
// Faults and warnings that clear automatically when motor is started
//========================================================================
#define FAULT_MAIN_GRP_CLEARSTART (\
        FAULT_SPDANGLDIF_TIMEOUT \
        | FAULT_MIN_MAX_SPEED \
        | FAULT_OPD \
        | FAULT_MOTOR_STALLED \
        | FAULT_COAST_TIMEOUT \
        | WARNING_ANGLDIF \
        | WARNING_SPDDIF \
        | WARNING_FOCCTRL_OVERFLOW \
        | WARNING_PWM_SATURATED \
        | FAULT_OU_VOLTAGE \
        | FAULT_PPM_PULSE_TIMEOUT \
        | FAULT_MOTOR_HALL_INVSTATE \
        | FAULT_EST_INVALID_INPUT \
        | FAULT_HALL_TIMEOUT \
        )

//========================================================================
// Faults and warnings that can be cleared on fault clear command from GUI
//========================================================================
#define FAULT_MAIN_GRP_CLEARCMD (\
        FAULT_PARAM_WHILE_RUN \
        | WARNING_STACK_OOR \
        | FAULT_MAIN_GRP_CLEARSTART\
        | WARNING_PIGAIN_RANGE \
        | WARNING_STALLFILTGAIN \
        | FAULT_MOTOR_HALL_INVSTATE \
        | FAULT_EST_INVALID_INPUT \
        | FAULT_HALL_TIMEOUT \
        | WARN_HALL_TIM_PERD_INVALID \
        )

//========================================================================
// Faults and warnings are cleared on warning read from GUI
//========================================================================
#define FAULT_MOTOR_GRP_CLEARREAD (\
        WARNING_FOCCTRL_OVERFLOW \
        | WARNING_PWM_SATURATED \
        )

//============================================================================================
//
// Groupings for FW Response
//
//============================================================================================

//========================================================================
// Faults that generate a "Fault Stop" condition
//========================================================================
#define FAULT_MAIN_GRP_STOP (\
        FAULT_SPDANGLDIF_TIMEOUT \
        | FAULT_OPD \
        | FAULT_OU_VOLTAGE \
        | FAULT_MOTOR_STALLED \
        | FAULT_COAST_TIMEOUT \
        | FAULT_MIN_MAX_SPEED \
        | FAULT_PPM_PULSE_TIMEOUT\
        | FAULT_MOTOR_HALL_INVSTATE\
        | FAULT_EST_INVALID_INPUT \
        | FAULT_HALL_TIMEOUT \
        )

//========================================================================
// Faults that generate a "Fault State" condition
//========================================================================
#define FAULT_MAIN_GRP_STATE (\
        FAULT_CAFE_OC \
        )

//========================================================================
// Faults that generate a "Terminal" condition
//========================================================================
#define FAULT_MAIN_GRP_TERMINAL (\
        FAULT_TERMINAL \
        )

#endif  //#ifndef FAULT_STRUCT_H
