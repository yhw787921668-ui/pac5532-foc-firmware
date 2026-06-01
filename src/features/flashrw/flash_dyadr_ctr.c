
/**
 * @file flash_ctr_dyadr.c
 * @brief flash library implementation
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

#include "flash_dyadr_ctr.h"
#include "flashrw_defs.h"
#include "flashrw_crc8.h"
#include "flashrw_struct.h"
#include "pac5xxx_driver_memory.h"
#include "pac5xxx_driver_system.h"

#if defined(ENABLE_FLASHDEMO)
int32_t flash_buff[FLASHDEMO_FLASH_WORD_ARRAY_SIZE + 2];

/**
 *@brief  Erase the specified flash page
 *@param  page_num: the Flash page to erase
 *@return None
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void flashrw_dyadr_erase_page(uint32_t page_num)
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
    pac_delay_asm_ramfunc(FLASH_WAIT_DELAY_MEM_CONT_READY);  // delay 20uS after WBUSY=0

    // Disable Write Access to FLash Controller
    PAC55XX_MEMCTL->FLASHLOCK = 0;
#endif
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
*@brief flash erase
*@return flash error type
*/
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC flash_error flashrw_dyadr_erase(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr)
{
    flash_error ret;
    uint16_t i;
    
    ret = FLASH_OK;

#if defined(FAMILY_52XX)
    PAC5XXX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_FLASHWRITE_KEY;
#endif

#if defined(FAMILY_55XX)
    // Enable Write Access to FLash Controller
//    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;
#endif

    for(i = dyctr_ptr->start_sector; i <= dyctr_ptr->end_sector; i++)
    {
#if defined(FAMILY_52XX)
        pac5xxx_memctl_flash_page_erase(i);
#endif

#if defined(FAMILY_55XX)
        // Enable Write Access to FLash Controller
        PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;
        // Set Page to Be Written
        PAC55XX_MEMCTL->FLASHPAGE.PAGE = i;
        // Erase Page
        PAC55XX_MEMCTL->FLASHERASE = FLASH_START_PAGE_ERASE;
       // Wait for erase to complete
       while (PAC55XX_MEMCTL->MEMSTATUS.EBUSY) { }
       pac_delay_asm_ramfunc(FLASH_WAIT_DELAY_MEM_CONT_READY);  // delay 20uS after WBUSY=0
#endif

#if defined(FAMILY_52XX)
        // wait until flash erases done.
        while(pac5xxx_memctl_page_erase_active());
#endif
    }
#if defined(FAMILY_52XX)
    PAC5XXX_MEMCTL->FLASHLOCK = 0;
#endif

#if defined(FAMILY_55XX)
    // Disable Write Access to FLash Controller
//    PAC55XX_MEMCTL->FLASHLOCK = 0;
#endif
 
    return ret;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
*@brief fast crc checking inside flash
*@param start_addr  data address
*@param size        size of data
*/
static uint8_t flashrw_dyadr_crc_checking(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t data_addr, uint32_t data_size)
{
    uint8_t crc = 0x00;
    uint32_t addr;
    uint32_t checking_point;
    uint32_t remain;

    checking_point = data_addr + data_size;
    addr = data_addr;
    if(checking_point >= dyctr_ptr->end_addr)
    {
        remain = (dyctr_ptr->end_addr + 1) - (data_addr);
        crc = crc_8_calculation(crc, (uint8_t*)addr, remain);
        remain = data_size - remain;
        addr = dyctr_ptr->start_addr;
    }
    else
    {
        remain = data_size;
    }   
    crc = crc_8_calculation(crc, (uint8_t*)addr, remain);
    return crc;
}
/**
 * @brief this function to detect physical address in flash memory
 * @return address on defined memory area
*/
static inline uint32_t flashrw_dyadr_jump_next_addr(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t cur_addr, uint32_t size)
{
    uint32_t checking_point;
    uint32_t next_addr;

    checking_point = cur_addr + size;
    if(checking_point >= dyctr_ptr->end_addr)
    {
        next_addr = dyctr_ptr->start_addr + size - (dyctr_ptr->end_addr + 1 - cur_addr);
        if(next_addr > dyctr_ptr->end_addr)
        {
            next_addr = dyctr_ptr->start_addr; // If next address is invalid, it should be configured as start address
        }
    }
    else
    {
        next_addr = cur_addr +  size;
    }
    return next_addr;
}

/**
 * @brief this function to detect flash page by physical address
 * @return flash error type
*/
static flash_error flashrw_dyadr_get_sector(uint32_t addr, uint32_t *sector)
{
    flash_error ret;

    ret = FLASH_OK;
    *sector = addr >> FLASH_SECTOR_SIZE_BITS_DYNAMIC;

    if(*sector > FLASH_MAX_SECTOR_DYNAMIC)
    {
        ret = FLASH_SECTOR_ERROR;
    }
    return ret;
}

#if defined(FAMILY_52XX)
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void pac52xx_flash_write_32bit(uint32_t addr, uint32_t data)
{
    PAC5XXX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_FLASHWRITE_KEY;
    (*((uint32_t *)addr)) = data;
    PAC5XXX_MEMCTL->FLASHLOCK = 0;
}
PAC5XXX_OPTIMIZE_SPEED_OFF

PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC uint32_t pac52xx_flash_write_array(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t addr, int32_t *data, uint32_t size_words)
{
    uint8_t i = 0;
    uint32_t next_addr = addr;
    PAC5XXX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_FLASHWRITE_KEY;
    for (i = 0; i < size_words - 1; i++)
    {
        (*((uint32_t *)(next_addr))) = data[i];
        next_addr = flashrw_dyadr_jump_next_addr(dyctr_ptr, next_addr, WORD_SIZE_DYNAMIC);
    }
    
    // write last word
    (*((uint32_t *)(next_addr))) = data[size_words - 1];
    PAC5XXX_MEMCTL->FLASHLOCK = 0;
    return next_addr;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

#if defined(FAMILY_55XX)
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC void pac55xx_flash_write_32bit(uint32_t addr, uint32_t data)
{
    // Clear WRITEWORDCNT in case it's not 0; must set FLASHLOCK to allow write to MEMCTL
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_MEMCTL;
    PAC55XX_MEMCTL->MEMCTL.WRITEWORDCNT = 0;

    // Set FLASHLOCK to allow Writes to Flash
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    (*((uint32_t *)addr)) = data;
    (*((uint32_t *)(addr + 4))) = 0xFFFFFFFF;
    (*((uint32_t *)(addr + 8))) = 0xFFFFFFFF;
    (*((uint32_t *)(addr + 12))) = 0xFFFFFFFF;
    // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
    while (PAC55XX_MEMCTL->MEMSTATUS.WBUSY)
    { }  // wait for Flash Write WBUSY bit to be 0
    pac_delay_asm_ramfunc(320); // delay 20uS after WBUSY=0

    // Return FLASHLOCK to locked state
    PAC55XX_MEMCTL->FLASHLOCK =0;
}
PAC5XXX_OPTIMIZE_SPEED_OFF

PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC uint32_t pac55xx_flash_write_array(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t addr, int32_t *data, uint32_t size_words)
{
    uint8_t i = 0;
    uint32_t next_addr = addr;
    // Clear WRITEWORDCNT in case it's not 0; must set FLASHLOCK to allow write to MEMCTL
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_MEMCTL;
    PAC55XX_MEMCTL->MEMCTL.WRITEWORDCNT = 0;

    // Set FLASHLOCK to allow Writes to Flash
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_ERASE_FLASH;

    for (i = 0; i < size_words - 4; i = i + 4)
    {
        (*((uint32_t *)(next_addr))) = data[i];
        (*((uint32_t *)(next_addr + 4))) = data[i + 1];
        (*((uint32_t *)(next_addr + 8))) = data[i + 2];
        (*((uint32_t *)(next_addr + 12))) = data[i + 3];
        // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
        while(PAC55XX_MEMCTL->MEMSTATUS.WBUSY) { }  // wait for Flash Write WBUSY bit to be 0
        pac_delay_asm_ramfunc(320);                 // delay 20uS after WBUSY=0
        next_addr = flashrw_dyadr_jump_next_addr(dyctr_ptr, next_addr, 4 * WORD_SIZE_DYNAMIC);
    }

    if (size_words >= 4)
    {
        // write last 4-words
        (*((uint32_t *)(next_addr))) = data[size_words - 4];
        (*((uint32_t *)(next_addr + 4))) = data[size_words - 3];
        (*((uint32_t *)(next_addr + 8))) = data[size_words - 2];
        (*((uint32_t *)(next_addr + 12))) = data[size_words - 1];
        // Make sure were not still busy writing and add delay before allowing read/fetch access to flash
        while (PAC55XX_MEMCTL->MEMSTATUS.WBUSY)
        { }  // wait for Flash Write WBUSY bit to be 0
        pac_delay_asm_ramfunc(320);
    }

    next_addr = flashrw_dyadr_jump_next_addr(dyctr_ptr, next_addr, 3 * WORD_SIZE_DYNAMIC);
    // Return FLASHLOCK to locked state
    PAC55XX_MEMCTL->FLASHLOCK = 0;
    return next_addr;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif 

/**
*@brief flash initialize function
*@param start_add, starting point page address
*@param end_add, ending point page address
*@return flash error type
*/
flash_error flashrw_dyadr_init(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t start_addr, uint32_t end_addr)
{
    flash_error ret;
    uint32_t data_size_flash;
    uint32_t temp_ptr;
    uint32_t crc_position;

    ret = FLASH_OK;
    dyctr_ptr->isInit = FLASH_NOT_INIT_DYNAMIC;
    if(end_addr <= start_addr) // lack of memory
    {
        ret = FLASH_INIT_ERROR;
    }

    if(ret == FLASH_OK)
    {
        ret = flashrw_dyadr_get_sector(start_addr, &dyctr_ptr->start_sector);
        if(ret == FLASH_OK)
        {
            ret = flashrw_dyadr_get_sector(end_addr, &dyctr_ptr->end_sector);
        }
    }

    /* Align to correct start/end address */
    if(ret == FLASH_OK)
    {
        dyctr_ptr->info_addr = dyctr_ptr->start_sector << FLASH_SECTOR_SIZE_BITS_DYNAMIC;
        dyctr_ptr->end_addr = ((dyctr_ptr->end_sector + 1) << FLASH_SECTOR_SIZE_BITS_DYNAMIC) - 1;

#if defined(FAMILY_52XX)
        dyctr_ptr->start_addr = dyctr_ptr->info_addr + WORD_SIZE_DYNAMIC; // First word of page is flash info offset
#endif

#if defined(FAMILY_55XX)
        dyctr_ptr->start_addr = dyctr_ptr->info_addr + 4 * WORD_SIZE_DYNAMIC; // First 4-words of page is flash info offset
#endif

        dyctr_ptr->linked_addr = READ_DATA_AT(dyctr_ptr->info_addr); // if there is no appended data, the linked address is same as end_addr
        dyctr_ptr->capacity = dyctr_ptr->end_addr - dyctr_ptr->start_addr + 1; /* size of flash memory in bytes */

        // check the value in start page, starting-point or appending-point and find the last data position
        if(dyctr_ptr->linked_addr == NO_EXCESSIVE_DATA)
        {
            dyctr_ptr->linked_addr = dyctr_ptr->end_addr;
            dyctr_ptr->head = dyctr_ptr->start_addr;
        }
        else
        {
            if((dyctr_ptr->linked_addr < dyctr_ptr->start_addr) || (dyctr_ptr->linked_addr > dyctr_ptr->end_addr)) // invalid data in flash
            {
                ret = FLASH_DATA_DETECTION_ERROR;
            }
            else
            {
                dyctr_ptr->head = dyctr_ptr->linked_addr;
            }
        }

        if(ret == FLASH_OK)
        {
#if defined(FAMILY_52XX)
            dyctr_ptr->tail = dyctr_ptr->info_addr;
#endif
#if defined(FAMILY_55XX)
            dyctr_ptr->tail = dyctr_ptr->info_addr + 3 * WORD_SIZE_DYNAMIC;
#endif
            temp_ptr = dyctr_ptr->head;
            while (dyctr_ptr->capacity > 0)
            {
                // tracking back by valid data size checking : data_size - payload - crc
                data_size_flash = READ_DATA_AT(temp_ptr);
                if(data_size_flash > dyctr_ptr->capacity) // invalid data size
                {
                    break;
                }
                else
                {
                    crc_position = flashrw_dyadr_jump_next_addr(dyctr_ptr, temp_ptr, data_size_flash + WORD_SIZE_DYNAMIC);
                    dyctr_ptr->head = temp_ptr;
                    dyctr_ptr->tail = crc_position;
                    dyctr_ptr->capacity = dyctr_ptr->capacity - data_size_flash - (WORD_SIZE_DYNAMIC << 1);
                    temp_ptr = flashrw_dyadr_jump_next_addr(dyctr_ptr, dyctr_ptr->tail, WORD_SIZE_DYNAMIC);
                }
            };
        }
    }
    if(dyctr_ptr->capacity < 0)
    {
        ret = FLASH_DATA_DETECTION_ERROR; // can not detect last data position
    }

    // perform flash erase when unexpected data existed.
    if(ret == FLASH_DATA_DETECTION_ERROR)
    {
        //flashrw_dyadr_erase(dyctr_ptr);
        flashrw_dyadr_erase_page(31);
        dyctr_ptr->capacity = dyctr_ptr->end_addr - dyctr_ptr->start_addr + 1;
        dyctr_ptr->head = dyctr_ptr->start_addr;
        dyctr_ptr->tail = dyctr_ptr->info_addr;
    }

    if((ret == FLASH_OK) || (ret == FLASH_DATA_DETECTION_ERROR))
    {
        dyctr_ptr->isInit = FLASH_IS_INIT_DYNAMIC;
    }
    
    return ret;
}

/**
*@brief flash write function
*@param data, data to write to flash
*@param length, length of data in word
*@return flash error type
*/
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC flash_error flashrw_dyadr_write(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t *data, uint32_t length)
{
    flash_error ret;
    uint32_t check_size;
    uint8_t crc_code;
    uint8_t crc_readback;
    uint32_t data_addr;
    uint32_t i;
    static volatile int32_t test_flag;

    __disable_irq();     // Disable Interrupts

    ret = FLASH_OK;
    if(dyctr_ptr->isInit == FLASH_IS_INIT_DYNAMIC)
    {
        // unlock flash to write
        check_size = (length << 2) + (WORD_SIZE_DYNAMIC << 1);
        dyctr_ptr->tail = flashrw_dyadr_jump_next_addr(dyctr_ptr, dyctr_ptr->tail, WORD_SIZE_DYNAMIC);
        dyctr_ptr->head = dyctr_ptr->tail;
        if(check_size > dyctr_ptr->capacity)
        {
            flashrw_dyadr_erase(dyctr_ptr);
            // reset flash capacity
            dyctr_ptr->capacity = dyctr_ptr->end_addr - dyctr_ptr->start_addr + 1;
            // unlock flash to write
#if defined(FAMILY_52XX)
            pac52xx_flash_write_32bit(dyctr_ptr->info_addr, dyctr_ptr->head);
#endif

#if defined(FAMILY_55XX)
            pac55xx_flash_write_32bit(dyctr_ptr->info_addr, dyctr_ptr->head);
#endif

        }
        // Create a payload
        crc_code = crc_8_calculation(0x00, (uint8_t *)data, (length << 2));
        flash_buff[0] = (length << 2);
        for(i = 0; i < length; i++)
        {
            flash_buff[i + 1] = data[i];
        }
        flash_buff[length + 1] = (uint32_t)crc_code;

#if defined(FAMILY_52XX)
        dyctr_ptr->tail = pac52xx_flash_write_array(dyctr_ptr, dyctr_ptr->tail, flash_buff, length + 2);
#endif

#if defined(FAMILY_55XX)
        dyctr_ptr->tail = pac55xx_flash_write_array(dyctr_ptr, dyctr_ptr->tail, flash_buff, length + 2);
#endif

        dyctr_ptr->capacity = dyctr_ptr->capacity - check_size;
        //CRC read back checking 
        data_addr = flashrw_dyadr_jump_next_addr(dyctr_ptr, dyctr_ptr->head, WORD_SIZE_DYNAMIC);
        crc_readback = flashrw_dyadr_crc_checking(dyctr_ptr, data_addr, (length << 2));
        // flash_test_data1[length+4] = crc_readback;
        if(crc_readback != crc_code)
        {
            ret = FLASH_WRITE_ERROR;
        }
    }
    else
    {
        ret = FLASH_IS_NOT_INITIALIZED;
    }
    
    __enable_irq();     // Enable Interrupts
    return ret;
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
*@brief flash read function
*@param data, data to read from flash
*@param length, length of data in word
*@return flash error type
*/
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC flash_error flashrw_dyadr_read(Struct_Flash_Rd_Wr_DyCtr* dyctr_ptr, uint32_t *data, uint32_t length)
{
    flash_error ret;
    uint32_t i;
    uint32_t data_size;
    uint32_t temp_ptr;
    uint8_t crc_value;

    ret = FLASH_OK;
    if(dyctr_ptr->isInit == FLASH_IS_INIT_DYNAMIC)
    {
        data_size = READ_DATA_AT(dyctr_ptr->head);
        if(data_size != (length << 2))
        {
            ret = FLASH_READ_ERROR;
        }
        else
        {
            // Read data
            temp_ptr = dyctr_ptr->head;
            for(i = 0; i < length; i++)
            {
                temp_ptr = flashrw_dyadr_jump_next_addr(dyctr_ptr, temp_ptr, WORD_SIZE_DYNAMIC);
                data[i] = *(uint32_t*)temp_ptr;
            }
            crc_value = crc_8_calculation(0x00, (uint8_t*)data, (length << 2));
            if(crc_value != (uint8_t)READ_DATA_AT(dyctr_ptr->tail))
            {
                ret = FLASH_CRC_ERROR;
            }
        }
    }
    else
    {
        ret = FLASH_IS_NOT_INITIALIZED;
    }
    return ret;
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif
