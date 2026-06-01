
/**
 * @file stalldet_config.h
 * @brief Use for stall detect configuration
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#ifndef STALLDET_CONFIG_H
#define STALLDET_CONFIG_H

#define STALL_EVENT_CNT_LIMIT                   10
#define STALL_SPEED_ERROR_LIMIT_PCT             QCONST(0.01, Q28)                    // Error PCT of max outer loop scale in Q28
#define STALL_SPEED_FILTER_FC_Q16               QCONST(0.1, Q16)                    // Speed cutoff frequency in Hz
// TODO:JAG STALL_SPEED_FILTER_FC parameter is a function of ramp speed. Need to explore way to automatically calculate it.

#endif
