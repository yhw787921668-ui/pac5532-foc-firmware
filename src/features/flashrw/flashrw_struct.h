
/**
 * @file flashrw_struct.h
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

#ifndef FLASHRW_STRUCT_H
#define FLASHRW_STRUCT_H

#include "maths.h"


/**
* @brief flash error types enum
*/
typedef enum
{
    FLASH_OK = 0x00,                    /* NO issue */
    FLASH_INIT_ERROR = 0x01,            /* error while initializing */
    FLASH_DATA_DETECTION_ERROR = 0x02,  /* error while detect last data position */
    FLASH_WRITE_ERROR = 0x03,           /* error while writing, need to retry */
    FLASH_READ_ERROR = 0x04,            /* error while reading, need to re-write new data or erasing all flash */
    FLASH_SECTOR_ERROR = 0x05,          /* error in-correct page configuration */
    FLASH_CRC_ERROR = 0x06,             /* crc error in writing/reading */
    FLASH_IS_NOT_INITIALIZED = 0x07,
    FLASH_ERASE_ERROR = 0x08,
    FLASH_ERASE_ERROR1 = 0x09,
    FLASH_ERASE_ERROR2 = 0x0A,
    FLASH_ERASE_ERROR3 = 0x0B,
    FLASH_ERASE_ERROR4 = 0x0C,
    FLASH_ERASE_ERROR5 = 0x0D,
    FLASH_READ_WRITE_ADD_INVALID = 0x0E,
    FLASH_READ_WRITE_DATA_SIZE_INVALID = 0x0F,
} flash_error;


typedef enum
{
    STATE_FLASHRW_IDLE = 0,                 // 0
    STATE_FLASHRW_READ,                     // 1
    STATE_FLASHRW_WRITE,                    // 2
    STATE_FLASHRW_ERASE,                    // 3
    STATE_FLASHRW_END = STATE_FLASHRW_ERASE // 4
} eStateFlashRW;


typedef struct
{
    uint32_t start_write : 1;
    uint32_t done_write : 1;
    uint32_t start_read : 1;
    uint32_t done_read : 1;
    uint32_t start_erase : 1;
    uint32_t done_erase : 1;
    uint32_t busy : 1;
    uint32_t : 25;
    flash_error flash_status;
    eStateFlashRW state_flashrw;
} Struct_Flash_Read_Wr;


/** Memory structure
**/
typedef struct
{
    uint32_t info_addr;     /* infomation offset for checking excessive data    */
    uint32_t start_addr;    /* start address of Flash memory */
    uint32_t end_addr;      /* end address of Flash memory   */
    int32_t capacity;       /* memory capacity in byte  */
    uint32_t linked_addr;   /* linked address to append data at bottom flash */
    uint32_t tail;          /* current write pointer to flash mem*/
    uint32_t head;          /* current read pointer to flash mem*/
    uint32_t start_sector;  /* starting page to use in flash */
    uint32_t end_sector;    /* ending page to use in flash  */
    uint8_t isInit;         /* init flag checking           */
} Struct_Flash_Rd_Wr_DyCtr;


#endif  //#ifndef FLASHRW_STRUCT_H
