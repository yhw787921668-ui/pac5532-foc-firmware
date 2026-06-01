
/**
 * @file flashrw_defs.h
 * @brief Use for flash read write basic definitions
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


#ifndef FLASHRW_DEFS_H
#define FLASHRW_DEFS_H

#include "pac5xxx.h"
#include "board.h"
#include "config_features.h"

// Macro Definitions
#define READ_DATA_AT(address)       *((uint32_t *) (address))

#if defined(FAMILY_52XX)

#define FXADR_WORD_SIZE                         (0x4)              /* 4 bytes for CRC and data size */
#define FXADR_FLASH_MAX_SECTOR                  (0x20)

/* Size of flash sector */
#define FLASH_SECTOR_SIZE_BYTE                  (1024)
#define FLASH_SECTOR_SIZE_BITS_FIXED            (10)               /* 1024 bytes */

#define FLASH_LOCK_ALLOW_WRITE_MEMCTL           0xD513B490         // Write this value to FLASHLOCK to allow write to MEMCTL register
#define FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH      0xAAAAAAAA         // Write this value to FLASHLOCK to allow write and erase operations to FLASH
#define FLASH_START_PAGE_ERASE                  0xA5A55A5A         // Allow memory controller to start a FLASH page erase operation. 

#define WORD_SIZE_DYNAMIC                       (0x04)             /* 4 bytes for CRC and data size */
#define FLASH_MAX_SECTOR_DYNAMIC                FLASH_SECTOR_31

#endif

#if defined(FAMILY_55XX)

#define FXADR_WORD_SIZE                         (0x4)              /* 4 bytes for CRC and data size */
#define FXADR_FLASH_MAX_SECTOR                  (0x7F)
#define FLASH_WAIT_DELAY_MEM_CONT_READY         320

#define WORD_SIZE_DYNAMIC                       (0x04)             /* 4 bytes for CRC and data size */
#define FLASH_MAX_SECTOR_DYNAMIC                127                //FLASH_SECTOR_127

#endif

#define NO_EXCESSIVE_DATA                       0xFFFFFFFF
#define APPENDING_POINT_PAGE                    0x5A5A5A5A
#define FLASH_IS_INIT_DYNAMIC                   (0x01)
#define FLASH_NOT_INIT_DYNAMIC                  (0x00)


/* PAC52XX flash page address only */
#define ADDR_FLASH_SECTOR_0                     (0x00000000)
#define ADDR_FLASH_SECTOR_1                     (0x00000400)
#define ADDR_FLASH_SECTOR_2                     (0x00000800)
#define ADDR_FLASH_SECTOR_3                     (0x00000C00)
#define ADDR_FLASH_SECTOR_4                     (0x00001000)
#define ADDR_FLASH_SECTOR_5                     (0x00001400)
#define ADDR_FLASH_SECTOR_6                     (0x00001800)
#define ADDR_FLASH_SECTOR_7                     (0x00001C00)
#define ADDR_FLASH_SECTOR_8                     (0x00002000)
#define ADDR_FLASH_SECTOR_9                     (0x00002400)
#define ADDR_FLASH_SECTOR_10                    (0x00002800)
#define ADDR_FLASH_SECTOR_11                    (0x00002C00)
#define ADDR_FLASH_SECTOR_12                    (0x00003000)
#define ADDR_FLASH_SECTOR_13                    (0x00003400)
#define ADDR_FLASH_SECTOR_14                    (0x00003800)
#define ADDR_FLASH_SECTOR_15                    (0x00003C00)
#define ADDR_FLASH_SECTOR_16                    (0x00004000)
#define ADDR_FLASH_SECTOR_17                    (0x00004400)
#define ADDR_FLASH_SECTOR_18                    (0x00004800)
#define ADDR_FLASH_SECTOR_19                    (0x00004C00)
#define ADDR_FLASH_SECTOR_20                    (0x00005000)
#define ADDR_FLASH_SECTOR_21                    (0x00005400)
#define ADDR_FLASH_SECTOR_22                    (0x00005800)
#define ADDR_FLASH_SECTOR_23                    (0x00005C00)
#define ADDR_FLASH_SECTOR_24                    (0x00006000)
#define ADDR_FLASH_SECTOR_25                    (0x00006400)
#define ADDR_FLASH_SECTOR_26                    (0x00006800)
#define ADDR_FLASH_SECTOR_27                    (0x00006C00)
#define ADDR_FLASH_SECTOR_28                    (0x00007000)
#define ADDR_FLASH_SECTOR_29                    (0x00007400)
#define ADDR_FLASH_SECTOR_30                    (0x00007800)
#define ADDR_FLASH_SECTOR_31                    (0x00007C00)

/* Size of flash sector */
//#define FLASH_SECTOR_SIZE_BYTE                    (1024)
#define FLASH_SECTOR_SIZE_BITS_DYNAMIC                (10)    /* 1024 bytes */

typedef enum
{
    FLASH_SECTOR_0 = 0x00,
    FLASH_SECTOR_1 = 0x01,
    FLASH_SECTOR_2 = 0x02,
    FLASH_SECTOR_3 = 0x03,
    FLASH_SECTOR_4 = 0x04,
    FLASH_SECTOR_5 = 0x05,
    FLASH_SECTOR_6 = 0x06,
    FLASH_SECTOR_7 = 0x07,
    FLASH_SECTOR_8 = 0x08,
    FLASH_SECTOR_9 = 0x09,
    FLASH_SECTOR_10 = 0x0A,
    FLASH_SECTOR_11 = 0x0B,
    FLASH_SECTOR_12 = 0x0C,
    FLASH_SECTOR_13 = 0x0D,
    FLASH_SECTOR_14 = 0x0E,
    FLASH_SECTOR_15 = 0x0F,
    FLASH_SECTOR_16 = 0x10,
    FLASH_SECTOR_17 = 0x11,
    FLASH_SECTOR_18 = 0x12,
    FLASH_SECTOR_19 = 0x13,
    FLASH_SECTOR_20 = 0x14,
    FLASH_SECTOR_21 = 0x15,
    FLASH_SECTOR_22 = 0x16,
    FLASH_SECTOR_23 = 0x17,
    FLASH_SECTOR_24 = 0x18,
    FLASH_SECTOR_25 = 0x19,
    FLASH_SECTOR_26 = 0x1A,
    FLASH_SECTOR_27 = 0x1B,
    FLASH_SECTOR_28 = 0x1C,
    FLASH_SECTOR_29 = 0x1D,
    FLASH_SECTOR_30 = 0x1E,
    FLASH_SECTOR_31 = 0x1F
} flash_sector_e;

#endif // FLASHRW_DEFS_H


