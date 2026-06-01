
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

#include <stdint.h>

#ifdef __ICCARM__           // IAR C Compiler
// Nothing needed for IAR

#elif defined __CC_ARM      // Keil v5 RealView C Compiler
const uint16_t __crc16_checksum = 0xCAFE;

#elif defined(__GNUC__)  &&  defined(__clang__)   // Keil v6 Clang Compiler
__attribute__((used)) const uint16_t __crc16_checksum = 0xCAFE;

#elif defined __GNUC__      // GNU C Compiler
// Nothing needed for GCC/COIDE
#endif

