
/**
 * @file state_struct.h
 * @brief Use to define structure and states for AppMain, Post, ADCCal, PreStart, AppControl, Control, Struct_State
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

#ifndef STATES_H
#define STATES_H


typedef enum
{
    STATE_MAIN_RESET = 0,                       // 0: Power On Reset
    STATE_MAIN_POST,                            // 1: use for self test, like ADC path offset, motor winding test, etc
    STATE_MAIN_DISABLE,                         // 2: Motor State Stopping, its a transit state
    STATE_MAIN_BRAKE,                           // 3: Motor Braking State when motor is disabled
    STATE_MAIN_STOPPED,                         // 4: Motor Stop State
    STATE_MAIN_PRESTART,                        // 5: Motor Start Up
    STATE_MAIN_STARTUP,                         // 6: Motor Running
    STATE_MAIN_RUN,                             // 7: Motor Running
    STATE_MAIN_COASTING,                        // 8: Motor Close Loop Coasting Mode
    STATE_MAIN_FAULT,                           // 9: Fault state
    STATE_MAIN_TERMINAL,                        // 10: Terminal State
    STATE_MAIN_HALLTUNE,                        // 11: Hall Tuning Mode
    STATE_MAIN_MID1,                            // 12: MID1
    STATE_MAIN_FORCEANG,                        // 13: Forced angle drive mode
    STATE_MAIN_FLASHRW,                         // 14: FlashRW States
    STATE_MAIN_BUZZ,                            // 15: Buzz State
    STATE_MAIN_END = STATE_MAIN_BUZZ            // Use to check the range of enumeration
} eStateAppMain;

typedef enum
{
    STATE_POST_RESET = 0,                       // 0: Reset state
    STATE_POST_DEVID,                           // 1: Device ID check
    STATE_POST_ADCCAL_FACTORY,                  // 2: ADC Factory Calibration
    STATE_POST_ADCCAL_START,                    // 3: ADC Powerup Calibration start state
    STATE_POST_ADCCAL_RUN,                      // 4: ADC current path offset calculation
    STATE_POST_COMPLETE,                        // 5: Self test complete
    STATE_POST_END = STATE_POST_COMPLETE        // Use to check the range of enumeration
} eStatePost;

typedef enum
{
    STATE_ADCCAL_RESET = 0,                     // 0
    STATE_ADCCAL_SAMPLEDELAY,                   // 1
    STATE_ADCCAL_SAMPLEPROCESS,                 // 2
    STATE_ADCCAL_COMPLETE,                      // 3
    STATE_ADCCAL_END = STATE_ADCCAL_COMPLETE    // 4
} eStateADCCal;

typedef enum
{
    STATE_PRESTART_RESET = 0,                    // 0: Motor Startup Start In Motion Detection
    STATE_PRESTART_SIM_RUN,                      // 1: Motor Startup Start In Motion Detection
    STATE_PRESTART_IPD_INIT,                     // 2: Motor Startup Initial Position Detection
    STATE_PRESTART_IPD_RUN,                      // 3: Motor Startup Initial Position Detection
    STATE_PRESTART_COMPLETE,                     // 4: Motor Startup Initial Position Detection
} eStatePreStart;

typedef enum
{
    STATE_APPCONTROL_IDLE = 0,                  // 0: Control Stop State
    STATE_APPCONTROL_ADCCAL,                    // 1: ADC Calibration Control Mode
    STATE_APPCONTROL_SIM_RUN,                   // 2: SIM Control Mode
    STATE_APPCONTROL_IPD,                       // 3: Control Initial Position Detection
    STATE_APPCONTROL_MOTOR_RUN,                 // 4: Motor Run Control Mode
    STATE_APPCONTROL_END = STATE_APPCONTROL_MOTOR_RUN  // Use to check the range of enumeration
} eStateAppControl;

typedef enum
{
    STATE_CONTROL_IDLE = 0,                     // 0: Idle
    STATE_CONTROL_OUTERLOOP,                    // 1: Closed loop speed mode
    STATE_CONTROL_TORQUE                        // 2: Closed loop torque mode
} eStateControl;

typedef enum
{
    STATE_PRECONTROL_IDLE   = 0,                // 0:
    STATE_PRECONTROL_SIM,                       // 1:
    STATE_PRECONTROL_IPD,                       // 2:
    STATE_PRECONTROL_FOC,                       // 3:
    STATE_PRECONTROL_END    = STATE_PRECONTROL_FOC  // Use to check the range of enumeration
} eStatePreControl;

typedef struct
{
    eStateAppMain main;
    eStatePreStart prestart;
    eStateAppControl appcontrol;
    eStatePreControl precontrol;
    eStateControl control;
    eStatePost post;
    eStateADCCal adccal;

//#if defined(ENABLE_BRAKE_WHILE_STOPPED) // This #define is removed becauses of Lib dependency
    int32_t brake_stop_mode;
//#endif
} Struct_State;

#endif  //#ifndef STATES_H
