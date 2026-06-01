
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

#include "config_app.h"
#include <stdint.h>
#include "pac5xxx.h"
#include "pac5xxx_driver_system.h"
#include "pac5xxx_driver_memory.h"
#include "pac5xxx_flash_test.h"
#include "checksum.h"

void test_flash_crc(void)
{
    uint32_t ret;

#ifdef FAMILY_55XX
    // Call flash test using HW CRC Module
    ret = pac5xxx_hw_flash_crc_test(  (uint8_t *) VECTOR_TABLE_ADDR,                                // Start Address
                                      (uint32_t) &__crc16_checksum - (uint32_t)VECTOR_TABLE_ADDR,   // Size in bytes
                                      __crc16_checksum);                                            // Expected CRC Value
#else
    // Create 16-bit crc16 table in RAM at RAM Base Address, The memory will be reclaimed by the application
    // Note: The stack pointer should be at the top of RAM so there is no conflict
    make_crc16_table(RAM_BASE_ADDRESS_PTR);

    // Call fast flash test using generated RAM table
    ret = pac5xxx_fast_flash_crc_test(RAM_BASE_ADDRESS_PTR,                                         // Pointer to CRC Table
                                      (uint8_t *) VECTOR_TABLE_ADDR,                                // Start Address
                                      (uint32_t) &__crc16_checksum - (uint32_t)VECTOR_TABLE_ADDR,   // Size in bytes
                                      __crc16_checksum);                                            // Expected CRC Value
#endif // FAMILY_55XX

    // Store the results of the Flash CRC Test so the application can handle the CRC Failure
    uart_scratch_write(ret);  // CRC Passed-ret=0; CRC Failed-ret=1 (i.e. non zero)

}

//==============================================================================
///
/// @brief  This function performs a test of the flash using a 16-bit CRC
///
///         The test computes a 16-bit CRC checksum over the contents of the
///         specified memory.  If the results don't match the
///         checksum parameter passed in, then a 1 is returned.
///
/// @param  p_start_addr - starting address for the CRC calculation
/// @param  size_bytes   - size in bytes of the flash to be tested
/// @param  checksum     - CRC checksum to compare the test results against
///
/// @return int32_t 0= pass; 1=fail
///
//==============================================================================
int32_t pac5xxx_flash_crc_test(uint8_t *p_start_addr, uint32_t size_bytes, uint16_t checksum)
{
    uint16_t sum=0;
    const uint32_t zero=0;
    
    //===== Calculate the CRC using the previous sum and the remainder of the bytes =====
    sum = crc16(sum, p_start_addr, size_bytes);    

    //===== Push 16 more bits through to get the final sum =====
    sum = crc16(sum, (uint8_t *)&zero, 2);

    //===== Verify the final CRC against the stored CRC =====
    if (sum != checksum)
        return 1;
    else
        return 0;

}


//==============================================================================
///
/// @brief  This function performs a fast flash test using a CRC Table 
///
///         The test computes a 16-bit CRC checksum over the contents of the
///         flash used by the application.  The CRC calculation makes use of
///         a 256 entry CRC table to speed up the calculation.  If the results
///         don't match the checksum parameter passed in, then a 1 is returned.
///
/// @param  p_table      - pointer to the CRC table
/// @param  p_start_addr - starting address for the CRC calculation
/// @param  size_bytes   - size in bytes of the flash to be tested
/// @param  checksum     - CRC checksum to compare the test results against
///
/// @return int32_t 0= pass; 1=fail
///
//==============================================================================
int32_t pac5xxx_fast_flash_crc_test(uint16_t *p_table, uint8_t *p_start_addr, uint32_t size_bytes, uint16_t checksum)
{
    uint16_t sum = 0;
    
    sum = crc16_table_256(sum, p_start_addr, size_bytes, p_table);

    if (sum != checksum)
        return 1;
    else
        return 0;

}

#ifdef FAMILY_55XX

