
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

#ifndef FLASH_CTR_DYADR_H
#define FLASH_CTR_DYADR_H

#include "pac5xxx.h"
#include "maths.h"
#include "config_app.h"
#include "config_features.h"
#include "flashrw_defs.h"
#include "flashrw_struct.h"


extern PAC5XXX_RAMFUNC flash_error flashrw_dyadr_erase(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr);
extern flash_error flashrw_dyadr_init(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t start_addr, uint32_t end_addr);
extern PAC5XXX_RAMFUNC flash_error flashrw_dyadr_write(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t *data, uint32_t length);
extern PAC5XXX_RAMFUNC flash_error flashrw_dyadr_read(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t *data, uint32_t length);
extern PAC5XXX_RAMFUNC void flashrw_dyadr_erase_page(uint32_t page_num);
    
#if (COMMSEL_PWR_DOWN_INPUT == TRUE)
#include "flash_ctr_def.h"

#define FLASH_WORD_ARRAY_MAX_ALLOWABLE_SIZE (0xA)    /* User can not change this, it is fixed, max words write to flash limit */

#define TEST_FLASH_READ_WRITE          /* used to enable filling test data and reading data from flash, testing only */ 

#define FLASH_WORD_ARRAY_SIZE (0x2)    /* If need more room in array increase size of this define */
#define FLASH_START_ADDR    (0x7800)
#define FLASH_END_ADDR      (0x7BFF)
#define FLASH_DATA_SIZE     (8)
/**
* @brief flash error types enum
*/

extern flash_error flash_init(uint32_t start_addr, uint32_t end_addr);
extern PAC5XXX_RAMFUNC flash_error flash_write(uint32_t *data, uint32_t length);
extern PAC5XXX_RAMFUNC flash_error flash_read(uint32_t *data, uint32_t length);
extern PAC5XXX_RAMFUNC flash_error flash_erase(void);
#endif

#endif  //#ifndef FLASH_CTR_DYADR_H
