
/**
 * @file buzz_struct.h
 * @brief Define buzz function structures
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

#ifndef BUZZ_STRUCT_H
#define BUZZ_STRUCT_H

#include "aligngo_struct.h"

typedef struct
{
    uint16_t pitch;
    uint16_t duration;
} tone;


typedef struct
{
    uint16_t num_notes;
    const tone *notes;
} melody;


typedef union
{
    uint16_t     word;
    struct
    {
        uint16_t start : 1;
        uint16_t stop : 1;
        uint16_t contmode : 1;
        uint16_t running : 1;
        uint16_t melody : 3;
        uint16_t volume : 8;
        uint16_t unused : 1;
    };
} my_struct;

typedef struct
{
    uint8_t state_buzz;
    my_struct cmd_buzz;
    melody *ptr_melody;

    int tonelength_counter;
    int toneseq_count;
    int toneseq_target;

    Struct_AlignGoCMD aligngo;
} Struct_Buzz;

#endif  //BUZZ_STRUCT_H