//===========================================================================================
///
/// @brief  This function performs a flash test using the CRC HW Module in PAC55XX devices
///
///         The test computes a 16-bit CRC checksum over the contents of the
///         flash used by the application.  The CRC calculation makes use of
///         a 256 entry CRC table to speed up the calculation.  If the results
///         don't match the checksum parameter passed in, then a 1 is returned.
///
/// @param  p_table      - pointer to the CRC table
/// @param  p_start_addr - starting address for the CRC calculation
/// @param  size_bytes   - size in bytes of the flash to be tested
/// @param  checksum     - CRC checksum to compare the test results against
///
/// @return int32_t 0= pass; 1=fail
///
//=============================================================================================
int32_t pac5xxx_hw_flash_crc_test(uint8_t *p_start_addr, uint32_t size_bytes, uint16_t checksum)
{
    uint16_t sum = 0;

    sum = crc16_hw(sum, p_start_addr, size_bytes);

    if (sum != checksum)
        return 1;
    else
        return 0;

}


uint16_t crc16_hw(uint16_t sum, uint8_t *p, uint32_t size_bytes)
{
	uint8_t * p8;
	uint32_t * p32;

	PAC55XX_CRC->CTL.INREF = 0;
    PAC55XX_CRC->CTL.OUTREF = 0;
    PAC55XX_CRC->CTL.POLYSEL = CRC16_CCITT;
    PAC55XX_CRC->CTL.DATAWIDTH = CRC_DATA_WIDTH_8BITS;
    PAC55XX_CRC->SEED.CRCSEED=sum;

    // Process first set of bytes that are not aligned to a 32-bit boundary
    p8 = (uint8_t *)p;
    while (((uint32_t)p8 & 0x3) != 0)    // not 8 bits boundary
    {
        PAC55XX_CRC->DATAIN = *p8++;
        size_bytes--;
    }

    // Process any full 32-bit words
    p32 = (uint32_t *)p8;
    PAC55XX_CRC->CTL.DATAWIDTH = CRC_DATA_WIDTH_32BITS;

    while ( ( ((uint32_t)p32 & 0x3) == 0) & (size_bytes > 4) )    // 32 bits boundary
    {
        PAC55XX_CRC->DATAIN = *p32++;
        size_bytes = size_bytes - 4;
    }

    // Process any remaining bytes
    p8 = (uint8_t *)p32;
    PAC55XX_CRC->CTL.DATAWIDTH = CRC_DATA_WIDTH_8BITS;
    while (size_bytes > 0)    // rest of bytes 8 bits boundary
    {
        PAC55XX_CRC->DATAIN = *p8++;
        size_bytes--;
    }


    // Short delay before reading CRC output
#if defined(__CC_ARM)        // Keil RealView C Compiler
    __asm{ NOP
           NOP }    
#elif defined(__ICCARM__)   // IAR C Compiler
    asm("NOP");
    asm("NOP");
#elif defined (__GNUC__)      // GNU C Compiler
    __asm__("NOP");
    __asm__("NOP");
#else
    #error Compiler not suported
#endif

    // Return the computed CRC value
    return PAC55XX_CRC->OUT.CRCOUT;
}
#endif  // #ifdef FAMILY_55XX


//==============================================================================
///
/// @brief  This function makes a 256 entry 16-bit CRC table on the fly in RAM
///
/// @param  p_crctable   - pointer to the CRC table
///
/// @return none
///
//==============================================================================
void make_crc16_table(uint16_t *p_crctable)
{
    int32_t value;
    int32_t i;
    int32_t j;

    for(i=0; i < 256; i++)
    {
        value = i << 8;

        for(j=0; j < 8; j++)
        {
           if(value & (1 << 15))
               value = (value << 1)^0x1021;
           else
               value = value << 1;
        }

        p_crctable[i]= value;
    }
}

int32_t get_initial_sp_addr(void)
{
    return 0;
}
