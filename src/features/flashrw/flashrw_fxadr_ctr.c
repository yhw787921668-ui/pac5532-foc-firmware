
/**
 * @file flashrw_fxadr_ctr.c
 * @brief flash fixed library implementation
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

#include "flashrw_defs.h"
#include "flashrw_crc8.h"
#include "pac5xxx_driver_system.h"
#include "config_features.h"
#include "flashrw_fxadr_ctr.h"

/**
 *@brief  this function to detect physical address in flash memory
 *@param  cur_addr
 *@param  size
 *@param  end_address
 *@return address on defined memory area
 */
static inline uint32_t flashrw_fxadr_jump_next_addr(uint32_t cur_addr, uint32_t size, uint32_t end_address)
{
    uint32_t checking_point;
    uint32_t next_addr;

    checking_point = cur_addr + size;

    if(checking_point >= end_address)
    {
        next_addr = end_address; // If next address is invalid, it should be configured as start address
    }
    else
    {
        next_addr = cur_addr +  size;
    }
    return next_addr;
}


/**
*@brief  flash read fixed function
*@param  start_address
*@param  *data
*@param  length
*@param  end_address
*@return flash error type
*/
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC flash_error flashrw_fxadr_read(uint32_t start_address, int32_t *data, uint32_t length, uint32_t end_address)
{
    flash_error ret;
    uint32_t i;
    uint32_t temp_ptr;
    uint32_t FlashCtr_head;
#ifdef CRC_CALCULATION_ON
    uint8_t crc_value;
    uint32_t data_size;
    uint32_t FlashCtr_tail;
#endif

    ret = FLASH_OK;

    FlashCtr_head = start_address;

    if ((start_address < end_address) && ((start_address + length) <= end_address))
    {
#ifdef CRC_CALCULATION_ON
        data_size = READ_DATA_AT(FlashCtr_head);
        if(data_size != (length))
        {
            ret = FLASH_READ_WRITE_DATA_SIZE_INVALID;
        }
        else
        {
#endif
            // Read data
            temp_ptr = FlashCtr_head;
            for(i = 0; i < length; i++)
            {
#ifdef CRC_CALCULATION_ON
                temp_ptr = flashrw_fxadr_jump_next_addr(temp_ptr, FXADR_WORD_SIZE, end_address);
                data[i] = *(uint32_t*)temp_ptr;
#else
                data[i] = *(uint32_t*)temp_ptr;
                temp_ptr = flashrw_fxadr_jump_next_addr(temp_ptr, FXADR_WORD_SIZE, end_address);
#endif
            }
#ifdef CRC_CALCULATION_ON
            FlashCtr_tail = flashrw_fxadr_jump_next_addr(temp_ptr, FXADR_WORD_SIZE, end_address);
            crc_value = crc_8_calculation(0x00, (uint8_t*)data, (length << 2));
            if(crc_value != (uint8_t)READ_DATA_AT(FlashCtr_tail))
            {
                ret = FLASH_CRC_ERROR;
            }
       }
#endif
    }
    else
    {
        ret = FLASH_READ_WRITE_ADD_INVALID;
    }
    return ret;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 *@brief  Erase the specified flash page
 *@param  page_num: the Flash page to erase
 *@return None
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void flash_erase_page(uint32_t page_num)
{
#if defined(FAMILY_55XX)
    // Enable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Set Page to Be Written
    PAC55XX_MEMCTL->FLASHPAGE.PAGE = page_num;

    // Erase Page
    PAC55XX_MEMCTL->FLASHERASE = FLASH_START_PAGE_ERASE;

    // Wait for erase to complete
    while (PAC55XX_MEMCTL->MEMSTATUS.EBUSY) { }

    // Disable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = 0;
#endif

#if defined(FAMILY_52XX)
    // Enable Write Access to FLash Controller
    PAC5XXX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Set Page to Be Written
    PAC5XXX_MEMCTL->FLASHPAGE.PAGE = page_num;

    // Erase Page
    PAC5XXX_MEMCTL->FLASHPERASE = FLASH_START_PAGE_ERASE;

    // Wait for erase to complete
    while (PAC5XXX_MEMCTL-> FLASHCTL.PERASE) { }

    // Disable Write Access to FLash Controller
    PAC5XXX_MEMCTL->FLASHLOCK = 0;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/*************************************************************************
 * FLASH programmer block erase
 *************************************************************************/
/**
 *@brief  Erase the specified flash page
 *@param  start_address
 *@param  end_address
 *@return Status
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC flash_error flash_erase(uint32_t start_address, uint32_t end_address)
{
    volatile uint32_t address;
    volatile uint32_t block_start_add, block_end_add;
    volatile int i, starting_sector, num_sectors_to_erase;
  
    block_start_add = start_address;
    block_end_add = end_address; 

    // Check to see if the address is on a page boundary
    address = (uint32_t)block_start_add;
    if ((address & 0x3FF) != 0)
        return FLASH_ERASE_ERROR1;
  
    if (block_start_add == block_end_add)
        return FLASH_ERASE_ERROR2;
    
    if (block_end_add < block_start_add)
        return FLASH_ERASE_ERROR3;

    // Calculate the stor, and how many sectors to erase
    starting_sector = (address >> 10);
    num_sectors_to_erase = (uint32_t)(block_end_add - block_start_add) >> 10;
  
    // Make sure we don't run off the end of FLASH
    if (starting_sector + num_sectors_to_erase > FXADR_FLASH_MAX_SECTOR)
    {
        return FLASH_ERASE_ERROR4;
    }

    // For each sector to erase, make sure that the bit in the PRW mask is set
    for (i = starting_sector; i <= (starting_sector + num_sectors_to_erase); i++)
    {
        flash_erase_page(i);
    }
  
    return FLASH_OK;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief  Write to Flash location that is 16 byte aligned with a mulitple of 4 words
 * @note   Pay careful attention to alignment destination and source addresses along with 
 *         the number of words to write.  If these are not correct and Error will be returned
 * @param  p_dest: pointer to destination Flash to write, must be 16 byte aligned
 * @param  p_src:  pointer to source buffer to write, must be 32-bit aligned
 * @param  size_32bit_words: size in 32-bit words of the Flash to write, must be a multiple of 4
 * @retval PAC5XXX_OK: All is OK, PAC5XXX_ERROR: An error occurred
 * retval = flash_write_16byte_aligned((uint32_t *)0x0001E600, word_buffer, BUFF_SIZE_WORDS);
 * Destination, Source, Size Words
**/
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC uint32_t flashrw_fxadr_write_5xxx(uint32_t *p_dest, uint32_t *p_src, uint32_t size_32bit_words)
{

    // If destination is not 16-byte aligned OR source isn't 32-bit aligned OR size is not a multiple of 16 bytes, then return an error
#if defined(FAMILY_55XX)
    if( ((uint32_t)p_dest & 0xF) || ((uint32_t)p_src & 0x3) || (size_32bit_words % 4) )
        return FLASH_ERASE_ERROR5;

    // Enable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Ensure Flash write Word Count starts at 0
    PAC55XX_MEMCTL->MEMCTL.WRITEWORDCNT = 0;

    // Write data buffer to FLASH
    for(uint32_t i = 0; i < size_32bit_words; i++)
    {
        p_dest[i] = p_src[i];
    }

    // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
    while(PAC55XX_MEMCTL->MEMSTATUS.WBUSY) { }  // wait for Flash Write WBUSY bit to be 0
    pac_delay_asm_ramfunc(FLASH_WAIT_DELAY_MEM_CONT_READY);  // delay 20uS after WBUSY=0

    // Disable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = 0;
#endif

#if defined(FAMILY_52XX)
    // Enable Write Access to FLash Controller
    PAC5XXX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    // Write data buffer to FLASH
    for(uint32_t i = 0; i < size_32bit_words; i++)
    {
        p_dest[i] = p_src[i];
    }

    // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
    while (PAC5XXX_MEMCTL-> FLASHCTL.PERASE) { }  // wait for Flash Write WBUSY bit to be 0

    // Disable Write Access to FLash Controller
    PAC5XXX_MEMCTL->FLASHLOCK = 0;

#endif

    return FLASH_OK;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief  This function use to write src string to flash memory
 * @param  Struct_Flash_Read_Wr* flash_flag_ptr
 * @param  start_address
 * @param  src_string
 * @param  length
 * @param  end_address
 * @return Status
 */
PAC5XXX_RAMFUNC flash_error flashrw_fxadr_write_data(Struct_Flash_Read_Wr* flash_flag_ptr,
    uint32_t start_address, int32_t* src_string, uint32_t length, uint32_t end_address)
{
    flash_error ret;
    // Disable global interrupts
    __disable_irq();
    
    /* Buffer is in words and size of buffer is also in words */
    ret = FLASH_READ_WRITE_ADD_INVALID;

    if ((start_address <= end_address) && ((start_address + length) <= end_address))
    {
        // ret == 0 means we are able to write to flash
        ret = (flash_error) flashrw_fxadr_write_5xxx((uint32_t *)start_address, (uint32_t *)src_string, length);
    }
    __enable_irq();
    return (flash_error) ret;
}
