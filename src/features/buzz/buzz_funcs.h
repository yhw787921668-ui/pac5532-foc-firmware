
/**
 * @file buzz_funcs.h
 * @brief Buzz function declarations and parameter definitions
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

#ifndef BUZZ_FUNCS_H
#define BUZZ_FUNCS_H

#include "motor_struct.h"


#if defined(ENABLE_BUZZ)

extern const melody melody1;
extern const melody melody2;
extern const melody melody3;
extern const melody melody4;
extern const melody melody5;
extern const melody melody6;
extern const melody melody7;
extern const melody melody8;


#define BUZZ_VOLUME_Q08             ((uint8_t) QCONST(0.4f, Q08))
#define MAX_MELODIES                ((uint8_t) 8U)
#ifdef ENABLE_ANGLEDIFF_IDREF_RAMPING_2MS
    #define BUZZ_TIMESTEP_Q16           DT_SYSTICK_Q16
#else
    #define BUZZ_TIMESTEP_Q16           DT_DIVSYSTICK_Q16
#endif
#define BUZZER_IQ_RAMPRATE          52              // Ramp rate for buzz current. Units are scaled current ramp increments per control period.


//////////////////////////////
// FUNCTION DECLARATIONS    //
//////////////////////////////
void init_buzz(Struct_Motor* motor_ptr);
void command_buzz(const melody *ptr_melody, uint8_t cmd_contmode, uint8_t cmd_volume, Struct_Motor* motor_ptr);
void statemachine_buzz(Struct_Motor* motor_ptr);


//////////////////////////////
// DEFINITIONS              //
//////////////////////////////

// Buzzer States
#define STATE_BUZZ_IDLE             0
#define STATE_BUZZ_START            1
#define STATE_BUZZ_CHANGENOTE       2
#define STATE_BUZZ_PLAYNOTE         3
#define STATE_BUZZ_STOP             4
#define STATE_BUZZ_DONE             5

//Buzzer Frequency Definitions
#define BUZZFREQ_0100        628                                // 100Hz in rad/s
#define BUZZFREQ_0800        (8 * BUZZFREQ_0100)                // 800Hz in rad/s
#define BUZZFREQ_0900        (9 * BUZZFREQ_0100)                // 900Hz in rad/s
#define BUZZFREQ_1000        (10 * BUZZFREQ_0100)            // 1000Hz in rad/s
#define BUZZFREQ_1100        (11 * BUZZFREQ_0100)            // 1100Hz in rad/s
#define BUZZFREQ_1200        (12 * BUZZFREQ_0100)            // 1200Hz in rad/s
#define BUZZFREQ_1300        (13 * BUZZFREQ_0100)            // 1300Hz in rad/s
#define BUZZFREQ_1400        (14 * BUZZFREQ_0100)            // 1400Hz in rad/s
#define BUZZFREQ_1500        (15 * BUZZFREQ_0100)            // 1500Hz in rad/s
#define BUZZFREQ_1600        (16 * BUZZFREQ_0100)            // 1600Hz in rad/s

//Buzzer Frequency Definitions: Musical Notes
// Hz * 2pi = rad/s (q0 * f16 = f16>>16=q0)
#define PAUSE               0
#define BUZZFREQ_NOTE_5C    ((523 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5Cs    ((554 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5D    ((587 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5Eb    ((622 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5E    ((659 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5F    ((699 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5Fs    ((740 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5G    ((784 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5Gs    ((831 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5A    ((880 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5Bb    ((932 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_5B    ((988 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6C    ((1047 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6Cs    ((1109 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6D    ((1175 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6Eb    ((1245 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6E    ((1319 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6F    ((1397 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6Fs    ((1480 * FIX16_2PI) / 65536)
#define BUZZFREQ_NOTE_6G    ((1568 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_6Gs    ((1661 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_6A    ((1760 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_6Bb    ((1865 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_6B    ((1976 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_7C    ((2093 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_7Cs    ((2217 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_7D    ((2349 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_7Eb    ((2489 * FIX16_2PI) / 65536)
//#define BUZZFREQ_NOTE_7E    ((2637 * FIX16_2PI) / 65536)

#endif  //#if defined(ENABLE_BUZZ)
#endif  //BUZZ_FUNCS_H
