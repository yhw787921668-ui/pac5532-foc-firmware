
/**
 * @file flashrw_funcs.h
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

#ifndef FLASHRW_FUNCS_H
#define FLASHRW_FUNCS_H

#include "pac5xxx.h"
#include "maths.h"
#include "comm_struct.h"
#include "config_app.h"
#include "config_features.h"

#if defined(ENABLE_FLASH_RW)
#include "flashrw_struct.h"
#include "flashrw_fxadr_ctr.h"
#endif


#if defined(ENABLE_FLASH_RW)
extern void init_flash_data_write_status(void);

extern void flash_status_put_startwrite(int32_t value);
extern void flash_status_clear_startwrite(void);
extern uint32_t flash_status_get_startwrite(void);

extern uint32_t flash_status_get_writedone(void);
extern void flash_status_put_writedone(int32_t value);

extern uint32_t flash_status_get_startread(void);
extern void flash_status_put_startread(int32_t value);

extern void flash_status_put_readdone(int32_t value);
extern uint32_t flash_status_get_readdone(void);

extern void flash_status_put_starterase(int32_t value);
extern uint32_t flash_status_get_starterase(void);

extern void flash_status_put_erasedone(int32_t value);
extern uint32_t flash_status_get_erasedone(void);

extern void flash_status_put_busy(int32_t value);
extern uint32_t flash_status_get_busy(void);

extern void init_flash_at_pwr_up(void);

extern void flashrw_read(void);
extern PAC5XXX_RAMFUNC void flashrw_write(void);
extern PAC5XXX_RAMFUNC void flashrw_erase(void);

void flashrw_state_enter(Struct_Motor* motor_ptr);
void flashrw_state_exit(Struct_Motor* motor_ptr);
void flashrw_controller(Struct_Motor* motor_ptr);

#endif  //#if defined(ENABLE_FLASH_RW)

#endif  //#ifndef FLASHRW_FUNCS_H
