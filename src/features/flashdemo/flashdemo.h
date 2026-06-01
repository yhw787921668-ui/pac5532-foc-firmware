
/**
 * @file flashdemo.h
 * @brief Use to provide interface to read write data from and to flash
 */

//=============================================================================
// Copyright (C) 2022 - 2023, Qorvo, Inc.
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

#ifndef FLASHDEMO_H
#define FLASHDEMO_H

#include "pac5xxx.h"
#include "maths.h"
#include "comm_struct.h"
#include "config_app.h"
#include "config_features.h"

#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR )    /* Use fixed flash address writing scheme */ 
    #include "flashrw_fxadr_ctr.h"
#elif (FLSAHDEMO_MODESEL == FLASHRW_MODELSEL_DYADDR)    /* Use dynamic flash address writing scheme */ 
    #include "flashrw_dyadr_ctr.h"
#endif

#include "flashrw_struct.h"

#if defined(ENABLE_FLASHDEMO)

// Constant Definitions
#define TEST_VALUE (-10)

// Function Definitions
extern void flashdemo_param_process(ParameterCode param_code, int32_t rx_data);
extern int32_t flashdemo_param_export(int32_t index);
void flashdemo_data_set(void);
void flashdemo_data_clear(void);
void flashrw_init(void);
Struct_Flash_Rd_Wr_DyCtr* flashdemo_get_address(void);

#endif  //#if defined(ENABLE_FLASHDEMO)

#endif  //#ifndef FLASHDEMO_H
