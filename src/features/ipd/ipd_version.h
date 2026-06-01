
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

#ifndef IPD_VERSION_H
#define IPD_VERSION_H

// Include Library Build Information - Includes Build Date and Time
#if defined(__GNUC__) && defined(__clang__)    // Keil v6 Clang Compiler
#include "ipd_build_info_keil.h"
#elif defined __GNUC__              // GNU C Compiler
#include "ipd_build_info_coide.h"
#elif defined __ICCARM__            // IAR C Compiler
#include "ipd_build_info_iar.h"
#elif defined __CC_ARM              // Keil RealView C Compiler
#include "ipd_build_info_keil.h"
#endif

#endif

/*

Revision History:



*/
