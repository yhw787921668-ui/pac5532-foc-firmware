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

#include "pac5xxx.h"
#include "config_app.h"
#include "pac5xxx_flash_test.h"



#ifdef __ICCARM__           // IAR C Compiler
extern void init_system(void);
extern void test_flash_crc(void);
extern void pac_recovery(void);
extern void __iar_program_start(void);
extern int __ICFEDIT_region_RAM_end__;

#if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();
    
    uint32_t ram_end_addr = (uint32_t)&__ICFEDIT_region_RAM_end__;
    ram_end_addr -= 3;

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    // Set SP to top of RAM so it can't conflict with CRC RAM Table at bottom of RAM
    __set_MSP((uint32_t)ram_end_addr);

    init_system();

    // Note: Before calling, make sure stack pointer points to Top of RAM so stack won't conflict with RAM table
    //SP is set in g_pfnVectors
    test_flash_crc();

    // Put Stack pointer back by loading stack pointer stored at addr 0x0
    __set_MSP(*(uint32_t*)0x0);

    // Call Normal Reset Handler functions
    __iar_program_start();
}
#elif defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_55XX)
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    init_system();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    test_flash_crc();

    // Call Normal Reset Handler functions
    __iar_program_start();
}
#endif // #if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)
#endif	// #ifdef __ICCARM__           // IAR C Compiler

#ifdef __CC_ARM      // Keil RealView C Compiler

#include "system_pac5xxx.h"
extern void init_system(void);
extern void test_flash_crc(void);
extern void pac_recovery(void);
extern void __main(void);
#define RAM_END_ADDRESS     0x20002000

#if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    // Set SP to top of RAM so it can't conflict with CRC RAM Table at bottom of RAM
    __set_MSP((uint32_t)(RAM_END_ADDRESS-4));

    init_system();

    // Note: Before calling, make sure stack pointer points to Top of RAM so stack won't conflict with RAM table
    //SP is set in g_pfnVectors
    test_flash_crc();

    // Put Stack pointer back by loading stack pointer stored at addr 0x0
    __set_MSP(*((uint32_t*)get_initial_sp_addr()));

    // Call Normal Reset Handler functions
    SystemInit();
    __main();
}

#elif defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_55XX)
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    init_system();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    test_flash_crc();

    // Call Normal Reset Handler functions
    SystemInit();
    __main();
}
#endif // #if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)

#endif  // #ifdef __CC_ARM      // Keil RealView C Compiler

#if defined(__GNUC__) && defined(__clang__)      // Keil Compiler 6 - clang--
#include "system_pac5xxx.h"
extern void init_system(void);
extern void test_flash_crc(void);
extern void pac_recovery(void);
extern void __main(void);
#define RAM_END_ADDRESS     0x20002000

#if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    // Set SP to top of RAM so it can't conflict with CRC RAM Table at bottom of RAM
    __set_MSP((uint32_t)(RAM_END_ADDRESS-4));

    init_system();

    // Note: Before calling, make sure stack pointer points to Top of RAM so stack won't conflict with RAM table
    //SP is set in g_pfnVectors
    test_flash_crc();

    // Put Stack pointer back by loading stack pointer stored at addr 0x0
    __set_MSP(*((uint32_t*)get_initial_sp_addr()));

    // Call Normal Reset Handler functions
    SystemInit();
    __main();
}

#elif defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_55XX)
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    init_system();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    test_flash_crc();

    // Call Normal Reset Handler functions
    SystemInit();
    __main();
}
#endif // #if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)

#elif defined(__GNU__)      // GNU C Compiler
extern uint32_t __StackTop;                     // __StackTop is set to end of RAM by linker
extern void init_system(void);
extern void test_flash_crc(void);
extern void pac_recovery(void);
extern void Default_Reset_Handler(void);

#if defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)
void Reset_Handler(void) __attribute__( ( naked ) );
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    // Set SP to top of RAM so it can't conflict with CRC RAM Table at bottom of RAM
    __set_MSP((uint32_t)&__StackTop-4);

    init_system();

    // Note: Before calling, make sure stack pointer points to Top of RAM so stack won't conflict with RAM table
    //SP is set in g_pfnVectors
    test_flash_crc();

    // Put Stack pointer back to original top of stack by reading sp located at 0x0
    __set_MSP(*((uint32_t *)get_initial_sp_addr()));

    // Call Normal Reset Handler functions
    Default_Reset_Handler();
}

#elif defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_55XX)

void Reset_Handler(void) __attribute__( ( naked ) );
void Reset_Handler(void)
{
    // Call pac_recovery function to allow recovery of mis-configured device
    pac_recovery();

    init_system();

    //===== Perform Flash CRC Test before any processing; CPU clock is set for fast CRC Processing =====
    test_flash_crc();

    // Call Normal Reset Handler functions
    Default_Reset_Handler();
}

#endif // defined(ENABLE_FLASH_CRC_TEST) && defined(FAMILY_52XX)

#endif // #if defined __GNUC__      // GNU C Compiler
