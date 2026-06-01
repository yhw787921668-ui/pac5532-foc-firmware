
/**
 * @file sim_h_defs.h
 * @brief Use for SIM Hall States, SIM2CL States and Status definitions
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

#ifndef SIM_H_DEFS_H
#define SIM_H_DEFS_H

// SIM States
#define STATE_SIM_RESET             0
#define STATE_SIM_MDETECT           1
#define STATE_SIM_DETECT2           2
#define STATE_SIM_SIM2CL            3
#define STATE_SIM_WAITSTOP_INIT     4
#define STATE_SIM_WAITSTOP_RUN      5
#define STATE_SIM_BRAKE_INIT        6
#define STATE_SIM_BRAKE_RUN         7
#define STATE_SIM_BRAKE_ENLS        8
#define STATE_SIM_EXIT              9
#define STATE_SIM_COMPLETE          10


// SIM Status (If changed SIM library needs to be compiled!)
#define STATUS_SIM_MONITORING           (1 << 0)
#define STATUS_SIM_MOTIONNOTDETECTED    (1 << 1)
#define STATUS_SIM_MOTIONDETECTED       (1 << 2)
#define STATUS_SIM_WRONGDIRECTION       (1 << 3)

#define STATUS_SIM_SWITCH2CL            (1 << 4)
#define STATUS_SIM_SLOWSPEED            (1 << 5)
#define STATUS_SIM_WAITSTOP             (1 << 6)
#define STATUS_SIM_WAITSTOPCOMPLETE     (1 << 7)

#define STATUS_SIM_BRAKING              (1 << 8)
#define STATUS_SIM_BRAKECOMPLETE        (1 << 9)
#define STATUS_SIM_ABOVESIM2CL          (1 << 10)
#define STATUS_SIM_TIMEOUT              (1 << 11)

#define STATUS_SIM_COMPLETE             (1 << 12)

#endif  //#ifndef SIM_H_DEFS_H
