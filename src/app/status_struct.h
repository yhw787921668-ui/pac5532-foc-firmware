
/**
 * @file state_struct.h
 * @brief Use to define status for Motor, Prestart and Struct_Status
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

#ifndef STATUS_H
#define STATUS_H

#include "maths.h"


typedef enum
{
    STATUS_MOTOR_UNUSED_0       = BIT_00,           // 0 :
    STATUS_MOTOR_ALIGN          = BIT_01,           // 1 : FOC in align
    STATUS_MOTOR_UNUSED_2       = BIT_02,           // 2 :
    STATUS_MOTOR_UNUSED_3       = BIT_03,           // 3 :
    STATUS_MOTOR_UNUSED_4       = BIT_04,           // 4 :
    STATUS_MOTOR_UNUSED_5       = BIT_05,           // 5 :
    STATUS_MOTOR_UNUSED_6       = BIT_06,           // 6 :
    STATUS_MOTOR_UNUSED_7       = BIT_07,           // 7 :
    STATUS_MOTOR_UNUSED_8       = BIT_08,           // 8 :
    STATUS_MOTOR_UNUSED_9       = BIT_09,           // 9 :
    STATUS_MOTOR_UNUSED_10      = BIT_10,           // 10 :
    STATUS_MOTOR_MPOSMODE_TRAP  = BIT_11,           // 11 : MPOS operating in Trap mode
    STATUS_MOTOR_SIMBEMF_NODET  = BIT_12,           // 12 : SIM BEMF signal not detected
    STATUS_MOTOR_SIMBEMF_DET    = BIT_13,           // 13 : SIM BEMF signal detected
    STATUS_MOTOR_WAITSTOP       = BIT_14,           // 14 : Waiting for motor to stop
    STATUS_MOTOR_BRAKING        = BIT_15,           // 15 : Motor is braking
    STATUS_MOTOR_STOPPED        = BIT_16,           // 16 : Motor is stopped
    STATUS_MOTOR_ENABLED        = BIT_17,           // 17 : Motor is enabled
    STATUS_MOTOR_OPEN_LOOP      = BIT_18,           // 18 : FOC in open loop mode
    STATUS_MOTOR_OL_RAMPUP      = BIT_19,           // 19 : FOC in open loop speed ramp
    STATUS_MOTOR_CL_RAMPDOWN    = BIT_20,           // 20 : FOC in closed loop speed ramp down
    STATUS_MOTOR_CLOSED_LOOP    = BIT_21,           // 21 : FOC in closed loop mode
    STATUS_MOTOR_REVERSE        = BIT_22,           // 22 : Motor in reverse direction mode
    STATUS_MOTOR_FW_ACTIVE      = BIT_23,           // 23 : Motor in Field Weakening range
    STATUS_MOTOR_OVERMOD        = BIT_24,           // 24 : Motor operating in Over-Modulation region
    STATUS_MOTOR_MPOSMODE_HALL  = BIT_25,           // 25 : MPOS operating in Hall Sensor mode
    STATUS_MOTOR_MPOSMODE_EST   = BIT_26,           // 26 : MPOS operating in Estimator mode
    STATUS_FLASH_OPERATION_COMP = BIT_27,           // 27 : Set after FLASH Erase, Read, Write operation 
    STATUS_QEP_LINEAR_COMPLETE  = BIT_28,           // 28 : Set the flag once QEP linearization done
    STATUS_MOTOR_MPOSMODE_QEP   = BIT_29,           // 29 : MPOS operating in QEP mode
    FLAG_MPOS_UPDATE_CALC       = BIT_30            // 30 : Flag MPOS: Update MPOS Values
} eStatusMotor;

#define STATUS_MAIN_ALLSTATUS (\
        STATUS_MOTOR_ENABLED \
        | STATUS_MOTOR_OL_RAMPUP \
        | STATUS_MOTOR_CL_RAMPDOWN \
        | STATUS_MOTOR_OPEN_LOOP \
        | STATUS_MOTOR_CLOSED_LOOP \
        | STATUS_MOTOR_FW_ACTIVE \
        | STATUS_MOTOR_ALIGN \
        | STATUS_MOTOR_STOPPED \
        | STATUS_MOTOR_MPOSMODE_HALL \
        | STATUS_MOTOR_MPOSMODE_EST \
        | STATUS_MOTOR_MPOSMODE_QEP \
        )

//========================================================================
// Status bits that are cleared on status read from GUI
//========================================================================
#define STATUS_MOTOR_GRP_CLEARREAD (\
        STATUS_MOTOR_OVERMOD \
        | FLAG_MPOS_UPDATE_CALC \
        | STATUS_FLASH_OPERATION_COMP\
        | STATUS_QEP_LINEAR_COMPLETE\
        )



typedef enum
{
    STATUS_PRESTART_RUNNING = 0,         // 0: Motor Startup Start In Motion Detection
    STATUS_PRESTART_COMPLETED,           // 1: Motor Startup Start In Motion Detection
    STATUS_PRESTART_FAILED,              // 2: Motor Startup Initial Position Detection
} eStatusPrestart;


typedef struct
{
    eStatusMotor motor;
    eStatusPrestart prestart;
} Struct_Status;

#endif  //#ifndef STATUS_H
