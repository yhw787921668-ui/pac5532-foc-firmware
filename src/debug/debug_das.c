
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

#include "pac5xxx.h"
#include "motor_struct.h"
#include "debug_dac.h"
#include "debug_das.h"

#ifdef ENABLE_DEBUG_DAS
#if (DAS_VERSION == DAS_VERSION_1)

int debug_channel_offset = 0;
debug_pointer debug;

void das_debug_init(void)
{
#ifdef ENABLE_DEBUG_DAS
    unsigned long temp;
    temp = (unsigned long)&debug.offset[0];

#if defined(FAMILY_52XX)
    *((unsigned long *)0xD0000000) = temp;
#elif defined(FAMILY_55XX)
    *((unsigned long *)0x400D1004) = temp;
#else
    "Error"
#endif

    if ((temp & 0x0C) == 0x04)
        debug_channel_offset = 2;
    else
        debug_channel_offset = 0;
    debug.offset[0] = 0;
    debug.offset[1] = 4;
    debug.offset[2] = 8;
    debug.offset[3] = 12;
    debug.scale[0] = 0x10000;
    debug.scale[1] = 0x10000;
    debug.scale[2] = 0x10000;
    debug.scale[3] = 0x10000;
#endif //DEBUG_DAS_DAC_ENABLE
}

#elif (DAS_VERSION == DAS_VERSION_2)

#if defined(__CC_ARM)
__align(16) das_data_t debug;
#elif defined(__ICCARM__)
#pragma data_alignment = 16
das_data_t debug;
#elif defined(__GNUC__)
das_data_t debug __attribute__((aligned(16)));
#else
#error Not supported compiler type
#endif

void das_debug_init(void)
{
#if defined(FAMILY_52XX)
    *((unsigned long *)0xD0000000) = (unsigned long)&debug;
#elif defined(FAMILY_55XX)
    *((unsigned long *)0x400D1004) = (unsigned long)&debug;
#else
#error "Error"
#endif

    debug.scale[0] = DAS_SCALE_COEFFICIENT * 1;
    debug.scale[1] = DAS_SCALE_COEFFICIENT * 1;
    debug.scale[2] = DAS_SCALE_COEFFICIENT * 1;
    debug.scale[3] = DAS_SCALE_COEFFICIENT * 1;

    debugDASio_on();
}
#else
#error "DAS_VERSION has not been selected"
#endif

#endif //#ifdef ENABLE_DEBUG_DAS
