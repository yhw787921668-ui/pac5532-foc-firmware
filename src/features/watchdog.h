
/**
 * @file watchdog.h
 * @brief Use to initialize watchdog timer
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

#ifndef WatchDog_H
#define WatchDog_H
#include "motor_struct.h"

#if (ENABLE_WATCHDOG == TRUE)
#define WATCHDOG_FREQ_HZ    2                         // Watchdog timeout in Hz

/**
 * @brief  Use to initialize watchdog timer
 */
extern void init_Watchdog_time_base(void);
#endif

#endif //#if (ENABLE_WATCHDOG == TRUE)
