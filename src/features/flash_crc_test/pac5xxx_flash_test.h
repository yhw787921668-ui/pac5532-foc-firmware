
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

#ifndef PAC5XXX_FLASH_TEST_H
#define PAC5XXX_FLASH_TEST_H

#include "config_features.h"

extern uint16_t __crc16_checksum;

void set_reset_flash_crc_test_results(int32_t value);
int32_t get_reset_flash_crc_test_results(void);
int32_t pac5xxx_flash_crc_test(uint8_t *p_start_addr, uint32_t size_bytes, uint16_t checksum);
int32_t pac5xxx_fast_flash_crc_test(uint16_t *p_table, uint8_t *p_start_addr, uint32_t size_bytes, uint16_t checksum);
void make_crc16_table(uint16_t *p_crctable);
int32_t pac5xxx_hw_flash_crc_test(uint8_t *p_start_addr, uint32_t size_bytes, uint16_t checksum);
uint16_t crc16_hw(uint16_t sum, uint8_t *p, uint32_t len);
int32_t get_initial_sp_addr(void);

// Tool chain dependent Defines
// Define Vector Table Start Address; Normally VECTOR_TABLE_ADDR will be 0x0;  for the bootloader case, this will change
// Define RAM start address used for CRC Test with RAM based table
#if defined(__GNUC__) && defined(__clang__)                             // Keil Compiler 6
extern void __Vectors();
#define VECTOR_TABLE_ADDR &__Vectors
extern unsigned int Image$$RW_IRAM1$$Base;                              // Image$$RW_IRAM1$$Base is the symbol the Linker provides for the RAM base Address
#define RAM_BASE_ADDRESS_PTR    ((uint16_t *)&Image$$RW_IRAM1$$Base)    // Typically this will be the value 0x20000000

#elif defined(__GNUC__)                                                 // GNU C Compiler
extern void (* const g_pfnVectors[])(void);
#define VECTOR_TABLE_ADDR g_pfnVectors
extern uint32_t __ram_start;                                                // __ram_start is a symbol defined in the linker script for the RAM base Address
#define RAM_BASE_ADDRESS_PTR    ((uint16_t *)&__ram_start)                  // Typically this will be the value 0x20000000

#elif defined __ICCARM__                    // IAR C Compiler
extern void __vector_table();
#define VECTOR_TABLE_ADDR &__vector_table
extern uint32_t __ICFEDIT_region_RAM_start__;                               // __ICFEDIT_region_RAM_start__ is the symbol the Linker provides for the RAM base Address
#define RAM_BASE_ADDRESS_PTR    ((uint16_t *)&__ICFEDIT_region_RAM_start__) // Typically this will be the value 0x20000000

#elif defined __CC_ARM                      // Keil RealView C Compiler
#pragma diag_suppress 1295                  // Turn off deprecation warning for __Vectors
extern void __Vectors();
#define VECTOR_TABLE_ADDR &__Vectors
extern unsigned int Image$$RW_IRAM1$$Base;                              // Image$$RW_IRAM1$$Base is the symbol the Linker provides for the RAM base Address
#define RAM_BASE_ADDRESS_PTR    ((uint16_t *)&Image$$RW_IRAM1$$Base)    // Typically this will be the value 0x20000000

#endif


#endif // PAC5XXX_FLASH_TEST_H


