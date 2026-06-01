
/**
 * @file comm_funcs.c
 * @brief Use for Communication related functions
 */

//=============================================================================
// Copyright (C) 2017 - 2020, Qorvo, Inc.
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


#include "comm_funcs.h"


uint8_t uart_checksum(uint8_t* pBuffer, uint32_t bytesnum)
{
    uint8_t checksum = 0;
    int i;

    for (i = 0; i < bytesnum; i++)
        checksum += *pBuffer++;

    return checksum;
}
