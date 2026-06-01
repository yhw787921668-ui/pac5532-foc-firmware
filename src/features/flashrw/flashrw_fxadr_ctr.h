
/**
 * @file flashrw_fxadr_ctr.c
 * @brief flash controller fixed address functions
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

#ifndef FLASHRW_FXADR_CTR_H
#define FLASHRW_FXADR_CTR_H

#include "pac5xxx.h"
#include "maths.h"
#include "config_features.h"
#include "flashrw_defs.h"
#include "flashrw_struct.h"


extern PAC5XXX_RAMFUNC flash_error flashrw_fxadr_read(uint32_t start_address,
    int32_t *data, uint32_t length, uint32_t end_address);
extern PAC5XXX_RAMFUNC flash_error flash_erase(uint32_t start_address, uint32_t end_address);
extern PAC5XXX_RAMFUNC flash_error flashrw_fxadr_write_data(Struct_Flash_Read_Wr* flash_flag_ptr,
    uint32_t start_address, int32_t* src_string, uint32_t length, uint32_t end_address);

#endif  //#ifndef FLASHRW_FXADR_CTR_H
