
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

#include "stack_check.h"
#include "fault.h"

#if (ENABLE_STACK_CHECK == TRUE)

#define STACK_MAGIC_VALUE  0x1abe11ed         // Labeled using 1s for ls


// Compiler Specific Definitions
//===== IAR C Compiler =====
#ifdef __ICCARM__

#pragma location=".stack_start"
uint32_t stack_start_reserved;

#pragma location=".stack_end"
uint32_t stack_end_reserved;

//===== Keil RealView C Compiler =====
#elif defined __CC_ARM      // Keil RealView C Compiler

uint32_t stack_start_reserved __attribute__((section(".stack_start")));
uint32_t stack_end_reserved __attribute__((section(".stack_end")));

//===== Keil v6 Clang Compiler =====
#elif defined(__GNUC__) && defined(__clang__)

__attribute__((used)) uint32_t stack_start_reserved __attribute__((section(".stack_start")));
__attribute__((used)) uint32_t stack_end_reserved __attribute__((section(".stack_end")));

#elif defined __GNUC__      // GNU C Compiler

uint32_t stack_start_reserved __attribute__((section(".stack_start")));
uint32_t stack_end_reserved __attribute__((section(".stack_end")));

#endif  // Compiler Specific Definition




void init_stack_vars(void)
{
    stack_start_reserved = STACK_MAGIC_VALUE;
    stack_end_reserved = STACK_MAGIC_VALUE;
}

void stack_test(void)
{
    //===== Check for underflow; If Stack Top value has changed, then set warning =====
   if (stack_start_reserved != STACK_MAGIC_VALUE)
   {
       set_warning(WARNING_STACK_OOR);
   }

   //===== Check for overflow; If Stack Bottom value has changed, then set warning =====
   if (stack_end_reserved  != STACK_MAGIC_VALUE)
   {
       set_warning(WARNING_STACK_OOR);
   }
}


#endif  // #if (ENABLE_STACK_CHECK == TRUE)

