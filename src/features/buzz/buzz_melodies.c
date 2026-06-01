
/**
 * @file buzz_melodies.c
 * @brief Buzz melody definitions
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


#include "buzz_funcs.h"

#if defined(ENABLE_BUZZ)

//////////////////////////////
// MELODY TABLE STRUCTURE   //
//////////////////////////////
/*
FORMAT: {Frequency, Duration(in ms)}

EXAMPLES:
{BUZZFREQ_NOTE_6D, 200}     -> Play music note 6D for 200ms
{PAUSE, 10}                  -> While playing the previous music note, ramps dowm volume to 0 for 10ms)

NOTE: When building a melody table, MELODY_SIZE needs to be defined to the number of melody table entries, including PAUSE entries.
NOTE: First entry in melody table must always be the pause for continuous mode

*/


//////////////////////////////
// Melody Definition #1     //
//////////////////////////////
#define MELODY1_SIZE     4
const tone melody1_notes[MELODY1_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_6D, 225},
    {BUZZFREQ_NOTE_6F, 225},
    {BUZZFREQ_NOTE_6G, 225}
};
const melody melody1 = {MELODY1_SIZE, melody1_notes};


//////////////////////////////
// Melody Definition #2     //
//////////////////////////////
#define MELODY2_SIZE     3
const tone melody2_notes[MELODY2_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_6D, 225},
    {BUZZFREQ_NOTE_6G, 225},
};
const melody melody2 = {MELODY2_SIZE, melody2_notes};


//////////////////////////////
// Melody Definition #3     //
//////////////////////////////
#define MELODY3_SIZE     51
const tone melody3_notes[MELODY3_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_5G, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5G, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5A, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5G, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6C, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5B, 600},
    {PAUSE, 300},

    {BUZZFREQ_NOTE_5G, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5G, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5A, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5G, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6D, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6C, 600},
    {PAUSE, 300},

    {BUZZFREQ_NOTE_5G, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5G, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6G, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6E, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6C, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5B, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_5A, 600},
    {PAUSE, 300},

    {BUZZFREQ_NOTE_6F, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6F, 150},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6E, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6C, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6D, 300},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6C, 600},
    {PAUSE, 300},
};
const melody melody3 = {MELODY3_SIZE, melody3_notes};


//////////////////////////////
// Melody Definition #4     //
//////////////////////////////
#define MELODY4_SIZE    7
const tone melody4_notes[MELODY4_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_5A, 300},
    {BUZZFREQ_NOTE_5F, 150},
    {BUZZFREQ_NOTE_5D, 375},
    {BUZZFREQ_NOTE_5F, 375},
    {BUZZFREQ_NOTE_5A, 375},
    {BUZZFREQ_NOTE_6D, 600},
};
const melody melody4 = {MELODY4_SIZE, melody4_notes};


//////////////////////////////
// Melody Definition #5     //
//////////////////////////////
#define MELODY5_SIZE    10
const tone melody5_notes[MELODY5_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_5G, 225},
    {BUZZFREQ_NOTE_5A, 225},
    {BUZZFREQ_NOTE_5B, 225},
    {BUZZFREQ_NOTE_6C, 225},
    {BUZZFREQ_NOTE_6D, 225},
    {BUZZFREQ_NOTE_6E, 225},
    {BUZZFREQ_NOTE_6F, 225},
    {BUZZFREQ_NOTE_6G, 225},
    {BUZZFREQ_NOTE_6G, 225},
};
const melody melody5 = {MELODY5_SIZE, melody5_notes};


//////////////////////////////
// Melody Definition #6     //
//////////////////////////////
#define MELODY6_SIZE    6
const tone melody6_notes[MELODY6_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_6F, 225},
    {BUZZFREQ_NOTE_6G, 225},
    {BUZZFREQ_NOTE_6E, 225},
    {BUZZFREQ_NOTE_5E, 225},
    {BUZZFREQ_NOTE_5A, 225},
};
const melody melody6 = {MELODY6_SIZE, melody6_notes};


//////////////////////////////
// Melody Definition #7     //
//////////////////////////////
#define MELODY7_SIZE    5
const tone melody7_notes[MELODY7_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_6D, 225},
    {BUZZFREQ_NOTE_6F, 225},
    {BUZZFREQ_NOTE_6G, 225},
    {BUZZFREQ_NOTE_6F, 225},
};
const melody melody7 = {MELODY7_SIZE, melody7_notes};


//////////////////////////////
// Melody Definition #8     //
//////////////////////////////
#define MELODY8_SIZE    7
const tone melody8_notes[MELODY8_SIZE]=
{
    {PAUSE, 225},                // First entry must always be the pause for continuous mode
    {BUZZFREQ_NOTE_6D, 225},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6D, 225},
    {PAUSE, 30},
    {BUZZFREQ_NOTE_6D, 225},
    {PAUSE, 225},
};
const melody melody8 = {MELODY8_SIZE, melody8_notes};

#endif  //#if defined(ENABLE_BUZZ)












