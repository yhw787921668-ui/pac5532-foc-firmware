
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

#ifndef POST_STRUCT_H
#define POST_STRUCT_H


#include "board.h"
#include "config_app.h"
#include "config_features.h"


typedef struct
{
#if (CONFIG_OFFSET & CHANNEL_AIO10)
    uint32_t adc_offset_aio10;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO32)
    uint32_t adc_offset_aio32;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO54)
    uint32_t adc_offset_aio54;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO6)
    uint32_t adc_offset_aio6;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO7)
    uint32_t adc_offset_aio7;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO8)
    uint32_t adc_offset_aio8;
#endif
#if (CONFIG_OFFSET & CHANNEL_AIO9)
    uint32_t adc_offset_aio9;
#endif

#if defined(FAMILY_55XX) && defined (ENABLE_ADCCAL_FACTORY)
    int32_t adc_offset_counts_q16; 
	int32_t adc_gain_multp_q16;
#endif
} Struct_POST;

#endif  //#ifndef POST_STRUCT_H
